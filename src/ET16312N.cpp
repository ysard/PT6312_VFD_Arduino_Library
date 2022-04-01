/* Arduino Library for AD16312, HT16512, PT6312, etc. VFD Controller.
 * Copyright (C) <2022> - <ysard@users.noreply.github.com>
 *
 * Based on the work of <2007> Istrate Liviu - <istrateliviu24@yahoo.com>
 * Itself inspired by http://www.instructables.com/id/A-DVD-Player-Hack/
 * Also inspired from https://os.mbed.com/users/wim/code/mbed_PT6312/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */
#include "ET16312N.h"
uint8_t cursor;

void VFD_initialize(void){
    // Configure pins
    pinMode(VFD_CS_DDR, VFD_CS_PIN, _OUTPUT);
    pinMode(VFD_SCLK_DDR, VFD_SCLK_PIN, _OUTPUT);
    pinMode(VFD_DATA_DDR, VFD_DATA_PIN, _OUTPUT);

    digitalWrite(VFD_CS_PORT, VFD_CS_PIN, _HIGH);
    digitalWrite(VFD_SCLK_PORT, VFD_SCLK_PIN, _HIGH);

    // Waiting for the VFD driver to startup
    _delay_ms(500);

    // Set display mode
    #if VFD_DIGITS == 4
        VFD_command(0x00, 1); // 4 digits, 16 segments
    #elif VFD_DIGITS == 5
        VFD_command(0x01, 1); // 5 digits, 16 segments
    #elif VFD_DIGITS == 6
        VFD_command(0x02, 1); // 6 digits, 16 segments
    #elif VFD_DIGITS == 7
        VFD_command(0x03, 1); // 7 digits, 15 segments
    #elif VFD_DIGITS == 8
        VFD_command(0x04, 1); // 8 digits, 14 segments
    #elif VFD_DIGITS == 9
        VFD_command(0x05, 1); // 9 digits, 13 segments
    #elif VFD_DIGITS == 10
        VFD_command(0x06, 1); // 10 digits, 12 segments
    #elif VFD_DIGITS == 11
        VFD_command(0x07, 1); // 11 digits, 11 segments
    #endif

    // Turn on the display
    // Display control cmd, display on/off, default brightness
    VFD_displayOn(PT6312_BRT_DEF);

    // Data set cmd, normal mode, auto incr, write data to memory
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_DATA_WR, true);

    #if ENABLE_MENU_INTERFACE_RELATED == 1
        for(uint8_t i=0; i<VFD_DIGITS; i++){
            displaybuffer[i] = ' ';
        }
    #endif

    cursor = 1;
}


void VFD_setBrightness(const uint8_t brightness){
    // Display control cmd, display on/off, brightness
    // mask invalid bits with PT6312_BRT_MSK
    VFD_command(PT6312_DSP_CTRL_CMD | PT6312_DSP_ON | (brightness & PT6312_BRT_MSK), true);
}


void VFD_clear(void){
    // Set addr to 1st memory cell, no CS assertion
    VFD_setCursorPosition(1, false);

    for(uint8_t i=0; i<VFD_DIGITS; i++){
        // Display a segment
        VFD_command(0, false);
        VFD_command(0, false);
        #if ENABLE_MENU_INTERFACE_RELATED == 1
        displaybuffer[i] = ' ';
        #endif
    }
    VFD_CSSignal();
    cursor = VFD_DIGITS;
}


void VFD_setCursorPosition(uint8_t position, bool cmd){
    if(position > VFD_DIGITS){
        if(position == VFD_DIGITS + 1){
            position = 1;
        }else{
            position = VFD_DIGITS;
        }
    }else if(position == 0){
        position = VFD_DIGITS;
    }

    cursor = position;

    // map position to memory (x bytes per digit)
    position = (position * PT6312_BYTES_PER_GRID) - PT6312_BYTES_PER_GRID;
    // Address setting command: set an address of the display memory (0 to 15, 5 lowest bits)
    VFD_command(PT6312_ADDR_SET_CMD | (position & PT6312_ADDR_MSK), cmd);
}


void VFD_writeString(const char *string, bool colon_symbol){
    // Display control command: Write data to display memory, increment addr, normal operation
    uint8_t chrset;
    uint8_t chrset_tmp;

    while(*string > '\0'){ // TODO: security test cursor <= VFD_DIGITS

        if (cursor == 3 || cursor == 4){
            // MSB: set LSB of left char
            chrset_tmp = FONT[*string - 0x20][1];
            string++;
            // test string char validity
            if (*string > '\0'){
                // LSB: Send LSB of right char
                chrset = FONT[*string - 0x20][1];
            } else {
                chrset = 0;
            }

            // Set optional colon symbol (if its bit is < 8)
            if (colon_symbol && cursor == 4){
                #if VFD_COLON_SYMBOL_BIT > 8
                // Add the symbol on the MSB part of the byte
                chrset_tmp |= 1 << (VFD_COLON_SYMBOL_BIT - 9);
                #else // < 9
                // Add the symbol on the LSB part of the byte
                chrset |= 1 << (VFD_COLON_SYMBOL_BIT - 1);
                #endif
            }
            // LSB: 2nd char
            VFD_command(chrset, false);
            // MSB: 1st char
            VFD_command(chrset_tmp, false);
        }else{
            // Cursor positions: 1 or 2
            // Send LSB
            chrset = FONT[*string - 0x20][1];
            VFD_command(chrset, false);
            // Send MSB
            chrset = FONT[*string - 0x20][0];
            VFD_command(chrset, false);
        }
        cursor++;
        string++;
    }

    // Signal the driver that the data transmission is over
    VFD_CSSignal();
}


// Enables VFDBusySpinningCircle function
uint8_t busy_indicator_delay_count;
uint8_t busy_indicator_frame;
uint8_t busy_indicator_loop_nb;

void VFD_busySpinningCircleReset(void){
    busy_indicator_delay_count = 1;
    busy_indicator_frame = 1;
    busy_indicator_loop_nb = 0;
}

/**
 * @brief VFD_busySpinningCircle
 *
 *      _delay_ms should be set to have an accurate
 *
 *     // NOTE: rafraichi toutes les 2 boucles (1 pr la première); busy_indicator_delay_count
    // 1 frame est rafraichie 70 fois avant changement;
    // 1 changement tous les 70 rafraichissements (140 appels): busy_indicator_loop_nr
    // la frame revient à la frame 1 dès que busy_indicator_frame == 6
 */
void VFD_busySpinningCircle(void){

    uint8_t msb = 0;
    // 1 display for a fading segment each x loops
    uint8_t seg2_duty_cycle = 2, seg3_duty_cycle = 5, seg4_duty_cycle = 12;

    if(busy_indicator_delay_count == 2){
        seg2_duty_cycle = busy_indicator_loop_nb % seg2_duty_cycle;
        seg3_duty_cycle = busy_indicator_loop_nb % seg3_duty_cycle;
        seg4_duty_cycle = busy_indicator_loop_nb % seg4_duty_cycle;

        // left shifts
        // msb: segment -8 -1
        // lsb: segment -1
        // segments affichés à 100% du duty cycle de la frame:
        // 11, 12, 13, 14, 15, 16
        // Les 3 segments qui précèdent le segment affiché sont en fading de plus en plus prononcé
        // Ex: 16 affiché: 15, 14, 13 sont affichés, du plus marqué jusqu'au plus sombre ;
        // les autres sont non affichés (12, 11)
        if(busy_indicator_frame == 1){
            msb = 1 << (11 - 8 - 1); // segment 11 (first) (1)
        }else if(busy_indicator_frame == 2){
            msb = 1 << (12 - 8 - 1); // segment 12 (second) (1)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (11 - 8 - 1); // segment 11 (2)
            }
        }else if(busy_indicator_frame == 3){
            msb = 1 << (13 - 8 - 1); // segment 13 (third) (1)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (12 - 8 - 1); // segment 12 (2)
            }
            if(seg3_duty_cycle == 0){
                msb |= 1 << (11 - 8 - 1); // segment 11 (3)
            }
        }else if(busy_indicator_frame == 4){
            msb = 1 << (14 - 8 - 1); // segment 14 (fourth) (1)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (13 - 8 - 1); // segment 13 (2)
            }
            if(seg3_duty_cycle == 0){
                msb |= 1 << (12 - 8 - 1); // segment 12 (3)
            }
            if(seg4_duty_cycle == 0){
                msb |= 1 << (11 - 8 - 1); // segment 11 (4)
            }
        }else if(busy_indicator_frame == 5){
            msb = 1 << (15 - 8 - 1); // segment 15 (fifth) (1)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (14 - 8 - 1); // segment 14 (2)
            }
            if(seg3_duty_cycle == 0){
                msb |= 1 << (13 - 8 - 1); // segment 13 (3)
            }
            if(seg4_duty_cycle == 0){
                msb |= 1 << (12 - 8 - 1); // segment 12 (4)
            }
        }else if(busy_indicator_frame == 6){
            msb = 1 << (16 - 8 - 1); // segment 16 (sixth) (1)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (15 - 8 - 1); // segment 14 (2)
            }
             if(seg3_duty_cycle == 0){
                msb |= 1 << (14 - 8 - 1); // segment 15 (3)
            }
            if(seg4_duty_cycle == 0){
                msb |= 1 << (13 - 8 - 1); // segment 13 (4)
            }
        }

        busy_indicator_delay_count = 0;
        busy_indicator_loop_nb++;
        if(busy_indicator_loop_nb == 70){
            if(busy_indicator_frame == 6) busy_indicator_frame = 0;
            busy_indicator_frame++;
            busy_indicator_loop_nb = 0;
        }

        VFD_writeByte(cursor, msb); // TODO: receive specific addr to write this byte in param ?

        // If the spinning circle is on 2 bytes, lsb and msb should be sent.
        // Ex:
        // VFD_command(lsb, false);
        // VFD_command(msb, false);
        // VFD_CSSignal();

        cursor++;
    }

    // Update display
    // Set display mode
    VFD_command(0x00, 1); // 4 digits, 16 segments
    // Turn on the display
    VFD_displayOn(PT6312_BRT_DEF);
    // Data set cmd, normal mode, auto incr, write data to memory
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_DATA_WR, true);

    busy_indicator_delay_count++;
}


void VFD_setLEDs(uint8_t leds){
    uint8_t leds_value = 0xFF;

    // Enable LED Read mode
    // Data set cmd, normal mode, auto incr, write data to LED port
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_LED_WR, false);

    // 0: LED lights
    // 1: LED turns off
    // Invert the bits
    for(uint8_t i=0; i<8; i++){
        if((1 << i) & leds)
            // Bit is set: Clear the bit
            leds_value &= ~(1 << i);
    }

    VFD_command(leds_value & PT6312_LED_MSK, true);

    // Restore Data Write mode
    // Data set cmd, normal mode, auto incr, write data to memory
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_DATA_WR, true);
}


/**
 * @brief Get status of keys
 *      Key status are stored in the 3 least significant bytes of a uint32_t.
 *      Each of the 4 keys is sampled 6 times.
 *      In a sample, 4 bits for keys: 0, 1, 2, 3 (from least to most significant bit)
 *      If a key is pressed raw_keys is > 0.
 *      Sample Masks:
 *          Sample 0: raw_keys & 0x0F
 *          Sample 1: (raw_keys >> 4) & 0x0F
 *          Sample 2: (raw_keys >> 8) & 0x0F
 *          Sample 3: (raw_keys >> 12) & 0x0F
 *          Sample 4: (raw_keys >> 16) & 0x0F
 *          Sample 5: (raw_keys >> 20) & 0x0F
 * @return 6 samples of 4 bits each in the 3 least significant bytes of a uint32_t
 */
uint32_t VFD_getKeys(void){
    // Enable Key Read mode
    // Data set cmd, normal mode, auto incr, read data
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_KEY_RD, false);

    // Configure DATA pin input HIGH
    pinMode(VFD_DATA_DDR, VFD_DATA_PIN, _INPUT);
    digitalWrite(VFD_DATA_PORT, VFD_DATA_PIN, _HIGH);

    // Here: CS is still LOW, SCLK is still HIGH
    _delay_us(1);

    // Read the key matrix of size PT6312_KEY_MEM bytes
    // 3 bytes = 3 readings
    uint32_t raw_keys = VFD_readByte();
    raw_keys = (raw_keys << 8) + VFD_readByte();
    raw_keys = (raw_keys << 8) + VFD_readByte();

    // Restore DATA pin as OUTPUT
    pinMode(VFD_DATA_DDR, VFD_DATA_PIN, _OUTPUT);

    VFD_CSSignal();

    // Restore Data Write mode
    // Data set cmd, normal mode, auto incr, write data to memory
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_DATA_WR, true);

    return raw_keys;
}


/**
 * @brief Return the number of the first pressed button (no multi buttons)
 *      Button 0: 1
 *      ...
 *      Button 3: 4
 * @return The number of the first pressed button or 0 if no button is pressed.
 */
uint8_t VFD_getKeyPressed(void){
    uint8_t btn_nr = 1, pressed_btn = 0;
    // Get 1 sample (6th sample): Last 4 bits
    pressed_btn = PT6312_KEY_SMPL_MSK & VFD_getKeys();
    if(pressed_btn > 0){
        // Return the button number
        while(((1 << btn_nr) & pressed_btn) == 0)
            btn_nr++;
        return btn_nr;
    }
    return 0;
}


uint8_t VFD_getSwitches(void){
    // Enable Switch Read mode
    // Data set cmd, normal mode, auto incr, read data
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_SW_RD, false);

    // Make DATA pin input HIGH
    pinMode(VFD_DATA_DDR, VFD_DATA_PIN, _INPUT);
    digitalWrite(VFD_DATA_PORT, VFD_DATA_PIN, _HIGH);

    // Here: CS is still LOW, SCLK is still HIGH
    _delay_us(1);

    uint8_t raw_switches = VFD_readByte();

    // DATA pin as OUTPUT
    pinMode(VFD_DATA_DDR, VFD_DATA_PIN, _OUTPUT);

    VFD_CSSignal();

    // Restore Data Write mode
    // Data set cmd, normal mode, auto incr, write data to memory
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_DATA_WR, true);

    return raw_switches;
}


/**
 * @brief Test segment numbering
 *      To display a segment:
 *      The memory is filled progressively as the data is received.
 *      Each grid has 2 addresses: So 2 memory bytes.
 *
 *      The Least Significant Byte is received first, its bits are read from
 *      the lowest one to highest one (from right to left in the human representation
 *      0b00000000), so from 0th bit to 7th bit (8th segment).
 *      Then, the Most Significant Byte is received
 *      for the definition of the 8th bit to the 15th bit (16th segment).
 */
void VFD_segmentsGenericTest(void){
    // msb: segments 16-9
    // lsb: segments 8-1
    uint8_t msb = 0, lsb = 0;

    for(uint8_t grid=1; grid<=VFD_DIGITS; grid++){

        for(uint8_t i=0; i<16; i++){
            if(i < 8){
                lsb = 1 << i;
                msb = 0;
            }else{
                msb = 1 << (i - 8);
                lsb = 0;
            }

            // Set grid
            VFD_setCursorPosition(grid, false);
            // Set segments
            VFD_command(lsb, false);
            VFD_command(msb, true);

            _delay_ms(500);
        }
        VFD_clear();
    }
}


void VFD_displayAllSegments(){
    VFD_setCursorPosition(1, false);
    for(uint8_t grid=1; grid<=VFD_DIGITS; grid++){
        // Display a segment
        VFD_command(255, false);
        VFD_command(255, false);
    }
    VFD_CSSignal();

    cursor = VFD_DIGITS;
}


void VFD_command(uint8_t value, bool cmd){
    digitalWrite(VFD_CS_PORT, VFD_CS_PIN, _LOW);
    _delay_us(1); // NOTE: not in datasheet

    for(uint8_t i=0; i<8; i++){
        digitalWrite(VFD_SCLK_PORT, VFD_SCLK_PIN, _LOW);

        if(value & (1 << i)){
            digitalWrite(VFD_DATA_PORT, VFD_DATA_PIN, _HIGH);
        }else{
            digitalWrite(VFD_DATA_PORT, VFD_DATA_PIN, _LOW);
        }
        // wait 500ns
        _delay_us(0.5);
        // Data is read at the rising edge
        digitalWrite(VFD_SCLK_PORT, VFD_SCLK_PIN, _HIGH);
        _delay_us(0.5);
    }

    if(cmd) {
        VFD_CSSignal();
    }
}


inline void VFD_CSSignal(){
    // Signal the driver that the data transmission is over
    _delay_us(1);
    digitalWrite(VFD_CS_PORT, VFD_CS_PIN, _HIGH);
    _delay_us(1);
}


uint8_t VFD_readByte(void){
    uint8_t data_in = 0xFF;
    for(uint8_t i=0; i<8; i++){
        digitalWrite(VFD_SCLK_PORT, VFD_SCLK_PIN, _LOW);
        _delay_us(0.5);

        // Data is read at the falling edge
        // DigitalRead (read only) of VFD_DATA_PIN status
        if(bit_is_set(VFD_DATA_R_ONLY_PORT, VFD_DATA_PIN) == 0)
            // Bit is not set: Clear the bit
            data_in &= ~(1 << i);

        digitalWrite(VFD_SCLK_PORT, VFD_SCLK_PIN, _HIGH);
        _delay_us(0.5);
    }
    return data_in;
}


void VFD_writeByte(uint8_t address, char data){
    VFD_command(PT6312_ADDR_SET_CMD | (address & PT6312_ADDR_MSK), false);
    VFD_command(data, true);
    cursor++;
}


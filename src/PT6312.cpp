/* Arduino Library for AD16312, HT16512, PT6312, etc. VFD Controller.
 * Copyright (C) 2022 Ysard - <ysard@users.noreply.github.com>
 *
 * Based on the work of 2017 Istrate Liviu - <istrateliviu24@yahoo.com>
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

uint8_t grid_cursor;

// Select font & functions according to global.h setting
#if defined(VFD_VARIANT_1)
    #include "display_variants/variant_1_font.h"
    #include "display_variants/variant_1_functions.cpp"
    #warning "enabled default"
#elif defined(VFD_VARIANT_2)
    #include "display_variants/variant_2_font.h"
    #include "display_variants/variant_2_functions.cpp"
    #warning "enabled variant"
#else
    #error "Display variant not implemented!"
#endif


/**
 * @brief Configure the controller and the pins of the MCU.
 */
void VFD_initialize(void){
    // Configure pins
    pinMode(VFD_CS_DDR, VFD_CS_PIN, _OUTPUT);
    pinMode(VFD_SCLK_DDR, VFD_SCLK_PIN, _OUTPUT);
    pinMode(VFD_DATA_DDR, VFD_DATA_PIN, _OUTPUT);

    digitalWrite(VFD_CS_PORT, VFD_CS_PIN, _HIGH);
    digitalWrite(VFD_SCLK_PORT, VFD_SCLK_PIN, _HIGH);

    // Waiting for the VFD driver to startup
    _delay_ms(500);

    // Configure the controller
    VFD_resetDisplay();

    grid_cursor = 1;
}

/**
 * @brief Configure/reset the controller
 *      - Set display mode (number of digits & segments);
 *      - Turn on the display by setting the brightness
 *      - Init default command mode (write to memory, auto increment the memory address)
 */
void VFD_resetDisplay(void){
    // Set display mode
    #if VFD_GRIDS == 4
        VFD_command(PT6312_GR4_SEG16, 1);  // 4 digits, 16 segments
        #define VFD_SEGMENTS 16
    #elif VFD_GRIDS == 5
        VFD_command(PT6312_GR5_SEG16, 1);  // 5 digits, 16 segments
        #define VFD_SEGMENTS 16
    #elif VFD_GRIDS == 6
        VFD_command(PT6312_GR6_SEG16, 1);  // 6 digits, 16 segments
        #define VFD_SEGMENTS 16
    #elif VFD_GRIDS == 7
        VFD_command(PT6312_GR7_SEG15, 1);  // 7 digits, 15 segments
        #define VFD_SEGMENTS 15
    #elif VFD_GRIDS == 8
        VFD_command(PT6312_GR8_SEG14, 1);  // 8 digits, 14 segments
        #define VFD_SEGMENTS 14
    #elif VFD_GRIDS == 9
        VFD_command(PT6312_GR9_SEG13, 1);  // 9 digits, 13 segments
        #define VFD_SEGMENTS 13
    #elif VFD_GRIDS == 10
        VFD_command(PT6312_GR10_SEG12, 1); // 10 digits, 12 segments
        #define VFD_SEGMENTS 12
    #elif VFD_GRIDS == 11
        VFD_command(PT6312_GR11_SEG11, 1); // 11 digits, 11 segments
        #define VFD_SEGMENTS 11
    #endif

    // Turn on the display
    // Display control cmd, display on/off, default brightness
    VFD_displayOn(PT6312_BRT_DEF);

    // Data set cmd, normal mode, auto incr, write data to memory
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_DATA_WR, true);
}

/**
 * @brief Set display brightness
 * @param brightness Valid range 0..7
 *      for 1/16, 2/16, 4/16, 10/16, 11/16, 12/16, 13/16, 14/16 dutycycles.
 */
void VFD_setBrightness(const uint8_t brightness){
    // Display control cmd, display on/off, brightness
    // mask invalid bits with PT6312_BRT_MSK
    VFD_command(PT6312_DSP_CTRL_CMD | PT6312_DSP_ON | (brightness & PT6312_BRT_MSK), true);
}


/**
 * @brief Clear the display by turning off all segments
 *      If ENABLE_ICON_BUFFER is enabled, icons will be cleared by this function
 *      but the buffer is preserved. i.e the icons will reappear with the next call
 *      to VFD_writeString(), VFD_writeInt() or VFD_busySpinningCircle().
 * @see VFD_clearIcons()
 */
void VFD_clear(void){
    // Set addr to 1st memory cell, no CS assertion
    VFD_setGridCursor(1, false);

    for(uint8_t i=0; i<VFD_GRIDS; i++){
        // Display a segment
        VFD_command(0, false);
        VFD_command(0, false);
    }
    VFD_CSSignal();
    grid_cursor = VFD_GRIDS;
}


/**
 * @brief Set the cursor on the controller memory according to the given grid position.
 *      The first address of a grid will be selected for writing.
 *      Should be used BEFORE sending segments data.
 *      Ex: If PT6312_BYTES_PER_GRID == 2 (default), position 1 relies on the first grid,
 *      the memory address on the controller will be 0.
 *      Position 2 relies on the 2nd grid, the address will be 2 (2 bytes further).
 * @param position Position where the next segments will be written.
 *      Valid range 1..VFD_GRIDS.
 *      If position == VFD_GRIDS + 1: The first grid will be selected.
 *      If position > VFD_GRIDS + 1 or VFD_GRIDS == 0: The last grid will be selected.
 * @param cmd (Optional) Boolean transmitted to VFD_command();
 *      If True the CS/Strobe line is asserted to HIGH (end of transmission)
 *      after setting the address.
 *      Default: false
 */
void VFD_setGridCursor(uint8_t position, bool cmd){
    if(position > VFD_GRIDS){
        if(position == VFD_GRIDS + 1){
            position = 1;
        }else{
            position = VFD_GRIDS;
        }
    }else if(position == 0){
        position = VFD_GRIDS;
    }

    grid_cursor = position;

    // Map position to memory (x bytes per digit)
    position = (position * PT6312_BYTES_PER_GRID) - PT6312_BYTES_PER_GRID;
    // Address setting command: set an address of the display memory (0 to 15, 5 lowest bits)
    VFD_command(PT6312_ADDR_SET_CMD | (position & PT6312_ADDR_MSK), cmd);
}


/**
 * @brief Write a number
 * @param number Number to display. Can be negative.
 * @param digits_number Number of reserved characters to represent the given number.
 *      If the number is negative, the minus sign '-' will be counted in the digits_number;
 *      thus the digit_number is always respected.
 *      If the number representation uses more space than digits_number,
 *      it will be divided by 10 until it fits in the reserved space.
 *      If the number representation uses less space than digits_number,
 *      it will be padded with zeros.
 * @param colon_symbol Boolean set to true to display the special colon symbol
 *      segment if possible (See VFD_writeString()).
 */
void VFD_writeInt(int32_t number, int8_t digits_number, bool colon_symbol){
    int16_t number_temp = number;
    uint8_t length = 0;
    bool isNegative = false;

    // Find number of digits
    // TODO: no check of uint8 overflow here or later (by adding minus sign)
    while(number_temp != 0){
        length++;
        number_temp /= 10;
    }

    // Reserve 1 space for the sign
    // Note: If we don't want to count the sign in the digits_number restriction
    //  move this block after the next block of length adjustments.
    if (number < 0){
        // Switch to positive number
        number = -number;
        length++;
        isNegative = true;
    }

    // Adjust the number to digits_number param
    if (length < digits_number)
        // Add remaining wanted space (filled later by zeros)
        length += digits_number - length;
    else if (length > digits_number) {
        // Reduce the size of the number by discarding units
        // Ex: 100 to 1 digit: 2 divisions by 10 are made
        for (uint8_t i=0; i<(length-digits_number); i++)
            number /= 10;
        length = digits_number;
    }

    // Avoid a memory overflow if digit is too long
    // (since VFD_writeString doesn't control the size)
    // WARNING: This code will cut the string from left (not from right like previous adjustments)
    // Ex: VFD_writeInt(-123456, 7, true); on a 6 digits display.
    // Will display: -23456 (The 1 is dropped here)
    uint8_t remaining_space = VFD_DISPLAYABLE_DIGITS - grid_cursor +1;
    uint8_t size = ((length > remaining_space) ? remaining_space : length);
    char string[size + 1] = ""; // +1 for null byte

    if (isNegative)
        string[0] = '-';

    for(uint8_t i=size-1; i>=(isNegative) ? 1 : 0; --i){
        // Convert number to ASCII value
        // PS: even if the modulo is 0, the displayed number will be 0
        // => takes care of the digits_number param.
        string[i] = (number % 10) + 0x30;
        number /= 10;
    }

    VFD_writeString(string, colon_symbol);
}


/**
 * @brief Scroll the given string on the display
 *      The speed can be adjusted by modifying VFD_SCROLL_SPEED define.
 * @param string String to display; must be null terminated '\0'.
 * @param pfunc (Optional) Callback called at the end of each scrolling iteration.
 *      It avoids blocking the program during the display loop.
 *      Can be used to test keys, set leds, etc.
 */
void VFD_scrollText(const char *string, void (pfunc)()){
    // Save the current grid cursor to start the scrolling on the same position at each iteration
    uint8_t cursor_save = grid_cursor;

    // Find the input string length
    // Save the addr of the string
    const char *string_start_pos = string;
    uint8_t size = 0;
    while(*string > '\0'){
        size++;
        string++;
    }
    // Restore the pointer to the input string
    string = string_start_pos;

    // Split the string into segments of the number of displayable characters,
    // then shift one letter at each iteration
    char string_temp[VFD_DISPLAYABLE_DIGITS + 1] = "";
    uint8_t left_shift = 0;
    while((left_shift + VFD_DISPLAYABLE_DIGITS -1) < size) {
        // Copy the segment from original string to a temporary string
        uint8_t temp_index = 0;
        for(uint8_t i=left_shift; i<(left_shift + VFD_DISPLAYABLE_DIGITS); i++){
            string_temp[temp_index] = string[i];
            temp_index++;
        }
        string_temp[temp_index] = '\0';

        // Send the string to the controller
        VFD_writeString(string_temp, false);

        // Reset/Update display
        // => Don't know why but it appears to be mandatory to avoid forever black screen... (?)
        // See VFD_busySpinningCircle() (same behavior)
        VFD_resetDisplay();

        if (left_shift == 0)
             _delay_ms(1000);
        else
             _delay_ms(VFD_SCROLL_SPEED);

        left_shift++;

        if (pfunc != nullptr){
            pfunc();
        }

        // Restore grid cursor
        // not on last iteration // todo PROPRE
        if ((left_shift + VFD_DISPLAYABLE_DIGITS -1) < size)
            VFD_setGridCursor(cursor_save, false);
    }
    _delay_ms(2000);
}


/**
 * @brief Set status of LEDs.
 *      Up to 4 LEDs can be controlled.
 * @param leds Byte where the 4 least significant bits are used.
 *      Set a bit to 1 to turn on a LED.
 *      Bit 0: LED 1
 *      ...
 *      Bit 3: LED 4
 */
void VFD_setLEDs(uint8_t leds){
    // Enable LED Read mode
    // Data set cmd, normal mode, auto incr, write data to LED port
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_LED_WR, false);

    // Invert the bits:
    // 0: LED lights
    // 1: LED turns off
    for(uint8_t i=0; i<8; i++){
        if((1 << i) & leds)
            // Bit is set: Clear the bit
            leds &= ~(1 << i);
    }

    VFD_command(leds & PT6312_LED_MSK, true);

    // Restore Data Write mode
    // Data set cmd, normal mode, auto incr, write data to memory
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_DATA_WR, true);
}


/**
 * @brief Get status of keys
 *      Keys status are stored in the 3 least significant bytes of a uint32_t.
 *      Each of the 4 keys is sampled 6 times.
 *      In a sample, 4 bits for keys: 0, 1, 2, 3 (from least to most significant bit).
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
    uint32_t raw_keys = PT6312_KEY_MSK & VFD_readByte();
    raw_keys = (raw_keys << 8) + (PT6312_KEY_MSK & VFD_readByte());
    raw_keys = (raw_keys << 8) + (PT6312_KEY_MSK & VFD_readByte());

    // Restore DATA pin as OUTPUT
    pinMode(VFD_DATA_DDR, VFD_DATA_PIN, _OUTPUT);

    VFD_CSSignal();

    // Restore Data Write mode
    // Data set cmd, normal mode, auto incr, write data to memory
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_DATA_WR, true);

    return raw_keys;
}


/**
 * @brief Get the number of the first pressed button (no multi buttons)
 *      Button 0: 1
 *      ...
 *      Button 3: 4
 * @see VFD_getKeys()
 * @return The number of the first pressed button or 0 if no button is pressed.
 */
uint8_t VFD_getKeyPressed(void){
    uint8_t btn_nr = 1, pressed_btn = 0;
    // Get 1 sample (6th sample): Last 4 bits of the uint32_t
    pressed_btn = PT6312_KEY_SMPL_MSK & PT6312_KEY_MSK & VFD_getKeys();
    if(pressed_btn > 0){
        // Return the button number
        while(((1 << btn_nr) & pressed_btn) == 0)
            btn_nr++;
        return btn_nr;
    }
    return 0;
}


/**
 * @brief Get status of switches
 *      Switches status are stored in the last 4 bits of the returned byte.
 *      The first (lowest) bit represents switch 0.
 *      Ex: 0b....0001
 *                   |
 *                   switch 0 is pressed
 */
uint8_t VFD_getSwitches(void){
    // Enable Switch Read mode
    // Data set cmd, normal mode, auto incr, read data
    VFD_command(PT6312_DATA_SET_CMD | PT6312_MODE_NORM | PT6312_ADDR_INC | PT6312_SW_RD, false);

    // Make DATA pin input HIGH
    pinMode(VFD_DATA_DDR, VFD_DATA_PIN, _INPUT);
    digitalWrite(VFD_DATA_PORT, VFD_DATA_PIN, _HIGH);

    // Here: CS is still LOW, SCLK is still HIGH
    _delay_us(1);

    uint8_t raw_switches = PT6312_SW_MSK & VFD_readByte();

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
 *      Lights up a segment from 1st to 16th every 2 seconds so you can
 *      note the VFD structure.
 * @note
 *      To display a segment:
 *      The memory is filled progressively as the data is received.
 *      Each grid has 2 addresses: So 2 memory bytes.
 *
 *      The Least Significant Byte is received first, its bits are read from
 *      the lowest one to highest one (from right to left in the common
 *      human representation: 0b00000000), so from 0th bit to 7th bit (8th segment).
 *      Then, the Most Significant Byte is received
 *      for the definition of the 8th bit to the 15th bit (16th segment).
 */
void VFD_segmentsGenericTest(void){
    // msb: segments 16-9
    // lsb: segments 8-1
    uint8_t msb = 0, lsb = 0;

    for(uint8_t grid=1; grid<=VFD_GRIDS; grid++){

        // Note: VFD_SEGMENTS is defined in VFD_resetDisplay()
        for(uint8_t i=0; i<VFD_SEGMENTS; i++){
            if(i < 8){
                lsb = 1 << i;
                msb = 0;
            }else{
                msb = 1 << (i - 8);
                lsb = 0;
            }

            // Set grid
            VFD_setGridCursor(grid, false);
            // Set segments
            VFD_command(lsb, false);
            VFD_command(msb, true);

            _delay_ms(2000);
        }
        VFD_clear();
    }
}


/**
 * @brief Test the display of all segments of the display.
 *      It's the opposite of VFD_clear().
 */
void VFD_displayAllSegments(void){
    VFD_setGridCursor(1, false);
    for(uint8_t grid=1; grid<=VFD_GRIDS; grid++){
        // Display a segment
        VFD_command(255, false);
        VFD_command(255, false);
    }
    VFD_CSSignal();

    grid_cursor = VFD_GRIDS;

    // Reset/Update display
    // => Don't know why but it appears to be mandatory to avoid forever black screen... (?)
    // See VFD_busySpinningCircle() (same behavior)
    VFD_resetDisplay();
}


/**
 * @brief Display and scroll all available characters in the current font
 */
void VFD_displayAllFontGlyphes(void){
    uint8_t i, j=0;
    const uint8_t arrayLength = ((sizeof(FONT) / sizeof(uint8_t)) / 2);
    char string[arrayLength + 1] = "";

    for(i=0; i<arrayLength; i++){
        // Do not display N/A chars
        if((FONT[i][0] | FONT[i][1]) > 0){
            string[j] = i + 0x20;
            j++;
        }
    }
    VFD_scrollText(string);
}


/**
 * @brief Send a byte in a write command to the controller
 * @param value Byte to send.
 * @param cmd (Optional)
 *      If True, the CS/Strobe line is asserted to HIGH (end of transmission)
 *      after the byte has been sent.
 *      Default: false
 */
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


/**
 * @brief Signal the driver that the data transmission is over
 *      The CS/Strobe line is asserted to HIGH (end of transmission).
 */
extern inline void VFD_CSSignal();


/**
 * @brief Obtain a byte from the controller (i.e get keys & switches status)
 * @see VFD_getSwitches(), VFD_getKeys(), VFD_getKeyPressed().
 * @return Byte of data
 */
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


/**
 * @brief Write a specific byte at the given address in the controller memory.
 *      This function doesn't use VFD_setGridCursor() to map the position
 *      to a grid.
 * @warning Note that the CS/Strobe line is asserted to HIGH (end of transmission)
 *      after the byte has been sent.
 * @param address Value range 0x00..0x15 (22 addresses).
 * @param data Byte to write at the given address.
 */
void VFD_writeByte(uint8_t address, char data){
    VFD_command(PT6312_ADDR_SET_CMD | (address & PT6312_ADDR_MSK), false);
    VFD_command(data, true);
    grid_cursor++;
}


#if ENABLE_ICON_BUFFER == 1
char iconDisplayBuffer[PT6312_MAX_NR_GRIDS * PT6312_BYTES_PER_GRID] = {0};

/**
 * @brief Add an icon to the buffer.
 *      The icon will be displayed on the next call to VFD_writeString(), VFD_writeInt()
 *      or VFD_busySpinningCircle();
 * @param icon_font_index Index of the icon in the ICONS_FONT array.
 *      Defines can be used.
 */
void VFD_setIcon(uint8_t icon_font_index){
    // Get memory address from grid
    // Grid obtained starts from 0
    // Ex: for grid=1: (1+1)*2-2 = 2
    uint8_t addr = convertGridToMemoryAddress(ICONS_FONT[icon_font_index] & 0x0F);
    uint8_t segment = ICONS_FONT[icon_font_index] >> 4;

    // Address in iconDisplayBuffer is depends on the localization of the segment
    // (LSB or MSB)
    if (segment < 8) {
        // LSB
        iconDisplayBuffer[addr] |= 1 << segment;
    } else { // > 7
        // MSB
        iconDisplayBuffer[addr + 1] |= 1 << (segment - 8);
    }
}


/**
 * @brief Remove an icon from the buffer.
 *      The icon will be removed on the next call to VFD_writeString(), VFD_writeInt()
 *      or VFD_busySpinningCircle();
 * @param icon_font_index Index of the icon in the ICONS_FONT array.
 *      Defines can be used.
 */
void VFD_clearIcon(uint8_t icon_font_index){
    // Get memory address from grid
    // Grid obtained starts from 0
    // Ex: for grid=1: (1+1)*2-2 = 2
    uint8_t addr = convertGridToMemoryAddress(ICONS_FONT[icon_font_index] & 0x0F);
    uint8_t segment = ICONS_FONT[icon_font_index] >> 4;

    // Address in iconDisplayBuffer is depends on the localization of the segment
    // (LSB or MSB)
    if (segment < 8) {
        // LSB
        iconDisplayBuffer[addr] |= ~(1 << segment);
    } else { // > 7
        // MSB
        iconDisplayBuffer[addr + 1] |= ~(1 << (segment - 8));
    }
}


/**
 * @brief Clear the icon buffer.
 *      All the icons will be removed on the next call to VFD_writeString(), VFD_writeInt()
 *      or VFD_busySpinningCircle();
 */
void VFD_clearIcons(){
    for(uint8_t i=0; i<(PT6312_MAX_NR_GRIDS * PT6312_BYTES_PER_GRID); i++){
        iconDisplayBuffer[i] = 0;
    }
}


/**
 * @brief Convert grid number to a memory address
 * @param grid Grid number (starting from 0)
 * @return Address of the memory cell in the icon buffer
 *      (or in the memory of the controller).
 */
inline uint8_t convertGridToMemoryAddress(uint8_t grid) {
    return ((grid + 1) * PT6312_BYTES_PER_GRID) - PT6312_BYTES_PER_GRID;
}
#endif

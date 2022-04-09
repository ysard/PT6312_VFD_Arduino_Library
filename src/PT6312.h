/* PT6312 is an Arduino library for the PT6312 family of Vacuum Fluorescent Display controllers.
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
#ifndef ET16312N_H
#define ET16312N_H

#include <avr/io.h>
#include <util/delay.h>
#include <global.h>


/**
 * AVR macros
 * They are all prefixed with '_' to avoid overwriting the functions/macros of the Arduino library.
 */
#define _INPUT                          &= ~
#define _OUTPUT                         |=
#define _HIGH                           |=
#define _LOW                            &= ~
#define _pinMode(DDR, PIN, MODE)        (DDR MODE (1 << PIN))
#define _digitalWrite(PORT, PIN, MODE)  (PORT MODE (1 << PIN))

/**
 * Driver constants
 */
// PT6312 Display and Keymatrix data
#define PT6312_MAX_NR_GRIDS      VFD_GRIDS
#define PT6312_BYTES_PER_GRID    2
// Significant bits Keymatrix data
#define PT6312_KEY_MSK           0xFF
#define PT6312_KEY_SMPL_MSK      0x0F

// Memory size in bytes for Display and Keymatrix
#define PT6312_DISPLAY_MEM       (PT6312_MAX_NR_GRIDS * PT6312_BYTES_PER_GRID)
#define PT6312_KEY_MEM           3

// Reserved bits for commands
#define PT6312_CMD_MSK           0xE0

// Mode setting command
#define PT6312_MODE_SET_CMD      0x00
#define PT6312_GR4_SEG16         0x00
#define PT6312_GR5_SEG16         0x01
#define PT6312_GR6_SEG16         0x02
#define PT6312_GR7_SEG15         0x03 //default
#define PT6312_GR8_SEG14         0x04
#define PT6312_GR9_SEG13         0x05
#define PT6312_GR10_SEG12        0x06
#define PT6312_GR11_SEG11        0x07

// Data setting commands
#define PT6312_DATA_SET_CMD      0x40
#define PT6312_DATA_WR           0x00
#define PT6312_LED_WR            0x01
#define PT6312_KEY_RD            0x02
#define PT6312_SW_RD             0x03
#define PT6312_ADDR_INC          0x00
#define PT6312_ADDR_FIXED        0x04
#define PT6312_MODE_NORM         0x00
#define PT6312_MODE_TEST         0x08

// Key settings data
// In some configurations, you may want to always discard
// the bit of a key press retrieved from the controller.
// See VFD_getKeys() for the structure of byte filtered by this mask.
// By default all bits are kept.
#define PT6312_KEY_MSK           0xFF

// LED settings data
#define PT6312_LED_MSK           0x0F
#define PT6312_LED1              0x01
#define PT6312_LED2              0x02
#define PT6312_LED3              0x04
#define PT6312_LED4              0x08

// Switch settings data
#define PT6312_SW_MSK            0x0F
#define PT6312_SW1               0x01
#define PT6312_SW2               0x02
#define PT6312_SW3               0x04
#define PT6312_SW4               0x08

// Address setting commands
#define PT6312_ADDR_SET_CMD      0xC0
#define PT6312_ADDR_MSK          0x1F

// Display control commands
#define PT6312_DSP_CTRL_CMD      0x80
#define PT6312_BRT_MSK           0x07
#define PT6312_BRT0              0x00 //Pulsewidth 1/16
#define PT6312_BRT1              0x01
#define PT6312_BRT2              0x02
#define PT6312_BRT3              0x03
#define PT6312_BRT4              0x04
#define PT6312_BRT5              0x05
#define PT6312_BRT6              0x06
#define PT6312_BRT7              0x07 //Pulsewidth 14/16

#define PT6312_BRT_DEF           PT6312_BRT7

#define PT6312_DSP_OFF           0x00
#define PT6312_DSP_ON            0x08


/**
 * Library handy macros
 */
// Generic shortcuts
#define VFD_displayOn(BRIGHTNESS)    VFD_setBrightness(BRIGHTNESS)
#define VFD_displayOff()             VFD_command(PT6312_DSP_CTRL_CMD | PT6312_BRT0, true)
#define VFD_home()                   VFD_setGridCursor(1, false)

// Shortcuts for write functions
#define VFD_writeStringPosition(string, position, colon_symbol) \
    {                                                           \
        VFD_setGridCursor(position);                            \
        VFD_writeString(string, colon_symbol);                  \
    }

#define VFD_writeIntPosition(number, digits_number, position, colon_symbol) \
    {                                                                       \
        VFD_setGridCursor(position);                                        \
        VFD_writeInt(number, digits_number, colon_symbol);                  \
    }

/**
 * Global variables
 */
// Grid cursor (starting from 1)
extern uint8_t grid_cursor;
// VFD_busySpinningCircle global variables
extern uint8_t busy_indicator_delay_count;
extern uint8_t busy_indicator_frame;
extern uint8_t busy_indicator_loop_nb;

/**
 * Generic API
 */
void VFD_initialize(void);
void VFD_resetDisplay(void);
void VFD_setBrightness(const uint8_t brightness);
void VFD_clear(void);

/**
 * Display functions
 */
void VFD_setGridCursor(uint8_t position, bool cmd=false);
void VFD_writeString(const char *string, bool colon_symbol); // Adapted if ENABLE_ICON_BUFFER is set
void VFD_writeInt(int32_t number, int8_t digits_number, bool colon_symbol);
void VFD_busySpinningCircleReset(void);
void VFD_busySpinningCircle(void); // Adapted if ENABLE_ICON_BUFFER is set
void VFD_scrollText(const char *string, void (pfunc)()=nullptr);

#if ENABLE_ICON_BUFFER == 1
extern char iconDisplayBuffer[PT6312_MAX_NR_GRIDS * PT6312_BYTES_PER_GRID];
void VFD_setIcon(uint8_t icon_font_index);
void VFD_clearIcon(uint8_t icon_font_index);
void VFD_clearIcons();
inline uint8_t convertGridToMemoryAddress(uint8_t grid);
#endif

/**
 * Keys, switches and LEDs
 */
void VFD_setLEDs(uint8_t leds);
uint32_t VFD_getKeys(void);
uint8_t VFD_getKeyPressed(void);
uint8_t VFD_getSwitches(void);

/**
 * Test functions
 */
void VFD_segmentsGenericTest(void);
void VFD_displayAllSegments(void);
void VFD_displayAllFontGlyphes(void);

/**
 * Low level API
 */
void VFD_command(uint8_t value, bool cmd=false);
inline void VFD_CSSignal(){
    _delay_us(1);
    _digitalWrite(VFD_CS_PORT, VFD_CS_PIN, _HIGH);
    _delay_us(1);
}
uint8_t VFD_readByte(void);
void VFD_writeByte(uint8_t address, char data);

#endif

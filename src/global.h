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
#ifndef ET16312N_GLOBAL_H
#define ET16312N_GLOBAL_H


/**
 * User setup
 */
// MCU IO: Pins, Registers, Ports
#define VFD_CS_DDR              DDRB
#define VFD_CS_PORT             PORTB
#define VFD_CS_PIN              PB0
#define VFD_SCLK_DDR            DDRB
#define VFD_SCLK_PORT           PORTB
#define VFD_SCLK_PIN            PB1
#define VFD_DATA_DDR            DDRB
#define VFD_DATA_PORT           PORTB
#define VFD_DATA_PIN            PB2
#define VFD_DATA_R_ONLY_PORT    PINB
// VFD Display features
#define VFD_GRIDS               4 // Number of grids
#define VFD_DISPLAYABLE_DIGITS  6 // Number of characters that can be displayed simultaneously
#define VFD_SCROLL_SPEED        500 // In milliseconds
// Library options
#define ENABLE_ICON_BUFFER      0 // Enable functions and extra buffer to display icons (except spinning circle)

// Fonts (files are included in ET16312N.cpp)
// "2 chars per grid display"
#define VFD_VARIANT_1
// "1 char per grid display"
// #define VFD_VARIANT_2

#endif // ET16312N_GLOBAL_H

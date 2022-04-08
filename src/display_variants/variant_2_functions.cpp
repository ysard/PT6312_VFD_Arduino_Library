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

/* This file is dedicated for a "1 char per grid display".
 */

 /**
 * @brief Write a string of characters present in the font
 * @param string String must be null terminated '\0'. Grid cursor is auto-incremented.
 *          For this display 1 grid = 1 character.
 * @param colon_symbol Boolean set to true to display the special colon symbol
 *          segment on grid 3 or 5.
 *          The symbol is displayed between chars 3 and 4, or 4 and 5.
 * @warning The string MUST be null terminated.
 */
void VFD_writeString(const char *string, bool colon_symbol){
    uint8_t chrset;

    while(*string > '\0'){ // TODO: security test cursor <= VFD_DIGITS
        // Send LSB
        chrset = FONT[*string - 0x20][1];

        #if VFD_COLON_SYMBOL_BIT < 9
        // Set optional colon symbol (if its bit number is < 9, starting from 1)
        if (colon_symbol && (grid_cursor == 3 || grid_cursor == 5)){
            // Add the symbol on the MSB part of the byte
            chrset |= 1 << (VFD_COLON_SYMBOL_BIT - 1);
        }
        #endif

        #if ENABLE_ICON_BUFFER == 1
        uint8_t memory_addr = (grid_cursor * PT6312_BYTES_PER_GRID) - PT6312_BYTES_PER_GRID;
        VFD_command(chrset | iconDisplayBuffer[memory_addr], false);
        #else
        VFD_command(chrset, false);
        #endif

        // Send MSB
        chrset = FONT[*string - 0x20][0];

        #if VFD_COLON_SYMBOL_BIT > 8
        // Set optional colon symbol (if its bit number is > 8, starting from 1)
        if (colon_symbol && (grid_cursor == 3 || grid_cursor == 5)){
            // Add the symbol on the MSB part of the byte
            chrset |= 1 << (VFD_COLON_SYMBOL_BIT - 9);
        }
        #endif

        #if ENABLE_ICON_BUFFER == 1
        VFD_command(chrset | iconDisplayBuffer[memory_addr + 1], false);
        #else
        VFD_command(chrset, false);
        #endif

        grid_cursor++;
        string++;
    }

    // Signal the driver that the data transmission is over
    VFD_CSSignal();
}

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

/* This file is dedicated for a "1 char per grid display".
 */

 /**
 * @brief Write a string of characters present in the font (If VARIANT_2 is defined in global.h).
 * @param string String must be null terminated '\0'. Grid cursor is auto-incremented.
 *          For this display 1 grid = 1 character.
 * @param colon_symbol Boolean set to true to display the special colon symbol
 *          segment on grid 3 or 5.
 *          The symbol is displayed between chars 3 and 4, or 4 and 5.
 * @warning The string MUST be null terminated.
 */
void VFD_writeString(const char *string, bool colon_symbol){
    uint8_t chrset;

    while(*string > '\0'){ // TODO: security test cursor <= VFD_GRIDS
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


/**
 * @brief Animation for a busy spinning circle that uses 2 bytes (full grid).
 * @param position Grid number where the animation frames must be displayed.
 * @param frame_number Current frame to display (Value range 1..6 (6 segments));
 *      This value is updated when the frame is modified.
 *      The frame number goes back to 1 once 6 is exceeded.
 * @param loop_number Number of refreshes for a frame; used to set the duty cycle of fading frames.
 *      This value is incremented at each call.
 * @note
 *      The animation takes place on the current position set by the value of cursor.
 *      The concerned segments for this display are localized on the grid 1.
 *      The segments are: 4, 5, 16, 13, 12, 1.
 *
 *      A same frame is refreshed 70 times before moving to the next.
 *      An entire loop is made in 420 calls (6 frames * 70 calls each).
 *      It's up to you to adjust the total time of a loop to 1 second by setting up
 *      a delay (VFD_BUSY_DELAY) after a call (should be ~2.35ms).
 *      The number of refreshes for a frame is stored in loop_number.
 *
 *      A frame is composed of segments displayed at different duty cycles (1, 1/2, 1/5, 1/12)
 *      to obtain a fading effect for the segments behind the main segment.
 *      Ex: For 5th main segment:
 *          16, 13, 12 are displayed, from the most marked to the darkest;
 *          the others are not displayed (1, 4).
 * @note grid_cursor global variable is incremented and kept valid by ths function.
 * @see VFD_busyWrapper()
 */
void VFD_busySpinningCircle(uint8_t position, uint8_t &frame_number, uint8_t &loop_number){
    uint8_t msb = 0, lsb = 0;
    // Init duty cycles divisors
    uint8_t seg2_duty_cycle = 2, seg3_duty_cycle = 5, seg4_duty_cycle = 12;

    // Compute duty cycles triggers
    seg2_duty_cycle = loop_number % seg2_duty_cycle; // 1/2
    seg3_duty_cycle = loop_number % seg3_duty_cycle; // 1/5
    seg4_duty_cycle = loop_number % seg4_duty_cycle; // 1/12

    // Left shifts notes from segment number to bit number:
    // msb: segment number -8 -1
    // lsb: segment number -1
    // Segments successively displayed with 100% of the duty cycle of 1 frame:
    // 4, 5, 16, 13, 12, 1
    // The 3 segments that precede the main displayed segment are fading more and more pronounced.
    if(frame_number == 1){
        lsb = 1 << (4 - 1); // segment 4 (first)
    }else if(frame_number == 2){
        lsb = 1 << (1 - 1); // segment 1 (second)
        if(seg2_duty_cycle == 0){
            lsb |= 1 << (4 - 1); // segment 4
        }
    }else if(frame_number == 3){
        msb = 1 << (12 - 8 - 1); // segment 12 (third)
        if(seg2_duty_cycle == 0){
            lsb = 1 << (1 - 1); // segment 1
        }
        if(seg3_duty_cycle == 0){
            lsb |= 1 << (4 - 1); // segment 4
        }
    }else if(frame_number == 4){
        msb = 1 << (13 - 8 - 1); // segment 13 (fourth)
        if(seg2_duty_cycle == 0){
            msb |= 1 << (12 - 8 - 1); // segment 12
        }
        if(seg3_duty_cycle == 0){
            lsb = 1 << (1 - 1); // segment 1
        }
        if(seg4_duty_cycle == 0){
            lsb |= 1 << (4 - 1); // segment 4
        }
    }else if(frame_number == 5){
        msb = 1 << (16 - 8 - 1); // segment 16 (fifth)
        if(seg2_duty_cycle == 0){
            msb |= 1 << (13 - 8 - 1); // segment 13
        }
        if(seg3_duty_cycle == 0){
            msb |= 1 << (12 - 8 - 1); // segment 12
        }
        if(seg4_duty_cycle == 0){
            lsb |= 1 << (1 - 1); // segment 1
        }
    }else if(frame_number == 6){
        lsb = 1 << (5 - 1); // segment 5 (sixth)
        if(seg2_duty_cycle == 0){
            msb |= 1 << (16 - 8 - 1); // segment 16
        }
        if(seg3_duty_cycle == 0){
            msb = 1 << (13 - 8 - 1); // segment 13
        }
        if(seg4_duty_cycle == 0){
            msb |= 1 << (12 - 8 - 1); // segment 12
        }
    }

    loop_number++;
    if(loop_number == 70){
        if(frame_number == 6) frame_number = 0;
        frame_number++;
        loop_number = 0;
    }

    VFD_setGridCursor(position);
    #if ENABLE_ICON_BUFFER == 1
    uint8_t address = convertGridToMemoryAddress(position);
    VFD_command(lsb | iconDisplayBuffer[address], false);
    VFD_command(msb | iconDisplayBuffer[address+1], true);
    #else
    VFD_command(lsb, false);
    VFD_command(msb, true);
    #endif

    // Sync cursor
    grid_cursor++;
}

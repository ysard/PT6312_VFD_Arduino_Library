/* Arduino Library for AD16312, HT16512, PT6312, etc. VFD Controller.
 * Copyright (C) 2022 Ysard - <ysard@users.noreply.github.com>
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

/* This file is dedicated for a "2 chars per grid display".
 */

 /**
 * @brief Write a string of characters present in the font (If VARIANT_1 is defined in global.h).
 * @param string String must be null terminated '\0'. Grid cursor is auto-incremented.
 *          For this display 6 characters can be displayed simultaneously.
 *          For positions 3 and 4, the grids accept 2 characters.
 *          Positions 1 and 2 accept only 1 char (segments of LSB only),
 *          the other positions are reserved for icons.
 * @param colon_symbol Boolean set to true to display the special colon symbol
 *          segment on grid 4.
 *          The symbol is displayed between chars 4 and 5.
 * @warning The string MUST be null terminated.
 */
void VFD_writeString(const char *string, bool colon_symbol){
    uint8_t lsb_byte;
    uint8_t msb_byte;

    while(*string > '\0'){ // TODO: security test cursor <= VFD_GRIDS//DISPLAYABLE

        if (grid_cursor == 3 || grid_cursor == 4){
            // Cursor positions: 3 or 4: 2 chars per grid
            // MSB: Get LSB of left/1st char
            msb_byte = FONT[*string - 0x20][1];
            string++;
            // Test char validity
            if (*string > '\0'){
                // LSB: Get LSB of right/2nd char
                lsb_byte = FONT[*string - 0x20][1];
            } else {
                lsb_byte = 0;
                string--; // Allow end of while loop
            }

            // Set optional colon symbol
            if (colon_symbol && grid_cursor == 4){
                #if VFD_COLON_SYMBOL_BIT > 8
                // Add the symbol on the MSB part of the grid
                msb_byte |= 1 << (VFD_COLON_SYMBOL_BIT - 9);
                #else // < 9
                // Add the symbol on the LSB part of the grid
                lsb_byte |= 1 << (VFD_COLON_SYMBOL_BIT - 1);
                #endif
            }
        }else{
            // Cursor positions: 1 or 2: 1 char only
            // TODO: set only the LSB part to avoid erasing MSB part ?
            // Send LSB
            lsb_byte = FONT[*string - 0x20][1];
            // Send MSB
            msb_byte = FONT[*string - 0x20][0];
        }

        #if ENABLE_ICON_BUFFER == 1
        // Merge icons and char data
        uint8_t memory_addr = (grid_cursor * PT6312_BYTES_PER_GRID) - PT6312_BYTES_PER_GRID;
        VFD_command(lsb_byte | iconDisplayBuffer[memory_addr], false);
        VFD_command(msb_byte | iconDisplayBuffer[memory_addr + 1], false);
        #else
        VFD_command(lsb_byte, false);
        VFD_command(msb_byte, false);
        #endif

        grid_cursor++;
        string++;
    }

    // Signal the driver that the data transmission is over
    VFD_CSSignal();
}


// VFD_busySpinningCircle global variables
uint8_t busy_indicator_delay_count;
uint8_t busy_indicator_frame;
uint8_t busy_indicator_loop_nb;

/**
 * @brief Reset counters of the spinning circle animation
 * @warning Must be used before initiating an animation
 * @see VFD_busySpinningCircle()
 */
void VFD_busySpinningCircleReset(void){
    busy_indicator_delay_count = 0;
    busy_indicator_frame = 1;
    busy_indicator_loop_nb = 0;
}

/**
 * @brief Animation for a busy spinning circle.
 * @note
 *      The animation takes place on the current position set by the value of cursor.
 *      The concerned segments for this display are localized on the grid 1.
 *      The segments are: 11, 12, 13, 14, 15, 16 (it's the MSB part of the grid).
 *
 *      The loops count is stored in busy_indicator_delay_count.
 *      The screen is refreshed every 2 calls.
 *
 *      A same frame is refreshed 70 times before moving to the next.
 *      So there is a frame change every 140 calls.
 *      An entire loop is made in 840 calls (6 frames * 140 calls each).
 *      It's up to you to adjust the total time of a loop to 1 second by setting up
 *      a delay (_delay_ms()) after a call (should be ~1.17ms).
 *      The number of refreshes for a frame is stored in busy_indicator_loop_nb.
 *
 *      A frame is composed of segments displayed at different duty cycles (1, 1/2, 1/5, 1/12)
 *      to obtain a fading effect for the segments behind the main segment.
 *      Ex: For 16th main segment:
 *          15, 14, 13 are displayed, from the most marked to the darkest;
 *          the others are not displayed (12, 11).
 *
 *
 *      The frame number is stored in busy_indicator_frame (value range 1-6 (6 segments)).
 *      The frame number goes back to 1 once 6 is exceeded.
 */
void VFD_busySpinningCircle(void){
    if(busy_indicator_delay_count == 0){
        uint8_t msb = 0;
        // Init duty cycles divisors
        uint8_t seg2_duty_cycle = 2, seg3_duty_cycle = 5, seg4_duty_cycle = 12;

        // Compute duty cycles triggers
        seg2_duty_cycle = busy_indicator_loop_nb % seg2_duty_cycle; // 1/2
        seg3_duty_cycle = busy_indicator_loop_nb % seg3_duty_cycle; // 1/5
        seg4_duty_cycle = busy_indicator_loop_nb % seg4_duty_cycle; // 1/12

        // Left shifts notes from segment number to bit number:
        // msb: segment number -8 -1
        // lsb: segment number -1
        // Segments successively displayed with 100% of the duty cycle of 1 frame:
        // 11, 12, 13, 14, 15, 16
        // The 3 segments that precede the main displayed segment are fading more and more pronounced.
        if(busy_indicator_frame == 1){
            msb = 1 << (11 - 8 - 1); // segment 11 (first)
        }else if(busy_indicator_frame == 2){
            msb = 1 << (12 - 8 - 1); // segment 12 (second)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (11 - 8 - 1); // segment 11
            }
        }else if(busy_indicator_frame == 3){
            msb = 1 << (13 - 8 - 1); // segment 13 (third)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (12 - 8 - 1); // segment 12
            }
            if(seg3_duty_cycle == 0){
                msb |= 1 << (11 - 8 - 1); // segment 11
            }
        }else if(busy_indicator_frame == 4){
            msb = 1 << (14 - 8 - 1); // segment 14 (fourth)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (13 - 8 - 1); // segment 13
            }
            if(seg3_duty_cycle == 0){
                msb |= 1 << (12 - 8 - 1); // segment 12
            }
            if(seg4_duty_cycle == 0){
                msb |= 1 << (11 - 8 - 1); // segment 11
            }
        }else if(busy_indicator_frame == 5){
            msb = 1 << (15 - 8 - 1); // segment 15 (fifth)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (14 - 8 - 1); // segment 14
            }
            if(seg3_duty_cycle == 0){
                msb |= 1 << (13 - 8 - 1); // segment 13
            }
            if(seg4_duty_cycle == 0){
                msb |= 1 << (12 - 8 - 1); // segment 12
            }
        }else if(busy_indicator_frame == 6){
            msb = 1 << (16 - 8 - 1); // segment 16 (sixth)
            if(seg2_duty_cycle == 0){
                msb |= 1 << (15 - 8 - 1); // segment 14
            }
             if(seg3_duty_cycle == 0){
                msb |= 1 << (14 - 8 - 1); // segment 15
            }
            if(seg4_duty_cycle == 0){
                msb |= 1 << (13 - 8 - 1); // segment 13
            }
        }

        busy_indicator_delay_count = 2;
        busy_indicator_loop_nb++;
        if(busy_indicator_loop_nb == 70){
            if(busy_indicator_frame == 6) busy_indicator_frame = 0;
            busy_indicator_frame++;
            busy_indicator_loop_nb = 0;
        }

        #if ENABLE_ICON_BUFFER == 1
        VFD_writeByte(grid_cursor, msb | iconDisplayBuffer[grid_cursor]);
        #else
        VFD_writeByte(grid_cursor, msb); // TODO: receive specific addr to write this byte from param ?
        #endif

        // If the spinning circle was on 2 bytes, lsb and msb should be sent.
        // Ex:
        // VFD_command(lsb, false);
        // VFD_command(msb, false);
        // VFD_CSSignal();

        grid_cursor++;

        // Reset/Update display
        // => Don't know why but it appears to be mandatory to avoid forever black screen... (?)
        VFD_resetDisplay();
    }
    --busy_indicator_delay_count;
}

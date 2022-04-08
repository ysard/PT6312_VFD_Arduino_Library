#include "ET16312N.h"

/**
 * @brief Write a string of characters present in the font
 * @param string String must be null terminated '\0'. Cursor is auto-incremented.
 *          For this display 6 characters can be displayed simultaneously.
 *          For positions 3 and 4, the grids accept 2 characters.
 *          Positions 1 and 2 accept only 1 char (segments of LSB only).
 * @param colon_symbol Boolean set to true to display the special colon symbol
 *          segment on grid 4.
 *          The symbol is displayed between chars 4 and 5.
 * @warning The string MUST be null terminated.
 * @todo TODO: fill the display buffer
 * @todo TODO: take an icon mask for every char
 */
void VFD_writeString(const char *string, bool colon_symbol){
    uint8_t lsb_byte;
    uint8_t msb_byte;

    while(*string > '\0'){ // TODO: security test cursor <= VFD_DIGITS//DISPLAYABLE

        if (cursor == 3 || cursor == 4){
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
            if (colon_symbol && cursor == 4){
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
        uint8_t memory_addr = (cursor * PT6312_BYTES_PER_GRID) - PT6312_BYTES_PER_GRID;
        VFD_command(lsb_byte | iconDisplayBuffer[memory_addr], false);
        VFD_command(msb_byte | iconDisplayBuffer[memory_addr + 1], false);
        #else
        VFD_command(lsb_byte, false);
        VFD_command(msb_byte, false);
        #endif

        cursor++;
        string++;
    }

    // Signal the driver that the data transmission is over
    VFD_CSSignal();
}

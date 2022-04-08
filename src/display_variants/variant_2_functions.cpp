/**
 * @brief Write a string of characters present in the font
 * @param string String must be null terminated '\0'. Cursor is auto-incremented.
 *          For this display 1 grid = 1 character.
 * @param colon_symbol Boolean set to true to display the special colon symbol
 *          segment on grid 3 or 5.
 *          The symbol is displayed between chars 3 and 4, or 4 and 5.
 * @warning The string MUST be null terminated.
 */
void VFD_writeString(const char *string, bool colon_symbol){
    uint8_t chrset;

    while(*string > '\0'){ // TODO: security test cursor <= VFD_DIGITS
        #if ENABLE_MENU_INTERFACE_RELATED == 1 //TODO UPDATE
        if(writeToLastString){
            // Store the string to be used by the VFDBlinkUserInput function
            if(cursor <= VFD_DIGITS && *string != ':'){
                displaybuffer[cursor-1] = *string;
            }
        }
        #endif

        // Send LSB
        chrset = FONT[*string - 0x20][1];

        #if VFD_COLON_SYMBOL_BIT < 9
        // Set optional colon symbol (if its bit number is < 9, starting from 1)
        if (colon_symbol && (cursor == 3 || cursor == 5)){
            // Add the symbol on the MSB part of the byte
            chrset |= 1 << (VFD_COLON_SYMBOL_BIT - 1);
        }
        #endif

        #if ENABLE_ICON_BUFFER == 1
        uint8_t memory_addr = (cursor * PT6312_BYTES_PER_GRID) - PT6312_BYTES_PER_GRID;
        VFD_command(chrset | iconDisplayBuffer[memory_addr], false);
        #else
        VFD_command(chrset, false);
        #endif

        // Send MSB
        chrset = FONT[*string - 0x20][0];

        #if VFD_COLON_SYMBOL_BIT > 8
        // Set optional colon symbol (if its bit number is > 8, starting from 1)
        if (colon_symbol && (cursor == 3 || cursor == 5)){
            // Add the symbol on the MSB part of the byte
            chrset |= 1 << (VFD_COLON_SYMBOL_BIT - 9);
        }
        #endif

        #if ENABLE_ICON_BUFFER == 1
        VFD_command(chrset | iconDisplayBuffer[memory_addr + 1], false);
        #else
        VFD_command(chrset, false);
        #endif

        cursor++;
        string++;
    }

    // Signal the driver that the data transmission is over
    VFD_CSSignal();
}

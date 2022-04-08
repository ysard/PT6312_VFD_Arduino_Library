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
#define VFD_DIGITS              4 // Number of grids
#define VFD_DISPLAYABLE_DIGITS  6 // Number of characters that can be displayed simultaneously
#define VFD_SCROLL_SPEED        500 // In milliseconds
// Library options
#define ENABLE_ICON_BUFFER      0 // Enable functions and extra buffer to display icons (except spinning circle)

// Fonts (files are included in ET16312N.cpp)
// "2 chars per grid display"
#define VFD_VARIANT_1
// "1 char per grid display"
#define VFD_VARIANT_2

#endif // ET16312N_GLOBAL_H

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
#ifndef FONT_H
#define FONT_H
// Segment numbering for ET16312n VFD driver
//         8
//     ---------
//    |\        |
//  6 | 1       | 7
//    |  \      |
//    -----5-----
//    |     \   |
//  3 |      1  | 4
//    |       \ |
//     ---------
//         2
//
// ASCII codes
#define VFD_COLON_SYMBOL_BIT    1  // Segment number (starting from 1)
const uint8_t FONT[65][2] = {
    {0b00000000, 0b00000000}, // space 0x20
    {0b00000000, 0b00000000}, // ! N/A
    {0b00000000, 0b00000000}, // " N/A
    {0b00000000, 0b00000000}, // # N/A
    {0b00000000, 0b00000000}, // $ N/A
    {0b00000000, 0b00000000}, // % N/A
    {0b00000000, 0b00000000}, // & N/A
    {0b00000000, 0b00000000}, // ' N/A
    {0b00000000, 0b10100110}, // ( (bits 8, 6, 3, 2)
    {0b00000000, 0b11001010}, // ) (bits 8, 7, 4, 2)
    {0b00000000, 0b00000000}, // * N/A
    {0b00000000, 0b00000000}, // + N/A
    {0b00000000, 0b00000000}, // , N/A
    {0b00000000, 0b00010000}, // - (bit 5)
    {0b00000000, 0b00000000}, // . N/A
    {0b00000000, 0b00000000}, // / N/A
    {0b00000000, 0b11101110}, // 0 (bits 8, 7, 6, 4, 3, 2)
    {0b00000000, 0b01001000}, // 1 (bits 7, 4)
    {0b00000000, 0b11010110}, // 2 (bits 8, 7, 5, 3, 2)
    {0b00000000, 0b11011010}, // 3 (bits 8, 7, 5, 4, 2)
    {0b00000000, 0b01111000}, // 4 (bits 7, 6, 5, 4)
    {0b00000000, 0b10111010}, // 5 (bits 8, 6, 5, 4, 2)
    {0b00000000, 0b10111110}, // 6 (bits 8, 6, 5, 4, 3, 2)
    {0b00000000, 0b11001000}, // 7 (bits 8, 7, 4)
    {0b00000000, 0b11111110}, // 8 (bits 8, 7, 6, 5, 4, 3, 2)
    {0b00000000, 0b11111010}, // 9 (bits 8, 7, 6, 5, 4, 2)
    {0b00000000, 0b00000001}, // : (bit 1) available before 5-th digit only
    {0b00000000, 0b00000000}, // ; N/A
    {0b00000000, 0b00000000}, // < N/A
    {0b00000000, 0b00010010}, // = (bits 5, 2)
    {0b00000000, 0b00000000}, // > N/A
    {0b00000000, 0b00000000}, // ? N/A
    {0b00000000, 0b00000000}, // @ N/A
    {0b00000000, 0b11111100}, // A (bits 8, 7, 6, 5, 4, 3)
    {0b00000000, 0b11111110}, // B (bits 8, 7, 6, 5, 4, 3, 2)
    {0b00000000, 0b10100110}, // C (bits 8, 6, 3, 2)
    {0b00000000, 0b11101110}, // D (bits 8, 7, 6, 4, 3, 2)
    {0b00000000, 0b10110110}, // E (bits 8, 6, 5, 3, 2)
    {0b00000000, 0b10110100}, // F (bits 8, 6, 5, 3)
    {0b00000000, 0b10111110}, // G (bits 8, 6, 5, 4, 3, 2)
    {0b00000000, 0b01111100}, // H (bits 7, 6, 5, 4, 3)
    {0b00000000, 0b01001000}, // I (bits 7, 4)
    {0b00000000, 0b01001010}, // J (bits 7, 4, 2)
    {0b00000000, 0b00000000}, // K N/A
    {0b00000000, 0b00100110}, // L (bits 6, 3, 2)
    {0b00000000, 0b11101100}, // M (bits 8, 7, 6, 4, 3)
    {0b00000000, 0b11101100}, // N (bits 8, 7, 6, 4, 3)
    {0b00000000, 0b00011110}, // O (bits 5, 4, 3, 2)
    {0b00000000, 0b11100100}, // P (bits 8, 7, 6, 3)
    {0b00000000, 0b00011110}, // Q (bits 5, 4, 3, 2)
    {0b00000000, 0b11010110}, // R (bits 8, 7, 5, 3, 2)
    {0b00000000, 0b10111010}, // S (bits 8, 6, 5, 4, 2)
    {0b00000000, 0b11001000}, // T (bits 8, 7, 4)
    {0b00000000, 0b01101110}, // U (bits 7, 6, 4, 3, 2)
    {0b00000000, 0b01101110}, // V (bits 7, 6, 4, 3, 2)
    {0b00000000, 0b01101110}, // W (bits 7, 6, 4, 3, 2)
    {0b00000000, 0b11111110}, // X (bits 8, 7, 6, 5, 4, 3, 2)
    {0b00000000, 0b01111000}, // Y (bits 7, 6, 5, 4)
    {0b00000000, 0b11010110}, // Z (bits 8, 7, 5, 3, 2)
    {0b00000000, 0b10100110}, // [ (bits 8, 6, 3, 2)
    {0b00000000, 0b00000001}, // \ (bit 1) available for 1st digit only
    {0b00000000, 0b11001010}, // ] (bits 8, 7, 4, 2)
    {0b00000000, 0b00000000}, // ^ N/A
    {0b00000000, 0b00000010}, // _ (bit 2)
};

// Shortcuts refering to the indexes in the table ICONS_FONT
#define ICON_PBC          0
#define ICON_DVD          1
#define ICON_PLAY         2
#define ICON_S            3
#define ICON_SPEAKER_R    4
#define ICON_SPEAKER_L    5
#define ICON_QUADRATIN    6
#define ICON_ANTENNAS     7
#define ICON_PAUSE        8
#define ICON_CD           9
#define ICON_V            10
#define ICON_DTS          11
#define ICON_DOLBY        12
#define ICON_COLON        13
#define ICON_MP3          14

// 2 sections of 4 bits in 1 byte:
// LSB: grid number starting from 0
// MSB: segment number starting from 0
const uint8_t ICONS_FONT[] = {
    0b10000000, // Index 0:  Grid 0; 9;  PBC
    0b10010000, // Index 1:  Grid 0; 10; DVD
    0b00000001, // Index 2:  Grid 1; 1;  Play
    0b10000001, // Index 3:  Grid 1; 9;  S
    0b10010001, // Index 4:  Grid 1; 10; Speaker right
    0b10100001, // Index 5:  Grid 1; 11; Speaker left
    0b10110001, // Index 6:  Grid 1; 12; Quadratin
    0b11000001, // Index 7:  Grid 1; 13; Antennas
    0b11010001, // Index 8:  Grid 1; 14; Pause
    0b11100001, // Index 9:  Grid 1; 15; CD
    0b11110001, // Index 10: Grid 1; 16; V
    0b00000010, // Index 11: Grid 2; 1;  DTS
    0b10000010, // Index 12: Grid 2; 9;  Dolby Digital
    0b00000011, // Index 13: Grid 3; 1;  Colon
    0b10000011, // Index 14: Grid 3; 9;  MP3
};

#endif

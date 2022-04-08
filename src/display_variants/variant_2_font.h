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
#ifndef FONT_H
#define FONT_H
// Segment numbering for ET16312n VFD driver
//         7
//     ---------
//    |\   |   /|
//  2 | 5  6  4 | 3
//    |  \ | /  |
//    -16--9--1--
//    |   /|\   |
// 14 | 13 6 12 | 15
//    | /  |  \ |
//     ---------
//         11
//
// ASCII codes
#define VFD_COLON_SYMBOL_BIT    10
const uint8_t FONT[65][2] = {
    {0b00000000, 0b00000000}, // space 0x20
    {0b00000000, 0b00000000}, // ! N/A
    {0b00000000, 0b00000000}, // " N/A
    {0b00000000, 0b00000000}, // # N/A
    {0b11000101, 0b01100011}, // $ (bits 16, 15, 11, 9, 7, 6, 2, 1)
    {0b00000000, 0b00000000}, // % N/A
    {0b00000000, 0b00000000}, // & N/A
    {0b00000000, 0b00000000}, // ' N/A
    {0b00001001, 0b00001000}, // ( (bits 12, 9, 4)
    {0b00010001, 0b00010000}, // ) (bits 13, 9, 5)
    {0b10011001, 0b00111001}, // * (bits 16, 13, 12, 9, 6, 5, 4, 1)
    {0b10000001, 0b00100001}, // + (bits 16, 9, 6, 1)
    {0b00000000, 0b00000000}, // , N/A
    {0b10000001, 0b00000001}, // - (bits 16, 9, 1)
    {0b00000001, 0b00000000}, // . (bit 9)
    {0b00010001, 0b00001000}, // / (bits 13, 9, 4)
    {0b01110100, 0b01001110}, // 0 (bits 15, 14, 13, 11, 7, 4, 3, 2)
    {0b01000000, 0b00001100}, // 1 (bits 15, 4, 3)
    {0b10100101, 0b01000101}, // 2 (bits 16, 14, 11, 9, 7, 3, 1)
    {0b11000101, 0b01000101}, // 3 (bits 16, 15, 11, 9, 7, 3, 1)
    {0b11000001, 0b00000111}, // 4 (bits 16, 15, 9, 3, 2, 1)
    {0b11000101, 0b01000011}, // 5 (bits 16, 15, 11, 9, 7, 2, 1)
    {0b11100101, 0b01000011}, // 6 (bits 16, 15, 14, 11, 9, 7, 2, 1)
    {0b00010001, 0b01001000}, // 7 (bits 13, 9, 7, 4)
    {0b11100101, 0b01000111}, // 8 (bits 16, 15, 14, 11, 9, 7, 3, 2, 1)
    {0b11000101, 0b01000111}, // 9 (bits 16, 15, 11, 9, 7, 3, 2, 1)
    {0b00000010, 0b00000000}, // : (bit 10)  available for 3-th and 5-th digits only
    {0b00000000, 0b00000000}, // ; N/A
    {0b00001000, 0b00001000}, // < (bits 12, 4)
    {0b10000101, 0b00000001}, // = (bits 16, 11, 9, 1)
    {0b00010000, 0b00010000}, // > (bits 13, 5)
    {0b00000000, 0b00000000}, // ? N/A
    {0b00000000, 0b00000000}, // @ N/A
    {0b11100001, 0b01000111}, // A (bits 16, 15, 14, 9, 7, 3, 2, 1)
    {0b01000101, 0b01100101}, // B (bits 15, 11, 9, 7, 6, 3, 1)
    {0b00100100, 0b01000010}, // C (bits 14, 11, 7, 2)
    {0b01000101, 0b01100100}, // D (bits 15, 11, 9, 7, 6, 3)
    {0b10100101, 0b01000011}, // E (bits 16, 14, 11, 9, 7, 2, 1)
    {0b10100001, 0b01000010}, // F (bits 16, 14, 9, 7, 2)
    {0b01100100, 0b01000011}, // G (bits 15, 14, 11, 7, 2, 1)
    {0b11100001, 0b00000111}, // H (bits 16, 15, 14, 9, 3, 2, 1)
    {0b00000001, 0b00100000}, // I (bits 9, 6)
    {0b01100100, 0b00000100}, // J (bits 15, 14, 11, 3)
    {0b10101001, 0b00001010}, // K (bits 16, 14, 12, 9, 4, 2)
    {0b00100100, 0b00000010}, // L (bits 14, 11, 2)
    {0b01100000, 0b00011110}, // M (bits 15, 14, 5, 4, 3, 2)
    {0b01101001, 0b00010110}, // N (bits 15, 14, 12, 9, 5, 3, 2)
    {0b01100100, 0b01000110}, // O (bits 15, 14, 11, 7, 3, 2)
    {0b10100001, 0b01000111}, // P (bits 16, 14, 9, 7, 3, 2, 1)
    {0b01101100, 0b01000110}, // Q (bits 15, 14, 12, 11, 7, 3, 2)
    {0b10101001, 0b01000111}, // R (bits 16, 14, 12, 9, 7, 3, 2, 1)
    {0b11000101, 0b01000011}, // S (bits 16, 15, 11, 9, 7, 2, 1)
    {0b00000001, 0b01100000}, // T (bits 9, 7, 6)
    {0b01100100, 0b00000110}, // U (bits 15, 14, 11, 3, 2)
    {0b00110000, 0b00001010}, // V (bits 14, 13, 4, 2)
    {0b01111001, 0b00000110}, // W (bits 15, 14, 13, 12, 9, 3, 2)
    {0b00011001, 0b00011000}, // X (bits 13, 12, 9, 5, 4)
    {0b00010001, 0b00011000}, // Y (bits 13, 9, 5, 4)
    {0b00010101, 0b01001000}, // Z (bits 13, 11, 9, 7, 4)
    {0b00100100, 0b01000010}, // [ (bits 14, 11, 7, 2)
    {0b00001001, 0b00010000}, // \ (bits 12, 9, 5)
    {0b01000100, 0b01000100}, // ] (bits 15, 11, 7, 3)
    {0b00000000, 0b00000000}, // ^ N/A
    {0b00000100, 0b00000000}, // _ (bits 11)

    //Special non-standard symbols
    {0b10000001, 0b01000111}  // degree (bits 16, 9, 7, 3, 2, 1) (index 64)
};


// Shortcuts refering to the indexes in the table ICONS_FONT
#define ICON_COLON_1      0
#define ICON_COLON_2      1

// LSB: grid number starting from 0
// MSB: segment number starting from 0
const uint8_t ICONS_FONT[] = {
    0b10010011, // Index 0:  Grid 3; 9;  Colon
    0b00010101, // Index 13: Grid 5; 9;  Colon
};

#endif

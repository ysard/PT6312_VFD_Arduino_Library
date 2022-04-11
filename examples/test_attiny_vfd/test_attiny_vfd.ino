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
#include "PT6312.h"

char currentTime[] = "000655"; // 06h55m00s

/**
 * @brief Refresh currentTime string with a time updated by 1 second.
 */
void updateTime()
{
    // Read previous numbers from currentTime char array
    // ASCII values of numbers start from 0x30 (0)
    uint8_t seconds = (currentTime[0] - 0x30) * 10 + currentTime[1] - 0x30;
    uint8_t hours   = (currentTime[2] - 0x30) * 10 + currentTime[3] - 0x30;
    uint8_t minutes = (currentTime[4] - 0x30) * 10 + currentTime[5] - 0x30;

    // Add 1 sec, recompute minutes and hours
    seconds++;
    if (seconds >= 60) {
        seconds = 0;
        minutes++;
    }
    if (minutes >= 60) {
        minutes = 0;
        hours++;
    }
    if (hours >= 24) {
        hours = 0;
    }

    // Refresh the currentTime string
    currentTime[0] = (seconds / 10) + 0x30;
    currentTime[1] = (seconds % 10) + 0x30;
    currentTime[2] = (hours / 10) + 0x30;
    currentTime[3] = (hours % 10) + 0x30;
    currentTime[4] = (minutes / 10) + 0x30;
    currentTime[5] = (minutes % 10) + 0x30;
}


/**
 * @brief Function called at the end of each iteration of VFD_scrollText() function.
 */
void scrollCallback(){
    // Do whatever you want here

    // Currently let's just blink a LED
    _digitalWrite(PORTB, PB4, _HIGH);
    _delay_ms(500);
    _digitalWrite(PORTB, PB4, _LOW);
}


void setup(){
    // Just configure a blinking LED on Pin 3 (PB4)
    _pinMode(DDRB, PB4, _OUTPUT);
    _digitalWrite(PORTB, PB4, _HIGH);

    VFD_initialize();

    // Enable LED output
    // VFD_setLEDs(PT6312_LED3);

    // Set specific icons (Enable ENABLE_ICON_BUFFER in the library config)
    // VFD_setIcon(ICON_DVD);
    // VFD_setIcon(ICON_PLAY);
    // VFD_setIcon(ICON_DOLBY);

    // Reset the display & preserve the icons
    // VFD_clear();

    // Test all segments
    VFD_displayAllSegments();
    _delay_ms(3000);

    // Test dimming
    VFD_setBrightness(1);
    _delay_ms(1000);
    VFD_setBrightness(2);
    _delay_ms(1000);
    VFD_setBrightness(3);
    _delay_ms(1000);
    VFD_setBrightness(4);
    _delay_ms(1000);
    VFD_setBrightness(5);
    _delay_ms(1000);
    VFD_setBrightness(7);
    _delay_ms(1000);

    // Scrolling text
    VFD_home();
    VFD_scrollText("HELLO WORLD", &scrollCallback);
}


void loop(){
    _digitalWrite(PORTB, PB4, _HIGH);

    // Display time
    updateTime();
    VFD_home();
    VFD_writeString(currentTime, true); // True is used to end the transmission

    // Spinning circle segments are on grid 1, MSB (address 1)
    VFD_busyWrapper(1);
    _digitalWrite(PORTB, PB4, _LOW);

    // Get Keys status
    VFD_getKeys();
}

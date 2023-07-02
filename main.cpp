#include "ArduinoCompatible.h"
#include <cstdint>
#include <SDL_timer.h>

U8G2_SDL_128X64 u8g2;

void setup() {
    Serial.begin(9527);//没实际意义
    u8g2.begin();
    u8g2.setFont(u8g2_font_8x13_tf);

    u8g2.clearBuffer();
    u8g2.setDrawColor(1);

    u8g2.setFontPosTop();
    u8g2.drawStr(0, 0, "Hello World!");

    u8g2.sendBuffer();
}

void loop() {
    int k;

    k = u8g_sdl_get_key();
    switch (k) {
        case 'w': {
            Serial.println("w");
        }
            break;
        case 's': {
            Serial.println("s");
        }
            break;
        case 'a': {
            Serial.println("a");
        }
            break;
        case 'd': {
            Serial.println("d");

        }
            break;
        default: {
        }
    }
    SDL_Delay(20);
}
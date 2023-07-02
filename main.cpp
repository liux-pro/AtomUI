#include "Arduino.h"
#include "U8g2lib.h"



class U8G2_SDL_128X64 : public U8G2 {
public: U8G2_SDL_128X64() : U8G2() {
        u8g2_SetupBuffer_SDL_128x64_4(&u8g2, &u8g2_cb_r0);
    }
};

U8G2_SDL_128X64 u8g2;



void setup() {
    u8g2.begin();
}

void loop() {
    int k;

    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_ncenB08_tr);
    u8g2.setCursor(25, 10);

    u8g2.firstPage();
    do
    {
        u8g2.drawLine(0, 0, 128, 64);
    } while( u8g2.nextPage() );


    u8g2.sendBuffer();


    do
    {
        k = u8g_sdl_get_key();
    } while( k < 0 );


}
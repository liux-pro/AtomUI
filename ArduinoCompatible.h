#include "Arduino.h"
#include "U8g2lib.h"
#include <cstdint>
#include "iostream"

class U8G2_SDL_128X64 : public U8G2 {
public:
    U8G2_SDL_128X64() : U8G2() {
        u8g2_SetupBuffer_SDL_128x64(&u8g2, &u8g2_cb_r0);
    }
};

class Serial {
public:
    void println(const char *format, ...) {
        va_list args;
        va_start(args, format);

        char buffer[1024];
        vsnprintf(buffer, sizeof(buffer), format, args);

        va_end(args);

        std::cout << buffer << std::endl;
    }

    void begin(int rate) {
        std::cout << "set fake Serial baud rate  " << rate << std::endl;

    }
} Serial;

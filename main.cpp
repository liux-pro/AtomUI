#include "Arduino.h"
#include <SDL_timer.h>

U8G2_SDL_128X64 u8g2;


//主菜单内容
const char *menu_main[] = {
        "- 2",
        "- Number",
        "- 1",
        "- 11",
        "- 111",
        "- 1111",
        "- 11111",
        "- 111111",
        "- 1111111",
        "- 11111111",
        "- 111111111",
        "- 1111111111",
        "- 1",
        "- 1111111111",
        "- 1",
        "- 1111111111",
        "- 1",
        "- 1111111111",
        "- 1",
        "- 1111111111",
        "- 1",
        "- 1111111111",
        "- 1",
        "- 1111111111",
        "- 1",
};


namespace AtomUI {
    class Item {
    protected:
        uint8_t position = 0;

        static constexpr int MAX_STEP = 4;
        uint8_t current = 0;
        uint8_t step = 0;
    public:
        [[nodiscard]] uint8_t getPosition() const {
            return position;
        }

        void setPosition(uint8_t pos) {
            position = pos;
        }

        void up() {
            if (position > 0) {
                position--;
                step = MAX_STEP;
            }
        }

        void down() {
            if (position < 3) {
                position++;
                step = MAX_STEP;
            }
        }

        virtual void draw() = 0;
    };

    class Cursor : public Item {
    private:
        static constexpr int HEIGHT = 16;
        static constexpr int RADIUS = 1;
        static constexpr int OFFSET_X = 0;
        static constexpr int OFFSET_Y = 1;
        static constexpr int OFFSET_WIDTH = 5;
        static constexpr int OFFSET_HEIGHT = 14;

    public:
        void nextStep() {
            if (step > 0) {
                step--;
            } else {
                current = position;
            }
        }


        void draw() override {
            Serial.println("%d %d",position,current);

            u8g2.setDrawColor(2);
            int16_t pos;
            int16_t width;
            auto width1 = u8g2.getStrWidth(menu_main[position]);

            if (step == 0) {
                pos = 0;
                width = (int16_t) width1;

            } else {
                auto width2 = u8g2.getStrWidth(menu_main[current]);

                float percent = (float) step / MAX_STEP;
                pos = (int16_t) ((float) (current - position) * HEIGHT * percent);
                width = width1 + (int16_t) ((float) (width2 - width1) * percent);

            }
            nextStep();

            u8g2.drawRBox(OFFSET_X, position * HEIGHT + pos + OFFSET_Y,
                          width + OFFSET_WIDTH,
                          OFFSET_HEIGHT,
                          RADIUS);
        }
    };

    class List : public Item {
    private:
        static constexpr int HEIGHT = 16;
        static constexpr int OFFSET_X = 0;
        static constexpr int OFFSET_Y = 12;

    public:
        void draw() override {
            for (int i = 0; i < 4; ++i) {
                drawItem(i);
            }
        }

    private:
        static void drawItem(uint8_t pos) {
            u8g2.setDrawColor(1);
            u8g2.drawStr(OFFSET_X, pos * HEIGHT + OFFSET_Y, menu_main[pos]);
        }
    };
}


void setup() {
    Serial.begin(115200);//没实际意义
    u8g2.begin();
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);
    u8g2.setFontPosBaseline();

    Serial.println("h %d", u8g2.getBufferTileHeight());
    Serial.println("w %d", u8g2.getBufferTileWidth());
}

AtomUI::Cursor cursor;
AtomUI::List list;

void loop() {


    u8g2.clearBuffer();
    list.draw();
    cursor.draw();

    u8g2.sendBuffer();


    int k;

    k = u8g_sdl_get_key();
    switch (k) {
        case 'w': {
            Serial.println("w");
            cursor.up();
        }
            break;
        case 's': {
            Serial.println("s");
            cursor.down();
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
    delay(16);
}
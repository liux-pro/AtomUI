#include "Arduino.h"
#include <SDL_timer.h>

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

U8G2_SDL_128X64 u8g2;


//主菜单内容
const char *menu_main[] = {
        "- 1 start",
        "- 2 bbbbbbbb",
        "- 3",
        "- 4 aaaaaaaa",
        "- 5",
        "- 6",
        "- 7 cccccccc",
        "- 8",
        "- 9",
        "- 10 end",
};


namespace AtomUI {
    class Item {
    protected:
        static constexpr int HEIGHT = 16;
        static constexpr int SPEED = 3;
        uint8_t position = 0;
        uint8_t current = 0;
        int8_t speed = 0;
        uint8_t slotSizeScreen = 0;  //屏幕最多显示多少个
        uint8_t slotSize = 0;

    public:
        Item() {
            slotSizeScreen = u8g2.getHeight() / HEIGHT;
        }

        [[nodiscard]] uint8_t getPosition() const {
            return position;
        }

        void setPosition(uint8_t pos) {
            position = pos;
        }

        virtual void up() {
            if (position > 0) {
                position--;
                speed = -SPEED;
            }
        }

        virtual void down() {
            if (position < slotSize) {
                position++;
                speed = SPEED;
            }
        }

        virtual void draw() = 0;
    };


    class List : public Item {
    private:
        static constexpr int OFFSET_X = 0;
        static constexpr int OFFSET_Y = 12;
        int offsetY = 0;
        bool allowScroll = false;

    public:
        List() {
            if (getListSize() > slotSizeScreen) {
                slotSize = slotSizeScreen + 1;
                allowScroll = true;
            } else {
                slotSize = getListSize();
            }
        }

        uint8_t getListSize() {
            return (ARRAY_LENGTH (menu_main));
        }

        void draw() override {
            if (allowScroll) {
                auto targetOffsetY = position * HEIGHT;
                // 匀速滚动
                if (speed == 0) {
                    offsetY = targetOffsetY;
                } else {
                    offsetY = offsetY + speed;
                    if (speed > 0) {  //向下滚动
                        if (offsetY >= targetOffsetY) {//滚多了,超过了目标位置  也可能是恰好滚到位
                            offsetY = targetOffsetY;
                            speed = 0;  // 停止滚动
                        }
                    } else if (speed < 0) {
                        if (offsetY <= targetOffsetY) {//滚多了,超过了目标位置  也可能是恰好滚到位
                            offsetY = targetOffsetY;
                            speed = 0;  // 停止滚动
                        }
                    }
                }

            }

            for (int i = 0; i < getListSize(); ++i) {
                u8g2.setDrawColor(1);
                u8g2.drawStr(OFFSET_X, i * HEIGHT + OFFSET_Y - offsetY, menu_main[i]);
            }
        }

        void down() override {
            if (allowScroll) {
                if (position < getListSize() - slotSizeScreen) {
                    position++;
                    speed = SPEED;
                }
            }
        }
    };


    class Cursor : public Item {
    private:
        static constexpr int RADIUS = 1;
        static constexpr int OFFSET_X = 0;
        static constexpr int OFFSET_Y = 1;
        static constexpr int OFFSET_WIDTH = 5;
        static constexpr int OFFSET_HEIGHT = 14;

        Item *item;
        int currentY = OFFSET_Y;

    public:
        Cursor(Item *item) {
            this->item = item;
            slotSize = MIN(ARRAY_LENGTH(menu_main), slotSizeScreen);
        };

        void up() override {
            if (position > 0) {
                position--;
                speed = -SPEED;
            } else {
                item->up();
            }
        }

        void down() override {
            if (position < slotSize - 1) {
                position++;
                speed = SPEED;
            } else {
                item->down();
            }
        }

        void draw() override {
            u8g2.setDrawColor(2);
            int16_t width;
            auto targetPosition = position * HEIGHT + OFFSET_Y;

            // 匀速滚动
            if (speed == 0) {
                currentY = targetPosition;
            } else {
                auto nextPosition = currentY + speed;// 一周滚动
                int diff = targetPosition - nextPosition;

                if (speed > 0) {  //向下滚动
                    if (diff > 0) { //滚了一小步,没到目标位置
                        currentY = nextPosition;
                    } else { //滚多了,超过了目标位置  也可能是恰好滚到位
                        currentY = targetPosition;
                        speed = 0;  // 停止滚动
                    }
                } else if (speed < 0) {
                    if (diff < 0) {
                        currentY = nextPosition;
                    } else {
                        currentY = targetPosition;
                        speed = 0;  // 停止滚动
                    }
                }
                current = (currentY - OFFSET_Y) / HEIGHT;
            }

//            // 计算宽度,宽度为前进方向最近的那个文字的宽度
//            if (step == 0) {
//                width = u8g2.getStrWidth(menu_main[position]);
//            } else {
//                float percent = (float) ((currentY - OFFSET_Y) % HEIGHT) / (float) HEIGHT;
//                if (step > 0) {
//                    auto fromWidth = u8g2.getStrWidth(menu_main[current]);
//                    auto toWidth = u8g2.getStrWidth(menu_main[current + 1]);
//                    width = fromWidth + (int16_t) ((float) (toWidth - fromWidth) * percent);
//                } else {
//                    percent = 1.0f - percent;
//                    auto fromWidth = u8g2.getStrWidth(menu_main[current + 1]);
//                    auto toWidth = u8g2.getStrWidth(menu_main[current]);
//                    width = fromWidth + (int16_t) ((float) (toWidth - fromWidth) * percent);
//                }
//            }


            u8g2.drawRBox(OFFSET_X, currentY,
//                          width + OFFSET_WIDTH,
                          60,
                          OFFSET_HEIGHT,
                          RADIUS);
        }
    };
}


void setup() {
    Serial.begin(115200);//没实际意义
    u8g2.begin();
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);
    u8g2.setFontPosBaseline();

    Serial.println("h %d", u8g2.getDisplayHeight());
    Serial.println("w %d", u8g2.getDisplayWidth());
}

AtomUI::List list;
AtomUI::Cursor cursor(&list);

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
//            list.up();
            cursor.up();
        }
            break;
        case 's': {
            Serial.println("s");
//            list.down();
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
    delay(17);
}
#include "Arduino.h"

#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define ARRAY_LENGTH(arr) (sizeof(arr) / sizeof(arr[0]))

#ifdef ARDUINO
// work on esp32

#include "SPI.h"
#include <U8g2lib.h>

#define SPI_CLK 2
#define SPI_DATA 3
#define SPI_CS 7
#define SPI_DC 6
#define SPI_RST 10

class U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI_ESP32 : public U8G2 {
public:
  U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI_ESP32(const u8g2_cb_t *rotation, uint8_t clock, uint8_t data, uint8_t cs, uint8_t dc, uint8_t reset = U8X8_PIN_NONE)
    : U8G2() {
    u8g2_Setup_ssd1306_128x64_noname_f(&u8g2, rotation, u8x8_byte_arduino_hw_spi, u8x8_gpio_and_delay_arduino);
    u8x8_SetPin_4Wire_SW_SPI(getU8x8(), clock, data, cs, dc, reset);
  }
};
U8G2_SSD1306_128X64_NONAME_F_4W_HW_SPI_ESP32 u8g2(U8G2_R0, SPI_CLK, SPI_DATA, SPI_CS, SPI_DC, SPI_RST);
#else
U8G2_SDL_128X64 u8g2;
#endif


namespace AtomUI {
    class Animation {
    protected:
        static constexpr int HEIGHT = 16;
        static constexpr int SPEED = 1;
        uint8_t position = 0;        // 相当于行数, 实际位置在 position*HEIGHT, 不受渲染位置无关
        int8_t speed = 0;            //当前移动速度 上负 下正
        uint8_t slotSizeScreen = 0;  //屏幕最多显示多少行
        int currentY = 0;            //动画渲染 Y轴绝对坐标

    public:
        Animation() {
            slotSizeScreen = u8g2.getHeight() / HEIGHT;
        }

        [[nodiscard]] uint8_t getPosition() const {
            return position;
        }

        virtual void calculateY() {
            auto targetY = position * HEIGHT;
            if (speed == 0) {  // 有速度才滚动
                currentY = targetY;
            } else {
                currentY = currentY + speed;  // 以特定速度滚动一步
                if (speed > 0) {              // 判断滚动反向 发现滚过头了后撤回来,并清除速度
                    if (currentY >= targetY) {  //滚动多了,超过目标位置
                        currentY = targetY;       //退回到实际位置
                        speed = 0;                // 停止滚动
                    }
                } else if (speed < 0) {
                    if (currentY <= targetY) {
                        currentY = targetY;
                        speed = 0;
                    }
                }
            }
        }

        virtual void up() {
            if (position > 0) {
                position--;
                speed = -SPEED;
            }
        }

        virtual void down() = 0;

        virtual void draw() = 0;
    };

    class Item : public Animation {
    public:
        virtual uint8_t getWidth(int y) = 0;

        virtual uint8_t getSize() = 0;
    };

    class List : public Item {
    public:
        static constexpr int OFFSET_X = 0;
        static constexpr int OFFSET_Y = 13;
        bool allowScroll = false;
        uint8_t listSize;
        char **list;
    public:
        List(char *list[], int size) {
            this->listSize = size;
            this->list = list;

            if (listSize > slotSizeScreen) {
                allowScroll = true;
            }
        }

        uint8_t getSize() override {
            return listSize;
        }

        void draw() override {
            if (allowScroll) {
                calculateY();
            }

            for (int i = 0; i < listSize; ++i) {  //渲染所有列表项
                u8g2.setDrawColor(1);
                u8g2.drawStr(OFFSET_X, i * HEIGHT + OFFSET_Y - currentY, list[i]);
            }
        }

        /**
               * 根据屏幕坐标,获取插值后的宽度
               * @param screenY 屏幕Y轴坐标
               * @return 宽度
               */
        uint8_t getWidth(int screenY) override {
            screenY = screenY + currentY;

            int index = screenY / HEIGHT;  // 绝对坐标转行数

            int remainder = screenY % HEIGHT;
            if (remainder == 0) {  // 如果余数为0,恰好落在了某行上,直接返回当前行的宽度
                return u8g2.getStrWidth(list[index]);
            } else {                                                                  // 落在两行之间
                auto fixedPoint = 1024;                                                 // 避免浮点运算
                int percent = remainder * fixedPoint / HEIGHT;                          // 计算当前位置在两行之间的百分比
                auto fromWidth = u8g2.getStrWidth(list[index]);                         // 获取当前行的宽度
                auto toWidth = u8g2.getStrWidth(list[index + 1]);                       // 获取下一行的宽度
                auto width = fromWidth + (toWidth - fromWidth) * percent / fixedPoint;  // 根据百分比进行插值计算得到宽度
                return width;                                                           // 返回计算得到的宽度
            }
        }

        void down() override {
            if (allowScroll) {
                if (position < listSize - slotSizeScreen) {
                    position++;
                    speed = SPEED;
                }
            }
        }
    };


    class Cursor : public Animation {
    private:
        static constexpr int RADIUS = 1;    // 圆角矩形半径
        static constexpr int OFFSET_X = 0;  // 渲染位置偏移量
        static constexpr int OFFSET_Y = 1;
        static constexpr int OFFSET_WIDTH = 5;    // 游标框向右扩展像素数
        static constexpr int OFFSET_HEIGHT = 14;  // 游标框总高度
        int slotSize = 0;                         // 实际渲染行数  游标可能出现的位置数

        Item *item;

    public:
        explicit Cursor(Item *item) {
            this->item = item;
            slotSize = MIN(this->item->getSize(), slotSizeScreen);
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
            calculateY();
            u8g2.drawRBox(OFFSET_X, currentY + OFFSET_Y,
                          item->getWidth(currentY) + OFFSET_WIDTH,
                          OFFSET_HEIGHT,
                          RADIUS);
        }

        uint8_t getSelectedIndex() {
            return item->getPosition() + position;
        }
    };
}


void setup() {
    Serial.begin(115200);  //没实际意义
    u8g2.begin();
    u8g2.setFont(u8g2_font_wqy12_t_chinese1);
    u8g2.setFontPosBaseline();
}


//主菜单内容
const char *menu_main1[] = {
        "- 1 start",
        "- 2 bbbbbbbb",
        "- 3",
        "- 4 LEGEND",
        "- 5 ",
        "- 6",
        "- 7 cccccccc",
        "- 8",
        "- 9",
        "- 10 end",
};


AtomUI::List list(const_cast<char **>(menu_main1), ARRAY_LENGTH(menu_main1));
AtomUI::Cursor cursor(&list);

void loop() {
    delay(5);
    u8g2.clearBuffer();
    list.draw();
    cursor.draw();
    u8g2.sendBuffer();

#ifdef ARDUINO

    static uint8_t fps = 0;
    static int count = 0;
    static bool flag = true;
    fps++;
    if (fps % 32 == 0) {
        count++;
        if (flag) {
            cursor.down();
        } else {
            cursor.up();
        }
        if (count == 10) {
            count = 0;
            flag = !flag;
        }
    }

#else
    int k = 0;
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
#endif
}
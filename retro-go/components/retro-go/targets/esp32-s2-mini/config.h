#pragma once

#define RG_TARGET_NAME "ESP32-S2-MINI"

/*
 * Storage: SD card on separate SPI bus.
 * Important because your LCD has no CS pin.
 */
#define RG_STORAGE_ROOT "/sd"
#define RG_STORAGE_SDSPI_HOST SPI3_HOST
#define RG_STORAGE_SDSPI_SPEED 4000   // kHz. Try 1000 if SD is unstable.

// Audio
#define RG_AUDIO_USE_INT_DAC 0
#define RG_AUDIO_USE_EXT_DAC 0
#define RG_AUDIO_USE_BUZZER_PIN 16

// Video: ST7789 240x240, no CS, SPI mode 3.
#define RG_SCREEN_DRIVER 0
#define RG_SCREEN_HOST SPI2_HOST
#define RG_SCREEN_SPEED SPI_MASTER_FREQ_40M
#define RG_SCREEN_SPI_MODE 3

#define RG_SCREEN_WIDTH 240
#define RG_SCREEN_HEIGHT 240
#define RG_SCREEN_ROTATION 5
#define RG_SCREEN_RGB_BGR 1

#define RG_SCREEN_X_OFFSET 80
#define RG_SCREEN_Y_OFFSET 0

#define RG_SCREEN_VISIBLE_AREA {0, 0, 0, 0}
#define RG_SCREEN_SAFE_AREA {0, 0, 0, 0}

#define RG_SCREEN_INIT() \
    ILI9341_CMD(0x21); /* INVON: many IPS ST7789 panels need this */ \
    ILI9341_CMD(0xB2, 0x0C, 0x0C, 0x00, 0x33, 0x33); \
    ILI9341_CMD(0xB7, 0x35); \
    ILI9341_CMD(0xBB, 0x19); \
    ILI9341_CMD(0xC0, 0x2C); \
    ILI9341_CMD(0xC2, 0x01); \
    ILI9341_CMD(0xC3, 0x12); \
    ILI9341_CMD(0xC4, 0x20); \
    ILI9341_CMD(0xC6, 0x0F); \
    ILI9341_CMD(0xD0, 0xA4, 0xA1); \
    ILI9341_CMD(0xE0, 0xD0, 0x04, 0x0D, 0x11, 0x13, 0x2B, 0x3F, 0x54, 0x4C, 0x18, 0x0D, 0x0B, 0x1F, 0x23); \
    ILI9341_CMD(0xE1, 0xD0, 0x04, 0x0C, 0x11, 0x13, 0x2C, 0x3F, 0x44, 0x51, 0x2F, 0x1F, 0x1F, 0x20, 0x23)

// LCD pins
#define RG_GPIO_LCD_MISO GPIO_NUM_NC
#define RG_GPIO_LCD_MOSI GPIO_NUM_35
#define RG_GPIO_LCD_CLK  GPIO_NUM_36
#define RG_GPIO_LCD_CS   GPIO_NUM_NC
#define RG_GPIO_LCD_DC   GPIO_NUM_34
#define RG_GPIO_LCD_RST  GPIO_NUM_33

/*
 * Backlight:
 * - If BLK is connected to 3V3, leave this disabled.
 * - If BLK is connected to a GPIO, uncomment and set your GPIO.
 */
// #define RG_SCREEN_BACKLIGHT 1
// #define RG_GPIO_LCD_BCKL GPIO_NUM_38

// SD card pins
#define RG_GPIO_SDSPI_MISO GPIO_NUM_13
#define RG_GPIO_SDSPI_MOSI GPIO_NUM_11
#define RG_GPIO_SDSPI_CLK  GPIO_NUM_12
#define RG_GPIO_SDSPI_CS   GPIO_NUM_10

/*
 * Buttons.
 * These are only placeholders.
 * Retro-Go is almost unusable without buttons, so later you should map:
 * UP/DOWN/LEFT/RIGHT/A/B/START/SELECT/MENU.
 *
 * Avoid GPIO0 for buttons unless you understand boot strapping.
 */
#define RG_GAMEPAD_ADC_MAP {}

#define RG_GAMEPAD_GPIO_MAP { \
    {RG_KEY_UP,     .num = GPIO_NUM_1,  .pullup = 1, .level = 0}, \
    {RG_KEY_DOWN,   .num = GPIO_NUM_2,  .pullup = 1, .level = 0}, \
    {RG_KEY_LEFT,   .num = GPIO_NUM_3,  .pullup = 1, .level = 0}, \
    {RG_KEY_RIGHT,  .num = GPIO_NUM_4,  .pullup = 1, .level = 0}, \
    {RG_KEY_A,      .num = GPIO_NUM_5,  .pullup = 1, .level = 0}, \
    {RG_KEY_B,      .num = GPIO_NUM_6,  .pullup = 1, .level = 0}, \
    {RG_KEY_X,      .num = GPIO_NUM_7,  .pullup = 1, .level = 0}, \
    {RG_KEY_Y,      .num = GPIO_NUM_8,  .pullup = 1, .level = 0}, \
    {RG_KEY_START,  .num = GPIO_NUM_9,  .pullup = 1, .level = 0}, \
    {RG_KEY_SELECT, .num = GPIO_NUM_14, .pullup = 1, .level = 0}, \
    {RG_KEY_MENU,   .num = GPIO_NUM_15, .pullup = 1, .level = 0}, \
}
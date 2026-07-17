# Retro-Go on ESP32-S2 Mini

This guide explains how to build and flash **Retro-Go** for an **ESP32-S2 Mini** board with:

- ESP32-S2 Mini
  - 2 MB PSRAM
  - 4 MB Flash
- 1.3" IPS LCD, 240x240, ST7789-compatible
  - Pins: `GND`, `VCC`, `SCL`, `SDA`, `RES`, `DC`, `BLK`
  - No `CS` pin
  - Requires `SPI_MODE3`
- MicroSD card module
  - Pins: `CS`, `MOSI`, `CLK`, `MISO`

> There is no official ready-made Retro-Go target for esp32-s2 mini. In this guide we create our own.
> Throughout the guide please ensure all paths to your folder do not contain spaces or cyrillic letters

---
## Hardware Pinout

### Display

| Display Pin | ESP32-S2 Mini Pin |
|---|---:|
| `SCL` / `CLK` | GPIO36 |
| `SDA` / `MOSI` | GPIO35 |
| `RES` / `RST` | GPIO33 |
| `DC` | GPIO34 |
| `BLK` | 3V3 |
| `VCC` | 3V3 |
| `GND` | GND |

The display does not have a `CS` pin. Because of that, it should be placed on its own SPI bus.

### MicroSD Card

| SD Module Pin | ESP32-S2 Mini Pin |
|---|---:|
| `CS` | GPIO10 |
| `MOSI` | GPIO11 |
| `CLK` / `SCK` | GPIO12 |
| `MISO` | GPIO13 |
| `VCC` | 3V3 |
| `GND` | GND |

### Buttons

```text
GPIO pin -> button -> GND
```

| Button | ESP32-S2 Mini Pin |
|---|---:|
| D-Pad Up | GPIO1 |
| D-Pad Down | GPIO2 |
| D-Pad Left | GPIO3 |
| D-Pad Right | GPIO4 |
| A | GPIO5 |
| B | GPIO6 |
| Start | GPIO9 |
| Menu | GPIO15 |

### Speaker/Buzzer

For quick simple sound:

| Speaker Pin | ESP32-S2 Mini Pin |
|---|---:|
| `Positive` | GPIO16 |
| `Negative` | GND |




## 1. Download Git

Go to the official Git website:

```text
https://git-scm.com/downloads
```

Choose your platform. In the video, we used **Windows**.

Download the Windows installer, run the downloaded file, and install Git using the default settings.

After installation, you should have **Git Bash** available in the Windows context menu.





## 2. Download Retro-Go

Now download the Retro-Go project.

Open any convenient folder where you want to store the project.

Right-click inside the folder and choose:

```text
Git Bash Here
```

Then paste this command:

```bash
git clone -b master https://github.com/ducalex/retro-go.git
```

Press **Enter** and wait until the repository is downloaded.

After that, you should have a new folder:

```text
retro-go
```
This is the project we will modify and build.




## 3. Install ESP32 Toolchain

Now we need to install the ESP-IDF toolchain from Espressif.

For this project, use the **offline installer for ESP-IDF v4.4.6**.

ESP-IDF v4.4.6 is an older release, but it is stable for many Retro-Go builds and is known to work well with this kind of projects.

Direct Download Link from GitHub:
```text
https://github.com/espressif/idf-installer/releases/download/offline-4.4.6/esp-idf-tools-setup-offline-4.4.6.exe
```




Download the installer, run it, accept the installation steps, and wait.

After installation, you should see a folder in the root of drive `C:`:

```text
C:\Espressif
```

Open this:

```text
C:\Espressif\frameworks\
```

Inside, you should see your installed ESP-IDF version, for example:

```text
C:\Espressif\frameworks\esp-idf-v4.4.6
```

Now open the Windows Start menu and run:

```text
cmd
```

Go to just opened ESP-IDF folder. You can do this by typing `cd`, then dragging the folder into the command prompt window.

Example:

```bat
cd C:\Espressif\frameworks\esp-idf-v4.4.6
```

Press **Enter**.

Now install tools for ESP32-S2:

```bat
start install.bat esp32s2
```

Wait until installation finishes.

Then run:

```bat
start export.bat
```

A new command window will open.

This new window is the prepared ESP-IDF command line environment. It contains all paths and tools required to build ESP projects.

Without this prepared environment, `idf.py`, compilers, and build tools may not work correctly.

In the future, you can open this environment from the Start menu by choosing:

```text
ESP-IDF 4.4 CMD
```

or:

```text
ESP-IDF 4.4 PowerShell
```

Now go to the Retro-Go project folder from this ESP-IDF command window:

```bat
cd path\to\your\retro-go
```

For example:

```bat
cd E:\MyProjects\retro-go
```




## 4. Prepare Project Files

### You can skip this step if you just copy content from folder `retro-go` in this guide repository and paste it to the root folder of your local retro-go repository downloaded in the 2nd step. And with this shortcut you can move to step 5.

#### But for those who wants to do it themselves:

We need to create a custom target for the ESP32-S2 Mini.

In the Retro-Go folder, go to:

```text
components/retro-go/targets
```

Copy the existing folder:

```text
esp32-s3-devkit
```

Rename the copy to:

```text
esp32-s2-mini
```

The path should now be:

```text
components/retro-go/targets/esp32-s2-mini
```

This is needed because ESP32-S2 Mini is not supported by Retro-Go by default.

Go back one folder to:

```text
components/retro-go
```

Open:

```text
config.h
```

Find the target selection block.

After the existing `redroid-go` block, or near the other target blocks, add:

```c
#elif defined(RG_TARGET_ESP32_S2_MINI)
#include "targets/esp32-s2-mini/config.h"
```

This connects our new ESP32-S2 Mini config to the whole Retro-Go project.

Our display has no `CS` pin and works correctly with `SPI_MODE3`.

Open:

```text
components/retro-go/drivers/display/ili9341.h
```

Find this line inside the SPI device configuration:

```c
.mode = 0,
```

Replace it with:

```c
#ifdef RG_SCREEN_SPI_MODE
        .mode = RG_SCREEN_SPI_MODE,
#else
        .mode = 0,
#endif
```

This allows us to set the SPI mode from our target config.

If you want to hardcode it quickly, you can set:

```c
.mode = 3,
```

But using `RG_SCREEN_SPI_MODE` is cleaner.

Now go to:

```text
retro-go/tools
```

Open:

```text
mkfw.py
```

Retro-Go may build the app correctly, but fail when creating the final firmware image, because `esp32s2` is not listed as a valid image type.

Find this block:

```python
bootloader_offset, table_offset, prog_offset = {
    "esp32": (0x1000, 0x8000, 0x10000),
    "esp32s3": (0x0000, 0x8000, 0x10000),
    "esp32p4": (0x2000, 0x8000, 0x10000),
}.get(chip_type)
```

Add `esp32s2` next to `esp32s3`:

```python
bootloader_offset, table_offset, prog_offset = {
    "esp32": (0x1000, 0x8000, 0x10000),
    "esp32s2": (0x1000, 0x8000, 0x10000),
    "esp32s3": (0x0000, 0x8000, 0x10000),
    "esp32p4": (0x2000, 0x8000, 0x10000),
}.get(chip_type)
```

Then find this line:

```python
parser.add_argument("--type", choices=["odroid", "esplay", "esp32", "esp32s3", "esp32p4"], default="odroid")
```

Add `esp32s2` to the list:

```python
parser.add_argument("--type", choices=["odroid", "esplay", "esp32", "esp32s2", "esp32s3", "esp32p4"], default="odroid")
```

Go back to the folder we created:

```text
components/retro-go/targets/esp32-s2-mini
```

Open:

```text
env.py
```

Replace its content with:

```python
IDF_TARGET = "esp32s2"
FW_FORMAT = "none"
DEFAULT_APPS = "launcher retro-core"
```

Do not set `IDF_TARGET` to `esp32`. The ESP32-S2 must be built as `esp32s2`.

Open:

```text
components/retro-go/targets/esp32-s2-mini/config.h
```

Replace the file with this config:

```c
#pragma once

#define RG_TARGET_NAME "ESP32-S2-MINI"

/*
 * Storage: SD card on a separate SPI bus.
 */
#define RG_STORAGE_ROOT "/sd"
#define RG_STORAGE_SDSPI_HOST SPI3_HOST
#define RG_STORAGE_SDSPI_SPEED 4000   // kHz. Try 1000 if SD is unstable.

// Video: ST7789 240x240, no CS, SPI mode 3.
#define RG_SCREEN_DRIVER 0
#define RG_SCREEN_HOST SPI2_HOST
#define RG_SCREEN_SPEED SPI_MASTER_FREQ_40M
#define RG_SCREEN_SPI_MODE 3

#define RG_SCREEN_WIDTH 240
#define RG_SCREEN_HEIGHT 240
#define RG_SCREEN_ROTATION 0
#define RG_SCREEN_RGB_BGR 1

#define RG_SCREEN_VISIBLE_AREA {0, 0, 0, 0}
#define RG_SCREEN_SAFE_AREA {0, 0, 0, 0}

/*
 * ST7789 init.
 */
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

// SD card pins
#define RG_GPIO_SDSPI_MISO GPIO_NUM_13
#define RG_GPIO_SDSPI_MOSI GPIO_NUM_11
#define RG_GPIO_SDSPI_CLK  GPIO_NUM_12
#define RG_GPIO_SDSPI_CS   GPIO_NUM_10

/*
 * Buttons.
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

/*
 * Optional quick audio
 */
#define RG_AUDIO_USE_INT_DAC 0
#define RG_AUDIO_USE_EXT_DAC 0
#define RG_AUDIO_USE_BUZZER_PIN 16
```

On many 240x240 ST7789 displays, the visible 240x240 area is internally placed inside a larger display memory area, often 240x320. Because of that, after rotating the screen, the image can be shifted to the left, right, up, or down.

If rotation `5` works but the image origin is shifted, add X/Y offset support to the Retro-Go display driver.

Open:

```text
components/retro-go/drivers/display/ili9341.h
```

Find this function:

```c
static void lcd_set_window(int left, int top, int width, int height)
{
    int right = left + width - 1;
    int bottom = top + height - 1;

    if (left < 0 || top < 0 || right >= display.screen.real_width || bottom >= display.screen.real_height)
        RG_LOGW("Bad lcd window (x0=%d, y0=%d, x1=%d, y1=%d)\n", left, top, right, bottom);

    ILI9341_CMD(0x2A, left >> 8, left & 0xff, right >> 8, right & 0xff);
    ILI9341_CMD(0x2B, top >> 8, top & 0xff, bottom >> 8, bottom & 0xff);
    ILI9341_CMD(0x2C);
}
```

Replace it with this version:

```c
#ifndef RG_SCREEN_X_OFFSET
#define RG_SCREEN_X_OFFSET 0
#endif

#ifndef RG_SCREEN_Y_OFFSET
#define RG_SCREEN_Y_OFFSET 0
#endif

static void lcd_set_window(int left, int top, int width, int height)
{
    int right = left + width - 1;
    int bottom = top + height - 1;

    if (left < 0 || top < 0 || right >= display.screen.real_width || bottom >= display.screen.real_height)
        RG_LOGW("Bad lcd window (x0=%d, y0=%d, x1=%d, y1=%d)\n", left, top, right, bottom);

    left += RG_SCREEN_X_OFFSET;
    right += RG_SCREEN_X_OFFSET;
    top += RG_SCREEN_Y_OFFSET;
    bottom += RG_SCREEN_Y_OFFSET;

    ILI9341_CMD(0x2A, left >> 8, left & 0xff, right >> 8, right & 0xff);
    ILI9341_CMD(0x2B, top >> 8, top & 0xff, bottom >> 8, bottom & 0xff);
    ILI9341_CMD(0x2C);
}
```

Then open your target config:

```text
components/retro-go/targets/esp32-s2-mini/config.h
```

For this display, with `RG_SCREEN_ROTATION 5`, try this first:

```c
#define RG_SCREEN_X_OFFSET 80
#define RG_SCREEN_Y_OFFSET 0
```



## 5. Configure the Project
Now go back to the ESP-IDF command prompt.

Make sure you are in the root folder of the Retro-Go project:

```bat
cd path\to\your\retro-go
```

First run clean:

```bat
python rg_tool.py --target=esp32-s2-mini clean
```

Then build the launcher:

```bat
python rg_tool.py --target=esp32-s2-mini build launcher
```

Wait until the launcher builds.

Now go to the launcher folder:

```bat
cd launcher
```

Open the ESP-IDF configuration menu:

```bat
idf.py menuconfig
```

You will see the configuration menu for this target.

#### Serial Flasher Config

Go to:

```text
Serial flasher config
```

Set:

```text
Flash size: 4 MB
Flash SPI speed: 80 MHz
Flash SPI mode: DIO
```

If 80 MHz is unstable on your board, use 40 MHz.

---

#### ESP32S2-Specific Settings

Go to:

```text
Component config -> ESP32S2-specific
```

Check:

```text
CPU frequency: 240 MHz
Support for external RAM: Enabled
```

Then go inside:

```text
SPI RAM config
```

Enable:

```text
Initialize SPI RAM during startup
```

---

#### FAT Filesystem Support

Go to:

```text
Component config -> FAT Filesystem support
```

Check:

```text
Long filename support: Long filename buffer in heap
Max long filename length: 255
API character encoding: UTF-8
```

---

#### FreeRTOS

Go to:

```text
Component config -> FreeRTOS
```

Check:

```text
Idle Task stack size: 8192
```

If you also see `Main task stack size`, set it to at least:

```text
8192
```

---

#### Save the Configuration

Press:

```text
S
```

Then press:

```text
Enter
```

After saving, press:

```text
Esc
```

until you return to the command prompt.

Now copy the generated `sdkconfig` from the `launcher` folder to your custom target folder.

If you are still inside the `launcher` folder, run:

```bat
copy sdkconfig ..\components\retro-go\targets\esp32-s2-mini\sdkconfig
```

Or from the Retro-Go root folder:

```bat
copy launcher\sdkconfig components\retro-go\targets\esp32-s2-mini\sdkconfig
```

---





## 6. Build and Flash
Now return to the root folder of Retro-Go:

```bat
cd ..
```

If you are already in the root folder, you do not need this command.

Connect your ESP32-S2 Mini to the computer.

Find the COM port of your board.

One simple way is to open Arduino IDE and check which port appears when you connect the board.

In this example, the board is on:

```text
COM5
```

Now build and flash Retro-Go:

```bat
python rg_tool.py --target=esp32-s2-mini --port=COM5 install launcher retro-core
```

Here:

- `--target=esp32-s2-mini` means we are using our custom target.
- `--port=COM5` is the port where the ESP32-S2 Mini is connected.
- `launcher` is the Retro-Go menu.
- `retro-core` contains several emulators in one app.
- `prboom` or `prboom-go` if you want Doom to run on your ESP32-S2 Mini

Press **Enter** and wait until the project compiles and uploads to the device.





## 7. SD Card Structure Games
Format the SD card as FAT32.

NES ROM files should be placed here:

```text
/roms/nes/
```

Example:

```text
SD_CARD_ROOT/
└── roms/
    └── nes/
        ├── game1.nes
        ├── game2.nes
        └── game3.nes
```


## 8. Thank you! Have Fun!

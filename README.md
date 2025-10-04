# ESP32 Cheap Yellow Display (CYD) Domino Clock

This is the accompanying repository for my article "Create a digital clock in a Domino style on an ESP32 Cheap Yellow Display ("CYD")" available here: 

For short - what is a "Cheap Yellow Display" ? This device was introduced some years ago and allowed for very fast development of projects where an ESP32, a TFT (optional Touch surface), an SD Card Reader and an RGB LED is included. The first version was equipped with a 2.8 inch large TFT display with **ILI9341** driver chip and **XPT2046** resistive Touch driver chip. Newer versions are sold with a **ST7789** display driver chip. Nowadays, the device is available with different display sizes (1.28 up to 7 inches) and driver chips, but I'm focusing on the 2.8 inch variants. The display has a size of **320 x 240** pixels in Landscape orientation. Most of the devices are driven by an ESP32 WROOM microcontroller, but I saw some others with an ESP32-S3 chip.

![Image 1](./images/esp32_cyd_brick_clock_v02_600w.png)

## Source of the Domino images

I'm using the Domino style images provided by **judge2005** in the GitHub repository **https://github.com/judge2005/EleksTubeIPS**. You will find the originals in the path "more_faces | dom2". The original images are BMP encoded images with a size of 135 x 240 pixels, but this is far to much for our display, as I want to display the current time and date with 6 digits each. The second issue would be: how do I load the images into the sketch (SD-Card, LittleFS). I decided to use the "inline" style by converting the files in C-style header files.

For both the image size reducing and converting I used an internet service: https://mischianti.org/rgb-image-to-byte-array-converter-for-arduino-tft-displays/

Simply open each of the ten digit BMP files, set the new image size (here 50 x 88 pixels), convert the file and download it as *h file.

The are the default settings I used:

````plaintext
Settings (default):
Code format Hex 0x00
Palette mod 16bit RRRRRGGGGGGBBBBB (2byte/pixel)
Resize: use both parameters to match exact size, e.g. 50 x 88
Multi line yes
Endianness Little Endian
static yes
const yes
Data type uint16_t
PROGMEM yes
````

## Required Library
````plaintext
TFT_eSPI Version: 2.4.3 *1) (https://github.com/Bodmer/TFT_eSPI)

*1) In case you encounter any problems with the TFT_eSPI library you should consider to use my forked TFT_eSPI library that solved some problems, see link below
````
Forked TFT_eSPI library by AndroidCrypto: https://github.com/AndroidCrypto/TFT_eSPI

## Set up the TFT_eSPI library

Please don't forget to copy the files "*Setup801_ESP32_CYD_ILI9341_240x320.h*" and "*Setup805_ESP32_CYD_ST7789_240x320.h*" in the "User_Setups" folder of the TFT_eSPI library and edit the "*User_Setup_Select.h*" to include the set up, depending on your CYD board type (ILI9341 or ST7789).

If you own an older version (one Micro-USB port) your display is driven by an **ILI9341** driver chip, newer boards (one Micro-USB and one USB-C port) are using an **ST7789** driver chip. Both of them should use a resistive touch controller of type **XPT2046**.

**ILI9341**: Setup801_ESP32_CYD_ILI9341_240x320.h

**ST7789**: Setup805_ESP32_CYD_ST7789_240x320.

## Development Environment
````plaintext
Arduino IDE Version 2.3.6 (Windows)
arduino-esp32 boards Version 3.2.0 (https://github.com/espressif/arduino-esp32)
````

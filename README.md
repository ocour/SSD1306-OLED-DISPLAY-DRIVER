# SSD1306-OLED-DISPLAY-DRIVER
SSD1306 OLED DISPLAY DRIVER 128x32

Uses I2C.

For STM32 MCU.
Uses STM32 HAL for I2C transmitting.
I2C event interrupts need to be enabled.

Works only for 128x32 display, i didn't bother to make this work for 128x64 display as i do not own one.
Its possible to make it work for one, some values need to be changed however.

With this driver its possible to add bitmaps and strings to the display.
I have only tested one font, (6x8), but should be possible to use custom fonts as well.

Example project: https://github.com/ocour/SSD1306_Example/tree/master.
![SSD1306](https://user-images.githubusercontent.com/112856256/219875575-73381443-6cce-406e-a0ac-b424743129c3.jpg)

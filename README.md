# SSD1306-OLED-DISPLAY-DRIVER
SSD1306 OLED DISPLAY DRIVER 128x32

Uses I2C.

Should work for any MCU/MPU.

User needs to implement a callback function for data trasmitting.

Example callback:

```C
static void i2c_trasmit_cb(uint16_t dev_address, uint8_t start_byte, uint8_t* data, uint16_t size_of_data)
{
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();

    // Queue "START signal" 
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (dev_address << 1) | I2C_MASTER_WRITE, true);

    // Queue start byte to indicate command or data
    i2c_master_write(cmd, &start_byte, 1, true);

    // Queue actual data
    i2c_master_write(cmd, data, size_of_data, true);

    // Queue "STOP signal"
    i2c_master_stop(cmd);

    // Write all queued data
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if(ret != 0) {
        ESP_LOGI(TAG, "I2C Error: %d", ret);
    }
}
```

Contains only 128x32 defines, should be configurable for 128x64 as well.

With this driver its possible to add bitmaps and strings to the display.
I have only tested one font, (6x8), but should be possible to use custom fonts as well.

Example project: https://github.com/ocour/SSD1306_Example/tree/master.
![SSD1306](https://user-images.githubusercontent.com/112856256/219875575-73381443-6cce-406e-a0ac-b424743129c3.jpg)

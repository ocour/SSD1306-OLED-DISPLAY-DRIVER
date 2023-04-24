#ifndef INC_DRIVERS_SSD1306_H_
#define INC_DRIVERS_SSD1306_H_

#include "font.h"

#include <stdint.h>
#include <assert.h>
#include <string.h>

/* SSD1306 128x32 */
#define SSD1306_128_32_ADDR             0x3C
#define SSD1306_128_32_HEIGHT 	        32
#define SSD1306_128_32_WIDTH	        128
#define SSD1306_128_32_BUFFERSIZE       ((SSD1306_128_32_HEIGHT * SSD1306_128_32_WIDTH) / 8)
#define SSD1306_128_32_DATABUFFERSIZE   (SSD1306_128_32_BUFFERSIZE + 1)

/* SSD1306 128x64 */

/* PAGE 28 OF DATASHEET */
/* FUNDAMENTAL COMMAND TABLE */
#define CMD_SET_CONTRAST 					0x81
#define CMD_ENTIRE_DISPLAY_ON_RESUME 		0xA4
#define CMD_ENTIRE_DISPLAY_ON_IGNORE 		0xA5
#define CMD_DISPLAY_NORMAL 					0xA6
#define CMD_DISPLAY_INVERSE 				0xA7
#define CMD_DISPLAY_OFF 					0xAE
#define CMD_DISPLAY_ON 						0xAF

/* PAGE 29 */
/* SCROLLING COMMAND TABLE */
#define CMD_DEACTIVATE_SCROLL				0x2E
#define CMD_ACTIVATE_SCROLL					0x2F

/* PAGE 30 */
/* ADDRESSING SETTING COMMAND TABLE */
#define CMD_SET_COLUMN_LWR					0x00	/* 0x00~0F */
#define CMD_SET_COLUMN_HGR					0x10	/* 0x10~1F */
#define CMD_SET_MEMORY_ADR_MODE 			0x20
#define CMD_SET_COLUMN_ADR					0x21
#define CMD_SET_PAGE_ADR					0x22
#define CMD_SET_PAGE_START_ADR				0xB0	/* 0xB0~B7 */

/* PAGE 31 */
/* HARDWARE CONFIGURATION COMMAND TABLE */
#define CMD_SET_DISPLAY_START_LINE			0x40	/* 0x40~7F */
#define CMD_SET_SEGMENT_RMAP_0				0xA0
#define CMD_SET_SEGMENT_RMAP_127			0xA1
#define CMD_SET_MULTIPLEX_RATIO				0xA8
#define CMD_SET_COM_SCAN_DIR				0xC0	/* 0xC0/C8 */
#define CMD_SET_DISPLAY_OFFSET				0xD3
#define CMD_SET_COM_PINS					0xDA

/* PAGE 32 */
/* TIMING & DRIVING SCHEME SETTING COMMAND TABLE */
#define CMD_SET_CLOCK_DIVIDE_RATIO_AND_F	0xD5
#define CMD_SET_PREC_PERIOD					0xD9
#define CMD_SET_COMH_DESELECT_LVL			0xDB
#define CMD_NOP								0xE3

/* PAGE 62 */
/* CHARGE PUMP COMMAND TABLE */
#define CMD_CHARGE_PUMP_SETTING				0x8D

/* PAGE 30 */
#define MM_ADR_MODE_HORIZONTAL				0x0
#define MM_ADR_MODE_VERTICAL				0x1
#define MM_ADR_MODE_PAGE					0x2

/* CONTINUATION BIT, PAGE 20 */
#define WRITE_COMMAND						0x00
#define WRITE_READ_DATA						0x40

/* Colors */
#define SSD1306_COLOR_BLACK					0
#define SSD1306_COLOR_WHITE					1

// void (*transmit_cb)(uint16_t dev_address, uint8_t start_byte, uint8_t* data, uint16_t size_of_data);

typedef struct {
    /* Device address */
	uint16_t dev_address;

    /* 
        Buffers size, use predefined sizes.
        Example: SSD1306_128_32_BUFFERSIZE
    */
    uint16_t buffer_size;

    /* 
        Pointer to display buffer,
        User has to create a global array and assign this pointer to point to it
    */
	uint8_t *buffer;

    /* 
        User specified callback function that transmits given data
    */
    void (*transmit_cb)(uint16_t dev_address, uint8_t start_byte, uint8_t* data, uint16_t size_of_data);

    /* 
        height and width of display,
    */
	uint8_t height;
	uint8_t width;

    /* 
        Displays inverted state,
        Optional initialization
    */
	uint8_t inverted:1;

    /* 
        Used to keep track of letter placement,
        Optional initialization
    */
	uint16_t x_cursor;
	uint16_t y_cursor;

} SSD1306_HandleTypeDef;

/*
 * Writes command to SSD1306
 * @param hssd: SSD1306 handle
 * @param cmd: command to be transmitted
 */
void SSD1306_WriteCommand(SSD1306_HandleTypeDef* hssd, uint8_t cmd);

/*
 * 	Writes Buffers content to Display,
 * 	Needs to be called so that Display's content is updated
 */
void SSD1306_Display(SSD1306_HandleTypeDef* hssd);

/*
 *	Places pixel into x and y coordinates,
 *	 SSD1306_Display function needs to be called to write buffer to OLED display
 */
void SSD1306_PutPixel(SSD1306_HandleTypeDef* hssd, uint16_t x, uint16_t y, uint8_t color);

/*
 * 	Draws a bitmap at x and y coordinates
 * 	width and height are bitmaps width and height
 * 	Credit to Adafruit's GFX library: https://github.com/adafruit/Adafruit-GFX-Library
 */
void SSD1306_DrawBitmap(
		SSD1306_HandleTypeDef* hssd, uint16_t x, uint16_t y,
		uint8_t* bitmap, uint16_t width, uint16_t height, uint8_t color);

/*
 * Puts a character on the screen at x and y coordinates
 * Updates x and y cursor,
 * To automatically calculate next character placement, give hssd->x_cursor and y_cursor as x and y coordinates
 */
void SSD1306_DrawChar(SSD1306_HandleTypeDef* hssd, SSD1306_FontTypeDef* font,
     char ch, uint16_t x, uint16_t y, uint8_t color);

/*
 * 	Draws string,
 * 	Stops at terminating null character '\0'
 */
void SSD1306_DrawString(SSD1306_HandleTypeDef* hssd, SSD1306_FontTypeDef* font, 
    char* s, uint16_t x, uint16_t y, uint8_t color);

/*
 *	Inverts Display, not buffer
 */
void SSD1306_InvertDisplay(SSD1306_HandleTypeDef* hssd);

/*
 * Turns display OFF
 */
void SSD1306_DisplayOFF(SSD1306_HandleTypeDef* hssd);

/*
 * Turns display ON
 */
void SSD1306_DisplayON(SSD1306_HandleTypeDef* hssd);

/*
 * 	Fills buffer, NOT DISPLAY
 */
void SSD1306_FillDisplay(SSD1306_HandleTypeDef* hssd);

/*
 *	Clears SSD1306 buffer
 *	@param hssd: SSD1306 handle
 *	Call to SSD1306_Display needed to write content to display
 */
void SSD1306_ClearDisplay(SSD1306_HandleTypeDef* hssd);

/*
    Initializes display and writes initial commands to prepare it.
    Checks that handle struct is configured correctly
    @Param hssd: SSD1306 handle
*/
void SSD1306_Init(SSD1306_HandleTypeDef* hssd);

/* 
    Prepares display by writing set of commands to it.
    Called by SSD1306_Init()
*/
void SSD1306_InitCommands(SSD1306_HandleTypeDef* hssd);

#endif /* INC_DRIVERS_SSD1306_H_ */
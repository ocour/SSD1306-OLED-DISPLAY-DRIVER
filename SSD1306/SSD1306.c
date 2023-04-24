#include "./include/SSD1306.h"

static void assert_handle(SSD1306_HandleTypeDef* hssd)
{
    assert(hssd);
	assert(hssd->height);
	assert(hssd->width);
    assert(hssd->buffer_size); 
}

void SSD1306_WriteCommand(SSD1306_HandleTypeDef* hssd, uint8_t cmd)
{
    assert_handle(hssd);

    /* Call the user specified transmit callback function */
	hssd->transmit_cb(hssd->dev_address, WRITE_COMMAND, &cmd, sizeof(cmd));
}

void SSD1306_PutPixel(SSD1306_HandleTypeDef* hssd, uint16_t x, uint16_t y, uint8_t color)
{
	assert_handle(hssd);

	/* Outside buffer dimensions */
	if((x > hssd->width) || (y > hssd->height))
		return;

	if(color == SSD1306_COLOR_BLACK) {
		hssd->buffer[x + (y / 8) * hssd->width] |= (1 << (y & 7));
	}
	else if(color == SSD1306_COLOR_WHITE) {
		hssd->buffer[x + (y / 8) * hssd->width] &= ~(1 << (y & 7));
	}
}

void SSD1306_DrawBitmap(
		SSD1306_HandleTypeDef* hssd, uint16_t x, uint16_t y,
		uint8_t* bitmap, uint16_t width, uint16_t height, uint8_t color)
{
	int16_t byteWidth = (width + 7) / 8;
	uint8_t b = 0;

	for(uint16_t j = 0; j < height; j++, y++) {
		for(uint16_t i = 0; i < width; i++) {
			if(i & 7)
				b <<= 1;
			else
				b = bitmap[j * byteWidth + i / 8];
			if(b & 0x80)
				SSD1306_PutPixel(hssd, x + i, y, color);
		}
	}
}

void SSD1306_DrawChar(SSD1306_HandleTypeDef* hssd, SSD1306_FontTypeDef* font, char ch, uint16_t x, uint16_t y, uint8_t color)
{
	hssd->x_cursor = x;
	hssd->y_cursor = y;

	uint16_t i, b, j;

	for (i = 0; i < font->width; i++) {
		b = font->font[(ch - 32) * font->width + i];
		for (j = 0; j < font->height; j++) {
			if ((b << j) & 0x80) {
				SSD1306_PutPixel(hssd, (hssd->x_cursor + i), ((hssd->y_cursor  + font->height) - j), color);
			}
			else {
				SSD1306_PutPixel(hssd, (hssd->x_cursor + i), ((hssd->y_cursor  + font->height) - j), !color);
			}
		}
	}


	hssd->x_cursor += font->width;
}

void SSD1306_DrawString(SSD1306_HandleTypeDef* hssd, SSD1306_FontTypeDef* font, char* s, uint16_t x, uint16_t y, uint8_t color)
{
	hssd->x_cursor = x;
	hssd->y_cursor = y;

	uint16_t i = 0;
	while(s[i] != '\0') {
		SSD1306_DrawChar(hssd, font, s[i++], hssd->x_cursor, hssd->y_cursor, color);
	}
}

void SSD1306_InvertDisplay(SSD1306_HandleTypeDef* hssd)
{
	assert_handle(hssd);

	if(hssd->inverted == 0) {
		SSD1306_WriteCommand(hssd, CMD_DISPLAY_INVERSE);
		hssd->inverted = 1;
	}
	else {
		SSD1306_WriteCommand(hssd, CMD_DISPLAY_NORMAL);
		hssd->inverted = 0;
	}
}

void SSD1306_DisplayOFF(SSD1306_HandleTypeDef* hssd)
{
	assert_handle(hssd);

	SSD1306_WriteCommand(hssd, CMD_DISPLAY_OFF);
}

void SSD1306_DisplayON(SSD1306_HandleTypeDef* hssd)
{
	assert_handle(hssd);

	SSD1306_WriteCommand(hssd, CMD_DISPLAY_ON);
}

void SSD1306_FillDisplay(SSD1306_HandleTypeDef* hssd)
{
	assert_handle(hssd);

	/* Fills buffer */
	memset(hssd->buffer, 0xff, hssd->buffer_size);
}

void SSD1306_ClearDisplay(SSD1306_HandleTypeDef* hssd)
{
	assert_handle(hssd);

	/* Clears buffer */
	memset(hssd->buffer, 0, hssd->buffer_size);
}

void SSD1306_Display(SSD1306_HandleTypeDef* hssd)
{
	assert_handle(hssd);

    /* Call the user specified transmit callback function */
	hssd->transmit_cb(hssd->dev_address, WRITE_READ_DATA, hssd->buffer, hssd->buffer_size);
}

void SSD1306_Init(SSD1306_HandleTypeDef* hssd)
{
	/* Check parameters */
	assert_handle(hssd);

	/* SET BUFFER TO 0 */
	memset(hssd->buffer, 0, hssd->buffer_size);

	hssd->inverted = 0;
	hssd->x_cursor = 0;
	hssd->y_cursor = 0;

	/* Display Initialisation commands */
	SSD1306_InitCommands(hssd);
}

void SSD1306_InitCommands(SSD1306_HandleTypeDef* hssd)
{
    assert_handle(hssd);

    /* Turn Display OFF */
    SSD1306_WriteCommand(hssd, CMD_DISPLAY_OFF);

	/* Set Horizontal memory Addressing mode  */
    SSD1306_WriteCommand(hssd, CMD_SET_MEMORY_ADR_MODE);
    SSD1306_WriteCommand(hssd, MM_ADR_MODE_HORIZONTAL);

	/* Set COM Output Scan Direction */
    SSD1306_WriteCommand(hssd, (CMD_SET_COM_SCAN_DIR | 0x08));  // 0xC8

	/* Set Column Address  */
    SSD1306_WriteCommand(hssd, CMD_SET_COLUMN_ADR);
    SSD1306_WriteCommand(hssd, 0x00);
    SSD1306_WriteCommand(hssd, (hssd->width - 1));

	/* Set Page Address  */
    SSD1306_WriteCommand(hssd, CMD_SET_PAGE_ADR);
    SSD1306_WriteCommand(hssd, 0x00);
    SSD1306_WriteCommand(hssd, ((hssd->height / 8) - 1));   // 64 = 7, 32 = 3

	/* Set Display Contrast */
    SSD1306_WriteCommand(hssd, CMD_SET_CONTRAST);
    SSD1306_WriteCommand(hssd, 0x8F);

	/* Set Segment re-map */
    SSD1306_WriteCommand(hssd, CMD_SET_SEGMENT_RMAP_127);

	/* Set Multiplex ratio */
    SSD1306_WriteCommand(hssd, CMD_SET_MULTIPLEX_RATIO);
    SSD1306_WriteCommand(hssd, (hssd->height - 1));

	/* Set Display Start Line */
    SSD1306_WriteCommand(hssd, CMD_SET_DISPLAY_START_LINE);

	/* Set Display Offset */
    SSD1306_WriteCommand(hssd, CMD_SET_DISPLAY_OFFSET);
    SSD1306_WriteCommand(hssd, 0x0);

	/* Set Display's clock ratio */
    SSD1306_WriteCommand(hssd, CMD_SET_CLOCK_DIVIDE_RATIO_AND_F);
    SSD1306_WriteCommand(hssd, 0x80);

	/* Set Pre-charge period */
    SSD1306_WriteCommand(hssd, CMD_SET_PREC_PERIOD);
    SSD1306_WriteCommand(hssd, 0x22);

	/* Set Vcomh Deselect level */
    SSD1306_WriteCommand(hssd, CMD_SET_COMH_DESELECT_LVL);
    SSD1306_WriteCommand(hssd, 0x40);

	/* Set COM Pins */
    SSD1306_WriteCommand(hssd, CMD_SET_COM_PINS);
    SSD1306_WriteCommand(hssd, 0x02);

	/* Set Chargepump */
    SSD1306_WriteCommand(hssd, CMD_CHARGE_PUMP_SETTING);
    SSD1306_WriteCommand(hssd, 0x14);

	/* Set Entire Display ON, Resume RAM content  */
    SSD1306_WriteCommand(hssd, CMD_ENTIRE_DISPLAY_ON_RESUME);

	/* Set Display mode to Normal */
    SSD1306_WriteCommand(hssd, CMD_DISPLAY_NORMAL);

	/* Disable Scroll  */
    SSD1306_WriteCommand(hssd, CMD_DEACTIVATE_SCROLL);

	/* Set Display ON */
    SSD1306_WriteCommand(hssd, CMD_DISPLAY_ON);
}
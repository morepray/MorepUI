#include "oled.h"
#include "oledfont.h"
#include "main.h"
#include "spi.h"

unsigned char DisplayBuffer[128][8]; // 1024Byte = 1KB

void OLED_WR_Byte(uint8_t dat, uint8_t cmd)
{
	if (cmd)
	{
		OLED_DC_Set(); // Cmd
	}
	else
		OLED_DC_Clr(); // Data
	OLED_CS_Clr();
	HAL_SPI_Transmit(&hspi1, &dat, 1, 1000);
	OLED_CS_Set();
	OLED_DC_Set();
}

void OLED_Set_Pos(unsigned char x, unsigned char y)
{
	OLED_WR_Byte(0xb0 + y, OLED_CMD);
	OLED_WR_Byte(((x & 0xf0) >> 4) | 0x10, OLED_CMD);
	OLED_WR_Byte((x & 0x0f), OLED_CMD);
}

void OLED_Display_On(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);
	OLED_WR_Byte(0X14, OLED_CMD);
	OLED_WR_Byte(0XAF, OLED_CMD);
}

void OLED_Display_Off(void)
{
	OLED_WR_Byte(0X8D, OLED_CMD);
	OLED_WR_Byte(0X10, OLED_CMD);
	OLED_WR_Byte(0XAE, OLED_CMD);
}

void OLED_Clear(void)
{
	uint8_t i, n;
	for (i = 0; i < 8; i++)
	{
		OLED_WR_Byte(0xb0 + i, OLED_CMD);
		OLED_WR_Byte(0x02, OLED_CMD);
		OLED_WR_Byte(0x10, OLED_CMD);
		for (n = 0; n < 128; n++)
			OLED_WR_Byte(0, OLED_DATA);
	}
}

/**
 * @brief 显示一个字符
 * @param x
 * @param y
 * @param chr
 * @param size 16 or 12
 */
void OLED_ShowChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size)
{
	unsigned char c = 0, i = 0;
	c = chr - ' ';
	if (x > Max_Column - size)
	{
		x = 0;
		y = y + 1;
	}
	if (size == 16)
	{
		OLED_Set_Pos(x, y);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i], OLED_DATA);
		OLED_Set_Pos(x, y + 1);
		for (i = 0; i < 8; i++)
			OLED_WR_Byte(F8X16[c * 16 + i + 8], OLED_DATA);
	}
	else
	{
		OLED_Set_Pos(x, y);
		for (i = 0; i < 6; i++)
			OLED_WR_Byte(F6x8[c][i], OLED_DATA);
	}
}

uint32_t oled_pow(uint8_t m, uint8_t n)
{
	uint32_t result = 1;
	while (n--)
		result *= m;
	return result;
}

/**
 * @brief 显示数字
 * @param x
 * @param y
 * @param num 0 ~ 4294967295
 * @param size 6 or 8
 */
void OLED_ShowNum(uint8_t x, uint8_t y, uint32_t num, uint8_t size)
{
	uint8_t t, temp;
	uint32_t templen;
	uint8_t enshow = 0;
	uint8_t len = 0;
	templen = num;
	while (templen > 0)
	{
		templen = templen / 10;
		len++;
	}
	for (t = 0; t < len; t++)
	{
		temp = (num / oled_pow(10, len - t - 1)) % 10;
		if (enshow == 0 && t < (len - 1))
		{
			if (temp == 0)
			{
				OLED_ShowChar(x + size * t, y, ' ', size);
				continue;
			}
			else
				enshow = 1;
		}
		OLED_ShowChar(x + size * t, y, temp + '0', size);
	}
}

/**
 * @brief 显示图片
 * @param x0 0 ~ 127
 * @param y0 0 ~ 7
 * @param x1 0 ~ 127
 * @param y1 0 ~ 7
 * @param BMP
 */
void OLED_DrawBMP(uint8_t x0, uint8_t y0, uint8_t x1, uint8_t y1, uint8_t BMP[])
{
	unsigned int j = 0;
	unsigned char x, y;

	if (y1 % 8 == 0)
		y = y1 / 8;
	else
		y = y1 / 8 + 1;
	for (y = y0; y < y1; y++)
	{
		OLED_Set_Pos(x0, y);
		for (x = x0; x < x1; x++)
		{
			OLED_WR_Byte(BMP[j++], OLED_DATA);
		}
	}
}

void OLED_Init(void)
{
	OLED_RST_Clr();
	HAL_Delay(200);
	OLED_RST_Set();
	HAL_Delay(200);

	OLED_WR_Byte(0xAE, OLED_CMD); //--turn off oled panel
	OLED_WR_Byte(0x00, OLED_CMD); //---set low column address
	OLED_WR_Byte(0x10, OLED_CMD); //---set high column address
	OLED_WR_Byte(0x40, OLED_CMD); //--set start line address  Set Mapping RAM Display Start Line (0x00~0x3F)
	OLED_WR_Byte(0x81, OLED_CMD); //--set contrast control register
	OLED_WR_Byte(0xCF, OLED_CMD); // Set SEG Output Current Brightness
	OLED_WR_Byte(0xA1, OLED_CMD); //--Set SEG/Column Mapping     0Xa0左右反置 0Xa1正常
	OLED_WR_Byte(0xA6, OLED_CMD); //--set normal display
	OLED_WR_Byte(0xA8, OLED_CMD); //--set multiplex ratio(1 to 64)
	OLED_WR_Byte(0x3F, OLED_CMD); //--1/64 duty
	OLED_WR_Byte(0xC8, OLED_CMD); // Set COM/Row Scan Direction   0Xc0上下反置 0Xc8正常
	OLED_WR_Byte(0xD3, OLED_CMD); //-set display offset	Shift Mapping RAM Counter (0x00~0x3F)
	OLED_WR_Byte(0x00, OLED_CMD); //-not offset
	OLED_WR_Byte(0xD5, OLED_CMD); //--set display clock divide ratio/oscillator frequency
	OLED_WR_Byte(0x80, OLED_CMD); //--set divide ratio, Set Clock as 100 Frames/Sec
	OLED_WR_Byte(0xD9, OLED_CMD); //--set pre-charge period
	OLED_WR_Byte(0xF1, OLED_CMD); // Set Pre-Charge as 15 Clocks & Discharge as 1 Clock
	OLED_WR_Byte(0xDA, OLED_CMD); //--set com pins hardware configuration
	OLED_WR_Byte(0x12, OLED_CMD);
	OLED_WR_Byte(0xDB, OLED_CMD); //--set vcomh
	OLED_WR_Byte(0x40, OLED_CMD); // Set VCOM Deselect Level
	OLED_WR_Byte(0x8D, OLED_CMD); //--set Charge Pump enable/disable
	OLED_WR_Byte(0x14, OLED_CMD); //--set(0x10) disable
	OLED_WR_Byte(0xAF, OLED_CMD); /*display ON*/
	OLED_Clear();
}

/**
 * @file oled.c
 * @author 電学渣
 * @brief The New UI Called Morep
 * @version 0.1
 * @date 2023-03-06
 *
 * @copyright Copyright (c) 2023
 *
 */
void MP_SendBuffer(void)
{
	uint8_t x, y;
	for (y = 0; y < 8; y++)
	{
		OLED_WR_Byte(0xb0 + y, OLED_CMD);
		OLED_WR_Byte(0x00, OLED_CMD); // set low column address
		OLED_WR_Byte(0x10, OLED_CMD); // set high column address
		for (x = 0; x < 128; x++)
		{
			OLED_WR_Byte(DisplayBuffer[x][y], OLED_DATA);
		}
	}
}

/**
 * @brief 清除Buffer，不发送
 *
 */
void MP_ClearBuffer(void)
{
	uint8_t x, y;
	for (y = 0; y < 8; y++)
	{
		for (x = 0; x < 128; x++)
		{
			DisplayBuffer[x][y] = 0;
		}
	}
}
/**
 * @brief 画一个点
 * @param x 0 ~ 127
 * @param y 0 ~ 63
 */
void MP_DrawPoint(uint8_t x, uint8_t y)
{
	/* if y = 66, m = 8, n = 2*/
	uint8_t m, n, i;
	m = y / 8;
	n = y % 8;
	i = 1 << n;
	if (m < 8)
		DisplayBuffer[x][m] |= i;
	else
		return;
}
/**
 * @brief 反相显示(异或)
 * @param x
 * @param y
 */
void MP_DrawPointInV(uint8_t x, uint8_t y)
{
	uint8_t m, n, i;
	m = y / 8;
	n = y % 8;
	i = 1 << n;
	if (m < 8)
		DisplayBuffer[x][m] = DisplayBuffer[x][m] ^ i;
	else
		return;
}

void MP_ClearPoint(uint8_t x, uint8_t y)
{
	uint8_t i, m, n;
	i = y / 8;
	m = y % 8;
	n = 1 << m;
	DisplayBuffer[x][i] = ~DisplayBuffer[x][i];
	DisplayBuffer[x][i] |= n;
	DisplayBuffer[x][i] = ~DisplayBuffer[x][i];
}

/**
 * @brief 显示一个字符
 * @param x 0 ~ 127
 * @param y 0 ~63
 * @param chr 字符
 * @param size 6
 */
void MP_DrawChar(uint8_t x, uint8_t y, uint8_t chr, uint8_t size)
{
	uint8_t i, m, chartemp, temp;
	uint8_t y0 = y;
	chartemp = chr - ' '; // 获取起始位置
	for (i = 0; i < 6; i++)
	{
		temp = F6x8[chartemp][i]; // 获取当前一列0x
		for (m = 0; m < 8; m++)
		{
			if (temp & 0x80)
				MP_DrawPoint(x, y0 - m); // 自下往上画点
			else
				MP_ClearPoint(x, y0 - m);
			temp <<= 1;
			y++;
			if ((y - y0) == 8)
			{
				y = y0;
				x++;
				break;
			}
		}
	}
}
/**
 * @brief 显示字符串
 * @param x 1 ~ 128
 * @param y 1 ~ 64
 * @param string 字符串
 * @param size 6
 */
void MP_DrawStr(uint8_t x, uint8_t y, uint8_t *string, uint8_t size)
{
	x--;
	y--;
	while ((*string >= ' ') && (*string <= '~'))
	{
		MP_DrawChar(x, y, *string, size);
		x += size;
		if (x > 127 - size)
			break;
		string++;
	}
}

uint8_t MP_StrWidth(uint8_t *string)
{
	uint8_t len = 0;
	while ((*string >= ' ') && (*string <= '~'))
	{
		len++;
		string++;
	}
	return len;
}
/**
 * @brief 画一条线
 * @param x 0 ~ 128
 * @param y 0 ~ 63
 * @param len 1 ~ 128
 * @param dir UP or RIGHT or DOWN or LEFT
 */
void MP_DrawHVLine(uint8_t x, uint8_t y, uint8_t len, uint8_t dir)
{
	if (dir == UP)
		for (int i = 0; i < len; i++)
			MP_DrawPoint(x, y - i);
	else if (dir == RIGHT)
		for (int i = 0; i < len; i++)
			MP_DrawPoint(x + i, y);
	else if (dir == DOWN)
		for (int i = 0; i < len; i--)
			MP_DrawPoint(x, y + i);
	else
		for (int i = 0; i < len; i++)
			MP_DrawPoint(x - i, y);
}
void MP_DrawHVLineInV(uint8_t x, uint8_t y, uint8_t len, uint8_t dir)
{
	if (dir == UP)
		for (int i = 0; i < len; i++)
			MP_DrawPointInV(x, y - i);
	else if (dir == RIGHT)
		for (int i = 0; i < len; i++)
			MP_DrawPointInV(x + i, y);
	else if (dir == DOWN)
		for (int i = 0; i < len; i--)
			MP_DrawPointInV(x, y + i);
	else
		for (int i = 0; i < len; i++)
			MP_DrawPointInV(x - i, y);
}

/**
 * @brief 画Box
 * @param x 1 ~ 128
 * @param y 1 ~ 64
 * @param width 长度
 * @param height 高度
 * @param R 圆角
 * @param mode Normal or Invert
 */
void MP_DrawRBox(uint8_t x, uint8_t y, uint8_t width, uint8_t height, uint8_t R, uint8_t mode)
{
	if (x + width - 1 > 128 || y - height < 0)
		return;
	R++;
	x--;
	y--;
	if (mode == Normal)
	{
		for (int R1 = 0; R1 < R - 1; R1++)
			MP_DrawHVLine(x + R - 1 - R1, y - height + 1 + R1, width - 2 * (R - 1) + 2 * R1, RIGHT);
		for (int i = 0; i < height - 2 * (R - 1); i++)
			MP_DrawHVLine(x, y - R + 1 - i, width, RIGHT);
		for (int R2 = 0; R2 < R - 1; R2++)
			MP_DrawHVLine(x + R - 1 - R2, y - R2, width - 2 * (R - 1) + 2 * R2, RIGHT);
	}
	else if (mode == Invert)
	{
		for (int R1 = 0; R1 < R - 1; R1++)
			MP_DrawHVLineInV(x + R - 1 - R1, y - height + 1 + R1, width - 2 * (R - 1) + 2 * R1, RIGHT);
		for (int i = 0; i < height - 2 * (R - 1); i++)
			MP_DrawHVLineInV(x, y - R + 1 - i, width, RIGHT);
		for (int R2 = 0; R2 < R - 1; R2++)
			MP_DrawHVLineInV(x + R - 1 - R2, y - R2, width - 2 * (R - 1) + 2 * R2, RIGHT);
	}
}

/**
 * @brief Transition animation
 *
 * @param pos
 * @param Tar
 * @param speed
 */
void MP_RunGui(uint8_t *pos, uint8_t *Tar, uint8_t speed)
{
	if (*pos < *Tar)
	{
		if (*Tar - *pos > speed)
			*pos = *pos + speed;
		else
			*pos = *pos + 1;
	}
	else if (*pos > *Tar)
	{
		if (*pos - *Tar > speed)
			*pos = *pos - speed;
		else
			*pos = *pos - 1;
	}
	else
	{
		return;
	}
}

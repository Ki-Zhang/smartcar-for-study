#ifndef _LCD_H
#define _LCD_H

#include "common.h"
void lcd_clear_position(uint16 x, uint16 y, uint16 color);
void my_lcd_showchar(uint16 x, uint16 y, const int8 dat, uint16 color);
void my_lcd_showstr(uint16 x,uint16 y,const int8 dat[], uint16 color);
void my_lcd_showint16(uint16 x, uint16 y, int16 dat, uint8 select, uint8 cur_bit);
void my_lcd_showint32(uint16 x, uint16 y, int32 dat, uint8 select, uint8 cur_bit);
void my_lcd_showfloat(uint16 x, uint16 y, float dat, uint8 select, uint8 cur_bit);
void my_lcd_displayimage_gray(uint8 *p, uint16 width, uint16 height);
void my_lcd_displayimage_binary(uint8 *p, uint16 width, uint16 height);
void my_lcd_displayimage_debug(uint8 *p, uint8* s, uint16 width, uint16 height);

#endif
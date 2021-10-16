#include "lcd.h"
#include "zf_spi.h"
#include "zf_gpio.h"
#include "zf_systick.h"
#include "SEEKFREE_FONT.h"
#include "SEEKFREE_18TFT.h"
#include "SEEKFREE_PRINTF.h"
#include "imparam.h"

/*
@author:	KD
@version:	v1.0 	2021.10.3 	for flow light
			v2.0	2021.10.6	for test2
@note		参考王子毅学长代码并更改my_lcd_showint32
*/



/*
@brief          clear the specific position
@param          x				    坐标x方向的起点
@param		    y				    坐标y方向的起点
@param		    color			    需要显示的颜色
@return         void
@note           
@since          v1.0    2021.10.3
Sample usage:   lcd(0, 0, BLACK)    //坐标0,0画一个红色的点
*/

void lcd_clear_position(uint16 x, uint16 y, uint16 color)
{

    lcd_set_region(x, y*16, x + 7, y*16 + 15);
    lcd_writedata_16bit(color);
}

/* 
@brief		    液晶显示字符
@param		    x				坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
@param		    y				坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
@param		    dat				需要显示的字符
@param		    color			需要显示的颜色
@return		    void
@note           将*显示为蓝色
@since		    v1.0    2021.10.3
Sample usage:	lcd_showchar(0,0,'x',BLUE);//坐标0,0写一个蓝色字符x
*/

void my_lcd_showchar(uint16 x, uint16 y, const int8 dat, uint16 color)
{
	uint8 i = 0;
    uint8 j = 0;
	uint8 temp = 0;

	for(i = 0; i < 16; i ++)
	{
		lcd_set_region(x,y+i,x+7,y+i);
		temp = tft_ascii[dat - 32][i];                //只打印*
		for(j=0; j<8; j++)
		{
			if(temp&0x01)
				lcd_writedata_16bit(color);
			else
				lcd_writedata_16bit(TFT_BGCOLOR);
			temp>>=1;
		}
	}
}

/* 
@brief		    液晶显示字符串
@param		    x				坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
@param		    y				坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
@param		    dat				需要显示的字符
@param		    color			需要显示的颜色
@return		    void
@note           将*显示为蓝色
@since		    v1.0    2021.10.3
Sample usage:	lcd_showchar(0,0,"123",BLUE);//坐标0,0写一串123
*/

void my_lcd_showstr(uint16 x,uint16 y,const int8 dat[], uint16 color)
{
	uint16 j;

	j = 0;
	while(dat[j] != '\0')
	{
		my_lcd_showchar(x+8*j, y*16, dat[j], color);
		j++;
	}
}

/* 
@brief		    液晶显示16位数字
@param		    x				坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
@param		    y				坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
@param		    dat				需要显示的字符
@param		    select          如果该目标变量被选中
@param          cur_bit         光标所在位置
@return		    void
@note           选择具体的位置
@since		    v1.0    2021.10.3
Sample usage:	lcd_showint16(0,0,123,1,2);//坐标0,0写一串123,选中并将3变成红色
*/

void my_lcd_showint16(uint16 x, uint16 y, int16 dat, uint8 select, uint8 cur_bit)
{
#define INT16_DATA_LENGTH	7
	uint8 a[INT16_DATA_LENGTH] = 0;
	uint8 i;

	if(dat < 0)
	{
		lcd_showchar(x, y * 16, '-');
		dat = -dat;
	}
	else
	{
		lcd_showchar(x, y * 16, ' ');
	}

	a[0] = 0;
	a[1] = 0;
	a[2] = dat/10000;
	a[3] = dat/1000%10;
	a[4] = dat/100%10;
	a[5] = dat/10%10;
	a[6] = dat%10;

	i = 0;
    if(select)
    {
        while(i < INT16_DATA_LENGTH)
        {
            if (i == INT16_DATA_LENGTH - 1 - cur_bit)
            {
                my_lcd_showchar(x+(8*(i+1)), y*16, '0' + a[i], RED);
            }
            else
            {
                my_lcd_showchar(x+(8*(i+1)), y*16, '0' + a[i], BLUE);
            }
            i++;
        }
    }
    else
    {
        while(i < INT16_DATA_LENGTH)
        {
            lcd_showchar(x+(8*(i+1)),y*16,'0' + a[i]);
            i++;
        }
    }
}

/* 
@brief		    液晶显示32位数
@param		    x				坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
@param		    y				坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
@param		    dat				需要显示的字符
@param		    select          如果该目标变量被选中
@param          cur_bit         光标所在位置
@return		    void
@note           选择具体的位置
@since		    v1.0    2021.10.3
Sample usage:	lcd_showint32(0,0,123,1,2);//坐标0,0写一串123,选中并将3变成红色
*/

void my_lcd_showint32(uint16 x, uint16 y, int32 dat, uint8 select, uint8 cur_bit)
{
#define INT32_DATA_LENGTH	10
	uint8 a[INT32_DATA_LENGTH] = 0;
	uint8 i = 0;

	if(dat < 0)
	{
		lcd_showchar(x, y * 16, '-');
		dat = -dat;
	}
	else
	{
		lcd_showchar(x, y * 16, ' ');
	}

	a[0] = 0;
	a[1] = (dat/100000000)%10;
	a[2] = (dat/10000000)%10;
	a[3] = (dat/1000000)%10;
	a[4] = (dat/100000)%10;
	a[5] = (dat/10000)%10;
	a[6] = (dat/1000)%10;
    a[7] = (dat/100)%10;
    a[8] = (dat/10)%10;
    a[9] = dat%10;

    if(select)
    {
        while(i < INT32_DATA_LENGTH)
        {
            if (i == INT32_DATA_LENGTH - 1 - cur_bit)
            {
                my_lcd_showchar(x+(8*(i+1)), y*16, '0' + a[i], RED);
            }
            else
            {
                my_lcd_showchar(x+(8*(i+1)), y*16, '0' + a[i], BLUE);
            }
            i++;
        }
    }
    else
    {
        while(i < INT32_DATA_LENGTH)
        {
            lcd_showchar(x+(8*(i+1)), y*16, '0' + a[i]);
            i++;
        }
    }
}

/* 
@brief		    液晶显示字符
@param		    x				坐标x方向的起点 参数范围 0 -（TFT_X_MAX-1）
@param		    y				坐标y方向的起点 参数范围 0 -（TFT_Y_MAX/16-1）
@param		    dat				需要显示的字符
@param		    select          如果该目标变量被选中
@param          cur_bit         光标所在位置
@return		    void
@note           选择具体的位置
@since		    v1.0    2021.10.3
Sample usage:	lcd_showint32(0,0,123,1,2);//坐标0,0写一串123,选中并将3变成红色
*/

void my_lcd_showfloat(uint16 x, uint16 y, float dat, uint8 select, uint8 cur_bit)
{
#define FLOAT_DATA_LENGTH	7
    uint8 a[FLOAT_DATA_LENGTH];
	uint8 i;
    if(dat < 0)
	{
		lcd_showchar(x, y*16, '-');
		dat = -dat;
	}
	else
	{
		lcd_showchar(x, y*16, ' ');
	}

	a[0] = (int16)(dat/10000)%10;
    a[1] = (int16)(dat/1000)%10;
	a[2] = (int16)(dat/100)%10;
	a[3] = (int16)(dat/10)%10;
	a[4] = (int16)(dat*1)%10;
	a[5] = (int32)(dat*10)%10;
	a[6] = (int32)(dat*100)%10;

    i = 0;
    if(select)
    {
        while(i < FLOAT_DATA_LENGTH)
        {
            if (i == FLOAT_DATA_LENGTH - 1 - cur_bit)
            {
                my_lcd_showchar(x+(8*(i+1)),y*16,'0' + a[i], GREEN);
            }
            else
            {
                my_lcd_showchar(x+(8*(i+1)),y*16,'0' + a[i], BLUE);
            }
            i++;
        }
    }
    else
    {
        while(i < FLOAT_DATA_LENGTH)
        {
            lcd_showchar(x+(8*(i+1)),y*16,'0' + a[i]);
            i++;
        }
    }
}

/* 
@brief		    下位机灰度画面
@param		    p				图像的左上角位置
@param		    width			图像的宽
@param		    height			图像的高
@return		    void
@note           
@since		    v1.0    2021.10.3
Sample usage:	
*/

void my_lcd_displayimage_gray(uint8 *p, uint16 width, uint16 height) 
{
	uint32 i,j;

	uint16 color = 0;
	uint16 temp = 0;

	uint16 coord_x = 0;
	uint16 coord_y = 0;


	if(0==TFT_DISPLAY_DIR || 1==TFT_DISPLAY_DIR)									// 竖屏
	{
		coord_x = height>TFT_X_MAX?TFT_X_MAX:height;
		coord_y = width>TFT_Y_MAX?TFT_Y_MAX:width;

		for(j=0;j<coord_y;j++)
		{
			lcd_set_region(0,j,coord_x-1,j);
			for(i=0;i<coord_x;i++)
			{
				temp = *(p+i*width+j*width/coord_y);								// 读取像素点
				color=(0x001f&((temp)>>3))<<11;					// 4~5 -> 12~13
				color=color|(((0x003f)&((temp)>>2))<<5);		// 2~7 -> 5~10
				color=color|(0x001f&((temp)>>3));				// 
				lcd_writedata_16bit(color); 
			}
		}
	}
	else																			// 横屏
	{
		coord_x = width>TFT_X_MAX?TFT_X_MAX:width;
		coord_y = height>TFT_Y_MAX?TFT_Y_MAX:height;
		lcd_set_region(0,0,coord_x-1,coord_y-1);

		for(j=0;j<coord_y;j++)
		{
			for(i=0;i<coord_x;i++)
			{
				temp = *(p+j*width+i*width/coord_x);								// 读取像素点
				color=(0x001f&((temp)>>3))<<11;
				color=color|(((0x003f)&((temp)>>2))<<5);
				color=color|(0x001f&((temp)>>3));
				lcd_writedata_16bit(color); 
			}
		}
	}
}

/* 
@brief		    下位机图像二值化画面
@param		    p				图像的左上角位置
@param		    width			图像的宽
@param		    height			图像的高
@return		    void
@note           
@since		    v1.0    2021.10.3
Sample usage:	
*/

void my_lcd_displayimage_binary(uint8 *p, uint16 width, uint16 height) 
{
	uint32 i,j;

	uint16 temp = 0;

	uint16 coord_x = 0;
	uint16 coord_y = 0;

	coord_x = width>TFT_X_MAX?TFT_X_MAX:width;
	coord_y = height>TFT_Y_MAX?TFT_Y_MAX:height;
	lcd_set_region(0,0,coord_x-1,coord_y-1);

	for(j=0;j<coord_y;j++)
	{
		for(i=0;i<coord_x;i++)
		{

			temp = g_thres_table[*(p+j*width+i*width/coord_x)];
			switch (temp)
			{
			case 0:
				lcd_writedata_16bit(BLACK); 
				break;
			case 1:
				lcd_writedata_16bit(WHITE); 
				break;
			}
		}
	}
}

/* 
@brief		    下位机图像调试画面
@param		    p				图像的左上角位置
@param		    width			图像的宽
@param		    height			图像的高
@return		    void
@note           
@since		    v1.0    2021.10.3
Sample usage:	
*/

void my_lcd_displayimage_debug(uint8 *p, uint8* s, uint16 width, uint16 height) 
{
	uint32 i,j;

	uint16 color = 0;
	uint8 temp = 0;

	uint16 coord_x = 0;
	uint16 coord_y = 0;


	if(0==TFT_DISPLAY_DIR || 1==TFT_DISPLAY_DIR)									// 竖屏
	{
		coord_x = height>TFT_X_MAX?TFT_X_MAX:height;
		coord_y = width>TFT_Y_MAX?TFT_Y_MAX:width;

		for(j=0;j<coord_y;j++)
		{
			lcd_set_region(0,j,coord_x-1,j);
			for(i=0;i<coord_x;i++)
			{
				temp = *(p+i*width+j*width/coord_y);								// 读取像素点
				switch (temp)
				{
				case 0:
					temp = *(s+j*width+i*width/coord_x);							// 读取像素点
					color = (0x001f&((temp)>>3))<<11;
					color = color|(((0x003f)&((temp)>>2))<<5);
					color = color|(0x001f&((temp)>>3));
					lcd_writedata_16bit(color);
					break;
				case 1:
					lcd_writedata_16bit(WHITE);
					break;
				case 2:
					lcd_writedata_16bit(GREEN);
					break;
				case 3:
					lcd_writedata_16bit(RED);
					break;
				case 4:
					lcd_writedata_16bit(RED);
					break;
				case 5:
					lcd_writedata_16bit(BLUE);
					break;
				case 6:
					lcd_writedata_16bit(PURPLE);
					break;
				default:
					lcd_writedata_16bit(YELLOW);
					break;
				}
			}
		}
	}
	else																			// 横屏
	{
		coord_x = width>TFT_X_MAX?TFT_X_MAX:width;
		coord_y = height>TFT_Y_MAX?TFT_Y_MAX:height;
		lcd_set_region(0,0,coord_x-1,coord_y-1);

		for(j=0;j<coord_y;j++)
		{
			for(i=0;i<coord_x;i++)
			{
				temp = *(p+j*width+i*width/coord_x);								// 读取像素点
				switch (temp)
				{
				case 0:
					temp = *(s+j*width+i*width/coord_x);							// 读取像素点
					color = (0x001f&((temp)>>3))<<11;
					color = color|(((0x003f)&((temp)>>2))<<5);
					color = color|(0x001f&((temp)>>3));
					lcd_writedata_16bit(color);
					break;
				case 1:
					lcd_writedata_16bit(WHITE);
					break;
				case 2:
					lcd_writedata_16bit(GREEN);
					break;
				case 3:
					lcd_writedata_16bit(RED);
					break;
				case 4:
					lcd_writedata_16bit(RED);
					break;
				case 5:
					lcd_writedata_16bit(BLUE);
					break;
				case 6:
					lcd_writedata_16bit(PURPLE);
					break;
				default:
					lcd_writedata_16bit(YELLOW);
					break;
				}
			}
		}
	}
}
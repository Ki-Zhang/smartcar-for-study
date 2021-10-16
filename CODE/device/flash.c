#include "flash.h"
#include "zf_flash.h"


/*
@author:	KD
@version:	v1.0 	2021.10.3 	for flow light
			v2.0	2021.10.6	for test2
@note		v2.0版本中增加两个函数:
                将无符号32位数字的每一位重组成一个无符号32位数
                将无符号32位数字每一位拆分
*/


/* 
@brief		    32位数字拆分每一位
@param		    origin  //32位数字
@param		    str     //数组首地址
@return		    void
@note           v1.0	最初版本的按键,放置在while循环中
				v1.1	增加按键时长控制,放在TIM中断中
@since		    v1.0    2021.10.3
				v1.1	2021.10.5
                v2.0    2021.10.6   修复BUG:UINT32_MAXBIT和UITN32_MAX的区别
Sample usage:	
*/

void convert_32_to_str(uint32 origin, uint8 str[])
{
#define UINT32_MAXBIT 9
    uint8 i = 0;
    while (i < UINT32_MAXBIT)
    {
        str[i++] = origin % 10;
        origin /= 10;
    }
}

/* 
@brief		    将每一位数字存储成32位数组
@param		    str     //数组首地址
@return		    uint32  //32位无符号数
@note           v1.0	最初版本的按键,放置在while循环中
				v1.1	增加按键时长控制,放在TIM中断中
@since		    v1.0    2021.10.3
				v1.1	2021.10.5
Sample usage:	
*/

uint32 convert_str_to_32(uint8 str[])
{
#define UINT32_MAXBIT 9
    uint8 i = 0;
    uint32 temp = 0;
    //read_data[9]=str[9];        
    while (i < UINT32_MAXBIT)
    {
        temp *= 10;
        temp += str[UINT32_MAXBIT - 1 - i];
        i++;
    }
    return temp;
}
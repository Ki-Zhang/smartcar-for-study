#ifndef _BUTTON_H
#define _BUTTON_H

#include "common.h"
#include "param.h"
#include "general.h"

//引脚定义
#define UP_PIN              D0  //to pause the number
#define DOWN_PIN            D1  //to pause the number
#define CHOOSE_PIN          D2  //to calculate the number
#define RETURN_PIN          D3  //to calculate the number
//常量定义
#define DEBOUNCE_DELAY      50
#define LONG_PRESSED_DELAY  2000
#define CURRENT_TIME		g_time
#define VALUE_NONE          0

//定义模式
#define UP 	        0x01
#define UP_QUICK    0x11
#define DOWN   	    0x02
#define DOWN_QUICK  0x12
#define CHOOSE	    0x03
#define PWM_MODE    0x13
#define RETURN      0x04
#define IMAGE       0x14


//函数定义
void button_init(void);
void button_scan(void);

#endif
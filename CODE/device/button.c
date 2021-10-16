#include "button.h"
#include "zf_gpio.h"
#include "param.h"
#include "zf_systick.h"
#include "common.h"
#include "general.h"

#define INIT_UP_PIN()        	gpio_init(UP_PIN, GPI, GPIO_HIGH, GPI_PULL_UP)
#define INIT_CHOOSE_PIN()    	gpio_init(CHOOSE_PIN, GPI, GPIO_HIGH, GPI_PULL_UP)
#define INIT_DOWN_PIN()    		gpio_init(DOWN_PIN, GPI, GPIO_HIGH, GPI_PULL_UP)
#define INIT_RETURN_PIN()    	gpio_init(RETURN_PIN, GPI, GPIO_HIGH, GPI_PULL_UP)

//内部变量
static uint8 LAST_BUTTON_VALUE = 0;
static uint32 DEBOUNCE_TIME = 0;
static uint32 LONG_PRESSED_TIME = 0;

/*
@author:	KD
@version:	v1.0 	2021.10.3 	for flow light
			v1.1	2021.10.5	for flow light
			v2.0	2021.10.6 	for test2
@note		v1.0版本中采用的是陈宏学长的代码即加上相应的延时来判断按键按下
			v1.1版本中增加了按键的长按短按
			v2.0版本中开启了D0,D1,D2三个按键并区分长按短按
*/


/* 
@brief		    按键初始化
@param		    void
@return		    void
@note           v1.0	最初版本
				v1.1	更改了每一个按键,可以区别长按和短按
@since		    v1.0    2021.10.3
				v1.1	2021.10.5
Sample usage:	
*/

void button_init(void)
{
    //内部上拉电阻
    INIT_UP_PIN();
    INIT_CHOOSE_PIN();
	INIT_DOWN_PIN();
	INIT_RETURN_PIN();
}

/* 
@brief		    按键扫描
@param		    void
@return		    void
@note           v1.0	最初版本的按键,放置在while循环中
				v1.1	增加按键时长控制,放在TIM中断中
@since		    v1.0    2021.10.3
				v1.1	2021.10.5
Sample usage:	
*/

void button_scan(void)
{
    if(gpio_get(UP_PIN)==0)								//选择升,加,上
    {
		if (LAST_BUTTON_VALUE == UP)
		{
			// 消抖处理
			if (CURRENT_TIME > DEBOUNCE_TIME)
			{
				g_button_value = UP;
				if (CURRENT_TIME > LONG_PRESSED_TIME)
				{
					g_button_value = UP_QUICK;
				}
			}			                   
		}
		else
		{
			// 保存上次按键信息
			LAST_BUTTON_VALUE = UP;
			// 开始计时
			DEBOUNCE_TIME = CURRENT_TIME + DEBOUNCE_DELAY;
			LONG_PRESSED_TIME = CURRENT_TIME + LONG_PRESSED_DELAY;
		}
		return;
	}
    else if(gpio_get(DOWN_PIN)==0)						//选择降,减,下
    {
		if (LAST_BUTTON_VALUE == DOWN)
		{
			// 消抖处理
			if (CURRENT_TIME > DEBOUNCE_TIME)
			{
				g_button_value = DOWN;
				if (CURRENT_TIME > LONG_PRESSED_TIME)
				{
					g_button_value = DOWN_QUICK;
				}
			}			                   
		}
		else
		{
			// 保存上次按键信息
			LAST_BUTTON_VALUE = DOWN;
			// 开始计时
			DEBOUNCE_TIME = CURRENT_TIME + DEBOUNCE_DELAY;
			LONG_PRESSED_TIME = CURRENT_TIME + LONG_PRESSED_DELAY;
		}
		return;
	}
	else if(gpio_get(CHOOSE_PIN)==0)					//选择,模式切换
    {
		if (LAST_BUTTON_VALUE == CHOOSE)
		{
			// 消抖处理
			if (CURRENT_TIME > DEBOUNCE_TIME)
			{
				g_button_value = CHOOSE;				//选择该数值
				if (CURRENT_TIME > LONG_PRESSED_TIME)
				{
					g_button_value = PWM_MODE;			//进入PWM模式
				}
			}			                   
		}
		else
		{
			// 保存上次按键信息
			LAST_BUTTON_VALUE = CHOOSE;
			// 开始计时
			DEBOUNCE_TIME = CURRENT_TIME + DEBOUNCE_DELAY;
			LONG_PRESSED_TIME = CURRENT_TIME + LONG_PRESSED_DELAY;
		}
		return;
	}
	else if(gpio_get(RETURN_PIN)==0)					//返回
    {
		if (LAST_BUTTON_VALUE == RETURN)
		{
			// 消抖处理
			if (CURRENT_TIME > DEBOUNCE_TIME)
			{
				g_button_value = RETURN;				//返回
				if (CURRENT_TIME > LONG_PRESSED_TIME)
				{
					g_button_value = IMAGE;			//返回
				}
			}			                   
		}
		else
		{
			// 保存上次按键信息
			LAST_BUTTON_VALUE = RETURN;
			// 开始计时
			DEBOUNCE_TIME = CURRENT_TIME + DEBOUNCE_DELAY;
			LONG_PRESSED_TIME = CURRENT_TIME + LONG_PRESSED_DELAY;
		}
		return;
	}
	else												//所有按键清零以防对下次长按做成干扰
	{
		LAST_BUTTON_VALUE = VALUE_NONE;
		DEBOUNCE_TIME = VALUE_NONE;
		LONG_PRESSED_TIME = VALUE_NONE;
	}
	/*if(!gpio_get(D0))
   	{
      	systick_delay_ms(50);
      	if(!gpio_get(PAUSE_PIN))
    	  {
        	while(!gpio_get(PAUSE_PIN));
            g_button_value = pause;
      	}
   	}
	if(!gpio_get(D3))
   	{
      	systick_delay_ms(50);
      	if(!gpio_get(D3))
    	  {
        	while(!gpio_get(D3));
            g_button_value = add;
      	}
   	}
	if(!gpio_get(D2))
   	{
      	systick_delay_ms(50);
      	if(!gpio_get(D2))
    	  {
        	while(!gpio_get(D2));
            g_button_value = sub;
      	}
   	}*/
}
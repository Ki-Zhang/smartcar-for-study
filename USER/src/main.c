/*********************************************************************************************************************
* COPYRIGHT NOTICE
* Copyright (c) 2019,逐飞科技
* All rights reserved.
* 技术讨论QQ群：一群：179029047(已满)  二群：244861897
*
* 以下所有内容版权均属逐飞科技所有，未经允许不得用于商业用途，
* 欢迎各位使用并传播本程序，修改内容时必须保留逐飞科技的版权声明。
*
* @file				main
* @company			成都逐飞科技有限公司
* @author			逐飞科技(QQ3184284598)
* @version			查看doc内version文件 版本说明
* @Software			IAR 8.3 or MDK 5.24
* @Target core		MM32F3277
* @Taobao			https://seekfree.taobao.com/
* @date				2021-02-22
********************************************************************************************************************/

#include "headfile.h"
#include "page.h"
#include "image.h"
#include "lcd.h"
#include "flash.h"
#include "button.h"
#include "pwm.h"

//uint32 temp=0;
// *************************** 例程说明 ***************************
// 
// 测试需要准备逐飞科技 MM32F3277 核心板一块
// 
// 调试下载需要准备逐飞科技 CMSIS-DAP 调试下载器 或 ARM 调试下载器 一个
// 
// 本例程初始化了 主板上屏幕接口
// 
// 烧录本例程后 请断电接线
// 需要安装1.8TFT模块
// 确认连接无误 上电 请务必使用电池供电 并确保电池电量充足
// 此时1.8TFT上显示信息
// 
// 打开新的工程或者工程移动了位置务必执行以下操作
// 第一步 关闭上面所有打开的文件
// 第二步 project->clean  等待下方进度条走完
// 
// *************************** 例程说明 ***************************

// **************************** 宏定义 ****************************
// **************************** 宏定义 ****************************

// **************************** 变量定义 ****************************
// **************************** 变量定义 ****************************

// **************************** 代码区域 ****************************

/* 
@brief		    main函数
@param		    void
@return		    int
@note           v1.0	最初版本,将按键函数写在while里
				v1.1	更改了按键扫描函数,将按键放置在中断中
				v2.1	增加摄像头
@since		    v1.0    2021.10.3
				v1.1	2021.10.5
				v2.0	2021.10.10
				v2.1	2021.10.13
Sample usage:	
*/

int main(void)
{
	board_init(true);																// 初始化 debug 输出串口

	//关闭相应功能
	g_motor = MOTOR_OFF;
	g_image_enable = DISABLE;

	//此处编写用户代码(例如：外设初始化代码等)
	lcd_init();			//lcd初始化
	button_init();		//按键初始化
	motor_init();		//电机初始化
	servo_init();		//舵机初始化
	mt9v03x_init(); // 摄像头初始化

	// 打开定时器中断，图像处理中断
	tim_interrupt_init(TIM_5, 1000, 1);
	exti_interrupt_init(A0, EXTI_Trigger_Rising, 2);
	
	lcd_clear(WHITE);
    
	//g_image_enable = ENABLE;
	//g_servo = ENABLE;
	//g_motor = ENABLE;
	//MOTOR = 8000;

	//为了测量图像处理时间用
    //systick_start();
	while(1)
	{
		//my_lcd_displayimage_binary(mt9v03x_image[0][0], MT9V03X_W, MT9V03X_H);
		//lcd_showint16(0, 0, (int16)g_image_err);
        //lcd_showint32(0,0,(int32)(end_time-start_time),9);
		//my_lcd_displayimage_debug(&g_watch[0][0], &g_image[0][0], MT9V03X_W, MT9V03X_H);
		page_start();
	}
}
// **************************** 代码区域 ****************************

#ifndef _GENERAL_H
#define _GENERAL_H

// #define CAR_3WHEEL

#include "common.h"
#include "headfile.h"

#define ENABLE          1
#define DISABLE         0

#define DIR_LEFT        0
#define DIR_RIGHT       1

#define LINE_LEFT       0
#define LINE_RIGHT      1

//电机
#define MOTOR_OFF       0
#define MOTOR_ON        1

//引导模式
#define GUIDE_CAMERA   0
#define GUIDE_INDUCT   1
#define GUIDE_NONE     2

//蜂鸣器
#define BEEP_SHORT  1  // 短鸣
#define BEEP_LONG   2  // 长鸣
#define BEEP_DOUBLE 3  // 双响鸣叫
#define BEEP_HURRY  4  // 急促鸣叫
#define BEEP_DECAY  5  // 衰减鸣叫
#define BEEP_ONCE   6  // 打点鸣叫
#define BEEP_LOW    7  // 低音鸣叫

//定义按键
/*#define BUTTON_NONE             0x00

#define BUTTON_UP		        0x01
#define BUTTON_DOWN 	        0x02
#define BUTTON_LEFT	            0x03
#define BUTTON_RIGHT	        0x04
#define BUTTON_OK		        0x05
#define BUTTON_UP_LONG          0x11
#define BUTTON_DOWN_LONG        0x12
#define BUTTON_LEFT_LONG        0x13
#define BUTTON_RIGHT_LONG	    0x14
#define BUTTON_OK_LONG		    0x15

#define BUTTON_UP2		        0x06
#define BUTTON_DOWN2 	        0x07
#define BUTTON_PAUSE	        0x08
#define BUTTON_RIGHT2	        0x09
#define BUTTON_ADD              0x0a
#define BUTTON_UP2_LONG         0x16
#define BUTTON_DOWN2_LONG       0x17
#define BUTTON_PAUSE_LONG       0x18
#define BUTTON_RIGHT2_LONG	    0x19
#define BUTTON_SUB              0x1a
*/
//定义类型
#define TYPE_INT16  1
#define TYPE_UINT16 2
#define TYPE_INT32  3
#define TYPE_UINT32 4
#define TYPE_FLOAT  5
// #define TYPE_INT8   6
// #define TYPE_UINT8  7

//陀螺仪
#define MPU_ACC_X   icm_acc_x
#define MPU_ACC_Y   icm_acc_y
#define MPU_ACC_Z   icm_acc_z
#define MPU_GYRO_X  icm_gyro_x
#define MPU_GYRO_Y  icm_gyro_y
#define MPU_GYRO_Z  icm_gyro_z
#define GYRO_LBS    (16.384f)      // 陀螺仪精度
#define GET_ACC()   get_icm20602_accdata_spi()
#define GET_GYRO()  get_icm20602_gyro_spi()

//时间
#define GET_CURRENT_TIME()          (g_time)

//计时
// #define TIMER_INIT()                {systick_start();gpio_set(A8, 1);}
// #define TIMER_RECORD()              {g_clock=systick_getval_us();gpio_set(A8, 0);}

#endif
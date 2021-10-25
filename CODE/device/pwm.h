#ifndef _PWM_H
#define _PWM_H

//void pwm_control(void);
#define MAX_DUTY			50														// 最大 75% 占空比
//#define DIR_L_CH1			A0
//#define DIR_L_CH2			A1
//#define DIR_R_CH3			A2
//#define DIR_R_CH4			A3

#define MIDDLE_DUTY         25000
#define MAX_N_DUTY          -50000
#define MAX_P_DUTY          50000

#define MAX_DUTY			50														// 最大 75% 占空比
/*#define DIR_L				A0
#define DIR_R				A2

#define PWM_TIM				TIM_2
#define PWM_L				TIM_2_CH2_A01
#define PWM_R				TIM_2_CH4_A03*/

#define PWM_TIM				TIM_2
#define LEFT_MOTOR_CH1      TIM_2_CH2_A01
#define LEFT_MOTOR_CH3		TIM_2_CH4_A03
//#define RIGHT_MOTOR_CH3     TIM_2_CH3_A02
//#define RIGHT_MOTOR_CH4		TIM_2_CH4_A03*/

#define SERVO_MOTOR_TIM				TIM_2											// 定义主板上舵机对应TIM
#define SERVO_MOTOR_PWM				TIM_2_CH1_A15									// 定义主板上舵机对应引脚
#define SERVO_MOTOR_FREQ			50											    // 定义主板上舵机频率  请务必注意范围 50-300

//#define SERVO_MOTOR_L_MAX			60												// 定义主板上舵机活动范围 角度
//#define SERVO_MOTOR_R_MAX			120	

#define MOTOR_DUTYMAX 40000            //最大的电机duty
#define SERVO_DUTYMAX 162           //最大的舵机duty
#define SERVO_DUTYMIN 2            //最小的舵机duty
#define SERVO_MIDDUTY 72            //最小的舵机duty

#include "common.h"

#define SERVO_MOTOR_DUTY(x)			(PWM_DUTY_MAX/(1000/SERVO_MOTOR_FREQ)*(1+x/180))

extern uint8 g_motor;
extern uint8 g_servo;

void motor_init(void);
void servo_init(void);
void change_duty_motor(int16 duty);
void change_duty_servo(uint16 duty);
void motor_output(void);
void servo_output(void);


void change_left_duty_motor(int32 duty, uint8 compensate);
void change_right_duty_motor(int32 duty, uint8 compensate);
//int8 duty_l = 0;
//int8 duty_r = 0;
//bool dir_l = true;
//bool dir_r = true;

#endif
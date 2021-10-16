#include "pwm.h"
#include "lcd.h"
#include "SEEKFREE_18TFT.h"
#include "page.h"
#include "button.h"
#include "zf_gpio.h"
#include "zf_pwm.h"

/*
@author:	KD
@version:	v2.0	2021.10.6	for test2
@note		
*/

// ------------------ 舵机占空比计算方式 ------------------


#if (SERVO_MOTOR_FREQ<50 || SERVO_MOTOR_FREQ>300)
	#error "SERVO_MOTOR_FREQ ERROE!"
#endif
// **************************** 宏定义 ****************************

// **************************** 变量定义 ****************************
//float servo_motor_duty = 0;															// 舵机动作角度
//float servo_motor_dir = 1;


uint8 g_motor = 0;
uint8 g_servo = 0;
//static uint8 speed = 0;

/* 
@brief		    电机初始化
@param		    void
@return		    void
@note           v1.0	电机初始化
@since		    v1.0    2021.10.7
Sample usage:	
*/

void motor_init(void)
{
    gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);								// GPIO 初始化为输出 默认上拉输出高
	gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);							// GPIO 初始化为输出 默认上拉输出高

    pwm_init(PWM_TIM, PWM_L, 10000, 0);											// PWM 通道2 初始化频率10KHz 占空比初始为0
	pwm_init(PWM_TIM, PWM_R, 10000, 0);
}
/*void motor_init(void)
{
    gpio_init(DIR_L_CH1, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(DIR_L_CH2, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(DIR_R_CH3, GPO, GPIO_HIGH, GPO_PUSH_PULL);
    gpio_init(DIR_R_CH4, GPO, GPIO_HIGH, GPO_PUSH_PULL);
}*/

/* 
@brief		    舵机初始化
@param		    void
@return		    void
@note           v1.0	电机初始化
@since		    v1.0    2021.10.7
Sample usage:	
*/

void servo_init(void)
{
    pwm_init(SERVO_MOTOR_TIM, SERVO_MOTOR_PWM, SERVO_MOTOR_FREQ, 3500);
}


/* 
@brief		    电机pwm控制
@param		    void
@return		    void
@note           v1.0    
@since		    v1.0    2021.10.7
Sample usage:	
*/

void change_duty_motor(int16 duty)
{
    if(duty >= 0)
    {
        if(duty >= MOTOR_DUTYMAX)
        {
            duty = MOTOR_DUTYMAX;
        }
        gpio_set(DIR_L, GPIO_HIGH);
        gpio_set(DIR_R, GPIO_HIGH);
        pwm_duty_updata(PWM_TIM, PWM_R, duty*(PWM_DUTY_MAX/100));
        pwm_duty_updata(PWM_TIM, PWM_L, duty*(PWM_DUTY_MAX/100));
    }
    else
    {
        duty = -duty;
        if(duty >= MOTOR_DUTYMAX)
        {
            duty = MOTOR_DUTYMAX;
        }
        gpio_set(DIR_L, GPIO_LOW);
        gpio_set(DIR_R, GPIO_LOW);
        pwm_duty_updata(PWM_TIM, PWM_R, duty*(PWM_DUTY_MAX/100));
        pwm_duty_updata(PWM_TIM, PWM_L, duty*(PWM_DUTY_MAX/100));
    }
}

/* 
@brief		    左电机pwm控制
@param		    void
@return		    void
@note           v1.0    
@since		    v1.0    2021.10.7
Sample usage:	
*/
/*
void change_left_duty_motor(int32 duty, uint8 compensate)
{
    uint16 duty_1 = 0;
    uint16 duty_2 = 0;

    if(compensate)
    {
        if(duty >0)
        {
            duty = duty + g_left_motor_bias;
        }
        if(duty >0)
        {
            duty = duty - g_left_motor_bias;
        }
    }
    if(duty <= MAX_N_DUTY)
    {
        duty = MAX_N_DUTY;
    }
    else if(duty >= MAX_P_DUTY)
    {
        duty = MAX_P_DUTY;
    }
    duty_1 = MIDDLE_DUTY - duty/2;
    duty_2 = MIDDLE_DUTY + duty/2;
    pwm_duty_updata(PWM_TIM, LEFT_MOTOR_CH1, duty_1);
    pwm_duty_updata(PWM_TIM, LEFT_MOTOR_CH2, duty_1);
}*/

/* 
@brief		    右电机pwm控制
@param		    void
@return		    void
@note           v1.0    
@since		    v1.0    2021.10.7
Sample usage:	
*/
/*
void change_right_duty_motor(int32 duty, uint8 compensate)
{
    uint32 duty_1 = 0;
    uint32 duty_2 = 0;

    if(compensate)
    {
        if(duty >0)
        {
            duty = duty + g_right_motor_bias;
        }
        if(duty >0)
        {
            duty = duty - g_right_motor_bias;
        }
    }
    if(duty <= MAX_N_DUTY)
    {
        duty = MAX_N_DUTY;
    }
    else if(duty >= MAX_P_DUTY)
    {
        duty = MAX_P_DUTY;
    }
    duty_1 = MIDDLE_DUTY - duty/2;
    duty_2 = MIDDLE_DUTY + duty/2;
    pwm_duty_updata(PWM_TIM, RIGHT_MOTOR_CH3, duty_1);
    pwm_duty_updata(PWM_TIM, RIGHT_MOTOR_CH4, duty_1);
}*/
/* 
@brief		    电机输出
@param		    void
@return		    void
@note           v1.0
@since		    v1.0    2021.10.7
Sample usage:	
*/

void motor_output(void)
{
    if (g_motor == MOTOR_ON)
    {
        change_duty_motor(MOTOR);
    }
    else // off
    {
        change_duty_motor(0);
    }
}

/* 
@brief		    舵机输出
@param		    void
@return		    void
@note           v1.0
@since		    v1.0    2021.10.7
Sample usage:	
*/

void servo_output(void)
{
    if(g_servo == MOTOR_ON)
    {
        pwm_duty_updata(SERVO_MOTOR_TIM, SERVO_MOTOR_PWM, SERVO_MOTOR_DUTY(SERVO));
    }
    else
    {
        pwm_duty_updata(SERVO_MOTOR_TIM, SERVO_MOTOR_PWM, 3500);//SERVO_MOTOR_DUTY(SERVO_MIDDUTY));
    }
}
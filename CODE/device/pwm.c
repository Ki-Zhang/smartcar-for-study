#include "pwm.h"
#include "page.h"
//#include "zf_gpio.h"
#include "zf_pwm.h"
#include "imparam.h"

/*
@author:	KD
@version:	v2.0	2021.10.6	for test2
            v3.0    2021.11.4   pid for exam
@note		
*/

#if (SERVO_MOTOR_FREQ<50 || SERVO_MOTOR_FREQ>300)
	#error "SERVO_MOTOR_FREQ ERROE!"
#endif
// **************************** 宏定义 ****************************

// **************************** 变量定义 ****************************
uint8 g_motor = 0;
uint8 g_servo = 0;

int32 MOTOR = 0;                    //电机PWM值 (范围-MOTOR_DUTYMAX  -- MOTOR_DUTYMAX)
float SERVO = SERVO_MIDDUTY;        //舵机PWM值
//舵机PID
float SERVO_P = 0.0;//9
float SERVO_I = 0.0;
float SERVO_D = 0.0;//80

// uint16 DUTY1 = 0;
// uint16 DUTY2 = 0;

#define SERVO_I_LIMIT 100
static float Current_error = 0.0;
static float Last_error = 0.0;

/* 
@brief		    电机初始化
@param		    void
@return		    void
@note           v1.0	电机初始化
@since		    v1.0    2021.10.7
Sample usage:	
*/

/*void motor_init(void)
{
    //gpio_init(DIR_L, GPO, GPIO_HIGH, GPO_PUSH_PULL);								// GPIO 初始化为输出 默认上拉输出高
	gpio_init(DIR_R, GPO, GPIO_HIGH, GPO_PUSH_PULL);							// GPIO 初始化为输出 默认上拉输出高

    //pwm_init(PWM_TIM, PWM_L, 10000, 0);											// PWM 通道2 初始化频率10KHz 占空比初始为0
	pwm_init(PWM_TIM, PWM_R, 10000, 0);
}*/
void motor_init(void)
{
    pwm_init(PWM_TIM, LEFT_MOTOR_CH1, 10000, 0);
    pwm_init(PWM_TIM, LEFT_MOTOR_CH3, 10000, 0);
    pwm_init(PWM_TIM, RIGHT_MOTOR_CH2, 10000, 0);
    pwm_init(PWM_TIM, RIGHT_MOTOR_CH4, 10000, 0);
}

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
/*
void change_duty_motor(int16 duty)
{
    if(duty >= 0)
    {
        if(duty >= MOTOR_DUTYMAX)
        {
            duty = MOTOR_DUTYMAX;
        }
        //gpio_set(DIR_L, GPIO_HIGH);
        gpio_set(DIR_R, GPIO_HIGH);
        pwm_duty_updata(PWM_TIM, PWM_R, duty*(PWM_DUTY_MAX/100));
        //pwm_duty_updata(PWM_TIM, PWM_L, duty*(PWM_DUTY_MAX/100));
    }
    else
    {
        duty = -duty;
        if(duty >= MOTOR_DUTYMAX)
        {
            duty = MOTOR_DUTYMAX;
        }
        //gpio_set(DIR_L, GPIO_LOW);
        gpio_set(DIR_R, GPIO_LOW);
        pwm_duty_updata(PWM_TIM, PWM_R, duty*(PWM_DUTY_MAX/100));
        //pwm_duty_updata(PWM_TIM, PWM_L, duty*(PWM_DUTY_MAX/100));
    }
}*/

/* 
@brief		    左电机pwm控制
@param		    void
@return		    void
@note           v1.0    
@since		    v1.0    2021.10.7
Sample usage:	
*/

void change_left_duty_motor(int32 duty)//, uint8 compensate)
{
    uint16 duty_1 = 0;
    uint16 duty_2 = 0;
    // if(compensate)
    // {
    //     if(duty >0)
    //     {
    //         duty = duty + g_left_motor_bias;
    //     }
    //     if(duty >0)
    //     {
    //         duty = duty - g_left_motor_bias;
    //     }
    // }
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
    // DUTY1 = duty_1;
    // DUTY2 = duty_2;
    pwm_duty_updata(PWM_TIM, LEFT_MOTOR_CH1, duty_1);
    pwm_duty_updata(PWM_TIM, LEFT_MOTOR_CH3, duty_2);
}

/* 
@brief		    右电机pwm控制
@param		    void
@return		    void
@note           v1.0    
@since		    v1.0    2021.10.7
Sample usage:	
*/

void change_right_duty_motor(int32 duty)//, uint8 compensate)
{
    uint32 duty_1 = 0;
    uint32 duty_2 = 0;

    // if(compensate)
    // {
    //     if(duty >0)
    //     {
    //         duty = duty + g_right_motor_bias;
    //     }
    //     if(duty >0)
    //     {
    //         duty = duty - g_right_motor_bias;
    //     }
    // }
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
    pwm_duty_updata(PWM_TIM, RIGHT_MOTOR_CH2, duty_1);
    pwm_duty_updata(PWM_TIM, RIGHT_MOTOR_CH4, duty_2);
}
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
        //change_duty_motor(MOTOR);
        change_left_duty_motor(MOTOR);//, 0);
        change_right_duty_motor(MOTOR);
    }
    else // off
    {
        //change_duty_motor(0);
        change_left_duty_motor(0);//, 0);
        change_right_duty_motor(0);
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

void servo_output(uint8 flag)
{
    if(flag == DISABLE)
    {
        uint32 real_servo_pwm = SERVO_MIDDUTY;
        
        Current_error += g_image_err;//计算积分
        //积分限幅
        Current_error = Current_error > SERVO_I_LIMIT? SERVO_I_LIMIT:Current_error;
        Current_error = Current_error < -SERVO_I_LIMIT? -SERVO_I_LIMIT:Current_error;
        real_servo_pwm -= SERVO_P*g_image_err         //比例P
                        + SERVO_I*Current_error       //积分I
                        + SERVO_D*(g_image_err - Last_error);  //微分D
        Last_error =    g_image_err;

        if(real_servo_pwm >=SERVO_DUTYMAX)
            real_servo_pwm = SERVO_DUTYMAX;
        else if(real_servo_pwm <= SERVO_DUTYMIN)
            real_servo_pwm = SERVO_DUTYMIN;

        if(g_servo == MOTOR_ON)
        {
            pwm_duty_updata(SERVO_MOTOR_TIM, SERVO_MOTOR_PWM, real_servo_pwm);
        }
        else
        {
            pwm_duty_updata(SERVO_MOTOR_TIM, SERVO_MOTOR_PWM, SERVO_MIDDUTY);//SERVO_MOTOR_DUTY(SERVO_MIDDUTY));
        }
    }
    else if(flag == ENABLE)
    {
        if(g_servo == MOTOR_ON)
        {
            pwm_duty_updata(SERVO_MOTOR_TIM, SERVO_MOTOR_PWM, SERVO);
        }
        else
        {
            pwm_duty_updata(SERVO_MOTOR_TIM, SERVO_MOTOR_PWM, SERVO_MIDDUTY);//SERVO_MOTOR_DUTY(SERVO_MIDDUTY));
        }
    }
}
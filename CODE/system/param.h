#ifndef _PARAM_H
#define _PARAM_H

#include "common.h"
#include "general.h"
#include "imparam.h"

typedef struct
{
    void* buff;
    uint8* name;
    uint8 type;
}Varible;

typedef struct 
{
    float x;
    float y;
    float z;
}GYRO_STRUCT;

typedef struct 
{
    int16 x;
    int16 y;
    int16 z;
}ACC_STRUCT;

typedef struct
{
  float pitch; 
  float yaw; 
  float roll;
}Euler_struct;

typedef struct
{
  float acc_angle;
  float gyro_angle;
}Pitch_struct;

extern Euler_struct g_euler;
extern Pitch_struct g_pitch;
extern ACC_STRUCT g_acc_filter;
extern GYRO_STRUCT g_gyro_filter;
extern GYRO_STRUCT g_gyro_bias;

extern float g_gyro_min;

extern uint16 g_rotate_record_enable;

extern float g_angular_speed;

extern float g_speed_p;
extern float g_speed_i;
extern float g_speed_i_max;
extern float g_speed_i_err;
extern float g_r_speed;
extern float g_l_speed;
extern float g_car_speed;
extern float g_speed_set;

//extern uint8 g_motor;
extern int16 g_left_motor_bias;
extern int16 g_right_motor_bias;

extern float g_cur_distance;

extern uint32 g_time;         //用到了
extern uint32 g_clock;

extern uint8 g_button_value;

extern uint8 g_page;
extern uint16 g_page_value;

extern uint16 g_send;

extern uint16 g_signal[5];
extern float s_y_l_now;
extern float s_x_l_now;
extern float s_x_m_now;
extern float s_x_r_now;
extern float s_y_r_now;
extern float s_x_now;
extern float s_y_now;
extern float g_signal_cali;
extern int16 g_inductor_filter_width;
extern float g_signal_p;
extern float g_signal_d;

extern uint16 g_guide;

extern uint16 g_beep;
extern uint16 g_beep_status;
extern uint16 g_beep_set;

extern uint16 g_mode;

#endif
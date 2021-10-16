#include "param.h"
#include "common.h"

// 角度值
Euler_struct g_euler;
Pitch_struct g_pitch;
ACC_STRUCT g_acc_filter;
GYRO_STRUCT g_gyro_filter;
GYRO_STRUCT g_gyro_bias;

// 角速度过滤阈值
float g_gyro_min = 4.00;

// 转向角度积分
uint16 g_rotate_record_enable = DISABLE;
// 转向角速度
float g_angular_speed = 0.0;

// 速度PI参数
float g_speed_p = 0.0;
float g_speed_i = 0.0;
float g_speed_i_max = 10000.0;
float g_speed_i_err = 0.0;
// 左轮速度
float g_r_speed = 0.0;
// 右轮速度
float g_l_speed = 0.0;
// 当前车速
float g_car_speed = 0.0;
// 期望速度
float g_speed_set = 0.0;

// 电机使能
//uint8 g_motor = MOTOR_OFF;
// 电机死区补偿
int16 g_left_motor_bias = 0;
int16 g_right_motor_bias = 0;

// 运行距离
float g_cur_distance = 0.0;

// 系统时间
uint32 g_time = 0;
// 计时器时钟
uint32 g_clock = 0;

// 按键值
uint8 g_button_value = 0;

// 页面使能
uint16 g_page = ENABLE;
// 页面值
uint16 g_page_value = 0;

// 调试发送功能
uint16 g_send = 1;

// 电磁原始值
uint16 g_signal[5] = {0};
// 电磁归一化值
float s_y_l_now = 0.0;
float s_x_l_now = 0.0;
float s_x_m_now = 0.0;
float s_x_r_now = 0.0;
float s_y_r_now = 0.0;
float s_x_now = 0.0;
float s_y_now = 0.0;
// 电磁归一化参数
float g_signal_cali = 1.0;
// 电磁滤波器宽度
int16 g_inductor_filter_width = 25;
// 电磁PD
float g_signal_p = 0.0;
float g_signal_d = 0.0;

// 引导方式
uint16 g_guide = GUIDE_CAMERA;

// 蜂鸣器
uint16 g_beep = ENABLE;
uint16 g_beep_status = 0;
uint16 g_beep_set = 0;

// 运行模式（调试模式、比赛模式）
uint16 g_mode = 1;
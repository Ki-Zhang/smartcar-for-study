#ifndef _PAGE_H
#define _PAGE_H

#include "common.h"

#define PAGE_NUMBER 5       //最大页数
#define VARIABLE_EACH 8     //每一页的变量数量
#define UINT32_MAXBIT 9    //32位无符号数最大位数
#define MOTOR_EACH  1000   //每步加的pwm值

extern uint8 a[UINT32_MAXBIT];
extern float SERVO;
extern int32 MOTOR;

void welcome(void);
void page_start(void);
void show_star(void);
void flag_page_control(void);
void flag_pwm_control(void);
void page_control(void);
void page(uint8 num, const int8 no_first);
void pwm_start(void);
void button_to_flag(void);

#endif
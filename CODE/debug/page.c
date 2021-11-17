#include "page.h"
#include "pwm.h"
#include "lcd.h"
#include "SEEKFREE_18TFT.h"
#include "common.h"
#include "general.h"
#include "zf_systick.h"
#include "button.h"
#include "param.h"
#include "zf_flash.h"
#include "flash.h"
#include "zf_systick.h"
#include "zf_gpio.h"
#include "image.h"

static uint8 CLEAR_ALL = 0x11;              //第四位清屏标志，高四位清除flash
static uint8 flag = 0;                      //承接键值

/*MODE = 0 初始化调参页面
MODE = 1 进入具体页面的调试
MODE = 2 进入PWM调试
MODE = 3 电机调试
MODE = 4 舵机调试
MODE = 5 图像开启
MODE = 6 小车开跑
MODE = 7 小车带图像跑
MODE = 8 小车不带图像跑*/
static uint8 MODE = 0;                      //模式的选择

static uint8 page_select = 0x00;            //{具体的选择界面,是否为激发状态}
static uint8 item = 0x00;                   //{具体的变量选择,变量的位}


/*参数表
RUN:    {SERVO_P, SERVO_I, SERVO_D, MOTOR, SERVO, g_motor, g_servo, 0}
IMA:
TRA:
*/
static uint32 VARIABLE[PAGE_NUMBER][VARIABLE_EACH] =    {{8, 0, 30, 8200, SERVO_MIDDUTY, 0, 0, 130},
                                                        {0, 0, 0, 0, 0, 0, 0, 0},
                                                        {0, 0, 0, 0, 0, 0, 0, 0},
                                                        {0, 0, 0, 0, 0, 0, 0, 0},
                                                        {0, 0, 0, 0, 0, 0, 0, 0},};

uint8 a[UINT32_MAXBIT] = {0};            //装32位无符号数的各个位置


//debug program
//static uint8 read_bit = 0;


/*
@author:	KD
@version:	v1.0 	2021.10.3 	for flow light
			v1.1	2021.10.5	for flow lignt
            v2.0    2021.10.6   for test2
            v3.0    2021.11.4   for exam
@note		v1.1版本中增加了中断处理模式，采用按键中断处理
            v2.0版本中将flag转化单独设立成函数，用以记录g_button_value的数值
                并且采用flag控制相应的调试窗口界面,另外为了减小程序的内存存储
                采用位运算操作，其中两个变量的说明：
                page_select为"具体的选择界面(0 -- PAGE_NUMBER-1)"和"是否为激发(debug)状态(1/0)"两者结合
                item为"具体的变量选择(0 -- VARIABLE-1)"和"变量具体的位(0 -- UINT32_MAXBIT-1)"两者结合
            v3.0写进Flash,并从flash中读出数值
*/



/* 
@brief		    开机欢迎
@param		    void
@return		    void
@note           
@since		    v1.0    2021.10.3
Sample usage:	
*/

void welcome(void)
{
    lcd_showstr(8, 3, "welcome!");//, YELLOW);
	systick_delay_ms(3000);
    lcd_clear(WHITE);
}

/* 
@brief		    flag控制函数
@param		    void
@return		    void
@note           用于接收按键的值并只接收一次
                改名
@since		    v2.0    2021.10.5
                v2.1    2021.10.6
Sample usage:	
*/

void button_to_flag(void)
{
    switch (g_button_value)
    {
        case UP:                                    //当up键短按
            if(gpio_get(UP_PIN) == 1)               //按键松开
            {
                g_button_value = VALUE_NONE;
                flag = UP;
            }
            break;
        case UP_QUICK:                              //当up键长按
            if(gpio_get(UP_PIN) == 1)               //按键松开
            {
                g_button_value = VALUE_NONE;
                flag = UP_QUICK;
            }
            break;
        case DOWN:                                  //当down键短按
            if(gpio_get(DOWN_PIN) == 1)             //按键松开
            {
                g_button_value = VALUE_NONE;
                flag = DOWN;
            }
            break;
        case DOWN_QUICK:                            //当down键长按
            if(gpio_get(DOWN_PIN) == 1)             //按键松开
            {
                g_button_value = VALUE_NONE;
                flag = DOWN_QUICK;
            }
            break;
        case CHOOSE:                                //当choose键短按
            if(gpio_get(CHOOSE_PIN) == 1)           //按键松开
            {
                g_button_value = VALUE_NONE;
                flag = CHOOSE;
            }
            break;
        case PWM_MODE:                              //当choose键长按
            if(gpio_get(CHOOSE_PIN) == 1)           //按键松开
            {
                g_button_value = VALUE_NONE;
                flag = PWM_MODE;
            }
            break;
        case RETURN:                              //当choose键长按
            if(gpio_get(RETURN_PIN) == 1)           //按键松开
            {
                g_button_value = VALUE_NONE;
                flag = RETURN;
            }
            break;
        case IMAGE:                                 //当choose键长按
            if(gpio_get(RETURN_PIN) == 1)           //按键松开
            {
                g_button_value = VALUE_NONE;
                flag = IMAGE;
            }
            break;
        case VALUE_NONE:                              //无键值
            
            break;
        default:
            g_button_value = VALUE_NONE;
            flag = 0;
            lcd_clear(WHITE);
            my_lcd_showstr(0, 0, "wrong in", RED);
            my_lcd_showstr(0, 1, "button_to_flag", RED);
            my_lcd_showstr(0, 0, "/page.c", RED);
            break;
    }
}

/* 
@brief		    LCD入口函数
@param		    void
@return		    void
@note           
@since		    v1.0    2021.10.3
                v2.0    2021.10.6
Sample usage:	
*/

void page_start(void)
{
    //一堆page初始化的事情
    if(CLEAR_ALL == 1)                      //clear the screen
    {
        lcd_clear(WHITE);
        CLEAR_ALL = 0;
    }
    else if(CLEAR_ALL == 0x11)              //when first run, init the flash, later if want to add more,just in this section
    {
        flash_page_program(FLASH_SECTION, FLASH_PAGE, VARIABLE[0], VARIABLE_EACH);
        flash_page_program(FLASH_SECTION, FLASH_PAGE_1, VARIABLE[1], VARIABLE_EACH);
        flash_page_program(FLASH_SECTION, FLASH_PAGE_2, VARIABLE[2], VARIABLE_EACH);
        flash_page_program(FLASH_SECTION, FLASH_PAGE_3, VARIABLE[3], VARIABLE_EACH);
        flash_page_program(FLASH_SECTION_127, FLASH_PAGE, VARIABLE[4], VARIABLE_EACH);
        flash_page_program(FLASH_SECTION_127, FLASH_PAGE_1, VARIABLE[5], VARIABLE_EACH);
        lcd_clear(WHITE);
        CLEAR_ALL = 0;
        g_page = ENABLE;
    }
    button_to_flag();
    switch (MODE)
    {
        //调参界面开始，每次换页面时触发
        case 0:
        //具体界面的调试
        case 1:
            switch(page_select>>4)              //选择page
            {
                case 0:
                    page(0,"RUN");
                    flash_page_program(FLASH_SECTION, FLASH_PAGE, VARIABLE[0], VARIABLE_EACH);
                    break;
                case 1:
                    page(1,"IMA");
                    flash_page_program(FLASH_SECTION, FLASH_PAGE_1, VARIABLE[1], VARIABLE_EACH);
                    break;
                case 2:
                    page(2,"N");
                    flash_page_program(FLASH_SECTION, FLASH_PAGE_2, VARIABLE[2], VARIABLE_EACH);
                    break;
                case 3:
                    page(3,"O");
                    flash_page_program(FLASH_SECTION, FLASH_PAGE_3, VARIABLE[3], VARIABLE_EACH);
                    break;
                case 4:
                    page(4,"P");
                    flash_page_program(FLASH_SECTION_127, FLASH_PAGE, VARIABLE[4], VARIABLE_EACH);
                    break;
                // case 5:
                //     page(5,"Q");
                //     flash_page_program(FLASH_SECTION_127, FLASH_PAGE_1, VARIABLE[5], VARIABLE_EACH);
                //     break;
                default:
                    lcd_clear(WHITE);
                    my_lcd_showstr(0, 0, "only 6 pages,forget", RED);
                    my_lcd_showstr(0, 1, "add in page_control", RED);
                    systick_delay_ms(3000);
                    break;
            }
            break;
        //pwm初始界面
        case 2:
            my_lcd_showstr(0, 0, "now in pwm_start", GREEN);
            my_lcd_showstr(0, 1, "choose:", GREEN);
            my_lcd_showstr(0, 2, "UP: motor", GREEN);
            my_lcd_showstr(0, 3, "DOWN: servo", GREEN);
            flag_pwm_control();
            break;
        //motor界面的调试
        case 3:
            my_lcd_showstr(0, 0, "now in motor", GREEN);
            my_lcd_showstr(0, 1, "duty:", GREEN);
            my_lcd_showint32(60, 1, (int32) MOTOR, 0, 0);
            my_lcd_showstr(0, 3, "UP for add", BLUE);
            my_lcd_showstr(0, 4, "DOWN for sub", BLUE);
            my_lcd_showstr(0, 5, "g_motor:", BLUE);
            my_lcd_showint16(60, 5, (int16) g_motor, 0, 0);
            flag_pwm_control();
            break;
        //servo界面的调试
        case 4:
            my_lcd_showstr(0, 0, "now in servo", GREEN);
            my_lcd_showstr(0, 1, "duty:", GREEN);
            my_lcd_showint16(60, 1, (int16) SERVO, 0, 0);
            my_lcd_showstr(0, 3, "UP for add", BLUE);
            my_lcd_showstr(0, 4, "DOWN for sub", BLUE);
            my_lcd_showstr(0, 5, "g_servo:", BLUE);
            my_lcd_showint16(60, 5, (int16) g_servo, 0, 0);
            flag_pwm_control();
            break;
        //图像调试
        case 5:
            //my_lcd_displayimage_binary(mt9v03x_image[0][0], MT9V03X_W, MT9V03X_H);
            my_lcd_displayimage_debug(&g_watch[0][0], &g_image[0][0], MT9V03X_W, MT9V03X_H);
            lcd_showint16(0, 0, (int16)g_thres_value);  //显示阈值
            lcd_showint16(0, 1, (int16)g_image_err);    //显示误差
            if(flag == IMAGE)
            {
                CLEAR_ALL = 1;
                MODE = 0;
                g_image_enable = DISABLE;
                flag = VALUE_NONE;
            }
            break;
        //图像选择界面
        case 6:
            my_lcd_showstr(0, 0, "choose run mode", BLUE);
            my_lcd_showstr(0, 1, "UP:with image", GREEN);
            my_lcd_showstr(0, 2, "DOWN:no image", GREEN);
            if (flag == UP)
            {
                MODE = 7;
                CLEAR_ALL = 1;
                g_image_enable = ENABLE;
                SERVO_P = (float)VARIABLE[0][0];
                SERVO_I = (float)VARIABLE[0][1];
                SERVO_D = (float)VARIABLE[0][2];
                MOTOR = (int32)VARIABLE[0][3];
                g_servo = ENABLE;
                g_motor = ENABLE;
                g_page = DISABLE;
                flag = VALUE_NONE;
            }
            else if(flag == DOWN)
            {
                MODE = 8;
                CLEAR_ALL = 1;
                g_image_enable = DISABLE;
                SERVO_P = (float)VARIABLE[0][0];
                SERVO_I = (float)VARIABLE[0][1];
                SERVO_D = (float)VARIABLE[0][2];
                MOTOR = (int32)VARIABLE[0][3];
                g_servo = ENABLE;
                g_motor = ENABLE;
                g_page = DISABLE;
                flag = VALUE_NONE;
            }
            else if(flag == RETURN)
            {
                MODE = 0;
                CLEAR_ALL = 1;
                g_image_enable = DISABLE;
                g_motor = DISABLE;
                g_servo = DISABLE;
                g_page = ENABLE;
                flag = VALUE_NONE;
            }
            break;
        //带图像跑
        case 7:
            my_lcd_displayimage_debug(&g_watch[0][0], &g_image[0][0], MT9V03X_W, MT9V03X_H);
            lcd_showint16(0, 0, (int16)g_image_err);    //显示误差
            if(flag == RETURN)
            {
                MODE = 0;
                CLEAR_ALL = 1;
                g_image_enable = DISABLE;
                g_motor = DISABLE;
                g_servo = DISABLE;
                g_page = ENABLE;
                flag = VALUE_NONE;
            }
            break;
        //不带图像跑
        case 8:
            if(flag == RETURN)
            {
                MODE = 0;
                CLEAR_ALL = 1;
                g_image_enable = DISABLE;
                g_motor = DISABLE;
                g_servo = DISABLE;
                g_page = ENABLE;
                flag = VALUE_NONE;
            }
            break;
        default:
            break;
    }

    // if(MODE == 7)
    // {
    //     button_to_flag();
    //     if(flag == )
    //     {

    //     }
    //     return;
    // }
    // else if(MODE == 6)
    // {
    //     g_image_enable = ENABLE;
    //     //my_lcd_displayimage_binary(&g_image[0][0], MT9V03X_W, MT9V03X_H);
    //     my_lcd_displayimage_debug(&g_watch[0][0], &g_image[0][0], MT9V03X_W, MT9V03X_H);
    //     lcd_showint16(0, 0, (int16)g_thres_value);
    // }


    // else if(MODE == 5)   //camera mode
    // {
    //     g_image_enable = ENABLE;
    //     //my_lcd_displayimage_binary(&g_image[0][0], MT9V03X_W, MT9V03X_H);
    //     my_lcd_displayimage_debug(&g_watch[0][0], &g_image[0][0], MT9V03X_W, MT9V03X_H);
    //     lcd_showint16(0, 0, (int16)g_thres_value);
    //     if(flag == RETURN)
    //     {
    //         flag = VALUE_NONE;
    //         MODE = 0;
    //         CLEAR_ALL += 1;
    //         g_image_enable = DISABLE;
    //     }
    //     //return;
    // }
    // else if(MODE == 0 || MODE == 2)              //参数模式
    // {
    //     page_control();
    // }
    // else if(MODE == 1||MODE == 3||MODE == 4)           //参数模式
    // {
    //     pwm_start();
    // }
    //else if(MODE = 5||MODE == 6)            //舵机
}

/* 
@brief		    界面跳转选择函数
@param		    void
@return		    void
@note           
@since		    v2.0    2021.10.6
Sample usage:	
*/

// void page_control(void)
// {
//     if(flag == PWM_MODE)                    //切换pwm模式
//     {
//         MODE = 1;                           //pwm界面
//         flag = VALUE_NONE;                  //将flag的值清零
//         CLEAR_ALL = 1;
//         return ;
//     }
//     else if(flag == IMAGE)                  //切换图像调试模式
//     {
//         MODE = 5;                           //pwm界面
//         flag = VALUE_NONE;                  //将flag的值清零
//         CLEAR_ALL = 1;
//         return ;
//     }
//     else
//     {
//         switch(page_select>>4)              //选择page
//         {
//         case 0:
//             page(0,'RUN');
//             flash_page_program(FLASH_SECTION, FLASH_PAGE, RUN, VARIABLE_EACH);
//             break;
//         case 1:
//             page(1,'IMA');
//             flash_page_program(FLASH_SECTION, FLASH_PAGE_1, IMA, VARIABLE_EACH);
//             break;
//         case 2:
//             page(2,'N');
//             flash_page_program(FLASH_SECTION, FLASH_PAGE_2, IMA, VARIABLE_EACH);
//             break;
//         case 3:
//             page(3,'O');
//             flash_page_program(FLASH_SECTION, FLASH_PAGE_3, IMA, VARIABLE_EACH);
//             break;
//         case 4:
//             page(4,'P');
//             flash_page_program(FLASH_SECTION_127, FLASH_PAGE, IMA, VARIABLE_EACH);
//             break;
//         case 5:
//             page(5,'Q');
//             flash_page_program(FLASH_SECTION_127, FLASH_PAGE_1, IMA, VARIABLE_EACH);
//             break;
//         default:
//             lcd_clear(WHITE);
//             my_lcd_showstr(0, 0, "only 6 pages,forget", RED);
//             my_lcd_showstr(0, 1, "add in page_control", RED);
//             systick_delay_ms(3000);
//             break;
//         }
//     }
// }

/* 
@brief		    pwm调试
@param		    num             //第几个界面
@param		    no_first        //第一个变量的名字
@return		    void
@note           
@since		    v2.0    2021.10.6
Sample usage:	
*/

// void pwm_start(void)
// {
//     if(MODE == 1)
//     {
//         my_lcd_showstr(0, 0, "now in pwm_start", GREEN);
//         my_lcd_showstr(0, 1, "choose:", GREEN);
//         my_lcd_showstr(0, 2, "D0: motor", GREEN);
//         my_lcd_showstr(0, 3, "D1: servo", GREEN);
//     }
//     else if(MODE == 3)
//     {
//         my_lcd_showstr(0, 0, "now in motor", GREEN);
//         my_lcd_showstr(0, 1, "duty:", GREEN);
//         my_lcd_showint32(60, 1, (int32) MOTOR, 0, 0);
//         my_lcd_showstr(0, 3, "UP(D0) for add", BLUE);
//         my_lcd_showstr(0, 4, "DOWN(D1) for sub", BLUE);
//         my_lcd_showstr(0, 5, "g_motor:", BLUE);
//         my_lcd_showint16(60, 5, (int16) g_motor, 0, 0);
//     }
//     else if(MODE == 4)
//     {
//         my_lcd_showstr(0, 0, "now in servo", GREEN);
//         my_lcd_showstr(0, 1, "duty:", GREEN);
//         my_lcd_showint16(60, 1, (int16) SERVO, 0, 0);
//         my_lcd_showstr(0, 3, "UP(D0) for add", BLUE);
//         my_lcd_showstr(0, 4, "DOWN(D1) for sub", BLUE);
//         my_lcd_showstr(0, 5, "g_servo:", BLUE);
//         my_lcd_showint16(60, 5, (int16) g_servo, 0, 0);
//         //my_lcd_showstr(0, 5, ":", BLUE);
//         //my_lcd_showint16(60, 5, (int16) SERVO_MOTOR_DUTY(SERVO), 0, 0);
//     }
//     if(flag == PWM_MODE)        //切换为调参模式
//     {
//         MODE = 0;
//         CLEAR_ALL = 1;
//         flag = VALUE_NONE;
//         return;
//     }
//     else if(flag == IMAGE)                  //切换图像调试模式
//     {
//         MODE = 5;                           //pwm界面
//         flag = VALUE_NONE;                  //将flag的值清零
//         CLEAR_ALL = 1;
//         return ;
//     }
//     flag_pwm_control();
// }

/* 
@brief		    flag控制pwm界面模块
@param		    void
@return		    void
@note           
@since		    v2.0    2021.10.6
Sample usage:	
*/

void flag_pwm_control(void)
{
    switch(flag)
    {
        case UP:
            if(MODE == 2)//初始PWM->电机模式
            {
                MODE = 3;
                CLEAR_ALL = 1;          //清屏
            }
            else if(MODE == 3)//电机调试
            {
                if(MOTOR >= MOTOR_DUTYMAX)//控制最大
                {
                    MOTOR = MOTOR_DUTYMAX;
                }
                else
                {
                    MOTOR += MOTOR_EACH;
                }
            }
            else if(MODE == 4)//舵机调试
            {
                if(SERVO >= SERVO_DUTYMAX)//控制最大
                {
                    SERVO = SERVO_DUTYMAX;
                }
                else
                {
                    SERVO += SERVO_EACH;
                }
            }
            flag = VALUE_NONE;
            break;
        case DOWN:
            if(MODE == 2)//初始PWM->舵机调试
            {
                MODE = 4;
                CLEAR_ALL = 1;          //清屏
            }
            else if(MODE == 3)
            {
                if(MOTOR <= -MOTOR_DUTYMAX)//控制最小
                {
                    MOTOR = -MOTOR_DUTYMAX;
                }
                else
                {
                    MOTOR -= MOTOR_EACH;
                }
            }
            else if(MODE == 4)//舵机调试
            {
                if(SERVO <= SERVO_DUTYMIN)//控制最小
                {
                    SERVO = SERVO_DUTYMIN;
                }
                else
                {
                    SERVO -= SERVO_EACH;
                }
            }
            flag = VALUE_NONE;
            break;
        case CHOOSE:                                        //进入或退出debug模式
            if(MODE == 3)
            {
                g_motor = 1 - g_motor;
            }
            else if(MODE == 4)
            {
                g_servo = 1 - g_servo;
            }
            flag = VALUE_NONE;
            break;
        case PWM_MODE:                      //pwm page -> parameter page
            MODE = 0;
            VARIABLE[0][5] = g_motor;
            VARIABLE[0][6] = g_servo;
            VARIABLE[0][3] = (uint32)MOTOR;
            flash_page_program(FLASH_SECTION, FLASH_PAGE, VARIABLE[0], VARIABLE_EACH);
            CLEAR_ALL = 1;                      //清屏
            flag = VALUE_NONE;
            break;
        case RETURN:                        //servo/motor -> pwm page
            MODE = 2;
            CLEAR_ALL = 1;                  //清屏
            flag = VALUE_NONE;
            break;
        case IMAGE:                         //图像调试
            MODE = 5;
            CLEAR_ALL = 1;                      //清屏
            g_image_enable = ENABLE;
            flag = VALUE_NONE;
            break;
        default:
            flag = VALUE_NONE;                                  //flag清零
            break;
    }
}

/* 
@brief		    界面显示
@param		    num             //第几个界面
@param		    no_first        //第一个变量的名字
@return		    void
@note           
@since		    v2.0    2021.10.5
                v2.1    2021.10.6
Sample usage:	
*/

void page(uint8 num, const int8 *no_first)
{
    uint8 i = 0;                                                //for circulation

    if(MODE == 0)                                               //将存进flash对应的数据读出，且只读一次
    {
        if(num <= 3)
        {
            flash_page_read(FLASH_SECTION, FLASH_PAGE + num, VARIABLE[num], VARIABLE_EACH);
        }
        else if(num < PAGE_NUMBER)
        {
            flash_page_read(FLASH_SECTION_127, FLASH_PAGE + num - 4, VARIABLE[num], VARIABLE_EACH);
        }
        MODE = 1;
        //打印出变量列表
        for(i = 0; i < 8; i++)
        {
            my_lcd_showstr(0, i, no_first, GREEN);          //给每个变量边上字母
            my_lcd_showchar(25, i*16, '0' + i, GREEN);           //给每个变量编上号码
            my_lcd_showint32(68, i, (int32)VARIABLE[num][i], 0, 0);      //打印出变量
        }
    }

    flag_page_control(num);
}


/* 
@brief		    flag控制参量界面模块
@param		    void
@return		    void
@note           
@since		    v2.0    2021.10.6
Sample usage:	
*/

void flag_page_control(uint8 num)
{
    uint8 bit_select = item & 0x0f;                         //选择具体的位
    uint8 item_select = item >> 4;                          //选择具体的项目

    my_lcd_showchar(40, item_select*16, '-', BLUE);
    my_lcd_showchar(49, item_select*16, '>', BLUE);
    switch (flag)
    {
        case UP:
            if(page_select & 1 == 1)                        //处于debug状态
            {
                my_lcd_showint32(70, item_select, (int32)VARIABLE[num][item_select], 1, bit_select);
                if(a[bit_select] == 9)                      //位于最大
                {
                    a[bit_select] = 0;
                }
                else
                {
                    a[bit_select] ++;
                }
                VARIABLE[num][item_select] = convert_str_to_32(a);
                my_lcd_showint32(70, item_select, (int32)VARIABLE[num][item_select], 1, bit_select);
                //read_bit = temp[item_select];              /***************debug*********/
            }
            else                                            //不处于debug状态
            {
                if(item_select == 0)                        //最小的项目
                {
                    if(page_select >> 4 == 0)               //最小的页码
                    {
                        page_select += ((PAGE_NUMBER - 1) << 4);        //回到最大页
                    }
                    else
                    {
                        page_select -= 1 << 4;              //减少一页
                    }
                    item += (VARIABLE_EACH - 1) << 4;       //回到最大项
                    CLEAR_ALL = 1;                          //清屏换页
                    MODE = 0;                               //更新页
                    flag = VALUE_NONE;                      //flag清屏
                    return;
                }
                else                                        //清除"->"标识
                {
                    my_lcd_showchar(40, item_select*16, ' ', WHITE);
                    my_lcd_showchar(49, item_select*16, ' ', WHITE);
                    item -= 1<<4;                           //减少一个项目
                }
            }
            flag = VALUE_NONE;
            break;
        case UP_QUICK:
            if(page_select & 1 == 1)                        //处于debug模式
            {
                if(bit_select < UINT32_MAXBIT - 1)
                {
                    item ++;
                }
                else
                {
                    item &= 0xf0;
                }
                bit_select = item&0x0f;
                my_lcd_showint32(70, item_select, (int32)VARIABLE[num][item_select], 1, bit_select);
            }
            else                                            //不处于debug模式，换页
            {
                if(page_select >> 4 == 0)                   //最小的页码
                {
                    page_select += (PAGE_NUMBER - 1) << 4;  //回到最大页
                }
                else
                {
                    page_select -= 1 << 4;                  //减少一页
                }
                item = 0x00;                                //回到第一项
                CLEAR_ALL = 1;                              //清屏
                MODE = 0;                                   //更新页
                flag = VALUE_NONE;                          //flag清零
                return;
            }
            flag = VALUE_NONE;
            break;
        case DOWN:
            if(page_select & 1 == 1)                        //处于debug状态
            {
                my_lcd_showint32(70, item_select, (int32)VARIABLE[num][item_select], 1, bit_select);
                if(a[bit_select] == 0)                      //位于最小
                {
                    a[bit_select] = 9;
                    //read_bit = a[bit_select];               /***************debug*********/
                }
                else
                {
                    a[bit_select] --;
                    //read_bit = a[bit_select];               /***************debug*********/
                }
                VARIABLE[num][item_select] = convert_str_to_32(a);
                my_lcd_showint32(70, item_select, (int32)VARIABLE[num][item_select], 1, bit_select);
                //read_bit = temp[item_select];              /***************debug*********/
            }
            else                                            //不处于debug状态
            {
                if(item_select == VARIABLE_EACH - 1)        //最大的项目，换页
                {
                    if(page_select >> 4 == PAGE_NUMBER - 1) //最大的页码
                    {
                        page_select &= 0x0f;                //回到最小页
                    }
                    else
                    {
                        page_select += 1 << 4;              //增大一页
                    }
                    item &= 0x0f;                           //回到最小项
                    CLEAR_ALL = 1;                          //清屏
                    MODE = 0;                               //更新页
                    flag = VALUE_NONE;                      //flag清零
                    return;
                }
                else                                        //清除->标识
                {
                    my_lcd_showchar(40, item_select*16, ' ', WHITE);
                    my_lcd_showchar(49, item_select*16, ' ', WHITE);
                    item += 1<<4;
                }
            }
            flag = VALUE_NONE;                              //flag清零
            break;
        case DOWN_QUICK:
            if(page_select & 1 == 1)
            {
                if(bit_select > 0)
                {
                    item --;
                }
                else
                {
                    item += UINT32_MAXBIT - 1;
                }
                my_lcd_showint32(70, item_select, (int32)VARIABLE[num][item_select], 1, bit_select);
            }
            else
            {
                if(page_select >> 4 == PAGE_NUMBER - 1)     //最大的页码
                {
                    page_select &= 0x0f;                    //回到最小页
                }
                else
                {
                    page_select += 1 << 4;                  //增加一页
                }
                item = 0x00;                                //回到第一项
                CLEAR_ALL = 1;                              //清屏
                MODE = 0;                                   //更新页
                flag = VALUE_NONE;                          //flag清零
                return;
            }
            flag = VALUE_NONE;
            break;
        case CHOOSE:                                        //进入或退出debug模式
            if(page_select & 1 == 1)                        //原来就在debug模式
            {
                page_select --;
                my_lcd_showchar(58, item_select*16, ' ', BLUE);
                my_lcd_showint32(70, item_select, (int32)VARIABLE[num][item_select], 0, 0);
                VARIABLE[num][item_select] = convert_str_to_32(a);
            }
            else                                            //原来不在debug模式
            {
                page_select ++;
                my_lcd_showint32(70, item_select, (int32)VARIABLE[num][item_select], 1, bit_select);
                my_lcd_showchar(58, item_select*16, '*', BLUE);
                convert_32_to_str(VARIABLE[num][item_select], a);
            }
            flag = VALUE_NONE;
            break;
        case PWM_MODE:
            MODE = 2;
            CLEAR_ALL = 1;              //清屏
            flag = VALUE_NONE;
            break;
        case GO_FOR_RUN:                //小车跑
            MODE = 6;
            CLEAR_ALL = 1;
            flag = VALUE_NONE;
            g_page = DISABLE;
            break;
        case IMAGE:
            MODE = 5;
            CLEAR_ALL = 1;                      //清屏
            //g_thres_value = VARIABLE[0][7];
            g_image_enable = ENABLE;
            flag = VALUE_NONE;
            break;
        default:
            flag = VALUE_NONE;                                  //flag清零
            break;
    }
}
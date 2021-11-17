#include "imparam.h"

// 图像使能
uint16 g_image_enable = DISABLE;
// 图像
uint8 (*g_image)[MT9V03X_W];
uint8 g_watch[MT9V03X_H][MT9V03X_W] = { 0 };
// 路宽参数
float g_road_width_bias = 27.0;
float g_road_width_ratio = 1.19;
// 图像误差
float g_image_err = 0.0;
// 图像PD
float g_image_err_p = 8.80;
float g_image_err_d = 16.6;
// 阈值
int16 g_differ_threshold1 = 15;
int16 g_differ_threshold2 = 5;
uint16 g_thres_min = 80;
uint16 g_thres_max = 160;
uint16 g_thres_value = 200;
uint8 g_thres_table[256] = {0};
// 前瞻
uint16 g_useful_row = 70;
// 图像误差限幅
int16 g_camera_limit = 0;
float g_camera_limit_value = 0.0;
float g_camera_limit_high = 0.0;
float g_camera_limit_low = 0.0;
// 左右边界
uint16 g_left_count = 0;
uint16 g_right_count = 0;
uint16 g_left_line[IMAGE_HEIGHT];
uint16 g_right_line[IMAGE_HEIGHT];
// 图像权重参数
float g_image_err_bias = 100.0;
float g_image_err_ratio = -0.12;
uint16 g_min_weight = 5;
// 全白行
uint16 g_allwhite_n = 0;
uint16 g_allwhite_y = 0;
// 最长白列
uint16 g_whiteline_n = 0;
uint16 g_whiteline_x = 0;
// 相关系数
float g_left_correlation = 0;
float g_right_correlation = 0;
float g_correlation_thres = 0.98;
// otsu 使能
uint16 g_otsu = ENABLE;
// search 
uint16 g_search_top = SEARCH_TOP;
uint16 g_search_top_max = SEARCH_BOT;
uint16 g_search_top_min = SEARCH_TOP;
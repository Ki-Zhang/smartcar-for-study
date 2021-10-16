#ifndef _IMPARAMETER_H
#define _IMPARAMETER_H

#include "common.h"
#include "general.h"

extern uint8 (*g_image)[MT9V03X_W];
extern uint8 g_watch[MT9V03X_H][MT9V03X_W];

// image
#define IMAGE_HEIGHT    MT9V03X_H
#define IMAGE_WIDTH     MT9V03X_W
// search boundary
#define SEARCH_LEFT     1
#define SEARCH_RIGHT    (IMAGE_WIDTH - 2)
#define SEARCH_BOT      (MT9V03X_H * 3 / 4)
#define SEARCH_TOP      (MT9V03X_H * 1 / 24)

#define G_THRES_MIN     80
#define G_THRES_MAX     200

extern int16 g_differ_threshold1;
extern int16 g_differ_threshold2;
extern uint16 g_image_enable;
extern float g_road_width_bias;
extern float g_road_width_ratio;
extern float g_image_err;
extern float g_image_err_p;
extern float g_image_err_d;
extern uint16 g_thres_min;
extern uint16 g_thres_max;
extern uint16 g_thres_value;
extern uint8 g_thres_table[256];
extern uint16 g_useful_row;
extern uint16 g_left_count;
extern uint16 g_right_count;
extern uint16 g_left_line[IMAGE_HEIGHT];
extern uint16 g_right_line[IMAGE_HEIGHT];

extern float g_image_err_bias;
extern float g_image_err_ratio;
extern uint16 g_allwhite_n;
extern uint16 g_allwhite_y;
extern uint16 g_whiteline_x;
extern uint16 g_whiteline_n;
extern float g_left_correlation;
extern float g_right_correlation;
extern float g_correlation_thres;
extern uint16 g_otsu;
extern uint16 g_search_top;
extern uint16 g_search_top_max;
extern uint16 g_search_top_min;

#endif

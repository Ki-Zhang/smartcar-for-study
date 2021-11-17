#ifndef _IMAGE_H
#define _IMAGE_H

#include "common.h"
#include "param.h"
#include "general.h"
#include "imparam.h"

#define PIXEL_NUM   256
#define BINARY_WHITE    1

// search setting
#define SEARCH_STEP_Y	1
#define SEARCH_STEP_X	1
#define SEARCH_MARGIN   5
#define SEARCH_TRY      3

#define DOWNSAMPLE_X	5
#define DOWNSAMPLE_Y	5
#define DOWNSAMPLE_H	24
#define DOWNSAMPLE_W	38
#define DOWNSAMPLE_N	(DOWNSAMPLE_W * DOWNSAMPLE_H)
#define DOWNSAMPLE_C	1                           // color
#define DOWNSAMPLE_S	256
#define WHITE_TRY       5   //判断全白行的阈值  

extern uint32 start_time;
extern uint32 end_time;

//uint8 my_otsu(void);
uint16 image_fast_otsu(void);
void image_update_thresvalue(void);
uint16 image_find_whiteline(void);
uint8 image_fastsearch_leftline(void);
uint8 image_fastsearch_rightline(void);
void image_find_midline(void);
void image_debug(void);
void image_interpolate(float k, float b, uint16 from, uint16 to, uint16 line_type);
void calcular_err(void);

uint16 image_fast_otsu(void);

#endif
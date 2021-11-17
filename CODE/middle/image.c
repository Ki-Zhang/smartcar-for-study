#include "image.h"
#include "imparam.h"

//#define MAX(a, b) (a > b ? a : b)
//#define MIN(a, b) (a > b ? b : a)
#define ABS(a) (a > 0 ? a : -a)
#define DELT(a, b) (a - b)
#define ABS_DELT(a, b) (a > b ? a - b : b - a)
#define DIFFER(a, b) ((a - b) * 100 / (a + b))
#define FAST_DIFFER(a, b) (((a - b) << 7) / (a + b + 1))
#define ABS_DIFFER(a, b) a >= b ? (((a - b) << 7) / (a + b + 1) \
								   : ((b - a) << 7) / (a + b + 1))
#define LIMITER(val, low, high)	((val)<(low)?(low):((val)>(high)?(high):(val)))

static int16 g_mid_line[IMAGE_HEIGHT];
static uint16 g_err_weight[IMAGE_HEIGHT];
//static uint16 x_conditon_left = 0;
//static uint16 x_conditon_right = 0;
// static uint8 cross_start = 0;
// static uint8 cross_stop = 0;
// static uint8 CROSS = 0;
//for debug
// static uint8 WHITESTART_L = 0;
// static uint8 WHITEEND_L = 0;
// static uint8 WHITESTART_R = 0;
// static uint8 WHITEEND_R = 0;


/* 
@brief		    更行二值化查找表
@param		    void
@return		    void
@note           copy
@since		    v3.0    for image
Sample usage:	
*/

// 更新二值化查找表
void image_update_thresvalue(void)
{
	static uint8 init_flag = 0;
	static uint8 last_threshold_value = 0;
    uint16 i = 0;       //循环变量

	g_thres_value = g_thres_value + 20;
	if (init_flag == 0)
	{
		for (i = 0; i < g_thres_value; i++)
		{
			g_thres_table[i] = 0;
		}
		for (i = g_thres_value; i < 256; i++)
		{
			g_thres_table[i] = 1;
		}
		init_flag = 1;
	}
	else
	{
		if (last_threshold_value < g_thres_value)
		{
			for (i = last_threshold_value; i < g_thres_value; i++)
			{
				g_thres_table[i] = 0;
			}
		}
		else
		{
			for (i = g_thres_value; i < last_threshold_value; i++)
			{
				g_thres_table[i] = 1;
			}
		}
	}
	
	last_threshold_value = g_thres_value;
}

/* 
@brief		    寻找最长白列
@param		    uint16
@return		    void
@note           copy PPT
@since		    v3.0    for image
Sample usage:	
*/

uint16 image_find_whiteline(void)
{
    uint16 white_sum[DOWNSAMPLE_W+1] = {0};
    uint16 white_mid_x = 0; //最长白列位置
    uint16 white_max = 0;   //白线最大长度
    uint16 white_n = 0;     //最长白线数量
    uint8 x = 0;            //从左到右扫描
    uint8 y = 0;            //从下往上搜索
    for(x = 0; x < MT9V03X_W/5; x++)
    {
        uint16 *sum = &white_sum[x];
        uint8* src = &g_image[0][x*DOWNSAMPLE_X+SEARCH_LEFT];
        
        //从下往上搜索白色像素的数量
        for(y = SEARCH_BOT; y >= SEARCH_TOP; y-= DOWNSAMPLE_Y)
        {
            if(g_thres_table[*(src+IMAGE_WIDTH*y)])
            {
                *sum += 1;
            }
            else
            {
                //如果搜到黑色为止，并记录最大值
                break;
            }
        }
        if(*sum > white_max)
        {
            white_max = *sum;
            white_mid_x = x;
            white_n = 1;
        }
        else if(*sum == white_max)
        {
            white_mid_x += x;
            white_n ++;
        }
    }
    white_mid_x = white_mid_x / white_n;
    g_whiteline_n = white_n;
    g_whiteline_x = white_mid_x *DOWNSAMPLE_X + SEARCH_LEFT;
    //
    uint16 search_top = SEARCH_TOP;
    uint8* src = &g_image[0][g_whiteline_x];
    for(y = SEARCH_BOT - white_sum[white_mid_x]*DOWNSAMPLE_Y/2; y >= SEARCH_TOP; y -= SEARCH_STEP_Y)
    {
        //出现第一个黑点
        if(!g_thres_table[*(src+IMAGE_WIDTH*y)])
        {
            //选择上一行
            search_top = y+1;
            break;
        }
    }
    g_search_top = LIMITER(search_top, g_search_top_min, g_search_top_max);
    return g_whiteline_x;
}

/* 
@brief		    类内方差OTSU最佳阈值
@param		    uint16
@return		    void
@note           copy
@since		    v3.0    for image
Sample usage:	
*/

uint16 image_fast_otsu(void)
{
	if (!g_otsu)
	{
		return 0;
	}

	uint16 histogram[DOWNSAMPLE_S] = { 0 };
	uint32 histstd[DOWNSAMPLE_S] = { 0 };

	// 直方图统计
	for (uint16 y = 0; y < IMAGE_HEIGHT; y += DOWNSAMPLE_Y)
	{
		for (uint16 x = SEARCH_LEFT; x <= SEARCH_RIGHT; x += DOWNSAMPLE_X)
		{
			histogram[g_image[y][x] / DOWNSAMPLE_C]++;
		}
	}

	uint16 background_mean = 0;
	uint16 foreground_mean = 0;
	uint32 background_sum = 0;
	uint32 foreground_sum = 0;
	uint16 background_n = 0;
	uint16 foreground_n = 0;
	uint16 i = g_thres_min / DOWNSAMPLE_C;

	// 初始化积分表
	for (uint16 k = 0; k < i; k++)
	{
		background_n += histogram[k];
		background_sum += histogram[k] * k;
	}

	for (uint16 k = i; k < DOWNSAMPLE_S; k++)
	{
		foreground_n += histogram[k];
		foreground_sum += histogram[k] * k;
	}

	// 遍历计算类间方差
	for (; i <= g_thres_max / DOWNSAMPLE_C; i++)
	{
		background_n += histogram[i];
		foreground_n -= histogram[i];

		if (!background_n)
		{
			continue;
		}
		if (!foreground_n)
		{
			break;
		}
		if (!histogram[i])
		{
			histstd[i] = histstd[i - 1];
		}
		
		background_sum += histogram[i] * i;
		foreground_sum -= histogram[i] * i;

		background_mean = background_sum / background_n;
		foreground_mean = foreground_sum / foreground_n;

		histstd[i] = background_n * foreground_n * (int32)(background_mean - foreground_mean) * (int32)(background_mean - foreground_mean);
	}

	uint32 temp = 0x00;
	uint16 thres = 0;
	uint16 thres_n = 0;

	// 寻找方差最小的灰度级，如果有多个则取平均
	for (uint16 i = g_thres_min / DOWNSAMPLE_C; i <= g_thres_max / DOWNSAMPLE_C; i++)
	{
		if (histstd[i])
		{
			if (histstd[i] > temp)
			{
				temp = histstd[i];
				thres = i;
				thres_n = 1;
			}
			else if (histstd[i] == temp)
			{
				thres += i;
				thres_n++;
			}
		}
	}

	static uint16 flag = 0;
	static float last_thres = 0.0;

	// 防止无效结果
	if (thres)
	{
		// 首次进入更新记录阈值
		if (!flag)
		{
			last_thres = thres / (float)thres_n;
			flag = 1;
		}

		last_thres = 0.95 * last_thres + 0.05 * thres / (float)thres_n;

		g_thres_value = (uint16)(last_thres * DOWNSAMPLE_C);
	}

	return g_thres_value;
}

// 搜左线
/*uint16 image_fastsearch_leftline(void)
{
    int16 end_x = SEARCH_LEFT;
    uint16 count = 0;
    uint16 times = 0;
    uint16 try_time = SEARCH_TRY;
    uint16 diff_thres = 0;
    uint16 find_flag = 0x0000;
    uint16 last_x = g_whiteline_x;
    uint16 margin = SEARCH_MARGIN;
    uint8* thres_table = g_thres_table;
    uint16 y = SEARCH_BOT;
    int16 x = 0;

    for(y = SEARCH_BOT; y >= SEARCH_TOP; y -= SEARCH_STEP_Y);
    {
        g_left_line[y] = 0;
    }
    
    while(y >= g_search_top)
    {
        //动态阈值，近处阈值大，远处阈值小
        diff_thres = (g_differ_threshold1 - g_differ_threshold2)*(y-SEARCH_TOP)/(SEARCH_BOT - SEARCH_TOP) + g_differ_threshold2;
        //如果上一行未找到，则进行全白判断，若是全白，则跳过
        if(!find_flag&&thres_table[g_image[y][SEARCH_LEFT]]&&thres_table[g_image[y][x_conditon_left]])
        {
            g_left_line[y] = 0;
            y -= SEARCH_STEP_Y;
            continue;
        }
        //否则进行搜索
        else
        {
            times++;
            //第一次寻找根据之前的结果
            if(times == 1)
            {
                x = last_x + margin;
                end_x = SEARCH_LEFT;
            }
            //第二次寻找扩大范围
            else
            {
                x = g_whiteline_x;
                end_x = last_x + margin +1;
            }
            //防止x越界
            if(x < SEARCH_LEFT)
            {
                x = SEARCH_LEFT;
            }
            //清除寻找结果
            find_flag = 0x0000;
            //开始搜索
            for(; x >= end_x; x -= SEARCH_STEP_X)
            {
                //搜索白黑黑
                if(thres_table[g_image[y][x+1]]&&!thres_table[g_image[y][x]]&&!thres_table[g_image[y][x-1]])
                {
                    g_left_line[y] = x;
                    last_x = x;
                    find_flag = 0xffff;
                    count ++;
                    times = 0;
                    y -= SEARCH_STEP_Y;
                    break;
                }
                else
                    continue;
            }
            //如果两次没找到，搜索下一行
            if(!find_flag && times == 2)
            {
                times = 0;
                try_time --;
                y -= SEARCH_STEP_Y;
                if(try_time)
                    continue;
                else
                    break;
            }
        }
    }

    g_left_count = count;
    return count;
}*/

/* 
@brief		    搜左线
@param		    uint16
@return		    void
@note           copy
@since		    v3.0    for image
Sample usage:	
*/
uint8 image_fastsearch_leftline(void)
{
	uint8 count = 0;
	uint8 try1 = SEARCH_TRY;
	uint8 times = 0;
	uint8 find_flag = 0;
	uint8 last_x = g_whiteline_x;
	uint8* thres_table = g_thres_table;
	uint8 count_white = WHITE_TRY;
	uint8 margin = SEARCH_MARGIN;
	//uint8 t = 0;

	uint8 y = SEARCH_BOT;
	int16 x = 0;
	for(y = SEARCH_BOT; y >= SEARCH_TOP; y -= SEARCH_STEP_Y)
	{
		g_left_line[y] = 0;
	}
	y = SEARCH_BOT;
	while(y >= g_search_top)
	{
		if(x <= SEARCH_LEFT)
		{
			x = SEARCH_LEFT+1;
		}
		//进行全白行判断，超过一定阈值则断定为十字
		if(!count_white)
		{
			break;
		}
		//如果上一行未找到，则进行全白行判断，如果是全白，则跳过
		if(!find_flag && thres_table[g_image[y][SEARCH_LEFT]])
		{
			g_left_line[y] = 0;
			count_white --;
			y -= SEARCH_STEP_Y;
			continue;
		}
		//刚开始从最长白列搜
		else if(times == 0)
		{
			for(x = g_whiteline_x; x > SEARCH_LEFT; x -= SEARCH_STEP_X)
			{
				//白黑黑
				if(thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && !thres_table[g_image[y][x - 1]])
				{
					g_left_line[y]= x;
					last_x = x;
					find_flag = 1;
					count ++;
					times ++;
					y -= SEARCH_STEP_Y;
					break;
				}
			}
			//for啥都没搜到
			if(x <= SEARCH_LEFT)
			{
				try1 --;
				//防止数组溢出
				x = g_whiteline_x;
				last_x = x;
				g_left_line[y] = 0;
				y -= SEARCH_STEP_Y;
				if(!try1)
					break;
			}
		}
		else
		{
			times++;
			x = last_x;
			find_flag = 0;

			//如果为黑，则往右搜
			if(thres_table[g_image[y][x]] == 0)
			{
				for(; x < g_whiteline_x; x += SEARCH_STEP_X)
				{
					if(thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && !thres_table[g_image[y][x - 1]])
					{
						g_left_line[y] = x;
						last_x = x;
						find_flag = 1;
						count++;
						y -= SEARCH_STEP_Y;
						times = 1;
						break;
					}
				}
				//for啥都没搜到
				if(x >= g_whiteline_x)
				{
					try1 --;
					y -= SEARCH_STEP_Y;
					if(!try1)
						break;
				}
			}
			//如果为白，则往左搜
			else if(thres_table[g_image[y][x]] == 1)
			{
				for(; x > SEARCH_LEFT; x -= SEARCH_STEP_X)
				{
					if(thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && !thres_table[g_image[y][x - 1]])
					{
						g_left_line[y] = x;
						if(x + margin <= last_x)
						{
							count_white--;//为了判断全白行
						}
						else{}
						last_x = x;
						find_flag = 1;
						count++;
						y -= SEARCH_STEP_Y;
						times = 1;
						break;
					}
				}
				//for啥都没搜到
				if(x <= SEARCH_LEFT)
				{
					try1 --;
					//防止数组溢出
					x = g_whiteline_x;
					last_x = g_whiteline_x;
					g_left_line[y] = 0;
					y -= SEARCH_STEP_Y;
					if(!try1)
						break;
				}
			}
			if(times == 3)
			{
				times = 1;
				try1 --;
				y -= SEARCH_STEP_Y;
				if(!try1)
					break;
			}
		}
	}

	// for(t = SEARCH_BOT; t >= SEARCH_TOP; t--)
	// {
	// 	if(g_left_line[t] > g_whiteline_x)
	// 	{
	// 		g_left_line[t] = SEARCH_LEFT;
	// 	}
	// }

	if(y > g_search_top)
	{
		uint8 white_start = MIN(y+WHITE_TRY+1, SEARCH_BOT);
		uint8 white_end = g_search_top;
		
		count_white = WHITE_TRY;
		times = 0;
		y = g_search_top;
		while (y <= white_start)
		{
			//如果十字检测到
			if(!count_white)
			{
				break;
			}
			//如果上一行未找到，则进行全白行判断，如果是全白，则跳过
			if(!find_flag && thres_table[g_image[y][SEARCH_LEFT]])
			{
				g_left_line[y] = SEARCH_LEFT;
				count_white --;
				y += SEARCH_STEP_Y;
				continue;
			}
			else if(times == 0)
			{
				for(x = g_whiteline_x; x > SEARCH_LEFT; x -= SEARCH_STEP_X)
				{
					//白黑黑
					if(thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && !thres_table[g_image[y][x - 1]])
					{
						g_left_line[y]= x;
						last_x = x;
						count ++;
						times ++;
						y += SEARCH_STEP_Y;
						break;
					}
				}
				//for啥都没搜到
				if(x <= SEARCH_LEFT)
				{
					try1 --;
					//防止数组溢出
					g_left_line[y] = 0;
					y += SEARCH_STEP_Y;
				}
			}
			else
			{
				times++;
				x = last_x;
				find_flag = 0;

				//如果下行该位置为黑
				if(thres_table[g_image[y][x]] == 0)
				{
					for(; x < g_whiteline_x; x += SEARCH_STEP_X)
					{
						if(thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && !thres_table[g_image[y][x - 1]])
						{
							g_left_line[y] = x;
							last_x = x;
							find_flag = 1;
							count++;
							y += SEARCH_STEP_Y;
							times = 1;
							break;
						}
					}
					if(x >= g_whiteline_x)
					{
						try1 --;
						//防止数组溢出
						g_left_line[y] = 0;
						y += SEARCH_STEP_Y;
					}
				}
				//如果下行该位置为白
				else
				{
					for(; x >SEARCH_LEFT; x -= SEARCH_STEP_X)
					{
						if(thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && !thres_table[g_image[y][x - 1]])
						{
							g_left_line[y] = x;
							if(x + margin <= last_x)
							{
								count_white--;
							}
							else{}
							last_x = x;
							find_flag = 1;
							count++;
							y += SEARCH_STEP_Y;
							times = 1;
							break;
						}
					}
					//for啥都没搜到
					if(x <= SEARCH_LEFT)
					{
						try1 --;
						//防止数组溢出
						x = g_whiteline_x;
						last_x = g_whiteline_x;
						g_left_line[y] = 0;
						y += SEARCH_STEP_Y;
					}
				}
				if(find_flag == 0 && times == 3)
				{
					times = 1;
					try1 --;
					y += SEARCH_STEP_Y;
					if(!try1)
						break;
				}
			}
		}
		if(y < white_start)
		{
			uint8 i = 0;
			int8 temp = 0;
			white_end = MAX(y-WHITE_TRY-1, g_search_top);
			if(white_end <= g_search_top)
			{
				for(y = white_start; y >= g_search_top; y--)
				{
					temp= 2*g_left_line[y-1] - g_left_line[y-2];
					if(temp >= SEARCH_LEFT)
						g_left_line[y] = temp;
					else
						g_left_line[y] = SEARCH_LEFT;
				}
			}
			else if(white_start >= SEARCH_BOT)
			{
				for(y = white_end; y <= SEARCH_BOT; y++)
				{
					temp= 2*g_left_line[y-1] - g_left_line[y-2];
					if(temp >= SEARCH_LEFT)
						g_left_line[y] = temp;
					else
						break;
				}
				for(; y <= SEARCH_BOT; y++)
					g_left_line[y] = SEARCH_LEFT;
			}
			else
			{
				//使用定比分点拟合十字边缘
				count = white_start - white_end;
				for(y = white_end; y < white_start; y++)
				{
					i++;
					g_left_line[y] = ((count - i)*g_left_line[white_end-1] + i*g_left_line[white_start+1])/count;
				}
			}
		}
	// WHITESTART_L = white_start;
	// WHITEEND_L = white_end;
	}

	return count;
}

/*
uint16 image_fastsearch_leftline(void)
{
    int16 end_x = SEARCH_LEFT;
	uint16 count = 0;
	uint16 times = 0;
	//uint16 try1 = SEARCH_TRY;
	//uint16 diff_thres = 0;
	uint16 find_flag = 0x0000;
	uint16 last_x = g_whiteline_x;
	uint16 margin = SEARCH_MARGIN;
	uint8* thres_table = g_thres_table;

	//拟合十字赛道
	//uint8 white_start = 0;
	//uint8 white_stop = 0;
	//uint8 white_num = 0;
	//uint8 t = 0;

	for (uint16 y = SEARCH_BOT; y >= SEARCH_TOP; y -= SEARCH_STEP_Y)
	{
		g_left_line[y] = 0;
	}

	int16 x;
	uint16 y = SEARCH_BOT;
	while (y >= g_search_top)
	{
		// 动态阈值，近处阈值大，远处阈值小
		//diff_thres = (g_differ_threshold1 - g_differ_threshold2) * (y - SEARCH_TOP) / (SEARCH_BOT - SEARCH_TOP) + g_differ_threshold2;
		// 如果上一行未找到，则进行全白行判断，如果是全白，则跳过
		if (!find_flag && thres_table[g_image[y][SEARCH_LEFT]] && thres_table[g_image[y][SEARCH_RIGHT]])// && thres_table[g_image[y][x_conditon_left]])
		{
			/*if(white_num == 0)//记录下第一行白行的位置
			{
				white_start = y	+ SEARCH_STEP_Y;
			}
			else{}*/
			//g_left_line[y] = 0;
			//white_start = y - 1;
			//white_num ++;
			/*times = 2;
			y -= SEARCH_STEP_Y;
			g_left_line[y] = SEARCH_LEFT;
			continue;
		}
		// 否则进行搜索
		else
		{
			times++;
			// 第一次寻找根据之前的结果
			if (times == 1)
			{
				x = last_x + margin;
				end_x = SEARCH_LEFT;
			}
			// 第二次寻找扩大范围
			else
			{
				x = g_whiteline_x;
				end_x = last_x + margin + 1;
			}
			// 防止 x 越界
			if (x > SEARCH_RIGHT)
			{
				x = SEARCH_RIGHT;
			}
			// 清除寻找结果
			find_flag = 0x0000;
			// 开始搜索
			for (; x >= end_x; x -= SEARCH_STEP_X)
			{
				if (thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && !thres_table[g_image[y][x - 1]])
					// if (FAST_DIFFER((int16)g_image[y][x + 1], (int16)g_image[y][x]) > diff_thres)
				{
					g_left_line[y] = x;
					last_x = x;
					find_flag = 0xffff;
					count++;
					times = 0;
					y -= SEARCH_STEP_Y;
					break;
				}
				else
				{
					continue;
				}
			}
			// 如果两次没找到，搜索下一行
			if (!find_flag && times == 2)
			{
				times = 0;
				g_left_line[y] = SEARCH_LEFT;
				//try1--;
				y -= SEARCH_STEP_Y;
			}
		}
	}*/
	/*
	white_num += 2;	//加上两个端点转化成定比分点公式
	white_stop = white_start - white_num;
	y = MIN(SEARCH_BOT,white_start);
	if(white_start - 1 == SEARCH_BOT)
	{
		while(t < white_num)
		{
			g_left_line[y--] = g_left_line[white_stop];
			t++;
		}
	}
	else if(white_stop + 1 > SEARCH_TOP)
	{
		while(t < white_num)
		{			
			g_left_line[y--] =(uint16)((white_num-t)*g_left_line[white_start] + t*g_left_line[white_stop])/white_num;
			t++;
		}
	}
	else if(white_stop + 1 == SEARCH_TOP)
	{
		while(t < white_num)
		{
			g_left_line[y--] = g_left_line[white_start];
			t++;
		}
	}
	else{};*/

	/*g_left_count = count;
	return count;
}*/

// 搜右线
/*uint16 image_fastsearch_rightline(void)
{
	int16 end_x = SEARCH_RIGHT;
    uint16 count = 0;
    uint16 times = 0;
    uint16 try_time = SEARCH_TRY;
    uint16 diff_thres = 0;
    uint16 find_flag = 0x0000;
    uint16 last_x = g_whiteline_x;
    uint16 margin = SEARCH_MARGIN;
    uint8* thres_table = g_thres_table;
    uint16 y = SEARCH_BOT;
    int16 x = 0;

    for(y = SEARCH_BOT; y >= SEARCH_TOP; y -= SEARCH_STEP_Y);
    {
        g_right_line[y] = 0;
    }
    
    while(y >= g_search_top)
    {
        //动态阈值，近处阈值大，远处阈值小
        diff_thres = (g_differ_threshold1 - g_differ_threshold2)*(y-SEARCH_TOP)/(SEARCH_BOT - SEARCH_TOP) + g_differ_threshold2;
        //如果上一行未找到，则进行全白判断，若是全白，则跳过
        if(!find_flag&&thres_table[g_image[y][SEARCH_RIGHT]]&&thres_table[g_image[y][x_conditon_right]])
        {
            g_right_line[y] = 0;
            y -= SEARCH_STEP_Y;
            continue;
        }
        //否则进行搜索
        else
        {
            times++;
            //第一次寻找根据之前的结果
            if(times == 1)
            {
                x = last_x - margin;
                end_x = SEARCH_RIGHT;
            }
            //第二次寻找扩大范围
            else
            {
                x = g_whiteline_x;
                end_x = last_x - margin - 1;
            }
            //防止x越界
            if(x > SEARCH_RIGHT)
            {
                x = SEARCH_RIGHT;
            }
            //清除寻找结果
            find_flag = 0x0000;
            //开始搜索
            for(; x <= end_x; x += SEARCH_STEP_X)
            {
                //搜索白黑黑
                if(thres_table[g_image[y][x-1]]&&!thres_table[g_image[y][x]]&&!thres_table[g_image[y][x+1]])
                {
                    g_right_line[y] = x;
                    last_x = x;
                    find_flag = 0xffff;
                    count ++;
                    times = 0;
                    y -= SEARCH_STEP_Y;
                    break;
                }
                else
                    continue;
            }
            //如果两次没找到，搜索下一行
            if(!find_flag && times == 2)
            {
                times = 0;
                try_time --;
                y -= SEARCH_STEP_Y;
                if(try_time)
                    continue;
                else
                    break;
            }
        }
    }

    g_right_count = count;
    return count;
}*/

/* 
@brief		    搜右线
@param		    uint16
@return		    void
@note           copy
@since		    v3.0    for image
Sample usage:	
*/

uint8 image_fastsearch_rightline(void)
{
	uint8 count = 0;
	uint8 try1 = SEARCH_TRY;
	uint8 times = 0;
	uint8 find_flag = 0;
	uint8 last_x = g_whiteline_x;
	uint8* thres_table = g_thres_table;
	uint8 count_white = WHITE_TRY;
	uint8 margin = SEARCH_MARGIN;

	uint8 y = SEARCH_BOT;
	uint8 x = 0;
	for(y = SEARCH_BOT; y >= SEARCH_TOP; y -= SEARCH_STEP_Y)
	{
		g_right_line[y] = SEARCH_RIGHT;
	}
	y = SEARCH_BOT;
	while(y >= g_search_top)
	{
		//进行全白行判断，超过一定阈值则断定为十字
		if(!count_white)
		{
			break;
		}
		//如果上一行未找到，则进行全白行判断，如果是全白，则跳过
		if(!find_flag && thres_table[g_image[y][SEARCH_RIGHT]])
		{
			g_right_line[y] = SEARCH_RIGHT;
			count_white --;
			y -= SEARCH_STEP_Y;
			continue;
		}
		//刚开始从最长白列搜
		else if(times == 0)
		{
			for(x = g_whiteline_x; x < SEARCH_RIGHT; x += SEARCH_STEP_X)
			{
				//白黑黑
				if(!thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && thres_table[g_image[y][x - 1]])
				{
					g_right_line[y]= x;
					last_x = x;
					find_flag = 1;
					count ++;
					times ++;
					y -= SEARCH_STEP_Y;
					break;
				}
			}
			//for啥都没搜到
			if(x >= SEARCH_RIGHT)
			{
				try1 --;
				x = g_whiteline_x;
				last_x = g_whiteline_x;
				g_right_line[y] = SEARCH_RIGHT;
				y -= SEARCH_STEP_Y;
			}
		}
		else
		{
			times++;
			x = last_x;
			find_flag = 0;

			//如果为黑，则往左搜
			if(thres_table[g_image[y][x]] == 0)
			{
				for(; x > g_whiteline_x; x -= SEARCH_STEP_X)
				{
					if(!thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && thres_table[g_image[y][x - 1]])
					{
						g_right_line[y] = x;
						last_x = x;
						find_flag = 1;
						count++;
						y -= SEARCH_STEP_Y;
						times = 1;
						break;
					}
				}
			}
			//如果为白，则往右搜
			else if(thres_table[g_image[y][x]] == 1)
			{
				for(; x < SEARCH_RIGHT; x += SEARCH_STEP_X)
				{
					if(!thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && thres_table[g_image[y][x - 1]])
					{
						g_right_line[y] = x;
						if(x - margin >= last_x)
						{
							count_white--;//为了判断全白行
						}
						else{}
						last_x = x;
						find_flag = 1;
						count++;
						y -= SEARCH_STEP_Y;
						times = 1;
						break;
					}
				}
				//for啥都没搜到
				if(x >= SEARCH_RIGHT)
				{
					try1 --;
					x = g_whiteline_x;
					last_x = g_whiteline_x;
					g_right_line[y] = SEARCH_RIGHT;
					y -= SEARCH_STEP_Y;
				}
			}
			if(find_flag == 0 && times == 3)
			{
				times = 1;
				try1 --;
				y -= SEARCH_STEP_Y;
				if(!try1)
					break;
			}
		}
	}
	if(y > g_search_top)
	{
		uint8 white_start = MIN(y+WHITE_TRY+1, SEARCH_BOT);
		uint8 white_end = g_search_top;
		
		count_white = WHITE_TRY;
		times = 0;
		y = g_search_top;
		while (y <= white_start)
		{
			//如果十字检测到
			if(!count_white)
			{
				break;
			}
			//如果上一行未找到，则进行全白行判断，如果是全白，则跳过
			if(!find_flag && thres_table[g_image[y][SEARCH_RIGHT]])
			{
				g_right_line[y] = SEARCH_RIGHT;
				count_white --;
				y += SEARCH_STEP_Y;
				continue;
			}
			else if(times == 0)
			{
				for(x = g_whiteline_x; x < SEARCH_RIGHT; x += SEARCH_STEP_X)
				{
					//白黑黑
					if(!thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && thres_table[g_image[y][x - 1]])
					{
						g_right_line[y]= x;
						last_x = x;
						count ++;
						times ++;
						y += SEARCH_STEP_Y;
						break;
					}
				}
				//for啥都没搜到
				if(x >= SEARCH_RIGHT)
				{
					try1 --;
					x = g_whiteline_x;
					last_x = g_whiteline_x;
					g_right_line[y] = SEARCH_RIGHT;
					y += SEARCH_STEP_Y;
				}
			}
			else
			{
				times++;
				x = last_x;
				find_flag = 0;

				//如果下行该位置为黑
				if(thres_table[g_image[y][x]] == 0)
				{
					for(; x > g_whiteline_x; x -= SEARCH_STEP_X)
					{
						if(!thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && thres_table[g_image[y][x - 1]])
						{
							g_right_line[y] = x;
							last_x = x;
							find_flag = 1;
							count++;
							y += SEARCH_STEP_Y;
							times = 1;
							break;
						}
					}
				}
				//如果下行该位置为白
				else
				{
					for(; x < SEARCH_RIGHT; x += SEARCH_STEP_X)
					{
						if(!thres_table[g_image[y][x + 1]] && !thres_table[g_image[y][x]] && thres_table[g_image[y][x - 1]])
						{
							g_right_line[y] = x;
							if(x - margin >= last_x)
							{
								count_white--;
							}
							else{}
							last_x = x;
							find_flag = 1;
							count++;
							y += SEARCH_STEP_Y;
							times = 1;
							break;
						}
					}
					//for啥都没搜到
					if(x >= SEARCH_RIGHT)
					{
						try1 --;
						x = g_whiteline_x;
						last_x = g_whiteline_x;
						g_right_line[y] = SEARCH_RIGHT;
						y += SEARCH_STEP_Y;
					}
				}
				if(find_flag == 0 && times == 3)
				{
					times = 1;
					try1 --;
					y += SEARCH_STEP_Y;
					if(!try1)
						break;
				}
			}
		}
		if(y < white_start)
		{
			uint8 i = 0;
			white_end = MAX(y-WHITE_TRY-2, g_search_top);
			if(white_end <= g_search_top)
			{
				for(y = white_start; y >= g_search_top; y--)
				{
					g_right_line[y] = 2*g_right_line[y+1]-g_right_line[y-2];
					if(g_right_line[y] >= SEARCH_RIGHT)
						g_right_line[y] = SEARCH_RIGHT;
				}
			}
			else if(white_start >= SEARCH_BOT)
			{
				for(y = white_end; y <= SEARCH_BOT; y++)
				{
					g_right_line[y] = 2*g_right_line[y-1]-g_right_line[y-2];
					if(g_right_line[y] >= SEARCH_RIGHT)
					{
						g_right_line[y] = SEARCH_RIGHT;
						break;
					}
				}
				for(; y <= SEARCH_BOT; y++)
					g_right_line[y] = SEARCH_RIGHT;
			}
			else
			{	
				//使用定比分点拟合十字边缘
				count = white_start - white_end;
				for(y = white_end; y <= white_start; y++)
				{
					i++;
					g_right_line[y] = ((count - i)*g_right_line[white_end-1] + i*g_right_line[white_start+1])/count;
					
				}
			}
		}
	// WHITESTART_R = white_start;
	// WHITEEND_R = white_end;
	}

	return count;
}

/*
uint16 image_fastsearch_rightline()
{
	int16 end_x = SEARCH_RIGHT;
	uint16 count = 0;
	uint16 times = 0;
	//uint16 try1 = SEARCH_TRY;
	//uint16 diff_thres = 0;
	uint16 last_x = g_whiteline_x;
	uint16 find_flag = 0x0000;
	uint16 margin = SEARCH_MARGIN;
	uint8* thres_table = g_thres_table;

	//拟合十字赛道
	// uint8 white_start = 0;
	// uint8 white_stop = 0;
	// uint8 white_num = 0;
	// uint8 t = 0;

	for (uint16 y = SEARCH_BOT; y >= SEARCH_TOP; y -= SEARCH_STEP_Y)
	{
		g_right_line[y] = 0;
	}

	int16 x;
	uint16 y = SEARCH_BOT;
	while (y >= g_search_top)
	{
		//diff_thres = (g_differ_threshold1 - g_differ_threshold2) * (y - SEARCH_TOP) / (SEARCH_BOT - SEARCH_TOP) + g_differ_threshold2;
		if (!find_flag && thres_table[g_image[y][SEARCH_RIGHT]] && thres_table[g_image[y][SEARCH_LEFT]])// && thres_table[g_image[y][x_conditon_right]])
		{
			/*if(white_num == 0)//记录下第一行白行的位置
			{
				white_start = y	+ SEARCH_STEP_Y;
			}
			else{}*/
			//g_right_line[y] = 0;
			//white_num ++;
			/*y -= SEARCH_STEP_Y;
			g_right_line[y] = SEARCH_RIGHT;
			times=2;
			continue;
		}
		else
		{
			times++;
			if (times == 1)
			{
				x = last_x - margin;
				end_x = SEARCH_RIGHT;
			}
			else
			{
				x = g_whiteline_x;
				end_x = last_x - margin + 1;
			}
			if (x < SEARCH_LEFT)
			{
				x = SEARCH_LEFT;
			}
			find_flag = 0x0000;
			for (; x <= end_x; x += SEARCH_STEP_X)
			{
				if (thres_table[g_image[y][x - 1]] && !thres_table[g_image[y][x]] && !thres_table[g_image[y][x + 1]])
					// if (FAST_DIFFER((int16)g_image[y][x - 1], (int16)g_image[y][x]) > diff_thres)
				{
					g_right_line[y] = x;
					last_x = x;
					find_flag = 0xffff;
					count++;
					times = 0;
					y -= SEARCH_STEP_Y;
					break;
				}
				else
				{
					continue;
				}
			}
			if (!find_flag && times == 2)
			{
				times = 0;
				g_right_line[y] = 0;
				y -= SEARCH_STEP_Y;
				/*try1--;
				if (try1)
				{
					continue;
				}
				else
				{
					break;
				}*/
	// 		}
	// 	}
	// }
/*
	white_num += 2;	//加上两个端点转化成定比分点公式
	white_stop = white_start - white_num;
	y = MIN(SEARCH_BOT,white_start);
	if(white_start - 1 == SEARCH_BOT)
	{
		while(t < white_num)
		{
			g_right_line[y--] = g_right_line[white_stop];
			t++;
		}
	}
	else if(white_stop + 1 > SEARCH_TOP)
	{
		while(t < white_num)
		{
			g_right_line[y--] =(uint16)((white_num-t)*g_right_line[white_start] + t*g_right_line[white_stop])/white_num;
			t++;
		}
	}
	else if(white_stop + 1 == SEARCH_TOP)
	{
		while(t < white_num)
		{
			g_right_line[y--] = g_right_line[white_start];
			t++;
		}
	}else{};*/


// 	g_right_count = count;
// 	return count;
// }

/* 
@brief		    计算偏差
@param		    uint16
@return		    void
@note           copy
@since		    v3.0    for image
Sample usage:	
*/
void calcular_err(void)
{
	//static float last_image_err_bias = 0.0;
	//static float last_image_err_ratio = 0.0;

	//if (last_image_err_bias != g_image_err_bias || last_image_err_ratio != g_image_err_ratio)
	//{
	for (uint16 y = SEARCH_BOT; y >= SEARCH_TOP; y -= SEARCH_STEP_Y)
	{
		g_err_weight[y] = (uint16)((SEARCH_BOT - y) * g_image_err_ratio + g_image_err_bias);
	}
		//last_image_err_bias = g_image_err_bias;
		//last_image_err_ratio = g_image_err_ratio;
	//}

	uint32 weight_sum = 0;
	int32 err_sum = 0;
	static float last_image_err = 0.0;

	// 误差计算
	uint16 y = MIN(g_search_top + g_useful_row, SEARCH_BOT);
	for (; y >= g_search_top; y -= SEARCH_STEP_Y)
	{
		if (g_mid_line[y] != 0xfff)
		{
			err_sum +=(int32) g_err_weight[y] * (g_mid_line[y] - IMAGE_WIDTH / 2);
			weight_sum += g_err_weight[y];
		}
	}

	// 加权平均
	//if (weight_sum > g_min_weight)
	//{
		g_image_err = err_sum / (float)weight_sum;
	//}

	last_image_err = g_image_err;
}
/*void calcular_err(void)
{
	int16 err1= g_mid_line[SEARCH_BOT]-IMAGE_WIDTH/2;
	int16 err2= g_mid_line[SEARCH_BOT-1]-IMAGE_WIDTH/2;
	int16 err3= g_mid_line[SEARCH_BOT-2]-IMAGE_WIDTH/2;
	int16 err4= g_mid_line[SEARCH_BOT-3]-IMAGE_WIDTH/2;
	g_image_err =err1+err2+err3+err4;
} */


/* 
@brief		    计算中线
@param		    uint16
@return		    void
@note           copy
@since		    v3.0    for image
Sample usage:	
*/

void image_find_midline(void)
{
	//uint8 ek_now = 0;
	//uint8 ek_last = 0;
	for (uint16 y = SEARCH_BOT; y >= g_search_top; y -= SEARCH_STEP_Y)
	{
		g_mid_line[y] = 0xfff;
	}
    for (uint16 y = SEARCH_BOT; y >= g_search_top; y -= SEARCH_STEP_Y)
    {
        if (g_left_line[y] && g_right_line[y])
        {
                g_mid_line[y] = (g_right_line[y] + g_left_line[y]) >> 1;
        }
    	else if (g_left_line[y])
        {
                g_mid_line[y] = (g_left_line[y] + SEARCH_RIGHT) >> 1;
        }
        else if (g_right_line[y])
        {
                g_mid_line[y] = (g_right_line[y] + SEARCH_LEFT) >> 1;
        }
        else
        {
                g_mid_line[y] = 0xfff;
        }
    }
}

/* 
@brief		    图像测试画面生成
@param		    void
@return		    void
@note           copy
@since		    v3.0    for image
Sample usage:	
*/

void image_debug(void)
{
	memset(g_watch, 0, IMAGE_HEIGHT * IMAGE_WIDTH);

	for (uint16 y = SEARCH_BOT; y >= SEARCH_TOP; y -= 3)
	{
		g_watch[y][g_whiteline_x] = 5;
	}
	for (uint16 y = SEARCH_BOT; y >= SEARCH_TOP; y -= SEARCH_STEP_Y)
	{
		if (g_left_line[y])
		{
			g_watch[y][g_left_line[y]] = 3;
			g_watch[y][g_left_line[y] + 1] = 3;
		}
		if (g_right_line[y])
		{
			g_watch[y][g_right_line[y]] = 4;
			g_watch[y][g_right_line[y] - 1] = 4;
			/*if(g_right_line[y] <= 160)
			{
				
			}
			else
			{
				g_watch[y][160] = 4;
				g_watch[y][159] = 4;
			}*/
		}
		if (g_mid_line[y] != 0xfff)
		{
			uint16 mx = g_mid_line[y] < SEARCH_LEFT ? SEARCH_LEFT 
					  : g_mid_line[y] > SEARCH_RIGHT ? SEARCH_RIGHT
					  : g_mid_line[y];
			g_watch[y][mx] = 2;
			g_watch[y][mx + 1] = 2;
		}
	}
	for (uint16 x = 0; x < IMAGE_WIDTH; x += 3)
	{
		g_watch[g_search_top][x] = 5;
		g_watch[SEARCH_TOP][x] = 5;
		g_watch[SEARCH_BOT][x] = 5;
	}
}

/* 
@brief		    线性插值
@param		    void
@return		    void
@note           copy
@since		    v3.0    for image
Sample usage:	
*/

void image_interpolate(float k, float b, uint16 from, uint16 to, uint16 line_type)
{
#define INTERPOLATE_SCALE	100
	uint16* lines;
	int16 ik = (int16)(k * INTERPOLATE_SCALE);
	int16 ib = (int16)(b * INTERPOLATE_SCALE);

	if (line_type == LINE_LEFT)
	{
		lines = g_left_line;
	}
	else
	{
		lines = g_right_line;
	}

	for (uint16 y = from; y >= to; y -= SEARCH_STEP_Y)
	{
		int16 x = (ik * y + ib) / INTERPOLATE_SCALE;
		lines[y] = LIMITER(x, SEARCH_LEFT, SEARCH_RIGHT);
	}
}
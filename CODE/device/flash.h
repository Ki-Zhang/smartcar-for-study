#ifndef _FLASH_H
#define _FLASH_H

#include "common.h"
#include "param.h"
#include "general.h"

#define FLASH_SECTION	FLASH_SECTION_126
#define FLASH_PAGE		FLASH_PAGE_0
#define FLASH_LENGTH	(FLASH_VAR_NUM)

uint32 convert_str_to_32(uint8 str[]);
void convert_32_to_str(uint32 origin, uint8 str[]);


#endif
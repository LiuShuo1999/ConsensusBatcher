#ifndef _DELAY_H
#define _DELAY_H
#include <sys.h>	  


void delay_init(u8 SYSCLK);
void delay_us(u32 nus);
void delay_ms(u32 nms);
void delay_xms(u32 nms);
#endif


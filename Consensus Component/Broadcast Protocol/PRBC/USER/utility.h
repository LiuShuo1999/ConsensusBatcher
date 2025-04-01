#ifndef UTILITY_H
#define UTILITY_H
#include "sys.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include "delay.h"

#define ULONG_MAX 0xffffffffUL

extern uint8_t private_key[13+1][32];
extern uint8_t public_key[13+1][64];

void PreSleepProcessing(uint32_t ulExpectedIdleTime);
void PostSleepProcessing(uint32_t ulExpectedIdleTime);
u8 Sixteen2Ten(char ch);
static int my_RNG(uint8_t *dest, unsigned size);
void init_public_key();
void SystemClock_Config(void);
void SystemClock_Config_low(void);
void MPU_Config(void);
void CPU_CACHE_Enable(void);

#endif /* UTILITY_H */
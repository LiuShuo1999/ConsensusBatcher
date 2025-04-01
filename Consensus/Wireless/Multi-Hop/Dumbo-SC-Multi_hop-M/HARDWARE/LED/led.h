#ifndef _LED_H
#define _LED_H
#include "sys.h"

//LED端口定义
//#define LED0(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_0,GPIO_PIN_RESET))
#define LED0_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0)) //LED0输出电平翻转
//#define LED1(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_7,GPIO_PIN_RESET))
#define LED1_Toggle (HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_7)) //LED1输出电平翻转
//#define LED2(n)		(n?HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_SET):HAL_GPIO_WritePin(GPIOB,GPIO_PIN_14,GPIO_PIN_RESET))

#define LED0(x)			GPIO_Pin_Set(GPIOB,PIN0,x)		// DS0
#define LED1(x)			GPIO_Pin_Set(GPIOB,PIN7,x)		// DS1 
#define LED2(x)			GPIO_Pin_Set(GPIOB,PIN14,x)		// DS1 

extern u8 led2sta;
extern u8 led1sta;
extern u8 led0sta;

void LED_Init(void); //LED初始化函数
#endif

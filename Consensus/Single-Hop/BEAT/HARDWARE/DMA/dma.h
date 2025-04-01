#ifndef __DMA_H
#define __DMA_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F746开发板
//DMA驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2015/12/28
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define RECE_BUF_SIZE 400
extern u8 ReceBuff[RECE_BUF_SIZE]; 
//extern u8 OkReceBuff[3][RECE_BUF_SIZE];
//extern u8 OkIdx;
extern u8 PACKET_LEN;
extern DMA_HandleTypeDef  UART1TxDMA_Handler;      //DMA句柄
extern DMA_HandleTypeDef  UART3RxDMA_Handler,UART3TxDMA_Handler; 
void MYDMA_Config_Rece(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
void MYDMA_Config_Send(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
#endif

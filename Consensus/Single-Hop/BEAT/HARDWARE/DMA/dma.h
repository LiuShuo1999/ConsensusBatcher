#ifndef __DMA_H
#define __DMA_H
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F746������
//DMA��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2015/12/28
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	

#define RECE_BUF_SIZE 400
extern u8 ReceBuff[RECE_BUF_SIZE]; 
//extern u8 OkReceBuff[3][RECE_BUF_SIZE];
//extern u8 OkIdx;
extern u8 PACKET_LEN;
extern DMA_HandleTypeDef  UART1TxDMA_Handler;      //DMA���
extern DMA_HandleTypeDef  UART3RxDMA_Handler,UART3TxDMA_Handler; 
void MYDMA_Config_Rece(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
void MYDMA_Config_Send(DMA_Stream_TypeDef *DMA_Streamx,u32 chx);
#endif

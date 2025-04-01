#include "dma.h"
#include "usart3.h"
#include "led.h"
#include "bls_BN158.h"
#include "tendermint2.h"
#include "FreeRTOS.h"
#include "ABA_Shared.h"
#include "FreeRTOS.h"
#include "task.h"
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

DMA_HandleTypeDef  UART3RxDMA_Handler,UART3TxDMA_Handler;      //DMA���
u8 ReceBuff[RECE_BUF_SIZE];
//u8 OkReceBuff[3][RECE_BUF_SIZE];
//u8 OkIdx = 0;
extern u8 low_flag;
extern u32 low_IDX;
extern u32 low_time[400];
extern u32 wfi_time[400];
extern u8 Send_Data_buff[800];
//DMAx�ĸ�ͨ������
//����Ĵ�����ʽ�ǹ̶���,���Ҫ���ݲ�ͬ��������޸�
//�Ӵ洢��->����ģʽ/8λ���ݿ��/�洢������ģʽ
//DMA_Streamx:DMA������,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMAͨ��ѡ��,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
void MYDMA_Config_Rece(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2ʱ��ʹ��	
	}else 
	{
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ�� 
	}
    
    __HAL_LINKDMA(&uart3_handler,hdmarx,UART3RxDMA_Handler);    //��DMA��USART3��ϵ����(����DMA)

    //Tx DMA����
    UART3RxDMA_Handler.Instance=DMA_Streamx;                            //������ѡ��
    UART3RxDMA_Handler.Init.Channel=chx;                                //ͨ��ѡ��
    UART3RxDMA_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;             //�洢��������
    UART3RxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
    UART3RxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
    UART3RxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
    UART3RxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
    UART3RxDMA_Handler.Init.Mode=DMA_NORMAL;                            //��������ģʽ
    UART3RxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�
    UART3RxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    UART3RxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    UART3RxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
    UART3RxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���
    
    HAL_DMA_DeInit(&UART3RxDMA_Handler);   
    HAL_DMA_Init(&UART3RxDMA_Handler);
	
	
	__HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler,DMA_IT_TC);    //������������ж�
	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,0,0);        //DMA�ж����ȼ�
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	
	
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_HT);    //�ر������ж��ж�
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_TE);    //�ر������ж��ж�
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_FE);    //�ر������ж��ж�
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_DME);    //�ر������ж��ж�
	
// ����
//	//�ڿ���DMA֮ǰ��ʹ��__HAL_UNLOCK()����һ��DMA,��ΪHAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
//    //����������һ��ʼҪ��ʹ��__HAL_LOCK()����DMA,������__HAL_LOCK()���жϵ�ǰ��DMA״̬�Ƿ�Ϊ����״̬�������
//    //����״̬�Ļ���ֱ�ӷ���HAL_BUSY�������ᵼ�º���HAL_DMA_Statrt()��HAL_DMAEx_MultiBufferStart()������DMA����
//    //����ֱ�ӱ�������DMAҲ�Ͳ�������������Ϊ�˱���������������������DMA֮ǰ�ȵ���__HAL_UNLOC()�Ƚ���һ��DMA��
//    u8 mem1addr = 1;
//	__HAL_UNLOCK(&UART3RxDMA_Handler);
//    if(mem1addr==0)    //����DMA����ʹ��˫����
//    {
//        //HAL_DMA_Start(&UART3RxDMA_Handler,(u32)&DCMI->DR,mem0addr,memsize);
//    }
//    else                //ʹ��˫����
//    {
//        //HAL_DMAEx_MultiBufferStart(&DMADMCI_Handler,(u32)&DCMI->DR,mem0addr,mem1addr,memsize);//����˫����
//        __HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler,DMA_IT_TC);    //������������ж�
//        HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,0,0);        //DMA�ж����ȼ�
//        HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
//    }    
} 

void MYDMA_Config_Send(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
	if((u32)DMA_Streamx>(u32)DMA2)//�õ���ǰstream������DMA2����DMA1
	{
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2ʱ��ʹ��	
	}else 
	{
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1ʱ��ʹ�� 
	}
    
    __HAL_LINKDMA(&uart3_handler,hdmatx,UART3TxDMA_Handler);    //��DMA��USART3��ϵ����(����DMA)

    //Tx DMA����
    UART3TxDMA_Handler.Instance=DMA_Streamx;                            //������ѡ��
    UART3TxDMA_Handler.Init.Channel=chx;                                //ͨ��ѡ��
    UART3TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //�洢��������
    UART3TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //���������ģʽ
    UART3TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //�洢������ģʽ
    UART3TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //�������ݳ���:8λ
    UART3TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //�洢�����ݳ���:8λ
    UART3TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //��������ģʽ
    UART3TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //�е����ȼ�
    UART3TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    UART3TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    UART3TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //�洢��ͻ�����δ���
    UART3TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //����ͻ�����δ���
    
    HAL_DMA_DeInit(&UART3TxDMA_Handler);   
    HAL_DMA_Init(&UART3TxDMA_Handler);
	
	
	__HAL_DMA_ENABLE_IT(&UART3TxDMA_Handler,DMA_IT_TC);    //������������ж�
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,1,3);        //DMA�ж����ȼ�
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	
	
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_HT);    //�ر������ж��ж�
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_TE);    //�ر������ж��ж�
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_FE);    //�ر������ж��ж�
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_DME);    //�ر������ж��ж�
	
// ����
//	//�ڿ���DMA֮ǰ��ʹ��__HAL_UNLOCK()����һ��DMA,��ΪHAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
//    //����������һ��ʼҪ��ʹ��__HAL_LOCK()����DMA,������__HAL_LOCK()���жϵ�ǰ��DMA״̬�Ƿ�Ϊ����״̬�������
//    //����״̬�Ļ���ֱ�ӷ���HAL_BUSY�������ᵼ�º���HAL_DMA_Statrt()��HAL_DMAEx_MultiBufferStart()������DMA����
//    //����ֱ�ӱ�������DMAҲ�Ͳ�������������Ϊ�˱���������������������DMA֮ǰ�ȵ���__HAL_UNLOC()�Ƚ���һ��DMA��
//    u8 mem1addr = 1;
//	__HAL_UNLOCK(&UART3RxDMA_Handler);
//    if(mem1addr==0)    //����DMA����ʹ��˫����
//    {
//        //HAL_DMA_Start(&UART3RxDMA_Handler,(u32)&DCMI->DR,mem0addr,memsize);
//    }
//    else                //ʹ��˫����
//    {
//        //HAL_DMAEx_MultiBufferStart(&DMADMCI_Handler,(u32)&DCMI->DR,mem0addr,mem1addr,memsize);//����˫����
//        __HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler,DMA_IT_TC);    //������������ж�
//        HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,0,0);        //DMA�ж����ȼ�
//        HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
//    }    
}
u32 total_data = 500;
u32 buff_data = 0;
u8 start_flag = 1;
u32 buffer_IDX = 0;
u32 DMA_buffer[30][250];
u8 PACKET_LEN;

u8 iii = 0;
void DMA1_Stream1_IRQHandler(void)
{

    if(__HAL_DMA_GET_FLAG(&UART3RxDMA_Handler,DMA_FLAG_TCIF1_5)!=RESET)//DMA�������
    {

        __HAL_DMA_CLEAR_FLAG(&UART3RxDMA_Handler,DMA_FLAG_TCIF1_5);//���DMA��������жϱ�־λ
		
		HAL_UART_DMAStop(&uart3_handler);
		HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);//����DMA����

//		HAL_UART_Receive_DMA(&uart3_handler,OkReceBuff[OkIdx],DMA_buff_Size);//����DMA����
//		OkIdx  = (OkIdx+1) % 3;
//		//�����������ݰ��¼�
//		PACKET_LEN = DMA_buff_Size - UART3RxDMA_Handler.Instance->NDTR;
//		xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//��������֪ͨ��������
//					(uint32_t		)EVENT_NACK,			//Ҫ���µ�bit
//					(eNotifyAction	)eSetBits);				//����ָ����bit
//		�����������ݰ��¼�Ŀǰ�Ľڵ㲻�ᵽ������������
		
		
		
	} 
	
	
	if(__HAL_DMA_GET_FLAG(&UART3RxDMA_Handler,DMA_FLAG_HTIF1_5)!=RESET)//DMA�봫�����
    {
        __HAL_DMA_CLEAR_FLAG(&UART3RxDMA_Handler,DMA_FLAG_HTIF1_5);//���DMA�봫������жϱ�־λ
		
		
//			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//��������֪ͨ��������
//						(uint32_t		)EVENT_NACK,			//Ҫ���µ�bit
//						(eNotifyAction	)eSetBits);				//����ָ����bit
		
		
		//�����������ݰ��¼�Ŀǰ�Ľڵ㲻�ᵽ������������
		PACKET_LEN = DMA_buff_Size - UART3RxDMA_Handler.Instance->NDTR;
		BaseType_t xHigherPriorityTaskWoken = pdTRUE;
		xTaskNotifyFromISR((TaskHandle_t	)EventGroupTask_Handler,//��������֪ͨ��������
						(uint32_t		)EVENT_PACKET,			//Ҫ���µ�bit
						(eNotifyAction	)eSetBits,				//����ָ����bit
						(BaseType_t*	)xHigherPriorityTaskWoken);//�˳�����������л�
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		
	} 
		
}

//����
void DMA1_Stream3_IRQHandler(){
	if(__HAL_DMA_GET_FLAG(&UART3TxDMA_Handler,DMA_FLAG_TCIF3_7)!=RESET)//DMA�������
    {
        __HAL_DMA_CLEAR_FLAG(&UART3TxDMA_Handler,DMA_FLAG_TCIF3_7);//���DMA��������жϱ�־λ
		HAL_UART_DMAStop(&uart3_handler);
		HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);
	} 
	
	
	if(__HAL_DMA_GET_FLAG(&UART3TxDMA_Handler,DMA_FLAG_HTIF3_7)!=RESET)//DMA�봫�����
    {
        __HAL_DMA_CLEAR_FLAG(&UART3TxDMA_Handler,DMA_FLAG_HTIF3_7);//���DMA�봫������жϱ�־λ
	}
}
 
 

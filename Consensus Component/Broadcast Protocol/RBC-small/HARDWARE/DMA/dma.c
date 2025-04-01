#include "dma.h"
#include "usart3.h"
#include "led.h"
#include "bls_BN158.h"

#include "FreeRTOS.h"
#include "RBC.h"
#include "FreeRTOS.h"
#include "task.h"

DMA_HandleTypeDef  UART3RxDMA_Handler,UART3TxDMA_Handler;      //DMA句柄
u8 ReceBuff[RECE_BUF_SIZE];
//u8 OkReceBuff[3][RECE_BUF_SIZE];
//u8 OkIdx = 0;
extern u8 low_flag;
extern u32 low_IDX;
extern u32 low_time[400];
extern u32 wfi_time[400];
extern u8 Send_Data_buff[800];
//DMAx的各通道配置
//这里的传输形式是固定的,这点要根据不同的情况来修改
//从存储器->外设模式/8位数据宽度/存储器增量模式
//DMA_Streamx:DMA数据流,DMA1_Stream0~7/DMA2_Stream0~7
//chx:DMA通道选择,@ref DMA_channel DMA_CHANNEL_0~DMA_CHANNEL_7
void MYDMA_Config_Rece(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2时钟使能	
	}else 
	{
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能 
	}
    
    __HAL_LINKDMA(&uart3_handler,hdmarx,UART3RxDMA_Handler);    //将DMA与USART3联系起来(发送DMA)

    //Tx DMA配置
    UART3RxDMA_Handler.Instance=DMA_Streamx;                            //数据流选择
    UART3RxDMA_Handler.Init.Channel=chx;                                //通道选择
    UART3RxDMA_Handler.Init.Direction=DMA_PERIPH_TO_MEMORY;             //存储器到外设
    UART3RxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
    UART3RxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
    UART3RxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
    UART3RxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //存储器数据长度:8位
    UART3RxDMA_Handler.Init.Mode=DMA_NORMAL;                            //外设流控模式
    UART3RxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
    UART3RxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    UART3RxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    UART3RxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
    UART3RxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
    
    HAL_DMA_DeInit(&UART3RxDMA_Handler);   
    HAL_DMA_Init(&UART3RxDMA_Handler);
	
	
	__HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler,DMA_IT_TC);    //开启传输完成中断
	HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,0,0);        //DMA中断优先级
	HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
	
	
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_HT);    //关闭其他中断中断
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_TE);    //关闭其他中断中断
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_FE);    //关闭其他中断中断
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_DME);    //关闭其他中断中断
	
// 例程
//	//在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
//    //这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
//    //锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
//    //程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOC()先解锁一次DMA。
//    u8 mem1addr = 1;
//	__HAL_UNLOCK(&UART3RxDMA_Handler);
//    if(mem1addr==0)    //开启DMA，不使用双缓冲
//    {
//        //HAL_DMA_Start(&UART3RxDMA_Handler,(u32)&DCMI->DR,mem0addr,memsize);
//    }
//    else                //使用双缓冲
//    {
//        //HAL_DMAEx_MultiBufferStart(&DMADMCI_Handler,(u32)&DCMI->DR,mem0addr,mem1addr,memsize);//开启双缓冲
//        __HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler,DMA_IT_TC);    //开启传输完成中断
//        HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,0,0);        //DMA中断优先级
//        HAL_NVIC_EnableIRQ(DMA1_Stream1_IRQn);
//    }    
} 

void MYDMA_Config_Send(DMA_Stream_TypeDef *DMA_Streamx,u32 chx)
{ 
	if((u32)DMA_Streamx>(u32)DMA2)//得到当前stream是属于DMA2还是DMA1
	{
        __HAL_RCC_DMA2_CLK_ENABLE();//DMA2时钟使能	
	}else 
	{
        __HAL_RCC_DMA1_CLK_ENABLE();//DMA1时钟使能 
	}
    
    __HAL_LINKDMA(&uart3_handler,hdmatx,UART3TxDMA_Handler);    //将DMA与USART3联系起来(发送DMA)

    //Tx DMA配置
    UART3TxDMA_Handler.Instance=DMA_Streamx;                            //数据流选择
    UART3TxDMA_Handler.Init.Channel=chx;                                //通道选择
    UART3TxDMA_Handler.Init.Direction=DMA_MEMORY_TO_PERIPH;             //存储器到外设
    UART3TxDMA_Handler.Init.PeriphInc=DMA_PINC_DISABLE;                 //外设非增量模式
    UART3TxDMA_Handler.Init.MemInc=DMA_MINC_ENABLE;                     //存储器增量模式
    UART3TxDMA_Handler.Init.PeriphDataAlignment=DMA_PDATAALIGN_BYTE;    //外设数据长度:8位
    UART3TxDMA_Handler.Init.MemDataAlignment=DMA_MDATAALIGN_BYTE;       //存储器数据长度:8位
    UART3TxDMA_Handler.Init.Mode=DMA_NORMAL;                            //外设流控模式
    UART3TxDMA_Handler.Init.Priority=DMA_PRIORITY_MEDIUM;               //中等优先级
    UART3TxDMA_Handler.Init.FIFOMode=DMA_FIFOMODE_DISABLE;              
    UART3TxDMA_Handler.Init.FIFOThreshold=DMA_FIFO_THRESHOLD_FULL;      
    UART3TxDMA_Handler.Init.MemBurst=DMA_MBURST_SINGLE;                 //存储器突发单次传输
    UART3TxDMA_Handler.Init.PeriphBurst=DMA_PBURST_SINGLE;              //外设突发单次传输
    
    HAL_DMA_DeInit(&UART3TxDMA_Handler);   
    HAL_DMA_Init(&UART3TxDMA_Handler);
	
	
	__HAL_DMA_ENABLE_IT(&UART3TxDMA_Handler,DMA_IT_TC);    //开启传输完成中断
	HAL_NVIC_SetPriority(DMA1_Stream3_IRQn,1,3);        //DMA中断优先级
	HAL_NVIC_EnableIRQ(DMA1_Stream3_IRQn);
	
	
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_HT);    //关闭其他中断中断
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_TE);    //关闭其他中断中断
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_FE);    //关闭其他中断中断
//	__HAL_DMA_DISABLE_IT(&UART3RxDMA_Handler,DMA_IT_DME);    //关闭其他中断中断
	
// 例程
//	//在开启DMA之前先使用__HAL_UNLOCK()解锁一次DMA,因为HAL_DMA_Statrt()HAL_DMAEx_MultiBufferStart()
//    //这两个函数一开始要先使用__HAL_LOCK()锁定DMA,而函数__HAL_LOCK()会判断当前的DMA状态是否为锁定状态，如果是
//    //锁定状态的话就直接返回HAL_BUSY，这样会导致函数HAL_DMA_Statrt()和HAL_DMAEx_MultiBufferStart()后续的DMA配置
//    //程序直接被跳过！DMA也就不能正常工作，为了避免这种现象，所以在启动DMA之前先调用__HAL_UNLOC()先解锁一次DMA。
//    u8 mem1addr = 1;
//	__HAL_UNLOCK(&UART3RxDMA_Handler);
//    if(mem1addr==0)    //开启DMA，不使用双缓冲
//    {
//        //HAL_DMA_Start(&UART3RxDMA_Handler,(u32)&DCMI->DR,mem0addr,memsize);
//    }
//    else                //使用双缓冲
//    {
//        //HAL_DMAEx_MultiBufferStart(&DMADMCI_Handler,(u32)&DCMI->DR,mem0addr,mem1addr,memsize);//开启双缓冲
//        __HAL_DMA_ENABLE_IT(&UART3RxDMA_Handler,DMA_IT_TC);    //开启传输完成中断
//        HAL_NVIC_SetPriority(DMA1_Stream1_IRQn,0,0);        //DMA中断优先级
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

    if(__HAL_DMA_GET_FLAG(&UART3RxDMA_Handler,DMA_FLAG_TCIF1_5)!=RESET)//DMA传输完成
    {

        __HAL_DMA_CLEAR_FLAG(&UART3RxDMA_Handler,DMA_FLAG_TCIF1_5);//清除DMA传输完成中断标志位
		
		HAL_UART_DMAStop(&uart3_handler);
		HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);//开启DMA传输

//		HAL_UART_Receive_DMA(&uart3_handler,OkReceBuff[OkIdx],DMA_buff_Size);//开启DMA传输
//		OkIdx  = (OkIdx+1) % 3;
//		//触发处理数据包事件
//		PACKET_LEN = DMA_buff_Size - UART3RxDMA_Handler.Instance->NDTR;
//		xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//					(uint32_t		)EVENT_NACK,			//要更新的bit
//					(eNotifyAction	)eSetBits);				//更新指定的bit
//		触发处理数据包事件目前四节点不会到上面的完成那里
		
		
		
	} 
	
	
	if(__HAL_DMA_GET_FLAG(&UART3RxDMA_Handler,DMA_FLAG_HTIF1_5)!=RESET)//DMA半传输完成
    {
        __HAL_DMA_CLEAR_FLAG(&UART3RxDMA_Handler,DMA_FLAG_HTIF1_5);//清除DMA半传输完成中断标志位
		
		
//			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//						(uint32_t		)EVENT_NACK,			//要更新的bit
//						(eNotifyAction	)eSetBits);				//更新指定的bit
		
		
		//触发处理数据包事件目前四节点不会到上面的完成那里
		PACKET_LEN = DMA_buff_Size - UART3RxDMA_Handler.Instance->NDTR;
		BaseType_t xHigherPriorityTaskWoken = pdTRUE;
		xTaskNotifyFromISR((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_PACKET,			//要更新的bit
						(eNotifyAction	)eSetBits,				//更新指定的bit
						(BaseType_t*	)xHigherPriorityTaskWoken);//退出后进行任务切换
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		
	} 
		
}

//发送
void DMA1_Stream3_IRQHandler(){
	if(__HAL_DMA_GET_FLAG(&UART3TxDMA_Handler,DMA_FLAG_TCIF3_7)!=RESET)//DMA传输完成
    {
        __HAL_DMA_CLEAR_FLAG(&UART3TxDMA_Handler,DMA_FLAG_TCIF3_7);//清除DMA传输完成中断标志位
		HAL_UART_DMAStop(&uart3_handler);
		HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);
	} 
	
	
	if(__HAL_DMA_GET_FLAG(&UART3TxDMA_Handler,DMA_FLAG_HTIF3_7)!=RESET)//DMA半传输完成
    {
        __HAL_DMA_CLEAR_FLAG(&UART3TxDMA_Handler,DMA_FLAG_HTIF3_7);//清除DMA半传输完成中断标志位
	}
}
 
 

#include "mw1268_app.h"
#include "mw1268_cfg.h"
#include "usart3.h"
#include "string.h"
#include "led.h"
#include "delay.h"

#include "core.h"
//#include "bls_BN254.h"
#include "dma.h"
#include "btim.h"

//#include "stm32f7xx_hal_lptim.h"

/* Field GF(2^8)  Threshold Secret Sharing*/

//u32 TIM5_Exceed_Times;
u32 TIM9_Exceed_Times;
//u32 TIM10_Exceed_Times;


u32 obj_addr;
u8 obj_chn;
const struct uECC_Curve_t * curves[5];

//u32 TIM2_Exceed_Times;
u8 Send_Data_buff[800] = {0};


extern LPTIM_HandleTypeDef hlptim1;


//#define RECE_BUF_SIZE 500
//u8 ReceBuff[500]; 

#define PERIOD  65535
#define PULSE  32767 
u32 hash_time=100;
void tendermint2(){
	
	init();
	init_public_key();
	

//	RCC_PeriphCLKInitTypeDef RCC_PeriphCLKInitStruct = {0};
//	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
//	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSE;
//	RCC_OscInitStruct.LSEState = RCC_LSE_ON;
//	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
//	if (HAL_RCC_OscConfig(&RCC_OscInitStruct)!= HAL_OK)
//	{
//		while(1);
//		//Error_Handler(__FILE__, __LINE__);
//	}
//	RCC_PeriphCLKInitStruct.PeriphClockSelection = RCC_PERIPHCLK_LPTIM1;
//	RCC_PeriphCLKInitStruct.Lptim1ClockSelection = RCC_LPTIM1CLKSOURCE_LSE;
//	HAL_RCCEx_PeriphCLKConfig(&RCC_PeriphCLKInitStruct);

	//MX_LPTIM1_Init();
	
	
	//HAL_LPTIM_TimeOut_Start(&hlptim1, PERIOD, PULSE);
	
//	//LPTIM1->CNT = 0;
//	TIM9->CNT = 0;
//	while(1){
//		//LPTIM1->CNT = 0;
//		TIM9->CNT = 0;
//		delay_ms(1000);
//		u32 tmp_times;
//		tmp_times = TIM9->CNT;
//		tmp_times = tmp_times + 1;
//	}
//	
//	while(1);

	//sha256 init
	//calculting
	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	char digest[50];
	hash256 sh256;
	for(int i=0;i<50;i++)digest[i] = NULL;
	HASH256_init(&sh256);for(u32 j=0;j<1500;j++){for (u32 i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);HASH256_hash(&sh256,digest);}
	
//	TIM5->CNT = 0;
//	TIM5_Exceed_Times = 0;
//	u32 data_size = 500;
//	while(1){
//		TIM5->CNT = 0;
//		TIM5_Exceed_Times = 0;
//		for(u16 m=0;m<data_size/10;m++)
//		for(u32 j=0;j<1500;j++){for (u32 i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);HASH256_hash(&sh256,digest);}
//		u32 tot = TIM5->CNT + TIM5_Exceed_Times*9000;
//		sprintf((char*)Send_Data_buff,"Total_time:%d",tot);
//		LoRa_SendData(Send_Data_buff);
//	}
	
//	hash_time = TIM5->CNT;
//	hash_time = hash_time + 1;
	
	MYDMA_Config_Rece(DMA1_Stream1,DMA_CHANNEL_4);//初始化DMA
	MYDMA_Config_Send(DMA1_Stream3,DMA_CHANNEL_4);
	
	//transmit
	u32 data_size = 1000;
	for(u32 i=1;i<data_size;i++)Send_Data_buff[i-1] = 0x00+i-1;
	Send_Data_buff[data_size] = 0x00;
	//LoRa_SendData(Send_Data_buff);
	HAL_UART_Transmit_DMA(&uart3_handler,Send_Data_buff,1000);

//	for(u16 i=0;i<51;i++){
//		delay_ms(3000);
//		LoRa_SendData(Send_Data_buff);
//	}
	while(1);
	
	
	for(u32 i=0;i<RECE_BUF_SIZE;i++)ReceBuff[i]=NULL;
	
	btim_timx_enable(DISABLE);
	HAL_NVIC_DisableIRQ(BTIM_TIMX_INT_IRQn);
	HAL_NVIC_DisableIRQ(USART3_IRQn);
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,2000);//开启DMA传输
	
	
	//while(1);
	//__HAL_DMA_GET_COUNTER(&UART3RxDMA_Handler);//得到当前还剩余多少个数据
//	while(1);
	
	LED0(1);LED1(1);LED2(1);
	
}



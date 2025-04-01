#include "sys.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include "delay.h"
#include "usart.h"
#include "led.h"
#include "FreeRTOS.h"
#include "task.h"
#include "utility.h"
#include "mw1268_app.h"

#include "uECC.h"
#include "rng.h"
#include "CBC.h"
#include "btim.h"
#include "usart3.h"
#include "core.h"
#include "bls_BN158.h"
#include "dma.h"
#define USE_STM32F7XX_NUCLEO_144

#include "mbedtls/aes.h"

u32 Total_low_time = 0;
u8 low_flag = 0;
u32 low_time[400] = {0};
u32 wfi_time[400] = {0};
u32 low_IDX = 0;

u32 MY_TIME = 0;
u32 MY_Consensus_TIME=0;

u8 parallel_D = 3;

u32 TIM9_Exceed_Times;
u8 Send_Data_buff[800] = {0};

/****************ABA_Shared***************************************/
u8 cur_aba_id;
u8 state_aba;
u32 TIM5_Exceed_Times;
u32 TIM2_Exceed_Times;
u8 ABA_share[MAX_ABA][MAX_round][MAX_Nodes][Share_Size] = {0};//第x轮的哪个节点的share
u8 ACK_ABA_share[MAX_ABA][MAX_round][MAX_Nodes] = {0};//第x轮的哪个节点的share收到了
u8 ABA_share_Number[MAX_ABA][MAX_round];//第x轮share数量
u8 Shared_Coin[MAX_ABA][MAX_round];//第x轮的coin //FF->NULL
u8 COIN[Share_Size];
u8 est[MAX_ABA][MAX_round];//第x轮的估计值
u8 bin_values[MAX_ABA][MAX_round][2];

//BVAL
u8 BVAL[MAX_ABA][MAX_round][2];//发过的BVAL
u8 ACK_BVAL[MAX_ABA][MAX_round][MAX_Nodes][2];//第x轮哪个节点的0/1收到了
u8 ACK_BVAL_Number[MAX_ABA][MAX_round][2];//第x轮的0/1各收到了多少个

//AUX
u8 AUX[MAX_ABA][MAX_round][2];//发过的AUX
u8 vals[MAX_ABA][MAX_round][2];//收到的AUX
u8 ACK_AUX[MAX_ABA][MAX_round][MAX_Nodes][2];//第x轮哪个节点的0/1收到了
u8 ACK_AUX_Number[MAX_ABA][MAX_round][2];//第x轮的0/1各收到了多少个

//发送几个Round 的STATE。
u8 STATE_Set[MAX_Nodes];
u8 STATE_Set_Number;
//发送几个Round 的NACK。
u8 NACK_Set[MAX_Nodes];
u8 NACK_Set_Number;

u8 ABA_decided[MAX_ABA];//FF->NULL
u32 ABA_time[MAX_ABA];
u8 ABA_round[MAX_ABA];
u8 ABA_R[MAX_ABA];
u32 ABA_round_time[5][8];
/********************************************************************/


/****************RBC_Bracha***************************************/
u8 Block[MAX_Nodes][Block_Num][Block_Size];
u8 Block_each_Size[MAX_Nodes][Block_Num];
u8 hash_v[MAX_Nodes][Hash_Size];
u8 RBC_Block[MAX_Nodes][Block_Num];
u8 RBC_Block_Number[MAX_Nodes];
u8 RBC_Init[MAX_Nodes];

u8 ACK_RBC_Echo[MAX_Nodes][MAX_Nodes];
u8 RBC_Echo[MAX_Nodes];
u8 RBC_Echo_Number[MAX_Nodes];
u8 ACK_RBC_Ready[MAX_Nodes][MAX_Nodes];
u8 RBC_Ready[MAX_Nodes];
u8 RBC_Ready_Number[MAX_Nodes];

u8 RBC_done[MAX_Nodes];
u8 RBC_done_Number;



/***********************PRBC_Sig*************************************/
u8 ACK_PRBC_share_Number[MAX_Nodes];
u8 ACK_PRBC_share[MAX_Nodes][MAX_Nodes];
u8 PRBC_share[MAX_Nodes][MAX_Nodes][Share_Size];//第几个RBC，第几个节点的share
u8 PRBC_sig[MAX_Nodes][Share_Size];
u8 PRBC_done_Number;

u8 need_EVENT_RBC_ER = 0;


/****************************************************************/
u8 tmp_block[Block_Size];
u8 tmp_hash[Hash_Size];

/*********************CBC-commit***************************************/

u8 CBC_v[MAX_Nodes][CBC_v_SIZE];
u8 ACK_CBC_Echo[MAX_Nodes];// 自己的v的Echo
u8 CBC_Echo[MAX_Nodes];	//对其他节点的Echo
u8 CBC_Echo_Share[MAX_Nodes][Share_Size];
u8 My_CBC_Echo_Share[MAX_Nodes][Share_Size];
u8 My_CBC_Echo_Number;

u8 ACK_CBC_Finish[MAX_Nodes];
u8 CBC_Finish_Share[MAX_Nodes][Share_Size];
u8 CBC_done_Number;

/*******************CBC-value**************************************/
u8 VAL_CBC_v[MAX_Nodes][Block_Num][CBC_block_Size];
//u8 ACK_VAL_CBC_block[MAX_Nodes][Block_Num];
u8 ACK_VAL_CBC[MAX_Nodes][Block_Num];
u8 ACK_VAL_CBC_Number[MAX_Nodes];

u8 ACK_VAL_CBC_Echo[MAX_Nodes];// 自己的v的Echo
u8 VAL_CBC_Echo[MAX_Nodes];	//对其他节点的Echo
u8 VAL_CBC_Echo_Share[MAX_Nodes][Share_Size];
u8 My_VAL_CBC_Echo_Share[MAX_Nodes][Share_Size];
u8 My_VAL_CBC_Echo_Number;

u8 ACK_VAL_CBC_Finish[MAX_Nodes];
u8 VAL_CBC_Finish_Share[MAX_Nodes][Share_Size];
u8 VAL_CBC_done_Number;

u8 W_arr[2];
/*******************************************************************/

/********************Threshole Encryption*************************************/
u8 Transaction[MAX_Nodes][Proposal_Size];
u8 tx_passwd[PD_Size];

u32 Proposal_Packet_Size;

u8 enc_tx_passwd[MAX_Nodes][PD_Size];
u8 dec_tx_passwd[MAX_Nodes][PD_Size];
u8 tmp_tx_buff[MAX_PROPOSAL];
u8 tmp_enc_tx_buff[MAX_PROPOSAL];
u8 tmp_dec_tx_buff[MAX_PROPOSAL];
u8 ACK_Share[MAX_Nodes][MAX_Nodes];
u8 Tx_Share[MAX_Nodes][MAX_Nodes][Share_Size];
ECP_BN158 Tx_U[MAX_Nodes];
u8 Tx_U_str[MAX_Nodes][Share_Size];

ECP2_BN158 Tx_W[MAX_Nodes],Tx_H[MAX_Nodes];
ECP_BN158 Tx_shares[MAX_Nodes][MAX_SK];
u8 Tx_shares_str[MAX_Nodes][MAX_SK][Share_Size];
u8 Tx_shares_Number[MAX_Nodes];
u8 dec_tx_flag[MAX_Nodes];
u8 dec_tx_number;

u8 dec_done_number = 0;
u8 tmp_buff[3000];
void ENC_handler();
void Tx_init();
u8 RBC_init_Number = 0;
u8 CBC_v_number = 0;
/****************************************************************/

#ifdef __GNUC__
/* With GCC, small printf (option LD Linker->Libraries->Small printf
   set to 'Yes') calls __io_putchar() */
#define PUTCHAR_PROTOTYPE int __io_putchar(int ch)
#else
#define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)
#endif /* __GNUC__ */

//任务优先级
#define START_TASK_PRIO		1
//任务堆栈大小	
#define START_STK_SIZE 		10240  
//任务句柄
TaskHandle_t StartTask_Handler;
//任务函数
void start_task(void *pvParameters);

//任务优先级
#define EVENTSETBIT_TASK_PRIO	3
//任务堆栈大小	
#define EVENTSETBIT_STK_SIZE 	256
//任务句柄
TaskHandle_t EventSetBit_Handler;
//任务函数
void eventsetbit_task(void *pvParameters);

//任务优先级
#define EVENTGROUP_TASK_PRIO	3
//任务堆栈大小	
#define EVENTGROUP_STK_SIZE 	1024*4
//任务句柄
TaskHandle_t EventGroupTask_Handler;
//任务函数
void eventgroup_task(void *pvParameters);

u8 buff[3000];

void init(){
	while(LoRa_Init())//?????LORA???
	{
		delay_ms(300);
	}
	LoRa_CFG.chn = 10;
	LoRa_CFG.tmode = LORA_TMODE_PT;
	LoRa_CFG.addr = 0;
	LoRa_CFG.power = LORA_TPW_9DBM;
	LoRa_CFG.wlrate = LORA_RATE_62K5;
	LoRa_CFG.lbt = LORA_LBT_ENABLE;
	LoRa_Set();
}


u8 decidd_flag = 1;

void ABA_init();
void ABA_STATE_handler();
//void ABA_NACK_handler();

void RBC_init();
void RBC_INIT_handler();
void RBC_ER_handler();
void PRBC_SIG_handler();

void VAL_CBC_init();
void VAL_CBC_STATE_handler();
void VAL_CBC_STATE_EF_handler();
void CBC_init();
void CBC_STATE_handler();

void PACKET_handler();

typedef enum
{
	//TypeNACK = 0,
	TypeRBC_INIT,
	TypeRBC_ER,
	TypePRBC_SIG,
	TypeCBC_STATE,
	TypeVAL_CBC_STATE,
	TypeVAL_CBC_STATE_EF,
	TypeSTATE,
	TypeTX_ENC
} PacketType;

int main(void)
{
    /* Configure the MPU attributes */
	MPU_Config();
	//CPU_CACHE_Enable();                 //打开L1-Cache  DMA
    HAL_Init();				        //初始化HAL库
	SystemClock_Config();
	
    delay_init(216);                //延时初始化
    LED_Init();                     //初始化LED
	
    //创建开始任务
    xTaskCreate((TaskFunction_t )start_task,            //任务函数
                (const char*    )"start_task",          //任务名称
                (uint16_t       )START_STK_SIZE,        //任务堆栈大小
                (void*          )NULL,                  //传递给任务函数的参数
                (UBaseType_t    )START_TASK_PRIO,       //任务优先级
                (TaskHandle_t*  )&StartTask_Handler);   //任务句柄              
    vTaskStartScheduler();          //开启任务调度
}

//开始任务任务函数
void start_task(void *pvParameters)
{
	init();
	init_public_key();
	init_thres_sig();
	VAL_CBC_init();
	TIM5_Exceed_Times = 0;
	TIM2_Exceed_Times = 0;
	btim_tim5_int_init(9000-1,10800-1);//108Mhz/10800 = 10000hz  900ms
	btim_tim2_int_init(9000-1,10800-1);
	
    taskENTER_CRITICAL();           //进入临界区
    
	//创建设置事件位的任务
    xTaskCreate((TaskFunction_t )eventsetbit_task,             
                (const char*    )"eventsetbit_task",           
                (uint16_t       )EVENTSETBIT_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTSETBIT_TASK_PRIO,        
                (TaskHandle_t*  )&EventSetBit_Handler);   	
    //创建事件标志组处理任务
    xTaskCreate((TaskFunction_t )eventgroup_task,             
                (const char*    )"eventgroup_task",           
                (uint16_t       )EVENTGROUP_STK_SIZE,        
                (void*          )NULL,                  
                (UBaseType_t    )EVENTGROUP_TASK_PRIO,        
                (TaskHandle_t*  )&EventGroupTask_Handler);     
    vTaskDelete(StartTask_Handler); //删除开始任务

	btim_tim5_enable(1);			
    taskEXIT_CRITICAL();            //退出临界区
}




//设置事件位的任务
void eventsetbit_task(void *pvParameters)
{
	
//	btim_timx_enable(DISABLE);
//	HAL_NVIC_DisableIRQ(BTIM_TIMX_INT_IRQn);
//	HAL_NVIC_DisableIRQ(USART3_IRQn);
	
	while(1)
	{
//		STATE_Set_Number = 1;
//		STATE_Set[0] = 1;
//		NACK_Set_Number = 1;
//		NACK_Set[0] = 1;
		if(EventGroupTask_Handler!=NULL)
		{
			vTaskDelay(1000);
//			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//						(uint32_t		)EVENT_CBC_STATE,			//要更新的bit
//						(eNotifyAction	)eSetBits);				//更新指定的bit
			
			vTaskDelay(2000);
//			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//						(uint32_t		)EVENT_NACK,			//要更新的bit
//						(eNotifyAction	)eSetBits);				//更新指定的bit
//			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//						(uint32_t		)EVENTBIT_2,			//要更新的bit
//						(eNotifyAction	)eSetBits);				//更新指定的bit
			vTaskDelay(1000);	
		}

	}
}

//事件标志组处理任务
void eventgroup_task(void *pvParameters)
{
	u8 num=0,enevtvalue;
	static u8 event0flag,event1flag,event2flag;
	uint32_t NotifyValue;
	BaseType_t err;
	
	while(1)
	{
		//获取任务通知值
		err=xTaskNotifyWait((uint32_t	)0x00,				//进入函数的时候不清除任务bit
							(uint32_t	)ULONG_MAX,			//退出函数的时候清除所有的bit
							(uint32_t*	)&NotifyValue,		//保存任务通知值
							(TickType_t	)portMAX_DELAY);	//阻塞时间
		
		if(err==pdPASS)	   //任务通知获取成功
		{
			if((NotifyValue&EVENTBIT_0)!=0)			//事件0发生	
			{
				for(int i=0;i<300;i++)ReceBuff[i] = 0x00;
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT 0.");LoRa_SendData(Send_Data_buff);
			}				
			else if((NotifyValue&EVENTBIT_1)!=0)	//事件1发生	
			{
//			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//						(uint32_t		)EVENT_PACKET,			//要更新的bit
//						(eNotifyAction	)eSetBits);				//更新指定的bit
//				delay_ms(300);
//				btim_tim2_enable(1);
//				for(int i=0;i<100;i++)buff[i] = OkReceBuff[OkIdx][i];
//				sprintf((char*)Send_Data_buff,"STATE: Get EVENT 1.");LoRa_SendData(Send_Data_buff);
			}
			else if((NotifyValue&EVENTBIT_2)!=0)	//事件2发生	
			{
				for(int i=0;i<300;i++)ReceBuff[i] = 0x00;
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT 2.");LoRa_SendData(Send_Data_buff);	
			}
			else if((NotifyValue&EVENT_PACKET)!=0)	//PACKET	
			{
				PACKET_handler();
			}
			else if((NotifyValue&EVENT_VAL_CBC_STATE)!=0)	//PACKET	
			{
				if(ID <= parallel_D){
					u32 random = RNG_Get_RandomRange(0, 5);
					random = random * 200;
					delay_ms(random);
					VAL_CBC_STATE_handler();
				}
			}
			else if((NotifyValue&EVENT_VAL_CBC_EF_STATE)!=0)	//PACKET	
			{
				VAL_CBC_STATE_EF_handler();
			}
		}
		else
		{
			sprintf((char*)Send_Data_buff,"ERROR: Get mission.");
			LoRa_SendData(Send_Data_buff);	
		}
	}
}

/********************************Multi-Hop************************************************/
int PAD(u8 *str, int len){
	//int _n = strlen(str);
	int n = 16 - (len % 16);
	for(int i=0;i<n;i++){
		str[len+i] = n;
	}
	return n + len;
}

void UNPAD(u8 *str, int len){
//	int _n = strlen(str);
	int n = str[len-1];
	for(int i=0;i<n;i++){
		str[len-i-1] = 0x00;
	}
}

int Tx_encrypt(u8 *key, u8 *raw, int raw_len, u8 *enc){
	int pad_len = raw_len;// = PAD(raw, raw_len);
	u8 iv[16];
	u8 iv2[16];
	for(u8 i=0;i<16;i++){
		iv[i] = i+1;//RNG_Get_RandomNum();
		iv2[i] = iv[i];
	}
	
	mbedtls_aes_context ctx;
	
	// 加密
	mbedtls_aes_setkey_enc(&ctx, (unsigned char *)key, 128);
	mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_ENCRYPT, pad_len, iv, raw, enc);	
	
	for(int i=0;i<pad_len;i++)tmp_buff[i] = enc[i];
	for(int i=0;i<16;i++)enc[i] = iv2[i];
	for(int i=0;i<pad_len;i++)enc[16+i] = tmp_buff[i];
	
	return pad_len+16;
}
	
void Tx_decrypt(u8 *key, u8 *enc, int enc_len, u8 *dec){
	u8 iv[16];
	for(int i=0;i<16;i++)iv[i] = enc[i];
	//int enc_len = strlen(enc);
	for(int i=0;i<enc_len + 16;i++)tmp_buff[i] = enc[i];
	for(int i=0;i<enc_len;i++)enc[i] = 0x00;
	for(int i=0;i<enc_len;i++)enc[i] = tmp_buff[16 + i];
	
	mbedtls_aes_context ctx;
	// 解密
	mbedtls_aes_setkey_dec(&ctx, (unsigned char *)key, 128);
	mbedtls_aes_crypt_cbc(&ctx, MBEDTLS_AES_DECRYPT, enc_len-16, iv, enc, dec);
}
void Tx_init(){}
/*************************************************************************************/

void VAL_CBC_init(){
	
	My_VAL_CBC_Echo_Number = 0;
	for(int i=0;i<MAX_Nodes;i++){
		ACK_VAL_CBC_Echo[i] = 0;
		VAL_CBC_Echo[i] = 0;
		ACK_VAL_CBC_Finish[i] = 0;
		for(int j=0;j<Share_Size;j++){
			VAL_CBC_Echo_Share[i][j] = 0x00;
			My_VAL_CBC_Echo_Share[i][j] = 0x00;
			VAL_CBC_Finish_Share[i][j] = 0x00;
		}
	}
	for(int i=0;i<MAX_Nodes;i++){
		ACK_VAL_CBC_Number[i] = 0x00;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_VAL_CBC[i][j] = 0x00;
		}
	}
	
//init VAL_CBC_v[]
	for(int i=0;i<Block_Num ;i++){
		ACK_VAL_CBC[ID][i] = 1;
		for(int j=0;j<CBC_block_Size;j++)
			VAL_CBC_v[ID][i][j] = RNG_Get_RandomNum();
		ACK_VAL_CBC_Number[ID]++;
	}
	
	VAL_CBC_SK_sign(ID);
	
	VAL_CBC_done_Number = 0;
	W_arr[0] = W_arr[1] = 0;
	
	MYDMA_Config_Rece(DMA1_Stream1,DMA_CHANNEL_4);//初始化DMA
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);
}

void CBC_init(){}

void RBC_init(){}

void ABA_init(){}

u8 ttt = 0;
u8 xxx = 0;
u8 mmm = 0;
u8 nnn = 0;

int subset_or_not(u8 aba, u8 round){}

void ABA_STATE_handler(){}

void RBC_INIT_handler(){}

void RBC_ER_handler(){}

void PRBC_SIG_handler(){}

void CBC_STATE_handler(){}

void VAL_CBC_STATE_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	for(int block_id = 0;block_id<Block_Num;block_id++){
		//start
		u8 padding = 0;
		Send_Data_buff[0] = ID;
		Send_Data_buff[1] = TypeVAL_CBC_STATE;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		
		u8 packet_idx = 3 + padding;
		
		//echo nack 没有收到谁的echo
		for(int k=1 ; k<=Nodes_N ; k++){
			if(k == ID)continue;
			if(ACK_VAL_CBC_Echo[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		if(My_VAL_CBC_Echo_Number >= 2 * Nodes_f + 1) Send_Data_buff[packet_idx] = 0xFF;
		packet_idx++;
		
		//Finish nack
		for(int k=1 ; k<=Nodes_N ; k++){
			if(ACK_VAL_CBC_Finish[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		packet_idx++;
		
		//Init nack
		for(int k=1;k<=Nodes_N;k++){
			if(ACK_VAL_CBC_Number[k] == Block_Num){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		packet_idx++;
		//block_id
		Send_Data_buff[packet_idx] = block_id;
		
		packet_idx++;
		for(int i=0;i<CBC_block_Size;i++){
			Send_Data_buff[packet_idx+i] = VAL_CBC_v[ID][block_id][i];
		}
		packet_idx += CBC_block_Size;
		
		//Sign
		uint8_t hash[32] = {0};
		uint8_t sig[64] = {0};
		//sha2(Send_Data_buff,packet_idx,hash,0);
		hash256 sh256;
		HASH256_init(&sh256);
		for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
		HASH256_hash(&sh256,hash);
		
		sign_data(hash,sig);
		for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];
		
//		HAL_UART_DMAStop(&uart3_handler);
//		HAL_UART_Transmit_DMA(&uart3_handler,Send_Data_buff,packet_idx+Sig_Size);
		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		u32 random = RNG_Get_RandomRange(0, 5);
		random = random * 50;
		delay_ms(random);
	}
	
}



void VAL_CBC_STATE_EF_handler(){
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	if(Nodes_N < 8){
		u8 padding = 0;
		
		u8 sig_num = 0;
		for(int k=1 ; k <= Nodes_N ; k++){
			if(VAL_CBC_Echo[k]){
				sig_num++;
			}
		}

		if(sig_num >= 2) padding = 60;
		if(sig_num < 2) padding = 60;
		
		
		Send_Data_buff[0] = ID;
		Send_Data_buff[1] = TypeVAL_CBC_STATE_EF;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		
		u8 packet_idx = 3 + padding;
		
		//echo ack 已有的echo
		for(int k=1 ; k <= Nodes_N ; k++){
			if(VAL_CBC_Echo[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		packet_idx++;
		//echo nack 没有收到谁的echo
		for(int k=1 ; k<=Nodes_N ; k++){
			if(k == ID)continue;
			if(ACK_VAL_CBC_Echo[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		if(My_VAL_CBC_Echo_Number >= 2 * Nodes_f + 1) Send_Data_buff[packet_idx] = 0xFF;
		packet_idx++;
		
		//Finish nack
		for(int k=1 ; k<=Nodes_N ; k++){
			if(ACK_VAL_CBC_Finish[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
			if(k == ID && My_VAL_CBC_Echo_Number >= 2 * Nodes_f + 1){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			} 
		}
		packet_idx++;
		
		//Init nack
		for(int k=1;k<=Nodes_N;k++){
			if(ACK_VAL_CBC_Number[k] == Block_Num){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		packet_idx++;
		
		//signature for other v
		for(int k=1 ; k <= Nodes_N ; k++){
			if(VAL_CBC_Echo[k]){
				for(int i=0;i<Share_Size;i++){
					Send_Data_buff[packet_idx+i] = VAL_CBC_Echo_Share[k][i];
				}
			packet_idx+=Share_Size;
			}
		} 
			
		//Finish
		if(My_VAL_CBC_Echo_Number >= 2 * Nodes_f + 1){
			if( ACK_VAL_CBC_Finish[ID] == 0){
				VAL_CBC_PK_sig_combine_share();
				if(VAL_CBC_verify_sig(ID) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"verify Thres Fail11111111");
						Lora_Device_Sta = LORA_TX_STA;
						LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
					}
				}
				ACK_VAL_CBC_Finish[ID] = 1;
				
				VAL_CBC_done_Number++;
				
				if(VAL_CBC_done_Number == Nodes_N){
					if(ID == 1 && VAL_CBC_done_Number == Nodes_N && MY_Consensus_TIME==0){
						LoRa_CFG.chn = 66;
						LoRa_Set();	
						
						MY_Consensus_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
						sprintf((char*)Send_Data_buff,"CBC-%d-%d",Block_Num,MY_Consensus_TIME);
						LoRa_SendData(Send_Data_buff);
						while(1);
					}
				}
			}
			
			for(int i=0;i<Share_Size;i++){
				Send_Data_buff[packet_idx+i] = VAL_CBC_Finish_Share[ID][i];
			}
			packet_idx+=Share_Size;
			
			if(VAL_CBC_verify_sig(ID) == 0){
				while(1){
					sprintf((char*)Send_Data_buff,"verify Thres Fail22222222222");
					Lora_Device_Sta = LORA_TX_STA;
					LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
				}
			}
		}
	
		//Sign
		uint8_t hash[32] = {0};
		uint8_t sig[64] = {0};
		//sha2(Send_Data_buff,packet_idx,hash,0);
		hash256 sh256;
		HASH256_init(&sh256);
		for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
		HASH256_hash(&sh256,hash);
		
		sign_data(hash,sig);
		for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];
		
//		HAL_UART_DMAStop(&uart3_handler);
//		HAL_UART_Transmit_DMA(&uart3_handler,Send_Data_buff,packet_idx+Sig_Size);
		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
	}
}

void ENC_handler(){}

void PACKET_handler(){
	ttt = ttt + 1;
	delay_ms(20);
//	ttt = ttt + 1;
//	HAL_UART_DMAStop(&uart3_handler);
//	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);//开启DMA传输
//	return;
//	if(ReceBuff[0] == 0x00||ReceBuff[0] > Nodes_N || ReceBuff[1] > 20 || ReceBuff[2] > Nodes_N){
	if(ReceBuff[0] == 0x00||ReceBuff[0] > Nodes_N || ReceBuff[1] > 20){
		mmm++;
		HAL_UART_DMAStop(&uart3_handler);
		HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);//开启DMA传输
		return;
	}
	nnn++;
	if(Nodes_N > 8)while(1);
	
	//现在是一个一个处理的
	//之后修改为接收多个，
	
	for(int i=0;i<300;i++)buff[i] = ReceBuff[i];
	//验证签名
	u8 len = DMA_buff_Size - UART3RxDMA_Handler.Instance->NDTR;
	if(len != 64 && len != 89) xxx++;
	u8 s_id = buff[0];
	u8 packet_type = buff[1];
	uint8_t hash[32] = {0};
	uint8_t sig[64] = {0};
	u8 verify_flag = 1;
	
	//sha2(buff,len-Sig_Size,hash,0);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<len-Sig_Size;i++) HASH256_process(&sh256,buff[i]);
	HASH256_hash(&sh256,hash);
	
	int tmp_idx=0;
	for(int i=len-Sig_Size;i<len;i++){
		sig[tmp_idx] = buff[i];
		tmp_idx = tmp_idx + 1;
	}
	if (!uECC_verify(public_key[s_id], hash, 32, sig, uECC_secp160r1())) {
		verify_flag = 0;
	}
	
	if(packet_type == TypeVAL_CBC_STATE_EF && verify_flag){
		u8 EF_flag = 0;
		u8 init_flag = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		u8 echo_ack = buff[packet_idx + 0];
		u8 echo_nack = buff[packet_idx + 1];
		u8 finish_nack = buff[packet_idx + 2];
		u8 init_nack = buff[packet_idx + 3];
		
		packet_idx += 4;
		
		//finish nack
		if((finish_nack & (1 << (ID - 1))) == 0 && ACK_VAL_CBC_Finish[ID] == 1){
			EF_flag = 1;
		}
		
		//init nack
		if((init_nack & (1 << (ID - 1))) == 0){
			init_flag = 1;
		}
		
		//echo nack
		if((echo_nack & (1<<(ID-1))) == 0 && VAL_CBC_Echo[s_id] == 1){
			EF_flag = 1;
		}
		
		//signature for ID
		for(int k=1;k<=Nodes_N;k++){
			if((echo_ack & (1<<(k-1))) != 0 && k == ID && ACK_VAL_CBC_Echo[s_id] == 0 && k <= parallel_D){
				for(int i=0;i<Share_Size;i++){
					My_VAL_CBC_Echo_Share[s_id][i] = buff[packet_idx + i];
				}
				
				//验证share, verify
				if(VAL_CBC_PK_verify_sig_share(s_id) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"verify share Thres Fail11");
						Lora_Device_Sta = LORA_TX_STA;
						LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
					}
				}
				ACK_VAL_CBC_Echo[s_id] = 1;
				My_VAL_CBC_Echo_Number++;
				if(My_VAL_CBC_Echo_Number >= 2 * Nodes_f + 1){	
					if( ACK_VAL_CBC_Finish[ID] == 0){
						VAL_CBC_PK_sig_combine_share();
						if(VAL_CBC_verify_sig(ID) == 0){
							while(1){
								sprintf((char*)Send_Data_buff,"verify Thres Fail11111111");
								Lora_Device_Sta = LORA_TX_STA;
								LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
							}
						}
						ACK_VAL_CBC_Finish[ID] = 1;
						VAL_CBC_done_Number++;
						if(VAL_CBC_done_Number == parallel_D){
							if(ID == 1 && VAL_CBC_done_Number == parallel_D && MY_Consensus_TIME==0){
								LoRa_CFG.chn = 77;
								LoRa_Set();	
								
								MY_Consensus_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
								sprintf((char*)Send_Data_buff,"CBC-parallel-%d-%d",parallel_D,MY_Consensus_TIME);
								LoRa_SendData(Send_Data_buff);
								while(1);
							}
						}
					}
				}
			}
			if((echo_ack & (1<<(k-1))) != 0)
				packet_idx += Share_Size;
		}
		//signature for finish s_id
		if((finish_nack & (1<<(s_id-1))) != 0 && ACK_VAL_CBC_Finish[s_id] == 0 && ACK_VAL_CBC_Number[s_id] == Block_Num && s_id <= parallel_D){
			for(int i=0;i<Share_Size;i++){
				VAL_CBC_Finish_Share[s_id][i] = buff[packet_idx + i];
			}
			//验证share, verify
			if(VAL_CBC_verify_sig(s_id) == 0){
				while(1){
					sprintf((char*)Send_Data_buff,"verify Thres Fail");
					Lora_Device_Sta = LORA_TX_STA;
					LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
				}
			}
			//verify share
			ACK_VAL_CBC_Finish[s_id] = 1;
			
			VAL_CBC_done_Number++;
			if(ID == 1 && VAL_CBC_done_Number == parallel_D && MY_Consensus_TIME==0){
				LoRa_CFG.chn = 77;
				LoRa_Set();	
				
				MY_Consensus_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
				sprintf((char*)Send_Data_buff,"CBC-parallel-%d-%d",parallel_D,MY_Consensus_TIME);
				LoRa_SendData(Send_Data_buff);
				while(1);
			}
		}
		
		if(EF_flag){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_VAL_CBC_EF_STATE,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		if(init_flag && ID <= parallel_D){
			if(EF_flag)delay_ms(500);
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_VAL_CBC_STATE,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
	}
	else if(packet_type == TypeVAL_CBC_STATE && verify_flag){
		u8 EF_flag = 0;
		u8 init_flag = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		u8 echo_nack = buff[packet_idx + 0];
		u8 finish_nack = buff[packet_idx + 1];
		u8 init_nack = buff[packet_idx + 2];
		u8 block_id = buff[packet_idx + 3];
		
		packet_idx += 4;
		
		if(ACK_VAL_CBC[s_id][block_id] == 0){
			ACK_VAL_CBC[s_id][block_id] = 1;
			ACK_VAL_CBC_Number[s_id]++;
			for(int i=0;i<CBC_block_Size;i++){
				VAL_CBC_v[s_id][block_id][i] = buff[packet_idx + i];
			}
			if(ACK_VAL_CBC_Number[s_id]==Block_Num){
				VAL_CBC_SK_sign(s_id);
			}
		}
		
	}

	HAL_UART_DMAStop(&uart3_handler);
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);//开启DMA传输
}










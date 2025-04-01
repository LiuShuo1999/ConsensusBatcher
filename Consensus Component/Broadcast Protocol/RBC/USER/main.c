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
#include "RBC.h"
#include "btim.h"
#include "usart3.h"
#include "core.h"
#include "bls_BN254.h"
#include "dma.h"

#include "mbedtls/aes.h"

u32 TIM9_Exceed_Times;
u8 Send_Data_buff[800] = {0};

#define USE_STM32F7XX_NUCLEO_144

u32 Total_low_time = 0;
u8 low_flag = 0;
u32 low_time[400] = {0};
u32 wfi_time[400] = {0};
u32 low_IDX = 0;

u32 MY_TIME=0;
u32 MY_Consensus_TIME = 0;
u32 time_run = 0;
u8 need_er = 0;

u8 parallel_D = 3;

/****************ABA_Shared***************************************/
u8 start_ABA = 0;

u32 TIM5_Exceed_Times;
u32 TIM2_Exceed_Times;
u8 ABA_share[MAX_round][MAX_Nodes][Share_Size] = {0};//第x轮的哪个节点的share
u8 ACK_ABA_share[MAX_round][MAX_Nodes] = {0};//第x轮的哪个节点的share收到了
u8 ABA_share_Number[MAX_round];//第x轮share数量
u8 Shared_Coin[MAX_round];//第x轮的coin //FF->NULL
u8 COIN[Share_Size];
u8 ABA_sig[MAX_round][Share_Size];

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
u8 ABA_done_Number;
u32 ABA_time[MAX_ABA];
u8 ABA_round[MAX_ABA];
u8 ABA_R[MAX_ABA];
u32 ABA_round_time[5][8];
/*******************************************************/


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
u8 RBC_init_Number = 0;
/****************************************************************/



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
ECP_BN254 Tx_U[MAX_Nodes];
u8 Tx_U_str[MAX_Nodes][Share_Size];

ECP2_BN254 Tx_W[MAX_Nodes],Tx_H[MAX_Nodes];
ECP_BN254 Tx_shares[MAX_Nodes][MAX_SK];
u8 Tx_shares_str[MAX_Nodes][MAX_SK][Share_Size];
u8 Tx_shares_Number[MAX_Nodes];
u8 dec_tx_flag[MAX_Nodes];
u8 dec_tx_number;

u8 dec_done_number = 0;
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
#define EVENTSETBIT_STK_SIZE 	512
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

u8 buff[300];

void init(){
	while(LoRa_Init())//?????LORA???
	{
		delay_ms(300);
	}
	LoRa_CFG.chn = 30;
	LoRa_CFG.tmode = LORA_TMODE_PT;
	LoRa_CFG.addr = 0;
	LoRa_CFG.power = LORA_TPW_9DBM;
	LoRa_CFG.wlrate = LORA_RATE_62K5;
	LoRa_CFG.lbt = LORA_LBT_ENABLE;
	LoRa_Set();
}

void RBC_init();

u8 decidd_flag = 1;
void ABA_init();
void ABA_STATE_handler();
void ABA_NACK_handler();
void RBC_INIT_handler();
void RBC_ER_handler();
void PACKET_handler();
void Tx_init();
void ENC_handler();
typedef enum
{
	TypeNACK = 0,
	TypeSTATE,
	TypeRBC_INIT,
	TypeRBC_ER,
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




u8 tmp_buff[3000];
u8 tx_buff[300];
u8 cipher_buff[300];
u8 decrypt_buff[300];
u8 PASSWD[PD_Size];

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



//开始任务任务函数
void start_task(void *pvParameters)
{

//	for(int i=0;i<300;i++){
//		tx_buff[i] = 0x00;
//		tmp_buff[i] = 0x00;
//		cipher_buff[i] = 0x00;
//		decrypt_buff[i] = 0x00;
//	}
//	for(int i=0;i<198;i++)tx_buff[i] = 'a';
//	for(int i=0;i<16;i++)PASSWD[i] = 'a'+i;
//	int len = Tx_encrypt(PASSWD, tx_buff, 198, cipher_buff);
//	Tx_decrypt(PASSWD,cipher_buff,len,decrypt_buff);
//	UNPAD(decrypt_buff,len);
	
	init();
	init_public_key();

	RBC_init();
	

	//test_thres();
	TIM5_Exceed_Times = 0;
	TIM2_Exceed_Times = 0;
	btim_tim5_int_init(9000-1,10800-1);//108Mhz/10800 = 10000hz  900ms
	btim_tim2_int_init(9000-1,10800-1);

//	TIM2->CNT = 0;
//	delay_ms(200);
//	btim_tim2_enable(0);
//	while(1);

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

	while(1)
	{

		if(EventGroupTask_Handler!=NULL)
		{
			vTaskDelay(1000);
//			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//						(uint32_t		)EVENTBIT_1,			//要更新的bit
//						(eNotifyAction	)eSetBits);				//更新指定的bit
			
			vTaskDelay(2000);
//			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//						(uint32_t		)EVENT_ABA_NACK,			//要更新的bit
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
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT_PACKET.");LoRa_SendData(Send_Data_buff);
				PACKET_handler();
			}
			else if((NotifyValue&EVENT_RBC_INIT)!=0)	//PACKET	
			{
				if(ID <= parallel_D){
					u32 random = RNG_Get_RandomRange(0, 5);
					random = random * 200;
					delay_ms(random);
					
					RBC_INIT_handler();
				}
				
			}
			else if((NotifyValue&EVENT_RBC_ER)!=0)	//PACKET	
			{
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT_PACKET.");LoRa_SendData(Send_Data_buff);
				RBC_ER_handler();
				
			}
			else if((NotifyValue&EVENT_ENC)!=0)	//PACKET	
			{
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT_PACKET.");LoRa_SendData(Send_Data_buff);
				if(ABA_done_Number == Nodes_N)
					ENC_handler();
			}

		}
		else
		{
			sprintf((char*)Send_Data_buff,"ERROR: Get mission.");
			LoRa_SendData(Send_Data_buff);	
		}
	}
}


void Tx_init(){

}

void RBC_init(){
	RBC_init_Number = 0;
	for(int i=0;i<MAX_Nodes;i++){
		RBC_Init[i] = 0;
		RBC_Echo[i] = 0;
		RBC_Ready[i] = 0;
		RBC_Echo_Number[i] = 0;
		RBC_Ready_Number[i] = 0;
		RBC_done[i] = 0;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_RBC_Echo[i][j] = 0;
			ACK_RBC_Ready[i][j] = 0;
		}
	}
	// init v
	RBC_Init[ID] = 1;
	RBC_Echo[ID] = 1;
	//tmp_tx_buff[passwd + U + tx]
	u32 packet_idx = 0;
	for(int b=0;b<Block_Num;b++){
		Block_each_Size[ID][b] = Block_Size;
		for(int i=0;i<Block_each_Size[ID][b];i++){
			Block[ID][b][i] = RNG_Get_RandomNum();	
		}
		packet_idx += Block_each_Size[ID][b];
	}

	hash256 sh256;
	HASH256_init(&sh256);
	for(int b=0;b<Block_Num;b++)
	for (int i=0;i<Block_each_Size[ID][b];i++) HASH256_process(&sh256,Block[ID][b][i]);
    HASH256_hash(&sh256,hash_v[ID]);
	
	for(int i=0;i<MAX_Nodes;i++){
		RBC_Block_Number[i] = 0;
		for(int b=0;b<Block_Num;b++){
			RBC_Block[i][b] = 0;
		}
	}
	
	RBC_done_Number = 0;
	
	MYDMA_Config_Rece(DMA1_Stream1,DMA_CHANNEL_4);//初始化DMA
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);
}
	
void ABA_init(){}
u8 ttt = 0;
u8 xxx = 0;
u8 mmm = 0;
u8 nnn = 0;
int subset_or_not(u8 aba, u8 round){
}


void ABA_STATE_handler(){
}

void ABA_NACK_handler(){
}



void RBC_INIT_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	if(Nodes_N <= 8){
	for(int block_id = 0;block_id<Block_Num;block_id++){
		u8 padding = 0;
		if(Block_each_Size[ID][block_id] < 100)padding = 100 - Block_each_Size[ID][block_id];
		Send_Data_buff[0] = ID;
		Send_Data_buff[1] = TypeRBC_INIT;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		
		u8 packet_idx = 3 + padding;
		Send_Data_buff[packet_idx] = block_id;
		packet_idx++;
		for(int k=1;k<=Nodes_N;k++){
			if(RBC_Init[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		packet_idx++;
		u8 this_block_size = Block_each_Size[ID][block_id];
		Send_Data_buff[packet_idx] = this_block_size;
		packet_idx++;
		for(int i=0;i<this_block_size;i++){
			Send_Data_buff[packet_idx + i] = Block[ID][block_id][i];
		}
		packet_idx += this_block_size;
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

}

void RBC_ER_handler(){
	//DMA send ER packet
	//只实现了16节点以下的情况。分情况讨论的
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	if(Nodes_N <= 8){
		u8 padding = 0;
		Send_Data_buff[0] = ID;
		Send_Data_buff[1] = TypeRBC_ER;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		u8 packet_idx = 2 + padding + 1;
		//ACK_init
		for(int k=1;k<=Nodes_N;k++){
			if(RBC_Init[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		
		//ACK_Echo
		for(int k=1;k<=Nodes_N;k++){
			if(RBC_Echo_Number[k] >= Nodes_N - Nodes_f)
				Send_Data_buff[packet_idx + 1] |= (1<<(k-1));
		}
		//Echo
		for(int k=1;k<=Nodes_N;k++){
			if(RBC_Echo[k])
				Send_Data_buff[packet_idx + 2] |= (1<<(k-1));
		}
		//ACK_Ready
		for(int k=1;k<=Nodes_N;k++){
			if(RBC_Ready_Number[k] >= Nodes_N - Nodes_f)
				Send_Data_buff[packet_idx + 3] |= (1<<(k-1));
		}
		//Ready
		for(int k=1;k<=Nodes_N;k++){
			if(RBC_Ready[k])
				Send_Data_buff[packet_idx + 4] |= (1<<(k-1));
		}
		packet_idx += 5;
		
		//add hash * N
		for(int k=1;k<=Nodes_N;k++){
			for(int i=0;i<Hash_Size;i++){
				Send_Data_buff[packet_idx + i] = hash_v[k][i];
			}
			packet_idx += Hash_Size;
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
	
	if(packet_type == TypeRBC_INIT && verify_flag){
		u8 reply_flag = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		u8 block_id = buff[packet_idx];packet_idx++;
		u8 init_ACK = buff[packet_idx];

		if((init_ACK & (1<<(ID-1)))==0){
			reply_flag = 1;
		}
		packet_idx++;
		u8 this_block_size = buff[packet_idx];
		packet_idx++;
		if(RBC_Block[s_id][block_id] == 0){
			RBC_Block[s_id][block_id] = 1;
			for(int i=0;i<this_block_size;i++){
				Block[s_id][block_id][i] = buff[packet_idx+i];
			}
			Block_each_Size[s_id][block_id] = this_block_size;
			RBC_Block_Number[s_id]++;
			
			if(RBC_Block_Number[s_id] == Block_Num){
				RBC_Init[s_id] = 1;
				RBC_init_Number++;
				//sha2(Block[s_id], Block_Size, hash_v[s_id], 0);
				hash256 sh256;
				HASH256_init(&sh256);
				
				for(int b=0;b<Block_Num;b++)
				for(int i=0;i<Block_each_Size[s_id][b];i++) 
					HASH256_process(&sh256,Block[s_id][b][i]);
				HASH256_hash(&sh256,hash_v[s_id]);
				
				RBC_Echo[s_id] = 1;
				RBC_Echo_Number[s_id] += 2;
				ACK_RBC_Echo[s_id][s_id] = 1;
			}
			if(block_id == 0){
				for(int i=0;i<Share_Size;i++)
					Tx_U_str[s_id][i] = Block[s_id][block_id][PD_Size+i];
				
				octet tmp_U;
				tmp_U.len = tmp_U.max = Share_Size;
				tmp_U.val = (char*)Tx_U_str[s_id];
				ECP_BN254_fromOctet(&Tx_U[s_id],&tmp_U);
				
				for(int i=0;i<PD_Size;i++)
					enc_tx_passwd[s_id][i] = Block[s_id][block_id][i];
			}
		}
		if(reply_flag){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_RBC_INIT,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		
	}
	else if(packet_type == TypeRBC_ER && verify_flag){
		u8 reply_er = 0;
		u8 reply_init = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		u8 init_ACK = buff[packet_idx + 0];
		u8 echo_ACK = buff[packet_idx + 1];
		u8 echo_VAL = buff[packet_idx + 2];
		u8 ready_ACK = buff[packet_idx + 3];
		u8 ready_VAL = buff[packet_idx + 4];
		
		//ACK_init
		if((init_ACK & (1<<(ID-1))) == 0){
			reply_init = 1;
		}
		//ACK
		for(int k = 1 ; k <= parallel_D ;k++){
			if((echo_ACK & (1<<(k-1))) == 0 && RBC_Echo[k] == 1){
				reply_er = 1;
				break;
			}
			if((ready_ACK & (1<<(k-1))) == 0 && RBC_Ready[k] == 1){
				reply_er = 1;
				break;
			}
		}		
		packet_idx += 5;
		u8 check_list[Nodes_N+1];
		for(int i=0;i<Nodes_N+1;i++)check_list[i] = 1;
		//check hash
		for(int k=1;k<=Nodes_N;k++){
			for(int i=0;i<Hash_Size;i++){
				if(buff[packet_idx+i] != hash_v[k][i]){
					check_list[k] = 0;
					break;
				}
			}
			packet_idx += Hash_Size;
		}
		//VAL
		for(int k=1;k<=parallel_D;k++){
			if(!check_list[k])continue;
			
			if((echo_VAL & (1<<(k-1))) != 0 && ACK_RBC_Echo[k][s_id] == 0){
				ACK_RBC_Echo[k][s_id] = 1;
				RBC_Echo_Number[k]++;
				if(RBC_Echo_Number[k] == Nodes_N - Nodes_f){
					RBC_Ready[k] = 1;
					RBC_Ready_Number[k]++;
					if(RBC_Ready_Number[k] == Nodes_N - Nodes_f){
						//accept it
						if(RBC_done[k] == 0){
							RBC_done[k] = 1;
							RBC_done_Number++;
							
							if(ID == 1 && RBC_done_Number == parallel_D ){
								LoRa_CFG.chn = 55;
								LoRa_Set();	
								
								u32 MY_Consensus_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
								sprintf((char*)Send_Data_buff,"RBC-parallel-%d-%d",parallel_D,MY_Consensus_TIME);
								LoRa_SendData(Send_Data_buff);
								while(1);
							}
						}
					}
					
				}
			}
			if((ready_VAL & (1<<(k-1))) != 0 && ACK_RBC_Ready[k][s_id] == 0){
				RBC_Ready_Number[k]++;
				ACK_RBC_Ready[k][s_id] =1;
				if(RBC_Ready_Number[k] == Nodes_N - Nodes_f){
					//accept it
					if(RBC_done[k] == 0){
						RBC_done[k] = 1;
						RBC_done_Number++;
						
						if(ID == 1 && RBC_done_Number == parallel_D ){
							LoRa_CFG.chn = 55;
							LoRa_Set();	
							
							u32 MY_Consensus_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
							sprintf((char*)Send_Data_buff,"RBC-parallel-%d-%d",parallel_D,MY_Consensus_TIME);
							LoRa_SendData(Send_Data_buff);
							while(1);
						}
					}
				}
			}
		
		}
		if(reply_er){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_RBC_ER,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		if(reply_init && ID <= parallel_D){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_RBC_INIT,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		
	}

	
//	for(int i=0;i<300;i++)ReceBuff[i] = 0x00;
	HAL_UART_DMAStop(&uart3_handler);
////	HAL_DMA_Init(&UART3RxDMA_Handler);
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);//开启DMA传输
	
	
}










///





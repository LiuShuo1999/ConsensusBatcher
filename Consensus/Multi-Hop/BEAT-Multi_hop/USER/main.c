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
#include "BEAT.h"
#include "btim.h"
#include "usart3.h"
#include "core.h"
#include "bls_BN158.h"
#include "dma.h"

#include "mbedtls/aes.h"

#define USE_STM32F7XX_NUCLEO_144

u32 Total_low_time = 0;
u8 low_flag = 0;
u32 low_time[400] = {0};
u32 wfi_time[400] = {0};
u32 low_IDX = 0;

u32 MY_TIME=0;

u8 need_er = 0;
u32 MY_Consensus_TIME=0;
u32 TIM9_Exceed_Times;
u8 Send_Data_buff[800] = {0};

/****************ABA_Shared***************************************/
u8 start_ABA = 0;

u32 TIM5_Exceed_Times;
u32 TIM2_Exceed_Times;
u8 ABA_share[MAX_round][MAX_Nodes][Share_Size] = {0};//第x轮的哪个节点的share
BIG_160_28 ABA_Z[MAX_round][MAX_Nodes];
u8 ABA_Z_share[MAX_round][MAX_Nodes][32];
BIG_160_28 ABA_C[MAX_round][MAX_Nodes];
u8 ABA_C_share[MAX_round][MAX_Nodes][32];

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

//下面的也用于多跳
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

/****************************************************************/

/***********************Multi-Hop*************************/
u8 start_Global_Consensus = 0;
u8 Global_Transaction[Global_Group_Number][(Group_per_Nodes-1)*Proposal_Size];
u32 Global_Proposal_Size[Global_Group_Number];
u8 Global_hash_tx[Global_Group_Number][Hash_Size];
u8 Global_Sig_hash[Global_Nodes][Sig_Size];
u8 ACK_Global_Sig_hash[Global_Nodes];

u8 Global_Sig_hash_Number;

u8 Global_tmp_tx_buff[Global_MAX_PROPOSAL];
u32 Global_Proposal_Packet_Size;

//global rbc
u8 Global_Block[Global_Group_Number][Global_Block_Num][Block_Size];
u8 Global_Block_each_Size[Global_Group_Number][Global_Block_Num];
u8 Global_Group_Block_Num[Global_Group_Number];//block tot num
u8 Global_RBC_Block_Number[Global_Group_Number];
u8 Global_RBC_Block[Global_Group_Number][MAX_Blocks];


void Global_TX_init();
void Global_RBC_init();
void Global_ABA_init();

void Global_Consensus_Init();
void SIG_handler();
void Global_RBC_INIT_handler();
void test_global_init();
/********************************************************/




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
	LoRa_CFG.chn = 10 * Group_ID;
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
	TypeTX_ENC,
	TypeTX_SIG,
	Global_TypeRBC_INIT
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
u8 tx_buff[600];
u8 cipher_buff[600];
u8 decrypt_buff[600];
u8 PASSWD[PD_Size];

int Tx_encrypt(u8 *key, u8 *raw, int raw_len, u8 *enc){
	//int pad_len = PAD(raw, raw_len);
	int pad_len = raw_len;
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
	
	start_Global_Consensus = 0;
	init();
	init_public_key();
	init_thres_enc();
	
/***********test********************/
//	
//	Global_TX_init();
//	while(1);
/***********************************/
	ABA_init();
	Tx_init();
	RBC_init();
//	/************test global consensus*******************/
//	
//	test_global_init();
//	Global_Consensus_Init();
//	/****************************************************/
	
	//test_thres();
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

void test_global_init(){
	ABA_decided[1] = 1;
	u32 tx_idx = 0;
	for(int k=1;k<=3;k++){
		for(int i=0;i<Proposal_Size;i++){
			Global_Transaction[Group_ID][tx_idx] = k + '1';
			tx_idx++;
		}
	}
	if(Group_ID == 3){
		u8 k = 4;
		for(int i=0;i<Proposal_Size;i++){
			Global_Transaction[Group_ID][tx_idx] = k + '1';
			tx_idx++;
		}
	}
		
	Global_Proposal_Size[Group_ID] = tx_idx;
	
	
	//hash
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<tx_idx;i++) HASH256_process(&sh256,Global_Transaction[Group_ID][i]);
	HASH256_hash(&sh256,Global_hash_tx[Group_ID]);
	//Sign
	sign_data(Global_hash_tx[Group_ID],Global_Sig_hash[Global_ID]);
	ACK_Global_Sig_hash[Global_ID] = 1;
	Global_Sig_hash_Number++;
	
	u8 global_s_id = 2 + 4*(Group_ID-1);
	if (!uECC_sign(private_key[global_s_id], Global_hash_tx[Group_ID], 32, Global_Sig_hash[global_s_id], uECC_secp160r1())) {
		sprintf((char*)Send_Data_buff,"Sign Fail");
		Lora_Device_Sta = LORA_TX_STA;
		LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
	}
	ACK_Global_Sig_hash[global_s_id] = 1;
	Global_Sig_hash_Number++;
	
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
			else if((NotifyValue&EVENT_ABA_STATE)!=0)	//STATE发生	
			{
				
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT_ABA_STATE");LoRa_SendData(Send_Data_buff);	
				if(start_ABA)
					ABA_STATE_handler();
			}
//			else if((NotifyValue&EVENT_ABA_NACK)!=0)	//NACK	
//			{
//				
//			}
			else if((NotifyValue&EVENT_PACKET)!=0)	//PACKET	
			{
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT_PACKET.");LoRa_SendData(Send_Data_buff);
				PACKET_handler();
			}
			else if((NotifyValue&EVENT_RBC_INIT)!=0)	//PACKET	
			{
				u32 random = RNG_Get_RandomRange(0, 5);
				random = random * 200;
				delay_ms(random);
				
				if(start_Global_Consensus == 0)
					RBC_INIT_handler();
				else 
					Global_RBC_INIT_handler();
			}
			else if((NotifyValue&EVENT_RBC_ER)!=0)	//PACKET	
			{
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT_PACKET.");LoRa_SendData(Send_Data_buff);
				u8 rbc_flag = 0;
				if(RBC_done_Number < 2 * Nodes_f + 1 || RBC_init_Number < Nodes_N - 1 || need_er){
					need_er = 0;
					RBC_ER_handler();
					rbc_flag = 1;
				}
				
				if(!rbc_flag && start_ABA && ABA_done_Number < Nodes_N){
					u8 min_round = 0xff;
					for(int i=1;i<=Nodes_N;i++){
						if(min_round > ABA_R[i])min_round = ABA_R[i];
					}
					STATE_Set_Number = 1;
					STATE_Set[0] = min_round;
					
					ABA_STATE_handler();
				}
				
				if(ABA_done_Number == Nodes_N && dec_tx_number < dec_done_number){
					ENC_handler();
					//replay_enc = 0;
				}
				
				//single-hop
				if(ID == 1 && ACK_Global_Sig_hash[Global_ID] && dec_tx_number >= 2*Nodes_f + 1 && dec_tx_number == dec_done_number && !start_Global_Consensus){
					SIG_handler();
				}
			}
			else if((NotifyValue&EVENT_ENC)!=0)	//PACKET	
			{
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT_PACKET.");LoRa_SendData(Send_Data_buff);
				if(ABA_done_Number == Nodes_N)
					ENC_handler();
			}
			else if((NotifyValue&EVENT_SIG)!=0)	//PACKET	
			{
				if(!start_Global_Consensus)
					SIG_handler();
			}

		}
		else
		{
			sprintf((char*)Send_Data_buff,"ERROR: Get mission.");
			LoRa_SendData(Send_Data_buff);	
		}
	}
}




/*****Multi-Hop*****************************************************/
void Global_Consensus_Init(){
	start_Global_Consensus = 1;
	
	//change channel to global channel
	LoRa_CFG.chn = Global_channel;
	LoRa_CFG.tmode = LORA_TMODE_PT;
	LoRa_CFG.addr = 0;
	LoRa_Set();
	
	MYDMA_Config_Rece(DMA1_Stream1,DMA_CHANNEL_4);//初始化DMA
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);
	//init thers key
	Global_init_thres_enc();
	//tx init
	Global_TX_init();
	//rbc init
	Global_RBC_init();
	//aba init
	Global_ABA_init();
}

void Global_TX_init(){
	dec_tx_number = 0;
	dec_done_number = 0;
	for(int i=0;i<MAX_Nodes;i++){
		Tx_shares_Number[i] = 0x00;
		dec_tx_flag[i] = 0x00;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_Share[i][j] = 0x00;
			for(int m=0;m<MAX_Nodes;m++){
				Tx_Share[i][j][m] = 0x00;
			}
		}
	}
	for(int i=0;i<16;i++)tx_passwd[i] = RNG_Get_RandomNum();
	
	Global_Tx_PK_encrypt(tx_passwd,enc_tx_passwd[Group_ID]);
	Global_Tx_SK_decrypt_share(Group_ID, Group_ID);
//	Global_Tx_SK_decrypt_share(ID,1);
//	Global_Tx_SK_decrypt_share(ID,2);
//	Global_Tx_SK_decrypt_share(ID,3);
//	Global_Tx_SK_decrypt_share(ID,4);
//	Global_Tx_PK_verify_share(ID,1);
//	Global_Tx_PK_verify_share(ID,2);
//	Global_Tx_PK_verify_share(ID,3);
//	Global_Tx_PK_verify_share(ID,4);
//	Global_Tx_PK_combine_share(ID, enc_tx_passwd[ID], dec_tx_passwd[ID]);
	
	//packet
	for(int i=0;i<PD_Size;i++)
		Global_tmp_tx_buff[i] = enc_tx_passwd[Group_ID][i];
	for(int i=0;i<Share_Size;i++)
		Global_tmp_tx_buff[PD_Size+i] = Tx_U_str[Group_ID][i];
	
	u8 enc_proposal[Global_MAX_PROPOSAL];
	for(int i=0;i<Global_MAX_PROPOSAL;i++)enc_proposal[i] = 0x00;
	int pad_len = Tx_encrypt(tx_passwd, Global_Transaction[Group_ID], Global_Proposal_Size[Group_ID], enc_proposal);
	if(pad_len > Global_MAX_PROPOSAL)while(1);
	
	for(int i=0;i<pad_len;i++)
		Global_tmp_tx_buff[PD_Size+Share_Size+i] = enc_proposal[i];
	
	Global_Proposal_Packet_Size = PD_Size+Share_Size+pad_len;
}

void Global_RBC_init(){
	RBC_init_Number=0;
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
	RBC_Init[Group_ID] = 1;
	RBC_Echo[Group_ID] = 1;
	//tmp_tx_buff[passwd + U + tx]
	
	
	for(int i=0;i<Global_Group_Number;i++){
		Global_Group_Block_Num[i] = 0;
		
	}
	//Block 0
	Global_Group_Block_Num[Group_ID]++;
	u8 sig_idx = 0;
	for(int k=1;k<=Nodes_N;k++){
		if(sig_idx == 2)break;
		if(ACK_Global_Sig_hash[(Group_ID-1)*4 + k]){
			Global_Block[Group_ID][0][sig_idx] = (Group_ID-1)*4 + k;
			sig_idx++;
		}
	}	
	for(int i=0;i<Hash_Size;i++){
		Global_Block[Group_ID][0][2+i] = Global_hash_tx[Group_ID][i];
	}
	u32 b_idx = 2+Hash_Size;
	
	u8 id1 = Global_Block[Group_ID][0][0];
	u8 id2 = Global_Block[Group_ID][0][1];
	
	for(int i=0;i<Sig_Size;i++){
		Global_Block[Group_ID][0][b_idx] = Global_Sig_hash[id1][i];
		b_idx++;
	}
	for(int i=0;i<Sig_Size;i++){
		Global_Block[Group_ID][0][b_idx] = Global_Sig_hash[id2][i];
		b_idx++;
	}
	Global_Block_each_Size[Group_ID][0] = b_idx;
	
	//Block1--Blockx
	u32 packet_idx = 0;
	
	
	//Global_Proposal_Packet_Size
	//Block_Size
	u8 remain_flag = 0;
	u8 block_tot = Global_Proposal_Packet_Size/Block_Size;
	if(Global_Proposal_Packet_Size%Block_Size != 0){ block_tot++; remain_flag=1;}
	
	Global_Group_Block_Num[Group_ID] = Global_Group_Block_Num[Group_ID] + block_tot;
	
	for(int b=1;b<=block_tot;b++){
		if(b == block_tot && remain_flag)Global_Block_each_Size[Group_ID][b] = Global_Proposal_Packet_Size%Block_Size;
		else Global_Block_each_Size[Group_ID][b] = Block_Size;
		for(int i=0;i<Global_Block_each_Size[Group_ID][b];i++){
			Global_Block[Group_ID][b][i] = Global_tmp_tx_buff[packet_idx + i];
		}
		packet_idx += Global_Block_each_Size[Group_ID][b];
	}

	hash256 sh256;
	HASH256_init(&sh256);
	for(int b=0;b<Global_Group_Block_Num[Group_ID];b++)
	for (int i=0;i<Global_Block_each_Size[Group_ID][b];i++) HASH256_process(&sh256,Global_Block[Group_ID][b][i]);
    HASH256_hash(&sh256,hash_v[Group_ID]);
	
	for(int i=0;i<Global_Group_Number;i++)Global_RBC_Block_Number[i] = 0;
	
	for(int i=0;i<Global_Group_Number;i++){
		for(int b=0;b<MAX_Blocks;b++){
			Global_RBC_Block[i][b] = 0;
		}
	}

	RBC_done_Number = 0;

}

void Global_ABA_init(){
	start_ABA = 0;
	ABA_done_Number = 0;
	for(int i=0;i<MAX_round;i++){
		ABA_share_Number[i] = 0x00;
		Shared_Coin[i] = 0xFF;
		for(int j=0;j<Share_Size;j++){
			ABA_sig[i][j] = 0x00;
		}
		for(int j=0;j<MAX_Nodes;j++){
			for(int k=0;k<Share_Size;k++)ABA_share[i][j][k] = 0x00;
			ACK_ABA_share[i][j] = 0x00;
		}
	}
	for(int a=0;a<MAX_ABA;a++)
	for(int i=0;i<MAX_round;i++){
		est[a][i] = 0x00;
		bin_values[a][i][0] = 0x00;bin_values[a][i][1] = 0x00;
		ACK_BVAL_Number[a][i][0] = 0x00;ACK_BVAL_Number[a][i][1] = 0x00;
		BVAL[a][i][0] = BVAL[a][i][1] = 0;
		AUX[a][i][0] = AUX[a][i][1] = 0;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_BVAL[a][i][j][0] = 0x00;ACK_BVAL[a][i][j][1] = 0x00;
		}
	}
	for(int a=0;a<MAX_ABA;a++)
	for(int i=0;i<MAX_round;i++){
		vals[a][i][0] = 0x00;vals[a][i][1] = 0x00;
		ACK_AUX_Number[a][i][0] = 0x00;ACK_AUX_Number[a][i][1] = 0x00;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_AUX[a][i][j][0] = 0x00;ACK_AUX[a][i][j][1] = 0x00;
		}
	}

	for(int i=0;i<MAX_ABA;i++)ABA_R[i] = 1;
	
	for(int i=0;i<MAX_ABA;i++)
		for(int j=0;j<MAX_round;j++)
			est[i][j] = 0xFF;
	
//	for(int i=0;i<MAX_ABA;i++){
//		est[i][1] = 1;
//		BVAL[i][1][1] = 1;
//		ACK_BVAL_Number[i][1][1] = 1;
//	}
		
	for(int i=0;i<MAX_Nodes;i++)STATE_Set[i]=0;
	STATE_Set_Number = 1;
	
	for(int i=0;i<MAX_ABA;i++)
		ABA_decided[i] = 0xFF;

}

void Global_RBC_INIT_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	if(Nodes_N <= 8){
	for(int block_id = 0;block_id<Global_Group_Block_Num[Group_ID];block_id++){
		u8 padding = 0;
		if(Global_Block_each_Size[Group_ID][block_id] < 100) padding = 100 - Global_Block_each_Size[Group_ID][block_id];
		Send_Data_buff[0] = Group_ID;
		Send_Data_buff[1] = Global_TypeRBC_INIT;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		
		u8 packet_idx = 3 + padding;
		//block-tot
		Send_Data_buff[packet_idx] = Global_Group_Block_Num[Group_ID];
		packet_idx++;
		//block-id
		Send_Data_buff[packet_idx] = block_id;
		packet_idx++;
		//ack-init
		for(int k=1;k<=Nodes_N;k++){
			if(RBC_Init[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		packet_idx++;
		//block-size
		u8 this_block_size = Global_Block_each_Size[Group_ID][block_id];
		Send_Data_buff[packet_idx] = this_block_size;
		packet_idx++;
		for(int i=0;i<this_block_size;i++){
			Send_Data_buff[packet_idx + i] = Global_Block[Group_ID][block_id][i];
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
/**********************************************************/

void Tx_init(){
	/*********multi-hop**************/
	Global_Sig_hash_Number = 0;
	for(int i=0;i<Global_Nodes;i++){
		ACK_Global_Sig_hash[i] = 0;
	}
	if(ID != 1) Global_Sig_hash_Number = 100;
	/*******************************/
	
	dec_tx_number = 0;
	for(int i=0;i<MAX_Nodes;i++){
		Tx_shares_Number[i] = 0x00;
		dec_tx_flag[i] = 0x00;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_Share[i][j] = 0x00;
			for(int m=0;m<MAX_Nodes;m++){
				Tx_Share[i][j][m] = 0x00;
			}
		}
	}
	for(int i=0;i<Proposal_Size;i++){
		//Transaction[ID][i] = '1' + ID;
		Transaction[ID][i] = RNG_Get_RandomNum();
	}
	for(int i=0;i<16;i++)tx_passwd[i] = RNG_Get_RandomNum();
	
	Tx_PK_encrypt(tx_passwd,enc_tx_passwd[ID]);
	Tx_SK_decrypt_share(ID, ID);
	
//	Tx_SK_decrypt_share(ID,1);
//	Tx_SK_decrypt_share(ID,2);
//	Tx_SK_decrypt_share(ID,3);
//	Tx_SK_decrypt_share(ID,4);
//	Tx_PK_verify_share(ID,1);
//	Tx_PK_verify_share(ID,2);
//	Tx_PK_verify_share(ID,3);
//	Tx_PK_verify_share(ID,4);
//	Tx_PK_combine_share(ID, enc_tx_passwd[ID], dec_tx_passwd[ID]);
//	
//	for(int i=0;i<16;i++){
//		if(tx_passwd[i]!=dec_tx_passwd[ID][i])while(1);
//	}

	//packet
	//passwd + U_str + tx
	for(int i=0;i<PD_Size;i++)
		tmp_tx_buff[i] = enc_tx_passwd[ID][i];
	for(int i=0;i<Share_Size;i++)
		tmp_tx_buff[PD_Size+i] = Tx_U_str[ID][i];
	
	u8 enc_proposal[MAX_PROPOSAL];
	for(int i=0;i<MAX_PROPOSAL;i++)enc_proposal[i] = 0x00;
	int pad_len = Tx_encrypt(tx_passwd, Transaction[ID], Proposal_Size, enc_proposal);
	
//	///test///
//	for(int i=0;i<MAX_PROPOSAL;i++)tmp_dec_tx_buff[i] = 0x00;
//	Tx_decrypt(tx_passwd, enc_proposal, pad_len, tmp_dec_tx_buff);
//	
//	while(1);
//	///test///
	
	if(pad_len > MAX_PROPOSAL)while(1);
	
	for(int i=0;i<pad_len;i++)
		tmp_tx_buff[PD_Size+Share_Size+i] = enc_proposal[i];
	
	Proposal_Packet_Size = PD_Size+Share_Size+pad_len;
	
	
//	while(1);
}

void RBC_init(){
	RBC_init_Number=0;
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
		if(b == Block_Num - 1&&(Proposal_Packet_Size % Block_Size!=0))Block_each_Size[ID][b] = Proposal_Packet_Size % Block_Size;
		else Block_each_Size[ID][b] = Block_Size;
		for(int i=0;i<Block_each_Size[ID][b];i++){
			Block[ID][b][i] = tmp_tx_buff[packet_idx + i];
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
	
}
	
void ABA_init(){
	start_ABA = 0;
	ABA_done_Number = 0;
	for(int i=0;i<MAX_round;i++){
		ABA_share_Number[i] = 0x00;
		Shared_Coin[i] = 0xFF;
		for(int j=0;j<Share_Size;j++){
			ABA_sig[i][j] = 0x00;
		}
		for(int j=0;j<MAX_Nodes;j++){
			for(int k=0;k<Share_Size;k++)ABA_share[i][j][k] = 0x00;
			ACK_ABA_share[i][j] = 0x00;
		}
	}
	for(int a=0;a<MAX_ABA;a++)
	for(int i=0;i<MAX_round;i++){
		est[a][i] = 0x00;
		bin_values[a][i][0] = 0x00;bin_values[a][i][1] = 0x00;
		BVAL[a][i][0] = BVAL[a][i][1] = 0;
		AUX[a][i][0] = AUX[a][i][1] = 0;
		ACK_BVAL_Number[a][i][0] = 0x00;ACK_BVAL_Number[a][i][1] = 0x00;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_BVAL[a][i][j][0] = 0x00;ACK_BVAL[a][i][j][1] = 0x00;
		}
	}
	for(int a=0;a<MAX_ABA;a++)
	for(int i=0;i<MAX_round;i++){
		vals[a][i][0] = 0x00;vals[a][i][1] = 0x00;
		ACK_AUX_Number[a][i][0] = 0x00;ACK_AUX_Number[a][i][1] = 0x00;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_AUX[a][i][j][0] = 0x00;ACK_AUX[a][i][j][1] = 0x00;
		}
	}
	
	MYDMA_Config_Rece(DMA1_Stream1,DMA_CHANNEL_4);//初始化DMA
	//MYDMA_Config_Send(DMA1_Stream3,DMA_CHANNEL_4);
	
//	xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
//				(uint32_t		)EVENTBIT_1,			//要更新的bit
//				(eNotifyAction	)eSetBits);				//更新指定的bit
	
	
//	while(1){
//		
//		HAL_UART_DMAStop(&uart3_handler);
//		HAL_UART_Receive_DMA(&uart3_handler,OkReceBuff[OkIdx],DMA_buff_Size);
		HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);
//		delay_ms(2000);
//	}
	
	
	init_thres_sig();
//	TIM5_Exceed_Times = 0;
//	TIM2_Exceed_Times = 0;
//	btim_tim5_int_init(5000-1,10800-1);//108Mhz/10800 = 10000hz  900ms
//	btim_tim2_int_init(9000-1,10800-1);

//	btim_tim5_enable(0);
	
	for(int i=0;i<MAX_ABA;i++)ABA_R[i] = 1;
	
	for(int i=0;i<MAX_ABA;i++)
		for(int j=0;j<MAX_round;j++)
			est[i][j] = 0xFF;
	
//	for(int i=0;i<MAX_ABA;i++){
//		est[i][1] = 1;
//		BVAL[i][1][1] = 1;
//		ACK_BVAL_Number[i][1][1] = 1;
//	}
		
	for(int i=0;i<MAX_Nodes;i++)STATE_Set[i]=0;
	STATE_Set_Number = 1;
	
	for(int i=0;i<MAX_ABA;i++)
		ABA_decided[i] = 0xFF;
}
u8 ttt = 0;
u8 xxx = 0;
u8 mmm = 0;
u8 nnn = 0;
int subset_or_not(u8 aba, u8 round){
	if(bin_values[aba][round][0] == 0 && bin_values[aba][round][1] == 0)return 0;
	if(bin_values[aba][round][0] == 1 && bin_values[aba][round][1] == 0){
		if(vals[aba][round][0] == 1 && vals[aba][round][1] == 0)return 1;
		return 0;
	}
	if(bin_values[aba][round][0] == 0 && bin_values[aba][round][1] == 1){
		if(vals[aba][round][0] == 0 && vals[aba][round][1] == 1)return 1;
		return 0;
	}
	if(bin_values[aba][round][0] == 1 && bin_values[aba][round][1] == 1){
		if(vals[aba][round][0] == 0 && vals[aba][round][1] == 0)return 0;
		return 1;
	}
}


void ABA_STATE_handler(){
	
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	if(Nodes_N<=8){// && ABA_decided[1]==0xFF && ABA_decided[2]==0xFF && ABA_decided[3]==0xFF && ABA_decided[4]==0xFF){
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeSTATE;
		u8 padding = 20;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		u8 packet_idx = 3 + padding;
		Send_Data_buff[packet_idx] = STATE_Set_Number;
		for(int o=0;o<STATE_Set_Number;o++){
			packet_idx++;
			Send_Data_buff[packet_idx] = STATE_Set[o];//round number
			u8 round = STATE_Set[o];
			//BVAL
			packet_idx++;
			for(int j=1;j<=Nodes_N;j++){//nodes
				if(IDX == j){
					for(int k=1;k<=Nodes_N;k++){//ABA id
						if(BVAL[k][round][0])Send_Data_buff[packet_idx+(j-1)*2] |= (1<<(k-1));
						if(BVAL[k][round][1])Send_Data_buff[packet_idx+1+(j-1)*2] |= (1<<(k-1));
					}
					continue;
				}
				for(int k=1;k<=Nodes_N;k++){//ABA id
					if(ACK_BVAL[k][round][j][0])Send_Data_buff[packet_idx+(j-1)*2] |= (1<<(k-1));
					if(ACK_BVAL[k][round][j][1])Send_Data_buff[packet_idx+1+(j-1)*2] |= (1<<(k-1));
				}
			}
			packet_idx += 2 * Nodes_N;
			
			//AUX
			for(int j=1;j<=Nodes_N;j++){//nodes
				if(IDX == j){
					for(int k=1;k<=Nodes_N;k++){//ABA id
						if(AUX[k][round][0])Send_Data_buff[packet_idx+(j-1)*2] |= (1<<(k-1));
						if(AUX[k][round][1])Send_Data_buff[packet_idx+1+(j-1)*2] |= (1<<(k-1));
					}
					continue;
				}
				for(int k=1;k<=Nodes_N;k++){//ABA id
					if(ACK_AUX[k][round][j][0])Send_Data_buff[packet_idx+(j-1)*2] |= (1<<(k-1));
					if(ACK_AUX[k][round][j][1])Send_Data_buff[packet_idx+1+(j-1)*2] |= (1<<(k-1));
				}
			}
			packet_idx += 2 * Nodes_N;
			
			//share_nack
			for(int k=1;k<=Nodes_N;k++)
				if(ACK_ABA_share[round][k])Send_Data_buff[packet_idx] |= (1<<(k-1));
			packet_idx++;
			
			//share
			//share 自己的share 有没有计算?
			if(ACK_ABA_share[round][IDX] == 0){
				Coin_SK_sign(round);
			}
			for(int i=0;i<Share_Size;i++){
				Send_Data_buff[packet_idx + i] = ABA_share[round][IDX][i];
			}
			packet_idx += Share_Size;
			//verify share z
			for(int i=0;i<BIG_Size;i++){
				Send_Data_buff[packet_idx + i] = ABA_Z_share[round][IDX][i];
			}
			packet_idx += BIG_Size;
			//verify share c
			for(int i=0;i<BIG_Size;i++){
				Send_Data_buff[packet_idx + i] = ABA_C_share[round][IDX][i];
			}
			packet_idx += BIG_Size;
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
		
	}else if(Nodes_N>8 && Nodes_N<=16){
		while(1);
	}
	
	
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
		if(Block_each_Size[ID][block_id] < 100) padding = 100 - Block_each_Size[ID][block_id];
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
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	if(Nodes_N <= 8){
		u8 padding = 0;
		Send_Data_buff[0] = IDX;
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


void ENC_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	if(Nodes_N <= 8){
		u8 padding = 20;
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeTX_ENC;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		u8 packet_idx = 2 + padding + 1;
		//share nack
		for(int k=1;k<=Nodes_N;k++){
			if(ABA_decided[k] == 1 && Tx_shares_Number[k] >= Enc_Thres_k)
				Send_Data_buff[packet_idx] |= (1<<(k-1));
		}
		packet_idx++;
		//share ack
		for(int k=1;k<=Nodes_N;k++){
			if(ABA_decided[k] == 1)
				Send_Data_buff[packet_idx] |= (1<<(k-1));
		}
		packet_idx++;
		
		for(int k=1;k<=Nodes_N;k++){
			if(ABA_decided[k] == 1){
				for(int i=0;i<Share_Size;i++){
					Send_Data_buff[packet_idx + i] = Tx_shares_str[k][IDX-1][i]; 
				}
				packet_idx += Share_Size;
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

void SIG_handler(){
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	if(Nodes_N <= 8){
		u8 padding = 50;
		Send_Data_buff[0] = ID;
		Send_Data_buff[1] = TypeTX_SIG;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		u8 packet_idx = 2 + padding + 1;
		//Sig nack
		for(int k=1;k<=Nodes_N;k++){
			if(ACK_Global_Sig_hash[k + 4 * (Group_ID-1)])
				Send_Data_buff[packet_idx] |= (1<<(k-1));
		}
		packet_idx++;
		
		for(int i=0;i<Sig_Size;i++){
			Send_Data_buff[packet_idx] = Global_Sig_hash[Global_ID][i];
			packet_idx++;
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

void PACKET_handler(){
	//used for local consensus
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
	/******single-hop*******/
	u8 global_s_id = s_id + 4*(Group_ID - 1);
	/******multi-hop*******/
	if(start_Global_Consensus){
		global_s_id = (s_id-1)*4+1;
	}
	/*********************/
	
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
	if (!uECC_verify(public_key[global_s_id], hash, 32, sig, uECC_secp160r1())) {
		verify_flag = 0;
	}
	
	if(packet_type == TypeSTATE && verify_flag && RBC_init_Number >= Nodes_N-1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		u8 round_number = buff[packet_idx];
		packet_idx++;
//		if(start_Global_Consensus)
		//NACK
		u8 round_flag = 0;
		for(int o=0;o<round_number;o++){
			round_flag = 0;
			u8 round = buff[packet_idx];packet_idx++;
			for(int k=1;k<=Nodes_N;k++){
				
				//BVAL[0]
				if(((buff[packet_idx + (IDX - 1) * 2] & (1<<(k-1))) == 0) && BVAL[k][round][0]==1){
					round_flag = 1;
					break;
				}
				//BVAL[1]
				if(((buff[packet_idx + (IDX - 1) * 2 + 1] & (1<<(k-1))) == 0) && BVAL[k][round][1]==1){
					round_flag = 1;
					break;
				}
				
				//AUX[0]
				if(((buff[packet_idx + (IDX - 1) * 2 + 2 * Nodes_N] & (1<<(k-1))) == 0) && AUX[k][round][0]==1){
					round_flag = 1;
					break;
				}
				//AUX[1]
				if(((buff[packet_idx + (IDX - 1) * 2 + 1 + 2 * Nodes_N] & (1<<(k-1))) == 0) && AUX[k][round][1]==1){
					round_flag = 1;
					break;
				}
			}
			//share_nack
			packet_idx += 4 * Nodes_N;
			if((buff[packet_idx] & (1<<(IDX-1))) == 0 && ACK_ABA_share[round][IDX] == 1)round_flag = 1;	
			
			packet_idx++;
			if(round_flag){
				STATE_Set[0] = round;
				STATE_Set_Number=1;
				break;
			}
		}
		
		if(STATE_Set_Number)
			STATE_Set_Number = 1;
		
		if(STATE_Set_Number!=0)
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_ABA_STATE,			//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		
		//STATE
		packet_idx = 4+padding;
		for(int o=0;o<round_number;o++){
			round_flag = 0;
			u8 round = buff[packet_idx];packet_idx++;
//		    for(int i=0;i<round_number;i++){ 
			//u8 round = buff[packet_idx+1];
			u32 tmpp_packet_idx = packet_idx;
			packet_idx += 4 * Nodes_N;
			packet_idx ++;
			
			if(!ACK_ABA_share[round][s_id]){
				//验证share 复杂 耗时大
				u8 share[70];
				int tmp_packet_idx = packet_idx;
				for(int i=0;i<Share_Size;i++)share[i] = 0x00;
				//share
				for(int i=0;i<Share_Size;i++)share[i] = buff[tmp_packet_idx+i];
				tmp_packet_idx+=Share_Size;
				//Z
				for(int i=0;i<BIG_Size;i++)ABA_Z_share[round][s_id][i] = buff[tmp_packet_idx+i];
				BIG_160_28_fromBytes(ABA_Z[round][s_id],ABA_Z_share[round][s_id]);
				tmp_packet_idx+=BIG_Size;
				//C
				for(int i=0;i<BIG_Size;i++)ABA_C_share[round][s_id][i] = buff[tmp_packet_idx+i];
				BIG_160_28_fromBytes(ABA_C[round][s_id],ABA_C_share[round][s_id]);
				if(Coin_PK_verify_sig_share(s_id,round,share)){
					for(int i=0;i<Share_Size;i++)ABA_share[round][s_id][i] = buff[packet_idx+i];
					ABA_share_Number[round]++;
					ACK_ABA_share[round][s_id] = 1;
					if(ABA_share_Number[round] >= Nodes_N){
						//收到f+1个coin，进行合成
						if(Shared_Coin[round]==0xFF)
							Coin_PK_sig_combine_share(round);//内部会验证
					}
				}else{
					while(1){
						sprintf((char*)Send_Data_buff,"Node %d verify sig error",s_id);
						LoRa_SendData(Send_Data_buff);
					}
				}
			}
			packet_idx += Share_Size + BIG_Size*2;
			
			
			for(int k=1;k<=Nodes_N;k++){//ABA
				
				if(ABA_R[k]>round) continue;
				
				if((buff[tmpp_packet_idx + (s_id - 1) * 2] & (1<<(k-1))) && !ACK_BVAL[k][round][s_id][0]){ACK_BVAL[k][round][s_id][0] = 1;ACK_BVAL_Number[k][round][0]++;}
				if((buff[tmpp_packet_idx + (s_id - 1) * 2 + 1] & (1<<(k-1))) && !ACK_BVAL[k][round][s_id][1]){ACK_BVAL[k][round][s_id][1] = 1;ACK_BVAL_Number[k][round][1]++;}
				if((buff[tmpp_packet_idx + (s_id - 1) * 2 + 2 * Nodes_N] & (1<<(k-1))) && !ACK_AUX[k][round][s_id][0]){ACK_AUX[k][round][s_id][0] = 1;ACK_AUX_Number[k][round][0]++;}
				if((buff[tmpp_packet_idx + (s_id - 1) * 2 + 1 + 2 * Nodes_N] & (1<<(k-1))) && !ACK_AUX[k][round][s_id][1]){ACK_AUX[k][round][s_id][1] = 1;ACK_AUX_Number[k][round][1]++;}
				
				//收到f+1个BVAL(b)
				if(ACK_BVAL_Number[k][round][0] >= Nodes_f + 1 && !BVAL[k][round][0]){ACK_BVAL_Number[k][round][0]++;BVAL[k][round][0] = 1;}
				if(ACK_BVAL_Number[k][round][1] >= Nodes_f + 1 && !BVAL[k][round][1]){ACK_BVAL_Number[k][round][1]++;BVAL[k][round][1] = 1;}
				//收到2f+1个BVAL(b)
				if(ACK_BVAL_Number[k][round][0] >= 2 * Nodes_f + 1){bin_values[k][round][0] = 1;}
				if(ACK_BVAL_Number[k][round][1] >= 2 * Nodes_f + 1){bin_values[k][round][1] = 1;}
				
				//bin_values_R非空  自己发送AUX，记录number
				if(bin_values[k][round][0] && !AUX[k][round][0]){ACK_AUX_Number[k][round][0]++;AUX[k][round][0] = 1;}
				if(bin_values[k][round][1] && !AUX[k][round][1]){ACK_AUX_Number[k][round][1]++;AUX[k][round][1] = 1;}
				
				//收到N-f个AUX_r(b)   广播BVAL内
				if(ACK_AUX_Number[k][round][0] >= (Nodes_N - Nodes_f) && !BVAL[k][round][0]){ACK_BVAL_Number[k][round][0]++;BVAL[k][round][0] = 1;}
				if(ACK_AUX_Number[k][round][1] >= (Nodes_N - Nodes_f) && !BVAL[k][round][1]){ACK_BVAL_Number[k][round][1]++;BVAL[k][round][1] = 1;}
				//收到N-f个AUX_r(b)   收集到vals内	
				if(ACK_AUX_Number[k][round][0] >= (Nodes_N - Nodes_f))vals[k][round][0] = 1;
				if(ACK_AUX_Number[k][round][1] >= (Nodes_N - Nodes_f))vals[k][round][1] = 1;
				
				if(ACK_AUX_Number[k][round][0] > 0)vals[k][round][0] = 1;
				if(ACK_AUX_Number[k][round][1] > 0)vals[k][round][1] = 1;	
				
				u8 aux_flag = 0;
				if(ACK_AUX_Number[k][round][0] + ACK_AUX_Number[k][round][1] >= (Nodes_N - Nodes_f))aux_flag = 1;
				//vals是bin_values的子集
				if(aux_flag && subset_or_not(k,round) && ABA_R[k] == round){
					//shared coin
					//到这里一定会获得了N-f的share，因为是绑定到一起的
					//获取round的coin
					//暂定为1，需要合成
					u8 s = 0;
//					s = Shared_Coin[round];
					if(Shared_Coin[round] == 0xFF)
						Coin_PK_sig_combine_share(round);
					//if(round == 5) s = 1;
					s = Shared_Coin[round];
					if(ABA_share_Number[round] < Nodes_f + 1){

						while(1);
					}
						
					if(vals[k][round][0] == 1 && vals[k][round][1] == 0){
						est[k][round+1] = 0;
						BVAL[k][round+1][0] = 1;
						ACK_BVAL_Number[k][round+1][0]++;
						ABA_round_time[k][round] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
						ABA_R[k]++;
						if(s == 0){
							//ABA j done output 0
							if(ABA_decided[k] == 0xFF){
								ABA_decided[k] = 0;
								ABA_done_Number++;
								ABA_time[k] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
								ABA_round[k] = round;
								if(ABA_done_Number == Nodes_N){MY_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
						
								}

								
							}
							else if (ABA_decided[k] == 1){
								//error
								LED0(1);LED1(1);LED2(1);
								while(1){
									sprintf((char*)Send_Data_buff,"Node %d decided 11 %d",Group_ID, k);
									LoRa_SendData(Send_Data_buff);
								}
								while(1);
							}	
						}
					}
					if(vals[k][round][0] == 0 && vals[k][round][1] == 1){
						est[k][round+1] = 1;
						BVAL[k][round+1][1] = 1;
						ACK_BVAL_Number[k][round+1][1]++;
						ABA_R[k]++;
						ABA_round_time[k][round] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
						if(s == 1){
							//ABA j done output 1
							if(ABA_decided[k] == 0xFF){
								ABA_decided[k] = 1;
								ABA_time[k] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
								ABA_round[k] = round;
								ABA_done_Number++;
								if(ABA_done_Number == Nodes_N){MY_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
							
								}
								dec_done_number++;
								if(!start_Global_Consensus){
									//single-hop
									//decrypt the tx
									Tx_SK_decrypt_share(k, IDX);
									if(ACK_Share[k][IDX] == 0){
										ACK_Share[k][IDX] = 1;
										Tx_shares_Number[k]++;
									}
								}else{
									Global_Tx_SK_decrypt_share(k, IDX);
									if(ACK_Share[k][IDX] == 0){
										ACK_Share[k][IDX] = 1;
										Tx_shares_Number[k]++;
									}
								}
								
								
							}
							else if (ABA_decided[k] == 0){

								LED0(1);LED1(1);LED2(1);
								while(1){
									sprintf((char*)Send_Data_buff,"Node %d decided 00 %d ",Group_ID, k);
									LoRa_SendData(Send_Data_buff);
								}
								while(1);
							}
						}
					}	
					if(vals[k][round][0] == 1 && vals[k][round][1] == 1){
						est[k][round+1] = s;
						BVAL[k][round+1][s] = 1;
						ABA_R[k]++;
					}
				}
			}
//			packet_idx += 4 * Nodes_N;
//			packet_idx ++;
//			
//			if(!ACK_ABA_share[round][s_id]){
//				//验证share 复杂 耗时大
//				u8 share[70];
//				int tmp_packet_idx = packet_idx;
//				for(int i=0;i<Share_Size;i++)share[i] = 0x00;
//				//share
//				for(int i=0;i<Share_Size;i++)share[i] = buff[tmp_packet_idx+i];
//				tmp_packet_idx+=Share_Size;
//				//Z
//				for(int i=0;i<BIG_Size;i++)ABA_Z_share[round][s_id][i] = buff[tmp_packet_idx+i];
//				BIG_160_28_fromBytes(ABA_Z[round][s_id],ABA_Z_share[round][s_id]);
//				tmp_packet_idx+=BIG_Size;
//				//C
//				for(int i=0;i<BIG_Size;i++)ABA_C_share[round][s_id][i] = buff[tmp_packet_idx+i];
//				BIG_160_28_fromBytes(ABA_C[round][s_id],ABA_C_share[round][s_id]);
//				if(Coin_PK_verify_sig_share(s_id,round,share)){
//					for(int i=0;i<Share_Size;i++)ABA_share[round][s_id][i] = buff[packet_idx+i];
//					ABA_share_Number[round]++;
//					ACK_ABA_share[round][s_id] = 1;
//					if(ABA_share_Number[round] >= Nodes_N){
//						//收到f+1个coin，进行合成
//						if(Shared_Coin[round]==0xFF)
//							Coin_PK_sig_combine_share(round);//内部会验证
//					}
//				}else{
//					while(1){
//						sprintf((char*)Send_Data_buff,"Node %d verify sig error",s_id);
//						LoRa_SendData(Send_Data_buff);
//					}
//				}
//			}
//			packet_idx += Share_Size + BIG_Size*2;
//			if(!ACK_ABA_share[round][s_id]){
//				//验证share 复杂 耗时大
//				u8 share[70];
//				for(int i=0;i<Share_Size;i++)share[i] = 0x00;
//				for(int i=0;i<Share_Size;i++)share[i] = buff[packet_idx+i];
//				if(Coin_PK_verify_sig_share(s_id,round,share)){
//					for(int i=0;i<Share_Size;i++)ABA_share[round][s_id][i] = buff[packet_idx+i];
//					ABA_share_Number[round]++;
//					ACK_ABA_share[round][s_id] = 1;
//					if(ABA_share_Number[round] >= Nodes_N){
//						//收到f+1个coin，进行合成
//						if(Shared_Coin[round]==0xFF)
//							Coin_PK_sig_combine_share(round);//内部会验证
//					}
//				}else{
//					while(1){
//						sprintf((char*)Send_Data_buff,"Node %d verify sig error",s_id);
//						LoRa_SendData(Send_Data_buff);
//					}
//				}
//			}
//			packet_idx += Share_Size;
//		}                              
		}
	}
	else if(packet_type == TypeRBC_INIT && verify_flag){
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
				ECP_BN158_fromOctet(&Tx_U[s_id],&tmp_U);
				
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
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		
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
		if((init_ACK & (1<<(IDX-1))) == 0){
			reply_init = 1;
		}
		//ACK
		for(int k = 1 ; k <= Nodes_N ;k++){
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
		for(int k=1;k<=Nodes_N;k++){
			if(!check_list[k])continue;
			
			if((echo_VAL & (1<<(k-1))) != 0 && ACK_RBC_Echo[k][s_id] == 0){
				ACK_RBC_Echo[k][s_id] = 1;
				RBC_Echo_Number[k]++;
				if(RBC_Echo_Number[k] >= Nodes_N - Nodes_f){
					RBC_Ready[k] = 1;
					if(ACK_RBC_Ready[k][IDX] == 0){
						ACK_RBC_Ready[k][IDX] = 1;
						RBC_Ready_Number[k]++;
					}
					
				}
			}
			if((ready_VAL & (1<<(k-1))) != 0 && ACK_RBC_Ready[k][s_id] == 0){
				RBC_Ready_Number[k]++;
				ACK_RBC_Ready[k][s_id] =1;
				if(RBC_Ready_Number[k] >= Nodes_N - Nodes_f){
					//accept it
					if(RBC_done[k] == 0){
						RBC_done[k] = 1;
						RBC_done_Number++;
						
						//to ABA
						if(RBC_done_Number <= Nodes_N - Nodes_f){
							// 1 -> ABA[k]
							est[k][1] = 1;
							BVAL[k][1][1] = 1;
							ACK_BVAL_Number[k][1][1] = 1;
						}
						if(RBC_done_Number == Nodes_N - Nodes_f){
							//0 -> ABA[others]
							for(int m=1;m<=Nodes_N;m++){
								if(RBC_done[m] == 0){
									est[m][1] = 0;
									BVAL[m][1][0] = 1;
									ACK_BVAL_Number[m][1][0] = 1;
								}
							}
							if(start_ABA){
								while(1){
									sprintf((char*)Send_Data_buff,"Node %d start ABA error",IDX);
									LoRa_SendData(Send_Data_buff);
								}
							}
							start_ABA = 1;
						}
						
					}
					
					
				}
			}
		
		}
		if(reply_er){
			need_er = 1;
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_RBC_ER,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		if(reply_init){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_RBC_INIT,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		
	}
	else if(packet_type == TypeTX_ENC && verify_flag){
		u8 replay_enc = 0;
		u8 enc_ack = 0;
		u8 enc_nack = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		enc_nack = buff[packet_idx];packet_idx++;
		enc_ack = buff[packet_idx];		
		packet_idx++;
		u8 aba_done = 0;
		if(ABA_done_Number == Nodes_N) aba_done = 1;
		if(aba_done)
		for(int k=1;k<=Nodes_N;k++){
			if((enc_nack & (1<<(k-1))) == 0 && ABA_decided[k] == 1){
				replay_enc = 1;
				break;
			}
		}
		if(aba_done)
		for(int k=1;k<=Nodes_N;k++){
			if((enc_ack & (1<<(k-1))) != 0 && ACK_Share[k][s_id] == 0){
				for(int i=0;i<Share_Size;i++){
					Tx_shares_str[k][s_id - 1][i] = buff[packet_idx+i];
				}
				
				octet tmp_o;
				tmp_o.len = tmp_o.max = Share_Size;
				tmp_o.val = (char*)Tx_shares_str[k][s_id-1];
				ECP_BN158_fromOctet(&Tx_shares[k][s_id-1],&tmp_o);
				
				//single-hop
				//test!!!!!!!
				if(!start_Global_Consensus && Tx_PK_verify_share(k, s_id)){
					ACK_Share[k][s_id] = 1;
					Tx_shares_Number[k]++ ;
					if(Tx_shares_Number[k] >= Enc_Thres_k){
						//combine
						Tx_PK_combine_share(k,enc_tx_passwd[k],dec_tx_passwd[k]);
						u8 tmp_enc_tx[MAX_PROPOSAL];
						for(int i=0;i<MAX_PROPOSAL;i++)tmp_enc_tx[i] = 0x00;
						u32 proposal_packet_id = 0;
						for(int i=Share_Size+PD_Size;i<Block_each_Size[k][0];i++){
							tmp_enc_tx[proposal_packet_id] = Block[k][0][i];
							proposal_packet_id++;
						}
						
						for(int b=1;b<Block_Num;b++){
							for(int i=0;i<Block_each_Size[k][b];i++){
								tmp_enc_tx[proposal_packet_id] = Block[k][b][i];
								proposal_packet_id++;
							}
						}
						
						Tx_decrypt(dec_tx_passwd[k], tmp_enc_tx, proposal_packet_id, Transaction[k]);
						if(dec_tx_flag[k] == 0){
							dec_tx_flag[k] = 1;
							dec_tx_number++;
							
							if(dec_tx_number == dec_done_number && !start_Global_Consensus){
								//tx
								u32 tx_idx = 0;
								for(int k=1;k<=Nodes_N;k++){
									if(ABA_decided[k] == 1){
										for(int i=0;i<Proposal_Size;i++){
											Global_Transaction[Group_ID][tx_idx] = Transaction[k][i];
											tx_idx++;
										}
									}
								}
								Global_Proposal_Size[Group_ID] = tx_idx;
								//hash
								hash256 sh256;
								HASH256_init(&sh256);
								for (int i=0;i<tx_idx;i++) HASH256_process(&sh256,Global_Transaction[Group_ID][i]);
								HASH256_hash(&sh256,Global_hash_tx[Group_ID]);
								//Sign
								sign_data(Global_hash_tx[Group_ID],Global_Sig_hash[Global_ID]);
								ACK_Global_Sig_hash[Global_ID] = 1;
								Global_Sig_hash_Number++;
							}
						}
					}					
				}
				//multi-hop
				else if(start_Global_Consensus && Global_Tx_PK_verify_share(k, s_id)){
					ACK_Share[k][s_id] = 1;
					Tx_shares_Number[k]++ ;
					if(Tx_shares_Number[k] >= Enc_Thres_k){
						//combine
						Global_Tx_PK_combine_share(k,enc_tx_passwd[k],dec_tx_passwd[k]);
						u8 tmp_enc_tx[Global_MAX_PROPOSAL];
						for(int i=0;i<Global_MAX_PROPOSAL;i++)tmp_enc_tx[i] = 0x00;
						u32 proposal_packet_id = 0;
						for(int i=Share_Size+PD_Size;i < Global_Block_each_Size[k][1];i++){
							tmp_enc_tx[proposal_packet_id] = Global_Block[k][1][i];
							proposal_packet_id++;
						}
							
						for(int b=2;b<Global_Group_Block_Num[k];b++){
							for(int i=0;i<Global_Block_each_Size[k][b];i++){
								tmp_enc_tx[proposal_packet_id] = Global_Block[k][b][i];
								proposal_packet_id++;
							}
						}
						
						Tx_decrypt(dec_tx_passwd[k], tmp_enc_tx, proposal_packet_id, Global_Transaction[k]);
						Global_Proposal_Size[k] = proposal_packet_id - 16;
						if(dec_tx_flag[k] == 0){
							dec_tx_flag[k] = 1;
							dec_tx_number++;
							if(Group_ID == 1 && ID == 1 && dec_tx_number == dec_done_number && MY_Consensus_TIME == 0){
								LoRa_CFG.chn = 66;
								LoRa_Set();	
								u32 tmp_size = 0;
								for(int i=1;i<=Nodes_N;i++){
									if(dec_tx_flag[i])tmp_size += Global_Proposal_Size[i];
								}
								MY_Consensus_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
								sprintf((char*)Send_Data_buff,"%d-%d",MY_Consensus_TIME,tmp_size);
								LoRa_SendData(Send_Data_buff);
								while(1);
//								LoRa_CFG.chn = 0;
//								LoRa_Set();	
							}
						}
					}					
				}
				
			}
			if((enc_ack & (1<<(k-1))) != 0)packet_idx += Share_Size;
			//if(enc_ack & (1<<(k-1)) == 0 && ABA_decided[k] == 1)replay_enc = 1;
		}

		if(replay_enc){
			//need_er = 1;
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_ENC,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		
	}
	else if(packet_type == TypeTX_SIG && verify_flag){
		u8 replay_sig = 0;
		u8 sig_nack = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		sig_nack = buff[packet_idx];
		packet_idx++;
		
		if((sig_nack & (1<<(ID-1))) == 0){
			replay_sig = 1;
		}
		
		if(ACK_Global_Sig_hash[global_s_id] == 0){
			ACK_Global_Sig_hash[global_s_id] = 1;
			Global_Sig_hash_Number++;
			for(int i=0;i<Sig_Size;i++){
				Global_Sig_hash[global_s_id][i] = buff[packet_idx];
				packet_idx++;
			}
			//verify signature
			if(ID == 1 && !uECC_verify(public_key[global_s_id], Global_hash_tx[Group_ID], 32, Global_Sig_hash[global_s_id], uECC_secp160r1())) {
				while(1){	
					sprintf((char*)Send_Data_buff,"Signature verify fail TypeTX_SIG");
					LoRa_SendData(Send_Data_buff);
					delay_ms(1000);
				}
			}

			if(Global_Sig_hash_Number == Nodes_f + 1 && ID == 1){
			//	//start_Global_Consensus
				Global_Consensus_Init();
			}
		}

		if(replay_sig && ACK_Global_Sig_hash[Global_ID]){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_SIG,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		
	}
	else if(packet_type == Global_TypeRBC_INIT && verify_flag){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 reply_flag = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		u8 block_tot = buff[packet_idx];packet_idx++;
		u8 block_id = buff[packet_idx];packet_idx++;
		u8 init_ACK = buff[packet_idx];
		Global_Group_Block_Num[s_id] = block_tot;
		
		if((init_ACK & (1<<(IDX-1)))==0){
			reply_flag = 1;
		}
		packet_idx++;
		u8 this_block_size = buff[packet_idx];
		packet_idx++;
		if(Global_RBC_Block[s_id][block_id] == 0){
			Global_RBC_Block[s_id][block_id] = 1;
			for(int i=0;i<this_block_size;i++){
				Global_Block[s_id][block_id][i] = buff[packet_idx+i];
			}
			Global_Block_each_Size[s_id][block_id] = this_block_size;
			Global_RBC_Block_Number[s_id]++;

			if(Global_RBC_Block_Number[s_id] == Global_Group_Block_Num[s_id]){
				RBC_Init[s_id] = 1;
				RBC_init_Number++;
				//sha2(Block[s_id], Block_Size, hash_v[s_id], 0);
				
				hash256 sh256;
				HASH256_init(&sh256);
				for(int b=0;b<Global_Group_Block_Num[s_id];b++)
				for (int i=0;i<Global_Block_each_Size[s_id][b];i++) HASH256_process(&sh256,Global_Block[s_id][b][i]);
				HASH256_hash(&sh256,hash_v[s_id]);
				
				RBC_Echo[s_id] = 1;
				RBC_Echo_Number[s_id] += 2;
				ACK_RBC_Echo[s_id][s_id] = 1;
			}
			if(block_id == 0){
				//verify signature
				//id1 and id2
				u8 id1 = Global_Block[s_id][block_id][0];
				u8 id2 = Global_Block[s_id][block_id][1];
				//hash
				for(int i=0;i<Hash_Size;i++){
					Global_hash_tx[s_id][i] = Global_Block[s_id][block_id][2+i];
				}
				//signautre
				u32 tmp_packet_idx = 2+Hash_Size;
				for(int i=0;i<Sig_Size;i++){
					Global_Sig_hash[id1][i] = Global_Block[s_id][block_id][tmp_packet_idx];
					tmp_packet_idx++;
				}
				for(int i=0;i<Sig_Size;i++){
					Global_Sig_hash[id2][i] = Global_Block[s_id][block_id][tmp_packet_idx];
					tmp_packet_idx++;
				}	
				//verify signture
				if(!uECC_verify(public_key[id1], Global_hash_tx[s_id], 32, Global_Sig_hash[id1], uECC_secp160r1())) {
					while(1){	
						sprintf((char*)Send_Data_buff,"Signature verify fail Global_TypeRBC_INIT1");
						LoRa_SendData(Send_Data_buff);
						delay_ms(1000);
					}
				}
				if(!uECC_verify(public_key[id2], Global_hash_tx[s_id], 32, Global_Sig_hash[id2], uECC_secp160r1())) {
					while(1){	
						sprintf((char*)Send_Data_buff,"Signature verify fail Global_TypeRBC_INIT2");
						LoRa_SendData(Send_Data_buff);
						delay_ms(1000);
					}
				}
				//success
			}
			if(block_id == 1){
				for(int i=0;i<Share_Size;i++)
					Tx_U_str[s_id][i] = Global_Block[s_id][block_id][PD_Size+i];
				
				octet tmp_U;
				tmp_U.len = tmp_U.max = Share_Size;
				tmp_U.val = (char*)Tx_U_str[s_id];
				ECP_BN158_fromOctet(&Tx_U[s_id],&tmp_U);
				
				for(int i=0;i<PD_Size;i++)
					enc_tx_passwd[s_id][i] = Global_Block[s_id][block_id][i];
			}
			
		}
		if(reply_flag){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_RBC_INIT,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		
	}
	//	for(int i=0;i<300;i++)ReceBuff[i] = 0x00;
	HAL_UART_DMAStop(&uart3_handler);
	//	HAL_DMA_Init(&UART3RxDMA_Handler);
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);//开启DMA传输
	
	
}






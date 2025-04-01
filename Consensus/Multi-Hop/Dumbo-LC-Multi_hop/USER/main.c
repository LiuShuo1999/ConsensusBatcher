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
#include "Dumbo2.h"
#include "btim.h"
#include "usart3.h"
#include "core.h"
#include "bls_BN158.h"
#include "dma.h"
#define USE_STM32F7XX_NUCLEO_144

#include "mbedtls/aes.h"

u8 random_sleep_flag = 1;

u32 Total_low_time = 0;
u8 low_flag = 0;
u32 low_time[400] = {0};
u32 wfi_time[400] = {0};
u32 low_IDX = 0;

u32 MY_TIME = 0;
u8 cur_aba_id;
u8 state_aba;
u32 TIM5_Exceed_Times;
u32 TIM2_Exceed_Times;
u32 TIM9_Exceed_Times;
u8 Send_Data_buff[800] = {0};
u32 MY_Consensus_TIME=0;
///****************ABA_Shared***************************************/
//u8 ABA_share[MAX_ABA][MAX_round][MAX_Nodes][Share_Size] = {0};//第x轮的哪个节点的share
//u8 ACK_ABA_share[MAX_ABA][MAX_round][MAX_Nodes] = {0};//第x轮的哪个节点的share收到了
//u8 ABA_share_Number[MAX_ABA][MAX_round];//第x轮share数量
//u8 Shared_Coin[MAX_ABA][MAX_round];//第x轮的coin //FF->NULL
//u8 COIN[Share_Size];
//u8 est[MAX_ABA][MAX_round];//第x轮的估计值
//u8 bin_values[MAX_ABA][MAX_round][2];

////BVAL
//u8 BVAL[MAX_ABA][MAX_round][2];//发过的BVAL
//u8 ACK_BVAL[MAX_ABA][MAX_round][MAX_Nodes][2];//第x轮哪个节点的0/1收到了
//u8 ACK_BVAL_Number[MAX_ABA][MAX_round][2];//第x轮的0/1各收到了多少个

////AUX
//u8 AUX[MAX_ABA][MAX_round][2];//发过的AUX
//u8 vals[MAX_ABA][MAX_round][2];//收到的AUX
//u8 ACK_AUX[MAX_ABA][MAX_round][MAX_Nodes][2];//第x轮哪个节点的0/1收到了
//u8 ACK_AUX_Number[MAX_ABA][MAX_round][2];//第x轮的0/1各收到了多少个

/****************ABA_Local*********************************/
u8 Local_ABA_votes[MAX_ABA][MAX_round][Local_ABA_phase_Number][2];
u8 Local_ABA_votes_3_NULL[MAX_ABA][MAX_round];//第几个ABA第几轮第三阶段共有几个空值
u8 ACK_Local_ABA_votes[MAX_ABA][MAX_round][Local_ABA_phase_Number];
//u8 ACK_Local_ABA_votes[MAX_ABA][MAX_round][Local_ABA_phase_Number][MAX_Nodes];

//RBC initial
u8 Local_ABA_RBC_Init[MAX_ABA][MAX_round][Local_ABA_phase_Number][MAX_Nodes];//ABA-轮-阶段-N个RBC:0/1/3(NULL)/ff（未收到）
//u8 ACK_Local_ABA_RBC_Init[MAX_ABA][MAX_round][Local_ABA_phase_Number][MAX_Nodes];

//RBC echo
u8 Local_ABA_RBC_Echo[MAX_ABA][MAX_round][Local_ABA_phase_Number];//ABA-轮-阶段-每个比特表示是否发出echo(1/0)
u8 Local_ABA_RBC_Echo_Number[MAX_ABA][MAX_round][Local_ABA_phase_Number][MAX_Nodes];//ABA-轮-阶段-N个RBC收集数量
u8 ACK_Local_ABA_RBC_Echo[MAX_ABA][MAX_round][Local_ABA_phase_Number][MAX_Nodes];//ABA-轮-阶段-N个RBC（针对每个节点）

//RBC ready
u8 Local_ABA_RBC_Ready[MAX_ABA][MAX_round][Local_ABA_phase_Number];//ABA-轮-阶段-每个比特表示是否发出ready(1/0)
u8 Local_ABA_RBC_Ready_Number[MAX_ABA][MAX_round][Local_ABA_phase_Number][MAX_Nodes];//ABA-轮-阶段-N个RBC收集数量
u8 ACK_Local_ABA_RBC_Ready[MAX_ABA][MAX_round][Local_ABA_phase_Number][MAX_Nodes];//ABA-轮-阶段-N个RBC（针对每个节点）

//est and vset
u8 Local_ABA_est[MAX_ABA][MAX_round][Local_ABA_phase_Number];
u8 Local_ABA_vset[MAX_ABA][MAX_round][Local_ABA_phase_Number][2];

//u8 ABA_round[MAX_ABA];//decide round
//u8 ABA_R[MAX_ABA];//current round
//u8 ABA_phase[MAX_ABA];


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
u8 VAL_CBC_v[MAX_Nodes][MAX_Nodes][Share_Size];
u8 ACK_VAL_CBC[MAX_Nodes][MAX_Nodes];
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
void Tx_init();
void ENC_handler();
u8 RBC_init_Number = 0;
u8 CBC_v_number = 0;
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
	TypeSTATE,
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

//开始任务任务函数
void start_task(void *pvParameters)
{
	start_Global_Consensus = 0;
	init();
	init_public_key();
	
	init_thres_enc();
	ABA_init();
	Tx_init();
	
	RBC_init();
	init_thres_sig();
	//CBC_init();
	//VAL_CBC_init();
	//VAL_CBC_test();
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
			else if((NotifyValue & EVENT_STATE)!=0)	//STATE发生	
			{
				//ABA STATE
				if(CBC_done_Number >= 2 * Nodes_f + 1 && CBC_v_number == Nodes_N - 1)	
					ABA_STATE_handler();
			}
			else if((NotifyValue&EVENT_PACKET)!=0)	//PACKET	
			{
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

				u8 er_flag = 1;
				if(RBC_done_Number < 2 * Nodes_f + 1 || need_EVENT_RBC_ER || RBC_init_Number < Nodes_N - 1 ){
					need_EVENT_RBC_ER = 0;
					RBC_ER_handler();
					er_flag = 0;
				}
					
				
				if(PRBC_done_Number < 2 * Nodes_f + 1 && RBC_done_Number >= 2 * Nodes_f + 1  && RBC_init_Number == Nodes_N - 1){
					if(er_flag)delay_ms(1000);
					PRBC_SIG_handler();
				}
			}
			else if((NotifyValue&EVENT_PRBC_SIG)!=0)	//PACKET	
			{
				PRBC_SIG_handler();
			}
			else if((NotifyValue&EVENT_CBC_STATE)!=0)	//PACKET	
			{
				CBC_STATE_handler();
			}
			else if((NotifyValue&EVENT_VAL_CBC_STATE)!=0)	//PACKET	
			{
				VAL_CBC_STATE_handler();
			}
			else if((NotifyValue&EVENT_ENC)!=0)	//PACKET	
			{
				if(ABA_decided[cur_aba_id] == 0x01)
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
void Tx_init(){
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
	if(pad_len > MAX_PROPOSAL)while(1);
	
	for(int i=0;i<pad_len;i++)
		tmp_tx_buff[PD_Size+Share_Size+i] = enc_proposal[i];
	
	Proposal_Packet_Size = PD_Size+Share_Size+pad_len;
	
	
//	while(1);
}
/*************************************************************************************/

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
	RBC_init_Number = 0;
	CBC_v_number = 0;
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
	
	
	///PRBC
	for(int i=0;i<MAX_Nodes;i++){
		ACK_PRBC_share_Number[i] = 0;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_PRBC_share[i][j] = 0;
			for(int m=0;m<Share_Size;m++){
				PRBC_share[i][j][m] = 0x00;
			}
		}
	}

	PRBC_done_Number = 0;
	
	//VAL_CBC
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
	
	VAL_CBC_done_Number = 0;
	W_arr[0] = W_arr[1] = 0;
	
	
	//CBC
	My_CBC_Echo_Number = 0;
	for(int i=0;i<MAX_Nodes;i++){
		ACK_CBC_Echo[i] = 0;
		CBC_Echo[i] = 0;
		ACK_CBC_Finish[i] = 0;
		for(int j=0;j<Share_Size;j++){
			CBC_Echo_Share[i][j] = 0x00;
			My_CBC_Echo_Share[i][j] = 0x00;
			CBC_Finish_Share[i][j] = 0x00;
		}
	}
	CBC_done_Number = 0;
}

void Global_ABA_init(){
	cur_aba_id = 1;
	for(int i=0;i<MAX_ABA;i++){
		ABA_decided[i] = 0xFF;//0xFF->NULL or decided on 1/0
		
//		Local_ABA_est[i][1][1] = 1;
		
//		Local_ABA_vset[i][1][1][0] = 1;
//		Local_ABA_vset[i][1][1][1] = 1;
		
		for(int j=0;j<MAX_round;j++){
			Local_ABA_votes_3_NULL[i][j] = 0;
			for(int k=0;k<Local_ABA_phase_Number;k++){
				Local_ABA_est[i][j][k] = 0xFF;
				Local_ABA_vset[i][j][k][0] = 0;
				Local_ABA_vset[i][j][k][1] = 0;
				
				Local_ABA_votes[i][j][k][0] = 0;
				Local_ABA_votes[i][j][k][1] = 0;
				
				Local_ABA_RBC_Echo[i][j][k] = 0;
				Local_ABA_RBC_Ready[i][j][k] = 0;
				
				ACK_Local_ABA_votes[i][j][k] = 0;
				for(int m=0;m<MAX_Nodes;m++){
					
					
					Local_ABA_RBC_Init[i][j][k][m] = 0xFF;
					//ACK_Local_ABA_RBC_Init[i][j][k][m] = 0xFF;
					
					Local_ABA_RBC_Echo_Number[i][j][k][m] = 0;
					
					Local_ABA_RBC_Ready_Number[i][j][k][m] = 0;
					ACK_Local_ABA_RBC_Echo[i][j][k][m] = 0;
					ACK_Local_ABA_RBC_Ready[i][j][k][m] = 0;
					for(int n=0;n<MAX_Nodes;n++){
						
					}
				}
			}
		}
	}
	
	
	//for round 1
//	for(int i=0;i<MAX_ABA;i++){
//		
//		if(ID <= 1){
//			Local_ABA_RBC_Init[i][1][1][ID] = 0;
//			Local_ABA_est[i][1][1] = 0;
//		}else{
//			Local_ABA_RBC_Init[i][1][1][ID] = 1;
//			Local_ABA_est[i][1][1] = 1;
//		}
//		
//		Local_ABA_RBC_Echo_Number[i][1][1][ID] = 1;
//		Local_ABA_RBC_Echo[i][1][1] |= (1<<(ID-1));
//		
//		Local_ABA_vset[i][1][1][0] = 1;
//		Local_ABA_vset[i][1][1][1] = 1;
//	}
	
	
	for(int i=0;i<MAX_ABA;i++)ABA_R[i] = 1;
	
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
	
//	//init VAL_CBC_v[]
//	for(int i=1;i<=Nodes_N - Nodes_f ;i++){
//		ACK_VAL_CBC[ID][i] = 1;
//		for(int j=0;j<Share_Size;j++)
//			VAL_CBC_v[ID][i][j] = RNG_Get_RandomNum();
//		ACK_VAL_CBC_Number[ID]++;
//	}
//	
//	VAL_CBC_SK_sign(ID);
	
	VAL_CBC_done_Number = 0;
	W_arr[0] = W_arr[1] = 0;
}

void CBC_init(){
	My_CBC_Echo_Number = 0;
	for(int i=0;i<MAX_Nodes;i++){
		ACK_CBC_Echo[i] = 0;
		CBC_Echo[i] = 0;
		ACK_CBC_Finish[i] = 0;
		for(int j=0;j<Share_Size;j++){
			CBC_Echo_Share[i][j] = 0x00;
			My_CBC_Echo_Share[i][j] = 0x00;
			CBC_Finish_Share[i][j] = 0x00;
		}
	}
//	for(int j=0;j<CBC_v_SIZE;j++){
//		CBC_v[ID][j] = RNG_Get_RandomNum();
//	}
	CBC_SK_sign(ID,CBC_v[ID],CBC_v_SIZE);
	
	CBC_done_Number = 0;
}

void RBC_init(){

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
		if(b == Block_Num - 1)Block_each_Size[ID][b] = Proposal_Packet_Size % Block_Size;
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
	
	///PRBC
	for(int i=0;i<MAX_Nodes;i++){
		ACK_PRBC_share_Number[i] = 0;
		for(int j=0;j<MAX_Nodes;j++){
			ACK_PRBC_share[i][j] = 0;
			for(int m=0;m<Share_Size;m++){
				PRBC_share[i][j][m] = 0x00;
			}
		}
	}

	PRBC_done_Number = 0;
}

void ABA_init(){
	cur_aba_id = 1;

	
	MYDMA_Config_Rece(DMA1_Stream1,DMA_CHANNEL_4);//初始化DMA
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);

	
	cur_aba_id = 1;
	for(int i=0;i<MAX_ABA;i++){
		ABA_decided[i] = 0xFF;//0xFF->NULL or decided on 1/0
		
//		Local_ABA_est[i][1][1] = 1;
		
//		Local_ABA_vset[i][1][1][0] = 1;
//		Local_ABA_vset[i][1][1][1] = 1;
		
		for(int j=0;j<MAX_round;j++){
			Local_ABA_votes_3_NULL[i][j] = 0;
			for(int k=0;k<Local_ABA_phase_Number;k++){
				Local_ABA_est[i][j][k] = 0xFF;
				Local_ABA_vset[i][j][k][0] = 0;
				Local_ABA_vset[i][j][k][1] = 0;
				
				Local_ABA_votes[i][j][k][0] = 0;
				Local_ABA_votes[i][j][k][1] = 0;
				
				Local_ABA_RBC_Echo[i][j][k] = 0;
				Local_ABA_RBC_Ready[i][j][k] = 0;
				
				ACK_Local_ABA_votes[i][j][k] = 0;
				for(int m=0;m<MAX_Nodes;m++){
					
					
					Local_ABA_RBC_Init[i][j][k][m] = 0xFF;
					//ACK_Local_ABA_RBC_Init[i][j][k][m] = 0xFF;
					
					Local_ABA_RBC_Echo_Number[i][j][k][m] = 0;
					
					Local_ABA_RBC_Ready_Number[i][j][k][m] = 0;
					ACK_Local_ABA_RBC_Echo[i][j][k][m] = 0;
					ACK_Local_ABA_RBC_Ready[i][j][k][m] = 0;
					for(int n=0;n<MAX_Nodes;n++){
						
					}
				}
			}
		}
	}
	
	
	//for round 1
//	for(int i=0;i<MAX_ABA;i++){
//		
//		if(ID <= 1){
//			Local_ABA_RBC_Init[i][1][1][ID] = 0;
//			Local_ABA_est[i][1][1] = 0;
//		}else{
//			Local_ABA_RBC_Init[i][1][1][ID] = 1;
//			Local_ABA_est[i][1][1] = 1;
//		}
//		
//		Local_ABA_RBC_Echo_Number[i][1][1][ID] = 1;
//		Local_ABA_RBC_Echo[i][1][1] |= (1<<(ID-1));
//		
//		Local_ABA_vset[i][1][1][0] = 1;
//		Local_ABA_vset[i][1][1][1] = 1;
//	}
	
	
	for(int i=0;i<MAX_ABA;i++)ABA_R[i] = 1;
	
//	TIM5_Exceed_Times = 0;
//	TIM2_Exceed_Times = 0;
//	TIM5->CNT = 0;
//	TIM2->CNT = 0;
	
	btim_tim5_enable(1);
	
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

}

void ABA_STATE_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	
	u32 random = RNG_Get_RandomRange(0, 5);
	random = random * 100;
	delay_ms(random);

	
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	if(Nodes_N<=8){
		u8 aba_id = state_aba;
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeSTATE;
		u8 padding = 70;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		u8 packet_idx = 3+padding;
		Send_Data_buff[packet_idx] = aba_id;packet_idx++;
		Send_Data_buff[packet_idx] = STATE_Set_Number;packet_idx++;
		for(int i=0;i<STATE_Set_Number;i++){
			Send_Data_buff[packet_idx] = STATE_Set[i];//round 
//			for(int a=1;a<=Nodes_N;a++){//ABA number
			
				for(int j=1;j<=3;j++){//phase number
					//init
					for(int k=1;k<=Nodes_N;k++){//RBC number
						if(Local_ABA_RBC_Init[aba_id][STATE_Set[i]][j][k] != 0xFF){
							Send_Data_buff[packet_idx+1+6*(j-1)] |= (1<<(k-1));	//ACK_init
							if(Local_ABA_RBC_Init[aba_id][STATE_Set[i]][j][k])
								Send_Data_buff[packet_idx+2+6*(j-1)] |= (1<<(k-1)); 	//init
							if(j == 3 && Local_ABA_RBC_Init[aba_id][STATE_Set[i]][j][k] == 3){
								Send_Data_buff[packet_idx+1+6*(j-1)] &= ~(1<<(k-1));//0
								Send_Data_buff[packet_idx+2+6*(j-1)] |= (1<<(k-1));//1
							}
						}
						
						if(Local_ABA_RBC_Echo_Number[aba_id][STATE_Set[i]][j][k] >= Nodes_N-Nodes_f){
							Send_Data_buff[packet_idx+3+6*(j-1)] |= (1<<(k-1)); 	//ACK_echo
//							Send_Data_buff[packet_idx+5+6*(j-1)] |= (1<<(k-1)); 	//ACK_ready
						}
						
						if((Local_ABA_RBC_Echo[aba_id][STATE_Set[i]][j] & (1<<(k-1))) != 0)
							Send_Data_buff[packet_idx+4+6*(j-1)] |= (1<<(k-1)); 	//echo
						
						if(Local_ABA_RBC_Ready_Number[aba_id][STATE_Set[i]][j][k] >= 2 * Nodes_f+1){
//							Send_Data_buff[packet_idx+3+6*(j-1)] |= (1<<(k-1)); 	//ACK_echo
							Send_Data_buff[packet_idx+5+6*(j-1)] |= (1<<(k-1)); 	//ACK_ready
						}	
						
						if((Local_ABA_RBC_Ready[aba_id][STATE_Set[i]][j] & (1<<(k-1))) != 0)	
							Send_Data_buff[packet_idx+6+6*(j-1)] |= (1<<(k-1)); 	//ready
					}
				}
				packet_idx = packet_idx + 3 * 6;
//			}
		}
		packet_idx++;
		//Sign
		uint8_t hash[32] = {0};
		uint8_t sig[64] = {0};
		sha2(Send_Data_buff,packet_idx,hash,0);
		sign_data(hash,sig);
		for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];
		
//		HAL_UART_DMAStop(&uart3_handler);
//		HAL_UART_Transmit_DMA(&uart3_handler,Send_Data_buff,packet_idx+Sig_Size);
		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		
	}else if(Nodes_N>8 && Nodes_N<=16){
	
		while(1);
	}
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

void ENC_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	if(Nodes_N <= 8){
		u8 padding = 0;
		if(dec_done_number >= 3)padding = 20;
		if(dec_done_number < 3)padding = 40;
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeTX_ENC;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		u8 packet_idx = 2 + padding + 1;
		//share nack
		//CBC_v[cur_aba_id][i]
		for(int i=0;i<CBC_v_SIZE;i++){
			//if(ABA_decided[k] == 1 && Tx_shares_Number[k] >= Enc_Thres_k)
			if(Tx_shares_Number[CBC_v[cur_aba_id][i]] >= Enc_Thres_k)
				Send_Data_buff[packet_idx] |= (1<<(CBC_v[cur_aba_id][i]-1));
		}
		packet_idx++;
		//share ack
		for(int i=0;i<CBC_v_SIZE;i++){
			//if(ABA_decided[k] == 1)
				Send_Data_buff[packet_idx] |= (1<<(CBC_v[cur_aba_id][i]-1));
		}
		packet_idx++;
		
		u8 cbc_decided[MAX_Nodes];
		for(int i=0;i<MAX_Nodes;i++)cbc_decided[i] = 0;
		for(int i=0;i<CBC_v_SIZE;i++)cbc_decided[CBC_v[cur_aba_id][i]] = 1;
		
		for(int k=1;k<=Nodes_N;k++){
			if(cbc_decided[k] == 1){
				for(int i=0;i<Share_Size;i++){
					Send_Data_buff[packet_idx + i] = Tx_shares_str[k][IDX-1][i]; 
				}
				packet_idx += Share_Size;
			}
		}
//		for(int j=0;j<CBC_v_SIZE;j++){
//			//if(ABA_decided[k] == 1){
//				for(int i=0;i<Share_Size;i++){
//					Send_Data_buff[packet_idx + i] = Tx_shares_str[CBC_v[cur_aba_id][j]][IDX-1][i]; 
//				}
//				packet_idx += Share_Size;
//			//}
//			
//		}
		
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

void RBC_ER_handler(){
	//DMA send ER packet
	//只实现了16节点以下的情况。分情况讨论的
	
	u32 random = RNG_Get_RandomRange(0, 5);
	random = random * 100;
	delay_ms(random);
	
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

void PRBC_SIG_handler(){
	
	u32 random = RNG_Get_RandomRange(0, 5);
	random = random * 100;
	delay_ms(random);
	
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	u8 ack_sig[MAX_Nodes];
	for(int i=0;i<MAX_Nodes;i++)ack_sig[i] = 0x00;
	ack_sig[1] = ack_sig[2] = ack_sig[3] = ack_sig[4] = 1;
	if(Nodes_N <= 8){
		
		u8 padding = 0;
		u8 sig_num = 0;for(int k=1;k<=Nodes_N;k++)if(ACK_PRBC_share[IDX][k] == 1)sig_num++;
		if(sig_num <= 2) padding = 50;
		else padding = 30;
		
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypePRBC_SIG;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		
		u8 packet_idx = 3 + padding;
		
		//sig_nack
		for(int k=1;k<=Nodes_N;k++){
			if(ACK_PRBC_share_Number[k] >= 2 * Nodes_f + 1){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		packet_idx++;
		
		//sig_ack
		for(int k=1;k<=Nodes_N;k++){
			if(ACK_PRBC_share[k][IDX] && ack_sig[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		packet_idx++;
		for(int k=1;k<=Nodes_N;k++){
			if(ACK_PRBC_share[k][IDX] && ack_sig[k]){
				for(int i=0;i<Share_Size;i++){
					Send_Data_buff[packet_idx + i] = PRBC_share[k][IDX][i];
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

void CBC_STATE_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	
	u32 random = RNG_Get_RandomRange(0, 5);
	random = random * 100;
	delay_ms(random);
	
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	u8 echo_set[MAX_Nodes];
	for(int i=0;i<MAX_Nodes;i++) echo_set[i] = 0;
	echo_set[1] = echo_set[2] = echo_set[3] = echo_set[4] = 1;
	if(Nodes_N <= 8){
		
		u8 sig_num = 0;
		for(int k=1 ; k <= Nodes_N ; k++){
			if(echo_set[k] && CBC_Echo[k]){
				sig_num++;
			}
		}
		
		u8 padding = 0;
		padding = (Nodes_N - sig_num) * 32;
		
		
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeCBC_STATE;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		
		u8 packet_idx = 3 + padding;
		
		//echo ack
		for(int k=1 ; k <= Nodes_N ; k++){
			if(echo_set[k] && CBC_Echo[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		packet_idx++;
		//echo nack
		for(int k=1 ; k<=Nodes_N ; k++){
			if(k == IDX)continue;
			if(ACK_CBC_Echo[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		//if(My_CBC_Echo_Number >= 2 * Nodes_f + 1) Send_Data_buff[packet_idx] = 0xFF;
		//if(My_CBC_Echo_Number >= Nodes_N) Send_Data_buff[packet_idx] = 0xFF;
		
		packet_idx++;
		
		//Finish nack
		for(int k=1 ; k<=Nodes_N ; k++){
			if(ACK_CBC_Finish[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		
		packet_idx++;
		
		for(int i=0;i<CBC_v_SIZE;i++)Send_Data_buff[packet_idx + i] = CBC_v[IDX][i];
		
		packet_idx += CBC_v_SIZE;
		
		//signature for other v
		for(int k=1 ; k <= Nodes_N ; k++){
			
			if(echo_set[k] && CBC_Echo[k]){
				for(int i=0;i<Share_Size;i++){
					Send_Data_buff[packet_idx+i] = CBC_Echo_Share[k][i];
				}
			packet_idx+=Share_Size;
			}
		} 
		
//		if(My_CBC_Echo_Number >= 2 * Nodes_f + 1){
		if(My_CBC_Echo_Number >= 2 * Nodes_f + 1){	
			if( ACK_CBC_Finish[IDX] == 0){
				CBC_PK_sig_combine_share();
				if(CBC_verify_sig(CBC_v[IDX], CBC_v_SIZE, CBC_Finish_Share[IDX]) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"verify Thres Fail11111111");
						Lora_Device_Sta = LORA_TX_STA;
						LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
					}
				}
				ACK_CBC_Finish[IDX] = 1;
				CBC_done_Number++;
			}
			
			for(int i=0;i<Share_Size;i++){
				Send_Data_buff[packet_idx+i] = CBC_Finish_Share[IDX][i];
			}
			packet_idx+=Share_Size;
			
			if(CBC_verify_sig(CBC_v[IDX], CBC_v_SIZE, CBC_Finish_Share[IDX]) == 0){
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

void VAL_CBC_STATE_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	
	u32 random = RNG_Get_RandomRange(0, 5);
	random = random * 100;
	delay_ms(random);
	
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	u8 echo_set[MAX_Nodes];
	u8 W_set[MAX_Nodes];
	for(int i=0;i<MAX_Nodes;i++) echo_set[i] = 0;
	echo_set[1] = echo_set[2] = echo_set[3] = echo_set[4] = 1;
	for(int i=0;i<MAX_Nodes;i++) W_set[i] = 0;
	W_set[1] = W_set[2] = W_set[3] = W_set[4] = 1;
	u8 W_flag;
for(int w_id=0;w_id<2;w_id++){
	
	if(W_arr[w_id]){
		W_arr[w_id] = 0;
		W_flag = w_id;
	}
	else{
		continue;
	}
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	//W_flag = 1;
	if(Nodes_N <= 8){
		
		u8 sig_num = 0;
		for(int k=1 ; k <= Nodes_N ; k++){
			if(echo_set[k] && VAL_CBC_Echo[k]){
				sig_num++;
			}
		}
		
		u8 padding = 0;
		if(sig_num >= 2) padding = 40;
		if(sig_num < 2) padding = 60;
		if(W_flag) padding = 40;
		
		//start
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeVAL_CBC_STATE;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		
		u8 packet_idx = 3 + padding;
		
		//echo ack
		if(W_flag == 0){
			for(int k=1 ; k <= Nodes_N ; k++){
				if(echo_set[k] && VAL_CBC_Echo[k]){
					Send_Data_buff[packet_idx] |= (1<<(k-1));
				}	
			}
		}
		
		packet_idx++;
		//echo nack
		for(int k=1 ; k<=Nodes_N ; k++){
			if(k == IDX)continue;
			if(ACK_VAL_CBC_Echo[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		if(My_VAL_CBC_Echo_Number >= 2 * Nodes_f + 1) Send_Data_buff[packet_idx] = 0xFF;
		if(W_flag) Send_Data_buff[packet_idx] &= ~(1<<(IDX-1));
		//if(My_VAL_CBC_Echo_Number >= Nodes_N) Send_Data_buff[packet_idx] = 0xFF;
		
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
			if(ACK_VAL_CBC_Number[k] == Nodes_N - Nodes_f){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		packet_idx++;
		
		//W
		if(W_flag)
		for(int k=1;k<=Nodes_N;k++){
			if(ACK_VAL_CBC[IDX][k] && W_set[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}
		}
		
		if(Send_Data_buff[3] && Send_Data_buff[7]){
			u8 xx = 1;
			xx = xx+1;
		}
		
		packet_idx++;
		//////////////
		if(W_flag){
			for(int k=1;k<=Nodes_N;k++){
				if(W_set[k] && ACK_VAL_CBC[IDX][k]){
					for(int i=0;i<Share_Size;i++){
						Send_Data_buff[packet_idx+i] = VAL_CBC_v[IDX][k][i];
					}
				packet_idx+=Share_Size;
				}
			}
		}else{
			//signature for other v
			for(int k=1 ; k <= Nodes_N ; k++){
				
				if(echo_set[k] && VAL_CBC_Echo[k]){
					for(int i=0;i<Share_Size;i++){
						Send_Data_buff[packet_idx+i] = VAL_CBC_Echo_Share[k][i];
					}
				packet_idx+=Share_Size;
				}
			} 
			
			//Finish
			if(My_VAL_CBC_Echo_Number >= 2 * Nodes_f + 1){	
				if( ACK_VAL_CBC_Finish[IDX] == 0){
					VAL_CBC_PK_sig_combine_share();
					if(VAL_CBC_verify_sig(IDX) == 0){
						while(1){
							sprintf((char*)Send_Data_buff,"verify Thres Fail11111111");
							Lora_Device_Sta = LORA_TX_STA;
							LoRa_SendData(Send_Data_buff);// idx 0 "Hello LoRa"
						}
					}
					ACK_VAL_CBC_Finish[IDX] = 1;
					if(VAL_CBC_done_Number <= 2 * Nodes_f){
						CBC_v[IDX][VAL_CBC_done_Number] = IDX;
						//ABA
//						est[IDX][1] = 1;
//						BVAL[IDX][1][1] = 1;
//						ACK_BVAL_Number[IDX][1][1] = 1;
						Local_ABA_RBC_Init[IDX][1][1][IDX] = 1;
						Local_ABA_est[IDX][1][1] = 1;
						Local_ABA_RBC_Echo_Number[IDX][1][1][IDX] = 1;
						Local_ABA_RBC_Echo[IDX][1][1] |= (1<<(IDX-1));
						Local_ABA_vset[IDX][1][1][0] = 1;
						Local_ABA_vset[IDX][1][1][1] = 1;
					}
					
					VAL_CBC_done_Number++;
					
					if(VAL_CBC_done_Number == 2 * Nodes_f + 1){
						CBC_init();
						for(int q=0;q<=MAX_ABA;q++){
//							if(est[q][1] == 0xFF){
//								est[q][1] = 0;
//								BVAL[q][1][0] = 1;
//								ACK_BVAL_Number[q][1][0] = 1;
//							}
						if(Local_ABA_RBC_Init[q][1][1][IDX] == 0xFF){
//							est[m][1] = 1;
//							BVAL[m][1][1] = 1;
//							ACK_BVAL_Number[m][1][1] = 1;
							Local_ABA_RBC_Init[q][1][1][IDX] = 0;
							Local_ABA_est[q][1][1] = 0;
							Local_ABA_RBC_Echo_Number[q][1][1][IDX] = 1;
							Local_ABA_RBC_Echo[q][1][1] |= (1<<(IDX-1));
							Local_ABA_vset[q][1][1][0] = 1;
							Local_ABA_vset[q][1][1][1] = 1;
					}
						}
					}
					
				}
				
				for(int i=0;i<Share_Size;i++){
					Send_Data_buff[packet_idx+i] = VAL_CBC_Finish_Share[IDX][i];
				}
				packet_idx+=Share_Size;
				
				if(VAL_CBC_verify_sig(IDX) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"verify Thres Fail22222222222");
						Lora_Device_Sta = LORA_TX_STA;
						LoRa_SendData(Send_Data_buff);// idx 0 "Hello LoRa"
					}
				}
				
			}
		}
		
		// if(Send_Data_buff[3] && Send_Data_buff[7]){
		// 	u8 xx = 1;
		// 	xx = xx+1;
		// }
		
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
				if(RBC_Echo_Number[k] == Nodes_N - Nodes_f){
					RBC_Ready[k] = 1;
					RBC_Ready_Number[k]++;
				}
			}
			if((ready_VAL & (1<<(k-1))) != 0 && ACK_RBC_Ready[k][s_id] == 0){
				RBC_Ready_Number[k]++;
				ACK_RBC_Ready[k][s_id] = 1;
				if(RBC_Ready_Number[k] >= Nodes_N - Nodes_f){
					//accept it
					// go to ABA
					if(RBC_done[k] == 0){
						RBC_done[k] = 1;
						RBC_done_Number++;
						//PRBC sign k
						PRBC_SK_sign(k);
//						ACK_PRBC_share[k][IDX] = 1;
						ACK_PRBC_share_Number[k]++;
						if(PRBC_PK_verify_sig_share(IDX,k,PRBC_share[k][IDX]) == 0){
							while(1){
								sprintf((char*)Send_Data_buff,"MyMyMy_verify_sig_share: No");
								LoRa_SendData(Send_Data_buff);
							}
						}
						ACK_PRBC_share[k][IDX] = 1;
						
						if(ACK_PRBC_share_Number[k] == 2 * Nodes_f + 1){
							//combine
							PRBC_PK_sig_combine_share(k);
							//verify
							if(PRBC_PK_verify_sig(k,PRBC_sig[k]) == 0){
								while(1){
									sprintf((char*)Send_Data_buff,"Verify_sig: No");
									LoRa_SendData(Send_Data_buff);
								}
							}
							PRBC_done_Number++;
							if(PRBC_done_Number == 2 * Nodes_f + 1){
								VAL_CBC_init();
								//init VAL_CBC_v[]
								for(int kk=1;kk<=Nodes_N;kk++){
									if(ACK_PRBC_share_Number[kk] >= 2*Nodes_f+1){
										ACK_VAL_CBC[IDX][kk] = 1;
										for(int j=0;j<Share_Size;j++)
											VAL_CBC_v[IDX][kk][j] = PRBC_sig[kk][j];
										ACK_VAL_CBC_Number[IDX]++;
									}
								}
								VAL_CBC_SK_sign(IDX);
							}
						}
					}
				}
			}
		}
		if(reply_er){
			need_EVENT_RBC_ER = 1;
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
	else if(packet_type == TypePRBC_SIG && verify_flag && RBC_done_Number >= 2 * Nodes_f + 1 && RBC_init_Number == Nodes_N - 1){
		//DMA send STATE packet
		//只实现了16节点以下的情况。分情况讨论的
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 nack_flag = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		
		u8 sig_nack = buff[packet_idx + 0];
		u8 sig_ack = buff[packet_idx + 1];
		
		for(int k=1;k<=Nodes_N;k++){
			if((sig_nack & (1<<(k-1))) == 0 && ACK_PRBC_share[k][IDX] == 1){
				nack_flag = 1;
			}
		}
		packet_idx += 2;
		for(int k=1;k<=Nodes_N;k++){
			if((sig_ack & (1<<(k-1))) != 0 && ACK_PRBC_share[k][s_id] == 0){// && ACK_PRBC_share[k][ID] == 1){
				for(int i=0;i<Share_Size;i++){
					PRBC_share[k][s_id][i] = buff[packet_idx + i];
				}
				//verify
				if(PRBC_PK_verify_sig_share(s_id,k,PRBC_share[k][s_id]) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"Verify_sig_share: No");
						LoRa_SendData(Send_Data_buff);
					}
				}
				ACK_PRBC_share[k][s_id] = 1;
				ACK_PRBC_share_Number[k]++;
				
				if(ACK_PRBC_share_Number[k] == 2 * Nodes_f + 1){
					//combine
					PRBC_PK_sig_combine_share(k);
					//verify
					if(PRBC_PK_verify_sig(k,PRBC_sig[k]) == 0){
						while(1){
							sprintf((char*)Send_Data_buff,"Verify_sig: No");
							LoRa_SendData(Send_Data_buff);
						}
					}
					PRBC_done_Number++;
					if(PRBC_done_Number == 2 * Nodes_f + 1){
						VAL_CBC_init();
						//init VAL_CBC_v[]
						for(int kk=1;kk<=Nodes_N;kk++){
							if(ACK_PRBC_share_Number[kk] >= 2*Nodes_f+1){
								ACK_VAL_CBC[IDX][kk] = 1;
								for(int j=0;j<Share_Size;j++)
									VAL_CBC_v[IDX][kk][j] = PRBC_sig[kk][j];
								ACK_VAL_CBC_Number[IDX]++;
							}
						}
						VAL_CBC_SK_sign(IDX);
					}
					
				}
			}
			if((sig_ack & (1<<(k-1))) != 0)
				packet_idx += Share_Size;
		}
		
		if(nack_flag){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_PRBC_SIG,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
	}
	else if(packet_type == TypeSTATE && verify_flag && CBC_done_Number >= 2 * Nodes_f + 1 && CBC_v_number == Nodes_N - 1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 padding = buff[2];
		u8 reply_flag = 0;
		u8 packet_idx = 3 + padding;
		u8 aba_id = buff[packet_idx];packet_idx++;
		u8 round_number = buff[packet_idx];packet_idx++;
		
		//u8 min_round = 0xff;for(int i=1;i<=Nodes_N;i++){if(min_round > ABA_R[i])min_round = ABA_R[i];}
		
		for(int i=0;i<round_number;i++){
			u8 round = buff[packet_idx]; 
			//if(min_round > round){STATE_Set_Number = 1;STATE_Set[0] = round;}
//			for(int a=1;a<=Nodes_N;a++){//ABA number
				for(int j=1;j<=3;j++){//phase number
					//init
					u8 init_ACK = buff[packet_idx + 1 + (j - 1) * 6];
					u8 init_VAL = buff[packet_idx + 2 + (j - 1) * 6];
					u8 echo_ACK = buff[packet_idx + 3 + (j - 1) * 6];
					u8 echo_VAL = buff[packet_idx + 4 + (j - 1) * 6];//s_id 对N个RBC是否发出echo
					u8 ready_ACK = buff[packet_idx + 5 + (j - 1) * 6];
					u8 ready_VAL = buff[packet_idx + 6 + (j - 1) * 6];//s_id 对N个RBC是否发出ready
					
					//表示目前 a round j 
					if(Local_ABA_vset[aba_id][round][j][0]==0 && Local_ABA_vset[aba_id][round][j][1]==0)continue;
					
					/**********ACK***********/
					if(j <= 2 && (init_ACK & (1<<(IDX-1))) == 0 && Local_ABA_est[aba_id][round][j] != 0xFF) reply_flag = 1;
					if(j == 3 && (init_ACK & (1<<(IDX-1))) == 0 && (init_VAL & (1<<(IDX-1))) == 0 && Local_ABA_est[aba_id][round][j] != 0xFF) reply_flag = 1;
					for(int k=1;k<=Nodes_N;k++){//RBC number
						if((echo_ACK & (1<<(k-1)))==0 && (Local_ABA_RBC_Echo[aba_id][round][j] & (1<<(k-1))) != 0)reply_flag = 1;	
						if((ready_ACK & (1<<(k-1)))==0 && (Local_ABA_RBC_Ready[aba_id][round][j] & (1<<(k-1))) != 0)reply_flag = 1;
					}
					/**********VAL***********/
					//init
					if((init_ACK & (1<<(s_id-1))) != 0 && Local_ABA_RBC_Init[aba_id][round][j][s_id] == 0xFF){
						if((init_VAL & (1<<(s_id-1))) != 0)
							Local_ABA_RBC_Init[aba_id][round][j][s_id] = 1;
						else Local_ABA_RBC_Init[aba_id][round][j][s_id] = 0;
						//echo
						Local_ABA_RBC_Echo[aba_id][round][j] |= (1<<(s_id-1));
						Local_ABA_RBC_Echo_Number[aba_id][round][j][s_id]++;
						if(Local_ABA_RBC_Echo_Number[aba_id][round][j][s_id] >= Nodes_N - Nodes_f){
							Local_ABA_RBC_Echo[aba_id][round][j] |= (1<<(s_id-1));
							Local_ABA_RBC_Ready[aba_id][round][j] |= (1<<(s_id-1));
							//no need for RBC k Echo
							if(Local_ABA_vset[aba_id][round][j][Local_ABA_RBC_Init[aba_id][round][j][s_id]])
								ACK_Local_ABA_RBC_Echo[aba_id][round][j][s_id] = 0xFF;
						}
					}else if(j == 3 && (init_ACK & (1<<(s_id-1))) == 0 && (init_VAL & (1<<(s_id-1))) != 0 && Local_ABA_RBC_Init[aba_id][round][j][s_id] == 0xFF){
						Local_ABA_RBC_Init[aba_id][round][j][s_id] = 3;
						//echo
						Local_ABA_RBC_Echo[aba_id][round][j] |= (1<<(s_id-1));
						Local_ABA_RBC_Echo_Number[aba_id][round][j][s_id]++;
						
						if(Local_ABA_RBC_Echo_Number[aba_id][round][j][s_id] >= Nodes_N - Nodes_f){
							Local_ABA_RBC_Echo[aba_id][round][j] |= (1<<(s_id-1));
							Local_ABA_RBC_Ready[aba_id][round][j] |= (1<<(s_id-1));
							//no need for RBC k Echo
							if(Local_ABA_vset[aba_id][round][j][0] && Local_ABA_vset[aba_id][round][j][1])
								ACK_Local_ABA_RBC_Echo[aba_id][round][j][s_id] = 0xFF;
						}
					}
					//echo
					for(int k=1;k<=Nodes_N;k++){//RBC number
						if((echo_VAL & (1<<(k-1))) != 0 && (ACK_Local_ABA_RBC_Echo[aba_id][round][j][k] & (1<<(s_id-1))) == 0){
							Local_ABA_RBC_Echo_Number[aba_id][round][j][k]++;
							ACK_Local_ABA_RBC_Echo[aba_id][round][j][k] |= (1<<(s_id-1));
							
							if(Local_ABA_RBC_Echo_Number[aba_id][round][j][k] >= Nodes_N - Nodes_f){
								if((init_VAL & (1<<(k-1))) != 0)
									Local_ABA_RBC_Init[aba_id][round][j][k] = 1;
								else Local_ABA_RBC_Init[aba_id][round][j][k] = 0;
								
								//if(j == 3 && (init_VAL & (1<<(k-1))) != 0)
								if(j == 3 && (init_VAL & (1<<(k-1))) != 0 && (init_ACK & (1<<(k-1))) == 0)
									Local_ABA_RBC_Init[aba_id][round][j][k] = 3;
								
								Local_ABA_RBC_Echo[aba_id][round][j] |= (1<<(k-1));
								Local_ABA_RBC_Ready[aba_id][round][j] |= (1<<(k-1));
								
								//no need for RBC k Echo
								if(j <= 2){
									if(Local_ABA_vset[aba_id][round][j][Local_ABA_RBC_Init[aba_id][round][j][k]])
										ACK_Local_ABA_RBC_Echo[aba_id][round][j][k] = 0xFF;
								}else if(j == 3){
									if(Local_ABA_vset[aba_id][round][j][0] && Local_ABA_vset[aba_id][round][j][1])
										ACK_Local_ABA_RBC_Echo[aba_id][round][j][k] = 0xFF;
								}
								
							}
						
						}
					}

					//ready
					for(int k=1;k<=Nodes_N;k++){//RBC number
						if((ready_VAL & (1<<(k-1))) != 0 && (ACK_Local_ABA_RBC_Ready[aba_id][round][j][k] & (1<<(s_id-1))) == 0){
							Local_ABA_RBC_Ready_Number[aba_id][round][j][k]++;
							ACK_Local_ABA_RBC_Ready[aba_id][round][j][k] |= (1<<(s_id-1));
							if(Local_ABA_RBC_Ready_Number[aba_id][round][j][k] >= Nodes_f + 1){//ready_number >= f + 1
								if((init_VAL & (1<<(k-1))) != 0)
									Local_ABA_RBC_Init[aba_id][round][j][k] = 1;
								else Local_ABA_RBC_Init[aba_id][round][j][k] = 0;
								
								//if(j == 3 && (init_VAL & (1<<(k-1))) != 0)
								if(j == 3 && (init_VAL & (1<<(k-1))) != 0 && (init_ACK & (1<<(k-1))) == 0)
									Local_ABA_RBC_Init[aba_id][round][j][k] = 3;
								
								Local_ABA_RBC_Echo[aba_id][round][j] |= (1<<(k-1));
								Local_ABA_RBC_Ready[aba_id][round][j] |= (1<<(k-1));
							}
							if(Local_ABA_RBC_Ready_Number[aba_id][round][j][k] >= 2 * Nodes_f + 1){//ready_number >= 2*f + 1
								//accept go on phase? round?
								
								//no need for RBC k Ready
								//ACK_Local_ABA_RBC_Ready[a][round][j][k] = 0xFF;
								if(j <= 2){
									if(Local_ABA_vset[aba_id][round][j][Local_ABA_RBC_Init[aba_id][round][j][k]])
										ACK_Local_ABA_RBC_Ready[aba_id][round][j][k] = 0xFF;
								}else if(j == 3){
									if(Local_ABA_vset[aba_id][round][j][0] && Local_ABA_vset[aba_id][round][j][1])
										ACK_Local_ABA_RBC_Ready[aba_id][round][j][k] = 0xFF;
								}
								
								
								if((ACK_Local_ABA_votes[aba_id][round][j] & (1<<(k-1))) == 0){
									ACK_Local_ABA_votes[aba_id][round][j] |= (1<<(k-1));
									if(Local_ABA_RBC_Init[aba_id][round][j][k] == 1){
										//check if in vset
										if(Local_ABA_vset[aba_id][round][j][1])
											Local_ABA_votes[aba_id][round][j][1]++;
									}
									else if(Local_ABA_RBC_Init[aba_id][round][j][k] == 0){
										//check if in vset
										if(Local_ABA_vset[aba_id][round][j][0])
											Local_ABA_votes[aba_id][round][j][0]++;
										
										if(j == 3 && Local_ABA_RBC_Init[aba_id][round][j][k] == 3)
											Local_ABA_votes_3_NULL[aba_id][round]++;
									}
								}
								if(j <= 2 && (Local_ABA_votes[aba_id][round][j][1] + Local_ABA_votes[aba_id][round][j][0] == Nodes_N - Nodes_f)){
									//统计完votes了，而且收到了N-f
									u8 votes0 = Local_ABA_votes[aba_id][round][j][0];
									u8 votes1 = Local_ABA_votes[aba_id][round][j][1];
									if(j == 1){//phase 1
										if(votes0 == Nodes_N - Nodes_f || votes1 == Nodes_N - Nodes_f ){
											//correct
											u8 vote;
											if(votes0 == Nodes_N - Nodes_f)vote = 0;
											else vote = 1;
											
											//decide
											if(ABA_decided[aba_id] == 0xFF){
												ABA_decided[aba_id] = vote;
												if(vote == 0)cur_aba_id++;
												else {
													//thres_enc
													if(!start_Global_Consensus){
														//single_hop
														for(int m=0;m<CBC_v_SIZE;m++){
															dec_done_number++;
															//decrypt the tx
															u8 dec_id = CBC_v[aba_id][m];
															Tx_SK_decrypt_share(dec_id, IDX);
															if(ACK_Share[dec_id][IDX] == 0){
																ACK_Share[dec_id][IDX] = 1;
																Tx_shares_Number[dec_id]++;
															}
														}
													}
													else{
														//multi-hop
														for(int m=0;m<CBC_v_SIZE;m++){
															dec_done_number++;
															//decrypt the tx
															u8 dec_id = CBC_v[aba_id][m];
															Global_Tx_SK_decrypt_share(dec_id, IDX);
															if(ACK_Share[dec_id][IDX] == 0){
																ACK_Share[dec_id][IDX] = 1;
																Tx_shares_Number[dec_id]++;
															}
														}
													}
													MY_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
												}
											}
											else if(ABA_decided[aba_id] != vote){
												//error decide on different value
												while(1);
											}
											//iv_r+1 = v
											Local_ABA_est[aba_id][round + 1][1] = vote;
											//vset = {v}
											Local_ABA_vset[aba_id][round][2][vote] = 1;
											
											//for next phase
											Local_ABA_est[aba_id][round][2] = vote;
											
											// to next phase RBC
											Local_ABA_RBC_Init[aba_id][round][2][IDX] = vote;
											Local_ABA_RBC_Echo_Number[aba_id][round][2][IDX] = 1;
											Local_ABA_RBC_Echo[aba_id][round][2] |= (1<<(IDX-1));
											
										}else{
											u8 vote;
											if(votes0 > votes1) vote = 0;
											else vote = 1;
											
											//vset = {0,1}
											Local_ABA_vset[aba_id][round][2][0] = 1;
											Local_ABA_vset[aba_id][round][2][1] = 1;
											//for next phase
											Local_ABA_est[aba_id][round][2] = vote;
											
											// to next phase RBC
											Local_ABA_RBC_Init[aba_id][round][2][IDX] = vote;
											Local_ABA_RBC_Echo_Number[aba_id][round][2][IDX] = 1;
											Local_ABA_RBC_Echo[aba_id][round][2] |= (1<<(IDX-1));
										}
									}else if(j == 2){//phase 2
										if(votes0 >= (Nodes_N/2) || votes1 >= (Nodes_N/2) ){
											u8 vote;
											if(votes0 >= (Nodes_N/2))vote = 0;
											else vote = 1;
											
											//vset = {v}
											Local_ABA_vset[aba_id][round][3][vote] = 1;
											
											//for next phase
											Local_ABA_est[aba_id][round][3] = vote;
											
											// to next phase RBC
											Local_ABA_RBC_Init[aba_id][round][3][IDX] = vote;
											Local_ABA_RBC_Echo_Number[aba_id][round][3][IDX] = 1;
											Local_ABA_RBC_Echo[aba_id][round][3] |= (1<<(IDX-1));
										}else{
											//vset = {0,1}
											Local_ABA_vset[aba_id][round][3][0] = 1;
											Local_ABA_vset[aba_id][round][3][1] = 1;
											//for next phase
											Local_ABA_est[aba_id][round][3] = 3;//NULL
											
											// to next phase RBC
											Local_ABA_RBC_Init[aba_id][round][3][IDX] = 3;
											Local_ABA_RBC_Echo_Number[aba_id][round][3][IDX] = 1;
											Local_ABA_RBC_Echo[aba_id][round][3] |= (1<<(IDX-1));
										}
									}
								}
								if(j == 3){
									if(Local_ABA_votes[aba_id][round][j][0] + Local_ABA_votes[aba_id][round][j][1] + Local_ABA_votes_3_NULL[i][j] == Nodes_N - Nodes_f){
										u8 votes0 = Local_ABA_votes[aba_id][round][j][0];
										u8 votes1 = Local_ABA_votes[aba_id][round][j][1];
										if(votes0 == 2 * Nodes_f + 1 || votes1 == 2 * Nodes_f + 1){
											u8 vote;
											if(votes0 == 2 * Nodes_f + 1)vote = 0;
											else vote = 1;
											//decide
											if(ABA_decided[aba_id] == 0xFF){
												ABA_decided[aba_id] = vote;
												if(vote == 0)cur_aba_id++;
												else {
													//thres_enc
													if(!start_Global_Consensus){
														//single_hop
														for(int m=0;m<CBC_v_SIZE;m++){
															dec_done_number++;
															//decrypt the tx
															u8 dec_id = CBC_v[aba_id][m];
															Tx_SK_decrypt_share(dec_id, IDX);
															if(ACK_Share[dec_id][IDX] == 0){
																ACK_Share[dec_id][IDX] = 1;
																Tx_shares_Number[dec_id]++;
															}
														}
													}
													else{
														//multi-hop
														for(int m=0;m<CBC_v_SIZE;m++){
															dec_done_number++;
															//decrypt the tx
															u8 dec_id = CBC_v[aba_id][m];
															Global_Tx_SK_decrypt_share(dec_id, IDX);
															if(ACK_Share[dec_id][IDX] == 0){
																ACK_Share[dec_id][IDX] = 1;
																Tx_shares_Number[dec_id]++;
															}
														}
													}
													MY_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
												}
											}
											else if(ABA_decided[aba_id] != vote){
												//error decide on different value
												while(1);
											}
											
											//iv_r+1 = v
											Local_ABA_est[aba_id][round + 1][1] = vote;
											//vset = {v}
											Local_ABA_vset[aba_id][round + 1][1][vote] = 1;
											
											// to next round RBC
											Local_ABA_RBC_Init[aba_id][round + 1][1][IDX] = vote;
											Local_ABA_RBC_Echo_Number[aba_id][round + 1][1][IDX] = 1;
											Local_ABA_RBC_Echo[aba_id][round + 1][1] |= (1<<(IDX-1));
											
										}else if(votes0 == Nodes_f + 1 || votes1 == Nodes_f + 1){
											u8 vote;
											if(votes0 == Nodes_f + 1)vote = 0;
											else vote = 1;
											//iv_r+1 = v
											Local_ABA_est[aba_id][round + 1][1] = vote;
											//vset = {0,1}
											Local_ABA_vset[aba_id][round + 1][1][0] = 1;
											Local_ABA_vset[aba_id][round + 1][1][1] = 1;
											
											// to next round RBC
											Local_ABA_RBC_Init[aba_id][round + 1][1][IDX] = vote;
											Local_ABA_RBC_Echo_Number[aba_id][round + 1][1][IDX] = 1;
											Local_ABA_RBC_Echo[aba_id][round + 1][1] |= (1<<(IDX-1));
										}else{
											//iv_r+1 = v
											Local_ABA_est[aba_id][round + 1][1] = RNG_Get_RandomNum() % 2;
											//vset = {0,1}
											Local_ABA_vset[aba_id][round + 1][1][0] = 1;
											Local_ABA_vset[aba_id][round + 1][1][1] = 1;
											
											// to next round RBC
											Local_ABA_RBC_Init[aba_id][round + 1][1][IDX] = Local_ABA_est[aba_id][round + 1][1];
											Local_ABA_RBC_Echo_Number[aba_id][round + 1][1][IDX] = 1;
											Local_ABA_RBC_Echo[aba_id][round + 1][1] |= (1<<(IDX-1));
										}
										if(round == ABA_R[aba_id])
											ABA_R[aba_id]++;
									}
									
								}
							}
						}
					}
				}
				packet_idx = packet_idx + 3 * 6;
//			}
		if(reply_flag){STATE_Set_Number = 1;STATE_Set[0] = round;}
		}
		if(reply_flag){
			state_aba = aba_id;
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_STATE,			//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
	}
	else if(packet_type == TypeCBC_STATE && verify_flag && VAL_CBC_done_Number >= 2 * Nodes_f + 1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 nack_flag = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		u8 echo_ack = buff[packet_idx + 0];
		u8 echo_nack = buff[packet_idx + 1];
		u8 finish_nack = buff[packet_idx + 2];

		packet_idx += 3;
		//finish nack
		if((finish_nack & (1 << (IDX - 1))) == 0 && ACK_CBC_Finish[IDX] == 1){
			nack_flag = 1;
		}
		
		//v
		if(CBC_Echo[s_id] == 0){
			for(int i=0;i<CBC_v_SIZE;i++){
				CBC_v[s_id][i] = buff[packet_idx + i];
			}
			//threshold signature
			CBC_SK_sign(s_id,CBC_v[s_id],CBC_v_SIZE);
			CBC_Echo[s_id] = 1;
			nack_flag = 1;
			CBC_v_number++;
		}

		packet_idx += CBC_v_SIZE;
		for(int k=1;k<=Nodes_N;k++){
			if((echo_ack & (1<<(k-1))) != 0 && k == IDX && ACK_CBC_Echo[s_id] == 0){
				for(int i=0;i<Share_Size;i++){
					My_CBC_Echo_Share[s_id][i] = buff[packet_idx + i];
				}
				
				//验证share, verify
				if(CBC_PK_verify_sig_share(s_id, CBC_v[IDX], CBC_v_SIZE, My_CBC_Echo_Share[s_id]) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"verify share Thres Fail");
						Lora_Device_Sta = LORA_TX_STA;
						LoRa_SendData(Send_Data_buff);// idx 0 "Hello LoRa"
					}
				}
				ACK_CBC_Echo[s_id] = 1;
				My_CBC_Echo_Number++;
			}
			if((echo_ack & (1<<(k-1))) != 0)
				packet_idx += Share_Size;
		}
		
		if((echo_nack & (1<<(IDX-1))) == 0){nack_flag = 1;}
		if((finish_nack & (1<<(s_id-1))) != 0 && ACK_CBC_Finish[s_id] == 0){
			for(int i=0;i<Share_Size;i++){
				CBC_Finish_Share[s_id][i] = buff[packet_idx + i];
			}
			//验证share, verify
			if(CBC_verify_sig(CBC_v[s_id], CBC_v_SIZE, CBC_Finish_Share[s_id]) == 0){
				while(1){
					sprintf((char*)Send_Data_buff,"verify Thres Fail");
					Lora_Device_Sta = LORA_TX_STA;
					LoRa_SendData(Send_Data_buff);// idx 0 "Hello LoRa"
				}
			}
			//verify share
			ACK_CBC_Finish[s_id] = 1;
			CBC_done_Number++;
		}
		if((echo_nack & (1<<(s_id-1))) != 0) packet_idx += Share_Size;
		
		if(nack_flag && VAL_CBC_done_Number >= 2 * Nodes_f + 1){
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_CBC_STATE,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		
	}
	else if(packet_type == TypeVAL_CBC_STATE && verify_flag && PRBC_done_Number >= 2 * Nodes_f + 1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 nack_flag = 0;
		u8 W_nack = 0;
		u8 padding = buff[2];
		u8 packet_idx = 3+padding;
		u8 echo_ack = buff[packet_idx + 0];
		u8 echo_nack = buff[packet_idx + 1];
		u8 finish_nack = buff[packet_idx + 2];
		u8 init_nack = buff[packet_idx + 3];
		u8 W_set = buff[packet_idx + 4]; 
		
		packet_idx += 5;
		
		//finish nack
		if((finish_nack & (1 << (IDX - 1))) == 0 && ACK_VAL_CBC_Finish[IDX] == 1){
			nack_flag = 1;
		}
		
		//init nack
		if((init_nack & (1 << (IDX - 1))) == 0){
			W_nack = 1;
		}
		
		//W
		for(int k=1;k<=Nodes_N;k++){
			if((W_set & (1<<(k-1))) != 0 && ACK_VAL_CBC[s_id][k] == 0){
				ACK_VAL_CBC[s_id][k] = 1;
				ACK_VAL_CBC_Number[s_id]++;
				for(int i=0;i<Share_Size;i++){
					VAL_CBC_v[s_id][k][i] = buff[packet_idx + i];
				}
				if(ACK_VAL_CBC_Number[s_id] == Nodes_N - Nodes_f){
					//verify
					VAL_CBC_SK_sign(s_id);
				}
			}
			if((W_set & (1<<(k-1))) != 0){
				packet_idx += Share_Size;
			}
		}
		
		//echo nack
		if((echo_nack & (1<<(IDX-1))) == 0 && VAL_CBC_Echo[s_id] == 1){
			nack_flag = 1;
		}
		
		//signature for IDX
		for(int k=1;k<=Nodes_N;k++){
			if((echo_ack & (1<<(k-1))) != 0 && k == IDX && ACK_VAL_CBC_Echo[s_id] == 0){
				for(int i=0;i<Share_Size;i++){
					My_VAL_CBC_Echo_Share[s_id][i] = buff[packet_idx + i];
				}
				
				//验证share, verify
				if(VAL_CBC_PK_verify_sig_share(s_id) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"verify share Thres Fail");
						Lora_Device_Sta = LORA_TX_STA;
						LoRa_SendData(Send_Data_buff);// idx 0 "Hello LoRa"
					}
				}
				ACK_VAL_CBC_Echo[s_id] = 1;
				My_VAL_CBC_Echo_Number++;
				if(My_VAL_CBC_Echo_Number >= 2 * Nodes_f + 1){	
					if( ACK_VAL_CBC_Finish[IDX] == 0){
						VAL_CBC_PK_sig_combine_share();
						if(VAL_CBC_verify_sig(IDX) == 0){
							while(1){
								sprintf((char*)Send_Data_buff,"verify Thres Fail11111111");
								Lora_Device_Sta = LORA_TX_STA;
								LoRa_SendData(Send_Data_buff);// idx 0 "Hello LoRa"
							}
						}
						ACK_VAL_CBC_Finish[IDX] = 1;
						if(VAL_CBC_done_Number <= 2 * Nodes_f){
							CBC_v[IDX][VAL_CBC_done_Number] = IDX;
							//ABA
//							est[IDX][1] = 1;
//							BVAL[IDX][1][1] = 1;
//							ACK_BVAL_Number[IDX][1][1] = 1;
							Local_ABA_RBC_Init[IDX][1][1][IDX] = 1;
							Local_ABA_est[IDX][1][1] = 1;
							Local_ABA_RBC_Echo_Number[IDX][1][1][IDX] = 1;
							Local_ABA_RBC_Echo[IDX][1][1] |= (1<<(IDX-1));
							Local_ABA_vset[IDX][1][1][0] = 1;
							Local_ABA_vset[IDX][1][1][1] = 1;
						}
						VAL_CBC_done_Number++;
						if(VAL_CBC_done_Number == 2 * Nodes_f + 1){
							CBC_init();
							for(int q=0;q<=MAX_ABA;q++){
//								if(est[q][1] == 0xFF){
//									est[q][1] = 0;
//									BVAL[q][1][0] = 1;
//									ACK_BVAL_Number[q][1][0] = 1;
//								}
								if(Local_ABA_RBC_Init[q][1][1][IDX] == 0xFF){
//									est[m][1] = 1;
//									BVAL[m][1][1] = 1;
//									ACK_BVAL_Number[m][1][1] = 1;
									Local_ABA_RBC_Init[q][1][1][IDX] = 0;
									Local_ABA_est[q][1][1] = 0;
									Local_ABA_RBC_Echo_Number[q][1][1][IDX] = 1;
									Local_ABA_RBC_Echo[q][1][1] |= (1<<(IDX-1));
									Local_ABA_vset[q][1][1][0] = 1;
									Local_ABA_vset[q][1][1][1] = 1;
								}
								
							}
						}
					}
				}
			}
			if((echo_ack & (1<<(k-1))) != 0)
				packet_idx += Share_Size;
		}
		//signature for finish s_id
		if((echo_nack & (1<<(s_id-1))) != 0 && ACK_VAL_CBC_Finish[s_id] == 0 && ACK_VAL_CBC_Number[s_id] == Nodes_N - Nodes_f){
			for(int i=0;i<Share_Size;i++){
				VAL_CBC_Finish_Share[s_id][i] = buff[packet_idx + i];
			}
			//验证share, verify
			if(VAL_CBC_verify_sig(s_id) == 0){
				while(1){
					sprintf((char*)Send_Data_buff,"verify Thres Fail");
					Lora_Device_Sta = LORA_TX_STA;
					LoRa_SendData(Send_Data_buff);// idx 0 "Hello LoRa"
				}
			}
			//verify share
			ACK_VAL_CBC_Finish[s_id] = 1;

			if(VAL_CBC_done_Number <= 2 * Nodes_f){
				CBC_v[IDX][VAL_CBC_done_Number] = s_id;
				//ABA
//				est[s_id][1] = 1;
//				BVAL[s_id][1][1] = 1;
//				ACK_BVAL_Number[s_id][1][1] = 1;
				Local_ABA_RBC_Init[s_id][1][1][IDX] = 1;
				Local_ABA_est[s_id][1][1] = 1;
				Local_ABA_RBC_Echo_Number[s_id][1][1][IDX] = 1;
				Local_ABA_RBC_Echo[s_id][1][1] |= (1<<(IDX-1));
				Local_ABA_vset[s_id][1][1][0] = 1;
				Local_ABA_vset[s_id][1][1][1] = 1;
				
			}
			
			VAL_CBC_done_Number++;
			if(VAL_CBC_done_Number == 2 * Nodes_f + 1){
				CBC_init();
				for(int q=0;q<=Nodes_N;q++){
//					if(est[q][1]==0xFF){
//						est[q][1] = 0;
//						BVAL[q][1][0] = 1;
//						ACK_BVAL_Number[q][1][0] = 1;
//					}
					if(Local_ABA_RBC_Init[q][1][1][IDX] == 0xFF){
//						est[m][1] = 1;
//						BVAL[m][1][1] = 1;
//						ACK_BVAL_Number[m][1][1] = 1;
						Local_ABA_RBC_Init[q][1][1][IDX] = 0;
						Local_ABA_est[q][1][1] = 0;
						Local_ABA_RBC_Echo_Number[q][1][1][IDX] = 1;
						Local_ABA_RBC_Echo[q][1][1] |= (1<<(IDX-1));
						Local_ABA_vset[q][1][1][0] = 1;
						Local_ABA_vset[q][1][1][1] = 1;
					}
				}
			}
		}
		
		if(nack_flag){
			W_arr[0] = 1;
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_VAL_CBC_STATE,		//要更新的bit
						(eNotifyAction	)eSetBits);				//更新指定的bit
		}
		if(W_nack){
			if(nack_flag) delay_ms(1000);
			W_arr[1] = 1;
			xTaskNotify((TaskHandle_t	)EventGroupTask_Handler,//接收任务通知的任务句柄
						(uint32_t		)EVENT_VAL_CBC_STATE,		//要更新的bit
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
		
		//CBC_v[cur_aba_id][i]
		if(ABA_decided[cur_aba_id] == 0x01)
		for(int k=1;k<=Nodes_N;k++){
			u8 aba_flag = 0;
			for(int i=0;i<CBC_v_SIZE;i++)if(CBC_v[cur_aba_id][i] == k)aba_flag=1;
			if((enc_nack & (1<<(k-1))) == 0 && aba_flag){
				replay_enc = 1;
				break;
			}
		}
		if(ABA_decided[cur_aba_id] == 0x01)
		for(int k=1;k<=Nodes_N;k++){
			if((enc_ack & (1<<(k-1))) != 0 && ACK_Share[k][s_id] == 0){
				for(int i=0;i<Share_Size;i++){
					Tx_shares_str[k][s_id - 1][i] = buff[packet_idx+i];
				}
				
				octet tmp_o;
				tmp_o.len = tmp_o.max = Share_Size;
				tmp_o.val = (char*)Tx_shares_str[k][s_id-1];
				ECP_BN158_fromOctet(&Tx_shares[k][s_id-1],&tmp_o);
				
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
								u8 cbc_decided[MAX_Nodes];
								for(int i=0;i<MAX_Nodes;i++)cbc_decided[i] = 0;
								for(int i=0;i<CBC_v_SIZE;i++)cbc_decided[CBC_v[cur_aba_id][i]] = 1;
								
								u32 tx_idx = 0;
								for(int k=1;k<=Nodes_N;k++){
									if(cbc_decided[k] == 1){
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
								LoRa_CFG.chn = DATA_channel;
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
		u8 packet_idx = 3 + padding;
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
	
	
	////send 
//	for(int i=0;i<300;i++)ReceBuff[i] = 0x00;
	HAL_UART_DMAStop(&uart3_handler);
////	HAL_DMA_Init(&UART3RxDMA_Handler);
	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);//开启DMA传输
}


























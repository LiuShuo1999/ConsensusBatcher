
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
u32 Packet_len=0;
u32 MY_TIME = 0;
u32 MY_Consensus_TIME = 0;
/****************ABA_Shared***************************************/
u8 cur_aba_id;
u8 state_aba;
u32 TIM5_Exceed_Times;
u32 TIM2_Exceed_Times;
u32 TIM9_Exceed_Times;
u8 Send_Data_buff[800] = {0};
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
void Tx_init();

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
	LoRa_CFG.chn = chat_channel;
	LoRa_CFG.tmode = LORA_TMODE_PT;
	LoRa_CFG.addr = 0;
	LoRa_CFG.power = LORA_TPW_9DBM;
	LoRa_CFG.wlrate = LORA_RATE_62K5;
	LoRa_CFG.lbt = LORA_LBT_ENABLE;
	LoRa_Set();
}


u8 decidd_flag = 1;

void ABA_init();
void ABA_BVAL_handler(u8 aba, u8 round, u8 bval_val);
void ABA_AUX_handler(u8 aba, u8 round, u8 aux_val);
void ABA_SHARE_handler(u8 aba, u8 round);
void ABA_NACK_handler();


void RBC_init();
void RBC_INIT_handler();
void RBC_ECHO_handler(u8 rbc_id);
void RBC_READY_handler(u8 rbc_id);
void RBC_NACK_handler();
void RBC_ER_handler();


void PRBC_SIG_handler(u8 prbc_id);
void PRBC_NACK_handler();


void VAL_CBC_init();
void CBC_NACK_handler();
void CBC_INIT_handler();
void CBC_ECHO_handler(u8 cbc_id);
void CBC_FINISH_handler();


void CBC_init();
void VAL_CBC_NACK_handler();
void VAL_CBC_INIT_handler();
void VAL_CBC_ECHO_handler(u8 val_cbc_id);
void VAL_CBC_FINISH_handler();

void ENC_NACK_handler();
void ENC_handler(u8 rbc_id);

void PACKET_handler();

typedef enum
{
	//TypeNACK = 0,
	TypeRBC_INIT,
	TypeRBC_ER,
	TypeRBC_NACK,
	TypePRBC_SIG,
	TypePRBC_NACK,
	TypeCBC_STATE,//5
	TypeCBC_NACK,
	TypeVAL_CBC_STATE,//7
	TypeVAL_CBC_NACK,
	TypeSTATE,
	TypeNACK,
	TypeTX_ENC,//B
	TypeTX_ENC_NACK,
	TypeTX_SIG,
	Global_TypeRBC_INIT
} PacketType;
typedef enum
{
	Type_ECHO = 0,
	Type_READY,
} RBCType;
typedef enum
{
	TypeBVAL = 0,
	TypeAUX,
	TypeSHARE
}ABAType;

typedef enum
{
	TypeVAL_CBC_INIT = 0,
	TypeVAL_CBC_ECHO,
	TypeVAL_CBC_FINISH
} VAL_CBCType;

typedef enum
{
	TypeCBC_INIT = 0,
	TypeCBC_ECHO,
	TypeCBC_FINISH
}CBCType;



void global_consensus_init(){
    //tx
    u32 tx_idx = 0;
    for(int k=1;k<=3;k++){
        for(int i=0;i<Proposal_Size;i++){
            Global_Transaction[Group_ID][tx_idx] = RNG_Get_RandomNum();
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
    
    u8 tmp_id = 2 + 4 * (Group_ID-1);
    while(1){
        if (!uECC_sign(private_key[tmp_id], Global_hash_tx[Group_ID], 32, Global_Sig_hash[tmp_id], uECC_secp160r1())) {
            sprintf((char*)Send_Data_buff,"Sign Fail");
            Lora_Device_Sta = LORA_TX_STA;
            LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
        }else{
            break;
        }
    }
    ACK_Global_Sig_hash[tmp_id] = 1;
    Global_Sig_hash_Number++;
    Global_Consensus_Init();



}

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
	
	global_consensus_init();
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
			}				
			else if((NotifyValue&EVENTBIT_1)!=0)	//事件1发生	
			{
			}
			else if((NotifyValue&EVENTBIT_2)!=0)	//事件2发生	
			{
			}
			else if((NotifyValue & EVENT_NACK)!=0)	//STATE发生	
			{
				//ABA STATE
				if(CBC_done_Number >= 2 * Nodes_f + 1 && CBC_v_number == Nodes_N - 1)	
					ABA_NACK_handler();
			}
			else if((NotifyValue&EVENT_PACKET)!=0)	//PACKET	
			{
				PACKET_handler();
			}
//			else if((NotifyValue&EVENT_RBC_INIT)!=0)	//PACKET	
//			{
//				
////				u32 random = RNG_Get_RandomRange(0, 5);
////				random = random * 200;
////				delay_ms(random);
////				
////				if(start_Global_Consensus == 0)
////					RBC_INIT_handler();
////				else 
////					Global_RBC_INIT_handler();
//			}
			else if((NotifyValue&EVENT_RBC_NACK)!=0)	//PACKET	
			{
				RBC_NACK_handler();
//				u8 er_flag = 1;
//				if(RBC_done_Number < 2 * Nodes_f + 1 || need_EVENT_RBC_ER  || RBC_init_Number < Nodes_N - 1){
//					need_EVENT_RBC_ER = 0;
//					RBC_ER_handler();
//					er_flag = 0;
//				}
//					
//				
//				if(PRBC_done_Number < 2 * Nodes_f + 1 && RBC_done_Number >= 2 * Nodes_f + 1 && RBC_init_Number == Nodes_N - 1){
//					if(er_flag)delay_ms(1000);
//					PRBC_SIG_handler();
//				}
				
			}
			else if((NotifyValue&EVENT_PRBC_NACK)!=0)	//PACKET	
			{
				PRBC_NACK_handler();
			}
			else if((NotifyValue&EVENT_CBC_NACK)!=0)	//PACKET	
			{
				CBC_NACK_handler();
			}
			else if((NotifyValue&EVENT_VAL_CBC_NACK)!=0)	//PACKET	
			{
				VAL_CBC_NACK_handler();
			}
			else if((NotifyValue&EVENT_ENC_NACK)!=0)	//PACKET	
			{
				//sprintf((char*)Send_Data_buff,"STATE: Get EVENT_PACKET.");LoRa_SendData(Send_Data_buff);
				if(ABA_decided[cur_aba_id] == 0x01)
				ENC_NACK_handler();
			}
			else if((NotifyValue&EVENT_SIG)!=0)	//PACKET	
			{
				if(!start_Global_Consensus)
					SIG_handler();
			}
		}
		else
		{
//			sprintf((char*)Send_Data_buff,"ERROR: Get mission.");
//			LoRa_SendData(Send_Data_buff);	
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
//	LoRa_CFG.chn = Global_channel;
//	LoRa_CFG.tmode = LORA_TMODE_PT;
//	LoRa_CFG.addr = 0;
//	LoRa_Set();
	
//	MYDMA_Config_Rece(DMA1_Stream1,DMA_CHANNEL_4);//初始化DMA
//	HAL_UART_Receive_DMA(&uart3_handler,ReceBuff,DMA_buff_Size);
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
	for(int a=0;a<MAX_ABA;a++){
		for(int i=0;i<MAX_round;i++){
			ABA_share_Number[a][i] = 0x00;
			Shared_Coin[a][i] = 0xFF;
			for(int j=0;j<MAX_Nodes;j++){
				for(int k=0;k<Share_Size;k++)ABA_share[a][i][j][k] = 0x00;
				ACK_ABA_share[a][i][j] = 0x00;
			}
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
//		est[i][1] = 0;
//		BVAL[i][1][0] = 1;
//		ACK_BVAL_Number[i][1][0] = 1;
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
		usart3_rx(0);
	for(int block_id = 0;block_id<Global_Group_Block_Num[Group_ID];block_id++){
		u8 padding = 0;
		if(Global_Block_each_Size[Group_ID][block_id] < 100) padding = 100 - Global_Block_each_Size[Group_ID][block_id];
		Send_Data_buff[0] = Group_ID;
		Send_Data_buff[1] = Global_TypeRBC_INIT;
//		Send_Data_buff[2] = padding;
//		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		
		u8 packet_idx = 2;
		//block-tot
		Send_Data_buff[packet_idx] = Global_Group_Block_Num[Group_ID];
		packet_idx++;
		//block-id
		Send_Data_buff[packet_idx] = block_id;
		packet_idx++;
//		//ack-init
//		for(int k=1;k<=Nodes_N;k++){
//			if(RBC_Init[k]){
//				Send_Data_buff[packet_idx] |= (1<<(k-1));
//			}
//		}
//		packet_idx++;
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

		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		
		u32 random = RNG_Get_RandomRange(0, 5);
		random = random * 100;
		delay_ms(random);
		
	}
	usart3_rx(1);
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
	for(int a=0;a<MAX_ABA;a++){
		for(int i=0;i<MAX_round;i++){
			ABA_share_Number[a][i] = 0x00;
			Shared_Coin[a][i] = 0xFF;
			for(int j=0;j<MAX_Nodes;j++){
				for(int k=0;k<Share_Size;k++)ABA_share[a][i][j][k] = 0x00;
				ACK_ABA_share[a][i][j] = 0x00;
			}
		}
	}
	
	for(int a=0;a<MAX_ABA;a++)
	for(int i=0;i<MAX_round;i++){
		est[a][i] = 0x00;
		bin_values[a][i][0] = 0x00;bin_values[a][i][1] = 0x00;
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
	
	for(int i=0;i<MAX_ABA;i++)ABA_R[i] = 1;

	for(int i=0;i<MAX_ABA;i++)
		for(int j=0;j<MAX_round;j++)
			est[i][j] = 0xFF;
	
//	for(int i=0;i<MAX_ABA;i++){
//		est[i][1] = 0;
//		BVAL[i][1][0] = 1;
//		ACK_BVAL_Number[i][1][0] = 1;
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
	u8 aba_id = state_aba;
	if(Nodes_N<=8){// && ABA_decided[1]==0xFF && ABA_decided[2]==0xFF && ABA_decided[3]==0xFF && ABA_decided[4]==0xFF){
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeSTATE;
		u8 padding = 50;
		Send_Data_buff[2] = padding;
		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
		u8 packet_idx = 3 + padding;
		Send_Data_buff[packet_idx] = STATE_Set_Number;
		for(int o=0;o<STATE_Set_Number;o++){
			packet_idx++;
			Send_Data_buff[packet_idx] = aba_id;//aba number
			packet_idx++;
			Send_Data_buff[packet_idx] = STATE_Set[o];//round number
			packet_idx++;
			u8 round = STATE_Set[o];
			
			//my BVAL
			if(BVAL[aba_id][round][0])
				Send_Data_buff[packet_idx + 0] |= (1<<(IDX-1));
			if(BVAL[aba_id][round][1])
				Send_Data_buff[packet_idx + 1] |= (1<<(IDX-1));
			//my AUX
			if(AUX[aba_id][round][0])
				Send_Data_buff[packet_idx + 2] |= (1<<(IDX-1));
			if(AUX[aba_id][round][1])
				Send_Data_buff[packet_idx + 3] |= (1<<(IDX-1));
			
			//nack BVAL AUX
			for(int k=1;k<=Nodes_N;k++){
				if(k == IDX)continue;
				//BVAL0
				if(ACK_BVAL[aba_id][round][k][0])
					Send_Data_buff[packet_idx + 0] |= (1<<(k-1));
				//BVAL1
				if(ACK_BVAL[aba_id][round][k][1])
					Send_Data_buff[packet_idx + 1] |= (1<<(k-1));
				//AUX0
				if(ACK_AUX[aba_id][round][k][0])
					Send_Data_buff[packet_idx + 2] |= (1<<(k-1));
				//AUX1
				if(ACK_AUX[aba_id][round][k][1])
					Send_Data_buff[packet_idx + 3] |= (1<<(k-1));
			}
			packet_idx += 4;
			
			//share_nack
			for(int k=1;k<=Nodes_N;k++)
				if(ACK_ABA_share[aba_id][round][k])Send_Data_buff[packet_idx] |= (1<<(k-1));
			packet_idx++;
			
			//share
			//share 自己的share 有没有计算?
			if(ACK_ABA_share[aba_id][round][IDX] == 0){
				Coin_SK_sign(aba_id,round);
			}
			for(int i=0;i<Share_Size;i++){
				Send_Data_buff[packet_idx + i] = ABA_share[aba_id][round][IDX][i];
			}
			packet_idx += Share_Size;
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






void RBC_INIT_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	if(Nodes_N <= 8){
	for(int block_id = 0;block_id<Block_Num;block_id++){
		Send_Data_buff[0] = ID;
		Send_Data_buff[1] = TypeRBC_INIT;
//		Send_Data_buff[2] = padding;
//		for(int i=0;i<padding;i++)Send_Data_buff[3+i] = 0xFF;
//		
		u8 packet_idx = 2;
		Send_Data_buff[packet_idx] = block_id;
		packet_idx++;
//		for(int k=1;k<=Nodes_N;k++){
//			if(RBC_Init[k]){
//				Send_Data_buff[packet_idx] |= (1<<(k-1));
//			}
//		}
//		packet_idx++;
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

		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		u32 random = RNG_Get_RandomRange(0, 5);
		random = random * 50;
		delay_ms(random);
	}
	}

}

void RBC_ECHO_handler(u8 rbc_id){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	if(RBC_Init[rbc_id] == 0 || RBC_Echo[rbc_id] == 0) return;
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	u8 padding = 0;
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeRBC_ER;
	Send_Data_buff[2] = Type_ECHO;
	Send_Data_buff[3] = rbc_id;
	u8 packet_idx = 4;
	for(int i=0;i<Hash_Size;i++){
		Send_Data_buff[packet_idx + i] = hash_v[rbc_id][i];
	}
	packet_idx += Hash_Size;
	
	//Sign
	uint8_t hash[32] = {0};
	uint8_t sig[64] = {0};
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
	HASH256_hash(&sh256,hash);
	
	sign_data(hash,sig);
	for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];

	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
}

void RBC_READY_handler(u8 rbc_id){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	if(RBC_Init[rbc_id] == 0 || RBC_Ready[rbc_id] == 0) return;
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	u8 padding = 0;
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeRBC_ER;
	Send_Data_buff[2] = Type_READY;
	Send_Data_buff[3] = rbc_id;
	u8 packet_idx = 4;
	for(int i=0;i<Hash_Size;i++){
		Send_Data_buff[packet_idx + i] = hash_v[rbc_id][i];
	}
	packet_idx += Hash_Size;
	
	//Sign
	uint8_t hash[32] = {0};
	uint8_t sig[64] = {0};
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
	HASH256_hash(&sh256,hash);
	
	sign_data(hash,sig);
	for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];

	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
}

void RBC_NACK_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int rbc_id = 1;rbc_id <= Nodes_N;rbc_id++){
		if(RBC_done[rbc_id])continue;
		
		for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeRBC_NACK;
		Send_Data_buff[2] = rbc_id;
		Send_Data_buff[3] = RBC_Init[rbc_id];
		//ECHO
		u8 packet_idx = 4;
		for(int i=1;i<=Nodes_N;i++){
			if(RBC_Echo_Number[rbc_id] >= 2 * Nodes_f + 1) Send_Data_buff[packet_idx+i-1] = 1;
			else{
				Send_Data_buff[packet_idx+i-1] = ACK_RBC_Echo[rbc_id][i];
				if(i == IDX)Send_Data_buff[packet_idx+i-1] = RBC_Echo[rbc_id];
			}
		}
		packet_idx += Nodes_N;
		//READY
		for(int i=1;i<=Nodes_N;i++){
			if(RBC_Ready_Number[rbc_id] >= 2 * Nodes_f + 1) Send_Data_buff[packet_idx+i-1] = 1;
			else{
				Send_Data_buff[packet_idx+i-1] = ACK_RBC_Ready[rbc_id][i];
				if(i == IDX)Send_Data_buff[packet_idx+i-1] = RBC_Ready[rbc_id];
			}
		}
		packet_idx += Nodes_N;
		
		//Sign
		uint8_t hash[32] = {0};
		uint8_t sig[64] = {0};
		hash256 sh256;
		HASH256_init(&sh256);
		for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
		HASH256_hash(&sh256,hash);
		
		sign_data(hash,sig);
		for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];

		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		u32 random = RNG_Get_RandomRange(0, 5);
		random = random * 50;
		delay_ms(random);
	}
}










void PRBC_SIG_handler(u8 prbc_id){
	
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	
	if(!ACK_PRBC_share[prbc_id][IDX]){return;}
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypePRBC_SIG;
	Send_Data_buff[2] = prbc_id;
	u8 packet_idx = 3;

	for(int i=0;i<Share_Size;i++){
		Send_Data_buff[packet_idx + i] = PRBC_share[prbc_id][IDX][i];
	}
	packet_idx += Share_Size;
	
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

	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
	

}

void PRBC_NACK_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	for(int prbc_id = 1; prbc_id <= Nodes_N; prbc_id++){
		if(ACK_PRBC_share_Number[prbc_id] >= 2 * Nodes_f + 1)continue;
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypePRBC_NACK;
		Send_Data_buff[2] = prbc_id;
		
		u8 packet_idx = 3;
		
		//sig_nack
		for(int k=1;k<=Nodes_N;k++){
			if(ACK_PRBC_share[prbc_id][k]){
				Send_Data_buff[packet_idx + k - 1] = 1;
			}
		}
		packet_idx+= Nodes_N;
		
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
		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		
		u32 random = RNG_Get_RandomRange(0, 5);
		random = random * 50;
		delay_ms(random);
	}
	
	




}









void CBC_STATE_handler(){
	//DMA send STATE packet
	//只实现了16节点以下的情况。分情况讨论的
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


void CBC_NACK_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	
	for(int cbc_id=1;cbc_id<=Nodes_N;cbc_id++){
		if(ACK_CBC_Finish[cbc_id])continue;
		
		for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;

		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeCBC_NACK;
		Send_Data_buff[2] = cbc_id;
		u8 packet_idx = 3;
		//init nack
		if(CBC_Echo[cbc_id] == 1){
			Send_Data_buff[packet_idx] = 1;
		}
		packet_idx++;
		//echo nack
		if(IDX == cbc_id){
			for(int k=1 ; k<=Nodes_N ; k++){
				if(k == IDX)continue;
				if(ACK_CBC_Echo[k]){
					Send_Data_buff[packet_idx] |= (1<<(k-1));
				}	
			}
		}
		packet_idx++;
		
		//Finish nack
		for(int k=1 ; k<=Nodes_N ; k++){
			if(ACK_CBC_Finish[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		packet_idx++;
		
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
		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		
		u32 random = RNG_Get_RandomRange(0, 5);
		random = random * 50;
		delay_ms(random);
	}
}
void CBC_INIT_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeCBC_STATE;
	Send_Data_buff[2] = TypeCBC_INIT;
	u8 packet_idx = 3;
	
	for(int i=0;i<CBC_v_SIZE;i++)
		Send_Data_buff[packet_idx+i] = CBC_v[IDX][i];
	
	packet_idx += CBC_v_SIZE;
	
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
	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
}
void CBC_ECHO_handler(u8 cbc_id){
	if(!CBC_Echo[cbc_id])return;
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeCBC_STATE;
	Send_Data_buff[2] = TypeCBC_ECHO;
	Send_Data_buff[3] = cbc_id;
	u8 packet_idx = 4;
	
	if(CBC_Echo[cbc_id]){
		for(int i=0;i<Share_Size;i++){
			Send_Data_buff[packet_idx+i] = CBC_Echo_Share[cbc_id][i];
		}
	}
	packet_idx+=Share_Size;
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
	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
	
}
void CBC_FINISH_handler(){
	
	
	if(My_CBC_Echo_Number < 2 * Nodes_f + 1) return;
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
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
	
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeCBC_STATE;
	Send_Data_buff[2] = TypeCBC_FINISH;
	u8 packet_idx = 3;
	
	if(ACK_CBC_Finish[IDX]){
		for(int i=0;i<Share_Size;i++){
			Send_Data_buff[packet_idx+i] = CBC_Finish_Share[IDX][i];
		}
	}
	packet_idx+=Share_Size;
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
	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
}







void VAL_CBC_NACK_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	
	for(int val_cbc_id=1;val_cbc_id<=Nodes_N;val_cbc_id++){
		if(ACK_VAL_CBC_Finish[val_cbc_id])continue;
		
		for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
		
		
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeVAL_CBC_NACK;
		Send_Data_buff[2] = val_cbc_id;
		u8 packet_idx = 3;
		//init nack
		if(ACK_VAL_CBC_Number[val_cbc_id] == Nodes_N - Nodes_f){
			Send_Data_buff[packet_idx] = 1;
		}
		packet_idx++;
		//echo nack
		if(IDX == val_cbc_id){
			for(int k=1 ; k<=Nodes_N ; k++){
				if(k == IDX)continue;
				if(ACK_VAL_CBC_Echo[k]){
					Send_Data_buff[packet_idx] |= (1<<(k-1));
				}	
			}
		}
		packet_idx++;
		
		//Finish nack
		for(int k=1 ; k<=Nodes_N ; k++){
			if(ACK_VAL_CBC_Finish[k]){
				Send_Data_buff[packet_idx] |= (1<<(k-1));
			}	
		}
		packet_idx++;
		
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
		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		
		u32 random = RNG_Get_RandomRange(0, 5);
		random = random * 50;
		delay_ms(random);
	}
}


void VAL_CBC_INIT_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeVAL_CBC_STATE;
	Send_Data_buff[2] = TypeVAL_CBC_INIT;
	u8 packet_idx = 3;
	
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_VAL_CBC[IDX][k]){
			Send_Data_buff[packet_idx] |= (1<<(k-1));
		}
	}
	packet_idx++;
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_VAL_CBC[IDX][k]){
			for(int i=0;i<Share_Size;i++){
				Send_Data_buff[packet_idx+i] = VAL_CBC_v[IDX][k][i];
			}
		packet_idx+=Share_Size;
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
	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
	
}

void VAL_CBC_ECHO_handler(u8 val_cbc_id){
	if(!VAL_CBC_Echo[val_cbc_id])return;
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeVAL_CBC_STATE;
	Send_Data_buff[2] = TypeVAL_CBC_ECHO;
	Send_Data_buff[3] = val_cbc_id;
	u8 packet_idx = 4;
	
	if(VAL_CBC_Echo[val_cbc_id]){
		for(int i=0;i<Share_Size;i++){
			Send_Data_buff[packet_idx+i] = VAL_CBC_Echo_Share[val_cbc_id][i];
		}
	}
	packet_idx+=Share_Size;
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
	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
	
}



void VAL_CBC_FINISH_handler(){
	if(My_VAL_CBC_Echo_Number < 2 * Nodes_f + 1) return;
	
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	if( ACK_VAL_CBC_Finish[IDX] == 0){
		VAL_CBC_PK_sig_combine_share();
		if(VAL_CBC_verify_sig(IDX) == 0){
			while(1){
				sprintf((char*)Send_Data_buff,"verify Thres Fail11111111");
				Lora_Device_Sta = LORA_TX_STA;
				LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
			}
		}
		ACK_VAL_CBC_Finish[IDX] = 1;
		if(VAL_CBC_done_Number <= 2 * Nodes_f){
			CBC_v[IDX][VAL_CBC_done_Number] = IDX;
			//ABA
			est[IDX][1] = 1;
			BVAL[IDX][1][1] = 1;
			ACK_BVAL_Number[IDX][1][1] = 1;
		}
		
		VAL_CBC_done_Number++;
		
		if(VAL_CBC_done_Number == 2 * Nodes_f + 1){
			CBC_init();
			for(int q=0;q<=MAX_ABA;q++){
				if(est[q][1] == 0xFF){
					est[q][1] = 0;
					BVAL[q][1][0] = 1;
					ACK_BVAL_Number[q][1][0] = 1;
				}
			}
		}
	}
	
	
	
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeVAL_CBC_STATE;
	Send_Data_buff[2] = TypeVAL_CBC_FINISH;
	u8 packet_idx = 3;
	
	if(ACK_VAL_CBC_Finish[IDX]){
		for(int i=0;i<Share_Size;i++){
			Send_Data_buff[packet_idx+i] = VAL_CBC_Finish_Share[IDX][i];
		}
	}
	packet_idx+=Share_Size;
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
	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
	
}






void ABA_BVAL_handler(u8 aba, u8 round, u8 bval_val){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	
	if(!(bval_val ==  0 || bval_val == 1))return;
	if(BVAL[aba][round][bval_val] == 0) return;
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeSTATE;
	Send_Data_buff[2] = aba;
	Send_Data_buff[3] = round;
	Send_Data_buff[4] = TypeBVAL;
	Send_Data_buff[5] = bval_val;
	u8 packet_idx = 6;
	//Sign
	uint8_t hash[32] = {0};
	uint8_t sig[64] = {0};
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
	HASH256_hash(&sh256,hash);
	
	sign_data(hash,sig);
	for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];

	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
}

void ABA_AUX_handler(u8 aba, u8 round, u8 aux_val){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	if(!(aux_val ==  0 || aux_val == 1))return;
	if(AUX[aba][round][aux_val] == 0) return;
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeSTATE;
	Send_Data_buff[2] = aba;
	Send_Data_buff[3] = round;
	Send_Data_buff[4] = TypeAUX;
	Send_Data_buff[5] = aux_val;
	u8 packet_idx = 6;
	//Sign
	uint8_t hash[32] = {0};
	uint8_t sig[64] = {0};
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
	HASH256_hash(&sh256,hash);
	
	sign_data(hash,sig);
	for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];

	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
}

void ABA_SHARE_handler(u8 aba, u8 round){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	if(ABA_R[aba] < round)return;	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;

	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeSTATE;
	Send_Data_buff[2] = aba;
	Send_Data_buff[3] = round;
	Send_Data_buff[4] = TypeSHARE;
	if(!ACK_ABA_share[aba][round][IDX]){
		Coin_SK_sign(aba,round);
	}
	u8 packet_idx = 5;
	for(u8 i=0;i<Share_Size;i++)Send_Data_buff[packet_idx + i] = ABA_share[aba][round][IDX][i];
	packet_idx += Share_Size;
	
	//Sign
	uint8_t hash[32] = {0};
	uint8_t sig[64] = {0};
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
	HASH256_hash(&sh256,hash);
	
	sign_data(hash,sig);
	for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];

	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
}


void ABA_NACK_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	
	u8 aba = cur_aba_id;
	if(ABA_decided[aba] != 0xFF)return;
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeNACK;
	u8 round = ABA_R[aba];
	Send_Data_buff[2] = aba;
	Send_Data_buff[3] = round;
	for(u8 i=1;i<=Nodes_N;i++){
		if(ACK_BVAL[aba][round][i][0])Send_Data_buff[4] |= (1<<(i-1));
		if(ACK_BVAL[aba][round][i][1])Send_Data_buff[5] |= (1<<(i-1));
		if(ACK_AUX[aba][round][i][0])Send_Data_buff[6] |= (1<<(i-1));
		if(ACK_AUX[aba][round][i][1])Send_Data_buff[7] |= (1<<(i-1));
	}
	for(u8 i=1;i<=Nodes_N;i++)
		if(ACK_ABA_share[aba][round][i])Send_Data_buff[8] |= (1<<(i-1));
	u8 packet_idx = 9;
	//Sign
	uint8_t hash[32] = {0};
	uint8_t sig[64] = {0};
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<packet_idx;i++) HASH256_process(&sh256,Send_Data_buff[i]);
	HASH256_hash(&sh256,hash);
	
	sign_data(hash,sig);
	for(int i=0;i<Sig_Size;i++)Send_Data_buff[packet_idx+i] = sig[i];

	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
}






void ENC_NACK_handler(){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	u8 cbc_decided[MAX_Nodes];
	for(int i=0;i<MAX_Nodes;i++)cbc_decided[i] = 0;
	for(int i=0;i<CBC_v_SIZE;i++)cbc_decided[CBC_v[cur_aba_id][i]] = 1;
	
	for(int rbc_id =1;rbc_id<=Nodes_N;rbc_id++){
		if(cbc_decided[rbc_id] == 0 || Tx_shares_Number[rbc_id] >= Enc_Thres_k)continue;
		
		Send_Data_buff[0] = IDX;
		Send_Data_buff[1] = TypeTX_ENC_NACK;
		Send_Data_buff[2] = rbc_id;
		u8 packet_idx = 3;
		//share nack
		for(int idx=1;idx<=Nodes_N;idx++){
			if(cbc_decided[rbc_id] == 1 && ACK_Share[rbc_id][idx] == 1)
				Send_Data_buff[packet_idx+idx-1] = 1;
		}
		
		packet_idx += Nodes_N;
		
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

		HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
		
		u32 random = RNG_Get_RandomRange(0, 5);
		random = random * 50;
		delay_ms(random);
	}
}
void ENC_handler(u8 rbc_id){
	u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
	
	//if(ABA_decided[rbc_id] == 0) return;
	u8 cbc_decided[MAX_Nodes];
	for(int i=0;i<MAX_Nodes;i++)cbc_decided[i] = 0;
	for(int i=0;i<CBC_v_SIZE;i++)cbc_decided[CBC_v[cur_aba_id][i]] = 1;
	
	if(cbc_decided[rbc_id]==0)return;
	
	for(int i=0;i<300;i++)Send_Data_buff[i] = 0x00;
	
	Send_Data_buff[0] = IDX;
	Send_Data_buff[1] = TypeTX_ENC;
	Send_Data_buff[2] = rbc_id;
	u8 packet_idx = 3;
	for(int i=0;i<Share_Size;i++){
		Send_Data_buff[packet_idx + i] = Tx_shares_str[rbc_id][IDX-1][i]; 
	}
	packet_idx += Share_Size;
	
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
	
	HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, packet_idx+Sig_Size, 500);
	
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

	if(uart3RxBuffer[0] == 0x00 || uart3RxBuffer[0] > Nodes_N || uart3RxBuffer[1] > 20){
		mmm++;
		return;
	}
	
	nnn++;
	if(Nodes_N > 8)while(1);
	
	//现在是一个一个处理的
	//之后修改为接收多个，
	
	for(int i=0;i<300;i++)buff[i] = uart3RxBuffer[i];
	//验证签名
	u8 len = Packet_len;
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
		u8 packet_idx = 2;
		u8 block_id = buff[packet_idx];packet_idx++;
		
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
	}
	else if(packet_type == TypeRBC_ER && verify_flag){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		if(RBC_done_Number >= 2 * Nodes_f + 1)return;
		
		u8 ER_type = buff[2];
		u8 rbc_id = buff[3];
		if(RBC_Init[rbc_id] == 0) return;
		if(ER_type == Type_ECHO && ACK_RBC_Echo[rbc_id][s_id])return;
		if(ER_type == Type_READY && ACK_RBC_Ready[rbc_id][s_id])return;
		
		u8 hash_flag = 1;
		u8 packet_idx = 4;
		for(int i=0;i<Hash_Size;i++){
			if(buff[packet_idx + i] != hash_v[rbc_id][i]){
				hash_flag = 0;return;
			}
		}
		if(hash_flag){
			if(ER_type == Type_ECHO){
				ACK_RBC_Echo[rbc_id][s_id] = 1;
				RBC_Echo_Number[rbc_id]++;
				if(RBC_Echo_Number[rbc_id] == Nodes_N - Nodes_f && RBC_Ready[rbc_id] == 0){
					RBC_Ready[rbc_id] = 1;
					RBC_Ready_Number[rbc_id]++;
					if(RBC_Ready_Number[rbc_id] == Nodes_N - Nodes_f){
						//accept it
						if(RBC_done[rbc_id] == 0){
							RBC_done[rbc_id] = 1;
							RBC_done_Number++;
							//PRBC sign k
							PRBC_SK_sign(rbc_id);
							ACK_PRBC_share_Number[rbc_id]++;
							if(PRBC_PK_verify_sig_share(IDX,rbc_id,PRBC_share[rbc_id][IDX]) == 0){
								while(1){
									sprintf((char*)Send_Data_buff,"MyMyMy_verify_sig_share: No");
									LoRa_SendData(Send_Data_buff);
								}
							}
							ACK_PRBC_share[rbc_id][IDX] = 1;
							
							if(ACK_PRBC_share_Number[rbc_id] == 2 * Nodes_f + 1){
								//combine
								PRBC_PK_sig_combine_share(rbc_id);
								//verify
								if(PRBC_PK_verify_sig(rbc_id,PRBC_sig[rbc_id]) == 0){
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
			if(ER_type == Type_READY){
				ACK_RBC_Ready[rbc_id][s_id] = 1;
				RBC_Ready_Number[rbc_id]++;
				if(RBC_Ready_Number[rbc_id] >= Nodes_f + 1){
					if(RBC_Echo[rbc_id] == 0){
						RBC_Echo[rbc_id]=1;
						RBC_Echo_Number[rbc_id]++;
						if(RBC_Echo_Number[rbc_id] == Nodes_N - Nodes_f && RBC_Ready[rbc_id] == 0){
							RBC_Ready[rbc_id] = 1;
							RBC_Ready_Number[rbc_id]++;
						}
					}
					if(RBC_Ready[rbc_id] == 0){RBC_Ready[rbc_id]=1;RBC_Ready_Number[rbc_id]++;}
				}
				if(RBC_Ready_Number[rbc_id] == Nodes_N - Nodes_f){
					//accept it
					if(RBC_done[rbc_id] == 0){
						RBC_done[rbc_id] = 1;
						RBC_done_Number++;
						//PRBC sign k
						PRBC_SK_sign(rbc_id);
						ACK_PRBC_share_Number[rbc_id]++;
						if(PRBC_PK_verify_sig_share(IDX,rbc_id,PRBC_share[rbc_id][IDX]) == 0){
							while(1){
								sprintf((char*)Send_Data_buff,"MyMyMy_verify_sig_share: No");
								LoRa_SendData(Send_Data_buff);
							}
						}
						ACK_PRBC_share[rbc_id][IDX] = 1;
						
						if(ACK_PRBC_share_Number[rbc_id] == 2 * Nodes_f + 1){
							//combine
							PRBC_PK_sig_combine_share(rbc_id);
							//verify
							if(PRBC_PK_verify_sig(rbc_id,PRBC_sig[rbc_id]) == 0){
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
	}
	else if(packet_type == TypeRBC_NACK && verify_flag){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 rbc_id = buff[2];
		u8 init_ack = buff[3];
		u8 echo_ack = buff[3+IDX];
		u8 ready_ack = buff[3+Nodes_N+IDX];
		if(rbc_id == IDX && init_ack == 0){
			u32 random = RNG_Get_RandomRange(0, 5);
			random = random * 200;
			delay_ms(random);
			
			for(int i = IDX + 1; i <= Nodes_N; i++){
				if(RBC_Init[i] == 0) return;
			}
			
			if(start_Global_Consensus == 0)
				RBC_INIT_handler();
			else 
				Global_RBC_INIT_handler();
		}
		if(echo_ack == 0 && RBC_Echo[rbc_id]){RBC_ECHO_handler(rbc_id);}
		if(ready_ack == 0 && RBC_Ready[rbc_id]){RBC_READY_handler(rbc_id);}
	}
	
	else if(packet_type == TypePRBC_NACK && verify_flag && RBC_done_Number >= 2 * Nodes_f + 1  && RBC_init_Number == Nodes_N - 1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 prbc_id = buff[2];
		u8 prbc_nack = buff[2+IDX]; 
		if(prbc_nack == 0 && ACK_PRBC_share[prbc_id][IDX])PRBC_SIG_handler(prbc_id);
	}
	else if(packet_type == TypePRBC_SIG && verify_flag && RBC_done_Number >= 2 * Nodes_f + 1  && RBC_init_Number == Nodes_N - 1){
		//DMA send STATE packet
		//只实现了16节点以下的情况。分情况讨论的
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 prbc_id = buff[2];
		u8 packet_idx = 3;
		if(ACK_PRBC_share[prbc_id][s_id] == 0 && ACK_PRBC_share_Number[prbc_id] < 2 * Nodes_f + 1){// && ACK_PRBC_share[k][IDX] == 1){
			for(int i=0;i<Share_Size;i++){
				PRBC_share[prbc_id][s_id][i] = buff[packet_idx + i];
			}
			//verify
			if(PRBC_PK_verify_sig_share(s_id,prbc_id,PRBC_share[prbc_id][s_id]) == 0){
				while(1){
					sprintf((char*)Send_Data_buff,"Verify_sig_share: No");
					LoRa_SendData(Send_Data_buff);
				}
			}
			ACK_PRBC_share[prbc_id][s_id] = 1;
			ACK_PRBC_share_Number[prbc_id]++;
			
			if(ACK_PRBC_share_Number[prbc_id] == 2 * Nodes_f + 1){
				//combine
				PRBC_PK_sig_combine_share(prbc_id);
				//verify
				if(PRBC_PK_verify_sig(prbc_id,PRBC_sig[prbc_id]) == 0){
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
	
	
	
	else if(packet_type == TypeVAL_CBC_NACK && verify_flag && PRBC_done_Number >= 2 * Nodes_f + 1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 val_cbc_id = buff[2];
		u8 init_nack = buff[3];
		u8 echo_nack = buff[4];
		u8 finish_nack = buff[5];
		if(val_cbc_id == IDX && init_nack == 0)VAL_CBC_INIT_handler();
		if(val_cbc_id == s_id && ( ( echo_nack & (1 << (IDX - 1) ) ) == 0 ) )VAL_CBC_ECHO_handler(val_cbc_id);
		if(val_cbc_id == IDX && ( (finish_nack & (1 << (IDX - 1))) == 0 ) )VAL_CBC_FINISH_handler();
	}
	else if(packet_type == TypeVAL_CBC_STATE && verify_flag && PRBC_done_Number >= 2 * Nodes_f + 1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 val_cbc_type = buff[2];
		if(val_cbc_type == TypeVAL_CBC_INIT){
			u8 val_ack = buff[3];
			u8 packet_idx = 4;
			for(int k=1;k<=Nodes_N;k++){
				if((val_ack & (1<<(k-1))) != 0 && ACK_VAL_CBC[s_id][k] == 0){
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
				if((val_ack & (1<<(k-1))) != 0){
					packet_idx += Share_Size;
				}
			}
		}
		else if(val_cbc_type == TypeVAL_CBC_ECHO){
			u8 val_cbc_id = buff[3];
			if(val_cbc_id != IDX)return;
			u8 packet_idx = 4;
			for(int i=0;i<Share_Size;i++){
				My_VAL_CBC_Echo_Share[s_id][i] = buff[packet_idx + i];
			}
			
			//验证share, verify
			if(VAL_CBC_PK_verify_sig_share(s_id) == 0){
				while(1){
					sprintf((char*)Send_Data_buff,"verify share Thres Fail");
					Lora_Device_Sta = LORA_TX_STA;
					LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
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
							LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
						}
					}
					ACK_VAL_CBC_Finish[IDX] = 1;
					if(VAL_CBC_done_Number <= 2 * Nodes_f){
						CBC_v[IDX][VAL_CBC_done_Number] = IDX;
						//ABA
						est[IDX][1] = 1;
						BVAL[IDX][1][1] = 1;
						ACK_BVAL_Number[IDX][1][1] = 1;
					}
					VAL_CBC_done_Number++;
					if(VAL_CBC_done_Number == 2 * Nodes_f + 1){
						CBC_init();
						for(int q=0;q<=MAX_ABA;q++){
							if(est[q][1] == 0xFF){
								est[q][1] = 0;
								BVAL[q][1][0] = 1;
								ACK_BVAL_Number[q][1][0] = 1;
							}
						}
					}
				}
			}
		}
		else if(val_cbc_type == TypeVAL_CBC_FINISH && ACK_VAL_CBC_Number[s_id] == Nodes_N - Nodes_f){
			if(ACK_VAL_CBC_Finish[s_id] == 1) return;
			u8 packet_idx = 3;
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

			if(VAL_CBC_done_Number <= 2 * Nodes_f){
				CBC_v[IDX][VAL_CBC_done_Number] = s_id;
				//ABA
				est[s_id][1] = 1;
				BVAL[s_id][1][1] = 1;
				ACK_BVAL_Number[s_id][1][1] = 1;
			}
			
			VAL_CBC_done_Number++;
			if(VAL_CBC_done_Number == 2 * Nodes_f + 1){
				CBC_init();
				for(int q=0;q<=Nodes_N;q++){
					if(est[q][1]==0xFF){
						est[q][1] = 0;
						BVAL[q][1][0] = 1;
						ACK_BVAL_Number[q][1][0] = 1;
					}
				}
			}
		}
	}
	else if(packet_type == TypeCBC_NACK && verify_flag && VAL_CBC_done_Number >= 2 * Nodes_f + 1 ){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 cbc_id = buff[2];
		u8 init_nack = buff[3];
		u8 echo_nack = buff[4];
		u8 finish_nack = buff[5];
		
		if(cbc_id == IDX && init_nack == 0)CBC_INIT_handler();
		if(cbc_id == s_id && ( ( echo_nack & (1 << (IDX - 1) ) ) == 0 ) )CBC_ECHO_handler(cbc_id);
		if(cbc_id == IDX && ( (finish_nack & (1 << (IDX - 1))) == 0 ) )CBC_FINISH_handler();
	}
	else if(packet_type == TypeCBC_STATE && verify_flag && VAL_CBC_done_Number >= 2 * Nodes_f + 1 ){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 cbc_type = buff[2];
		
		if(cbc_type == TypeCBC_INIT){
			u8 packet_idx = 3;
			if(CBC_Echo[s_id] == 0){
				for(int i=0;i<CBC_v_SIZE;i++){
					CBC_v[s_id][i] = buff[packet_idx + i];
				}
				//threshold signature
				CBC_SK_sign(s_id,CBC_v[s_id],CBC_v_SIZE);
				CBC_Echo[s_id] = 1;
				CBC_v_number++;
			}
		}
		else if(cbc_type == TypeCBC_ECHO){
			u8 cbc_id = buff[3];
			u8 packet_idx = 4;
			if(cbc_id == IDX && ACK_CBC_Echo[s_id] == 0){
				for(int i=0;i<Share_Size;i++){
					My_CBC_Echo_Share[s_id][i] = buff[packet_idx + i];
				}
				
				//验证share, verify
				if(CBC_PK_verify_sig_share(s_id, CBC_v[IDX], CBC_v_SIZE, My_CBC_Echo_Share[s_id]) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"verify share Thres Fail");
						Lora_Device_Sta = LORA_TX_STA;
						LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
					}
				}
				ACK_CBC_Echo[s_id] = 1;
				My_CBC_Echo_Number++;
			}
		}
		else if(cbc_type == TypeCBC_FINISH){
			u8 packet_idx = 3;
			if(ACK_CBC_Finish[s_id] == 0){
				for(int i=0;i<Share_Size;i++){
					CBC_Finish_Share[s_id][i] = buff[packet_idx + i];
				}
				//验证share, verify
				if(CBC_verify_sig(CBC_v[s_id], CBC_v_SIZE, CBC_Finish_Share[s_id]) == 0){
					while(1){
						sprintf((char*)Send_Data_buff,"verify Thres Fail");
						Lora_Device_Sta = LORA_TX_STA;
						LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
					}
				}
				//verify share
				ACK_CBC_Finish[s_id] = 1;
				CBC_done_Number++;
			}
		}
	}
	else if(packet_type == TypeTX_ENC_NACK && verify_flag && ABA_decided[cur_aba_id] == 0x01){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 rbc_id = buff[2];
		u8 enc_nack = buff[2+IDX];
		u8 cbc_decided[MAX_Nodes];
		for(int i=0;i<MAX_Nodes;i++)cbc_decided[i] = 0;
		for(int i=0;i<CBC_v_SIZE;i++)cbc_decided[CBC_v[cur_aba_id][i]] = 1;
//		if(cbc_decided[rbc_id]==0)return;
		
		if(enc_nack == 0 && cbc_decided[rbc_id] == 1)ENC_handler(rbc_id);		
	}
	else if(packet_type == TypeTX_ENC && verify_flag && ABA_decided[cur_aba_id] == 0x01){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 rbc_id = buff[2];
		if(ABA_decided[rbc_id] == 0 || Tx_shares_Number[rbc_id] >= Enc_Thres_k) return;
		u8 packet_idx = 3;
		for(int i=0;i<Share_Size;i++){
			Tx_shares_str[rbc_id][s_id - 1][i] = buff[packet_idx+i];
		}
		
		octet tmp_o;
		tmp_o.len = tmp_o.max = Share_Size;
		tmp_o.val = (char*)Tx_shares_str[rbc_id][s_id-1];
		ECP_BN158_fromOctet(&Tx_shares[rbc_id][s_id-1],&tmp_o);
		if(!start_Global_Consensus && Tx_PK_verify_share(rbc_id, s_id)){
			ACK_Share[rbc_id][s_id] = 1;
			Tx_shares_Number[rbc_id]++ ;
			if(Tx_shares_Number[rbc_id] >= Enc_Thres_k){
				//combine
				Tx_PK_combine_share(rbc_id,enc_tx_passwd[rbc_id],dec_tx_passwd[rbc_id]);
				u8 tmp_enc_tx[MAX_PROPOSAL];
				for(int i=0;i<MAX_PROPOSAL;i++)tmp_enc_tx[i] = 0x00;
				u32 proposal_packet_id = 0;
				for(int i=Share_Size+PD_Size;i<Block_each_Size[rbc_id][0];i++){
					tmp_enc_tx[proposal_packet_id] = Block[rbc_id][0][i];
					proposal_packet_id++;
				}
				
				for(int b=1;b<Block_Num;b++){
					for(int i=0;i<Block_each_Size[rbc_id][b];i++){
						tmp_enc_tx[proposal_packet_id] = Block[rbc_id][b][i];
						proposal_packet_id++;
					}
				}
				
				Tx_decrypt(dec_tx_passwd[rbc_id], tmp_enc_tx, proposal_packet_id, Transaction[rbc_id]);
				if(dec_tx_flag[rbc_id] == 0){
					dec_tx_flag[rbc_id] = 1;
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
		else if(start_Global_Consensus && Global_Tx_PK_verify_share(rbc_id, s_id)){
			ACK_Share[rbc_id][s_id] = 1;
			Tx_shares_Number[rbc_id]++ ;
			if(Tx_shares_Number[rbc_id] >= Enc_Thres_k){
				//combine
				Global_Tx_PK_combine_share(rbc_id,enc_tx_passwd[rbc_id],dec_tx_passwd[rbc_id]);
				u8 tmp_enc_tx[Global_MAX_PROPOSAL];
				for(int i=0;i<Global_MAX_PROPOSAL;i++)tmp_enc_tx[i] = 0x00;
				u32 proposal_packet_id = 0;
				for(int i=Share_Size+PD_Size;i < Global_Block_each_Size[rbc_id][1];i++){
					tmp_enc_tx[proposal_packet_id] = Global_Block[rbc_id][1][i];
					proposal_packet_id++;
				}
					
				for(int b=2;b<Global_Group_Block_Num[rbc_id];b++){
					for(int i=0;i<Global_Block_each_Size[rbc_id][b];i++){
						tmp_enc_tx[proposal_packet_id] = Global_Block[rbc_id][b][i];
						proposal_packet_id++;
					}
				}
				
				Tx_decrypt(dec_tx_passwd[rbc_id], tmp_enc_tx, proposal_packet_id, Global_Transaction[rbc_id]);
				
				Global_Proposal_Size[rbc_id] = proposal_packet_id - 16;
				
				if(dec_tx_flag[rbc_id] == 0){
					dec_tx_flag[rbc_id] = 1;
					dec_tx_number++;
					if(Group_ID == 1 && ID == 1 && dec_tx_number == dec_done_number && MY_Consensus_TIME == 0){
						LoRa_CFG.chn = DATA_channel;
						LoRa_Set();	
						u32 tmp_size = 0;
						for(int i=1;i<=Nodes_N;i++){
							if(dec_tx_flag[i])tmp_size += Global_Proposal_Size[i];
						}
						MY_Consensus_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
						sprintf((char*)Send_Data_buff,"Dumbo-M-Multi-hop-%d-%d",MY_Consensus_TIME,tmp_size);
						LoRa_SendData(Send_Data_buff);
						while(1);
	//								LoRa_CFG.chn = 0;
	//								LoRa_Set();	
					}
				}
			}					
		}
//		if(Tx_PK_verify_share(rbc_id, s_id)){
//			ACK_Share[rbc_id][s_id] = 1;
//			Tx_shares_Number[rbc_id]++ ;
//			if(Tx_shares_Number[rbc_id] >= Enc_Thres_k){
//				//combine
//				Tx_PK_combine_share(rbc_id,enc_tx_passwd[rbc_id],dec_tx_passwd[rbc_id]);
//				u8 tmp_enc_tx[MAX_PROPOSAL];
//				for(int i=0;i<MAX_PROPOSAL;i++)tmp_enc_tx[i] = 0x00;
//				u32 proposal_packet_id = 0;
//				for(int i=Share_Size+PD_Size;i<Block_each_Size[rbc_id][0];i++){
//					tmp_enc_tx[proposal_packet_id] = Block[rbc_id][0][i];
//					proposal_packet_id++;
//				}
//				
//				for(int b=1;b<Block_Num;b++){
//					for(int i=0;i<Block_each_Size[rbc_id][b];i++){
//						tmp_enc_tx[proposal_packet_id] = Block[rbc_id][b][i];
//						proposal_packet_id++;
//					}
//				}
//				
//				Tx_decrypt(dec_tx_passwd[rbc_id], tmp_enc_tx, proposal_packet_id, Transaction[rbc_id]);
//				if(dec_tx_flag[rbc_id] == 0){
//					dec_tx_flag[rbc_id] = 1;
//					dec_tx_number++;
//					
//					if(IDX == 1 && dec_tx_number == dec_done_number && MY_Consensus_TIME==0){
//						LoRa_CFG.chn = DATA_channel;
//						LoRa_Set();	
//						
//						MY_Consensus_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
//						sprintf((char*)Send_Data_buff,"MMM-Dumbo-SC-%d-%d-%d",Block_Num,MY_Consensus_TIME,dec_tx_number * Proposal_Size);
//						LoRa_SendData(Send_Data_buff);
//						
//						LoRa_CFG.chn = DATA_channel;
//						LoRa_Set();	
//						delay_ms(500);
//						u32 send_size = strlen(Send_Data_buff);
//						HAL_UART_Transmit(&uart3_handler, (uint8_t *)Send_Data_buff, send_size, 500);
//						while(1);
//					}

//				}
//			}					
//		}

	}
	
	else if(packet_type == TypeNACK && verify_flag  && CBC_done_Number >= 2 * Nodes_f + 1 && CBC_v_number == Nodes_N - 1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;

		u8 aba = buff[2];
		u8 round = buff[3];
		u8 bval0 = buff[4];
		u8 bval1 = buff[5];
		u8 aux0 = buff[6];
		u8 aux1 = buff[7];
		u8 share_ack = buff[8];

		if(round > ABA_R[aba])return;
		if(cur_aba_id < aba) return;
		
		if(((bval0 & (1<<(IDX-1)))==0) && (BVAL[aba][round][0]))ABA_BVAL_handler(aba,round,0);
		if(((bval1 & (1<<(IDX-1)))==0) && (BVAL[aba][round][1]))ABA_BVAL_handler(aba,round,1);
		if(((aux0 & (1<<(IDX-1)))==0) && (AUX[aba][round][0]))ABA_AUX_handler(aba,round,0);
		if(((aux1 & (1<<(IDX-1)))==0) && (AUX[aba][round][1]))ABA_AUX_handler(aba,round,1);
		if((share_ack & (1<<(IDX-1)))==0)ABA_SHARE_handler(aba, round);
	}
	else if(packet_type == TypeSTATE && verify_flag && CBC_done_Number >= 2 * Nodes_f + 1 && CBC_v_number == Nodes_N - 1){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 aba = buff[2];
		u8 round = buff[3];
		if(ABA_R[aba] > round)return;
		if(cur_aba_id < aba) return;
		u8 aba_type = buff[4];
		
		if(aba_type == TypeBVAL){
			u8 bval_val = buff[5];
			if(!ACK_BVAL[aba][round][s_id][bval_val]){ACK_BVAL[aba][round][s_id][bval_val] = 1;ACK_BVAL_Number[aba][round][bval_val]++;}
		}
		if(aba_type == TypeAUX){
			u8 aux_val = buff[5];
			if(!ACK_AUX[aba][round][s_id][aux_val]){ACK_AUX[aba][round][s_id][aux_val] = 1;ACK_AUX_Number[aba][round][aux_val]++;}
		}
		if(aba_type == TypeSHARE){
			u32 packet_idx = 5;
			if(!ACK_ABA_share[aba][round][s_id]){
				//验证share 复杂 耗时大
				u8 share[70];
				for(int i=0;i<Share_Size;i++)share[i] = 0x00;
				for(int i=0;i<Share_Size;i++)share[i] = buff[packet_idx+i];
				if(Coin_PK_verify_sig_share(s_id,aba,round,share)){
					for(int i=0;i<Share_Size;i++)ABA_share[aba][round][s_id][i] = buff[packet_idx+i];
					ABA_share_Number[aba][round]++;
					ACK_ABA_share[aba][round][s_id] = 1;
					if(ABA_share_Number[aba][round] >= 2 * Nodes_f + 1){
						//收到f+1个coin，进行合成
						if(Shared_Coin[aba][round]==0xFF)
							Coin_PK_sig_combine_share(aba,round);//内部会验证
					}
				}else{
					while(1){
						sprintf((char*)Send_Data_buff,"Node %d verify sig error",s_id);
						LoRa_SendData(Send_Data_buff);
					}
				}
			}
		}

		//收到f+1个BVAL(b)
		if(ACK_BVAL_Number[aba][round][0] >= Nodes_f + 1 && !BVAL[aba][round][0]){ACK_BVAL_Number[aba][round][0]++;BVAL[aba][round][0] = 1;}
		if(ACK_BVAL_Number[aba][round][1] >= Nodes_f + 1 && !BVAL[aba][round][1]){ACK_BVAL_Number[aba][round][1]++;BVAL[aba][round][1] = 1;}
		//收到2f+1个BVAL(b)
		if(ACK_BVAL_Number[aba][round][0] >= 2 * Nodes_f + 1){bin_values[aba][round][0] = 1;}
		if(ACK_BVAL_Number[aba][round][1] >= 2 * Nodes_f + 1){bin_values[aba][round][1] = 1;}
		
		//bin_values_R非空  自己发送AUX，记录number
		if(bin_values[aba][round][0] && !AUX[aba][round][0]){ACK_AUX_Number[aba][round][0]++;AUX[aba][round][0] = 1;}
		if(bin_values[aba][round][1] && !AUX[aba][round][1]){ACK_AUX_Number[aba][round][1]++;AUX[aba][round][1] = 1;}
		
		//收到N-f个AUX_r(b)   广播BVAL内
		if(ACK_AUX_Number[aba][round][0] >= (Nodes_N - Nodes_f) && !BVAL[aba][round][0]){ACK_BVAL_Number[aba][round][0]++;BVAL[aba][round][0] = 1;}
		if(ACK_AUX_Number[aba][round][1] >= (Nodes_N - Nodes_f) && !BVAL[aba][round][1]){ACK_BVAL_Number[aba][round][1]++;BVAL[aba][round][1] = 1;}
		//收到N-f个AUX_r(b)   收集到vals内	
		if(ACK_AUX_Number[aba][round][0] >= (Nodes_N - Nodes_f))vals[aba][round][0] = 1;
		if(ACK_AUX_Number[aba][round][1] >= (Nodes_N - Nodes_f))vals[aba][round][1] = 1;
		
		if(ACK_AUX_Number[aba][round][0] > 0)vals[aba][round][0] = 1;
		if(ACK_AUX_Number[aba][round][1] > 0)vals[aba][round][1] = 1;	
		
		u8 aux_flag = 0;
		if(ACK_AUX_Number[aba][round][0] + ACK_AUX_Number[aba][round][1] >= (Nodes_N - Nodes_f))aux_flag = 1;
			
		//vals是bin_values的子集
		if(aux_flag && subset_or_not(aba,round) && ABA_R[aba] == round){
			//shared coin
			//到这里一定会获得了N-f的share，因为是绑定到一起的
			//获取round的coin
			//暂定为1，需要合成
			u8 s = 0;
			if(ABA_share_Number[aba][round] < Nodes_f + 1){
				return;
				while(1){
					sprintf((char*)Send_Data_buff,"Node %d error combine coin",IDX);
					LoRa_SendData(Send_Data_buff);
				}
			}
			
			if(Shared_Coin[aba][round] == 0xFF)
				Coin_PK_sig_combine_share(aba,round);
			s = Shared_Coin[aba][round];

			if(vals[aba][round][0] == 1 && vals[aba][round][1] == 0){
				est[aba][round+1] = 0;
				BVAL[aba][round+1][0] = 1;
				ACK_BVAL_Number[aba][round+1][0]++;
				ABA_round_time[aba][round] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
				ABA_R[aba]++;
				if(s == 0){
					//ABA j done output 0
					if(ABA_decided[aba] == 0xFF){
						ABA_decided[aba] = 0;
						if(cur_aba_id == aba)cur_aba_id++;
						
//						ABA_done_Number++;
//						ABA_time[aba] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
//						ABA_round[aba] = round;
					}
					else if (ABA_decided[aba] == 1){
						//error
						LED0(1);LED1(1);LED2(1);
						while(1);
					}	
				}
			}
			if(vals[aba][round][0] == 0 && vals[aba][round][1] == 1){
				est[aba][round+1] = 1;
				BVAL[aba][round+1][1] = 1;
				ACK_BVAL_Number[aba][round+1][1]++;
				ABA_R[aba]++;
				ABA_round_time[aba][round] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
				
				if(s == 1){
					//ABA j done output 1
//					if(ABA_decided[aba] == 0xFF){
//						ABA_decided[aba] = 1;
//						ABA_time[aba] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
//						ABA_round[aba] = round;
//						ABA_done_Number++;
//						dec_done_number++;
//						//decrypt the tx
//						Tx_SK_decrypt_share(aba, ID);
//						if(ACK_Share[aba][ID] == 0){
//							ACK_Share[aba][ID] = 1;
//							Tx_shares_Number[aba]++;
//						}
//					}
					if(ABA_decided[aba] == 0xFF){
						ABA_decided[aba] = 1;
						//ABA_time[aba_id] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
						ABA_round[aba] = round;
						MY_TIME = TIM2->CNT + 9000 * TIM2_Exceed_Times;
						
						//thres_enc
						if(!start_Global_Consensus){
							//single_hop
							for(int m=0;m<CBC_v_SIZE;m++){
								dec_done_number++;
								//decrypt the tx
								u8 dec_id = CBC_v[aba][m];
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
								u8 dec_id = CBC_v[aba][m];
								Global_Tx_SK_decrypt_share(dec_id, IDX);
								if(ACK_Share[dec_id][IDX] == 0){
									ACK_Share[dec_id][IDX] = 1;
									Tx_shares_Number[dec_id]++;
								}
							}
						}
						
					}
					else if (ABA_decided[aba] == 0){
						//error
						LED0(1);LED1(1);LED2(1);
						while(1);
					}
				}
			}
			if(vals[aba][round][0] == 1 && vals[aba][round][1] == 1){
				est[aba][round+1] = s;
				BVAL[aba][round+1][s] = 1;
				ABA_round_time[aba][round] = TIM2->CNT + 9000 * TIM2_Exceed_Times;
				ABA_R[aba]++;
			}
		}
	}
	
	
	
	
	
	
	


	else if(packet_type == Global_TypeRBC_INIT && verify_flag){
		u8 IDX;if(start_Global_Consensus)IDX = Group_ID;else IDX = ID;
		u8 packet_idx = 2;
		u8 block_tot = buff[packet_idx];packet_idx++;
		u8 block_id = buff[packet_idx];packet_idx++;
//		u8 init_ACK = buff[packet_idx];
		Global_Group_Block_Num[s_id] = block_tot;
		
//		if((init_ACK & (1<<(IDX-1)))==0){
//			reply_flag = 1;
//		}
//		packet_idx++;
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
	}

	
}






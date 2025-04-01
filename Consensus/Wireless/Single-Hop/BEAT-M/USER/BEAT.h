#ifndef BEAT_H
#define BEAT_H

#include "sys.h"
#include "stm32f7xx_hal.h"
#include "stm32f7xx.h"
#include "delay.h"
#include "FreeRTOS.h"
#include "task.h"
#include "dma.h"
#define Nodes_N 4
#define Nodes_f 1

#define EVENTBIT_0	(1<<0)				//事件位
#define EVENTBIT_1	(1<<1)
#define EVENTBIT_2	(1<<2)
#define EVENT_ABA_NACK	(1<<3)
#define EVENT_PACKET (1<<4)
#define EVENT_RBC_NACK (1<<5)
#define EVENT_ENC_NACK (1<<6)
#define EVENT_ENC (1<<7)
extern u8 ReceBuff[RECE_BUF_SIZE];
extern DMA_HandleTypeDef  UART3RxDMA_Handler,UART3TxDMA_Handler;
extern TaskHandle_t EventGroupTask_Handler;
extern u32 TIM5_Exceed_Times;
extern u32 TIM9_Exceed_Times;
extern u8 Send_Data_buff[800];//发送数据缓冲区

#include "Thres_Prf.h"
#include "Thres_Enc.h"

#define DMA_buff_Size 240


#define ID 1
#define Block_Num 1
#define Proposal_Size 128
#define chat_channel 40
#define DATA_channel 44
#define cycle_time 500

extern u32 Packet_len;



#define DEBUG 1
#define Sig_Size 40
#define MAX_round 20
#define MAX_Nodes 10 
#define MAX_ABA 10
#define Share_Size 21
#define Hash_Size 32
#define Block_Size 193

#define MAX_PROPOSAL 900 

#define PD_Size 16

#define BIG_Size 20
#define Verify_Share_Size BIG_Size*2//z + c
#define Packet_Share_Size Share_Size+Verify_Share_Size


/*******************ABA_shared**********************/
extern u8 ABA_done_Number;
extern u8 start_ABA;
extern u32 TIM2_Exceed_Times;

extern u8 ABA_share[MAX_Nodes][MAX_round][MAX_Nodes][Share_Size];//第x轮的哪个节点的share
extern u8 ACK_ABA_share[MAX_Nodes][MAX_round][MAX_Nodes];//第x轮的哪个节点的share收到了
extern u8 ABA_share_Number[MAX_Nodes][MAX_round];//第x轮share数量
extern u8 Shared_Coin[MAX_Nodes][MAX_round];//第x轮的coin


//发送几个Round 的STATE。
extern u8 STATE_Set[MAX_Nodes];
extern u8 STATE_Set_Number;
//发送几个Round 的NACK。
extern u8 NACK_Set[MAX_Nodes];
extern u8 NACK_Set_Number;
extern u8 ABA_R[MAX_ABA];
extern u8 ABA_round[MAX_ABA];
extern u32 ABA_time[MAX_ABA];
extern u8 COIN[Share_Size];

/********************RBC*******************/
extern u8 Block[MAX_Nodes][Block_Num][Block_Size];
extern u8 RBC_done_Number;
extern u8 RBC_init_Number;

/********************Threshole Encryption*************************************/
extern u8 Transaction[MAX_Nodes][Proposal_Size];
extern u8 ACK_Share[MAX_Nodes][MAX_Nodes];
extern u8 Tx_Share[MAX_Nodes][MAX_Nodes][Share_Size];
extern ECP_BN158 Tx_U[MAX_Nodes];
extern ECP2_BN158 Tx_W[MAX_Nodes],Tx_H[MAX_Nodes];
extern ECP_BN158 Tx_shares[MAX_Nodes][MAX_SK];
extern u8 Tx_U_str[MAX_Nodes][Share_Size];
extern u8 Tx_shares_str[MAX_Nodes][MAX_SK][Share_Size];
extern u8 dec_tx_flag[MAX_Nodes];
extern u8 dec_tx_number;
extern u8 dec_done_number;
/****************************************************************/






extern BIG_160_28 ABA_Z[MAX_Nodes][MAX_round][MAX_Nodes];
extern BIG_160_28 ABA_C[MAX_Nodes][MAX_round][MAX_Nodes];

extern u8 ABA_Z_share[MAX_Nodes][MAX_round][MAX_Nodes][32];
extern u8 ABA_C_share[MAX_Nodes][MAX_round][MAX_Nodes][32];





























#endif /* BEAT_H */
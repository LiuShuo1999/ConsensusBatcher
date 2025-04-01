#ifndef RBC_H
#define RBC_H

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
#define EVENT_STATE	(1<<3)
#define EVENT_NACK	(1<<4)
#define EVENT_PACKET (1<<5)
#define EVENT_RBC_INIT (1<<6)
#define EVENT_RBC_ER (1<<7)
#define EVENT_PRBC_SIG (1<<8)
#define EVENT_CBC_STATE (1<<9)
#define EVENT_VAL_CBC_STATE (1<<10)
#define EVENT_ENC (1<<11)

extern u8 ReceBuff[RECE_BUF_SIZE];
extern DMA_HandleTypeDef  UART3RxDMA_Handler,UART3TxDMA_Handler;
extern TaskHandle_t EventGroupTask_Handler;
extern u32 TIM5_Exceed_Times;
extern u8 Send_Data_buff[800];//发送数据缓冲区

#include "Thres_Sig.h"
#include "Thres_Enc.h"
#define DMA_buff_Size 240

#define DEBUG 0
#define ID 4
#define Block_Num 1

#define Sig_Size 40

#define MAX_round 9
#define MAX_Nodes 10
#define MAX_ABA 10
#define Share_Size 21
#define Hash_Size 32
#define Block_Size 193

#define Local_ABA_phase_Number 4

#define MAX_PROPOSAL 900
#define Proposal_Size 704

#define PD_Size 16

extern u32 TIM9_Exceed_Times;
extern u32 TIM2_Exceed_Times;
/****************ABA_Shared***************************************/
extern u8 cur_aba_id;
extern u8 state_aba;
extern u8 ABA_share[MAX_ABA][MAX_round][MAX_Nodes][Share_Size];//第x轮的哪个节点的share
extern u8 ACK_ABA_share[MAX_ABA][MAX_round][MAX_Nodes];//第x轮的哪个节点的share收到了
extern u8 ABA_share_Number[MAX_ABA][MAX_round];//第x轮share数量
extern u8 Shared_Coin[MAX_ABA][MAX_round];//第x轮的coin

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
extern u8 ABA_decided[MAX_ABA];
/********************RBC*******************/
extern u8 RBC_done_Number;

/******************PRBC*******************/
extern u8 PRBC_done_Number;
extern u8 ACK_PRBC_share_Number[MAX_Nodes];
extern u8 ACK_PRBC_share[MAX_Nodes][MAX_Nodes];
extern u8 PRBC_share[MAX_Nodes][MAX_Nodes][Share_Size];//第几个RBC，第几个节点的share
extern u8 PRBC_sig[MAX_Nodes][Share_Size];

/********************CBC*******************/
#define CBC_v_SIZE 2 * Nodes_f + 1

extern u8 CBC_v[MAX_Nodes][CBC_v_SIZE];
extern u8 ACK_CBC_Echo[MAX_Nodes];// 自己的v的Echo
extern u8 CBC_Echo[MAX_Nodes];	//对其他节点的Echo
extern u8 CBC_Echo_Share[MAX_Nodes][Share_Size];
extern u8 My_CBC_Echo_Share[MAX_Nodes][Share_Size];
extern u8 My_CBC_Echo_Number;

extern u8 ACK_CBC_Finish[MAX_Nodes];
extern u8 CBC_Finish_Share[MAX_Nodes][Share_Size];

extern u8 CBC_done_Number;
/*******************VAL_CBC**************************************/
extern u8 VAL_CBC_v[MAX_Nodes][MAX_Nodes][Share_Size];
extern u8 ACK_VAL_CBC[MAX_Nodes][MAX_Nodes];
extern u8 ACK_VAL_CBC_Number[MAX_Nodes];

extern u8 ACK_VAL_CBC_Echo[MAX_Nodes];// 自己的v的Echo
extern u8 VAL_CBC_Echo[MAX_Nodes];	//对其他节点的Echo
extern u8 VAL_CBC_Echo_Share[MAX_Nodes][Share_Size];
extern u8 My_VAL_CBC_Echo_Share[MAX_Nodes][Share_Size];
extern u8 My_VAL_CBC_Echo_Number;

extern u8 ACK_VAL_CBC_Finish[MAX_Nodes];
extern u8 VAL_CBC_Finish_Share[MAX_Nodes][Share_Size];
extern u8 VAL_CBC_done_Number;

//extern u8 W_flag;
extern u8 W_arr[2];

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
extern u8 RBC_init_Number;
extern u8 CBC_v_number;
/****************************************************************/






#endif /* RBC_H */
#ifndef _PBFT_H_
#define _PBFT_H_

#include "sys.h"
#include "uECC.h"
#include "sha2.h"

//#define N 4
//#define f 1

extern u8 N;
//extern u8 f;
extern u16 DONE_THRES;

#define PROPOSAL 1
#define CHOICE 2
#define COMMIT 3
#define ACK 4
#define VIEW_CHANGE 5
#define NEXT_ROUND 6
#define DECIDE 7
#define NACK 8

#define Sig_Size 48
#define MAX_PROPOSAL_PACKETS 30
#define SINGLE_COMMAND_SIZE 32  //单位byte，目前是用hash代替的command

#define Data_channel 0
#define Msg_channel 27

extern u8 Commands_Size;
extern u8 Proposal_Packet_Number;// = Command_Size / 4 上取整
extern u8 Rece_Proposal[MAX_PROPOSAL_PACKETS];
extern u8 Rece_Proposal_flag;
extern u8 Rece_Proposal_Number;
extern u8 Command_buff[800];
extern u8 Proposal_hash[40];
extern u8 start_flag;
extern u8 Timer_Flag_5;
extern u8 before_round;
extern u8 after_round;
extern u8 TIM_slot;
extern u32 TIM9_Exceed_Times;
extern u32 TIM10_Exceed_Times;
extern u32 Total_time;
extern u32 Total_time1;

extern u8 Need_Proposal_Flag;
extern u8 Need_Choice_Flag;
extern u8 Need_Commit_Flag;
extern u8 Need_VIEWCHANGE;
extern u8 Need_next;


extern u16 slot;

extern u8 Need_to_Wait;
///viewchange
extern u8 Rece_Commit[20];
extern u8 Commit_id[20];
extern u8 Commit_Sig[20][64];

extern u8 Rece_Viewchange[20];
extern u8 Viewchange_id[20];
extern u8 Viewchange_Sig[20][64];

extern u8 Commit_Number;
extern u8 Viewchange_Number;
extern u8 Rece_next;
extern u8 ACK_viewchange;
////////


extern u8 id;	//id = 0 => leader           1-2-3 => follower 
extern u8 PBFT_status;
extern u8 Choice_Truth_Set_Number;
extern u8 Choice_False_Set_Number;

extern u8 Commit_Truth_Set_Number;
extern u8 Commit_False_Set_Number;

extern u8 Choice[13+1];
extern u8 Commit[13+1];

extern u8 Send_Data_buff[800];//发送数据缓冲区


////////ACK
extern u8 ACK_Proposal[13+1];
extern u8 ACK_Choice[13+1];
extern u8 ACK_Commit[13+1];

extern u8 ACK_Proposal_Number;
extern u8 ACK_Choice_Number;
extern u8 ACK_Commit_Number;

extern u8 ACK_Proposal_Flag;
extern u8 ACK_Choice_Flag;
extern u8 ACK_Commit_Flag;

extern u8 ACK_TO_Send_Number;
extern u8 ACK_TO_Send[100][30];

//crypto
extern uint8_t public_key[13+1][64];
extern uint8_t private_key[13+1][32];
extern const struct uECC_Curve_t * curves[5];

//
extern u32 TIM2_Exceed_Times;
extern u32 TIM5_Exceed_Times;
extern u8 decided;



void tendermint2();

//void testdemo(){
//////	//test demo
//////	//pair1(g2*secret*r,g1) == pair2(g2,g1*secret*r)
//////	//R_num
//////	//SECRET
//////	BIG_256_28 Secret_num;
//////	FP_BN254CX_redc(Secret_num,&SECRET);
//////	//pair(g2*secret*r,g1)
//////	ECP2_BN254CX_copy(&g12,&g2);
//////	PAIR_BN254CX_G2mul(&g12,R_num);
//////	PAIR_BN254CX_G2mul(&g12,Secret_num);
//////	PAIR_BN254CX_ate(&pair1,&g12,&g1);
//////	PAIR_BN254CX_fexp(&pair1);
//////	
//////	//pair(g2,g1*secret*r)
//////	ECP_BN254CX_copy(&g21,&g1);
//////	PAIR_BN254CX_G1mul(&g21,R_num);
//////	PAIR_BN254CX_G1mul(&g21,Secret_num);
//////	PAIR_BN254CX_ate(&pair2,&g2,&g21);
//////	PAIR_BN254CX_fexp(&pair2);
//////	
//////	if(FP12_BN254CX_equals(&pair2,&pair1)){
//////		sprintf((char*)Send_Data_buff,"Test g12 == g21: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test g12 == g21: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	octet pair1_r_oct;				//VKr->oct
//////	char pair1_r_ch[400];for(int i=0;i<400;i++)pair1_r_ch[i]=NULL;
//////	pair1_r_oct.val = pair1_r_ch;
//////	FP12_BN254CX_toOctet(&pair1_r_oct,&pair1);
//////	
//////	hash256 sh256;//hash VKr_oct  
//////	HASH256_init(&sh256);
//////	char digest1[50];for(int i=0;i<50;i++)digest1[i]=NULL;
//////	for (int i=0;pair1_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair1_r_oct.val[i]);
//////	HASH256_hash(&sh256,digest1);
//////	
//////	
//////	octet pair2_r_oct;				//VKr->oct
//////	char pair2_r_ch[400];for(int i=0;i<400;i++)pair2_r_ch[i]=NULL;
//////	pair2_r_oct.val = pair2_r_ch;
//////	FP12_BN254CX_toOctet(&pair2_r_oct,&pair2);
//////	
//////	//hash256 sh256;//hash VKr_oct  
//////	HASH256_init(&sh256);
//////	char digest2[50];for(int i=0;i<50;i++)digest2[i]=NULL;
//////	for (int i=0;pair2_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair2_r_oct.val[i]);
//////	HASH256_hash(&sh256,digest2);
//////	
//////	int flag = 1;
//////	for(int i=0;i<50;i++)
//////		if(digest1[i]!=digest2[i])
//////			flag = 0;
//////	
//////	if(flag){
//////		sprintf((char*)Send_Data_buff,"Test demo 2: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test demo 2: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}	
//////	
///////*********************TEST**************************/	
//////	//RES = g1*r*ff[1]*lag[0]+..+g1*r*ff[k]*lag[k-1]
//////	
//////	ECP_BN254CX tmp;
//////	FP_BN254CX ff_FP,ff_idx;
//////	BIG_256_28 ff_num;
//////	//BIG_256_28 ff_idx;
//////	FP_BN254CX lag;
//////	BIG_256_28 lag_num;
//////	
//////	for(int i=0;i<k;i++){
//////		FP_BN254CX_from_int(&ff_idx,i+1);
//////		ff(&ff_FP,&ff_idx);
//////		FP_BN254_redc(ff_num,&ff_FP);
//////		
//////		lagrange(&lag,i);
//////		
//////		
//////		if(i==0){
//////			ECP_BN254_copy(&RES,&g1);
//////			PAIR_BN254_G1mul(&RES,R_num);
//////			PAIR_BN254_G1mul(&RES,ff_num);
//////			
//////			if(sign_FP(&lag)){
//////				// negative
//////				FP_BN254 lag_tmp;
//////				BIG_256_28 lag_num_tmp;
//////				FP_BN254_neg(&lag_tmp,&lag);
//////				FP_BN254_redc(lag_num_tmp,&lag_tmp);
//////				PAIR_BN254_G1mul(&RES,lag_num_tmp);
//////				ECP_BN254_neg(&RES);				
//////				sprintf((char*)Send_Data_buff,"lag(0) a negative value");
//////				LoRa_SendData(Send_Data_buff);
//////			}else{
//////				FP_BN254_redc(lag_num,&lag);
//////				PAIR_BN254_G1mul(&RES,lag_num);
//////				sprintf((char*)Send_Data_buff,"lag(0) a positive value");
//////				LoRa_SendData(Send_Data_buff);
//////			}
//////			continue;
//////		}
//////		
//////		ECP_BN254_copy(&tmp,&g1);
//////		PAIR_BN254_G1mul(&tmp,R_num);
//////		PAIR_BN254_G1mul(&tmp,ff_num);		
//////		//PAIR_BN254_G1mul(&tmp,lag_num);
//////		if(sign_FP(&lag)){//FP_BN254_sign
//////			// negative
//////			FP_BN254 lag_tmp;
//////			BIG_256_28 lag_num_tmp;
//////			FP_BN254_neg(&lag_tmp,&lag);
//////			FP_BN254_redc(lag_num_tmp,&lag_tmp);
//////			PAIR_BN254_G1mul(&tmp,lag_num_tmp);
//////			ECP_BN254_neg(&tmp);
//////			sprintf((char*)Send_Data_buff,"lag(1) a negative value");
//////			LoRa_SendData(Send_Data_buff);
//////		}else{
//////			FP_BN254_redc(lag_num,&lag);
//////			PAIR_BN254_G1mul(&tmp,lag_num);
//////			sprintf((char*)Send_Data_buff,"lag(1) a positive value");
//////			LoRa_SendData(Send_Data_buff);
//////		}

//////		ECP_BN254_add(&RES,&tmp);
//////	}
//////	//FP_BN254_mul
//////	
//////	if (ECP_BN254_equals(&RES,&g21)){
//////		sprintf((char*)Send_Data_buff,"Test demo RES = g21: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test demo RES = g21: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	
//////	//g1*r*(ff[1]*lag[0]+...+ff[k]*lag[k-1])
//////	FP_BN254 tot,tot1,tot2;
//////	FP_BN254_zero(&tot);
//////	for(int i=0;i<k;i++){
//////		FP_BN254_from_int(&ff_idx,i+1);
//////		ff(&ff_FP,&ff_idx);

//////		lagrange(&lag,i);
//////		
//////		FP_BN254_mul(&tot1,&ff_FP,&lag);
//////		FP_BN254_copy(&tot2,&tot);
//////		FP_BN254_add(&tot,&tot1,&tot2);
//////	}
//////	BIG_256_28 tot_num;
//////	FP_BN254_redc(tot_num,&tot);
//////	ECP_BN254 res2;
//////	ECP_BN254_copy(&res2,&g1);
//////	PAIR_BN254_G1mul(&res2,R_num);
//////	PAIR_BN254_G1mul(&res2,tot_num);
//////	
//////	if (ECP_BN254_equals(&res2,&g21)){
//////		sprintf((char*)Send_Data_buff,"Test demo res2 = g21: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test demo res2 = g21: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	

//////	
//////	/**********************test1*************************************/
//////	//r*(ff[1]*lag[0]+...+ff[k]*lag[k-1]) == r*ff[1]*lag[0]+...+r*ff[k]*lag[k-1])
//////	
//////	
//////	//r*(ff[1]*lag[0]+...+ff[k]*lag[k-1])
//////	FP_BN254 tot1_t1;
//////	FP_BN254 tmp1_t1,tmp2_t1;
//////	FP_BN254_zero(&tot1_t1);
//////	FP_BN254_zero(&tmp2_t1);
//////	for(int i=0;i<k;i++){
//////		FP_BN254_from_int(&ff_idx,i+1);
//////		ff(&ff_FP,&ff_idx);
//////		lagrange(&lag,i);

//////		FP_BN254_mul(&tmp1_t1,&ff_FP,&lag);
//////		FP_BN254_copy(&tmp2_t1,&tot1_t1);
//////		FP_BN254_add(&tot1_t1,&tmp1_t1,&tmp2_t1);
//////	}
//////	FP_BN254_mul(&tmp1_t1,&tot1_t1,&R);
//////	FP_BN254_copy(&tot1_t1,&tmp1_t1);
//////	
//////	//r*ff[1]*lag[0]+...+r*ff[k]*lag[k-1])
//////	
//////	FP_BN254 tot2_t1,tmp3_t1;
//////	FP_BN254_zero(&tot2_t1);
//////	FP_BN254_zero(&tmp2_t1);
//////	for(int i=0;i<k;i++){
//////		FP_BN254_from_int(&ff_idx,i+1);
//////		ff(&ff_FP,&ff_idx);
//////		lagrange(&lag,i);
//////		
//////		FP_BN254_mul(&tmp1_t1,&ff_FP,&lag);
//////		FP_BN254_mul(&tmp2_t1,&R,&tmp1_t1);
//////		
//////		FP_BN254_copy(&tmp3_t1,&tot2_t1);
//////		FP_BN254_add(&tot2_t1,&tmp2_t1,&tmp3_t1);
//////	}

//////	if (FP_BN254_equals(&tot1_t1,&tot2_t1)){
//////		sprintf((char*)Send_Data_buff,"TTTTTTTTTTTest 1: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"TTTTTTTTTTTest 1: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	/**********************test2*************************************/
//////	//g1*1+g1*2 == g1*(1+2)
//////	
//////	ECP_BN254 tot1_t2,t1_t2,t2_t2;
//////	ECP_BN254 tot2_t2;
//////	
//////	FP_BN254 tmp1_t2,tmp2_t2,tmp3_t2;
//////	BIG_256_28 num1,num2,num3;
//////	FP_BN254_from_int(&tmp1_t2,6);
//////	FP_BN254_from_int(&tmp2_t2,1);
//////	FP_BN254_from_int(&tmp3_t2,7);
//////	
//////	FP_BN254_redc(num1,&tmp1_t2);
//////	FP_BN254_redc(num2,&tmp2_t2);
//////	FP_BN254_redc(num3,&tmp3_t2);
//////	
//////	ECP_BN254_copy(&t1_t2,&g1);
//////	ECP_BN254_copy(&t2_t2,&g1);
//////	
//////	//ECP_BN254_mul(&t1_t2,num1);
//////	//ECP_BN254_mul(&t2_t2,num2);
//////	PAIR_BN254_G1mul(&t1_t2,num1);
//////	PAIR_BN254_G1mul(&t2_t2,num2);
//////	
//////	ECP_BN254_add(&t1_t2,&t2_t2);
//////	ECP_BN254_copy(&tot1_t2,&t1_t2);
//////	
//////	ECP_BN254_copy(&tot2_t2,&g1);
//////	
//////	//ECP_BN254_mul(&tot2_t2,num3);
//////	PAIR_BN254_G1mul(&tot2_t2,num3);
//////	
//////	if (ECP_BN254_equals(&tot1_t2,&tot2_t2)){
//////		sprintf((char*)Send_Data_buff,"g1*1+g1*2 == g1*(1+2): Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"g1*1+g1*2 == g1*(1+2): No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	FP_BN254_from_int(&tmp1_t2,-6);
//////	FP_BN254_from_int(&tmp2_t2,3);
//////	
//////	FP_BN254_add(&tmp3_t2,&tmp1_t2,&tmp2_t2);
//////	
//////	if (!sign_FP(&tmp3_t2)){
//////		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is +");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is -");
//////		LoRa_SendData(Send_Data_buff);
//////	}

//////	/*************	Test3	********************/
//////	//g1*(-6)+g1*(7) == g1
//////	FP_BN254 tmp1_t3,tmp2_t3,tmp3_t3;
//////	ECP_BN254 g1_t3,g2_t3,g3_t3;
//////	ECP_BN254_copy(&g1_t3,&g1);
//////	ECP_BN254_copy(&g2_t3,&g1);
//////	ECP_BN254_copy(&g3_t3,&g1);
//////	
//////	FP_BN254_from_int(&tmp1_t3,-10);
//////	FP_BN254_from_int(&tmp2_t3,12);
//////	BIG_256_28 tmp1_num,tmp2_num;
//////	
//////	FP_BN254_add(&tmp3_t2,&tmp1_t2,&tmp2_t2);
//////	
//////	if(sign_FP(&tmp1_t3)){
//////		FP_BN254_neg(&tmp3_t3,&tmp1_t3);
//////		FP_BN254_redc(tmp1_num,&tmp3_t3);
//////	}
//////	FP_BN254_redc(tmp2_num,&tmp2_t3);
//////	
//////	PAIR_BN254_G1mul(&g1_t3,tmp1_num);
//////	ECP_BN254_neg(&g1_t3);
//////	PAIR_BN254_G1mul(&g2_t3,tmp2_num);
//////	ECP_BN254_add(&g1_t3,&g2_t3);
//////	
//////	FP_BN254_from_int(&tmp1_t3,2);
//////	FP_BN254_redc(tmp1_num,&tmp1_t3);
//////	PAIR_BN254_G1mul(&g3_t3,tmp1_num);
//////	
//////	if (ECP_BN254_equals(&g1_t3,&g3_t3)){
//////		sprintf((char*)Send_Data_buff,"g1*(-6)+g1*(7) == g1: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"g1*(-6)+g1*(7) == g1: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	/*************	Test4	********************/
//////	FP_BN254 TMP;
//////	FP_BN254_from_int(&TMP,9);
//////	FP_BN254_from_int(&lag,1);
//////	FP_BN254_redc(lag_num,&lag);
//////	lagrange(&lag,1);
//////	FP_BN254_redc(lag_num,&lag);
//////	if(sign_FP(&lag)){
//////		sprintf((char*)Send_Data_buff,"Sign of lag is -");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Sign of lag is +");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	if(sign_FP(&TMP)){
//////		sprintf((char*)Send_Data_buff,"Sign of TMP is -");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Sign of TMP is +");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	if(FP_BN254_equals(&lag,&TMP)){
//////		sprintf((char*)Send_Data_buff,"lag == -1: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"lag == -1: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	
////////	if (!sign_FP(&tmp3_t2)){
////////		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is +");
////////		LoRa_SendData(Send_Data_buff);
////////	}else{
////////		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is -");
////////		LoRa_SendData(Send_Data_buff);
////////	}
//////	
///////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////

////////test5

////////g1*(r*ff[1]*lag[0] + r*ff[2]*lag[1]) == g1*r*ff[1]*lag[0] + g1*r*ff[2]*lag[1]
//////	// printf("\n\n\n122211\n\n\n");

//////	//FP_BN254 R;
//////	FP_BN254 ff1,ff2,lag0,lag1;
//////	ECP_BN254 e1,e2,e3,e4;
//////	ECP_BN254 _e1,_e2,_e3,_e4;
//////	FP_BN254 m1,m2,m3,m4;
//////	FP_BN254 _m1,_m2,_m3,_m4;
//////	FP_BN254 t1,t2,t3,t4;
//////	BIG_256_28 n1,n2,n3,n4;
//////	FP_BN254_from_int(&ff_idx,1);
//////	ff(&ff1,&ff_idx);

//////	FP_BN254_from_int(&ff_idx,2);
//////	ff(&ff2,&ff_idx);

//////	lagrange(&lag0,0);
//////	lagrange(&lag1,1);

//////	//t2 = r*ff[1]*lag[0]
//////	FP_BN254_mul(&t1,&ff1,&lag0);
//////	FP_BN254_mul(&t2,&t1,&R);
//////	//t3 = r*ff[2]*lag[1]
//////	FP_BN254_mul(&t1,&ff2,&lag1);
//////	FP_BN254_mul(&t3,&t1,&R);
//////	//m1 = r*ff[1]*lag[0] + r*ff[2]*lag[1]
//////	FP_BN254_add(&m1,&t2,&t3);
//////	int flag1 = 0;
//////	int flag2 = 0;
//////	int flag3 = 0;
//////	//e1 = g1*m1
//////	if(!sign_FP(&m1)){
//////		FP_BN254_redc(n1,&m1);
//////		ECP_BN254_copy(&e1,&g1);
//////		PAIR_BN254_G1mul(&e1,n1);
//////	}else{
//////		FP_BN254_neg(&_m1,&m1);	
//////		FP_BN254_redc(n1,&_m1);
//////		ECP_BN254_copy(&e1,&g1);
//////		PAIR_BN254_G1mul(&e1,n1);
//////		ECP_BN254_neg(&e1);
//////	}
//////	//m2 = r*ff[1]*lag[0]
//////	FP_BN254_mul(&t1,&ff1,&lag0);
//////	FP_BN254_mul(&m2,&t1,&R);

//////	//e2 = g1*m2
//////	if(!sign_FP(&m2)){
//////		FP_BN254_redc(n2,&m2);
//////		ECP_BN254_copy(&e2,&g1);
//////		PAIR_BN254_G1mul(&e2,n2);
//////	}else{
//////		FP_BN254_neg(&_m2,&m2);	
//////		FP_BN254_redc(n2,&_m2);
//////		ECP_BN254_copy(&e2,&g1);
//////		PAIR_BN254_G1mul(&e2,n2);
//////		ECP_BN254_neg(&e2);
//////	}
//////	
//////	//m3 = r*ff[2]*lag[1]
//////	FP_BN254_mul(&t1,&ff2,&lag1);
//////	FP_BN254_mul(&m3,&t1,&R);

//////	//e3 = g1*m3
//////	if(!sign_FP(&m3)){
//////		FP_BN254_redc(n3,&m3);
//////		ECP_BN254_copy(&e3,&g1);
//////		PAIR_BN254_G1mul(&e3,n3);	
//////	}else{
//////		FP_BN254_neg(&_m3,&m3);	
//////		FP_BN254_redc(n3,&_m3);
//////		ECP_BN254_copy(&e3,&g1);
//////		PAIR_BN254_G1mul(&e3,n3);
//////		ECP_BN254_neg(&e3);
//////	}
//////	
//////	//e2 = e2 + e3
//////	ECP_BN254_add(&e2,&e3);


//////	if (ECP_BN254_equals(&e1,&e2)){
//////		sprintf((char*)Send_Data_buff,"test 5: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"test 5: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	if (ECP_BN254_equals(&RES,&e1)){
//////		sprintf((char*)Send_Data_buff,"Test demo RES = e1: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test demo RES = e1: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	if (ECP_BN254_equals(&RES,&e2)){
//////		sprintf((char*)Send_Data_buff,"Test demo RES = e2: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test demo RES = e2: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	if (ECP_BN254_equals(&g21,&e1)){
//////		sprintf((char*)Send_Data_buff,"Test demo g21 = e1: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test demo g21 = e1: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
//////	
//////	if (ECP_BN254_equals(&g21,&e2)){
//////		sprintf((char*)Send_Data_buff,"Test demo g21 = e2: Yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test demo g21 = e2: No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
////////	for(int i=-10000;i<=10000;i++){
////////		FP_BN254_from_int(&ff_idx,i);
////////		if(sign_FP(&ff_idx)){
////////			sprintf((char*)Send_Data_buff,"----:%d",i);
////////			LoRa_SendData(Send_Data_buff);
////////		}else{
////////			sprintf((char*)Send_Data_buff,"++++:%d",i);
////////			LoRa_SendData(Send_Data_buff);
////////		}
////////	}

//}




#endif

#include "Thres_Sig.h"
#include "HoneyBadgerBFT.h"

char thres_private_key_string[Nodes_N][key_num1] ={
"26 96 96 dd 52 9c 80 e9 af 5b f7 ce a9 e2 31 e3 8d 62 ba 19 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"37 5b 1d 29 a1 97 10 5b 9c d6 5b ba b9 8a 09 54 ec 50 7e 3a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"24 1e 83 4e 8e 05 06 79 ba e1 4b ac b4 5c 41 a5 ac 3a 22 4e 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"34 e3 09 9a dc ff 95 eb a8 5b af 98 c4 04 19 17 0b 27 e6 6f 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00"
};
char thres_public_key_string[Nodes_N+1][122] = {
"03 07 c0 8b 66 c3 7c 24 4a 0f bd 10 dd 6d 26 c5 58 08 71 62 67 21 be 5d d5 b1 6c f4 fc bf be 1e ce 06 99 3e 63 5e da 21 25", 
"02 0d 51 f3 e6 16 1b a4 fb 99 1b f9 45 ed 94 0b 00 5e 40 a6 fb 14 c8 21 da 85 46 ad 28 8d 79 ea 98 c2 a1 58 c7 41 ba 7f 3f", 
"03 0b 29 64 ef 45 43 43 d2 08 fd c3 5c 5e 46 5e c8 02 7e 4a 63 15 28 02 09 56 cd d6 96 6c 97 a3 eb a3 2f 3e 11 da 7b 12 c0", 
"02 1c 81 a7 8d 1b 99 65 88 b5 ec 86 7b 67 b3 a3 d9 e7 49 9e e3 1c e2 a1 c6 70 01 30 d8 ea 95 ec 60 90 02 5d 96 12 28 93 c3", 
"02 02 6a b7 e0 78 0d 80 79 cd 88 7d c7 94 2e 32 b9 86 d7 04 29 0e 53 da 6c ce a6 62 50 b8 8b 59 c6 7d ad 13 b7 13 56 e8 9f"
};

char M[50];
//char M_[50];

u8 players = Nodes_N;
u8 Thres_k = Nodes_f + 1;

ECP_BN158 G1;
ECP2_BN158 G2;
ECP_BN158 g1;
ECP2_BN158 g2;

BIG_160_28 a[MAX_K];
u8 S[MAX_SK];
BIG_160_28 SECRET;
BIG_160_28 SKs[MAX_SK];

ECP2_BN158 g2;
ECP2_BN158 VK;
ECP2_BN158 VKs[MAX_SK];

ECP_BN158 M_point;
ECP_BN158 Sigs[MAX_SK];
ECP_BN158 SIG;

BIG_160_28 g1_num;

void sign_data(u8* hash,u8* sig){
	while(1){
		if (!uECC_sign(private_key[Global_ID], hash, 32, sig, uECC_secp160r1())) {
			sprintf((char*)Send_Data_buff,"Sign Fail");
			Lora_Device_Sta = LORA_TX_STA;
			LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
		}else{
			break;
		}
	}
}

//void init_thres_sig(){
//	FP_BN158 ONE;
//	FP_BN158 ZERO;
//	FP_BN158_zero(&ZERO);
//	FP_BN158_one(&ONE);
//	
//	//sha256 init
//	char digest[50];
//	hash256 sh256;
//	int i;
////	u32 lenn = NLEN_256_28;
////	int32_t gg[lenn];
//	//int32_t

//	char mc[] = "geng1";

//	for(int i=0;i<50;i++)digest[i] = NULL;
//	HASH256_init(&sh256);
//	for (i=0;mc[i]!=0;i++) HASH256_process(&sh256,mc[i]);
//    HASH256_hash(&sh256,digest);
//	BIG_160_28_fromBytes(g1_num,digest);
//	
//	if(!ECP_BN158_generator(&G1)){
//		sprintf((char*)Send_Data_buff,"G1 generator Fail");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	ECP_BN158_copy(&g1,&G1);
//	PAIR_BN158_G1mul(&g1,g1_num);
//	//ECP_BN158_copy(&g2,&g1);
//	
//	if (!ECP2_BN158_generator(&G2)){
//		sprintf((char*)Send_Data_buff,"G2 generator Fail");
//		LoRa_SendData(Send_Data_buff);
//	}
//	ECP2_BN158_copy(&g2,&G2);
//	PAIR_BN158_G2mul(&g2,g1_num);
//	


//	char sch[MAX_Nodes][45];
//	char vch[MAX_Nodes][65];
//	for(int i=0;i<MAX_Nodes;i++)
//		for(int j=0;j<65;j++){	
//			vch[i][j] = 0x00;
//		}
//	for(int i=0;i<MAX_Nodes;i++)
//		for(int j=0;j<45;j++)
//			sch[i][j] = 0x00;
//	
//	u8 string_idx=0;
//	u8 key_idx = 0;
//	u16 private_key_idx;
//	u16 public_key_idx;
//	for(u8 l=1;l<=Nodes_N+1;l++){
//		key_idx = 0;
//		for(string_idx = 0; string_idx < sizeof(thres_public_key_string[l-1]);){
//			vch[l][key_idx] = Sixteen2Ten(thres_public_key_string[l-1][string_idx])*16+Sixteen2Ten(thres_public_key_string[l-1][string_idx + 1]);
//			key_idx = key_idx + 1;
//			string_idx = string_idx + 3;
//		}
//		public_key_idx = key_idx;
//		
//		key_idx = 0;
//		for(string_idx = 0; string_idx < sizeof(thres_private_key_string[l-1]);){
//			sch[l][key_idx] = Sixteen2Ten(thres_private_key_string[l-1][string_idx])*16 + Sixteen2Ten(thres_private_key_string[l-1][string_idx + 1]);
//			key_idx = key_idx + 1;
//			string_idx = string_idx + 3;
//		}
//		private_key_idx = key_idx;
//		
//	}
//	
//	octet RAW[MAX_Nodes];
//	for(int i=0;i<MAX_Nodes;i++){
//		RAW[i].max = 200;
//		RAW[i].len = public_key_idx;
//		RAW[i].val = vch[i];
//	}
//	
//	
//	for(int i=1;i<=players;i++){
//		FP_BN158_fromBytes(&SKs[i],sch[i]);
//	}
//	for(int i=1;i<=players;i++){
//		ECP2_BN158_fromOctet(&VKs[i],&RAW[i]);
//	}
//	ECP2_BN158_fromOctet(&VK,&RAW[Nodes_N+1]);
//	
//	for(u8 i=0;i<10;i++)S[i] = i;
////	char test256[]="Round: 1";
////	
////	for(int i=0;i<50;i++)digest[i] = NULL;
////	HASH256_init(&sh256);
////	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
////    HASH256_hash(&sh256,digest);
////	
////	for(int i=0;i<50;i++)M[i]=digest[i];
////	
////	FP_BN158 M_FP;
////	BIG_160_28 M_num;
////	FP_BN158_fromBytes(&M_FP,M);
////	
////	if(FP_BN158_sign(&M_FP)){
////		// negative num
////		FP_BN158 M_FP_neg;
////		FP_BN158_neg(&M_FP_neg,&M_FP);
////		FP_BN158_redc(M_num,&M_FP);
////		ECP_BN158_copy(&M_point,&G1);
////		PAIR_BN158_G1mul(&M_point,M_num);
////		ECP_BN158_neg(&M_point);
////	}else{
////		FP_BN158_redc(M_num,&M_FP);
////		ECP_BN158_copy(&M_point,&G1);
////		PAIR_BN158_G1mul(&M_point,M_num);
////	}
//	
//	
//	
//	
//	
////	SK_sign();
////	PK_verify_sig_share();
////	PK_sig_combine_share();
////	PK_verify_sig();
//	//	while(1);
//	
//{	
////	octet RAW[20];
////	for(int i=0;i<20;i++){
////		RAW[i].max = 200;
////		RAW[i].len = public_key_idx;
////		RAW[i].val = vch[i];
////	}
////	
////	
////	for(int i=0;i<players;i++){
////		FP_BN158_fromBytes(&SKs[i],sch[i+1]);
////	}
////	for(int i=0;i<players;i++){
////		ECP2_BN158_fromOctet(&VKs[i],&RAW[i+1]);
////	}
////	ECP2_BN158_fromOctet(&VK,&RAW[Nodes_N+1]);
////	char test256[]="Round: 1";
////	
////	for(int i=0;i<50;i++)digest[i] = NULL;
////	HASH256_init(&sh256);
////	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
////    HASH256_hash(&sh256,digest);
////	
////	for(int i=0;i<50;i++)M[i]=digest[i];
////	
////	FP_BN158 M_FP;
////	BIG_160_28 M_num;
////	FP_BN158_fromBytes(&M_FP,M);
////	
////	if(FP_BN158_sign(&M_FP)){
////		// negative num
////		FP_BN158 M_FP_neg;
////		FP_BN158_neg(&M_FP_neg,&M_FP);
////		FP_BN158_redc(M_num,&M_FP);
////		ECP_BN158_copy(&M_point,&G1);
////		PAIR_BN158_G1mul(&M_point,M_num);
////		ECP_BN158_neg(&M_point);
////	}else{
////		FP_BN158_redc(M_num,&M_FP);
////		ECP_BN158_copy(&M_point,&G1);
////		PAIR_BN158_G1mul(&M_point,M_num);
////	}
////	
////	for(u8 i=0;i<10;i++)S[i] = i;
////	SK_sign();
////	PK_verify_sig_share();
////	PK_sig_combine_share();
////	PK_verify_sig();
////Debug comment
////	dealer();
////	
////	
////	for(int i=0;i<20;i++)
////		for(int j=0;j<70;j++){
////			sch[i][j] = 0x00;
////			vch[i][j] = 0x00;
////		}
////	octet RAW[20];
////	for(int i=0;i<20;i++){
////		RAW[i].max = 200;
////		RAW[i].len = 0;
////		RAW[i].val = vch[i];
////	}
////	for(int i=0;i<players;i++){
////		FP_BN158_toBytes(sch[i],&SKs[i]);
////	}
////	for(int i=0;i<players;i++){
////		ECP2_BN158_toOctet(&RAW[i],&VKs[i],true);
////	}
////	
////	FP_BN158 tmp_SKs[MAX_SK];
////	ECP2_BN158 tmp_VKs[MAX_SK];
////	
////	for(int i=0;i<players;i++){
////		FP_BN158_fromBytes(&tmp_SKs[i],sch[i]);
////	}
////	for(int i=0;i<players;i++){
////		ECP2_BN158_fromOctet(&tmp_VKs[i],&RAW[i]);
////	}
////	int flag = 1;
////	for(int i=0;i<players;i++){
////		if(!FP_BN158_equals(&tmp_SKs[i],&SKs[i])){
////			flag = 0;
////		}
////	}
////	
////	for(int i=0;i<players;i++){
////		if(!ECP2_BN158_equals(&tmp_VKs[i],&VKs[i])){
////			flag = 0;
////		}		
////	}
////	//VK
////	ECP2_BN158_toOctet(&RAW[19],&VK,true);
////	ECP2_BN158_fromOctet(&tmp_VKs[19],&RAW[19]);
////	if(!ECP2_BN158_equals(&tmp_VKs[19],&VK)){
////		flag = 0;
////	}

////	for(int i=0;i<players;i++){
////		for(int j=0;j<40;j++)Send_Data_buff[j] = sch[i][j];
////		Send_Data_buff[40] = 0x0D;
////		Send_Data_buff[41] = 0x0A;
////		HAL_UART_Transmit_DMA(&uart3_handler,Send_Data_buff,42);
////		
////		while(1)
////		{
////			if(__HAL_DMA_GET_FLAG(&UART3TxDMA_Handler,DMA_FLAG_TCIF3_7))//等待DMA1_Steam3传输完成
////			{
////				__HAL_DMA_CLEAR_FLAG(&UART3TxDMA_Handler,DMA_FLAG_TCIF3_7);//清除DMA1_Steam3传输完成标志
////				HAL_UART_DMAStop(&uart3_handler);      //传输完成以后关闭串口DMA
////				break;
////			}
////		}
////	}

////	for(int i=0;i<players;i++){
////		for(int j=0;j<RAW[i].len;j++)Send_Data_buff[j] = RAW[i].val[j];
////		Send_Data_buff[RAW[i].len] = 0x0D;
////		Send_Data_buff[RAW[i].len+1] = 0x0A;
////		HAL_UART_Transmit_DMA(&uart3_handler,Send_Data_buff,RAW[i].len+2);
////		while(1)
////		{
////			if(__HAL_DMA_GET_FLAG(&UART3TxDMA_Handler,DMA_FLAG_TCIF3_7))//等待DMA1_Steam3传输完成
////			{
////				__HAL_DMA_CLEAR_FLAG(&UART3TxDMA_Handler,DMA_FLAG_TCIF3_7);//清除DMA1_Steam3传输完成标志
////				HAL_UART_DMAStop(&uart3_handler);      //传输完成以后关闭串口DMA
////				break;
////			}
////		}
////		
////	}
////	//VK
////	for(int j=0;j<RAW[19].len;j++)Send_Data_buff[j] = RAW[19].val[j];
////	Send_Data_buff[RAW[19].len] = 0x0D;
////	Send_Data_buff[RAW[19].len+1] = 0x0A;
////	HAL_UART_Transmit_DMA(&uart3_handler,Send_Data_buff,RAW[19].len+2);
////	if(!flag){
////		while(1);
////	}
////	
//////	ECP2_BN158_fromOctet
////	
////	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
////	for(int i=0;i<50;i++)digest[i] = NULL;
////	HASH256_init(&sh256);
////	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
////    HASH256_hash(&sh256,digest);
////	for(int i=0;i<50;i++)M[i]=digest[i];
////	
////	FP_BN158 M_FP;
////	BIG_160_28 M_num;
////	FP_BN158_fromBytes(&M_FP,M);
////	
////	if(FP_BN158_sign(&M_FP)){
////		// negative num
////		FP_BN158 M_FP_neg;
////		FP_BN158_neg(&M_FP_neg,&M_FP);
////		FP_BN158_redc(M_num,&M_FP);
////		ECP_BN158_copy(&M_point,&G1);
////		PAIR_BN158_G1mul(&M_point,M_num);
////		ECP_BN158_neg(&M_point);
////	}else{
////		FP_BN158_redc(M_num,&M_FP);
////		ECP_BN158_copy(&M_point,&G1);
////		PAIR_BN158_G1mul(&M_point,M_num);
////	}
////	
////	SK_sign();
////	PK_verify_sig_share();
////	PK_sig_combine_share();
////	PK_verify_sig();
//}
//}
////ABA coin
////void Coin_SK_sign(int round){
////	if(ACK_ABA_share[round][ID])return;
////	//h ** sk
////	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
////	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
////	sprintf((char*)Coin_Str,"Coin:%d",round);
////	hash256 sh256;
////	HASH256_init(&sh256);
////	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
////    HASH256_hash(&sh256,digest);
////	
////	//for(int i=0;i<50;i++)M[i]=digest[i];
////	ECP_BN158 C_point;
////	
////	FP_BN158 C_FP;
////	BIG_160_28 C_num;
////	FP_BN158_fromBytes(&C_FP,digest);
////	
////	if(FP_BN158_sign(&C_FP)){
////		// negative num
////		FP_BN158 C_FP_neg;
////		FP_BN158_neg(&C_FP_neg,&C_FP);
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////		ECP_BN158_neg(&C_point);
////	}else{
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////	}
////	
////	BIG_160_28 num;
////	ECP_BN158 h;
////	FP_BN158_redc(num,&SKs[ID]);//sk
////	ECP_BN158_copy(&h,&C_point);//h
////	PAIR_BN158_G1mul(&h,num);
////	
////	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
////	octet Obj;
////	Obj.len = 0;
////	Obj.max = 70;
////	Obj.val = ch;
////	
////	ECP_BN158_toOctet(&Obj,&h,true);
////	if(Obj.len != Share_Size){while(1);}
////	for(int i=0;i<Obj.len;i++)
////		ABA_share[round][ID][i] = Obj.val[i];
////	if(!ACK_ABA_share[round][ID])ABA_share_Number[round]++;
////	ACK_ABA_share[round][ID] = 1;
////	
//////	BIG_160_28 num;
//////	for(int i=0;i<players;i++){
//////		FP_BN158_redc(num,&SKs[i]);
//////		ECP_BN158_copy(&Sigs[i],&M_point);
//////		PAIR_BN158_G1mul(&Sigs[i],num);
//////	}
////	
////	
////}

////void tmp_Coin_SK_sign(u8 id, int round){
////	if(ACK_ABA_share[round][id])return;
////	//h ** sk
////	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
////	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
////	sprintf((char*)Coin_Str,"Coin:%d",round);
////	hash256 sh256;
////	HASH256_init(&sh256);
////	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
////    HASH256_hash(&sh256,digest);
////	
////	//for(int i=0;i<50;i++)M[i]=digest[i];
////	ECP_BN158 C_point;
////	
////	FP_BN158 C_FP;
////	BIG_160_28 C_num;
////	FP_BN158_fromBytes(&C_FP,digest);
////	
////	if(FP_BN158_sign(&C_FP)){
////		// negative num
////		FP_BN158 C_FP_neg;
////		FP_BN158_neg(&C_FP_neg,&C_FP);
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////		ECP_BN158_neg(&C_point);
////	}else{
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////	}
////	
////	BIG_160_28 num;
////	ECP_BN158 h;
////	FP_BN158_redc(num,&SKs[id]);//sk
////	ECP_BN158_copy(&h,&C_point);//h
////	PAIR_BN158_G1mul(&h,num);
////	
////	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
////	octet Obj;
////	Obj.len = 0;
////	Obj.max = 70;
////	Obj.val = ch;
////	
////	ECP_BN158_toOctet(&Obj,&h,true);
////	if(Obj.len != Share_Size){while(1);}
////	for(int i=0;i<Obj.len;i++)
////		ABA_share[round][id][i] = Obj.val[i];
////	if(!ACK_ABA_share[round][id])ABA_share_Number[round]++;
////	ACK_ABA_share[round][id] = 1;
////	
//////	BIG_160_28 num;
//////	for(int i=0;i<players;i++){
//////		FP_BN158_redc(num,&SKs[i]);
//////		ECP_BN158_copy(&Sigs[i],&M_point);
//////		PAIR_BN158_G1mul(&Sigs[i],num);
//////	}
////}
////int Coin_PK_verify_sig_share(u8 id, u8 round, u8 *sig){

//////	TIM9_Exceed_Times = 0;
//////	TIM9->CNT = 0;
////	
//////	B = self.VKs[i]
//////	assert pair(g2, sig) == pair(B, h)
//////	return True
////	ECP2_BN158 B;
////	ECP2_BN158 g2_neg;
////	ECP2_BN158_copy(&g2_neg,&g2);
////	ECP2_BN158_neg(&g2_neg);
////	
////	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
////	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
////	sprintf((char*)Coin_Str,"Coin:%d",round);
////	hash256 sh256;
////	HASH256_init(&sh256);
////	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
////    HASH256_hash(&sh256,digest);
////	
////	ECP_BN158 C_point;
////	
////	FP_BN158 C_FP;
////	BIG_160_28 C_num;
////	FP_BN158_fromBytes(&C_FP,digest);
////	
////	if(FP_BN158_sign(&C_FP)){
////		// negative num
////		FP_BN158 C_FP_neg;
////		FP_BN158_neg(&C_FP_neg,&C_FP);
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////		ECP_BN158_neg(&C_point);
////	}else{
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////	}
////	
////	ECP_BN158 share_point;
////	octet Obj;
////	Obj.len = 0;
////	Obj.max = 70;
////	Obj.val = (char*)sig;
////	ECP_BN158_fromOctet(&share_point,&Obj);
////	
////	ECP2_BN158_copy(&B,&VKs[id]);
////	FP12_BN158 v;
////	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&B,&C_point);
////	PAIR_BN158_fexp(&v);
////	
////	if (FP12_BN158_isunity(&v)){
////		return 1;
////		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: Yes ");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		return 0;
////		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: No ");
////		LoRa_SendData(Send_Data_buff);
////	}
////	
////	
////}


////void Coin_PK_sig_combine_share(u8 round){
////	//	def combine_shares(self, (U,V,W), shares):
////	//	# sigs: a mapping from idx -> sig
////	//	S = set(shares.keys())
////	//	assert S.issubset(range(self.l))

////	//	# ASSUMPTION
////	//	# assert self.verify_ciphertext((U,V,W))

////	//	mul = lambda a,b: a*b
////	//	res = reduce(mul, [sig ** self.lagrange(S, j) for j,sig in sigs.iteritems()], 1)
////    //  return res
////	
////	//init S
////	u8 idx = 0;
////	ECP_BN158 Coin_Sigs[Nodes_N];
////	octet Obj[MAX_Nodes];
////	for(u8 i=0;i<MAX_Nodes;i++){
////		Obj[i].len = Share_Size;
////		Obj[i].max = Share_Size;
////		Obj[i].val = (char*)ABA_share[round][i];
////	}
////	for(u8 i=1;i<=Nodes_N;i++){
////		if(ACK_ABA_share[round][i]){
////			S[idx] = i-1;
////			ECP_BN158_fromOctet(&Coin_Sigs[i-1],&Obj[i]);
////			idx++;
////		}
////		if(ACK_ABA_share[round][i] == 0 && i <= Thres_k){
////			tmp_Coin_SK_sign(i,round);
////			S[idx] = i-1;
////			ECP_BN158_fromOctet(&Coin_Sigs[i-1],&Obj[i]);
////			idx++;
////		}
////	}
////	
////	
////	
////	FP_BN158 larg;
////	BIG_160_28 larg_num;
////	ECP_BN158 tmp;
////	ECP_BN158 r[20];
////	//ECP_BN158 res;
////	
////	for(int j=0 ; j<Thres_k ; j++){
//////		share ** self.lagrange(S, j)
////		u8 jj = S[j];
////		lagrange(&larg,jj);
////		FP_BN158_redc(larg_num,&larg);
////		ECP_BN158_copy(&tmp,&Coin_Sigs[jj]);
////		
////		if(FP_BN158_sign(&larg)){
////			// negative
////			FP_BN158 larg_tmp;
////			BIG_160_28 larg_num_tmp;
////			FP_BN158_neg(&larg_tmp,&larg);
////			FP_BN158_redc(larg_num_tmp,&larg_tmp);
////			PAIR_BN158_G1mul(&tmp,larg_num_tmp);
////			ECP_BN158_neg(&tmp);
////		}else{
////			FP_BN158_redc(larg_num,&larg);
////			PAIR_BN158_G1mul(&tmp,larg_num);
////		}
////		
////		ECP_BN158_copy(&r[j],&tmp);	
////	}
////	
////	for(int j=1 ; j<Thres_k ; j++){
////		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
////	}
//////	ECP_BN158_copy(&SIG,&r[0]);	
////	
////	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
////	octet OBJ;
////	OBJ.len = 0;
////	OBJ.max = 70;
////	OBJ.val = ch;
////	
////	ECP_BN158_toOctet(&OBJ,&r[0],true);
////	Shared_Coin[round] = OBJ.val[OBJ.len-1] % 2;
////	for(int i=0;i<OBJ.len;i++)
////		COIN[i] = OBJ.val[i];
////	
////	//verify the coin
////	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
////	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
////	sprintf((char*)Coin_Str,"Coin:%d",round);
////	hash256 sh256;
////	HASH256_init(&sh256);
////	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
////    HASH256_hash(&sh256,digest);
////	
////	//for(int i=0;i<50;i++)M[i]=digest[i];
////	ECP_BN158 C_point;
////	
////	FP_BN158 C_FP;
////	BIG_160_28 C_num;
////	FP_BN158_fromBytes(&C_FP,digest);
////	
////	if(FP_BN158_sign(&C_FP)){
////		// negative num
////		FP_BN158 C_FP_neg;
////		FP_BN158_neg(&C_FP_neg,&C_FP);
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////		ECP_BN158_neg(&C_point);
////	}else{
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////	}

////	
////	ECP2_BN158 g2_neg;
////	ECP2_BN158_copy(&g2_neg,&g2);
////	ECP2_BN158_neg(&g2_neg);
////	FP12_BN158 v;
////	PAIR_BN158_double_ate(&v,&g2_neg,&r[0],&VK,&C_point);
////	PAIR_BN158_fexp(&v);
////	
//////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
//////	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
//////	LoRa_SendData(Send_Data_buff);
////	
////	
////	if (FP12_BN158_isunity(&v)){
//////		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
//////		LoRa_SendData(Send_Data_buff);
////	}else{
////		while(1){
////			sprintf((char*)Send_Data_buff,"PK_verify_sig: No: round: %d", round);
////			LoRa_SendData(Send_Data_buff);
////		}
////		
////	}
////	
////}
//////void Coin_PK_verify_sig(u8 round){
////	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
////	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
////	sprintf((char*)Coin_Str,"Coin:%d",round);
////	hash256 sh256;
////	HASH256_init(&sh256);
////	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
////    HASH256_hash(&sh256,digest);
////	
////	//for(int i=0;i<50;i++)M[i]=digest[i];
////	ECP_BN158 C_point;
////	
////	FP_BN158 C_FP;
////	BIG_160_28 C_num;
////	FP_BN158_fromBytes(&C_FP,digest);
////	
////	if(FP_BN158_sign(&C_FP)){
////		// negative num
////		FP_BN158 C_FP_neg;
////		FP_BN158_neg(&C_FP_neg,&C_FP);
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////		ECP_BN158_neg(&C_point);
////	}else{
////		FP_BN158_redc(C_num,&C_FP);
////		ECP_BN158_copy(&C_point,&G1);
////		PAIR_BN158_G1mul(&C_point,C_num);
////	}
////	
////	ECP_BN158 share_point;
////	octet Obj;
////	Obj.len = 0;
////	Obj.max = 70;
////	Obj.val = (char*)sig;
////	ECP_BN158_fromOctet(&share_point,&Obj);
////	
////	ECP2_BN158 g2_neg;
////	ECP2_BN158_copy(&g2_neg,&g2);
////	ECP2_BN158_neg(&g2_neg);
////	FP12_BN158 v;
////	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&VK,&C_point);
////	PAIR_BN158_fexp(&v);
////	
//////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
//////	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
//////	LoRa_SendData(Send_Data_buff);
////	
////	
////	if (FP12_BN158_isunity(&v)){
////		//return 1;
////		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		//return 0;
////		sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
////		LoRa_SendData(Send_Data_buff);
////	}
////	
////}


////threshold signature
//void ff(FP_BN158 *r,FP_BN158 *x){
//	FP_BN158 y;
//	FP_BN158 xx;
//	FP_BN158_zero(&y);
//	FP_BN158_one(&xx);
//	FP_BN158 tmp;
//	FP_BN158 tmp1;
//	for(u16 i=0;i<Thres_k;i++){
//		
//		//y+=coeff * xx
//		FP_BN158_mul(&tmp,&a[i],&xx);//tmp = a[i] * xx
//		//BIG_160_28_norm(tmp);
//		FP_BN158_add(&tmp1,&y,&tmp);//tmp1 = y + tmp
//		//BIG_160_28_norm(tmp1);
//		FP_BN158_copy(&y,&tmp1);  //y = tmp1
//		
//		//xx*=x
//		FP_BN158_mul(&tmp,&xx,x);//tmp = xx * x
//		//BIG_160_28_norm(tmp);
//		FP_BN158_copy(&xx,&tmp);
//		
//	}
//	FP_BN158_copy(r,&y);
//}

//void lagrange(FP_BN158 *r,u8 j){
////	def lagrange(self, S, j):
////	# Assert S is a subset of range(0,self.l)
////	assert len(S) == self.k
////	assert type(S) is set
////	assert S.issubset(range(0,self.l))
////	S = sorted(S)

////	assert j in S
////	assert 0 <= j < self.l
////	mul = lambda a,b: a*b
////	num = reduce(mul, [0 - jj - 1 for jj in S if jj != j], ONE)
////	den = reduce(mul, [j - jj     for jj in S if jj != j], ONE)
////	return num / den
///******************************************************************/
//	
//	
//	//	num = reduce(mul, [0 - jj - 1 for jj in S if jj != j], ONE)
//	FP_BN158 num, ZERO, ONE, S_jj, tmp, tmp2, tmp3;
//	FP_BN158_one(&num);
//	FP_BN158_zero(&ZERO);
//	FP_BN158_one(&ONE);
//	
//	for(u8 jj=0;jj<Thres_k;jj++){
//		if(S[jj] == j) continue;
//		//num = num * (0 - S[jj] - 1);
//		FP_BN158_from_int(&S_jj,S[jj]);
//		FP_BN158_sub(&tmp,&ZERO,&S_jj);//tmp = 0 - S[jj]
//		FP_BN158_sub(&tmp2,&tmp,&ONE);//tmp2 = tmp - 1
//		FP_BN158_mul(&tmp3, &num, &tmp2);//tmp3 = num * tmp2
//		FP_BN158_copy(&num,&tmp3);	// num = tmp3 
//	}

//	//	den = reduce(mul, [j - jj     for jj in S if jj != j], ONE)
//	FP_BN158 den, BIG_J;
//	FP_BN158_one(&den);
//	FP_BN158_from_int(&BIG_J,j);
//	for(u8 jj=0;jj<Thres_k;jj++){
//		if(S[jj] == j) continue;
//		//den = den * (j - S[jj]);
//		FP_BN158_from_int(&S_jj,S[jj]);
//		FP_BN158_sub(&tmp,&BIG_J,&S_jj);//tmp = j - S[jj]
//		FP_BN158_mul(&tmp2, &den, &tmp);//tmp2 = den * tmp
//		FP_BN158_copy(&den,&tmp2);
//	}
//	//return num/den
//	FP_BN158_inv(&tmp, &den,NULL);
//	FP_BN158_mul(r, &num, &tmp);

//}

//void dealer(){
//	
////	TIM9_Exceed_Times = 0;
////	TIM9->CNT = 0;
//	
//	char raw[100];
//    octet RAW = {0, sizeof(raw), raw};
//    csprng RNGx;                // Crypto Strong RNG
//	u32 ran = RNG_Get_RandomNum();	
//    RAW.len = 100;              // fake random seed source
//    RAW.val[0] = ran;
//    RAW.val[1] = ran >> 8;
//    RAW.val[2] = ran >> 16;
//    RAW.val[3] = ran >> 24;
//    for (u16 i = 4; i < 100; i++) RAW.val[i] = i;
//    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
//	
//	
//	FP_BN158 secret;
//	
//	if(DEBUG){
//		FP_BN158_from_int(&secret,22);
//	}else{
//		FP_BN158_rand(&secret,&RNGx);
//	}
//	
//	
////	while(FP_BN158_sign(&secret)){
////		FP_BN158_rand(&secret,&RNGx);
////	}
//	ran = RNG_Get_RandomNum();
//	FP_BN158_from_int(&secret,ran);
//	FP_BN158_copy(&a[0],&secret);
//	FP_BN158_copy(&SECRET,&secret);

//	for(u16 i=1;i<Thres_k;i++){
//		
//		//FP_BN158_rand(&a[i],&RNGx);
//		if(DEBUG){
//			ran = RNG_Get_RandomNum();
//			FP_BN158_from_int(&a[i],88);
//		}else{
//			ran = RNG_Get_RandomNum();
//			FP_BN158_from_int(&a[i],ran);
//		}
//		
//		while(FP_BN158_sign(&a[i])){
//			ran = RNG_Get_RandomNum();
//			FP_BN158_from_int(&a[i],ran);
//			//FP_BN158_rand(&a[i],&RNGx);
//		}

//	}
//	
//	for(u16 i=0;i<players;i++){
//		FP_BN158 x;
//		FP_BN158_from_int(&x,i+1);
//		ff(&SKs[i],&x);
//	}
//	
//	//assert ff(0) == secret
//	FP_BN158 secret1;
//	FP_BN158 ZERO;
//	FP_BN158_zero(&ZERO);
//	ff(&secret1,&ZERO);
////	if(FP_BN158_equals(&secret1,&secret)){
////		sprintf((char*)Send_Data_buff,"hahaha");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"wuwuwu");
////		LoRa_SendData(Send_Data_buff);
////	}
//	
//	//VK = g2 ** secret
//	ECP2_BN158_copy(&VK,&g2);
//	BIG_160_28 FP_num;
//	FP_BN158_redc(FP_num,&secret);
//	PAIR_BN158_G2mul(&VK,FP_num);
//	
//	//VKs = [g2 ** xx for xx in SKs ]
//	for(u16 i=0 ; i<players ; i++){
//		ECP2_BN158_copy(&VKs[i],&g2);
//		FP_BN158_redc(FP_num,&SKs[i]);
//		PAIR_BN158_G2mul(&VKs[i],FP_num);	
//	}	
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	dealer_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"Dealer Time:%d",time);
////	LoRa_SendData(Send_Data_buff);
////	return;
//	/**************************Dealer end**********************************/
//	

////	# Check reconstruction of 0
////  S = set(range(0,k))[0,1,2]
////  lhs = f(0)
//	FP_BN158 lhs,rhs,tmp,tmp1,tmp2,tmp3,tmp4;
////	FP_BN158 ZERO;
//	FP_BN158_zero(&ZERO);
//	FP_BN158_zero(&rhs);
//	ff(&lhs,&ZERO);
//	
////  rhs = sum(public_key.lagrange(S,j) * f(j+1) for j in S)
//	for(u8 i=0;i<10;i++)S[i] = i;
//	
//	for(u16 i=0;i<Thres_k;i++){
//		FP_BN158_from_int(&tmp2,S[i]+1);
//		ff(&tmp,&tmp2);//tmp = ff[S[i]+1]
//		lagrange(&tmp1,S[i]);//tmp1 = lagrange
//		FP_BN158_mul(&tmp3,&tmp1,&tmp);//tmp3 = tmp1 * tmp
//		FP_BN158_add(&tmp4,&tmp3,&rhs);//tmp4 = tmp3 + rhs
//		FP_BN158_copy(&rhs,&tmp4);//rhs = tmp4
//	}
//	
//	BIG_160_28 lhs_num,rhs_num;
//	
//	FP_BN158_redc(lhs_num,&lhs);
//	FP_BN158_redc(rhs_num,&rhs);
//	
////	if(FP_BN158_equals(&lhs,&rhs) && BIG_160_28_comp(lhs_num,rhs_num)==0){
////		sprintf((char*)Send_Data_buff,"Reconstruction: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"Reconstruction: NO");
////		LoRa_SendData(Send_Data_buff);
////	}
//	
//}




//void SK_sign(){
////	TIM9_Exceed_Times = 0;
////	TIM9->CNT = 0;
//	
//	//h ** sk
//	BIG_160_28 num;
//	for(int i=0;i<players;i++){
//		FP_BN158_redc(num,&SKs[i]);
//		ECP_BN158_copy(&Sigs[i],&M_point);
//		PAIR_BN158_G1mul(&Sigs[i],num);
//	}
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	SK_sign_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"SK_sign:%d",time);
////	LoRa_SendData(Send_Data_buff);
//	
//	sprintf((char*)Send_Data_buff,"Sign: Yes");
//	LoRa_SendData(Send_Data_buff);
//	
//}

//void PK_verify_sig_share(){

////	TIM9_Exceed_Times = 0;
////	TIM9->CNT = 0;
//	
////	B = self.VKs[i]
////	assert pair(g2, sig) == pair(B, h)
////	return True
//	ECP2_BN158 B;
//	ECP2_BN158 g2_neg;
//	ECP2_BN158_copy(&g2_neg,&g2);
//	ECP2_BN158_neg(&g2_neg);
//	for(int i=0;i<players;i++){
////		TIM2_Exceed_Times = 0;
////		TIM2->CNT = 0;
//		
//		ECP2_BN158_copy(&B,&VKs[i]);
//		FP12_BN158 v;
//		PAIR_BN158_double_ate(&v,&g2_neg,&Sigs[i],&B,&M_point);
//		PAIR_BN158_fexp(&v);
//		
////		u32 timex = TIM2->CNT + TIM2_Exceed_Times * 9000;
////		
//		if (FP12_BN158_isunity(&v)){
//			sprintf((char*)Send_Data_buff,"PK_verify_sig_share[%d]: Yes ",i);
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"PK_verify_sig_share[%d]: No ",i);
//			LoRa_SendData(Send_Data_buff);
//		}
//	}
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_share_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"PK_verify_sig_share %d : %d",players,time);
////	LoRa_SendData(Send_Data_buff);
//}

//void PK_verify_sig(){
//	
////	TIM9_Exceed_Times = 0;
////	TIM9->CNT = 0;
//	
////	assert pair(sig, g2) == pair(h, self.VK)
////  return True
//	ECP2_BN158 g2_neg;
//	ECP2_BN158_copy(&g2_neg,&g2);
//	ECP2_BN158_neg(&g2_neg);
//	FP12_BN158 v;
//	PAIR_BN158_double_ate(&v,&g2_neg,&SIG,&VK,&M_point);
//	PAIR_BN158_fexp(&v);
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
////	LoRa_SendData(Send_Data_buff);
//	
//	
//	if (FP12_BN158_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//}

//void PK_sig_combine_share(){
////	TIM9_Exceed_Times = 0;
////	TIM9->CNT = 0;
//	
//	//	def combine_shares(self, (U,V,W), shares):
//	//	# sigs: a mapping from idx -> sig
//	//	S = set(shares.keys())
//	//	assert S.issubset(range(self.l))

//	//	# ASSUMPTION
//	//	# assert self.verify_ciphertext((U,V,W))

//	//	mul = lambda a,b: a*b
//	//	res = reduce(mul, [sig ** self.lagrange(S, j) for j,sig in sigs.iteritems()], 1)
//    //  return res
//	
//	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	FP_BN158 larg;
//	BIG_160_28 larg_num;
//	ECP_BN158 tmp;
//	ECP_BN158 r[20];
//	//ECP_BN158 res;
//	
//	for(int j=0 ; j<Thres_k ; j++){
////		share ** self.lagrange(S, j)
//		lagrange(&larg,j);
//		FP_BN158_redc(larg_num,&larg);
//		ECP_BN158_copy(&tmp,&Sigs[j]);
//		
//		if(FP_BN158_sign(&larg)){
//			// negative
//			FP_BN158 larg_tmp;
//			BIG_160_28 larg_num_tmp;
//			FP_BN158_neg(&larg_tmp,&larg);
//			FP_BN158_redc(larg_num_tmp,&larg_tmp);
//			PAIR_BN158_G1mul(&tmp,larg_num_tmp);
//			ECP_BN158_neg(&tmp);
//		}else{
//			FP_BN158_redc(larg_num,&larg);
//			PAIR_BN158_G1mul(&tmp,larg_num);
//		}
//		
//		ECP_BN158_copy(&r[j],&tmp);	
//	}
//	
//	for(int j=1 ; j<Thres_k ; j++){
//		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
//	}
//	ECP_BN158_copy(&SIG,&r[0]);	
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_sig_combine_share_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"PK_sig_combine_share:%d",time);
////	LoRa_SendData(Send_Data_buff);

//}


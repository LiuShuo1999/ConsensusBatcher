#include "Thres_Sig.h"
#include "Dumbo2.h"

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

//int sign_FP(FP_BN158 *x){
//    int cp;
//    BIG_160_28 m,pm1d2;
//    FP_BN158 y;
//    BIG_160_28_rcopy(pm1d2, Modulus_BN158);
//    BIG_160_28_dec(pm1d2,1);
//    BIG_160_28_fshr(pm1d2,1); //(p-1)/2
//    
//    FP_BN158_copy(&y,x);
//    FP_BN158_reduce(&y);
//    FP_BN158_redc(m,&y);
//    cp=BIG_160_28_comp(m,pm1d2);
//    return ((cp+1)&2)>>1;
//}


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


void init_thres_sig(){

	//sha256 init
	char digest[50];
	hash256 sh256;
	int i;
//	u32 lenn = NLEN_256_28;
//	int32_t gg[lenn];
	//int32_t

	char mc[] = "geng1";

	for(int i=0;i<50;i++)digest[i] = NULL;
	HASH256_init(&sh256);
	for (i=0;mc[i]!=0;i++) HASH256_process(&sh256,mc[i]);
    HASH256_hash(&sh256,digest);
	BIG_160_28_fromBytes(g1_num,digest);
	
	if(!ECP_BN158_generator(&G1)){
		sprintf((char*)Send_Data_buff,"G1 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	
	ECP_BN158_copy(&g1,&G1);
	PAIR_BN158_G1mul(&g1,g1_num);
	//ECP_BN158_copy(&g2,&g1);
	
	if (!ECP2_BN158_generator(&G2)){
		sprintf((char*)Send_Data_buff,"G2 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	ECP2_BN158_copy(&g2,&G2);
	PAIR_BN158_G2mul(&g2,g1_num);
	
	char sch[MAX_Nodes][100];
	char vch[MAX_Nodes][100];
	for(int i=0;i<MAX_Nodes;i++)
		for(int j=0;j<100;j++){	
			vch[i][j] = 0x00;
		}
	for(int i=0;i<MAX_Nodes;i++)
		for(int j=0;j<100;j++)
			sch[i][j] = 0x00;
	
	u8 string_idx=0;
	u8 key_idx = 0;
	u16 private_key_idx;
	u16 public_key_idx;
	for(u8 l=1;l<=Nodes_N+1;l++){
		key_idx = 0;
		for(string_idx = 0; string_idx < sizeof(thres_public_key_string[l-1]);){
			vch[l][key_idx] = Sixteen2Ten(thres_public_key_string[l-1][string_idx])*16+Sixteen2Ten(thres_public_key_string[l-1][string_idx + 1]);
			key_idx = key_idx + 1;
			string_idx = string_idx + 3;
		}
		public_key_idx = key_idx;
		
		key_idx = 0;
		for(string_idx = 0; string_idx < sizeof(thres_private_key_string[l-1]);){
			sch[l][key_idx] = Sixteen2Ten(thres_private_key_string[l-1][string_idx])*16 + Sixteen2Ten(thres_private_key_string[l-1][string_idx + 1]);
			key_idx = key_idx + 1;
			string_idx = string_idx + 3;
		}
		private_key_idx = key_idx;
		
	}
	
	octet RAW[MAX_Nodes];
	for(int i=0;i<MAX_Nodes;i++){
		RAW[i].max = 200;
		RAW[i].len = public_key_idx;
		RAW[i].val = vch[i];
	}
	
	
	for(int i=1;i<=players;i++){
		BIG_160_28_fromBytes(SKs[i],sch[i]);
	}
	for(int i=1;i<=players;i++){
		ECP2_BN158_fromOctet(&VKs[i],&RAW[i]);
	}
	ECP2_BN158_fromOctet(&VK,&RAW[Nodes_N+1]);
	
	for(u8 i=0;i<10;i++)S[i] = i;


}

//PRBC
void PRBC_SK_sign(int prbc_idx){
	//h ** sk
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char PRBC_Str[32];for(int i=0;i<32;i++)PRBC_Str[i] = NULL;
	sprintf((char*)PRBC_Str,"PRBC:%d",prbc_idx);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;PRBC_Str[i]!=0;i++) HASH256_process(&sh256,PRBC_Str[i]);
    HASH256_hash(&sh256,digest);

	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 h;
	ECP_BN158_copy(&h,&C_point);//h
	PAIR_BN158_G1mul(&h,SKs[ID]);
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = ch;
	
	ECP_BN158_toOctet(&Obj,&h,true);
	if(Obj.len != Share_Size){
		while(1){
			sprintf((char*)Send_Data_buff,"Sign Thres Fail");
			Lora_Device_Sta = LORA_TX_STA;
			LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
		}
	}
	
	for(int i=0;i<Obj.len;i++){
		PRBC_share[prbc_idx][ID][i] = Obj.val[i];
	}
	
}

int PRBC_PK_verify_sig_share(u8 id, u8 prbc_idx, u8 *sig){

//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
	
//	B = self.VKs[i]
//	assert pair(g2, sig) == pair(B, h)
//	return True
	ECP2_BN158 B;
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char PRBC_Str[32];for(int i=0;i<32;i++)PRBC_Str[i] = NULL;
	sprintf((char*)PRBC_Str,"PRBC:%d",prbc_idx);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;PRBC_Str[i]!=0;i++) HASH256_process(&sh256,PRBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 share_point;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = (char*)sig;
	ECP_BN158_fromOctet(&share_point,&Obj);
	
	ECP2_BN158_copy(&B,&VKs[id]);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&B,&C_point);
	PAIR_BN158_fexp(&v);
	
	if (FP12_BN158_isunity(&v)){
		return 1;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: Yes ");
		LoRa_SendData(Send_Data_buff);
	}else{
		u8 x = 1;
		x++;
		return 0;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: No ");
		LoRa_SendData(Send_Data_buff);
	}
}


void PRBC_PK_sig_combine_share(u8 prbc_idx){
	//	def combine_shares(self, (U,V,W), shares):
	//	# sigs: a mapping from idx -> sig
	//	S = set(shares.keys())
	//	assert S.issubset(range(self.l))

	//	# ASSUMPTION
	//	# assert self.verify_ciphertext((U,V,W))

	//	mul = lambda a,b: a*b
	//	res = reduce(mul, [sig ** self.lagrange(S, j) for j,sig in sigs.iteritems()], 1)
    //  return res
	
	//init S
	u8 idx = 0;
	ECP_BN158 PRBC_Sigs[Nodes_N];
	octet Obj[MAX_Nodes];
	for(u8 i=0;i<MAX_Nodes;i++){
		Obj[i].len = Share_Size;
		Obj[i].max = Share_Size;
		Obj[i].val = (char*)PRBC_share[prbc_idx][i];
	}
	for(u8 i=1;i<=Nodes_N;i++){
		if(ACK_PRBC_share[prbc_idx][i]){
			S[idx] = i-1;
			ECP_BN158_fromOctet(&PRBC_Sigs[i-1],&Obj[i]);
			idx++;
		}
//		if(ACK_PRBC_share[prbc_idx][i] == 0 && i <= Thres_k){
//			PRBC_SK_sign_1(i,prbc_idx);
//			S[idx] = i-1;
//			ECP_BN158_fromOctet(&PRBC_Sigs[i-1],&Obj[i]);
//			idx++;
//		}
	}
	
	BIG_160_28 larg;
	ECP_BN158 tmp;
	ECP_BN158 r[20];
	//ECP_BN158 res;
	
	for(int j=0 ; j<Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = S[j];
		lagrange(larg,jj);
		ECP_BN158_copy(&tmp,&PRBC_Sigs[jj]);
		PAIR_BN158_G1mul(&tmp,larg);
		ECP_BN158_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Thres_k ; j++){
		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
//	ECP_BN158_copy(&SIG,&r[0]);	
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet OBJ;
	OBJ.len = 0;
	OBJ.max = 70;
	OBJ.val = ch;
	
	ECP_BN158_toOctet(&OBJ,&r[0],true);
	
	for(int i=0;i<OBJ.len;i++)
		PRBC_sig[prbc_idx][i] = OBJ.val[i];
}
int PRBC_PK_verify_sig(u8 prbc_idx, u8 sig[]){
	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
	
//	assert pair(sig, g2) == pair(h, self.VK)
//  return True
	
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char PRBC_Str[32];for(int i=0;i<32;i++)PRBC_Str[i] = NULL;
	sprintf((char*)PRBC_Str,"PRBC:%d",prbc_idx);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;PRBC_Str[i]!=0;i++) HASH256_process(&sh256,PRBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 share_point;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = (char*)sig;
	ECP_BN158_fromOctet(&share_point,&Obj);
	
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&VK,&C_point);
	PAIR_BN158_fexp(&v);
	
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	
	if (FP12_BN158_isunity(&v)){
		return 1;
		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		return 0;
		sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
		LoRa_SendData(Send_Data_buff);
	}
}


//ABA coin
//void Coin_SK_sign(int aba,int round){
//	if(ACK_ABA_share[aba][round][ID])return;
//	//h ** sk
//	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
//	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
//	sprintf((char*)Coin_Str,"Coin:%d%d",aba,round);
//	hash256 sh256;
//	HASH256_init(&sh256);
//	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
//    HASH256_hash(&sh256,digest);
//	
//	ECP_BN158 C_point;
//	BIG_160_28 C_num;
//	BIG_160_28_fromBytes(C_num,digest);
//	ECP_BN158_copy(&C_point,&G1);
//	PAIR_BN158_G1mul(&C_point,C_num);

//	ECP_BN158 h;
//	ECP_BN158_copy(&h,&C_point);//h
//	PAIR_BN158_G1mul(&h,SKs[ID]);
//	
//	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
//	octet Obj;
//	Obj.len = 0;
//	Obj.max = 70;
//	Obj.val = ch;
//	
//	ECP_BN158_toOctet(&Obj,&h,true);
//	if(Obj.len != Share_Size){while(1);}
//	for(int i=0;i<Obj.len;i++)
//		ABA_share[aba][round][ID][i] = Obj.val[i];
//	if(!ACK_ABA_share[aba][round][ID])ABA_share_Number[aba][round]++;
//	ACK_ABA_share[aba][round][ID] = 1;
//	
////	BIG_160_28 num;
////	for(int i=0;i<players;i++){
////		FP_BN158_redc(num,&SKs[i]);
////		ECP_BN158_copy(&Sigs[i],&M_point);
////		PAIR_BN158_G1mul(&Sigs[i],num);
////	}
//	
//	
//}

//int Coin_PK_verify_sig_share(u8 id, u8 aba, u8 round, u8 *sig){

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
//	
//	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
//	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
//	sprintf((char*)Coin_Str,"Coin:%d%d",aba,round);
//	hash256 sh256;
//	HASH256_init(&sh256);
//	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
//    HASH256_hash(&sh256,digest);
//	
//	ECP_BN158 C_point;
//	BIG_160_28 C_num;
//	BIG_160_28_fromBytes(C_num,digest);
//	ECP_BN158_copy(&C_point,&G1);
//	PAIR_BN158_G1mul(&C_point,C_num);
//	
//	ECP_BN158 share_point;
//	octet Obj;
//	Obj.len = 0;
//	Obj.max = 70;
//	Obj.val = (char*)sig;
//	ECP_BN158_fromOctet(&share_point,&Obj);
//	
//	ECP2_BN158_copy(&B,&VKs[id]);
//	FP12_BN158 v;
//	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&B,&C_point);
//	PAIR_BN158_fexp(&v);
//	
//	if (FP12_BN158_isunity(&v)){
//		return 1;
//		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: Yes ");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		return 0;
//		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: No ");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	
//}


//void Coin_PK_sig_combine_share(u8 aba, u8 round){
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
//	//init S
//	u8 idx = 0;
//	ECP_BN158 Coin_Sigs[Nodes_N];
//	octet Obj[MAX_Nodes];
//	for(u8 i=0;i<MAX_Nodes;i++){
//		Obj[i].len = Share_Size;
//		Obj[i].max = Share_Size;
//		Obj[i].val = (char*)ABA_share[aba][round][i];
//	}
//	for(u8 i=1;i<=Nodes_N;i++){
//		if(ACK_ABA_share[aba][round][i]){
//			S[idx] = i-1;
//			ECP_BN158_fromOctet(&Coin_Sigs[i-1],&Obj[i]);
//			idx++;
//		}
////		if(ACK_ABA_share[aba][round][i] == 0 && i <= Thres_k){
////			tmp_Coin_SK_sign(i,aba,round);
////			S[idx] = i-1;
////			ECP_BN158_fromOctet(&Coin_Sigs[i-1],&Obj[i]);
////			idx++;
////		}
//	}
//	

//	BIG_160_28 larg;
//	ECP_BN158 tmp;
//	ECP_BN158 r[20];
//	//ECP_BN158 res;
//	
//	for(int j=0 ; j<Thres_k ; j++){
////		share ** self.lagrange(S, j)
//		u8 jj = S[j];
//		lagrange(larg,jj);
//		ECP_BN158_copy(&tmp,&Coin_Sigs[jj]);
//		PAIR_BN158_G1mul(&tmp,larg);
//		ECP_BN158_copy(&r[j],&tmp);	
//	}
//	
//	for(int j=1 ; j<Thres_k ; j++){
//		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
//	}
////	ECP_BN158_copy(&SIG,&r[0]);	
//	
//	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
//	octet OBJ;
//	OBJ.len = 0;
//	OBJ.max = 70;
//	OBJ.val = ch;
//	
//	ECP_BN158_toOctet(&OBJ,&r[0],true);
//	Shared_Coin[aba][round] = OBJ.val[OBJ.len-1] % 2;
//	for(int i=0;i<OBJ.len;i++)
//		COIN[i] = OBJ.val[i];
//	
//	//verify the coin
//	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
//	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
//	sprintf((char*)Coin_Str,"Coin:%d%d",aba,round);
//	hash256 sh256;
//	HASH256_init(&sh256);
//	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
//    HASH256_hash(&sh256,digest);
//	
//	//for(int i=0;i<50;i++)M[i]=digest[i];
//	ECP_BN158 C_point;
//	BIG_160_28 C_num;
//	BIG_160_28_fromBytes(C_num,digest);
//	ECP_BN158_copy(&C_point,&G1);
//	PAIR_BN158_G1mul(&C_point,C_num);

//	ECP2_BN158 g2_neg;
//	ECP2_BN158_copy(&g2_neg,&g2);
//	ECP2_BN158_neg(&g2_neg);
//	FP12_BN158 v;
//	PAIR_BN158_double_ate(&v,&g2_neg,&r[0],&VK,&C_point);
//	PAIR_BN158_fexp(&v);
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
////	LoRa_SendData(Send_Data_buff);
//	
//	
//	if (FP12_BN158_isunity(&v)){
////		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
////		LoRa_SendData(Send_Data_buff);
//	}else{
//		while(1){
//			sprintf((char*)Send_Data_buff,"PK_verify_sig: No: round: %d", round);
//			LoRa_SendData(Send_Data_buff);
//		}
//		
//	}
//	
//}

void sign_data(u8* hash,u8* sig){
	while(1){
		if (!uECC_sign(private_key[ID], hash, 32, sig, uECC_secp160r1())) {
			sprintf((char*)Send_Data_buff,"Sign Fail");
			Lora_Device_Sta = LORA_TX_STA;
			LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
		}else{
			break;
		}
	}
}


//CBC
void CBC_test(){
	for(int i=0;i<CBC_v_SIZE;i++){
		CBC_v[ID][i] = 0x01;
	}
	
	for(int k=1;k<=Nodes_N;k++){
		CBC_SK_sign(k,CBC_v[ID],CBC_v_SIZE);
	}
	CBC_PK_sig_combine_share();
	
}

void CBC_SK_sign(u8 id, u8 str[],u8 size){
	// ID -> id
	//if(ACK_ABA_share[round][ID])return;
	//h ** sk
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char CBC_Str[100];for(int i=0;i<100;i++)CBC_Str[i] = NULL;
	for(int i=0;i<size;i++)CBC_Str[i] = str[i];
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i<size;i++) HASH256_process(&sh256,CBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	

	ECP_BN158 h;
	ECP_BN158_copy(&h,&C_point);//h
	PAIR_BN158_G1mul(&h,SKs[ID]);
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = ch;
	
	ECP_BN158_toOctet(&Obj,&h,true);
	if(Obj.len != Share_Size){
		while(1){
			sprintf((char*)Send_Data_buff,"Sign Thres Fail");
			Lora_Device_Sta = LORA_TX_STA;
			LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
		}
	}
	//add
	for(int i=0;i<Obj.len;i++)
		CBC_Echo_Share[id][i] = Obj.val[i];
	if(id == ID){
		for(int i=0;i<Obj.len;i++)
			My_CBC_Echo_Share[ID][i] = Obj.val[i];
		ACK_CBC_Echo[ID] = 1;
		My_CBC_Echo_Number++;
	}
	
//	for(int i=0;i<Obj.len;i++)
//		ABA_share[round][ID][i] = Obj.val[i];
//	if(!ACK_ABA_share[round][ID])ABA_share_Number[round]++;
//	ACK_ABA_share[round][ID] = 1;
}

int CBC_PK_verify_sig_share(u8 id, u8 str[],u8 size, u8 *sig){

//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
	
//	B = self.VKs[i]
//	assert pair(g2, sig) == pair(B, h)
//	return True
	ECP2_BN158 B;
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char CBC_Str[100];for(int i=0;i<100;i++)CBC_Str[i] = NULL;
	for(int i=0;i<size;i++)CBC_Str[i] = str[i];
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;CBC_Str[i]!=0;i++) HASH256_process(&sh256,CBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 share_point;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = (char*)sig;
	ECP_BN158_fromOctet(&share_point,&Obj);
	
	ECP2_BN158_copy(&B,&VKs[id]);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&B,&C_point);
	PAIR_BN158_fexp(&v);
	
	if (FP12_BN158_isunity(&v)){
		return 1;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: Yes ");
		LoRa_SendData(Send_Data_buff);
	}else{
		return 0;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: No ");
		LoRa_SendData(Send_Data_buff);
	}
	
}



void CBC_PK_sig_combine_share(){
	//	def combine_shares(self, (U,V,W), shares):
	//	# sigs: a mapping from idx -> sig
	//	S = set(shares.keys())
	//	assert S.issubset(range(self.l))

	//	# ASSUMPTION
	//	# assert self.verify_ciphertext((U,V,W))

	//	mul = lambda a,b: a*b
	//	res = reduce(mul, [sig ** self.lagrange(S, j) for j,sig in sigs.iteritems()], 1)
    //  return res
	
	//init S
	u8 idx = 0;
	ECP_BN158 CBC_Sigs[Nodes_N];
	octet Obj[MAX_Nodes];
	for(u8 i=0;i<MAX_Nodes;i++){
		Obj[i].len = Share_Size;
		Obj[i].max = Share_Size;
		Obj[i].val = (char*)My_CBC_Echo_Share[i];
	}
	
	for(u8 i=1;i<=Nodes_N;i++){
		if(ACK_CBC_Echo[i]){
			S[idx] = i-1;
			ECP_BN158_fromOctet(&CBC_Sigs[i-1],&Obj[i]);
			idx++;
		}
//		if(ACK_CBC_Echo[i]==0 && i<=Thres_k){
//			CBC_SK_sign_ID(i);
//			S[idx] = i-1;
//			ECP_BN158_fromOctet(&CBC_Sigs[i-1],&Obj[i]);
//			idx++;
//		}
	}
//	for(u8 i=0;i<MAX_Nodes;i++){
//		Obj[i].len = Share_Size;
//		Obj[i].max = Share_Size;
//		Obj[i].val = (char*)CBC_Echo_Share[i];
//	}
//	for(u8 i=1;i<=Nodes_N;i++){
//		if(i == 2 || i == 4 || i == 0){
//			S[idx] = i-1;
//			ECP_BN158_fromOctet(&CBC_Sigs[i-1],&Obj[i]);
//			idx++;
//		}
//	}
	BIG_160_28 larg;
	ECP_BN158 tmp;
	ECP_BN158 r[20];
	//ECP_BN158 res;
	for(int j=0 ; j<Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = S[j];
		lagrange(larg,jj);
		ECP_BN158_copy(&tmp,&CBC_Sigs[jj]);
		PAIR_BN158_G1mul(&tmp,larg);
		ECP_BN158_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Thres_k ; j++){
		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
//	ECP_BN158_copy(&SIG,&r[0]);	
	/////////////////////////////////
	
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char CBC_Str[100];for(int i=0;i<100;i++)CBC_Str[i] = NULL;
	for(int i=0;i<CBC_v_SIZE;i++)CBC_Str[i] = CBC_v[ID][i];
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;CBC_Str[i]!=0;i++) HASH256_process(&sh256,CBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet OBJ;
	OBJ.len = 0;
	OBJ.max = 70;
	OBJ.val = ch;
	
	ECP_BN158_toOctet(&OBJ,&r[0],true);
	
	for(int i=0;i<OBJ.len;i++)
		CBC_Finish_Share[ID][i] = ch[i];
	
	
//	ECP_BN158 share_point;
//	octet Obj;
//	Obj.len = 0;
//	Obj.max = 70;
//	Obj.val = (char*)sig;
//	ECP_BN158_fromOctet(&share_point,&Obj);
	
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&r[0],&VK,&C_point);
	PAIR_BN158_fexp(&v);
	
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	
	if (FP12_BN158_isunity(&v)){
		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
		LoRa_SendData(Send_Data_buff);
	}
	

}

int CBC_verify_sig(u8 str[],u8 size, u8 sig[]){
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char CBC_Str[100];for(int i=0;i<100;i++)CBC_Str[i] = NULL;
	for(int i=0;i<size;i++)CBC_Str[i] = str[i];
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;CBC_Str[i]!=0;i++) HASH256_process(&sh256,CBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 share_point;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = (char*)sig;
	ECP_BN158_fromOctet(&share_point,&Obj);
	
//	assert pair(sig, g2) == pair(h, self.VK)
//  return True
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&VK,&C_point);
	PAIR_BN158_fexp(&v);
	
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	
	if (FP12_BN158_isunity(&v)){
		return 1;
		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		return 0;
		sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
		LoRa_SendData(Send_Data_buff);
	}
	
}











//VAL_CBC
void VAL_CBC_test(){
//	for(int i=0;i<CBC_v_SIZE;i++){
//		CBC_v[ID][i] = 0x01;
//	}
	
//	for(int k=1;k<=Nodes_N;k++){
//		if(k != ID)
//		tmpVAL_CBC_SK_sign(k);
//	}
//	VAL_CBC_PK_sig_combine_share();
	
}

void VAL_CBC_SK_sign(u8 id){
	if(ACK_VAL_CBC_Number[id] != Nodes_N - Nodes_f){while(1);}
	// ID -> id
	//if(ACK_ABA_share[round][ID])return;
	//h ** sk
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char CBC_Str[300];for(int i=0;i<300;i++)CBC_Str[i] = NULL;
	u8 packet_idx = 0;
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_VAL_CBC[id][k]){
			for(int i=0;i<Share_Size;i++){
				CBC_Str[packet_idx] = VAL_CBC_v[id][k][i];
				packet_idx++;
			}
		}
		
	}
	//for(int i=0;i<size;i++)CBC_Str[i] = str[i];
	
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i < (Nodes_N-Nodes_f)*Share_Size;i++) HASH256_process(&sh256,CBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 h;
	ECP_BN158_copy(&h,&C_point);//h
	PAIR_BN158_G1mul(&h,SKs[ID]);
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = ch;
	
	ECP_BN158_toOctet(&Obj,&h,true);
	if(Obj.len != Share_Size){
		while(1){
			sprintf((char*)Send_Data_buff,"Sign Thres Fail");
			Lora_Device_Sta = LORA_TX_STA;
			LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
		}
	}
	//add
	for(int i=0;i<Obj.len;i++)
		VAL_CBC_Echo_Share[id][i] = Obj.val[i];
	VAL_CBC_Echo[id] = 1;
	
	if(id == ID){
		for(int i=0;i<Obj.len;i++)
			My_VAL_CBC_Echo_Share[ID][i] = Obj.val[i];
		ACK_VAL_CBC_Echo[ID] = 1;
		My_VAL_CBC_Echo_Number++;
	}
	
//	for(int i=0;i<Obj.len;i++)
//		ABA_share[round][ID][i] = Obj.val[i];
//	if(!ACK_ABA_share[round][ID])ABA_share_Number[round]++;
//	ACK_ABA_share[round][ID] = 1;
}

int VAL_CBC_PK_verify_sig_share(u8 id){
	if(ACK_VAL_CBC_Number[ID] != Nodes_N - Nodes_f){while(1);}
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
	
//	B = self.VKs[i]
//	assert pair(g2, sig) == pair(B, h)
//	return True
	ECP2_BN158 B;
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char CBC_Str[300];for(int i=0;i<300;i++)CBC_Str[i] = NULL;
	u8 packet_idx = 0;
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_VAL_CBC[ID][k]){
			for(int i=0;i<Share_Size;i++){
				CBC_Str[packet_idx] = VAL_CBC_v[ID][k][i];
				packet_idx++;
			}
		}
		
	}
	//for(int i=0;i<size;i++)CBC_Str[i] = str[i];
	
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i < (Nodes_N-Nodes_f)*Share_Size;i++) HASH256_process(&sh256,CBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 share_point;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = (char*)My_VAL_CBC_Echo_Share[id];
	ECP_BN158_fromOctet(&share_point,&Obj);
	
	ECP2_BN158_copy(&B,&VKs[id]);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&B,&C_point);
	PAIR_BN158_fexp(&v);
	
	if (FP12_BN158_isunity(&v)){
		return 1;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: Yes ");
		LoRa_SendData(Send_Data_buff);
	}else{
		return 0;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: No ");
		LoRa_SendData(Send_Data_buff);
	}
	
}
//void tmp_VAL_CBC_SK_sign(u8 id){
//	//
//	// ID -> id
//	//if(ACK_ABA_share[round][ID])return;
//	//h ** sk
//	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
//	char CBC_Str[300];for(int i=0;i<300;i++)CBC_Str[i] = NULL;
//	u8 packet_idx = 0;
//	for(int k=1;k<=Nodes_N;k++){
//		if(ACK_VAL_CBC[ID][k]){
//			for(int i=0;i<Share_Size;i++){
//				CBC_Str[packet_idx] = VAL_CBC_v[ID][k][i];
//				packet_idx++;
//			}
//		}
//		
//	}
//	//for(int i=0;i<size;i++)CBC_Str[i] = str[i];
//	
//	hash256 sh256;
//	HASH256_init(&sh256);
//	for (int i=0;i < (Nodes_N-Nodes_f)*Share_Size;i++) HASH256_process(&sh256,CBC_Str[i]);
//    HASH256_hash(&sh256,digest);
//	
//	//for(int i=0;i<50;i++)M[i]=digest[i];
//	ECP_BN158 C_point;
//	
//	FP_BN158 C_FP;
//	BIG_160_28 C_num;
//	FP_BN158_fromBytes(&C_FP,digest);
//	
//	if(sign_FP(&C_FP)){
//		// negative num
//		FP_BN158 C_FP_neg;
//		FP_BN158_neg(&C_FP_neg,&C_FP);
//		FP_BN158_redc(C_num,&C_FP);
//		ECP_BN158_copy(&C_point,&G1);
//		PAIR_BN158_G1mul(&C_point,C_num);
//		ECP_BN158_neg(&C_point);
//	}else{
//		FP_BN158_redc(C_num,&C_FP);
//		ECP_BN158_copy(&C_point,&G1);
//		PAIR_BN158_G1mul(&C_point,C_num);
//	}
//	
//	BIG_160_28 num;
//	ECP_BN158 h;
//	FP_BN158_redc(num,&SKs[id]);//sk
//	ECP_BN158_copy(&h,&C_point);//h
//	PAIR_BN158_G1mul(&h,num);
//	
//	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
//	octet Obj;
//	Obj.len = 0;
//	Obj.max = 70;
//	Obj.val = ch;
//	
//	ECP_BN158_toOctet(&Obj,&h,true);
//	if(Obj.len != Share_Size){
//		while(1){
//			sprintf((char*)Send_Data_buff,"Sign Thres Fail");
//			Lora_Device_Sta = LORA_TX_STA;
//			LoRa_SendData(Send_Data_buff);// id 0 "Hello LoRa"
//		}
//	}
//	//add
//	
//	for(int i=0;i<Obj.len;i++)
//		My_VAL_CBC_Echo_Share[id][i] = Obj.val[i];
//	ACK_VAL_CBC_Echo[id] = 1;
////	for(int i=0;i<Obj.len;i++)
////		VAL_CBC_Echo_Share[id][i] = Obj.val[i];
////	if(id == ID){
////		for(int i=0;i<Obj.len;i++)
////			My_VAL_CBC_Echo_Share[ID][i] = Obj.val[i];
////		ACK_VAL_CBC_Echo[ID] = 1;
////		My_VAL_CBC_Echo_Number++;
////	}
//}

void VAL_CBC_PK_sig_combine_share(){
	//	def combine_shares(self, (U,V,W), shares):
	//	# sigs: a mapping from idx -> sig
	//	S = set(shares.keys())
	//	assert S.issubset(range(self.l))

	//	# ASSUMPTION
	//	# assert self.verify_ciphertext((U,V,W))

	//	mul = lambda a,b: a*b
	//	res = reduce(mul, [sig ** self.lagrange(S, j) for j,sig in sigs.iteritems()], 1)
    //  return res
	
	//init S
	u8 idx = 0;
	ECP_BN158 CBC_Sigs[Nodes_N];
	octet Obj[MAX_Nodes];
	for(u8 i=0;i<MAX_Nodes;i++){
		Obj[i].len = Share_Size;
		Obj[i].max = Share_Size;
		Obj[i].val = (char*)My_VAL_CBC_Echo_Share[i];
	}
	
	for(u8 i=1;i<=Nodes_N;i++){
		if(ACK_VAL_CBC_Echo[i]){
			S[idx] = i-1;
			ECP_BN158_fromOctet(&CBC_Sigs[i-1],&Obj[i]);
			idx++;
		}else{
			//
//			tmp_VAL_CBC_SK_sign(i);
//			S[idx] = i-1;
//			ECP_BN158_fromOctet(&CBC_Sigs[i-1],&Obj[i]);
//			idx++;
		}
	}

	BIG_160_28 larg;
	ECP_BN158 tmp;
	ECP_BN158 r[20];
	//ECP_BN158 res;
	
	for(int j=0 ; j<Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = S[j];
		lagrange(larg,jj);
		ECP_BN158_copy(&tmp,&CBC_Sigs[jj]);
		PAIR_BN158_G1mul(&tmp,larg);
		ECP_BN158_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Thres_k ; j++){
		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
//	ECP_BN158_copy(&SIG,&r[0]);	
	/////////////////////////////////
	
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char CBC_Str[300];for(int i=0;i<300;i++)CBC_Str[i] = NULL;
	u8 packet_idx = 0;
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_VAL_CBC[ID][k]){
			for(int i=0;i<Share_Size;i++){
				CBC_Str[packet_idx] = VAL_CBC_v[ID][k][i];
				packet_idx++;
			}
		}
		
	}
	//for(int i=0;i<size;i++)CBC_Str[i] = str[i];
	
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i < (Nodes_N-Nodes_f)*Share_Size;i++) HASH256_process(&sh256,CBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet OBJ;
	OBJ.len = 0;
	OBJ.max = 70;
	OBJ.val = ch;
	
	ECP_BN158_toOctet(&OBJ,&r[0],true);
	
	for(int i=0;i<OBJ.len;i++)
		VAL_CBC_Finish_Share[ID][i] = ch[i];
	
	
//	ECP_BN158 share_point;
//	octet Obj;
//	Obj.len = 0;
//	Obj.max = 70;
//	Obj.val = (char*)sig;
//	ECP_BN158_fromOctet(&share_point,&Obj);
	
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&r[0],&VK,&C_point);
	PAIR_BN158_fexp(&v);
	
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	
	if (FP12_BN158_isunity(&v)){
		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		while(1){
			sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
			LoRa_SendData(Send_Data_buff);
		}
		
	}
	

}

int VAL_CBC_verify_sig(u8 id){
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char CBC_Str[300];for(int i=0;i<300;i++)CBC_Str[i] = NULL;
	u8 packet_idx = 0;
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_VAL_CBC[id][k]){
			for(int i=0;i<Share_Size;i++){
				CBC_Str[packet_idx] = VAL_CBC_v[id][k][i];
				packet_idx++;
			}
		}
		
	}
	//for(int i=0;i<size;i++)CBC_Str[i] = str[i];
	
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;i < (Nodes_N-Nodes_f)*Share_Size;i++) HASH256_process(&sh256,CBC_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN158 C_point;
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	ECP_BN158_copy(&C_point,&G1);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 share_point;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = (char*)VAL_CBC_Finish_Share[id];
	ECP_BN158_fromOctet(&share_point,&Obj);
	
//	assert pair(sig, g2) == pair(h, self.VK)
//  return True
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&share_point,&VK,&C_point);
	PAIR_BN158_fexp(&v);
	
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	
	if (FP12_BN158_isunity(&v)){
		return 1;
		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		return 0;
		sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
		LoRa_SendData(Send_Data_buff);
	}
	
}




//threshold signature
void ff(BIG_160_28 r,BIG_160_28 x){
	BIG_160_28 q;
	BIG_160_28_rcopy(q, CURVE_Order_BN158);
	BIG_160_28 y;
	BIG_160_28 xx;
	BIG_160_28_zero(y);
	BIG_160_28_one(xx);
	BIG_160_28 tmp;
	BIG_160_28 tmp1;
	for(u16 i=0;i<Thres_k;i++){
		
		//y+=coeff * xx
		BIG_160_28_modmul(tmp,a[i],xx, q);//tmp = a[i] * xx
		//BIG_160_28_norm(tmp);
		BIG_160_28_add(tmp1,y,tmp);//tmp1 = y + tmp
		//BIG_160_28_norm(tmp1);
		BIG_160_28_copy(y,tmp1);  //y = tmp1
		
		//xx*=x
		BIG_160_28_modmul(tmp,xx,x,q);//tmp = xx * x
		//BIG_160_28_norm(tmp);
		BIG_160_28_copy(xx,tmp);
		//BIG_160_28_norm(xx);
	}
	BIG_160_28_copy(r,y);
}

void lagrange(BIG_160_28 r,u8 j){
//	def lagrange(self, S, j):
//	# Assert S is a subset of range(0,self.l)
//	assert len(S) == self.k
//	assert type(S) is set
//	assert S.issubset(range(0,self.l))
//	S = sorted(S)

//	assert j in S
//	assert 0 <= j < self.l
//	mul = lambda a,b: a*b
//	num = reduce(mul, [0 - jj - 1 for jj in S if jj != j], ONE)
//	den = reduce(mul, [j - jj     for jj in S if jj != j], ONE)
//	return num / den
/******************************************************************/
	BIG_160_28 modl;
	BIG_160_28_rcopy(modl,CURVE_Order_BN158);
	
	//	num = reduce(mul, [0 - jj - 1 for jj in S if jj != j], ONE)
	BIG_160_28 num, ZERO, ONE, S_jj, tmp, tmp2, tmp3;
	BIG_160_28_one(num);
	BIG_160_28_zero(ZERO);
	BIG_160_28_one(ONE);
	
	BIG_160_28 tmp_neg;
	for(u8 jj=0;jj<Thres_k;jj++){
		if(S[jj] == j) continue;
		//num = num * (0 - S[jj] - 1);
		BIG_160_28_zero(S_jj);
		BIG_160_28_inc(S_jj,S[jj]);
		
		//BIG_160_28_sub(tmp,ZERO,S_jj);//tmp = 0 - S[jj]
		BIG_160_28_modneg(tmp_neg, S_jj, modl);
		BIG_160_28_modadd(tmp, ZERO, tmp_neg, modl);
		
		//BIG_160_28_sub(tmp2,tmp,ONE);//tmp2 = tmp - 1
		BIG_160_28_modneg(tmp_neg, ONE, modl);
		BIG_160_28_modadd(tmp2, tmp, tmp_neg, modl);
		
		BIG_160_28_modmul(tmp3, num, tmp2, modl);//tmp3 = num * tmp2
		BIG_160_28_copy(num,tmp3);	// num = tmp3 
	}
	BIG_160_28_norm(num);
	//	den = reduce(mul, [j - jj     for jj in S if jj != j], ONE)
	BIG_160_28 den, BIG_J;
	BIG_160_28_one(den);
	BIG_160_28_zero(BIG_J);
	BIG_160_28_inc(BIG_J,j);
	for(u8 jj=0;jj<Thres_k;jj++){
		if(S[jj] == j) continue;
		//den = den * (j - S[jj]);
		BIG_160_28_zero(S_jj);
		BIG_160_28_inc(S_jj,S[jj]);
		
		//BIG_160_28_sub(tmp,BIG_J,S_jj);//tmp = j - S[jj]
		BIG_160_28_modneg(tmp_neg, S_jj, modl);
		BIG_160_28_modadd(tmp, BIG_J, tmp_neg, modl);
		
		BIG_160_28_modmul(tmp2, den, tmp, modl);//tmp2 = den * tmp
		BIG_160_28_copy(den,tmp2);
	}
	BIG_160_28_norm(den);
	
	//return num/den
	
	BIG_160_28_moddiv(r, num, den, modl);
	
}
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
//	FP_BN158 secret;
//	
//	if(DEBUG){
//		FP_BN158_from_int(&secret,22);
//	}else{
//		FP_BN158_from_int(&secret,ran);
//	}
//	
//	FP_BN158_copy(&a[0],&secret);
//	FP_BN158_copy(&SECRET,&secret);

//	for(u16 i=1;i<Thres_k;i++){
//		
//		
//		if(DEBUG){
//			ran = RNG_Get_RandomNum();
//			FP_BN158_from_int(&a[i],88);
//		}else{
//			ran = RNG_Get_RandomNum();
//			FP_BN158_from_int(&a[i],ran);
//		}
//		
//		while(sign_FP(&a[i])){
//			ran = RNG_Get_RandomNum();
//			FP_BN158_from_int(&a[i],ran);
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
//		if(sign_FP(&larg)){
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










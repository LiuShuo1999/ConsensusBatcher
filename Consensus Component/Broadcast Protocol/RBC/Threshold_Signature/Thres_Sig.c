#include "Thres_Sig.h"
#include "RBC.h"

char thres_private_key_string[Nodes_N][key_num1] ={
"18 a6 ef 8a 93 ff 56 7c 22 27 f5 da 2d 43 f9 6a 89 f8 9e 6e a3 4b 65 a8 27 3f eb f0 98 4b 7d 3f 00 00 00 00 00 00 00 00", 
"0b 5d 45 fa 68 59 3e 56 59 d6 93 b6 4c 47 d5 24 a8 c2 c5 d6 8c aa 2b 11 8a 60 df 12 0d 58 db e1 00 00 00 00 00 00 00 00", 
"23 37 00 ec 7c b3 26 32 4b b9 7f 12 6b 4b b0 e6 c7 2c 6d 3e 76 08 f0 8b 8e 81 d2 33 82 66 3a 90 00 00 00 00 00 00 00 00", 
"15 ed 57 5c 51 0d 0e 0c 83 68 1c ee 8a 4f 8c a0 e5 f6 94 a6 5f 67 b5 f4 f1 a2 c5 54 f7 73 99 32 00 00 00 00 00 00 00 00"
};
char thres_public_key_string[Nodes_N+1][key_num2] = {
"03 21 02 6e 75 42 93 d8 2b 76 62 c2 a3 9c 8b 4a 68 4f 06 06 ec 79 3b d1 78 61 90 98 1f bf d5 98 04 14 3f b2 73 d0 4e 7d 71 00 b5 af 68 1b 47 58 63 01 a0 50 1c a1 f9 02 74 5f 49 a9 e0 d2 15 db 81", 
"02 17 e4 83 e1 ab e5 b9 a1 ef c6 b0 bd 05 e1 eb e2 95 ab f4 e5 86 c7 35 41 06 60 eb bc ac 8e 9e 1a 10 51 7d 3c 44 75 4e a5 7a 4e 2b c6 26 18 00 57 92 87 58 9b 79 70 7e e6 56 0d 81 fc 17 a5 e8 52", 
"03 02 1f a1 70 58 3f 9e 0e 83 f6 1f 60 14 f1 22 5c 5a d3 35 bd a4 cd 2b 42 ce 8d 7c 0c f6 d6 ab fc 16 31 ea 9d 09 5a 90 2a f5 55 d2 94 30 2c 10 ff b8 b8 de 15 8b 7d 27 d9 6d 27 61 eb 20 58 00 d1", 
"03 17 d5 13 73 71 1c 16 a3 08 41 d3 17 cd 22 5b 5b b7 5a be 7a bb f2 d0 6d 8f 8a 05 c3 1d 4e 26 55 0e ef f6 f8 3a 5d 88 c8 dd 88 33 3e a2 63 ba 55 b2 f9 ba 9a 24 a8 d2 9b 92 51 46 8c cc 09 b1 40", 
"03 00 0f 0f 78 14 aa 54 f3 0b 29 e1 44 79 29 65 b3 57 75 53 29 18 c9 3c c8 b2 55 bc 74 5e 91 46 fc 0a f9 91 70 dc c2 8b db 8b d0 45 c2 46 1b d4 1c e4 e5 84 f3 d0 26 a0 c2 53 3f f4 df a8 0b 6a 62"
};

char M[50];
//char M_[50];

u8 players = Nodes_N;
u8 Thres_k = Nodes_f + 1;

ECP_BN254 G1;
ECP2_BN254 G2;
ECP_BN254 g1;
ECP2_BN254 g2;

BIG_256_28 a[MAX_K];
u8 S[MAX_SK];
BIG_256_28 SECRET;
BIG_256_28 SKs[MAX_SK];

ECP2_BN254 g2;
ECP2_BN254 VK;
ECP2_BN254 VKs[MAX_SK];

ECP_BN254 M_point;
ECP_BN254 Sigs[MAX_SK];
ECP_BN254 SIG;

BIG_256_28 g1_num;
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
	BIG_256_28_fromBytes(g1_num,digest);
	
	if(!ECP_BN254_generator(&G1)){
		sprintf((char*)Send_Data_buff,"G1 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	
	ECP_BN254_copy(&g1,&G1);
	PAIR_BN254_G1mul(&g1,g1_num);
	//ECP_BN254_copy(&g2,&g1);
	
	if (!ECP2_BN254_generator(&G2)){
		sprintf((char*)Send_Data_buff,"G2 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	ECP2_BN254_copy(&g2,&G2);
	PAIR_BN254_G2mul(&g2,g1_num);
	
	char sch[MAX_Nodes][45];
	char vch[MAX_Nodes][65];
	for(int i=0;i<MAX_Nodes;i++)
		for(int j=0;j<65;j++){	
			vch[i][j] = 0x00;
		}
	for(int i=0;i<MAX_Nodes;i++)
		for(int j=0;j<45;j++)
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
		BIG_256_28_fromBytes(SKs[i],sch[i]);
	}
	for(int i=1;i<=players;i++){
		ECP2_BN254_fromOctet(&VKs[i],&RAW[i]);
	}
	ECP2_BN254_fromOctet(&VK,&RAW[Nodes_N+1]);
	
	for(u8 i=0;i<10;i++)S[i] = i;

}
//ABA coin
void Coin_SK_sign(int round){
	if(ACK_ABA_share[round][ID])return;
	//h ** sk
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
	sprintf((char*)Coin_Str,"Coin:%d%d",round,Block[1][0][0]);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN254 C_point;
	BIG_256_28 C_num;
	BIG_256_28_fromBytes(C_num,digest);
	ECP_BN254_copy(&C_point,&G1);
	PAIR_BN254_G1mul(&C_point,C_num);
	

	ECP_BN254 h;
								//sk
	ECP_BN254_copy(&h,&C_point);//h
	PAIR_BN254_G1mul(&h,SKs[ID]);
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = ch;
	
	ECP_BN254_toOctet(&Obj,&h,true);
	if(Obj.len != Share_Size){while(1);}
	for(int i=0;i<Obj.len;i++)
		ABA_share[round][ID][i] = Obj.val[i];
	if(!ACK_ABA_share[round][ID])ABA_share_Number[round]++;
	ACK_ABA_share[round][ID] = 1;
	
//	BIG_256_28 num;
//	for(int i=0;i<players;i++){
//		FP_BN254_redc(num,&SKs[i]);
//		ECP_BN254_copy(&Sigs[i],&M_point);
//		PAIR_BN254_G1mul(&Sigs[i],num);
//	}
	
	
}

void tmp_Coin_SK_sign(u8 id, int round){
	
}
int Coin_PK_verify_sig_share(u8 id, u8 round, u8 *sig){

//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
	
//	B = self.VKs[i]
//	assert pair(g2, sig) == pair(B, h)
//	return True
	ECP2_BN254 B;
	ECP2_BN254 g2_neg;
	ECP2_BN254_copy(&g2_neg,&g2);
	ECP2_BN254_neg(&g2_neg);
	
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
	sprintf((char*)Coin_Str,"Coin:%d%d",round,Block[1][0][0]);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
    HASH256_hash(&sh256,digest);
	
	ECP_BN254 C_point;
	BIG_256_28 C_num;
	BIG_256_28_fromBytes(C_num,digest);
	ECP_BN254_copy(&C_point,&G1);
	PAIR_BN254_G1mul(&C_point,C_num);
	
	ECP_BN254 share_point;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = (char*)sig;
	ECP_BN254_fromOctet(&share_point,&Obj);
	
	ECP2_BN254_copy(&B,&VKs[id]);
	FP12_BN254 v;
	PAIR_BN254_double_ate(&v,&g2_neg,&share_point,&B,&C_point);
	PAIR_BN254_fexp(&v);
	
	if (FP12_BN254_isunity(&v)){
		return 1;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: Yes ");
		LoRa_SendData(Send_Data_buff);
	}else{
		return 0;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: No ");
		LoRa_SendData(Send_Data_buff);
	}
	
	
}


void Coin_PK_sig_combine_share(u8 round){
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
	ECP_BN254 Coin_Sigs[Nodes_N];
	octet Obj[MAX_Nodes];
	for(u8 i=0;i<MAX_Nodes;i++){
		Obj[i].len = Share_Size;
		Obj[i].max = Share_Size;
		Obj[i].val = (char*)ABA_share[round][i];
	}
	for(u8 i=1;i<=Nodes_N;i++){
		if(ACK_ABA_share[round][i]){
			S[idx] = i-1;
			ECP_BN254_fromOctet(&Coin_Sigs[i-1],&Obj[i]);
			idx++;
		}
	}
	
	
	
	BIG_256_28 larg;
	ECP_BN254 tmp;
	ECP_BN254 r[20];
	
	for(int j=0 ; j<Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = S[j];
		lagrange(larg,jj);
		ECP_BN254_copy(&tmp,&Coin_Sigs[jj]);
		PAIR_BN254_G1mul(&tmp,larg);
		ECP_BN254_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Thres_k ; j++){
		ECP_BN254_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
//	ECP_BN254_copy(&SIG,&r[0]);	
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet OBJ;
	OBJ.len = 0;
	OBJ.max = 70;
	OBJ.val = ch;
	
	ECP_BN254_toOctet(&OBJ,&r[0],true);
	Shared_Coin[round] = OBJ.val[OBJ.len-1] % 2;
	for(int i=0;i<OBJ.len;i++)
		COIN[i] = OBJ.val[i];
	
	//verify the coin
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
	sprintf((char*)Coin_Str,"Coin:%d%d",round,Block[1][0][0]);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
    HASH256_hash(&sh256,digest);

	ECP_BN254 C_point;
	BIG_256_28 C_num;
	BIG_256_28_fromBytes(C_num,digest);
	ECP_BN254_copy(&C_point,&G1);
	PAIR_BN254_G1mul(&C_point,C_num);

	
	ECP2_BN254 g2_neg;
	ECP2_BN254_copy(&g2_neg,&g2);
	ECP2_BN254_neg(&g2_neg);
	FP12_BN254 v;
	PAIR_BN254_double_ate(&v,&g2_neg,&r[0],&VK,&C_point);
	PAIR_BN254_fexp(&v);
	
	if (FP12_BN254_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
//		LoRa_SendData(Send_Data_buff);
	}else{
		while(1){
			sprintf((char*)Send_Data_buff,"PK_verify_sig: No: round: %d", round);
			LoRa_SendData(Send_Data_buff);
		}
		
	}
	
}

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

//threshold signature
void ff(BIG_256_28 r,BIG_256_28 x){
	BIG_256_28 q;
	BIG_256_28_rcopy(q, CURVE_Order_BN254);
	BIG_256_28 y;
	BIG_256_28 xx;
	BIG_256_28_zero(y);
	BIG_256_28_one(xx);
	BIG_256_28 tmp;
	BIG_256_28 tmp1;
	for(u16 i=0;i<Thres_k;i++){
		
		//y+=coeff * xx
		BIG_256_28_modmul(tmp,a[i],xx, q);//tmp = a[i] * xx
		//BIG_256_28_norm(tmp);
		BIG_256_28_add(tmp1,y,tmp);//tmp1 = y + tmp
		//BIG_256_28_norm(tmp1);
		BIG_256_28_copy(y,tmp1);  //y = tmp1
		
		//xx*=x
		BIG_256_28_modmul(tmp,xx,x,q);//tmp = xx * x
		//BIG_256_28_norm(tmp);
		BIG_256_28_copy(xx,tmp);
		//BIG_256_28_norm(xx);
	}
	BIG_256_28_copy(r,y);
}

void lagrange(BIG_256_28 r,u8 j){
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
	BIG_256_28 modl;
	BIG_256_28_rcopy(modl,CURVE_Order_BN254);
	
	//	num = reduce(mul, [0 - jj - 1 for jj in S if jj != j], ONE)
	BIG_256_28 num, ZERO, ONE, S_jj, tmp, tmp2, tmp3;
	BIG_256_28_one(num);
	BIG_256_28_zero(ZERO);
	BIG_256_28_one(ONE);
	
	BIG_256_28 tmp_neg;
	for(u8 jj=0;jj<Thres_k;jj++){
		if(S[jj] == j) continue;
		//num = num * (0 - S[jj] - 1);
		BIG_256_28_zero(S_jj);
		BIG_256_28_inc(S_jj,S[jj]);
		
		//BIG_256_28_sub(tmp,ZERO,S_jj);//tmp = 0 - S[jj]
		BIG_256_28_modneg(tmp_neg, S_jj, modl);
		BIG_256_28_modadd(tmp, ZERO, tmp_neg, modl);
		
		//BIG_256_28_sub(tmp2,tmp,ONE);//tmp2 = tmp - 1
		BIG_256_28_modneg(tmp_neg, ONE, modl);
		BIG_256_28_modadd(tmp2, tmp, tmp_neg, modl);
		
		BIG_256_28_modmul(tmp3, num, tmp2, modl);//tmp3 = num * tmp2
		BIG_256_28_copy(num,tmp3);	// num = tmp3 
	}
	BIG_256_28_norm(num);
	//	den = reduce(mul, [j - jj     for jj in S if jj != j], ONE)
	BIG_256_28 den, BIG_J;
	BIG_256_28_one(den);
	BIG_256_28_zero(BIG_J);
	BIG_256_28_inc(BIG_J,j);
	for(u8 jj=0;jj<Thres_k;jj++){
		if(S[jj] == j) continue;
		//den = den * (j - S[jj]);
		BIG_256_28_zero(S_jj);
		BIG_256_28_inc(S_jj,S[jj]);
		
		//BIG_256_28_sub(tmp,BIG_J,S_jj);//tmp = j - S[jj]
		BIG_256_28_modneg(tmp_neg, S_jj, modl);
		BIG_256_28_modadd(tmp, BIG_J, tmp_neg, modl);
		
		BIG_256_28_modmul(tmp2, den, tmp, modl);//tmp2 = den * tmp
		BIG_256_28_copy(den,tmp2);
	}
	BIG_256_28_norm(den);
	
	//return num/den
	
	BIG_256_28_moddiv(r, num, den, modl);
	
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
//	
//	FP_BN254 secret;
//	
//	if(DEBUG){
//		FP_BN254_from_int(&secret,22);
//	}else{
//		FP_BN254_rand(&secret,&RNGx);
//	}
//	
//	
////	while(FP_BN254_sign(&secret)){
////		FP_BN254_rand(&secret,&RNGx);
////	}
//	ran = RNG_Get_RandomNum();
//	FP_BN254_from_int(&secret,ran);
//	FP_BN254_copy(&a[0],&secret);
//	FP_BN254_copy(&SECRET,&secret);

//	for(u16 i=1;i<Thres_k;i++){
//		
//		//FP_BN254_rand(&a[i],&RNGx);
//		if(DEBUG){
//			ran = RNG_Get_RandomNum();
//			FP_BN254_from_int(&a[i],88);
//		}else{
//			ran = RNG_Get_RandomNum();
//			FP_BN254_from_int(&a[i],ran);
//		}
//		
//		while(sign_FP(&a[i])){
//			ran = RNG_Get_RandomNum();
//			FP_BN254_from_int(&a[i],ran);
//			//FP_BN254_rand(&a[i],&RNGx);
//		}

//	}
//	
//	for(u16 i=0;i<players;i++){
//		FP_BN254 x;
//		FP_BN254_from_int(&x,i+1);
//		ff(&SKs[i],&x);
//	}
//	
//	//assert ff(0) == secret
//	FP_BN254 secret1;
//	FP_BN254 ZERO;
//	FP_BN254_zero(&ZERO);
//	ff(&secret1,&ZERO);
//	if(FP_BN254_equals(&secret1,&secret)){
//		sprintf((char*)Send_Data_buff,"hahaha");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"wuwuwu");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	//VK = g2 ** secret
//	ECP2_BN254_copy(&VK,&g2);
//	BIG_256_28 FP_num;
//	FP_BN254_redc(FP_num,&secret);
//	PAIR_BN254_G2mul(&VK,FP_num);
//	
//	//VKs = [g2 ** xx for xx in SKs ]
//	for(u16 i=0 ; i<players ; i++){
//		ECP2_BN254_copy(&VKs[i],&g2);
//		FP_BN254_redc(FP_num,&SKs[i]);
//		PAIR_BN254_G2mul(&VKs[i],FP_num);	
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
//	FP_BN254 lhs,rhs,tmp,tmp1,tmp2,tmp3,tmp4;
////	FP_BN254 ZERO;
//	FP_BN254_zero(&ZERO);
//	FP_BN254_zero(&rhs);
//	ff(&lhs,&ZERO);
//	
////  rhs = sum(public_key.lagrange(S,j) * f(j+1) for j in S)
//	for(u8 i=0;i<10;i++)S[i] = i;
//	
//	for(u16 i=0;i<Thres_k;i++){
//		FP_BN254_from_int(&tmp2,S[i]+1);
//		ff(&tmp,&tmp2);//tmp = ff[S[i]+1]
//		lagrange(&tmp1,S[i]);//tmp1 = lagrange
//		FP_BN254_mul(&tmp3,&tmp1,&tmp);//tmp3 = tmp1 * tmp
//		FP_BN254_add(&tmp4,&tmp3,&rhs);//tmp4 = tmp3 + rhs
//		FP_BN254_copy(&rhs,&tmp4);//rhs = tmp4
//	}
//	
//	BIG_256_28 lhs_num,rhs_num;
//	
//	FP_BN254_redc(lhs_num,&lhs);
//	FP_BN254_redc(rhs_num,&rhs);
//	
//	if(FP_BN254_equals(&lhs,&rhs) && BIG_256_28_comp(lhs_num,rhs_num)==0){
//		sprintf((char*)Send_Data_buff,"Reconstruction: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Reconstruction: NO");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//}




//void SK_sign(){
////	TIM9_Exceed_Times = 0;
////	TIM9->CNT = 0;
//	
//	//h ** sk
//	BIG_256_28 num;
//	for(int i=0;i<players;i++){
//		FP_BN254_redc(num,&SKs[i]);
//		ECP_BN254_copy(&Sigs[i],&M_point);
//		PAIR_BN254_G1mul(&Sigs[i],num);
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
//	ECP2_BN254 B;
//	ECP2_BN254 g2_neg;
//	ECP2_BN254_copy(&g2_neg,&g2);
//	ECP2_BN254_neg(&g2_neg);
//	for(int i=0;i<players;i++){
////		TIM2_Exceed_Times = 0;
////		TIM2->CNT = 0;
//		
//		ECP2_BN254_copy(&B,&VKs[i]);
//		FP12_BN254 v;
//		PAIR_BN254_double_ate(&v,&g2_neg,&Sigs[i],&B,&M_point);
//		PAIR_BN254_fexp(&v);
//		
////		u32 timex = TIM2->CNT + TIM2_Exceed_Times * 9000;
////		
//		if (FP12_BN254_isunity(&v)){
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
//	ECP2_BN254 g2_neg;
//	ECP2_BN254_copy(&g2_neg,&g2);
//	ECP2_BN254_neg(&g2_neg);
//	FP12_BN254 v;
//	PAIR_BN254_double_ate(&v,&g2_neg,&SIG,&VK,&M_point);
//	PAIR_BN254_fexp(&v);
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
////	LoRa_SendData(Send_Data_buff);
//	
//	
//	if (FP12_BN254_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//}

//void PK_sig_combine_share(){
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
	
	//	def combine_shares(self, (U,V,W), shares):
	//	# sigs: a mapping from idx -> sig
	//	S = set(shares.keys())
	//	assert S.issubset(range(self.l))

	//	# ASSUMPTION
	//	# assert self.verify_ciphertext((U,V,W))

	//	mul = lambda a,b: a*b
	//	res = reduce(mul, [sig ** self.lagrange(S, j) for j,sig in sigs.iteritems()], 1)
    //  return res
	
	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	FP_BN254 larg;
//	BIG_256_28 larg_num;
//	ECP_BN254 tmp;
//	ECP_BN254 r[20];
//	//ECP_BN254 res;
//	
//	for(int j=0 ; j<Thres_k ; j++){
////		share ** self.lagrange(S, j)
//		lagrange(&larg,j);
//		FP_BN254_redc(larg_num,&larg);
//		ECP_BN254_copy(&tmp,&Sigs[j]);
//		
//		if(sign_FP(&larg)){
//			// negative
//			FP_BN254 larg_tmp;
//			BIG_256_28 larg_num_tmp;
//			FP_BN254_neg(&larg_tmp,&larg);
//			FP_BN254_redc(larg_num_tmp,&larg_tmp);
//			PAIR_BN254_G1mul(&tmp,larg_num_tmp);
//			ECP_BN254_neg(&tmp);
//		}else{
//			FP_BN254_redc(larg_num,&larg);
//			PAIR_BN254_G1mul(&tmp,larg_num);
//		}
//		
//		ECP_BN254_copy(&r[j],&tmp);	
//	}
//	
//	for(int j=1 ; j<Thres_k ; j++){
//		ECP_BN254_add(&r[0],&r[j]);//这里可能不是Group元素了！
//	}
//	ECP_BN254_copy(&SIG,&r[0]);	
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_sig_combine_share_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"PK_sig_combine_share:%d",time);
////	LoRa_SendData(Send_Data_buff);

//}


#include "mw1268_app.h"
#include "mw1268_cfg.h"
#include "usart3.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "ThresholdCrypto.h"
#include "core.h"
#include "bls_BN158.h"

char thres_enc_private_key_string[4][119] ={
"34 2d f4 ad 8b 0e 7c 1f 24 c9 81 25 23 2d a7 26 7e 63 bc 90 a4 1b 0e 4a a3 ba e1 7d 58 f5 1b 44 00 00 00 00 00 00 00 00", 
"31 d5 a8 c2 fe 3d 6e ed a1 d8 70 08 2f 30 53 f9 b9 9e 0a 88 4e 33 ca 5c 08 fc 8e 41 cf 2b 16 17 00 00 00 00 00 00 00 00", 
"24 18 1f 8a 8e 3b 96 af b1 b1 18 44 30 05 aa 78 67 2a c6 91 21 c2 31 52 7f 24 07 c6 b4 09 f9 42 00 00 00 00 00 00 00 00", 
"30 18 bd 86 7b 08 f3 67 0e 87 c7 59 25 ad aa aa 86 a9 70 ab 1e c6 43 3e a7 31 4e 0c 07 91 c4 d2 00 00 00 00 00 00 00 00"
};
char thres_enc_public_key_string[5][195] = {
"02 06 ca 35 33 9d 6c 3a f1 bb 27 15 a9 d7 fc e4 aa 68 5f ba 45 be 77 07 39 fd 64 2f 51 d2 4e 70 93 14 57 cc c3 a5 25 af 22 62 82 fc 5f dd a4 2a 32 1f 5c ee c2 3b e1 82 24 40 4e 2a af 83 37 b8 94", 
"03 17 4e 93 99 9e 18 07 4a 72 f7 96 0e 20 1f be e8 9d 61 cf a2 9c a8 be 29 89 f7 7b c1 9c 3f dc 51 06 22 0d 92 d8 a5 00 d6 41 17 89 8b 74 ba 8c 26 10 53 f6 08 d0 cf 2c eb 14 92 a5 31 f5 3c 04 6b", 
"02 1c 4d c4 a8 b3 7a c5 72 05 d8 5e 11 2e b3 f8 e6 10 8c 23 36 f3 6b cc cd e0 ec 9c 0a aa 53 07 e9 05 d9 4b 63 58 62 bf 8a a6 77 07 b7 94 56 05 3c f4 62 f8 aa 6c 68 c3 e4 dd 66 0f 8c c3 32 49 e3", 
"03 17 96 de 23 28 06 c1 98 93 4f ac 86 a1 c3 23 e1 2a a1 ba 87 17 6e c4 ee 20 98 3d 44 22 36 ba 47 11 ed c7 9e ca b7 a4 89 d7 7a 01 5c 95 c6 2e 1d 80 d2 22 4a b3 0f d1 e4 18 52 76 bf c1 a2 4a 37", 
"02 04 71 ef 74 ca b4 4a ad ba 24 cf d4 8f 30 12 dc 21 02 38 9e a7 94 4d 18 cb e1 6e 53 5f a4 db 47 0a ce a5 76 ca 5b af 28 69 21 da 96 c9 9b 12 be 62 32 47 50 7a dc 39 0e 11 91 b2 38 53 93 68 6a"
};

#define MAX_K 20
#define MAX_SK 20
/* Field GF(2^8)  Threshold Secret Sharing*/

//u32 TIM5_Exceed_Times;
u32 TIM9_Exceed_Times;
//u32 TIM10_Exceed_Times;

////////定向传输
u32 obj_addr;//记录用户输入目标地址
u8 obj_chn;//记录用户输入目标信道
const struct uECC_Curve_t * curves[5];

u32 TIM2_Exceed_Times;
u8 Send_Data_buff[800] = {0};

char M[50];
char M_[50];

void init(){
	while(LoRa_Init())//初始化LORA模块
	{
		delay_ms(300);
	}
	LoRa_CFG.chn = 66;
	LoRa_CFG.tmode = LORA_TMODE_PT;
	LoRa_CFG.addr = 0;
	LoRa_CFG.power = LORA_TPW_9DBM;
	LoRa_CFG.wlrate = LORA_RATE_62K5;
	LoRa_CFG.lbt = LORA_LBT_ENABLE;
	LoRa_Set();
}

#include <stdio.h>
#include <time.h>

static char message[] = "This is a test message";


int Run_idx=0;
int dealer_time[100],PK_encrypt_time[100],PK_verify_ciphertext_time[100],SK_decrypt_share_time[100],PK_verify_share_time[100],PK_combine_share_time[100];
int SK_sign_time[100],PK_verify_sig_share_time[100],PK_verify_sig_time[100],PK_sig_combine_share_time[100];
int Share_Size[100];
/*************************Threshold Encryption****************************************/

BIG_160_28 a[MAX_K];
BIG_160_28 SKs[MAX_SK];
u8 players;
u8 k;
ECP_BN158 G1;
ECP2_BN158 G2;
ECP_BN158 g1;
ECP2_BN158 g2;
ECP2_BN158 VK;
ECP2_BN158 VKs[MAX_SK];
u8 S[MAX_SK];
BIG_160_28 R_num;
BIG_160_28 R;
BIG_160_28 SECRET;

ECP_BN158 RES;

ECP2_BN158 g12;
FP12_BN158 pair1;
ECP_BN158 g21;
FP12_BN158 pair2;
ECP_BN158 U;
ECP_BN158 shares[MAX_SK];
char V[50];
ECP2_BN158 W,H;


/*************************Threshold Signature****************************************/

ECP_BN158 M_point;
ECP_BN158 Sigs[MAX_SK];
ECP_BN158 SIG;


void testdemo();
void ff(BIG_160_28 r,BIG_160_28 x){
	BIG_160_28 q;
	BIG_160_28_rcopy(q, CURVE_Order_BN158);
	BIG_160_28 y;
	BIG_160_28 xx;
	BIG_160_28_zero(y);
	BIG_160_28_one(xx);
	BIG_160_28 tmp;
	BIG_160_28 tmp1;
	for(u16 i=0;i<k;i++){
		
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
	for(u8 jj=0;jj<k;jj++){
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
	for(u8 jj=0;jj<k;jj++){
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


void dealer(){
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNGx;                // Crypto Strong RNG
	u32 ran = RNG_Get_RandomNum();	
    RAW.len = 100;              // fake random seed source
    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
	BIG_160_28 secret;
	BIG_160_28 modl;
	BIG_160_28_rcopy(modl,CURVE_Order_BN158);
	BIG_160_28_randomnum(secret,modl,&RNGx);
	
	BIG_160_28_copy(a[0],secret);

	for(u16 i=1;i<k;i++){
		BIG_160_28_randomnum(a[i],modl,&RNGx);
	}
	
	for(u16 i=0;i<players;i++){
		BIG_160_28 x;
		BIG_160_28_zero(x);
		BIG_160_28_inc(x,i+1);
		ff(SKs[i],x);
	}
	
	//assert ff(0) == secret
	BIG_160_28 secret1;
	BIG_160_28 ZERO;
	BIG_160_28_zero(ZERO);
	ff(secret1,ZERO);
	
	//VK = g2 ** secret
	ECP2_BN158_copy(&VK,&g2);
	PAIR_BN158_G2mul(&VK,secret);
	
	//VKs = [g2 ** xx for xx in SKs ]
	for(u16 i=0 ; i<players ; i++){
		ECP2_BN158_copy(&VKs[i],&g2);
		PAIR_BN158_G2mul(&VKs[i],SKs[i]);	
	}	
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	dealer_time[Run_idx] = time;
	for(int i=0;i<MAX_SK;i++)S[i] = i;
//	sprintf((char*)Send_Data_buff,"Dealer Time:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	/**************************Dealer end**********************************/
	

////	# Check reconstruction of 0
////  S = set(range(0,k))[0,1,2]
////  lhs = f(0)
//	BIG_160_28 lhs,rhs,tmp,tmp1,tmp2,tmp3,tmp4;
////	FP_BN158 ZERO;
//	BIG_160_28_zero(ZERO);
//	BIG_160_28_zero(rhs);
//	ff(lhs,ZERO);
//	
////  rhs = sum(public_key.lagrange(S,j) * f(j+1) for j in S)
//	for(u8 i=0;i<20;i++)S[i] = i;
//	
//	for(u16 i=0;i<k;i++){
//		BIG_160_28_zero(tmp2);
//		BIG_160_28_inc(tmp2,S[i]+1);
//		ff(tmp,tmp2);//tmp = ff[S[i]+1]
//		lagrange(tmp1,S[i]);//tmp1 = lagrange
//		BIG_160_28_modmul(tmp3,tmp1,tmp,modl);//tmp3 = tmp1 * tmp
//		BIG_160_28_modadd(tmp4,tmp3,rhs,modl);//tmp4 = tmp3 + rhs
//		BIG_160_28_copy(rhs,tmp4);//rhs = tmp4
//	}
//	
//	if(BIG_160_28_comp(lhs,rhs)==0){
//		sprintf((char*)Send_Data_buff,"Reconstruction: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Reconstruction: NO");
//		LoRa_SendData(Send_Data_buff);
//	}
	
}




/*************************Threshold Encryption****************************************/
void PK_encrypt(char* m){
//	# Only encrypt 32 byte strings
//	assert len(m) == 32
//	r = group.random(ZR)
//	U = g1 ** r
//	V = xor(m, hashG(self.VK ** r))
//	W = hashH(U, V) ** r
//	C = (U, V, W)
//	return C
	/***************************************************/
	//	r = group.random(ZR)
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	BIG_160_28 modl;
	BIG_160_28_rcopy(modl,CURVE_Order_BN158);
	
	char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNGx;                // Crypto Strong RNG
	u32 ran = RNG_Get_RandomNum();
    RAW.len = 100;              // fake random seed source
    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
	BIG_160_28 r;
	BIG_160_28_randomnum(r,modl,&RNGx);
	
	//	U = g1 ** r
	
	ECP_BN158_copy(&U,&g1);
	PAIR_BN158_G1mul(&U,r);
	
//	//test
//	ECP_BN158_copy(&shares[3],&U);
//	PAIR_BN158_G1mul(&shares[3],SKs[2]);
//	
//	ECP2_BN158 g2_neg;
//	ECP2_BN158_copy(&g2_neg,&g2);
//	ECP2_BN158_neg(&g2_neg);
//	
//	//pair(U_i, g2) == pair(U, Y_i)
//	FP12_BN158 v;
//	//PAIR_BN158_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//	PAIR_BN158_double_ate(&v,&g2_neg,&shares[3],&VKs[2],&U);
//	PAIR_BN158_fexp(&v);
//	
//	if (FP12_BN158_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"ddPK_verify_share[%d]: Yes",3);
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"ddPK_verify_share[%d]: No",3);
//		LoRa_SendData(Send_Data_buff);
//	}
//	while(1);
//	//end
	
	//BIG_160_28_copy(R_num,r_num);
	
///////V = xor(m, hashG(self.VK ** r))
	
	//#V = xor(m, hashG(pair(g1, self.VK ** r)))
	
	ECP_BN158 g1_neg;
	ECP_BN158_copy(&g1_neg,&g1);
	ECP_BN158_neg(&g1_neg);
	
	ECP2_BN158 VKr;				//VK ** r
	ECP2_BN158_copy(&VKr,&VK);
	PAIR_BN158_G2mul(&VKr,r);
	
	FP12_BN158 pair_r;//pair(g1, self.VK ** r)
	PAIR_BN158_ate(&pair_r,&VKr,&g1);
	PAIR_BN158_fexp(&pair_r);
	//testdemo();
//	if (FP12_BN158_equals(&pair_r,&pair1)){
//		sprintf((char*)Send_Data_buff,"Test Pair 1: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test Pair 1: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
	
	octet pair_r_oct;				//VKr->oct
	char pair_r_ch[4000];for(int i=0;i<4000;i++)pair_r_ch[i]=NULL;
	pair_r_oct.val = pair_r_ch;
	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
	
	
	hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
	HASH256_hash(&sh256,digest);
	
	for(int i=0;i<32;i++){	//xor m and VKr->oct
		V[i] = m[i]^digest[i];
	}
	V[32] = NULL;
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_encrypt_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"Encrypt Time:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
//	/**************************Encrypt end**********************************/
//	
////	W = hashH(U, V) ** r
//	octet U_oct;				//U->oct
//	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
//	U_oct.val = U_ch;
//	ECP_BN158_toOctet(&U_oct,&U,true);
//	
//	octet W_oct;		//U+V
//	char W_ch[100];for(int i=0;i<100;i++)W_ch[i]=NULL;
//	W_oct.val = W_ch;
//	W_oct.max = 100;
//	OCT_copy(&W_oct,&U_oct);
//	OCT_jstring(&W_oct,V);
//	
//	char digest1[50];for(int i=0;i<50;i++)digest1[i]=NULL;
//	HASH256_init(&sh256);
//	for (int i=0;W_oct.val[i]!=0;i++) HASH256_process(&sh256,W_oct.val[i]);
//	HASH256_hash(&sh256,digest1);
//	BIG_160_28 W_num;
//	BIG_160_28_fromBytes(W_num,digest1);
//	
//	
//	ECP2_BN158_copy(&W,&G2);
//	PAIR_BN158_G2mul(&W,W_num);
//	ECP2_BN158_copy(&H,&W); //	H
//	PAIR_BN158_G2mul(&W,r);
//	
//	ECP_BN158_copy(&g1_neg,&g1);
//	ECP_BN158_neg(&g1_neg);
//	
//	FP12_BN158 v;
//	PAIR_BN158_double_ate(&v,&W,&g1_neg,&H,&U);
//	PAIR_BN158_fexp(&v);
////    if (FP12_BN158_isunity(&v)){
////		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: No");
////		LoRa_SendData(Send_Data_buff);
////	}
//	
//	// test single fail 下面的
//	FP12_BN158 r1,r2;
//	
//	PAIR_BN158_ate(&r1,&W,&g1);
//	PAIR_BN158_ate(&r2,&H,&U);
//	
//	PAIR_BN158_fexp(&r1);
//	PAIR_BN158_fexp(&r2);
//	
////	if(FP12_BN158_equals(&r1,&r2)){
////		sprintf((char*)Send_Data_buff,"Test single verify: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"Test single verify: No");
////		LoRa_SendData(Send_Data_buff);
////	}
		
}

void PK_verify_ciphertext(){
//	def verify_ciphertext(self, (U, V, W)):
//	# Check correctness of ciphertext
//	H = hashH(U, V)
//	assert pair(g1, W) == pair(U, H)
//	return True
	//H = hashH(U+V);
//	ECP2_BN158 H;
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	octet U_oct;				//U->oct
	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
	U_oct.val = U_ch;
	ECP_BN158_toOctet(&U_oct,&U,true);
	
	octet H_oct;		//U+V
	char H_ch[100];for(int i=0;i<100;i++)H_ch[i]=NULL;
	H_oct.val = H_ch;
	H_oct.max = 100;
	OCT_copy(&H_oct,&U_oct);
	OCT_jstring(&H_oct,V);
	
	hash256 sh256;
	char digest1[50];for(int i=0;i<50;i++)digest1[i]=NULL;
	HASH256_init(&sh256);
	for (int i=0;H_oct.val[i]!=0;i++) HASH256_process(&sh256,H_oct.val[i]);
	HASH256_hash(&sh256,digest1);
	BIG_160_28 H_num;
	BIG_160_28_fromBytes(H_num,digest1);

	ECP2_BN158_copy(&H,&G2);
	PAIR_BN158_G2mul(&H,H_num);
	
	
	ECP_BN158 g1_neg;
	ECP_BN158_copy(&g1_neg,&g1);
	ECP_BN158_neg(&g1_neg);
	
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&W,&g1_neg,&H,&U);
	PAIR_BN158_fexp(&v);
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_ciphertext_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"Verify Ciphertext Time:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	/**************************verify end**********************************/
	
//    if (FP12_BN158_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"PK_verify_ciphertext: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"PK_verify_ciphertext: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
}
void SK_decrypt_share(){
	//U_i = U ** self.SK
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	BIG_160_28 sk_num;
	for(int i=0;i<players;i++){
		ECP_BN158_copy(&shares[i],&U);
		PAIR_BN158_G1mul(&shares[i],SKs[i]);
	}
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	SK_decrypt_share_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"Shares Construction Time:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	/**************************Shares Construction**********************************/
	
//	sprintf((char*)Send_Data_buff,"Shares Construction: Yes");
//	LoRa_SendData(Send_Data_buff);
}

void PK_verify_share(){
//	def verify_share(self, i, U_i, (U,V,W)):
//	assert 0 <= i < self.l
//	Y_i = self.VKs[i]
//	assert pair(U_i, g2) == pair(U, Y_i)
//	return True
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	
	for(int i=0;i<players;i++){
		//pair(U_i, g2) == pair(U, Y_i)
		FP12_BN158 v;
		PAIR_BN158_double_ate(&v,&g2_neg,&shares[i],&VKs[i],&U);
		PAIR_BN158_fexp(&v);
		
//		if (FP12_BN158_isunity(&v)){
//			sprintf((char*)Send_Data_buff,"PK_verify_share[%d]: Yes",i);
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"PK_verify_share[%d]: No",i);
//			LoRa_SendData(Send_Data_buff);
//		}
	}
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_share_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_verify_share%d:%d",players,time);
//	LoRa_SendData(Send_Data_buff);
	
	/**************************PK_verify_share**********************************/

}

void PK_combine_share(){
	//	def combine_shares(self, (U,V,W), shares):
	//	# sigs: a mapping from idx -> sig
	//	S = set(shares.keys())
	//	assert S.issubset(range(self.l))

	//	# ASSUMPTION
	//	# assert self.verify_ciphertext((U,V,W))

	//	mul = lambda a,b: a*b
	//	res = reduce(mul, [share ** self.lagrange(S, j)for j,share in shares.iteritems()], ONE)
	//	return xor(hashG(res), V)
	
//	sprintf((char*)Send_Data_buff,"Combine shares: start");
//	LoRa_SendData(Send_Data_buff);
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	BIG_160_28 larg;
	ECP_BN158 tmp;
	ECP_BN158 r[20];
	ECP_BN158 res;
	for(int j=0 ; j<k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = S[j];
		lagrange(larg,jj);
		ECP_BN158_copy(&tmp,&shares[jj]);
		PAIR_BN158_G1mul(&tmp,larg);
		ECP_BN158_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<k ; j++){
		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
	ECP_BN158_copy(&res,&r[0]);	
	
	u32 time1 = TIM9->CNT + TIM9_Exceed_Times * 5000;
	
	FP12_BN158 pair_r;//pair(g2, res)
	PAIR_BN158_ate(&pair_r,&g2,&res);
	PAIR_BN158_fexp(&pair_r);
	
	octet pair_r_oct;				//VKr->oct
	char pair_r_ch[4000];for(int i=0;i<4000;i++)pair_r_ch[i]=NULL;
	pair_r_oct.val = pair_r_ch;
	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
	
	u32 time2 = TIM9->CNT + TIM9_Exceed_Times * 5000;
	
	hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
	HASH256_hash(&sh256,digest);
	
	char m_[100];for(int i=0;i<100;i++)m_[i]=NULL;
	
	for(int i=0;i<32;i++){
		m_[i] = digest[i]^V[i];
	}	
	
	for(int i=0;i<50;i++)	M_[i] = m_[i];
	
	int ress = 1;
	for(int i=0;i<32;i++){
		if(M[i]!=M_[i])ress=0;
	}
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_combine_share_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"Combine shares: %d",time);
//	LoRa_SendData(Send_Data_buff);
//	sprintf((char*)Send_Data_buff,"Combine shares,time1: %d",time1);
//	LoRa_SendData(Send_Data_buff);
//	sprintf((char*)Send_Data_buff,"Combine shares,time2: %d",time2);
//	LoRa_SendData(Send_Data_buff);
//	sprintf((char*)Send_Data_buff,"Combine shares,time3: %d",time);
//	LoRa_SendData(Send_Data_buff);
	/**************************Combine shares**********************************/
	
	if(ress){
		sprintf((char*)Send_Data_buff,"Combine shares: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Combine shares: No");
		LoRa_SendData(Send_Data_buff);
	}
//		SS = range(PK.l)
//		for i in range(1):
//      random.shuffle(SS)
//      S = set(SS[:PK.k])
//      
//      m_ = PK.combine_shares(C, dict((s,shares[s]) for s in S))
//      assert m_ == m
	
}



/*************************Threshold Signature****************************************/
void SK_sign(){
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	//h ** sk
	for(int i=0;i<players;i++){
		ECP_BN158_copy(&Sigs[i],&M_point);
		PAIR_BN158_G1mul(&Sigs[i],SKs[i]);
	}
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	SK_sign_time[Run_idx] = time;

	octet o;
	char o_ch[200];for(int i=0;i<200;i++)o_ch[i]=0x00;
	o.val = o_ch;
	ECP_BN158_toOctet(&o,&Sigs[0],true);
	Share_Size[Run_idx] = o.len;
//	sprintf((char*)Send_Data_buff,"SK_sign:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
//	sprintf((char*)Send_Data_buff,"Sign: Yes");
//	LoRa_SendData(Send_Data_buff);
	
}

void PK_verify_sig_share(){

	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
//	B = self.VKs[i]
//	assert pair(g2, sig) == pair(B, h)
//	return True
	ECP2_BN158 B;
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	for(int i=0;i<players;i++){

		ECP2_BN158_copy(&B,&VKs[i]);
		FP12_BN158 v;
		PAIR_BN158_double_ate(&v,&g2_neg,&Sigs[i],&B,&M_point);
		PAIR_BN158_fexp(&v);
		
//		u32 timex = TIM2->CNT + TIM2_Exceed_Times * 9000;
//		
//		if (FP12_BN158_isunity(&v)){
//			sprintf((char*)Send_Data_buff,"PK_verify_sig_share[%d]: Yes %d",i,timex);
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"PK_verify_sig_share[%d]: No %d",i,timex);
//			LoRa_SendData(Send_Data_buff);
//		}
	}
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_share_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_verify_sig_share %d : %d",players,time);
//	LoRa_SendData(Send_Data_buff);
}

void PK_verify_sig(){
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
//	assert pair(sig, g2) == pair(h, self.VK)
//  return True
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&g2);
	ECP2_BN158_neg(&g2_neg);
	FP12_BN158 v;
	PAIR_BN158_double_ate(&v,&g2_neg,&SIG,&VK,&M_point);
	PAIR_BN158_fexp(&v);
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
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

void PK_sig_combine_share(){
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	//	def combine_shares(self, (U,V,W), shares):
	//	# sigs: a mapping from idx -> sig
	//	S = set(shares.keys())
	//	assert S.issubset(range(self.l))

	//	# ASSUMPTION
	//	# assert self.verify_ciphertext((U,V,W))

	//	mul = lambda a,b: a*b
	//	res = reduce(mul, [sig ** self.lagrange(S, j) for j,sig in sigs.iteritems()], 1)
    //  return res
	
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	BIG_160_28 larg;
	ECP_BN158 tmp;
	ECP_BN158 r[20];
	
	for(int j=0 ; j<k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = S[j];
		lagrange(larg,jj);
		ECP_BN158_copy(&tmp,&Sigs[jj]);
		PAIR_BN158_G1mul(&tmp,larg);
		ECP_BN158_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<k ; j++){
		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
	ECP_BN158_copy(&SIG,&r[0]);	
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_sig_combine_share_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_sig_combine_share:%d",time);
//	LoRa_SendData(Send_Data_buff);

}


void ThresCrypto(){
	
	init();
	init_public_key();
	int Run_times = 30;
	int Threshold_encryption_flag = 0;
	u8 print_key = 1;
	
for(int enc_flag=0;enc_flag<2;enc_flag++){
	Threshold_encryption_flag = enc_flag;
	if(Threshold_encryption_flag){
		sprintf((char*)Send_Data_buff,"***Threshold_Encryption***");LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"***Threshold_Signature***");LoRa_SendData(Send_Data_buff);
	}
	
for(Run_idx=0;Run_idx<Run_times;Run_idx++){
	players = 4;
	k = 3; 
	
	//sha256 init
	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	char digest[50];
	hash256 sh256;
	
	int i; 
	BIG_160_28 g1_num;
	
	char mc[10] = "geng1";
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
	
	dealer();
	
	octet RAW;
	char vch[200];
	for(int i=0;i<200;i++)vch[i] = 0x00;RAW.max = 200;RAW.len = 0;RAW.val = vch;
	BIG_160_28 sks[6];
	if(print_key){
		LoRa_CFG.chn = 66;
		LoRa_Set();
		sprintf((char*)Send_Data_buff,"SKs::");LoRa_SendData(Send_Data_buff);
		for(int i=0;i<players;i++){
			BIG_160_28_toBytes(vch,SKs[i]);
			sprintf((char*)Send_Data_buff,"SKs[%d]",i);LoRa_SendData(Send_Data_buff);
			for(int m=0;m<40;m++){
				sprintf((char*)Send_Data_buff,"%02x",vch[m]);LoRa_SendData(Send_Data_buff);
				delay_ms(200);
			}
		}
		sprintf((char*)Send_Data_buff,"VKs::");LoRa_SendData(Send_Data_buff);
		for(int i=0;i<players;i++){
			for(int i=0;i<200;i++)vch[i] = 0x00;RAW.max = 200;RAW.len = 0;RAW.val = vch;
			ECP2_BN158_toOctet(&RAW,&VKs[i],true);
			sprintf((char*)Send_Data_buff,"VKs[%d]",i);LoRa_SendData(Send_Data_buff);
			for(int m=0;m<RAW.len;m++){
				sprintf((char*)Send_Data_buff,"%02x",vch[m]);LoRa_SendData(Send_Data_buff);
				delay_ms(200);
			}	
		}
		
		sprintf((char*)Send_Data_buff,"VK::");LoRa_SendData(Send_Data_buff);
		for(int i=0;i<200;i++)vch[i] = 0x00;RAW.max = 200;RAW.len = 0;RAW.val = vch;
		ECP2_BN158_toOctet(&RAW,&VK,true);
		for(int m=0;m<RAW.len;m++){
			sprintf((char*)Send_Data_buff,"%02x",vch[m]);LoRa_SendData(Send_Data_buff);
			delay_ms(200);
		}	
		
		
		while(1);
	}
	
	
	
	for(int i=0;i<50;i++)digest[i] = NULL;
	HASH256_init(&sh256);
	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
    HASH256_hash(&sh256,digest);
	
	for(int i=0;i<50;i++)M[i]=digest[i];
	
	FP_BN158 M_FP;
	BIG_160_28 M_num;
	BIG_160_28_fromBytes(M_num,digest);
	
	ECP_BN158_copy(&M_point,&G1);
	PAIR_BN158_G1mul(&M_point,M_num);

	if(Threshold_encryption_flag){
		//Threshold Encryption
		PK_encrypt(digest);
		PK_verify_ciphertext();
		SK_decrypt_share();
		PK_verify_share();
		PK_combine_share();
	}else{
		//Threshold Signature
		SK_sign();
		PK_verify_sig_share();
		PK_sig_combine_share();
		PK_verify_sig();
	}

}

	sprintf((char*)Send_Data_buff,"Dealer Time:");LoRa_SendData(Send_Data_buff);
	for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",dealer_time[i]);LoRa_SendData(Send_Data_buff);}
	
	if(Threshold_encryption_flag){
		//Threshold Encryption
		sprintf((char*)Send_Data_buff,"PK_encrypt Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_encrypt_time[i]);LoRa_SendData(Send_Data_buff);}
		sprintf((char*)Send_Data_buff,"PK_verify_ciphertext Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_verify_ciphertext_time[i]);LoRa_SendData(Send_Data_buff);}
		sprintf((char*)Send_Data_buff,"SK_decrypt_share Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",SK_decrypt_share_time[i]);LoRa_SendData(Send_Data_buff);}
		sprintf((char*)Send_Data_buff,"PK_verify_share Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_verify_share_time[i]);LoRa_SendData(Send_Data_buff);}
		sprintf((char*)Send_Data_buff,"PK_combine_share Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_combine_share_time[i]);LoRa_SendData(Send_Data_buff);}
	}else{
		//Threshold Signature
		sprintf((char*)Send_Data_buff,"SK_sign Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",SK_sign_time[i]);LoRa_SendData(Send_Data_buff);}
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_verify_sig_share_time[i]);LoRa_SendData(Send_Data_buff);}
		sprintf((char*)Send_Data_buff,"PK_sig_combine_share Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_sig_combine_share_time[i]);LoRa_SendData(Send_Data_buff);}
		sprintf((char*)Send_Data_buff,"PK_verify_sig Time:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_verify_sig_time[i]);LoRa_SendData(Send_Data_buff);}
		sprintf((char*)Send_Data_buff,"Share_Size:");LoRa_SendData(Send_Data_buff);
		for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",Share_Size[i]);LoRa_SendData(Send_Data_buff);}
	}

}
	while(1);
}




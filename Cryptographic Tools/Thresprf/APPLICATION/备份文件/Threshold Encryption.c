#include "mw1268_app.h"
#include "mw1268_cfg.h"
#include "usart3.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "tendermint2.h"
#include "core.h"
#include "bls_BN254.h"


#define MAX_K 10
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
	LoRa_CFG.chn = Data_channel;
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

csprng R11;
int bls_BN254(csprng *RNGx)
{
	
	sprintf((char*)Send_Data_buff,"Start");
	LoRa_SendData(Send_Data_buff);
	
    int i,res;
    char s[BGS_BN254];
    char ikm[64];
#ifdef REVERSE
    char w[BFS_BN254+1], sig[4 * BFS_BN254 + 1];    // w is 2* if not compressed else 1*. sig is 4* if not compressed, else 2*
#else
    char w[4 * BFS_BN254+1], sig[BFS_BN254 + 1];    // w is 4* if not compressed else 2*. sig is 2* if not compressed, else 1*
#endif
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};
    octet SIG = {0, sizeof(sig), sig};
    octet IKM = {0, sizeof(ikm), ikm};
    octet M = {0,sizeof(message), message};
    OCT_jstring(&M,message);

    res = BLS_BN254_INIT();
    if (res == BLS_FAIL)
    {
       //printf("Failed to initialize\n");
	
		sprintf((char*)Send_Data_buff,"Failed to initialize\n");
		LoRa_SendData(Send_Data_buff);
		
        return res;
    }

    OCT_rand(&IKM,RNGx,32);
    //IKM.len=32;
    //for (i=0;i<IKM.len;i++)
    //        IKM.val[i]=i+1;


    res = BLS_BN254_KEY_PAIR_GENERATE(&IKM, &S, &W);
    if (res == BLS_FAIL)
    {
        //printf("Failed to generate keys\n");
		sprintf((char*)Send_Data_buff,"Failed to generate keys");
		LoRa_SendData(Send_Data_buff);
        return res;
    }
	
	sprintf((char*)Send_Data_buff,"Check point 0");
	LoRa_SendData(Send_Data_buff);
	
	
    //printf("Private key= 0x");
    //OCT_output(&S);
    //printf("Public key= 0x");
    //OCT_output(&W);

    BLS_BN254_CORE_SIGN(&SIG, &M, &S);
    //printf("Signature= 0x");
    //OCT_output(&SIG);

    //message[7]='f'; // change the message

    res = BLS_BN254_CORE_VERIFY(&SIG, &M, &W);
	
	sprintf((char*)Send_Data_buff,"Check point 1");
	LoRa_SendData(Send_Data_buff);
	
    if (res == BLS_OK){
//		printf("Signature is OK\n");
		sprintf((char*)Send_Data_buff,"Signature is OK");
		LoRa_SendData(Send_Data_buff);
	} 
    else{
		sprintf((char*)Send_Data_buff,"Signature is NOT OK");
		LoRa_SendData(Send_Data_buff);
	}
    return res;
}	


//Threshold Encryption

FP_BN254 a[MAX_K];
FP_BN254 SKs[MAX_SK];
u8 players;
u8 k;
ECP_BN254 G1;
ECP2_BN254 G2;
ECP_BN254 g1;
ECP2_BN254 g2;
ECP2_BN254 VK;
ECP2_BN254 VKs[MAX_SK];
u8 S[MAX_SK];
BIG_256_28 R_num;
FP_BN254 R;
FP_BN254 SECRET;

ECP_BN254 RES;

ECP2_BN254 g12;
FP12_BN254 pair1;
ECP_BN254 g21;
FP12_BN254 pair2;
void testdemo();
void ff(FP_BN254 *r,FP_BN254 *x){
	FP_BN254 y;
	FP_BN254 xx;
	FP_BN254_zero(&y);
	FP_BN254_one(&xx);
	FP_BN254 tmp;
	FP_BN254 tmp1;
	for(u16 i=0;i<k;i++){
		
		//y+=coeff * xx
		FP_BN254_mul(&tmp,&a[i],&xx);//tmp = a[i] * xx
		//BIG_256_28_norm(tmp);
		FP_BN254_add(&tmp1,&y,&tmp);//tmp1 = y + tmp
		//BIG_256_28_norm(tmp1);
		FP_BN254_copy(&y,&tmp1);  //y = tmp1
		
		//xx*=x
		FP_BN254_mul(&tmp,&xx,x);//tmp = xx * x
		//BIG_256_28_norm(tmp);
		FP_BN254_copy(&xx,&tmp);
		
	}
	FP_BN254_copy(r,&y);
}

void lagrange(FP_BN254 *r,u8 j){
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
	
	
	//	num = reduce(mul, [0 - jj - 1 for jj in S if jj != j], ONE)
	FP_BN254 num, ZERO, ONE, S_jj, tmp, tmp2, tmp3;
	FP_BN254_one(&num);
	FP_BN254_zero(&ZERO);
	FP_BN254_one(&ONE);
	
	for(u8 jj=0;jj<k;jj++){
		if(S[jj] == j) continue;
		//num = num * (0 - S[jj] - 1);
		FP_BN254_from_int(&S_jj,S[jj]);
		FP_BN254_sub(&tmp,&ZERO,&S_jj);//tmp = 0 - S[jj]
		FP_BN254_sub(&tmp2,&tmp,&ONE);//tmp2 = tmp - 1
		FP_BN254_mul(&tmp3, &num, &tmp2);//tmp3 = num * tmp2
		FP_BN254_copy(&num,&tmp3);	// num = tmp3 
	}

	//	den = reduce(mul, [j - jj     for jj in S if jj != j], ONE)
	FP_BN254 den, BIG_J;
	FP_BN254_one(&den);
	FP_BN254_from_int(&BIG_J,j);
	for(u8 jj=0;jj<k;jj++){
		if(S[jj] == j) continue;
		//den = den * (j - S[jj]);
		FP_BN254_from_int(&S_jj,S[jj]);
		FP_BN254_sub(&tmp,&BIG_J,&S_jj);//tmp = j - S[jj]
		FP_BN254_mul(&tmp2, &den, &tmp);//tmp2 = den * tmp
		FP_BN254_copy(&den,&tmp2);
	}
	//return num/den
	FP_BN254_inv(&tmp, &den,NULL);
	FP_BN254_mul(r, &num, &tmp);

}

void dealer(){
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNGx;                // Crypto Strong RNG
	u32 ran = RNG_Get_RandomNum();	
    RAW.len = 100;              // fake random seed source
    RAW.val[0] = ran;
    RAW.val[1] = ran >> 8;
    RAW.val[2] = ran >> 16;
    RAW.val[3] = ran >> 24;
    for (u16 i = 4; i < 100; i++) RAW.val[i] = i;
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
	FP_BN254 secret;
	FP_BN254_rand(&secret,&RNGx);
	
//	while(FP_BN254_sign(&secret)){
//		FP_BN254_rand(&secret,&RNGx);
//	}
	ran = RNG_Get_RandomNum();
	FP_BN254_from_int(&secret,ran);
	FP_BN254_copy(&a[0],&secret);
	FP_BN254_copy(&SECRET,&secret);

	for(u16 i=1;i<k;i++){
		
		//FP_BN254_rand(&a[i],&RNGx);
		ran = RNG_Get_RandomNum();
		FP_BN254_from_int(&a[i],ran);
		
		while(FP_BN254_sign(&a[i])){
			ran = RNG_Get_RandomNum();
			FP_BN254_from_int(&a[i],ran);
			//FP_BN254_rand(&a[i],&RNGx);
		}

	}
	
	for(u16 i=0;i<players;i++){
		FP_BN254 x;
		FP_BN254_from_int(&x,i+1);
		ff(&SKs[i],&x);
	}
	
	//assert ff(0) == secret
	FP_BN254 secret1;
	FP_BN254 ZERO;
	FP_BN254_zero(&ZERO);
	ff(&secret1,&ZERO);
	if(FP_BN254_equals(&secret1,&secret)){
		sprintf((char*)Send_Data_buff,"hahaha");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"wuwuwu");
		LoRa_SendData(Send_Data_buff);
	}
	
	//VK = g2 ** secret
	ECP2_BN254_copy(&VK,&g2);
	BIG_256_28 FP_num;
	FP_BN254_redc(FP_num,&secret);
	PAIR_BN254_G2mul(&VK,FP_num);
	
	//VKs = [g2 ** xx for xx in SKs ]
	for(u16 i=0 ; i<players ; i++){
		ECP2_BN254_copy(&VKs[i],&g2);
		FP_BN254_redc(FP_num,&SKs[i]);
		PAIR_BN254_G2mul(&VKs[i],FP_num);	
	}	
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 500;
	sprintf((char*)Send_Data_buff,"Dealer Time:%d",time);
	LoRa_SendData(Send_Data_buff);
	
	/**************************Dealer end**********************************/
	

//	# Check reconstruction of 0
//  S = set(range(0,k))[0,1,2]
//  lhs = f(0)
	FP_BN254 lhs,rhs,tmp,tmp1,tmp2,tmp3,tmp4;
//	FP_BN254 ZERO;
	FP_BN254_zero(&ZERO);
	FP_BN254_zero(&rhs);
	ff(&lhs,&ZERO);
	
//  rhs = sum(public_key.lagrange(S,j) * f(j+1) for j in S)
	for(u8 i=0;i<10;i++)S[i] = i;
	
	for(u16 i=0;i<k;i++){
		FP_BN254_from_int(&tmp2,S[i]+1);
		ff(&tmp,&tmp2);//tmp = ff[S[i]+1]
		lagrange(&tmp1,S[i]);//tmp1 = lagrange
		FP_BN254_mul(&tmp3,&tmp1,&tmp);//tmp3 = tmp1 * tmp
		FP_BN254_add(&tmp4,&tmp3,&rhs);//tmp4 = tmp3 + rhs
		FP_BN254_copy(&rhs,&tmp4);//rhs = tmp4
	}
	
	BIG_256_28 lhs_num,rhs_num;
	
	FP_BN254_redc(lhs_num,&lhs);
	FP_BN254_redc(rhs_num,&rhs);
	
	if(FP_BN254_equals(&lhs,&rhs) && BIG_256_28_comp(lhs_num,rhs_num)==0){
		sprintf((char*)Send_Data_buff,"Reconstruction: YES");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Reconstruction: NO");
		LoRa_SendData(Send_Data_buff);
	}
	
}

ECP_BN254 U;
ECP_BN254 shares[MAX_SK];
char V[50];
ECP2_BN254 W,H;
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
	
	char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNGx;                // Crypto Strong RNG
	u32 ran = RNG_Get_RandomNum();	
    RAW.len = 100;              // fake random seed source
    RAW.val[0] = ran;
    RAW.val[1] = ran >> 8;
    RAW.val[2] = ran >> 16;
    RAW.val[3] = ran >> 24;
    for (u16 i = 4; i < 100; i++) RAW.val[i] = i;
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
	FP_BN254 r;
	FP_BN254_rand(&r,&RNGx);
	
	while(FP_BN254_sign(&r)){
		FP_BN254_rand(&r,&RNGx);
	}
	
	//u16 xxx = ran;
	FP_BN254_from_int(&r,ran);
	while(FP_BN254_sign(&r)){
		ran = RNG_Get_RandomNum();
		FP_BN254_from_int(&r,ran);
	}
	
	FP_BN254_copy(&R,&r);
	
	
	//	U = g1 ** r
	
	BIG_256_28 r_num;
	ECP_BN254_copy(&U,&g1);
	FP_BN254_redc(r_num,&r);
	PAIR_BN254_G1mul(&U,r_num);
	
	BIG_256_28_copy(R_num,r_num);
	
///////V = xor(m, hashG(self.VK ** r))
	
	//#V = xor(m, hashG(pair(g1, self.VK ** r)))
	
	ECP_BN254 g1_neg;
	ECP_BN254_copy(&g1_neg,&g1);
	ECP_BN254_neg(&g1_neg);
	
	ECP2_BN254 VKr;				//VK ** r
	ECP2_BN254_copy(&VKr,&VK);
	PAIR_BN254_G2mul(&VKr,r_num);
	
	FP12_BN254 pair_r;//pair(g1, self.VK ** r)
	PAIR_BN254_ate(&pair_r,&VKr,&g1);
	PAIR_BN254_fexp(&pair_r);
//	testdemo();
//	if (FP12_BN254_equals(&pair_r,&pair1)){
//		sprintf((char*)Send_Data_buff,"Test Pair 1: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test Pair 1: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
	
	octet pair_r_oct;				//VKr->oct
	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
	pair_r_oct.val = pair_r_ch;
	FP12_BN254_toOctet(&pair_r_oct,&pair_r);
	
	
	hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
	HASH256_hash(&sh256,digest);
	
	for(int i=0;i<32;i++){	//xor m and VKr->oct
		V[i] = m[i]^digest[i];
	}
	V[32] = NULL;
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 500;
	sprintf((char*)Send_Data_buff,"Encrypt Time:%d",time);
	LoRa_SendData(Send_Data_buff);
	
	/**************************Encrypt end**********************************/
	
	
	
	
	
//	W = hashH(U, V) ** r
	octet U_oct;				//U->oct
	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
	U_oct.val = U_ch;
	ECP_BN254_toOctet(&U_oct,&U,true);
	
	octet W_oct;		//U+V
	char W_ch[100];for(int i=0;i<100;i++)W_ch[i]=NULL;
	W_oct.val = W_ch;
	W_oct.max = 100;
	OCT_copy(&W_oct,&U_oct);
	OCT_jstring(&W_oct,V);
	
	char digest1[50];for(int i=0;i<50;i++)digest1[i]=NULL;
	HASH256_init(&sh256);
	for (int i=0;W_oct.val[i]!=0;i++) HASH256_process(&sh256,W_oct.val[i]);
	HASH256_hash(&sh256,digest1);
	BIG_256_28 W_num;
	BIG_256_28_fromBytes(W_num,digest1);
	
	
	ECP2_BN254_copy(&W,&G2);
	PAIR_BN254_G2mul(&W,W_num);
	ECP2_BN254_copy(&H,&W); //	H
	PAIR_BN254_G2mul(&W,r_num);
	
	ECP_BN254_copy(&g1_neg,&g1);
	ECP_BN254_neg(&g1_neg);
	
	FP12_BN254 v;
	PAIR_BN254_double_ate(&v,&W,&g1_neg,&H,&U);
	PAIR_BN254_fexp(&v);
    if (FP12_BN254_isunity(&v)){
		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: No");
		LoRa_SendData(Send_Data_buff);
	}
	
	// test single fail 下面的
	FP12_BN254 r1,r2;
	
	PAIR_BN254_ate(&r1,&W,&g1);
	PAIR_BN254_ate(&r2,&H,&U);
	
	PAIR_BN254_fexp(&r1);
	PAIR_BN254_fexp(&r2);
	
	if(FP12_BN254_equals(&r1,&r2)){
		sprintf((char*)Send_Data_buff,"Test single verify: YES");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Test single verify: No");
		LoRa_SendData(Send_Data_buff);
	}
		
}

void PK_verify_ciphertext(){
//	def verify_ciphertext(self, (U, V, W)):
//	# Check correctness of ciphertext
//	H = hashH(U, V)
//	assert pair(g1, W) == pair(U, H)
//	return True
	//H = hashH(U+V);
//	ECP2_BN254 H;
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	octet U_oct;				//U->oct
	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
	U_oct.val = U_ch;
	ECP_BN254_toOctet(&U_oct,&U,true);
	
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
	BIG_256_28 H_num;
	BIG_256_28_fromBytes(H_num,digest1);

	ECP2_BN254_copy(&H,&G2);
	PAIR_BN254_G2mul(&H,H_num);
	
	
	ECP_BN254 g1_neg;
	ECP_BN254_copy(&g1_neg,&g1);
	ECP_BN254_neg(&g1_neg);
	
	FP12_BN254 v;
	PAIR_BN254_double_ate(&v,&W,&g1_neg,&H,&U);
	PAIR_BN254_fexp(&v);
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 500;
	sprintf((char*)Send_Data_buff,"Verify Ciphertext Time:%d",time);
	LoRa_SendData(Send_Data_buff);
	
	/**************************verify end**********************************/
	
    if (FP12_BN254_isunity(&v)){
		sprintf((char*)Send_Data_buff,"PK_verify_ciphertext: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"PK_verify_ciphertext: No");
		LoRa_SendData(Send_Data_buff);
	}
	
}


void SK_decrypt_share(){
	//U_i = U ** self.SK
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	BIG_256_28 sk_num;
	for(int i=0;i<players;i++){
		ECP_BN254_copy(&shares[i],&U);
		FP_BN254_redc(sk_num,&SKs[i]);
		PAIR_BN254_G1mul(&shares[i],sk_num);
	}
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 500;
	sprintf((char*)Send_Data_buff,"Shares Construction Time:%d",time);
	LoRa_SendData(Send_Data_buff);
	
	/**************************Shares Construction**********************************/
	
	sprintf((char*)Send_Data_buff,"Shares Construction: Yes");
	LoRa_SendData(Send_Data_buff);
}

void PK_verify_share(){
//	def verify_share(self, i, U_i, (U,V,W)):
//	assert 0 <= i < self.l
//	Y_i = self.VKs[i]
//	assert pair(U_i, g2) == pair(U, Y_i)
//	return True
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	ECP2_BN254 g2_neg;
	ECP2_BN254_copy(&g2_neg,&g2);
	ECP2_BN254_neg(&g2_neg);
	
	for(int i=0;i<players;i++){
				
		
		//pair(U_i, g2) == pair(U, Y_i)
		FP12_BN254 v;
		//PAIR_BN254_double_ate(&v,&shares[i],&g2_neg,&VKs[i],&U);
		PAIR_BN254_double_ate(&v,&g2_neg,&shares[i],&VKs[i],&U);
		PAIR_BN254_fexp(&v);
		
//		if (FP12_BN254_isunity(&v)){
//			sprintf((char*)Send_Data_buff,"PK_verify_share[%d]: Yes",i);
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"PK_verify_share[%d]: No",i);
//			LoRa_SendData(Send_Data_buff);
//		}
	}
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 500;
	sprintf((char*)Send_Data_buff,"PK_verify_share%d:%d",players,time);
	LoRa_SendData(Send_Data_buff);
	
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
	
	sprintf((char*)Send_Data_buff,"Combine shares: start");
	LoRa_SendData(Send_Data_buff);
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	FP_BN254 larg;
	BIG_256_28 larg_num;
	ECP_BN254 tmp;
	ECP_BN254 r[20];
	ECP_BN254 res;
	
	for(int j=0 ; j<k ; j++){
//		share ** self.lagrange(S, j)
		lagrange(&larg,j);
		FP_BN254_redc(larg_num,&larg);
		ECP_BN254_copy(&tmp,&shares[j]);
		
		if(FP_BN254_sign(&larg)){
			// negative
			FP_BN254 larg_tmp;
			BIG_256_28 larg_num_tmp;
			FP_BN254_neg(&larg_tmp,&larg);
			FP_BN254_redc(larg_num_tmp,&larg_tmp);
			PAIR_BN254_G1mul(&tmp,larg_num_tmp);
			ECP_BN254_neg(&tmp);
		}else{
			FP_BN254_redc(larg_num,&larg);
			PAIR_BN254_G1mul(&tmp,larg_num);
		}
		
		ECP_BN254_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<k ; j++){
		ECP_BN254_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
	ECP_BN254_copy(&res,&r[0]);	
	
	//PAIR_BN254_G1member
//	if(PAIR_BN254_G1member(&res)){
//		sprintf((char*)Send_Data_buff,"Test Res G1 member: YES");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test Res G1 member: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
	//PAIR_BN254_G1member
//	if(ECP_BN254_equals(&RES,&res)){
//		sprintf((char*)Send_Data_buff,"RES = res: YES");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"RES = res: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
//	if (ECP_BN254_equals(&res,&g21)){
//		sprintf((char*)Send_Data_buff,"Test g21: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test g21: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	ECP2_BN254 g2_neg;
//	ECP2_BN254_copy(&g2_neg,&g2);
//	ECP2_BN254_neg(&g2_neg);
	
	FP12_BN254 pair_r;//pair(g2, res)
	PAIR_BN254_ate(&pair_r,&g2,&res);
	PAIR_BN254_fexp(&pair_r);
	
//	if (FP12_BN254_equals(&pair_r,&pair2)){
//		sprintf((char*)Send_Data_buff,"Test Pair 2: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test Pair 2: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
	octet pair_r_oct;				//VKr->oct
	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
	pair_r_oct.val = pair_r_ch;
	FP12_BN254_toOctet(&pair_r_oct,&pair_r);
	
	hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
	HASH256_hash(&sh256,digest);
	
	
//	octet Res_oct;				//Res->oct
//	char Res_ch[100];for(int i=0;i<100;i++)Res_ch[i]=NULL;
//	Res_oct.val = Res_ch;
//	ECP_BN254_toOctet(&Res_oct,&res,true);
//	
//	hash256 sh256;//hash VKr_oct  
//	HASH256_init(&sh256);
//	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
//	for (int i=0;Res_oct.val[i]!=0;i++) HASH256_process(&sh256,Res_oct.val[i]);
//	HASH256_hash(&sh256,digest);
	
	char m_[100];for(int i=0;i<100;i++)m_[i]=NULL;
	
	for(int i=0;i<32;i++){
		m_[i] = digest[i]^V[i];
	}	
	
	for(int i=0;i<50;i++)	M_[i] = m_[i];
	
	int ress = 1;
	for(int i=0;i<32;i++){
		if(M[i]!=M_[i])ress=0;
	}
	
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 500;
	sprintf((char*)Send_Data_buff,"Combine shares: %d",time);
	LoRa_SendData(Send_Data_buff);
	
	/**************************Combine shares**********************************/
	
	if(ress){
		sprintf((char*)Send_Data_buff,"Combine shares: YES");
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


void testdemo(){
	//test demo
	//pair1(g2*secret*r,g1) == pair2(g2,g1*secret*r)
	//R_num
	//SECRET
	BIG_256_28 Secret_num;
	FP_BN254_redc(Secret_num,&SECRET);
	//pair(g2*secret*r,g1)
	ECP2_BN254_copy(&g12,&g2);
	PAIR_BN254_G2mul(&g12,R_num);
	PAIR_BN254_G2mul(&g12,Secret_num);
	PAIR_BN254_ate(&pair1,&g12,&g1);
	PAIR_BN254_fexp(&pair1);
	
	//pair(g2,g1*secret*r)
	ECP_BN254_copy(&g21,&g1);
	PAIR_BN254_G1mul(&g21,R_num);
	PAIR_BN254_G1mul(&g21,Secret_num);
	PAIR_BN254_ate(&pair2,&g2,&g21);
	PAIR_BN254_fexp(&pair2);
	
	if(FP12_BN254_equals(&pair2,&pair1)){
		sprintf((char*)Send_Data_buff,"Test demo: YES");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Test demo: No");
		LoRa_SendData(Send_Data_buff);
	}
	
	octet pair1_r_oct;				//VKr->oct
	char pair1_r_ch[400];for(int i=0;i<400;i++)pair1_r_ch[i]=NULL;
	pair1_r_oct.val = pair1_r_ch;
	FP12_BN254_toOctet(&pair1_r_oct,&pair1);
	
	hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest1[50];for(int i=0;i<50;i++)digest1[i]=NULL;
	for (int i=0;pair1_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair1_r_oct.val[i]);
	HASH256_hash(&sh256,digest1);
	
	
	octet pair2_r_oct;				//VKr->oct
	char pair2_r_ch[400];for(int i=0;i<400;i++)pair2_r_ch[i]=NULL;
	pair2_r_oct.val = pair2_r_ch;
	FP12_BN254_toOctet(&pair2_r_oct,&pair2);
	
	//hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest2[50];for(int i=0;i<50;i++)digest2[i]=NULL;
	for (int i=0;pair2_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair2_r_oct.val[i]);
	HASH256_hash(&sh256,digest2);
	
	int flag = 1;
	for(int i=0;i<50;i++)
		if(digest1[i]!=digest2[i])
			flag = 0;
	
	if(flag){
		sprintf((char*)Send_Data_buff,"Test demo 2: YES");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Test demo 2: No");
		LoRa_SendData(Send_Data_buff);
	}	
	
/*********************TEST**************************/	
	//RES = g1*r*ff[1]*lag[0]+..+g1*r*ff[k]*lag[k-1]
	
	ECP_BN254 tmp;
	FP_BN254 ff_FP,ff_idx;
	BIG_256_28 ff_num;
	//BIG_256_28 ff_idx;
	FP_BN254 lag;
	BIG_256_28 lag_num;
	
	for(int i=0;i<k;i++){
		FP_BN254_from_int(&ff_idx,i+1);
		ff(&ff_FP,&ff_idx);
		FP_BN254_redc(ff_num,&ff_FP);
		
		lagrange(&lag,i);
		
		
		if(i==0){
			ECP_BN254_copy(&RES,&g1);
			PAIR_BN254_G1mul(&RES,R_num);
			PAIR_BN254_G1mul(&RES,ff_num);
			
			if(FP_BN254_sign(&lag)){
				// negative
				FP_BN254 lag_tmp;
				BIG_256_28 lag_num_tmp;
				FP_BN254_neg(&lag_tmp,&lag);
				FP_BN254_redc(lag_num_tmp,&lag_tmp);
				PAIR_BN254_G1mul(&RES,lag_num_tmp);
				ECP_BN254_neg(&RES);
			}else{
				FP_BN254_redc(lag_num,&lag);
				PAIR_BN254_G1mul(&RES,lag_num);
			}
			continue;
		}
		
		ECP_BN254_copy(&tmp,&g1);
		PAIR_BN254_G1mul(&tmp,R_num);
		PAIR_BN254_G1mul(&tmp,ff_num);		
		//PAIR_BN254_G1mul(&tmp,lag_num);
		if(FP_BN254_sign(&lag)){//FP_BN254_sign
			// negative
			FP_BN254 lag_tmp;
			BIG_256_28 lag_num_tmp;
			FP_BN254_neg(&lag_tmp,&lag);
			FP_BN254_redc(lag_num_tmp,&lag_tmp);
			PAIR_BN254_G1mul(&tmp,lag_num_tmp);
			ECP_BN254_neg(&tmp);
		}else{
			FP_BN254_redc(lag_num,&lag);
			PAIR_BN254_G1mul(&tmp,lag_num);
		}

		ECP_BN254_add(&RES,&tmp);
	}
	//FP_BN254_mul
	
	if (ECP_BN254_equals(&RES,&g21)){
		sprintf((char*)Send_Data_buff,"Test demo RES = g21: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Test demo RES = g21: No");
		LoRa_SendData(Send_Data_buff);
	}
	
	
	//g1*r*(ff[1]*lag[0]+...+ff[k]*lag[k-1])
	FP_BN254 tot,tot1,tot2;
	FP_BN254_zero(&tot);
	for(int i=0;i<k;i++){
		FP_BN254_from_int(&ff_idx,i+1);
		ff(&ff_FP,&ff_idx);

		lagrange(&lag,i);
		
		FP_BN254_mul(&tot1,&ff_FP,&lag);
		FP_BN254_copy(&tot2,&tot);
		FP_BN254_add(&tot,&tot1,&tot2);
	}
	BIG_256_28 tot_num;
	FP_BN254_redc(tot_num,&tot);
	ECP_BN254 res2;
	ECP_BN254_copy(&res2,&g1);
	PAIR_BN254_G1mul(&res2,R_num);
	PAIR_BN254_G1mul(&res2,tot_num);
	
	if (ECP_BN254_equals(&res2,&g21)){
		sprintf((char*)Send_Data_buff,"Test demo res2 = g21: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Test demo res2 = g21: No");
		LoRa_SendData(Send_Data_buff);
	}
	

	
	/**********************test1*************************************/
	//r*(ff[1]*lag[0]+...+ff[k]*lag[k-1]) == r*ff[1]*lag[0]+...+r*ff[k]*lag[k-1])
	
	
	//r*(ff[1]*lag[0]+...+ff[k]*lag[k-1])
	FP_BN254 tot1_t1;
	FP_BN254 tmp1_t1,tmp2_t1;
	FP_BN254_zero(&tot1_t1);
	FP_BN254_zero(&tmp2_t1);
	for(int i=0;i<k;i++){
		FP_BN254_from_int(&ff_idx,i+1);
		ff(&ff_FP,&ff_idx);
		lagrange(&lag,i);

		FP_BN254_mul(&tmp1_t1,&ff_FP,&lag);
		FP_BN254_copy(&tmp2_t1,&tot1_t1);
		FP_BN254_add(&tot1_t1,&tmp1_t1,&tmp2_t1);
	}
	FP_BN254_mul(&tmp1_t1,&tot1_t1,&R);
	FP_BN254_copy(&tot1_t1,&tmp1_t1);
	
	//r*ff[1]*lag[0]+...+r*ff[k]*lag[k-1])
	
	FP_BN254 tot2_t1,tmp3_t1;
	FP_BN254_zero(&tot2_t1);
	FP_BN254_zero(&tmp2_t1);
	for(int i=0;i<k;i++){
		FP_BN254_from_int(&ff_idx,i+1);
		ff(&ff_FP,&ff_idx);
		lagrange(&lag,i);
		
		FP_BN254_mul(&tmp1_t1,&ff_FP,&lag);
		FP_BN254_mul(&tmp2_t1,&R,&tmp1_t1);
		
		FP_BN254_copy(&tmp3_t1,&tot2_t1);
		FP_BN254_add(&tot2_t1,&tmp2_t1,&tmp3_t1);
	}

	if (FP_BN254_equals(&tot1_t1,&tot2_t1)){
		sprintf((char*)Send_Data_buff,"TTTTTTTTTTTest 1: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"TTTTTTTTTTTest 1: No");
		LoRa_SendData(Send_Data_buff);
	}
	/**********************test2*************************************/
	//g1*1+g1*2 == g1*(1+2)
	
	ECP_BN254 tot1_t2,t1_t2,t2_t2;
	ECP_BN254 tot2_t2;
	
	FP_BN254 tmp1_t2,tmp2_t2,tmp3_t2;
	BIG_256_28 num1,num2,num3;
	FP_BN254_from_int(&tmp1_t2,6);
	FP_BN254_from_int(&tmp2_t2,1);
	FP_BN254_from_int(&tmp3_t2,7);
	
	FP_BN254_redc(num1,&tmp1_t2);
	FP_BN254_redc(num2,&tmp2_t2);
	FP_BN254_redc(num3,&tmp3_t2);
	
	ECP_BN254_copy(&t1_t2,&g1);
	ECP_BN254_copy(&t2_t2,&g1);
	
	//ECP_BN254_mul(&t1_t2,num1);
	//ECP_BN254_mul(&t2_t2,num2);
	PAIR_BN254_G1mul(&t1_t2,num1);
	PAIR_BN254_G1mul(&t2_t2,num2);
	
	ECP_BN254_add(&t1_t2,&t2_t2);
	ECP_BN254_copy(&tot1_t2,&t1_t2);
	
	ECP_BN254_copy(&tot2_t2,&g1);
	
	//ECP_BN254_mul(&tot2_t2,num3);
	PAIR_BN254_G1mul(&tot2_t2,num3);
	
	if (ECP_BN254_equals(&tot1_t2,&tot2_t2)){
		sprintf((char*)Send_Data_buff,"TTTTTTTTTTTest 2: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"TTTTTTTTTTTest 2: No");
		LoRa_SendData(Send_Data_buff);
	}
	
	FP_BN254_from_int(&tmp1_t2,-6);
	FP_BN254_from_int(&tmp2_t2,3);
	
	FP_BN254_add(&tmp3_t2,&tmp1_t2,&tmp2_t2);
	
	if (!FP_BN254_sign(&tmp3_t2)){
		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is +");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is -");
		LoRa_SendData(Send_Data_buff);
	}

	/*************	Test3	********************/
	//g1*(-6)+g1*(7) == g1
	FP_BN254 tmp1_t3,tmp2_t3,tmp3_t3;
	ECP_BN254 g1_t3,g2_t3,g3_t3;
	ECP_BN254_copy(&g1_t3,&g1);
	ECP_BN254_copy(&g2_t3,&g1);
	ECP_BN254_copy(&g3_t3,&g1);
	
	FP_BN254_from_int(&tmp1_t3,-10);
	FP_BN254_from_int(&tmp2_t3,12);
	BIG_256_28 tmp1_num,tmp2_num;
	
	FP_BN254_add(&tmp3_t2,&tmp1_t2,&tmp2_t2);
	
	if(FP_BN254_sign(&tmp1_t3)){
		FP_BN254_neg(&tmp3_t3,&tmp1_t3);
		FP_BN254_redc(tmp1_num,&tmp3_t3);
	}
	FP_BN254_redc(tmp2_num,&tmp2_t3);
	
	PAIR_BN254_G1mul(&g1_t3,tmp1_num);
	ECP_BN254_neg(&g1_t3);
	PAIR_BN254_G1mul(&g2_t3,tmp2_num);
	ECP_BN254_add(&g1_t3,&g2_t3);
	
	FP_BN254_from_int(&tmp1_t3,2);
	FP_BN254_redc(tmp1_num,&tmp1_t3);
	PAIR_BN254_G1mul(&g3_t3,tmp1_num);
	
	if (ECP_BN254_equals(&g1_t3,&g3_t3)){
		sprintf((char*)Send_Data_buff,"TTTTTTest neg function: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"TTTTTTest neg function: No");
		LoRa_SendData(Send_Data_buff);
	}
	
	/*************	Test4	********************/
	FP_BN254 TMP;
	FP_BN254_from_int(&TMP,9);
	FP_BN254_from_int(&lag,1);
	FP_BN254_redc(lag_num,&lag);
	lagrange(&lag,1);
	FP_BN254_redc(lag_num,&lag);
	if(FP_BN254_sign(&lag)){
		sprintf((char*)Send_Data_buff,"Sign of lag is -");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Sign of lag is +");
		LoRa_SendData(Send_Data_buff);
	}
	if(FP_BN254_sign(&TMP)){
		sprintf((char*)Send_Data_buff,"Sign of TMP is -");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"Sign of TMP is +");
		LoRa_SendData(Send_Data_buff);
	}
	if(FP_BN254_equals(&lag,&TMP)){
		sprintf((char*)Send_Data_buff,"lag == -1: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"lag == -1: No");
		LoRa_SendData(Send_Data_buff);
	}
	
	
//	if (!FP_BN254_sign(&tmp3_t2)){
//		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is +");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is -");
//		LoRa_SendData(Send_Data_buff);
//	}
	
/////////////////////////////////////////////////////////////////////////////
}
void tendermint2(){
	
	init();
	init_public_key();
//	//test

	
	FP_BN254 ONE;
	FP_BN254 ZERO;
	FP_BN254_zero(&ZERO);
	FP_BN254_one(&ONE);

	if(FP_BN254_iszilch(&ZERO) && FP_BN254_isunity(&ONE)){
		sprintf((char*)Send_Data_buff,"ONE and ZERO");
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"NOT ONE and ZERO");
		LoRa_SendData(Send_Data_buff);
	
	}
	
	players = 16;
	k = 4;
	
	//sha256 init
	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	char digest[50];
	hash256 sh256;
	
	int i;
	BIG_256_28 g1_num;
	
	//test BIG_256_28_fshr
	//BIG_256_28_fshr(g1_num,8);
	
	char mc[10] = "geng1";
	
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
	
	
	for(int i=0;i<50;i++)digest[i] = NULL;
	HASH256_init(&sh256);
	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
    HASH256_hash(&sh256,digest);
	
	if(PAIR_BN254_G1member(&g1) && PAIR_BN254_G2member(&g2)){
		sprintf((char*)Send_Data_buff,"G1 G2 Member generate: Yes");
		LoRa_SendData(Send_Data_buff);
	}
	
	
	dealer();
	
	for(int i=0;i<50;i++)M[i]=digest[i];
	
	PK_encrypt(digest);
	PK_verify_ciphertext();
	SK_decrypt_share();
	PK_verify_share();
	PK_combine_share();
	
	while(1);


}
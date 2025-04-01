#include "mw1268_app.h"
#include "mw1268_cfg.h"
#include "usart3.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "ThresPrf.h"
#include "core.h"
#include "bls_BN158.h"


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


char thres_private_key_string[4][119] ={
"2a 76 35 75 9e 84 04 81 52 a5 7a f1 a3 78 38 5a ab e0 c0 01 d2 d8 9e 54 a8 00 b0 b0 6b 86 f5 de 00 00 00 00 00 00 00 00", 
"32 77 d9 18 7b 27 5b 64 8d 4a 75 9c 4c 92 5c 8f f8 d6 21 90 a7 1e 41 ca e7 f2 3d 46 46 3b 6f 49 00 00 00 00 00 00 00 00", 
"3a 79 7c bb 57 ca b2 47 c7 ef 70 46 f5 ac 80 c5 45 cb 83 1f 7b 63 e5 41 27 e3 c9 dc 20 ef e8 b4 00 00 00 00 00 00 00 00", 
"42 7b 20 5e 34 6e 09 2b 02 94 6a f1 9e c6 a4 fa 92 c0 e4 ae 4f a9 88 b7 67 d5 56 71 fb a4 62 1f 00 00 00 00 00 00 00 00" 
};

char thres_public_key_string[4+1][98] = {
"02 0a 51 78 9b 6c 05 42 b4 3c ca 29 b4 1e 66 2b de 8d 39 6c 06 c6 53 6b 0b b1 cc fb ed c7 f1 c3 6a", 
"02 15 2a b3 50 9a e0 a1 cb 0a a1 84 80 3f b3 7f 83 a5 71 25 9c 81 cd 3b 68 e9 fc bb 13 9d 55 ec d9", 
"03 24 ea 07 d9 f0 da f8 86 e2 1c 5f dc 54 f9 42 90 3d 78 b0 7c e7 af d6 cb fe 0d e0 0f 29 94 69 48", 
"02 1a 65 13 53 17 62 41 f7 dd 58 ba b1 80 d8 bc 85 9d d9 15 50 20 67 0e 56 a0 c6 ac b8 0a 91 3e cf", 
"02 05 45 7f 56 e1 ef 26 50 fa 05 0e 1b c7 4e 0c 36 63 02 60 04 de e8 66 9b ef 06 c5 ce c0 47 10 54"
};

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

csprng R11;
int bls_BN158(csprng *RNGx)
{
	
	sprintf((char*)Send_Data_buff,"Start");
	LoRa_SendData(Send_Data_buff);
	
    int i,res;
    char s[BGS_BN158];
    char ikm[64];
#ifdef REVERSE
    char w[BFS_BN158+1], sig[4 * BFS_BN158 + 1];    // w is 2* if not compressed else 1*. sig is 4* if not compressed, else 2*
#else
    char w[4 * BFS_BN158+1], sig[BFS_BN158 + 1];    // w is 4* if not compressed else 2*. sig is 2* if not compressed, else 1*
#endif
    octet S = {0, sizeof(s), s};
    octet W = {0, sizeof(w), w};
    octet SIG = {0, sizeof(sig), sig};
    octet IKM = {0, sizeof(ikm), ikm};
    octet M = {0,sizeof(message), message};
    OCT_jstring(&M,message);

    res = BLS_BN158_INIT();
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


    res = BLS_BN158_KEY_PAIR_GENERATE(&IKM, &S, &W);
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

    BLS_BN158_CORE_SIGN(&SIG, &M, &S);
    //printf("Signature= 0x");
    //OCT_output(&SIG);

    //message[7]='f'; // change the message

    res = BLS_BN158_CORE_VERIFY(&SIG, &M, &W);
	
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

int Run_idx=0;
int dealer_time[100],PK_encrypt_time[100],PK_verify_ciphertext_time[100],SK_decrypt_share_time[100],PK_verify_share_time[100],PK_combine_share_time[100];
int SK_sign_time[100],PK_verify_sig_share_time[100],PK_verify_sig_time[100],PK_sig_combine_share_time[100];
/*************************Threshold Encryption****************************************/

BIG_160_28 a[MAX_K];
BIG_160_28 SKs[MAX_SK];
u8 players;
u8 k;
ECP_BN158 G1;
ECP_BN158 G3;
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
ECP2_BN158 M_point2;
ECP_BN158 Sigs[MAX_SK];
ECP2_BN158 Sigs2[MAX_SK];
ECP_BN158 SIG;

ECP_BN158 VK1;
ECP_BN158 VKs1[MAX_SK];

BIG_160_28 Z[MAX_SK];
BIG_160_28 C[MAX_SK];

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
	
//	BIG_160_28 q;
//	BIG_160_28_rcopy(q, CURVE_Order_BN158);
//	BIG_160_28_sdiv(num , den);
//	BIG_160_28_copy(r,num);
	//BIG_160_28_moddiv(r, num , den , q);
	//BIG_160_28_norm(r);
//	BIG_160_28_inv(tmp, den,NULL);
//	BIG_160_28_mul(r, num, tmp);

}

//u8 Sixteen2Ten(char ch){
//	if( (ch >= '0') && ( ch <= '9' ) )	return ch - '0';
//	if((ch >= 'A') && ( ch <= 'Z' )) return 10 + (ch - 'A');
//	return 10 + (ch - 'a');
//}
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
	
	BIG_160_28 secret, r;
	BIG_160_28_rcopy(r, CURVE_Order_BN158);
	BIG_160_28_randomnum(secret, r, &RNGx);
	
	BIG_160_28_copy(a[0],secret);
	BIG_160_28_copy(SECRET,secret);

	for(u16 i=1;i<k;i++){
		BIG_160_28_randomnum(a[i], r, &RNGx);
	}
	
	for(u16 i=0;i<players;i++){
		BIG_160_28 x;
		BIG_160_28_zero(x);
		BIG_160_28_inc(x,i+1);
		ff(SKs[i],x);
	}
	
	//test bytes
	char tmp_ch[300];
	for(int i=0;i<300;i++)tmp_ch[i] = 0x00;
	BIG_160_28_toBytes(tmp_ch,SKs[0]);
	BIG_160_28_fromBytes(SKs[0],tmp_ch);
	//end
	
	//assert ff(0) == secret
	BIG_160_28 secret1;
	BIG_160_28 ZERO;
	BIG_160_28_zero(ZERO);
	ff(secret1,ZERO);
//	
//////////////////	//VK = g2 ** secret
//////////////////	ECP2_BN158_copy(&VK,&g2);
//////////////////	PAIR_BN158_G2mul(&VK,secret);
//////////////////	
//////////////////	//VKs = [g2 ** xx for xx in SKs ]
//////////////////	for(u16 i=0 ; i<players ; i++){
//////////////////		ECP2_BN158_copy(&VKs[i],&g2);
//////////////////		PAIR_BN158_G2mul(&VKs[i],SKs[i]);	
//////////////////	}
	
	
	/**********************************************/	
	//VK1
	//VKs1
	ECP_BN158_copy(&VK1,&g1);
	PAIR_BN158_G1mul(&VK1,secret);
	//VKs = [g2 ** xx for xx in SKs ]
	for(u16 i=0 ; i<players ; i++){
		ECP_BN158_copy(&VKs1[i],&g1);
		PAIR_BN158_G1mul(&VKs1[i],SKs[i]);	
	}
	ECP_BN158 VK1;
	ECP_BN158_copy(&VK1,&g1);
	PAIR_BN158_G1mul(&VK1,secret);
	/*********************************************/
	
//	/******************output it**************************/
//	octet O;
//	char O_ch[100];for(int x=0;x<100;x++)O_ch[x]=0x00;
//	O.val = O_ch;O.max = 100;
//	sprintf((char*)Send_Data_buff,"SKs");
//	LoRa_SendData(Send_Data_buff);
//	for(int i=0;i<players;i++){
//		sprintf((char*)Send_Data_buff,"SKs[%d]",i);
//		LoRa_SendData(Send_Data_buff);
//		for(int x=0;x<100;x++)O_ch[x]=0x00;
//		BIG_160_28_toBytes(O_ch,SKs[i]);
//		for(int j=0;j<40;j++){
//			sprintf((char*)Send_Data_buff,"%02x",O_ch[j]);
//			LoRa_SendData(Send_Data_buff);
//			delay_ms(200);
//		}
//	}
//	sprintf((char*)Send_Data_buff,"VKs");
//	LoRa_SendData(Send_Data_buff);
//	for(int i=0;i<players;i++){
//		sprintf((char*)Send_Data_buff,"VKs[%d]",i);
//		LoRa_SendData(Send_Data_buff);
//		for(int x=0;x<100;x++)O_ch[x]=0x00;
//		ECP_BN158_toOctet(&O,&VKs1[i],true);
//		for(int j=0;j<O.len;j++){
//			sprintf((char*)Send_Data_buff,"%02x",O_ch[j]);
//			LoRa_SendData(Send_Data_buff);
//			delay_ms(200);
//		}
//	}
//	sprintf((char*)Send_Data_buff,"VK");
//	LoRa_SendData(Send_Data_buff);
//	for(int x=0;x<100;x++)O_ch[x]=0x00;
//	ECP_BN158_toOctet(&O,&VK1,true);
//	for(int j=0;j<O.len;j++){
//		sprintf((char*)Send_Data_buff,"%02x",O_ch[j]);
//		LoRa_SendData(Send_Data_buff);
//		delay_ms(200);
//	}
//	while(1);
	
//	u8 MAX_Nodes = 10;
//	u8 Nodes_N = 4;
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
//	octet RAWx[MAX_Nodes];
//	for(int i=0;i<MAX_Nodes;i++){
//		RAWx[i].max = 200;
//		RAWx[i].len = public_key_idx;
//		RAWx[i].val = vch[i];
//	}
//	
//	for(int i=1;i<=players;i++){
//		BIG_160_28_fromBytes(SKs[i],sch[i]);
//	}
//	for(int i=1;i<=players;i++){
//		ECP_BN158_fromOctet(&VKs1[i],&RAWx[i]);
//	}
//	ECP_BN158_fromOctet(&VK1,&RAWx[Nodes_N+1]);
	/***************************************************/
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	dealer_time[Run_idx] = time;

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
	for(u8 i=0;i<20;i++)S[i] = i;
//	S[0] = 0;
//	S[1] = 1;
//	for(u16 i=0;i<k;i++){
//		BIG_160_28_zero(tmp2);
//		BIG_160_28_inc(tmp2,S[i]+1);
//		ff(tmp,tmp2);//tmp = ff[S[i]+1]
//		lagrange(tmp1,S[i]);//tmp1 = lagrange
//		BIG_160_28_modmul(tmp3,tmp1,tmp,r);//tmp3 = tmp1 * tmp
//		BIG_160_28_modadd(tmp4,tmp3,rhs,r);//tmp4 = tmp3 + rhs
//		BIG_160_28_copy(rhs,tmp4);//rhs = tmp4
//	}
//	
//	BIG_160_28 lhs_num,rhs_num;
//	
////	FP_BN158_redc(lhs_num,lhs);
////	FP_BN158_redc(rhs_num,rhs);
//	
//	if(BIG_160_28_comp(lhs, rhs)==0){
//		sprintf((char*)Send_Data_buff,"Reconstruction: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Reconstruction: NO");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	
//	
//	////////////////////my test
//	
//	BIG_160_28 modl;
//	BIG_160_28_rcopy(modl,CURVE_Order_BN158);
//	
//	BIG_160_28 b1,b2,b3,b4,b1_2;
//	
//	BIG_160_28_zero(b1);
//	BIG_160_28_zero(b2);
//	BIG_160_28_zero(b3);
//	BIG_160_28_zero(b4);
//	BIG_160_28_zero(b1_2);
//	
//	BIG_160_28_inc(b1,1);
//	BIG_160_28_inc(b2,2);
//	BIG_160_28_inc(b3,3);
//	BIG_160_28_inc(b4,4);
//	
//	
//	
//	BIG_160_28_moddiv(b1_2,b1,b2,modl);
//	
//	
//	BIG_160_28 res1;
//	BIG_160_28_modmul(res1,b1_2,b2,modl);
//	
//	if(BIG_160_28_comp(res1, b1)==0){
//		sprintf((char*)Send_Data_buff,"res1: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"res1: NO");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	
//	BIG_160_28 b_1,b_2,b_3,b_4,b_1_2;
//	BIG_160_28_modneg(b_3,b3,modl);
//	BIG_160_28 res2;
//	BIG_160_28_sub(res2, b_1, b_4);
//	
//	if(BIG_160_28_comp(res2, b_3)==0){
//		sprintf((char*)Send_Data_buff,"res2: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"res2: NO");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	BIG_160_28 res3;
//	BIG_160_28_modneg(b_3,b3,modl);
//	BIG_160_28_modneg(b_4,b4,modl);
//	BIG_160_28_modadd(res3,b1,b_4,modl);
//	if(BIG_160_28_comp(res3, b_3)==0){
//		sprintf((char*)Send_Data_buff,"res3: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"res3: NO");
//		LoRa_SendData(Send_Data_buff);
//	}
}

//

//threshold signature
char digestx[50];

char ch__x[600];
ECP_BN158 ah;
ECP_BN158 ah1;

void SK_sign(){

	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	BIG_160_28 r,modl;
	BIG_160_28_rcopy(modl,CURVE_Order_BN158);
	char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNGx;                // Crypto Strong RNG
	u32 ran = RNG_Get_RandomNum();	
    RAW.len = 100;              // fake random seed source
    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
//new 
/*

	g_i_1 = g_1 ** self.SK
	s = group.random()
	h = gg ** s
	h_1 = g_1 ** s
	c = hashH(gg, self.VKs[self.i],h,g_1,g_i_1,h_1)
	z = s + self.SK * c
	return  (g_i_1,c,z)

*/	
	//g_1 -> M_point2
	//gg -> g
	//g_i_1 = g_1 ** self.SK
	
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
	sprintf((char*)Coin_Str,"Coin:%d",1);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
    HASH256_hash(&sh256,digest);

	ECP_BN158 C_point;
	ECP_BN158_copy(&C_point,&G1);
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	BIG_160_28_mod(C_num, modl);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	//g_i_1 = g_1 ** self.SK
	BIG_160_28 num;
	for(int i=0;i<players;i++){
		ECP_BN158_copy(&Sigs[i],&C_point);
		PAIR_BN158_G1mul(&Sigs[i],SKs[i]);
	}
	for(int x=0;x<players;x++){
		//s = group.random()
		BIG_160_28 s;
		
		BIG_160_28_zero(s);
		BIG_160_28_inc(s,10);
		BIG_160_28_mod(s,modl);
		//BIG_160_28_randomnum(s,modl,&RNGx);
		
		//h = gg ** s
		ECP_BN158 h;
		ECP_BN158_copy(&h,&g1);
		PAIR_BN158_G1mul(&h,s);
		
		ECP_BN158_copy(&ah,&h);
		//h_1 = g_1 ** s
		ECP_BN158 h_1;
		ECP_BN158_copy(&h_1,&C_point);
		PAIR_BN158_G1mul(&h_1,s);
		ECP_BN158_copy(&ah1,&h_1);
		//c = hashH(gg, self.VKs[self.i],h,g_1,g_i_1,h_1)
		u8 ch[600];
		int ch_idx = 0;
		for(int i=0;i<600;i++)ch[i] = 0;
		
		octet o;
		char o_ch[600];for(int i=0;i<600;i++)o_ch[i] = 0x00;
		o.val = o_ch;
		o.max = 600;
		//gg
		ECP_BN158_toOctet(&o,&g1,true);
		for(int i=0;i<o.len;i++){ch__x[i] = ch[ch_idx] = o.val[i];ch_idx++;}
		//self.VKs[self.x]
		ECP_BN158_toOctet(&o,&VKs1[x],true);
		for(int i=0;i<o.len;i++){ch__x[i] = ch[ch_idx] = o.val[i];ch_idx++;}
		//h
		ECP_BN158_toOctet(&o,&h,true);
		ECP_BN158_copy(&ah,&h);
		for(int i=0;i<o.len;i++){ch__x[i] = ch[ch_idx] = o.val[i];ch_idx++;}
		//g_1
		ECP_BN158_toOctet(&o,&C_point,true);
		for(int i=0;i<o.len;i++){ch__x[i] = ch[ch_idx] = o.val[i];ch_idx++;}
		//g_i_1
		ECP_BN158_toOctet(&o,&Sigs[x],true);
		for(int i=0;i<o.len;i++){ch__x[i] = ch[ch_idx] = o.val[i];ch_idx++;}
		//h_1
		ECP_BN158_toOctet(&o,&h_1,true);
		ECP_BN158_copy(&ah1,&h_1);
		for(int i=0;i<o.len;i++){ch__x[i] = ch[ch_idx] = o.val[i];ch_idx++;}
		
		for(int i=0;i<ch_idx;i++)ch__x[i] = ch[i];
		
		for(int i=0;i<50;i++)digestx[i]=0x00;	
		hash256 sh256;
		HASH256_init(&sh256);
		for(int i=0;i<ch_idx;i++) HASH256_process(&sh256,ch[i]);
		HASH256_hash(&sh256,digestx);
		
		BIG_160_28_fromBytesLen(C[x], digestx,32);
		BIG_160_28_mod(C[x],modl);
		
		//z = s + self.SK * c
		BIG_160_28 tmp;
		BIG_160_28_modmul(tmp,SKs[x],C[x],modl);
		BIG_160_28_modadd(Z[x],s,tmp,modl);
	}
	
	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	SK_sign_time[Run_idx] = time;

	char tmp_cch[100];
	for(int i=0;i<100;i++)tmp_cch[i] = 0x00;
	BIG_160_28_toBytes(tmp_cch,Z[0]);
	
	
/*************************************************************************/
//verify
	
//		assert 0 <= i < self.l
//        h = ( gg ** z )/(self.VKs[i] ** c)
//        h_1 = ( g_1 ** z)/(sig ** c)
//        c_1 = hashH(gg, self.VKs[i],h,g_1,sig,h_1)
//        return c_1 == c
	
	////g_1 -> M_point2
	//gg -> g
	//g_i_1 = g_1 ** self.SK
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	for(int x=0;x<players;x++){
		//h = ( gg ** z )/(self.VKs[i] ** c)
		ECP_BN158 hx;
		ECP_BN158 tmp1,tmp2,tmp3;
		ECP_BN158_copy(&tmp1,&g1);
		PAIR_BN158_G1mul(&tmp1,Z[x]);
		ECP_BN158_copy(&tmp2,&VKs1[x]);
		PAIR_BN158_G1mul(&tmp2,C[x]);
		ECP_BN158_neg(&tmp2);
		ECP_BN158_add(&tmp1,&tmp2);
		ECP_BN158_copy(&hx,&tmp1);
		
		
		
		//h_1 = ( g_1 ** z)/(sig ** c)
		ECP_BN158 hx_1;
	//	ECP_BN158 tmp1,tmp2;
		ECP_BN158_copy(&tmp1,&C_point);
		PAIR_BN158_G1mul(&tmp1,Z[x]);
		ECP_BN158_copy(&tmp2,&Sigs[x]);
		PAIR_BN158_G1mul(&tmp2,C[x]);
		ECP_BN158_neg(&tmp2);
		ECP_BN158_add(&tmp1,&tmp2);
		ECP_BN158_copy(&hx_1,&tmp1);
		
		
	//  c_1 = hashH(gg, self.VKs[i],h,g_1,sig,h_1)

		u8 ch[600];
		int ch_idx = 0;
		for(int i=0;i<600;i++)ch[i] = 0;
		octet o;
		char o_ch[600];for(int i=0;i<600;i++)o_ch[i] = 0x00;
		o.val = o_ch;
		o.max = 600;
		//gg
		ECP_BN158_toOctet(&o,&g1,true);
		for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
		//self.VKs[self.x]
		ECP_BN158_toOctet(&o,&VKs1[x],true);
		for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
		//h
		ECP_BN158_toOctet(&o,&hx,true);
		for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
		//g_1
		ECP_BN158_toOctet(&o,&C_point,true);
		for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
		//sig
		ECP_BN158_toOctet(&o,&Sigs[x],true);
		for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
		//h_1
		ECP_BN158_toOctet(&o,&hx_1,true);
		for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
		
		char digest[50];for(int i=0;i<50;i++)digest[i]=0x00;	
		hash256 sh256;
		HASH256_init(&sh256);
		for(int i=0;i<ch_idx;i++) HASH256_process(&sh256,ch[i]);
		HASH256_hash(&sh256,digest);
		
		BIG_160_28 c_1;
		BIG_160_28_fromBytesLen(c_1, digest,32);
		BIG_160_28_mod(c_1,modl);
		
//		if(BIG_160_28_comp(C[x],c_1)==0){
//			sprintf((char*)Send_Data_buff,"fangjia");
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"not not not");
//			LoRa_SendData(Send_Data_buff);
//		}

	}
	time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_share_time[Run_idx] = time;
//		while(1);
	// old
/*

	def sign(self, h):
	return h ** self.SK

*/
	
// 	TIM9_Exceed_Times = 0;
// 	TIM9->CNT = 0;
	
// 	//h ** sk
// 	BIG_160_28 num;
// 	for(int i=0;i<players;i++){
// 		FP_BN158_redc(num,&SKs[i]);
// 		ECP_BN158_copy(&Sigs[i],&M_point);
// 		PAIR_BN158_G1mul(&Sigs[i],num);
// 	}
	
// 	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	SK_sign_time[Run_idx] = time;
// 	sprintf((char*)Send_Data_buff,"SK_sign:%d",time);
// 	LoRa_SendData(Send_Data_buff);
	
// //	sprintf((char*)Send_Data_buff,"Sign: Yes");
// //	LoRa_SendData(Send_Data_buff);
	
}

void PK_verify_sig_share(){
// 	TIM9_Exceed_Times = 0;
// 	TIM9->CNT = 0;
//	
// //	B = self.VKs[i]
// //	assert pair(g2, sig) == pair(B, h)
// //	return True
// 	ECP2_BN158 B;
// 	ECP2_BN158 g2_neg;
// 	ECP2_BN158_copy(&g2_neg,&g2);
// 	ECP2_BN158_neg(&g2_neg);
// 	for(int i=0;i<players;i++){
// 		TIM2_Exceed_Times = 0;
// 		TIM2->CNT = 0;
//		
// 		ECP2_BN158_copy(&B,&VKs[i]);
// 		FP12_BN158 v;
// 		PAIR_BN158_double_ate(&v,&g2_neg,&Sigs[i],&B,&M_point);
// 		PAIR_BN158_fexp(&v);
//		
//		u32 timex = TIM2->CNT + TIM2_Exceed_Times * 9000;
//		
//		if (FP12_BN158_isunity(&v)){
//			sprintf((char*)Send_Data_buff,"PK_verify_sig_share[%d]: Yes %d",i,timex);
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"PK_verify_sig_share[%d]: No %d",i,timex);
//			LoRa_SendData(Send_Data_buff);
//		}
// 	}
//	
// 	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_share_time[Run_idx] = time;
// 	sprintf((char*)Send_Data_buff,"PK_verify_sig_share %d : %d",players,time);
// 	LoRa_SendData(Send_Data_buff);
 }

void PK_verify_sig(){
	
// 	TIM9_Exceed_Times = 0;
// 	TIM9->CNT = 0;
//	
// //	assert pair(sig, g2) == pair(h, self.VK)
// //  return True
// 	ECP2_BN158 g2_neg;
// 	ECP2_BN158_copy(&g2_neg,&g2);
// 	ECP2_BN158_neg(&g2_neg);
// 	FP12_BN158 v;
// 	PAIR_BN158_double_ate(&v,&g2_neg,&SIG,&VK,&M_point);
// 	PAIR_BN158_fexp(&v);
//	
// 	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_sig_time[Run_idx] = time;
// 	sprintf((char*)Send_Data_buff,"PK_verify_sig:%d",time);
// 	LoRa_SendData(Send_Data_buff);
//	
//	
// 	if (FP12_BN158_isunity(&v)){
// 		sprintf((char*)Send_Data_buff,"PK_verify_sig: Yes");
// 		LoRa_SendData(Send_Data_buff);
// 	}else{
// 		sprintf((char*)Send_Data_buff,"PK_verify_sig: No");
// 		LoRa_SendData(Send_Data_buff);
// 	}
 }




void PK_sig_combine_share(){
//	// TIM9_Exceed_Times = 0;
//	// TIM9->CNT = 0;

//	//	def combine_shares(self, (U,V,W), shares):
//	//	# sigs: a mapping from idx -> sig
//	//	S = set(shares.keys())
//	//	assert S.issubset(range(self.l))

//	//	# ASSUMPTION
//	//	# assert self.verify_ciphertext((U,V,W))

//	//	mul = lambda a,b: a*b
//	//	res = reduce(mul, [sig ** self.lagrange(S, j) for j,sig in sigs.iteritems()], 1)
//    //  return res

//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//while(1){

	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
//	delay_ms(50);
//	btim_tim9_enable(0);
//	while(1);
	
	BIG_160_28 larg;
	ECP_BN158 tmp;
	ECP_BN158 r[20];
	
S[0] = 0;S[1] = 1;S[2] = 2;S[3] = 3;
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
}

void ThresPrf(){
	
	init();
	init_public_key();
	int Run_times = 10;

for(Run_idx=0;Run_idx<Run_times;Run_idx++){
	players = 4;
	k = 2; 
	u8 print_key = 0;
	//sha256 init
	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	char digest[50];
	hash256 sh256;
	
	int i;
	BIG_160_28 q, modl;
	
	char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNGx;                // Crypto Strong RNG
	u32 ran = RNG_Get_RandomNum();	
    RAW.len = 100;              // fake random seed source
    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
	char mc[] = "geng1";
	for(int i=0;i<50;i++)digest[i] = NULL;
	HASH256_init(&sh256);
	for (i=0;mc[i]!=0;i++) HASH256_process(&sh256,mc[i]);
    HASH256_hash(&sh256,digest);
	BIG_160_28_fromBytes(q,digest);
	
	if(!ECP_BN158_generator(&G1)){
		sprintf((char*)Send_Data_buff,"G1 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	
	ECP_BN158_copy(&g1,&G1);
	PAIR_BN158_G1mul(&g1,q);
	
	dealer();
	
//	if(print_key){
//		octet RAW;
//		char vch[200];
//		for(int i=0;i<200;i++)vch[i] = 0x00;RAW.max = 200;RAW.len = 0;RAW.val = vch;
//		BIG_160_28 sks[6];
//		LoRa_CFG.chn = 66;
//		LoRa_Set();
//		sprintf((char*)Send_Data_buff,"SKs::");LoRa_SendData(Send_Data_buff);
//		for(int i=0;i<players;i++){
//			BIG_160_28_toBytes(vch,SKs[i]);
//			sprintf((char*)Send_Data_buff,"SKs[%d]",i);LoRa_SendData(Send_Data_buff);
//			for(int m=0;m<40;m++){
//				sprintf((char*)Send_Data_buff,"%02x",vch[m]);LoRa_SendData(Send_Data_buff);
//				delay_ms(200);
//			}
//		}
//		sprintf((char*)Send_Data_buff,"VKs::");LoRa_SendData(Send_Data_buff);
//		for(int i=0;i<players;i++){
//			for(int i=0;i<200;i++)vch[i] = 0x00;RAW.max = 200;RAW.len = 0;RAW.val = vch;
//			ECP2_BN158_toOctet(&RAW,&VKs[i],true);
//			sprintf((char*)Send_Data_buff,"VKs[%d]",i);LoRa_SendData(Send_Data_buff);
//			for(int m=0;m<RAW.len;m++){
//				sprintf((char*)Send_Data_buff,"%02x",vch[m]);LoRa_SendData(Send_Data_buff);
//				delay_ms(200);
//			}	
//		}
//		
//		sprintf((char*)Send_Data_buff,"VK::");LoRa_SendData(Send_Data_buff);
//		for(int i=0;i<200;i++)vch[i] = 0x00;RAW.max = 200;RAW.len = 0;RAW.val = vch;
//		ECP2_BN158_toOctet(&RAW,&VK,true);
//		for(int m=0;m<RAW.len;m++){
//			sprintf((char*)Send_Data_buff,"%02x",vch[m]);LoRa_SendData(Send_Data_buff);
//			delay_ms(200);
//		}	
//		
//		
//		while(1);
//	}
	
//	for(int i=0;i<50;i++)digest[i] = NULL;
//	HASH256_init(&sh256);
//	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
//    HASH256_hash(&sh256,digest);
//	
//	BIG_160_28 M_num;
//	BIG_160_28_fromBytesLen(M_num, digest,32);
//	BIG_160_28_mod(M_num,modl);
//	ECP_BN158_copy(&M_point,&g1);
//	PAIR_BN158_G1mul(&M_point,M_num);
	
	//Thresprf
	SK_sign();
//	PK_verify_sig_share();
	PK_sig_combine_share();
}

	sprintf((char*)Send_Data_buff,"Dealer Time:");LoRa_SendData(Send_Data_buff);
	for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",dealer_time[i]);LoRa_SendData(Send_Data_buff);}
	//Threshold Signature
	sprintf((char*)Send_Data_buff,"SK_sign Time:");LoRa_SendData(Send_Data_buff);
	for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",SK_sign_time[i]);LoRa_SendData(Send_Data_buff);}
	sprintf((char*)Send_Data_buff,"PK_verify_sig_share Time:");LoRa_SendData(Send_Data_buff);
	for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_verify_sig_share_time[i]);LoRa_SendData(Send_Data_buff);}
	sprintf((char*)Send_Data_buff,"PK_sig_combine_share Time:");LoRa_SendData(Send_Data_buff);
	for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_sig_combine_share_time[i]);LoRa_SendData(Send_Data_buff);}
//	sprintf((char*)Send_Data_buff,"PK_verify_sig Time:");LoRa_SendData(Send_Data_buff);
//	for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",PK_verify_sig_time[i]);LoRa_SendData(Send_Data_buff);}
//	sprintf((char*)Send_Data_buff,"Share_Size:");LoRa_SendData(Send_Data_buff);
//	for(int i = 0;i < Run_times; i++){sprintf((char*)Send_Data_buff,"%d",Share_Size[i]);LoRa_SendData(Send_Data_buff);}

	while(1);
}

/***************************************************************************************/









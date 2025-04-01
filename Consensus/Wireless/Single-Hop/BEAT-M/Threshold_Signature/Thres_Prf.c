#include "Thres_Prf.h"
#include "BEAT.h"

char thres_private_key_string[4][119] ={
"22 3a e9 89 86 b4 93 db 3b 34 26 78 6a 93 5d ef 6c 7f ea 51 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"11 e2 cd b5 c3 cb 98 36 56 97 1e 99 32 28 d4 1e 63 35 6c dd 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"25 8b d2 09 63 6f 35 e5 41 69 8a b4 0e 93 e9 6d f8 ef 0f 76 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"15 33 b6 35 a0 86 3a 40 5c cc 82 d4 d6 29 5f 9c ef a4 92 02 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" 
};

char thres_public_key_string[4+1][62] = {
"03 04 87 64 ac 6f 99 fe 7a 3c 37 79 04 2e 6f 40 ef ad d1 dd 51", 
"02 1c 1c d5 64 8f 5f 89 04 37 74 1b 23 92 f3 7a 38 c0 1b 31 a0", 
"03 10 f9 a7 fa ff e1 cc c1 c2 92 6c 5c 8d 3d ad d0 c3 2a 09 0b", 
"02 02 99 85 38 71 95 13 a7 cd d5 e9 36 97 50 22 4c 3f 2e c8 00", 
"03 15 47 43 11 1d 34 ca a5 69 31 d3 04 ef 8f 66 47 a5 7e b2 a9" 
};

//char thres_private_key_stringx[4][119] ={
//"33 25 57 c3 c0 88 3f 74 29 79 62 9a 1b 56 71 99 e8 f9 b0 4a c8 0f 3e 64 1a fb db a8 91 04 88 f3 00 00 00 00 00 00 00 00", 
//"24 87 36 ff a2 fb a3 43 ae 3d 2b db e3 90 3f 20 56 af 5d d4 84 6e 13 65 3f f1 2e d7 8d eb 35 6f 00 00 00 00 00 00 00 00", 
//"3b 0c 7a bd c5 6f 07 14 ed 35 42 9d ab ca 0c ae c4 04 8b 5e 40 cc e8 77 05 e6 82 06 8a d1 e1 f8 00 00 00 00 00 00 00 00", 
//"2c 6e 59 f9 a7 e2 6a e4 71 f9 0b df 74 03 da 35 31 ba 38 e7 fd 2b bd 78 2a db d5 35 87 b8 8e 74 00 00 00 00 00 00 00 00" 
//};

//char thres_public_key_stringx[4+1][98] = {
//"02 1c 3c 11 c6 49 cf 43 15 cb d8 83 7f 0d 4a 52 2e 6e 1d 22 cc 06 6c c8 6d 2f 41 76 29 49 18 78 d4", 
//"02 19 9b 09 1a cf 67 c2 0f 88 75 4a 93 a7 1b 14 3b 7d 65 0a 20 21 e6 7d 6d 77 a3 17 55 a3 a7 b1 13", 
//"03 22 c9 64 1d 87 b2 3a 24 5c 5e 21 72 85 54 9c 9e 39 ca ec ec ec 4c ab f4 11 bf bf 7a c3 90 14 fb",
//"02 1a a0 e5 12 d9 ff d4 09 9e 83 a5 fa 28 be b6 63 55 c4 5f 31 ac 71 3f 3c 7e 98 71 27 2a 1c b5 ad", 
//"03 1b 59 a6 2b 64 7a 5e bc d2 27 41 ad b2 f4 d2 ab f1 4b ff d6 94 50 af ac e6 3a 90 bf 3a 33 fd 0a"
//};

char M[50];
//char M_[50];
//BIG_160_28 a[MAX_K];

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
ECP_BN158 VK;
ECP_BN158 VKs[MAX_SK];

ECP_BN158 M_point;
ECP_BN158 Sigs[MAX_SK];
ECP_BN158 SIG;

ECP_BN158 VK1;
ECP_BN158 VKs1[MAX_SK];

BIG_160_28 g1_num;
void init_thres_sig(){

	
//	//sha256 init
	char digest[50];
	hash256 sh256;
//	int i;
////	u32 lenn = NLEN_256_28;
////	int32_t gg[lenn];
//	//int32_t

	char mc[] = "geng1";

	for(int i=0;i<50;i++)digest[i] = 0x00;
	HASH256_init(&sh256);
	for (int i=0;mc[i]!=0;i++) HASH256_process(&sh256,mc[i]);
    HASH256_hash(&sh256,digest);
	BIG_160_28_fromBytes(g1_num,digest);
	
	if(!ECP_BN158_generator(&G1)){
		sprintf((char*)Send_Data_buff,"G1 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	
	ECP_BN158_copy(&g1,&G1);
	PAIR_BN158_G1mul(&g1,g1_num);

	
//	
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
		BIG_160_28_fromBytes(SKs[i],sch[i]);
	}
	for(int i=1;i<=players;i++){
		ECP_BN158_fromOctet(&VKs[i],&RAW[i]);
	}
	ECP_BN158_fromOctet(&VK,&RAW[Nodes_N+1]);
	
	for(u8 i=0;i<10;i++)S[i] = i;

//////	//thresprf test
//////	//dealer();
//////	Coin_SK_sign(1);
//////	
////////	BIG_160_28_fromBytes(ABA_C[1][2],ABA_C_share[1][2]);
////////	BIG_160_28_fromBytes(ABA_Z[1][2],ABA_Z_share[1][2]);
//////	if(Coin_PK_verify_sig_share(3,1,ABA_share[1][3])){
//////		sprintf((char*)Send_Data_buff,"Test thresprf yes");
//////		LoRa_SendData(Send_Data_buff);
//////	}else{
//////		sprintf((char*)Send_Data_buff,"Test thresprf No");
//////		LoRa_SendData(Send_Data_buff);
//////	}
	
	
	
	
}
//ABA coin

BIG_160_28 Z[MAX_SK];
BIG_160_28 C[MAX_SK];
char digestx[50];

char ch__x[600];
ECP_BN158 ah;
ECP_BN158 ah1;

void Coin_SK_sign(u8 aba, int round){

	if(ACK_ABA_share[aba][round][ID])return;
	//h ** sk
	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
	sprintf((char*)Coin_Str,"Coin:%d%d%d",aba,round,Block[1][0][0]);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
    HASH256_hash(&sh256,digest);
	
	//for(int i=0;i<50;i++)M[i]=digest[i];
	BIG_160_28 modl;
	BIG_160_28_rcopy(modl,CURVE_Order_BN158);
	ECP_BN158 C_point;
	ECP_BN158_copy(&C_point,&G1);
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	BIG_160_28_mod(C_num, modl);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 sigs;
	ECP_BN158_copy(&sigs,&C_point);//h ** sk
	PAIR_BN158_G1mul(&sigs,SKs[ID]);
	
	char chx[70];for(int i=0;i<70;i++)chx[i] = 0x00;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = chx;
	
	ECP_BN158_toOctet(&Obj,&sigs,true);
	if(Obj.len != Share_Size){while(1);}
	for(int i=0;i<Obj.len;i++)
		ABA_share[aba][round][ID][i] = Obj.val[i];
	if(!ACK_ABA_share[aba][round][ID])ABA_share_Number[aba][round]++;
	ACK_ABA_share[aba][round][ID] = 1;
	
	
	char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNGx;                // Crypto Strong RNG
	u32 ran = RNG_Get_RandomNum();	
    RAW.len = 100;              // fake random seed source
    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	//s = group.random()
	BIG_160_28 s;

	BIG_160_28_randomnum(s,modl, &RNGx);
	
	//h = gg ** s
	ECP_BN158 h;
	ECP_BN158 tmp_p;
	ECP_BN158_copy(&h,&g1);
	PAIR_BN158_G1mul(&h,s);
	
	//h_1 = g_1 ** s
	ECP_BN158 h_1;
	ECP_BN158_copy(&h_1,&C_point);
	PAIR_BN158_G1mul(&h_1,s);
	
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
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//self.VKs[self.x]
	ECP_BN158_toOctet(&o,&VKs[ID],true);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//h
	ECP_BN158 tmp_h;
	ECP_BN158_copy(&tmp_h,&h);
	ECP_BN158_toOctet(&o,&h,true);
	ECP_BN158_copy(&h,&tmp_h);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//g_1
	ECP_BN158_toOctet(&o,&C_point,true);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//g_i_1
	ECP_BN158_toOctet(&o,&sigs,true);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//h_1
	ECP_BN158_copy(&tmp_h,&h_1);
	ECP_BN158_toOctet(&o,&h_1,true);
	ECP_BN158_copy(&h_1,&tmp_h);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	
	for(int i=0;i<50;i++)digest[i]=0x00;	
	HASH256_init(&sh256);
	for(int i=0;i<ch_idx;i++) HASH256_process(&sh256,ch[i]);
	HASH256_hash(&sh256,digest);
	
	BIG_160_28_fromBytesLen(ABA_C[aba][round][ID], digest,32);
	BIG_160_28_mod(ABA_C[aba][round][ID],modl);
	
	//z = s + self.SK * c
	BIG_160_28 tmp;
	BIG_160_28_modmul(tmp,SKs[ID],ABA_C[aba][round][ID],modl);
	BIG_160_28_modadd(ABA_Z[aba][round][ID],s,tmp,modl);
	
	BIG_160_28_toBytes((char*)ABA_C_share[aba][round][ID],ABA_C[aba][round][ID]);
	BIG_160_28_toBytes((char*)ABA_Z_share[aba][round][ID],ABA_Z[aba][round][ID]);
	
	
	
	
//	//h = ( gg ** z )/(self.VKs[i] ** c)
//	ECP_BN158 hx;
//	ECP_BN158 tmp1,tmp2,tmp3;
//	ECP_BN158_copy(&tmp1,&g1);
//	PAIR_BN158_G1mul(&tmp1,ABA_Z[round][ID]);
//	ECP_BN158_copy(&tmp2,&VKs[ID]);
//	PAIR_BN158_G1mul(&tmp2,ABA_C[round][ID]);
//	ECP_BN158_neg(&tmp2);
//	ECP_BN158_add(&tmp1,&tmp2);
//	ECP_BN158_copy(&hx,&tmp1);
//		
//		
//		
//	//h_1 = ( g_1 ** z)/(sig ** c)
//	ECP_BN158 hx_1;
////	ECP_BN158 tmp1,tmp2;
//	ECP_BN158_copy(&tmp1,&C_point);
//	PAIR_BN158_G1mul(&tmp1,ABA_Z[round][ID]);
//	ECP_BN158_copy(&tmp2,&sigs);
//	PAIR_BN158_G1mul(&tmp2,ABA_C[round][ID]);
//	ECP_BN158_neg(&tmp2);
//	ECP_BN158_add(&tmp1,&tmp2);
//	ECP_BN158_copy(&hx_1,&tmp1);
//		
//		
////  c_1 = hashH(gg, self.VKs[i],h,g_1,sig,h_1)

//	for(int i=0;i<600;i++)ch[i] = 0;
//	for(int i=0;i<600;i++)o_ch[i] = 0x00;
//	o.val = o_ch;
//	o.max = 600;
//	ch_idx=0;
//	//gg
//	ECP_BN158_toOctet(&o,&g1,true);
//	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
//	//self.VKs[self.x]
//	ECP_BN158_toOctet(&o,&VKs[ID],true);
//	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
//	//h
//	ECP_BN158_toOctet(&o,&hx,true);
//	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
//	//g_1
//	ECP_BN158_toOctet(&o,&C_point,true);
//	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
//	//sig
//	ECP_BN158_toOctet(&o,&sigs,true);
//	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
//	//h_1
//	ECP_BN158_toOctet(&o,&hx_1,true);
//	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
//	
//	//ch[0] = 0x02;
//	
//	for(int i=0;i<50;i++)digest[i]=0x00;	
//	HASH256_init(&sh256);
//	for(int i=0;i<ch_idx;i++) HASH256_process(&sh256,ch[i]);
//	HASH256_hash(&sh256,digest);
//	
//	BIG_160_28 c_1;
//	BIG_160_28_fromBytesLen(c_1, digest,32);
//	BIG_160_28_mod(c_1,modl);
//	
//	if(BIG_160_28_comp(ABA_C[round][ID],c_1)==0){
//		sprintf((char*)Send_Data_buff,"1_verify_sig_share[]: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"1PK_verify_sig_share[]: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	if(ECP_BN158_equals(&h,&hx)){
//		sprintf((char*)Send_Data_buff,"2_verify_sig_share[]: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"2PK_verify_sig_share[]: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	if(ECP_BN158_equals(&h_1,&hx_1)){
//		sprintf((char*)Send_Data_buff,"3_verify_sig_share[]: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"3PK_verify_sig_share[]: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
	//while(1);
	
	
}

void tmp_Coin_SK_sign(u8 id, int round){
//	if(ACK_ABA_share[round][id])return;
//	//h ** sk
//	char digest[50];for(int i=0;i<50;i++)digest[i] = NULL;
//	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
//	sprintf((char*)Coin_Str,"Coin:%d",round);
//	hash256 sh256;
//	HASH256_init(&sh256);
//	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
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
//	if(Obj.len != Share_Size){while(1);}
//	for(int i=0;i<Obj.len;i++)
//		ABA_share[round][id][i] = Obj.val[i];
//	if(!ACK_ABA_share[round][id])ABA_share_Number[round]++;
//	ACK_ABA_share[round][id] = 1;
//	
////	BIG_160_28 num;
////	for(int i=0;i<players;i++){
////		FP_BN158_redc(num,&SKs[i]);
////		ECP_BN158_copy(&Sigs[i],&M_point);
////		PAIR_BN158_G1mul(&Sigs[i],num);
////	}
}
int Coin_PK_verify_sig_share(u8 id, u8 aba, u8 round, u8 *sig){

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
	char Coin_Str[32];for(int i=0;i<32;i++)Coin_Str[i] = NULL;
	sprintf((char*)Coin_Str,"Coin:%d%d%d",aba,round,Block[1][0][0]);
	hash256 sh256;
	HASH256_init(&sh256);
	for (int i=0;Coin_Str[i]!=0;i++) HASH256_process(&sh256,Coin_Str[i]);
    HASH256_hash(&sh256,digest);
	
	BIG_160_28 modl;
	BIG_160_28_rcopy(modl,CURVE_Order_BN158);
	ECP_BN158 C_point;
	ECP_BN158_copy(&C_point,&G1);
	BIG_160_28 C_num;
	BIG_160_28_fromBytes(C_num,digest);
	BIG_160_28_mod(C_num, modl);
	PAIR_BN158_G1mul(&C_point,C_num);
	
	ECP_BN158 share_point;
	octet Obj;
	Obj.len = 0;
	Obj.max = 70;
	Obj.val = (char*)sig;
	ECP_BN158_fromOctet(&share_point,&Obj);
	
	//h = ( gg ** z )/(self.VKs[i] ** c)
	ECP_BN158 hx;
	ECP_BN158 tmp1,tmp2,tmp3;
	ECP_BN158_copy(&tmp1,&g1);
	PAIR_BN158_G1mul(&tmp1,ABA_Z[aba][round][id]);
	ECP_BN158_copy(&tmp2,&VKs[id]);
	PAIR_BN158_G1mul(&tmp2,ABA_C[aba][round][id]);
	ECP_BN158_neg(&tmp2);
	ECP_BN158_add(&tmp1,&tmp2);
	ECP_BN158_copy(&hx,&tmp1);
		
		
		
	//h_1 = ( g_1 ** z)/(sig ** c)
	ECP_BN158 hx_1;
//	ECP_BN158 tmp1,tmp2;
	ECP_BN158_copy(&tmp1,&C_point);
	PAIR_BN158_G1mul(&tmp1,ABA_Z[aba][round][id]);
	ECP_BN158_copy(&tmp2,&share_point);
	PAIR_BN158_G1mul(&tmp2,ABA_C[aba][round][id]);
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
	ECP_BN158_toOctet(&o,&VKs[id],true);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//h
	ECP_BN158_toOctet(&o,&hx,true);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//g_1
	ECP_BN158_toOctet(&o,&C_point,true);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//sig
	ECP_BN158_toOctet(&o,&share_point,true);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	//h_1
	ECP_BN158_toOctet(&o,&hx_1,true);
	for(int i=0;i<o.len;i++){ch[ch_idx] = o.val[i];ch_idx++;}
	
	for(int i=0;i<50;i++)digest[i]=0x00;	
	HASH256_init(&sh256);
	for(int i=0;i<ch_idx;i++) HASH256_process(&sh256,ch[i]);
	HASH256_hash(&sh256,digest);
	
	BIG_160_28 c_1;
	BIG_160_28_fromBytesLen(c_1, digest,32);
	BIG_160_28_mod(c_1,modl);
	
	if(BIG_160_28_comp(ABA_C[aba][round][id],c_1)==0){
		return 1;
		sprintf((char*)Send_Data_buff,"_verify_sig_share[]: Yes");
		LoRa_SendData(Send_Data_buff);
	}else{
		return 0;
		sprintf((char*)Send_Data_buff,"PK_verify_sig_share[]: No");
		LoRa_SendData(Send_Data_buff);
	}
	

	
	
}


void Coin_PK_sig_combine_share(u8 aba, u8 round){
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
	ECP_BN158 Coin_Sigs[Nodes_N];
	octet Obj[MAX_Nodes];
	for(u8 i=0;i<MAX_Nodes;i++){
		Obj[i].len = Share_Size;
		Obj[i].max = Share_Size;
		Obj[i].val = (char*)ABA_share[aba][round][i];
	}
	for(u8 i=1;i<=Nodes_N;i++){
		if(ACK_ABA_share[aba][round][i]){
			S[idx] = i-1;
			ECP_BN158_fromOctet(&Coin_Sigs[i-1],&Obj[i]);
			idx++;
		}
	}
	
	BIG_160_28 larg;
	ECP_BN158 tmp;
	ECP_BN158 r[20];
	
	for(int j=0 ; j<Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = S[j];
		lagrange(larg,jj);
		ECP_BN158_copy(&tmp,&Coin_Sigs[jj]);
		PAIR_BN158_G1mul(&tmp,larg);		
		ECP_BN158_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Thres_k ; j++){
		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
	
	char ch[70];for(int i=0;i<70;i++)ch[i] = 0x00;
	octet OBJ;
	OBJ.len = 0;
	OBJ.max = 70;
	OBJ.val = ch;
	
	ECP_BN158_toOctet(&OBJ,&r[0],true);
	Shared_Coin[aba][round] = OBJ.val[OBJ.len-1] % 2;
	for(int i=0;i<OBJ.len;i++)
		COIN[i] = OBJ.val[i];
	
	
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
	
//	BIG_160_28 q;
//	BIG_160_28_rcopy(q, CURVE_Order_BN158);
//	BIG_160_28_sdiv(num , den);
//	BIG_160_28_copy(r,num);
	//BIG_160_28_moddiv(r, num , den , q);
	//BIG_160_28_norm(r);
//	BIG_160_28_inv(tmp, den,NULL);
//	BIG_160_28_mul(r, num, tmp);

}


void dealer(){
	
//	char raw[100];
//    octet RAW = {0, sizeof(raw), raw};
//    csprng RNGx;                // Crypto Strong RNG
//	u32 ran = RNG_Get_RandomNum();	
//    RAW.len = 100;              // fake random seed source
//    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
//    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
//	BIG_160_28 secret, r;
//	BIG_160_28_rcopy(r, CURVE_Order_BN158);
//	BIG_160_28_randomnum(secret, r, &RNGx);
//	
//	BIG_160_28_copy(a[0],secret);
//	BIG_160_28_copy(SECRET,secret);

//	for(u16 i=1;i<k;i++){
//		BIG_160_28_randomnum(a[i], r, &RNGx);
//	}
//	
//	for(u16 i=0;i<players;i++){
//		BIG_160_28 x;
//		BIG_160_28_zero(x);
//		BIG_160_28_inc(x,i+1);
//		ff(SKs[i],x);
//	}
//	
//	//test bytes
//	char tmp_ch[300];
//	for(int i=0;i<300;i++)tmp_ch[i] = 0x00;
//	BIG_160_28_toBytes(tmp_ch,SKs[0]);
//	BIG_160_28_fromBytes(SKs[0],tmp_ch);
//	//end
//	
//	//assert ff(0) == secret
//	BIG_160_28 secret1;
//	BIG_160_28 ZERO;
//	BIG_160_28_zero(ZERO);
//	ff(secret1,ZERO);
//	
//	//VK = g2 ** secret
//	ECP2_BN158_copy(&VK,&g2);
//	PAIR_BN158_G2mul(&VK,secret);
//	
//	//VKs = [g2 ** xx for xx in SKs ]
//	for(u16 i=0 ; i<players ; i++){
//		ECP2_BN158_copy(&VKs[i],&g2);
//		PAIR_BN158_G2mul(&VKs[i],SKs[i]);	
//	}
	
	
	/**********************************************/	
//	//VK1
//	//VKs1
//	ECP_BN158_copy(&VK1,&g1);
//	PAIR_BN158_G1mul(&VK1,secret);
//	//VKs = [g2 ** xx for xx in SKs ]
//	for(u16 i=0 ; i<players ; i++){
//		ECP_BN158_copy(&VKs1[i],&g1);
//		PAIR_BN158_G1mul(&VKs1[i],SKs[i]);	
//	}
//	ECP_BN158 VK1;
//	ECP_BN158_copy(&VK1,&g1);
//	PAIR_BN158_G1mul(&VK1,secret);
	/*********************************************/
	
	/******************output it**************************/
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
	
	octet RAWx[MAX_Nodes];
	for(int i=0;i<MAX_Nodes;i++){
		RAWx[i].max = 200;
		RAWx[i].len = public_key_idx;
		RAWx[i].val = vch[i];
	}
	
	for(int i=1;i<=players;i++){
		BIG_160_28_fromBytes(SKs[i],sch[i]);
	}
	for(int i=1;i<=players;i++){
		ECP_BN158_fromOctet(&VKs1[i],&RAWx[i]);
	}
	ECP_BN158_fromOctet(&VK1,&RAWx[Nodes_N+1]);
	/***************************************************/
}





///







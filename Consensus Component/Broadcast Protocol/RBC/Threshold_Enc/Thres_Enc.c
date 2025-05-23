#include "Thres_Enc.h"
#include "RBC.h"

//char thres_enc_private_key_string[Nodes_N][key_num1] ={
//"4a 7a a9 52 9c 6b bb b9 65 35 c4 0f dd b9 cc 1c d2 70 dc fe 24 6d 20 e3 f7 ac a2 c6 e1 48 f4 3c 00 00 00 00 00 00 00 00", 
//"52 27 5c 92 c7 de 66 69 8f 31 7f b5 b5 e5 cd c5 3d 73 ba 5a 90 84 53 fa 53 88 24 ec 9a 52 38 36 00 00 00 00 00 00 00 00", 
//"3a 20 94 87 9a 52 79 54 b0 b9 17 76 f2 4f 3f 2f 79 68 17 bc 74 d6 16 d0 8a 6f 43 9c dc e8 e2 84 00 00 00 00 00 00 00 00", 
//"4c ad 1a 35 93 c7 f4 7e 3e 35 26 53 92 f6 20 6b 85 8c f5 23 d1 62 69 87 de 61 fe d7 a9 0c f3 40 00 00 00 00 00 00 00 00" 
//};
//char thres_enc_public_key_string[Nodes_N+1][key_num2] = {
//"03 0e 3c 4f 6a e5 c1 b1 37 e5 35 73 6b c4 60 08 da 7d 27 6e 67 27 f3 5b 29 18 d0 4c b0 d5 0e 21 e8 15 c1 6d fe f0 93 89 35 da 3b bb 29 16 54 cd 6e d1 7e b9 56 d3 96 6c 7d a5 ad bf d6 e1 a2 65 61", 
//"02 06 32 9b 40 f2 06 0c ce c1 f1 2b 1f 78 21 82 57 f3 30 d8 10 60 52 8a 34 e3 ae dc bf ab 2e 01 6e 18 15 8b 40 42 d4 5a c5 cc b1 b6 dd be 14 b7 f0 65 c4 0a cf 5c f4 07 11 1a 4d cc 7d d7 7e 31 24", 
//"02 1a 22 e0 b6 37 b3 6d e2 6f 74 2d 66 62 dd c7 85 72 c5 15 01 1a 3a 1b 5c b0 28 52 fa b4 89 01 07 19 44 44 77 a1 f2 81 d0 98 27 3d ed f2 12 1c 87 16 a7 49 e7 af ab 2e d6 af 53 bd b9 68 44 b2 e6", 
//"03 23 90 7d da 87 0f 50 97 32 aa 27 b4 cf 09 61 1e 82 fd ac 81 16 d7 e8 5b eb a9 b3 ec 9c 54 a4 47 0e 10 89 e5 70 60 7c 1a 37 5f 41 f1 56 f7 8e 5c d5 26 e2 5e 41 63 77 92 8c e1 30 9b b2 99 b7 a1", 
//"02 18 0f f5 00 66 5c 53 18 7e 34 57 be 93 75 1f a4 00 6d e6 ca 41 d0 d3 0e c2 8c 8e 9c c6 43 3e c6 06 a0 ae df 5a c0 a2 71 5b dc ab 87 4b 89 2e 17 e4 c9 bc 2c 48 e6 dc 39 84 c2 db 5f b4 07 69 74" 
//};


char thres_enc_private_key_string[Nodes_N][key_num1] ={
"34 2d f4 ad 8b 0e 7c 1f 24 c9 81 25 23 2d a7 26 7e 63 bc 90 a4 1b 0e 4a a3 ba e1 7d 58 f5 1b 44 00 00 00 00 00 00 00 00", 
"31 d5 a8 c2 fe 3d 6e ed a1 d8 70 08 2f 30 53 f9 b9 9e 0a 88 4e 33 ca 5c 08 fc 8e 41 cf 2b 16 17 00 00 00 00 00 00 00 00", 
"24 18 1f 8a 8e 3b 96 af b1 b1 18 44 30 05 aa 78 67 2a c6 91 21 c2 31 52 7f 24 07 c6 b4 09 f9 42 00 00 00 00 00 00 00 00", 
"30 18 bd 86 7b 08 f3 67 0e 87 c7 59 25 ad aa aa 86 a9 70 ab 1e c6 43 3e a7 31 4e 0c 07 91 c4 d2 00 00 00 00 00 00 00 00"
};
char thres_enc_public_key_string[Nodes_N+1][key_num2] = {
"02 06 ca 35 33 9d 6c 3a f1 bb 27 15 a9 d7 fc e4 aa 68 5f ba 45 be 77 07 39 fd 64 2f 51 d2 4e 70 93 14 57 cc c3 a5 25 af 22 62 82 fc 5f dd a4 2a 32 1f 5c ee c2 3b e1 82 24 40 4e 2a af 83 37 b8 94", 
"03 17 4e 93 99 9e 18 07 4a 72 f7 96 0e 20 1f be e8 9d 61 cf a2 9c a8 be 29 89 f7 7b c1 9c 3f dc 51 06 22 0d 92 d8 a5 00 d6 41 17 89 8b 74 ba 8c 26 10 53 f6 08 d0 cf 2c eb 14 92 a5 31 f5 3c 04 6b", 
"02 1c 4d c4 a8 b3 7a c5 72 05 d8 5e 11 2e b3 f8 e6 10 8c 23 36 f3 6b cc cd e0 ec 9c 0a aa 53 07 e9 05 d9 4b 63 58 62 bf 8a a6 77 07 b7 94 56 05 3c f4 62 f8 aa 6c 68 c3 e4 dd 66 0f 8c c3 32 49 e3", 
"03 17 96 de 23 28 06 c1 98 93 4f ac 86 a1 c3 23 e1 2a a1 ba 87 17 6e c4 ee 20 98 3d 44 22 36 ba 47 11 ed c7 9e ca b7 a4 89 d7 7a 01 5c 95 c6 2e 1d 80 d2 22 4a b3 0f d1 e4 18 52 76 bf c1 a2 4a 37", 
"02 04 71 ef 74 ca b4 4a ad ba 24 cf d4 8f 30 12 dc 21 02 38 9e a7 94 4d 18 cb e1 6e 53 5f a4 db 47 0a ce a5 76 ca 5b af 28 69 21 da 96 c9 9b 12 be 62 32 47 50 7a dc 39 0e 11 91 b2 38 53 93 68 6a"
};

u8 Enc_players = Nodes_N;
u8 Enc_Thres_k = 2 * Nodes_f + 1;

BIG_256_28 Enc_SKs[MAX_SK];

//ECP2_BN254 g2;
ECP2_BN254 Enc_VK;
ECP2_BN254 Enc_VKs[MAX_SK];

//ECP_BN254 M_point;
//ECP_BN254 Sigs[MAX_SK];
//ECP_BN254 SIG;

/*************************Threshold Encryption****************************************/
//FP_BN254 a[MAX_K];
//FP_BN254 SKs[MAX_SK];
u8 Enc_players;
//u8 k;
ECP_BN254 Enc_G1;
ECP2_BN254 Enc_G2;
ECP_BN254 Enc_g1;
ECP2_BN254 Enc_g2;
//ECP2_BN254 VK;
//ECP2_BN254 VKs[MAX_SK];
u8 Enc_S[MAX_SK];
BIG_256_28 R_num;
BIG_256_28 R;
//FP_BN254 SECRET;

ECP_BN254 RES;

ECP2_BN254 g12;
FP12_BN254 pair1;
ECP_BN254 g21;
FP12_BN254 pair2;
ECP_BN254 U;
ECP_BN254 shares[MAX_SK];
char V[50];
ECP2_BN254 W,H;

char Enc_M[50];
char Enc_M_[50];


void PK_encrypt(char* m);
void PK_verify_ciphertext();
void SK_decrypt_share();
void PK_verify_share();
void PK_combine_share();
void testdemo();
void Enc_ff(BIG_256_28 r,BIG_256_28 x);
void Enc_lagrange(BIG_256_28 r,u8 j);
void Enc_dealer();
BIG_256_28 Enc_g1_num;

//single hop//////////////////////////////////////////////////////////////////////////////////////////////////////////

void Tx_PK_encrypt(char* m, char *enc);
void Tx_PK_verify_ciphertext();
void Tx_SK_decrypt_share(u8 tx_id, u8 id);
int Tx_PK_verify_share(u8 tx_id, u8 id);
void Tx_PK_combine_share(u8 tx_id, char * enc, char *dec);

void init_thres_enc(){

	//sha256 init
	char digest[50];
	hash256 sh256;
	int i;
//	u32 lenn = NLEN_256_28;
//	int32_t gg[lenn];
	
	
	char mc[] = "geng1";
	for(int i=0;i<50;i++)digest[i] = NULL;
	HASH256_init(&sh256);
	for (i=0;mc[i]!=0;i++) HASH256_process(&sh256,mc[i]);
    HASH256_hash(&sh256,digest);
	BIG_256_28_fromBytes(Enc_g1_num,digest);
	
	if(!ECP_BN254_generator(&Enc_G1)){
		sprintf((char*)Send_Data_buff,"Enc_G1 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	
	ECP_BN254_copy(&Enc_g1,&Enc_G1);
	PAIR_BN254_G1mul(&Enc_g1,Enc_g1_num);
	//ECP_BN254_copy(&g2,&Enc_g1);
	
	if (!ECP2_BN254_generator(&Enc_G2)){
		sprintf((char*)Send_Data_buff,"Enc_G2 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	ECP2_BN254_copy(&Enc_g2,&Enc_G2);
	PAIR_BN254_G2mul(&Enc_g2,Enc_g1_num);
	
	
	//////////////////////////////////////

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
		for(string_idx = 0; string_idx < sizeof(thres_enc_public_key_string[l-1]);){
			vch[l][key_idx] = Sixteen2Ten(thres_enc_public_key_string[l-1][string_idx])*16+Sixteen2Ten(thres_enc_public_key_string[l-1][string_idx + 1]);
			key_idx = key_idx + 1;
			string_idx = string_idx + 3;
		}
		public_key_idx = key_idx;
		
		key_idx = 0;
		for(string_idx = 0; string_idx < sizeof(thres_enc_private_key_string[l-1]);){
			sch[l][key_idx] = Sixteen2Ten(thres_enc_private_key_string[l-1][string_idx])*16 + Sixteen2Ten(thres_enc_private_key_string[l-1][string_idx + 1]);
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
	
	
	for(int i=1;i<=Enc_players;i++){
		BIG_256_28_fromBytes(Enc_SKs[i],sch[i]);
	}
	for(int i=1;i<=Enc_players;i++){
		ECP2_BN254_fromOctet(&Enc_VKs[i],&RAW[i]);
	}
	ECP2_BN254_fromOctet(&Enc_VK,&RAW[Nodes_N+1]);
	//dealer();
	//while(1);
	//Enc_dealer();
	
	for(u8 i=0;i<20;i++)Enc_S[i] = i;
	
	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	//char digest[50];
	//hash256 sh256;
	
	for(int i=0;i<50;i++)digest[i] = NULL;
	HASH256_init(&sh256);
	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
    HASH256_hash(&sh256,digest);
	
	for(int i=0;i<50;i++)Enc_M[i]=digest[i];
	
	BIG_256_28 M_num;
	BIG_256_28_fromBytes(M_num,M);
	ECP_BN254_copy(&M_point,&Enc_G1);
	PAIR_BN254_G1mul(&M_point,M_num);
	
	
//	u8 tx[32];
//	u8 enc_tx[100];
//	u8 dec_tx[100];
//	for(int i=0;i<100;i++)enc_tx[i] = dec_tx[i] = 0x00;
//	
//	for(int i=0;i<32;i++)tx[i] = 'a';
//	Tx_PK_encrypt(tx,enc_tx);
//	//Tx_PK_verify_ciphertext();
//	Tx_SK_decrypt_share();
//	Tx_PK_verify_share();
//	Tx_PK_combine_share(ID, enc_tx, dec_tx);
	
	//while(1);
	
//void Tx_PK_encrypt(char* m, char *enc);
//void Tx_PK_verify_ciphertext();
//void Tx_SK_decrypt_share();
//void Tx_PK_verify_share();
//void Tx_PK_combine_share(u8 tx_id, char * enc, char *dec);
	
//	PK_encrypt(digest);
//	PK_verify_ciphertext();
//	SK_decrypt_share();
//	PK_verify_share();
//	PK_combine_share();
}




void Tx_PK_encrypt(char* m, char *enc){
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

	char raw[100];
    octet RAW = {0, sizeof(raw), raw};
    csprng RNGx;                // Crypto Strong RNG
	u32 ran = RNG_Get_RandomNum();
    RAW.len = 100;              // fake random seed source
    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
	BIG_256_28 modl;
	BIG_256_28_rcopy(modl, CURVE_Order_BN254);
	
	BIG_256_28 r;
	BIG_256_28_randomnum(r,modl,&RNGx);
	
	//	U = g1 ** r
	
	ECP_BN254_copy(&Tx_U[ID],&Enc_g1);
	PAIR_BN254_G1mul(&Tx_U[ID],r);
	
////	//test
//	ECP_BN254_copy(&shares[3],&Tx_U[ID]);
//	PAIR_BN254_G1mul(&shares[3],Enc_SKs[2]);
//	
//	ECP2_BN254 g2_neg;
//	ECP2_BN254_copy(&g2_neg,&Enc_g2);
//	ECP2_BN254_neg(&g2_neg);
//	
//	//pair(U_i, g2) == pair(U, Y_i)
//	FP12_BN254 v;
//	//PAIR_BN254_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//	PAIR_BN254_double_ate(&v,&g2_neg,&shares[3],&Enc_VKs[2],&Tx_U[ID]);
//	PAIR_BN254_fexp(&v);
//	
//	if (FP12_BN254_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"ddPK_verify_share[%d]: Yes",3);
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"ddPK_verify_share[%d]: No",3);
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	//	U = g1 ** r
//	ECP_BN254_copy(&U,&Enc_g1);
//	PAIR_BN254_G1mul(&U,r);
//	
//	//test
//	ECP_BN254_copy(&shares[3],&U);
//	PAIR_BN254_G1mul(&shares[3],Enc_SKs[4]);
//	
////	ECP2_BN254 g2_neg;
//	ECP2_BN254_copy(&g2_neg,&Enc_g2);
//	ECP2_BN254_neg(&g2_neg);
//	
//	//pair(U_i, g2) == pair(U, Y_i)
////	FP12_BN254 v;
//	//PAIR_BN254_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//	PAIR_BN254_double_ate(&v,&g2_neg,&shares[3],&Enc_VKs[4],&U);
//	PAIR_BN254_fexp(&v);
//	
//	if (FP12_BN254_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"ddPK_verify_share[%d]: Yes",3);
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"ddPK_verify_share[%d]: No",3);
//		LoRa_SendData(Send_Data_buff);
//	}
//	while(1);
////	//end
	
	octet U_oct;				//Res->oct
	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
	U_oct.val = U_ch;
	ECP_BN254_toOctet(&U_oct,&Tx_U[ID],true);
	for(int i=0;i<Share_Size;i++)Tx_U_str[ID][i] = U_ch[i];
	
	BIG_256_28_copy(R_num,r);
	
///////V = xor(m, hashG(self.VK ** r))
	//改成下面的
	//#V = xor(m, hashG(pair(g1, self.VK ** r)))
	
	ECP_BN254 g1_neg;
	ECP_BN254_copy(&g1_neg,&Enc_g1);
	ECP_BN254_neg(&g1_neg);
	
	ECP2_BN254 VKr;				//VK ** r
	ECP2_BN254_copy(&VKr,&Enc_VK);
	PAIR_BN254_G2mul(&VKr,r);
	
	FP12_BN254 pair_r;//pair(g1, self.VK ** r)
	PAIR_BN254_ate(&pair_r,&VKr,&Enc_g1);
	PAIR_BN254_fexp(&pair_r);
	//testdemo();
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
		enc[i] = m[i]^digest[i];
	}
	enc[32] = NULL;
	
	//u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_encrypt_time[Run_idx] = time;

	//	sprintf((char*)Send_Data_buff,"Encrypt Time:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
//	/**************************Encrypt end**********************************/
//	
////	W = hashH(U, V) ** r
//	octet U_oct;				//U->oct
//	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
//	U_oct.val = U_ch;
//	ECP_BN254_toOctet(&U_oct,&Tx_U[ID],true);
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
//	BIG_256_28 W_num;
//	BIG_256_28_fromBytes(W_num,digest1);
//	
//	
//	ECP2_BN254_copy(&Tx_W[ID],&Enc_G2);
//	PAIR_BN254_G2mul(&Tx_W[ID],W_num);
//	ECP2_BN254_copy(&Tx_H[ID],&Tx_W[ID]); //	H
//	PAIR_BN254_G2mul(&Tx_W[ID],r_num);
//	
//	ECP_BN254_copy(&g1_neg,&Enc_g1);
//	ECP_BN254_neg(&g1_neg);
//	
//	FP12_BN254 v;
//	PAIR_BN254_double_ate(&v,&W,&g1_neg,&H,&U);
//	PAIR_BN254_fexp(&v);
//    if (FP12_BN254_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
//	// test single fail 下面的
//	FP12_BN254 r1,r2;
//	
//	PAIR_BN254_ate(&r1,&W,&Enc_g1);
//	PAIR_BN254_ate(&r2,&H,&U);
//	
//	PAIR_BN254_fexp(&r1);
//	PAIR_BN254_fexp(&r2);
//	
//	if(FP12_BN254_equals(&r1,&r2)){
//		sprintf((char*)Send_Data_buff,"Test single verify: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test single verify: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
}











void Tx_PK_verify_ciphertext(){

}

void Tx_SK_decrypt_share(u8 tx_id, u8 id){
	// id -> tx_id

	ECP_BN254_copy(&Tx_shares[tx_id][id-1],&Tx_U[tx_id]);
	PAIR_BN254_G1mul(&Tx_shares[tx_id][id-1],Enc_SKs[id]);
	
	octet S_oct;				//Res->oct
	char S_ch[100];for(int i=0;i<100;i++)S_ch[i]=NULL;
	S_oct.val = S_ch;
	ECP_BN254_toOctet(&S_oct,&Tx_shares[tx_id][id-1],true);
	for(int i=0;i<Share_Size;i++)Tx_shares_str[tx_id][id-1][i] = S_ch[i];
	
	
//	ECP2_BN254 g2_neg;
//	ECP2_BN254_copy(&g2_neg,&Enc_g2);
//	ECP2_BN254_neg(&g2_neg);
//	
//	//pair(U_i, g2) == pair(U, Y_i)
//	FP12_BN254 v;
//	//PAIR_BN254_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//	PAIR_BN254_double_ate(&v,&g2_neg,&Tx_shares[tx_id][id-1],&Enc_VKs[id],&Tx_U[tx_id]);
//	PAIR_BN254_fexp(&v);
	
//	if (FP12_BN254_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: Yes",id);
//		LoRa_SendData(Send_Data_buff);
//		//return 1;
//	}else{
//		while(1){	
//			sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: No %d",tx_id, id);
//			LoRa_SendData(Send_Data_buff);
//			delay_ms(1000);
//		}
//		//return 0;
//	}
}

int Tx_PK_verify_share(u8 tx_id, u8 id){
	//id -> tx_id
	
	ECP2_BN254 g2_neg;
	ECP2_BN254_copy(&g2_neg,&Enc_g2);
	ECP2_BN254_neg(&g2_neg);
	
	//pair(U_i, g2) == pair(U, Y_i)
	FP12_BN254 v;
	//PAIR_BN254_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
	PAIR_BN254_double_ate(&v,&g2_neg,&Tx_shares[tx_id][id-1],&Enc_VKs[id],&Tx_U[tx_id]);
	PAIR_BN254_fexp(&v);
	
	if (FP12_BN254_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: Yes",id);
//		LoRa_SendData(Send_Data_buff);
		return 1;
	}else{
		while(1){	
			sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: No %d",tx_id, id);
			LoRa_SendData(Send_Data_buff);
			delay_ms(1000);
		}
		return 0;
	}
	
	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	ECP2_BN254 g2_neg;
//	ECP2_BN254_copy(&g2_neg,&Enc_g2);
//	ECP2_BN254_neg(&g2_neg);
//	
//	for(int i=1;i<=Enc_players;i++){
//		//pair(U_i, g2) == pair(U, Y_i)
//		FP12_BN254 v;
//		//PAIR_BN254_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//		PAIR_BN254_double_ate(&v,&g2_neg,&Tx_shares[ID][i-1],&Enc_VKs[i],&Tx_U[ID]);
//		PAIR_BN254_fexp(&v);
//		
//		if (FP12_BN254_isunity(&v)){
//			sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: Yes",i);
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: No",i);
//			LoRa_SendData(Send_Data_buff);
//		}
//	}
}

void Tx_PK_combine_share(u8 tx_id, char * enc, char *dec){
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
	
	BIG_256_28 larg;
	ECP_BN254 tmp;
	ECP_BN254 r[20];
	ECP_BN254 res;
	
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_Share[tx_id][k] == 0)
			Tx_SK_decrypt_share(tx_id, k);
	}
	
	for(int j=0 ; j<Enc_Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = Enc_S[j];
		Enc_lagrange(larg,jj);
		ECP_BN254_copy(&tmp,&Tx_shares[tx_id][jj]);
		PAIR_BN254_G1mul(&tmp,larg);
		ECP_BN254_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Enc_Thres_k ; j++){
		ECP_BN254_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
	ECP_BN254_copy(&res,&r[0]);	
	
//	//PAIR_BN254_G1member
//	if(PAIR_BN254_G1member(&res)){
//		sprintf((char*)Send_Data_buff,"Test Res Enc_G1 member: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test Res Enc_G1 member: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	//PAIR_BN254_G1member
//	if(ECP_BN254_equals(&RES,&res)){
//		sprintf((char*)Send_Data_buff,"RES = res: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"RES = res: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
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
	PAIR_BN254_ate(&pair_r,&Enc_g2,&res);
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
	
	
////	octet Res_oct;				//Res->oct
////	char Res_ch[100];for(int i=0;i<100;i++)Res_ch[i]=NULL;
////	Res_oct.val = Res_ch;
////	ECP_BN254_toOctet(&Res_oct,&res,true);
////	
////	hash256 sh256;//hash VKr_oct  
////	HASH256_init(&sh256);
////	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
////	for (int i=0;Res_oct.val[i]!=0;i++) HASH256_process(&sh256,Res_oct.val[i]);
////	HASH256_hash(&sh256,digest);
	
	char m_[100];for(int i=0;i<100;i++)m_[i]=NULL;
	
	for(int i=0;i<32;i++){
		dec[i] = digest[i]^enc[i];
	}	
	
	//while(1);
	
//	for(int i=0;i<50;i++)	Enc_M_[i] = m_[i];
//	
//	int ress = 1;
//	for(int i=0;i<32;i++){
//		if(Enc_M[i]!=Enc_M_[i])ress=0;
//	}
	
	
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_combine_share_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"Combine shares: %d",time);
//	LoRa_SendData(Send_Data_buff);
	
//	/**************************Combine shares**********************************/
//	
//	if(ress){
//		sprintf((char*)Send_Data_buff,"Combine shares: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Combine shares: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//		SS = range(PK.l)
//		for i in range(1):
//      random.shuffle(SS)
//      S = set(SS[:PK.k])
//      
//      m_ = PK.combine_shares(C, dict((s,shares[s]) for s in S))
//      assert m_ == m
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
	
	BIG_256_28 modl;
	BIG_256_28_rcopy(modl, CURVE_Order_BN254);
	
	BIG_256_28 r;
	BIG_256_28_randomnum(r,modl,&RNGx);
	//BIG_256_28_copy(R,r);

	//	U = g1 ** r
	ECP_BN254_copy(&U,&Enc_g1);
	PAIR_BN254_G1mul(&U,r);
	
	//test
	ECP_BN254_copy(&shares[3],&U);
	PAIR_BN254_G1mul(&shares[3],Enc_SKs[3]);
	
	ECP2_BN254 g2_neg;
	ECP2_BN254_copy(&g2_neg,&Enc_g2);
	ECP2_BN254_neg(&g2_neg);
	
	//pair(U_i, g2) == pair(U, Y_i)
	FP12_BN254 v;
	//PAIR_BN254_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
	PAIR_BN254_double_ate(&v,&g2_neg,&shares[3],&Enc_VKs[3],&U);
	PAIR_BN254_fexp(&v);
	
	if (FP12_BN254_isunity(&v)){
		sprintf((char*)Send_Data_buff,"ddPK_verify_share[%d]: Yes",3);
		LoRa_SendData(Send_Data_buff);
	}else{
		sprintf((char*)Send_Data_buff,"ddPK_verify_share[%d]: No",3);
		LoRa_SendData(Send_Data_buff);
	}
	while(1);
	//end
	
	
	
	
	
	
	
	
	
	
	
	
	
	//BIG_256_28_copy(R_num,r);
	
///////V = xor(m, hashG(self.VK ** r))
	
	//#V = xor(m, hashG(pair(g1, self.VK ** r)))
	
	ECP_BN254 g1_neg;
	ECP_BN254_copy(&g1_neg,&Enc_g1);
	ECP_BN254_neg(&g1_neg);
	
	ECP2_BN254 VKr;				//VK ** r
	ECP2_BN254_copy(&VKr,&Enc_VK);
	PAIR_BN254_G2mul(&VKr,r);
	
	FP12_BN254 pair_r;//pair(g1, self.VK ** r)
	PAIR_BN254_ate(&pair_r,&VKr,&Enc_g1);
	PAIR_BN254_fexp(&pair_r);
	//testdemo();
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
	
	//u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_encrypt_time[Run_idx] = time;

	//	sprintf((char*)Send_Data_buff,"Encrypt Time:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
//	/**************************Encrypt end**********************************/
//	
////	W = hashH(U, V) ** r
//	octet U_oct;				//U->oct
//	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
//	U_oct.val = U_ch;
//	ECP_BN254_toOctet(&U_oct,&U,true);
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
//	BIG_256_28 W_num;
//	BIG_256_28_fromBytes(W_num,digest1);
//	
//	
//	ECP2_BN254_copy(&W,&Enc_G2);
//	PAIR_BN254_G2mul(&W,W_num);
//	ECP2_BN254_copy(&H,&W); //	H
//	PAIR_BN254_G2mul(&W,r_num);
//	
//	ECP_BN254_copy(&g1_neg,&Enc_g1);
//	ECP_BN254_neg(&g1_neg);
//	
//	FP12_BN254 v;
//	PAIR_BN254_double_ate(&v,&W,&g1_neg,&H,&U);
//	PAIR_BN254_fexp(&v);
//    if (FP12_BN254_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
//	// test single fail 下面的
//	FP12_BN254 r1,r2;
//	
//	PAIR_BN254_ate(&r1,&W,&Enc_g1);
//	PAIR_BN254_ate(&r2,&H,&U);
//	
//	PAIR_BN254_fexp(&r1);
//	PAIR_BN254_fexp(&r2);
//	
//	if(FP12_BN254_equals(&r1,&r2)){
//		sprintf((char*)Send_Data_buff,"Test single verify: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test single verify: No");
//		LoRa_SendData(Send_Data_buff);
//	}
		
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

	ECP2_BN254_copy(&H,&Enc_G2);
	PAIR_BN254_G2mul(&H,H_num);
	
	
	ECP_BN254 g1_neg;
	ECP_BN254_copy(&g1_neg,&Enc_g1);
	ECP_BN254_neg(&g1_neg);
	
	FP12_BN254 v;
	PAIR_BN254_double_ate(&v,&W,&g1_neg,&H,&U);
	PAIR_BN254_fexp(&v);
	
	//u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_ciphertext_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"Verify Ciphertext Time:%d",time);
//	LoRa_SendData(Send_Data_buff);
	
	/**************************verify end**********************************/
	
//    if (FP12_BN254_isunity(&v)){
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
	
	BIG_256_28 sk_num;
	for(int i=1;i<=Enc_players;i++){
		ECP_BN254_copy(&shares[i-1],&U);
		PAIR_BN254_G1mul(&shares[i-1],Enc_SKs[i-1]);
	}
	
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	SK_decrypt_share_time[Run_idx] = time;
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
	
	ECP2_BN254 g2_neg;
	ECP2_BN254_copy(&g2_neg,&Enc_g2);
	ECP2_BN254_neg(&g2_neg);
	
	for(int i=1;i<=Enc_players;i++){
		//pair(U_i, g2) == pair(U, Y_i)
		FP12_BN254 v;
		//PAIR_BN254_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
		PAIR_BN254_double_ate(&v,&g2_neg,&shares[i-1],&Enc_VKs[i-1],&U);
		PAIR_BN254_fexp(&v);
		
//		if (FP12_BN254_isunity(&v)){
//			sprintf((char*)Send_Data_buff,"PK_verify_share[%d]: Yes",i);
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"PK_verify_share[%d]: No",i);
//			LoRa_SendData(Send_Data_buff);
//		}
	}
	while(1);
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_share_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"PK_verify_share%d:%d",Enc_players,time);
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
	
	BIG_256_28 larg;
	ECP_BN254 tmp;
	ECP_BN254 r[20];
	ECP_BN254 res;

	for(int j=0 ; j<Enc_Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = Enc_S[j];
		Enc_lagrange(larg,jj);
		ECP_BN254_copy(&tmp,&shares[jj]);
		PAIR_BN254_G1mul(&tmp,larg);

		ECP_BN254_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Enc_Thres_k ; j++){
		ECP_BN254_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
	ECP_BN254_copy(&res,&r[0]);	
	
//	//PAIR_BN254_G1member
//	if(PAIR_BN254_G1member(&res)){
//		sprintf((char*)Send_Data_buff,"Test Res Enc_G1 member: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test Res Enc_G1 member: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	//PAIR_BN254_G1member
//	if(ECP_BN254_equals(&RES,&res)){
//		sprintf((char*)Send_Data_buff,"RES = res: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"RES = res: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
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
	PAIR_BN254_ate(&pair_r,&Enc_g2,&res);
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
	
	
////	octet Res_oct;				//Res->oct
////	char Res_ch[100];for(int i=0;i<100;i++)Res_ch[i]=NULL;
////	Res_oct.val = Res_ch;
////	ECP_BN254_toOctet(&Res_oct,&res,true);
////	
////	hash256 sh256;//hash VKr_oct  
////	HASH256_init(&sh256);
////	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
////	for (int i=0;Res_oct.val[i]!=0;i++) HASH256_process(&sh256,Res_oct.val[i]);
////	HASH256_hash(&sh256,digest);
	
	char m_[100];for(int i=0;i<100;i++)m_[i]=NULL;
	
	for(int i=0;i<32;i++){
		m_[i] = digest[i]^V[i];
	}	
	
	for(int i=0;i<50;i++)	Enc_M_[i] = m_[i];
	
	int ress = 1;
	for(int i=0;i<32;i++){
		if(Enc_M[i]!=Enc_M_[i])ress=0;
	}
	
	
//	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_combine_share_time[Run_idx] = time;
//	sprintf((char*)Send_Data_buff,"Combine shares: %d",time);
//	LoRa_SendData(Send_Data_buff);
	
//	/**************************Combine shares**********************************/
//	
//	if(ress){
////		sprintf((char*)Send_Data_buff,"Combine shares: Yes");
////		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Combine shares: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//		SS = range(PK.l)
//		for i in range(1):
//      random.shuffle(SS)
//      S = set(SS[:PK.k])
//      
//      m_ = PK.combine_shares(C, dict((s,shares[s]) for s in S))
//      assert m_ == m
	
}

void testdemo(){
//	//test demo
//	//pair1(g2*secret*r,g1) == pair2(g2,g1*secret*r)
//	//R_num
//	//SECRET
//	BIG_256_28 Secret_num;
//	FP_BN254_redc(Secret_num,&SECRET);
//	//pair(g2*secret*r,g1)
//	ECP2_BN254_copy(&g12,&g2);
//	PAIR_BN254_G2mul(&g12,R_num);
//	PAIR_BN254_G2mul(&g12,Secret_num);
//	PAIR_BN254_ate(&pair1,&g12,&g1);
//	PAIR_BN254_fexp(&pair1);
//	
//	//pair(g2,g1*secret*r)
//	ECP_BN254_copy(&g21,&g1);
//	PAIR_BN254_G1mul(&g21,R_num);
//	PAIR_BN254_G1mul(&g21,Secret_num);
//	PAIR_BN254_ate(&pair2,&g2,&g21);
//	PAIR_BN254_fexp(&pair2);
//	
//	if(FP12_BN254_equals(&pair2,&pair1)){
//		sprintf((char*)Send_Data_buff,"Test g12 == g21: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test g12 == g21: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	octet pair1_r_oct;				//VKr->oct
//	char pair1_r_ch[400];for(int i=0;i<400;i++)pair1_r_ch[i]=NULL;
//	pair1_r_oct.val = pair1_r_ch;
//	FP12_BN254_toOctet(&pair1_r_oct,&pair1);
//	
//	hash256 sh256;//hash VKr_oct  
//	HASH256_init(&sh256);
//	char digest1[50];for(int i=0;i<50;i++)digest1[i]=NULL;
//	for (int i=0;pair1_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair1_r_oct.val[i]);
//	HASH256_hash(&sh256,digest1);
//	

//	octet pair2_r_oct;				//VKr->oct
//	char pair2_r_ch[400];for(int i=0;i<400;i++)pair2_r_ch[i]=NULL;
//	pair2_r_oct.val = pair2_r_ch;
//	FP12_BN254_toOctet(&pair2_r_oct,&pair2);
//	
//	//hash256 sh256;//hash VKr_oct  
//	HASH256_init(&sh256);
//	char digest2[50];for(int i=0;i<50;i++)digest2[i]=NULL;
//	for (int i=0;pair2_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair2_r_oct.val[i]);
//	HASH256_hash(&sh256,digest2);
//	
//	int flag = 1;
//	for(int i=0;i<50;i++)
//		if(digest1[i]!=digest2[i])
//			flag = 0;
//	
//	if(flag){
//		sprintf((char*)Send_Data_buff,"Test demo 2: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test demo 2: No");
//		LoRa_SendData(Send_Data_buff);
//	}	
//	
///*********************TEST**************************/	
//	//RES = g1*r*ff[1]*lag[0]+..+g1*r*ff[k]*lag[k-1]
//	
//	ECP_BN254 tmp;
//	FP_BN254 ff_FP,ff_idx;
//	BIG_256_28 ff_num;
//	//BIG_256_28 ff_idx;
//	FP_BN254 lag;
//	BIG_256_28 lag_num;
//	
//	for(int i=0;i<Enc_Thres_k;i++){
//		FP_BN254_from_int(&ff_idx,i+1);
//		Enc_ff(&ff_FP,&ff_idx);
//		FP_BN254_redc(ff_num,&ff_FP);
//		Enc_lagrange(&lag,i);
//		if(i==0){
//			ECP_BN254_copy(&RES,&g1);
//			PAIR_BN254_G1mul(&RES,R_num);
//			PAIR_BN254_G1mul(&RES,ff_num);
//			
//			if(sign_FP(&lag)){
//				// negative
//				FP_BN254 lag_tmp;
//				BIG_256_28 lag_num_tmp;
//				FP_BN254_neg(&lag_tmp,&lag);
//				FP_BN254_redc(lag_num_tmp,&lag_tmp);
//				PAIR_BN254_G1mul(&RES,lag_num_tmp);
//				ECP_BN254_neg(&RES);				
//				sprintf((char*)Send_Data_buff,"lag(0) a negative value");
//				LoRa_SendData(Send_Data_buff);
//			}else{
//				FP_BN254_redc(lag_num,&lag);
//				PAIR_BN254_G1mul(&RES,lag_num);
//				sprintf((char*)Send_Data_buff,"lag(0) a positive value");
//				LoRa_SendData(Send_Data_buff);
//			}
//			continue;
//		}
//		
//		ECP_BN254_copy(&tmp,&g1);
//		PAIR_BN254_G1mul(&tmp,R_num);
//		PAIR_BN254_G1mul(&tmp,ff_num);		
//		//PAIR_BN254_G1mul(&tmp,lag_num);
//		if(sign_FP(&lag)){//FP_BN254_sign
//			// negative
//			FP_BN254 lag_tmp;
//			BIG_256_28 lag_num_tmp;
//			FP_BN254_neg(&lag_tmp,&lag);
//			FP_BN254_redc(lag_num_tmp,&lag_tmp);
//			PAIR_BN254_G1mul(&tmp,lag_num_tmp);
//			ECP_BN254_neg(&tmp);
//			sprintf((char*)Send_Data_buff,"lag(1) a negative value");
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			FP_BN254_redc(lag_num,&lag);
//			PAIR_BN254_G1mul(&tmp,lag_num);
//			sprintf((char*)Send_Data_buff,"lag(1) a positive value");
//			LoRa_SendData(Send_Data_buff);
//		}

//		ECP_BN254_add(&RES,&tmp);
//	}
//	//FP_BN254_mul
//	
//	if (ECP_BN254_equals(&RES,&g21)){
//		sprintf((char*)Send_Data_buff,"Test demo RES = g21: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test demo RES = g21: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	
//	//g1*r*(ff[1]*lag[0]+...+ff[k]*lag[k-1])
//	FP_BN254 tot,tot1,tot2;
//	FP_BN254_zero(&tot);
//	for(int i=0;i<Enc_Thres_k;i++){
//		FP_BN254_from_int(&ff_idx,i+1);
//		Enc_ff(&ff_FP,&ff_idx);

//		Enc_lagrange(&lag,i);
//		
//		FP_BN254_mul(&tot1,&ff_FP,&lag);
//		FP_BN254_copy(&tot2,&tot);
//		FP_BN254_add(&tot,&tot1,&tot2);
//	}
//	BIG_256_28 tot_num;
//	FP_BN254_redc(tot_num,&tot);
//	ECP_BN254 res2;
//	ECP_BN254_copy(&res2,&g1);
//	PAIR_BN254_G1mul(&res2,R_num);
//	PAIR_BN254_G1mul(&res2,tot_num);
//	
//	if (ECP_BN254_equals(&res2,&g21)){
//		sprintf((char*)Send_Data_buff,"Test demo res2 = g21: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test demo res2 = g21: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	

//	
//	/**********************test1*************************************/
//	//r*(ff[1]*lag[0]+...+ff[k]*lag[k-1]) == r*ff[1]*lag[0]+...+r*ff[k]*lag[k-1])
//	
//	
//	//r*(ff[1]*lag[0]+...+ff[k]*lag[k-1])
//	FP_BN254 tot1_t1;
//	FP_BN254 tmp1_t1,tmp2_t1;
//	FP_BN254_zero(&tot1_t1);
//	FP_BN254_zero(&tmp2_t1);
//	for(int i=0;i<Enc_Thres_k;i++){
//		FP_BN254_from_int(&ff_idx,i+1);
//		Enc_ff(&ff_FP,&ff_idx);
//		Enc_lagrange(&lag,i);

//		FP_BN254_mul(&tmp1_t1,&ff_FP,&lag);
//		FP_BN254_copy(&tmp2_t1,&tot1_t1);
//		FP_BN254_add(&tot1_t1,&tmp1_t1,&tmp2_t1);
//	}
//	FP_BN254_mul(&tmp1_t1,&tot1_t1,&R);
//	FP_BN254_copy(&tot1_t1,&tmp1_t1);
//	
//	//r*ff[1]*lag[0]+...+r*ff[k]*lag[k-1])
//	
//	FP_BN254 tot2_t1,tmp3_t1;
//	FP_BN254_zero(&tot2_t1);
//	FP_BN254_zero(&tmp2_t1);
//	for(int i=0;i<Enc_Thres_k;i++){
//		FP_BN254_from_int(&ff_idx,i+1);
//		Enc_ff(&ff_FP,&ff_idx);
//		Enc_lagrange(&lag,i);
//		
//		FP_BN254_mul(&tmp1_t1,&ff_FP,&lag);
//		FP_BN254_mul(&tmp2_t1,&R,&tmp1_t1);
//		
//		FP_BN254_copy(&tmp3_t1,&tot2_t1);
//		FP_BN254_add(&tot2_t1,&tmp2_t1,&tmp3_t1);
//	}

//	if (FP_BN254_equals(&tot1_t1,&tot2_t1)){
//		sprintf((char*)Send_Data_buff,"TTTTTTTTTTTest 1: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"TTTTTTTTTTTest 1: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	/**********************test2*************************************/
//	//g1*1+g1*2 == g1*(1+2)
//	
//	ECP_BN254 tot1_t2,t1_t2,t2_t2;
//	ECP_BN254 tot2_t2;
//	
//	FP_BN254 tmp1_t2,tmp2_t2,tmp3_t2;
//	BIG_256_28 num1,num2,num3;
//	FP_BN254_from_int(&tmp1_t2,6);
//	FP_BN254_from_int(&tmp2_t2,1);
//	FP_BN254_from_int(&tmp3_t2,7);
//	
//	FP_BN254_redc(num1,&tmp1_t2);
//	FP_BN254_redc(num2,&tmp2_t2);
//	FP_BN254_redc(num3,&tmp3_t2);
//	
//	ECP_BN254_copy(&t1_t2,&g1);
//	ECP_BN254_copy(&t2_t2,&g1);
//	
//	//ECP_BN254_mul(&t1_t2,num1);
//	//ECP_BN254_mul(&t2_t2,num2);
//	PAIR_BN254_G1mul(&t1_t2,num1);
//	PAIR_BN254_G1mul(&t2_t2,num2);
//	
//	ECP_BN254_add(&t1_t2,&t2_t2);
//	ECP_BN254_copy(&tot1_t2,&t1_t2);
//	
//	ECP_BN254_copy(&tot2_t2,&g1);
//	
//	//ECP_BN254_mul(&tot2_t2,num3);
//	PAIR_BN254_G1mul(&tot2_t2,num3);
//	
//	if (ECP_BN254_equals(&tot1_t2,&tot2_t2)){
//		sprintf((char*)Send_Data_buff,"g1*1+g1*2 == g1*(1+2): Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"g1*1+g1*2 == g1*(1+2): No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	FP_BN254_from_int(&tmp1_t2,-6);
//	FP_BN254_from_int(&tmp2_t2,3);
//	
//	FP_BN254_add(&tmp3_t2,&tmp1_t2,&tmp2_t2);
//	
//	if (!sign_FP(&tmp3_t2)){
//		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is +");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is -");
//		LoRa_SendData(Send_Data_buff);
//	}

//	/*************	Test3	********************/
//	//g1*(-6)+g1*(7) == g1
//	FP_BN254 tmp1_t3,tmp2_t3,tmp3_t3;
//	ECP_BN254 g1_t3,g2_t3,g3_t3;
//	ECP_BN254_copy(&g1_t3,&g1);
//	ECP_BN254_copy(&g2_t3,&g1);
//	ECP_BN254_copy(&g3_t3,&g1);
//	
//	FP_BN254_from_int(&tmp1_t3,-10);
//	FP_BN254_from_int(&tmp2_t3,12);
//	BIG_256_28 tmp1_num,tmp2_num;
//	
//	FP_BN254_add(&tmp3_t2,&tmp1_t2,&tmp2_t2);
//	
//	if(sign_FP(&tmp1_t3)){
//		FP_BN254_neg(&tmp3_t3,&tmp1_t3);
//		FP_BN254_redc(tmp1_num,&tmp3_t3);
//	}
//	FP_BN254_redc(tmp2_num,&tmp2_t3);
//	
//	PAIR_BN254_G1mul(&g1_t3,tmp1_num);
//	ECP_BN254_neg(&g1_t3);
//	PAIR_BN254_G1mul(&g2_t3,tmp2_num);
//	ECP_BN254_add(&g1_t3,&g2_t3);
//	
//	FP_BN254_from_int(&tmp1_t3,2);
//	FP_BN254_redc(tmp1_num,&tmp1_t3);
//	PAIR_BN254_G1mul(&g3_t3,tmp1_num);
//	
//	if (ECP_BN254_equals(&g1_t3,&g3_t3)){
//		sprintf((char*)Send_Data_buff,"g1*(-6)+g1*(7) == g1: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"g1*(-6)+g1*(7) == g1: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	/*************	Test4	********************/
//	FP_BN254 TMP;
//	FP_BN254_from_int(&TMP,9);
//	FP_BN254_from_int(&lag,1);
//	FP_BN254_redc(lag_num,&lag);
//	Enc_lagrange(&lag,1);
//	FP_BN254_redc(lag_num,&lag);
//	if(sign_FP(&lag)){
//		sprintf((char*)Send_Data_buff,"Sign of lag is -");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Sign of lag is +");
//		LoRa_SendData(Send_Data_buff);
//	}
//	if(sign_FP(&TMP)){
//		sprintf((char*)Send_Data_buff,"Sign of TMP is -");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Sign of TMP is +");
//		LoRa_SendData(Send_Data_buff);
//	}
//	if(FP_BN254_equals(&lag,&TMP)){
//		sprintf((char*)Send_Data_buff,"lag == -1: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"lag == -1: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	
////	if (!sign_FP(&tmp3_t2)){
////		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is +");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"sign of tmp1_t2 is -");
////		LoRa_SendData(Send_Data_buff);
////	}
//	
///////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

////test5

////g1*(r*ff[1]*lag[0] + r*ff[2]*lag[1]) == g1*r*ff[1]*lag[0] + g1*r*ff[2]*lag[1]
//	// printf("\n\n\n122211\n\n\n");

//	//FP_BN254 R;
//	FP_BN254 ff1,ff2,lag0,lag1;
//	ECP_BN254 e1,e2,e3,e4;
//	ECP_BN254 _e1,_e2,_e3,_e4;
//	FP_BN254 m1,m2,m3,m4;
//	FP_BN254 _m1,_m2,_m3,_m4;
//	FP_BN254 t1,t2,t3,t4;
//	BIG_256_28 n1,n2,n3,n4;
//	FP_BN254_from_int(&ff_idx,1);
//	Enc_ff(&ff1,&ff_idx);

//	FP_BN254_from_int(&ff_idx,2);
//	Enc_ff(&ff2,&ff_idx);

//	Enc_lagrange(&lag0,0);
//	Enc_lagrange(&lag1,1);

//	//t2 = r*ff[1]*lag[0]
//	FP_BN254_mul(&t1,&ff1,&lag0);
//	FP_BN254_mul(&t2,&t1,&R);
//	//t3 = r*ff[2]*lag[1]
//	FP_BN254_mul(&t1,&ff2,&lag1);
//	FP_BN254_mul(&t3,&t1,&R);
//	//m1 = r*ff[1]*lag[0] + r*ff[2]*lag[1]
//	FP_BN254_add(&m1,&t2,&t3);
//	int flag1 = 0;
//	int flag2 = 0;
//	int flag3 = 0;
//	//e1 = g1*m1
//	if(!sign_FP(&m1)){
//		FP_BN254_redc(n1,&m1);
//		ECP_BN254_copy(&e1,&g1);
//		PAIR_BN254_G1mul(&e1,n1);
//	}else{
//		FP_BN254_neg(&_m1,&m1);	
//		FP_BN254_redc(n1,&_m1);
//		ECP_BN254_copy(&e1,&g1);
//		PAIR_BN254_G1mul(&e1,n1);
//		ECP_BN254_neg(&e1);
//	}
//	//m2 = r*ff[1]*lag[0]
//	FP_BN254_mul(&t1,&ff1,&lag0);
//	FP_BN254_mul(&m2,&t1,&R);

//	//e2 = g1*m2
//	if(!sign_FP(&m2)){
//		FP_BN254_redc(n2,&m2);
//		ECP_BN254_copy(&e2,&g1);
//		PAIR_BN254_G1mul(&e2,n2);
//	}else{
//		FP_BN254_neg(&_m2,&m2);	
//		FP_BN254_redc(n2,&_m2);
//		ECP_BN254_copy(&e2,&g1);
//		PAIR_BN254_G1mul(&e2,n2);
//		ECP_BN254_neg(&e2);
//	}
//	
//	//m3 = r*ff[2]*lag[1]
//	FP_BN254_mul(&t1,&ff2,&lag1);
//	FP_BN254_mul(&m3,&t1,&R);

//	//e3 = g1*m3
//	if(!sign_FP(&m3)){
//		FP_BN254_redc(n3,&m3);
//		ECP_BN254_copy(&e3,&g1);
//		PAIR_BN254_G1mul(&e3,n3);	
//	}else{
//		FP_BN254_neg(&_m3,&m3);	
//		FP_BN254_redc(n3,&_m3);
//		ECP_BN254_copy(&e3,&g1);
//		PAIR_BN254_G1mul(&e3,n3);
//		ECP_BN254_neg(&e3);
//	}
//	
//	//e2 = e2 + e3
//	ECP_BN254_add(&e2,&e3);


//	if (ECP_BN254_equals(&e1,&e2)){
//		sprintf((char*)Send_Data_buff,"test 5: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"test 5: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	if (ECP_BN254_equals(&RES,&e1)){
//		sprintf((char*)Send_Data_buff,"Test demo RES = e1: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test demo RES = e1: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	if (ECP_BN254_equals(&RES,&e2)){
//		sprintf((char*)Send_Data_buff,"Test demo RES = e2: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test demo RES = e2: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	if (ECP_BN254_equals(&g21,&e1)){
//		sprintf((char*)Send_Data_buff,"Test demo g21 = e1: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test demo g21 = e1: No");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	if (ECP_BN254_equals(&g21,&e2)){
//		sprintf((char*)Send_Data_buff,"Test demo g21 = e2: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test demo g21 = e2: No");
//		LoRa_SendData(Send_Data_buff);
//	}
////	for(int i=-10000;i<=10000;i++){
////		FP_BN254_from_int(&ff_idx,i);
////		if(sign_FP(&ff_idx)){
////			sprintf((char*)Send_Data_buff,"----:%d",i);
////			LoRa_SendData(Send_Data_buff);
////		}else{
////			sprintf((char*)Send_Data_buff,"++++:%d",i);
////			LoRa_SendData(Send_Data_buff);
////		}
////	}

}

void Enc_ff(BIG_256_28 r,BIG_256_28 x){
	BIG_256_28 q;
	BIG_256_28_rcopy(q, CURVE_Order_BN254);
	BIG_256_28 y;
	BIG_256_28 xx;
	BIG_256_28_zero(y);
	BIG_256_28_one(xx);
	BIG_256_28 tmp;
	BIG_256_28 tmp1;
	for(u16 i=0;i<Enc_Thres_k;i++){
		
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
void Enc_lagrange(BIG_256_28 r,u8 j){
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
	for(u8 jj=0;jj<Enc_Thres_k;jj++){
		if(Enc_S[jj] == j) continue;
		//num = num * (0 - S[jj] - 1);
		BIG_256_28_zero(S_jj);
		BIG_256_28_inc(S_jj,Enc_S[jj]);
		
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
	for(u8 jj=0;jj<Enc_Thres_k;jj++){
		if(Enc_S[jj] == j) continue;
		//den = den * (j - S[jj]);
		BIG_256_28_zero(S_jj);
		BIG_256_28_inc(S_jj,Enc_S[jj]);
		
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



void Enc_dealer(){
	
	TIM9_Exceed_Times = 0;
	TIM9->CNT = 0;
	
	//sha256 init
	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
	char digest[50];
	hash256 sh256;
	
	char mc[10] = "geng1";
	
//	BIG_256_28 g1_num;
//	
//	for(int i=0;i<50;i++)digest[i] = NULL;
//	HASH256_init(&sh256);
//	for (int i=0;mc[i]!=0;i++) HASH256_process(&sh256,mc[i]);
//    HASH256_hash(&sh256,digest);
//	BIG_256_28_fromBytes(g1_num,digest);
////	
//	if(!ECP_BN254_generator(&G1)){
//		sprintf((char*)Send_Data_buff,"G1 generator Fail");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	ECP_BN254_copy(&g1,&G1);
//	PAIR_BN254_G1mul(&g1,g1_num);
//	//ECP_BN254_copy(&g2,&g1);
//	
//	if (!ECP2_BN254_generator(&G2)){
//		sprintf((char*)Send_Data_buff,"G2 generator Fail");
//		LoRa_SendData(Send_Data_buff);
//	}
//	ECP2_BN254_copy(&g2,&G2);
//	PAIR_BN254_G2mul(&g2,g1_num);
	
//	char raw[100];
//    octet RAW = {0, sizeof(raw), raw};
//    csprng RNGx;                // Crypto Strong RNG
//	u32 ran = RNG_Get_RandomNum();	
//    RAW.len = 100;              // fake random seed source
//    for (u16 i = 0; i < 100; i++) RAW.val[i] = RNG_Get_RandomNum();
//    CREATE_CSPRNG(&RNGx, &RAW);  // initialise strong RNG
//	
//	BIG_256_28 secret;
//	BIG_256_28 modl;
//	BIG_256_28_rcopy(modl,CURVE_Order_BN254);
//	BIG_256_28_randomnum(secret,modl,&RNGx);
//	
//	BIG_256_28_copy(a[0],secret);
//	BIG_256_28_copy(SECRET,secret);

//	for(u16 i=1;i<Enc_Thres_k;i++){
//		BIG_256_28_randomnum(a[i],modl,&RNGx);
//	}
////	
//	for(u16 i=0;i<players;i++){
//		BIG_256_28 x;
//		BIG_256_28_zero(x);
//		BIG_256_28_inc(x,i+1);
//		Enc_ff(Enc_SKs[i],x);
//	}
//	
//	//assert ff(0) == secret
//	BIG_256_28 secret1;
//	BIG_256_28 ZERO;
//	BIG_256_28_zero(ZERO);
//	ff(secret1,ZERO);
//	
//	//VK = g2 ** secret
//	ECP2_BN254_copy(&Enc_VK,&Enc_g2);
//	PAIR_BN254_G2mul(&Enc_VK,secret);
//	
//	//VKs = [g2 ** xx for xx in SKs ]
//	for(u16 i=0 ; i<players ; i++){
//		ECP2_BN254_copy(&Enc_VKs[i],&Enc_g2);
//		PAIR_BN254_G2mul(&Enc_VKs[i],Enc_SKs[i]);	
//	}	
	
//	/**************************Dealer end**********************************/

////	# Check reconstruction of 0
////  S = set(range(0,k))[0,1,2]
////  lhs = f(0)
//	BIG_256_28 lhs,rhs,tmp,tmp1,tmp2,tmp3,tmp4;
////	FP_BN254 ZERO;
//	BIG_256_28_zero(ZERO);
//	BIG_256_28_zero(rhs);
//	ff(lhs,ZERO);
//	
////  rhs = sum(public_key.lagrange(S,j) * f(j+1) for j in S)
//	for(u8 i=0;i<20;i++)S[i] = i;
//	
//	for(u16 i=0;i<Enc_Thres_k;i++){
//		BIG_256_28_zero(tmp2);
//		BIG_256_28_inc(tmp2,Enc_S[i]+1);
//		ff(tmp,tmp2);//tmp = ff[S[i]+1]
//		Enc_lagrange(tmp1,Enc_S[i]);//tmp1 = lagrange
//		BIG_256_28_modmul(tmp3,tmp1,tmp,modl);//tmp3 = tmp1 * tmp
//		BIG_256_28_modadd(tmp4,tmp3,rhs,modl);//tmp4 = tmp3 + rhs
//		BIG_256_28_copy(rhs,tmp4);//rhs = tmp4
//	}
//	
//	if(BIG_256_28_comp(lhs,rhs)==0){
//		sprintf((char*)Send_Data_buff,"Reconstruction: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Reconstruction: NO");
//		LoRa_SendData(Send_Data_buff);
//	}
	
	
	for(int i=0;i<50;i++)digest[i] = NULL;
	HASH256_init(&sh256);
	for (int i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
    HASH256_hash(&sh256,digest);
	
//	for(int i=0;i<50;i++)M[i]=digest[i];
	
//	PK_encrypt(digest);
//	SK_decrypt_share();
//	PK_verify_share();
	
	
}





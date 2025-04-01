#include "Thres_Enc.h"
#include "HoneyBadgerBFT.h"

char thres_enc_private_key_string[Nodes_N][key_num1] ={
"22 35 2b 45 56 57 de 22 bc a9 7b bf 40 d3 41 6b bc 1d f6 19 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"1f 8c e1 e0 9a f9 1d 53 52 37 2e 7a 36 64 f8 6e 19 12 ad 2a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"47 b8 b1 48 6d d9 a2 b3 1b df 8d 7f 20 37 63 91 97 51 da 6a 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"2e b5 39 06 a7 53 a2 46 ab 54 3c df bf c9 a5 74 59 cf 1d b2 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" 
};
char thres_enc_public_key_string[Nodes_N+1][122] = {
"03 10 90 28 cc 87 37 0f 0d 78 14 4f 31 e2 84 16 f0 92 35 56 64 0d 64 ac 69 63 67 30 e8 a8 2d fb 30 bb 7a 5b d2 78 a0 ed dd", 
"02 08 6e 36 0c 56 7b 2f a1 38 41 ab 3a 3a 76 df c1 56 ba 8f 16 18 70 4f a7 3c f2 75 a7 37 47 e9 e9 da b1 c7 5f 39 eb 26 61", 
"02 07 16 f6 e5 7d 78 6c 12 78 c6 46 9f 28 9e 1c 37 e2 0d b1 22 12 08 04 9b eb 35 cb 78 7a 06 01 0d 6d c1 65 5f 49 20 e9 41", 
"02 13 50 80 21 d3 82 da 36 1f bd d6 30 a0 f3 b5 3e f6 e3 2b 58 17 49 63 ea 73 f5 8d 51 13 b2 d2 3a 04 6b 2e 7a 2c c6 f3 c7", 
"03 00 89 c9 a9 fb ec 86 fd 71 a5 df 0d bb 08 28 71 97 c2 29 19 13 41 5b c5 d0 15 cc 27 7e 05 86 b5 6d 19 8e 16 fd ee e0 fd"
};

char Global_thres_enc_private_key_string[Nodes_N][key_num1] ={
"3a 5c 0f 8d 26 5a be e9 fc b8 be 6f 9d 1c 5f 68 d1 f4 b3 4c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"49 5c 41 d3 88 49 5b e0 e5 1b 2f c0 c7 ee a5 44 a8 62 81 8c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00",
"45 62 ca 45 d6 be 8c cc ef 0c 96 f4 9f 75 7b 60 9e fd 8e 5c 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00", 
"52 70 c9 0b 74 46 eb 01 e9 fc 68 05 38 86 80 dd 54 c9 f9 c9 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00" 
};
char Global_thres_enc_public_key_string[Nodes_N+1][122] = {
"03 08 00 2b 0d 40 86 94 43 d8 ee d8 2f 20 d9 6c f1 37 bc 32 f8 0f 87 96 ec 78 79 c2 70 f4 ce 4b b8 43 4d 7c 95 74 ef a6 a1", 
"03 1c 66 11 c3 bc e9 19 87 23 46 66 d4 02 12 ce 69 ed ff 61 c1 20 5c cf 0e 3b c1 d5 24 71 cf 8d 93 fa 71 fa cd 71 52 41 c3", 
"03 0c 42 a0 62 68 c2 70 17 93 59 69 71 75 73 3d db f2 ce a1 97 20 ef 27 e1 24 b8 e7 4f 5c 10 83 71 d2 cc 71 b5 6b 26 74 65", 
"02 02 92 aa 93 44 44 ff a4 16 28 b0 33 06 06 8a 16 f9 a8 ba ac 0f 4f 80 f1 de 24 41 b7 bb 8b e3 fc bd 2d a7 86 d1 e6 73 84", 
"02 06 15 d9 95 f1 80 c1 79 ae e4 7a 20 32 38 e1 19 e2 c7 ec 77 09 08 a8 7d bf 64 76 bd e4 a3 a4 93 90 a5 f8 8b 42 d9 f2 bc"
};


//char thres_private_key_string[Nodes_N][key_num1] ={
//"25 23 64 82 40 00 00 01 BA 34 4D 80 00 00 00 08 61 21 00 00 00 00 00 13 A6 FF FF FF CC 5C 43 C7 00 00 00 00 00 00 00 00",
//"25 23 64 82 40 00 00 01 BA 34 4D 80 00 00 00 08 61 21 00 00 00 00 00 13 A6 FF FF FF CC 5C 44 1F 00 00 00 00 00 00 00 00",
//"25 23 64 82 40 00 00 01 BA 34 4D 80 00 00 00 08 61 21 00 00 00 00 00 13 A6 FF FF FF CC 5C 44 77 00 00 00 00 00 00 00 00",
//"25 23 64 82 40 00 00 01 BA 34 4D 80 00 00 00 08 61 21 00 00 00 00 00 13 A6 FF FF FF CC 5C 44 CF 00 00 00 00 00 00 00 00"
//};
//char thres_public_key_string[Nodes_N+1][key_num2] = {
//"03 1B 8F A4 79 28 B7 A6 32 D1 7B 23 1C D1 67 14 9A D2 BB 35 73 08 C0 C0 37 D7 F2 6F 61 EA 35 35 5D 0C 0F D5 EF 68 CB B4 9B C6 8C 53 2B C5 98 33 9C C1 A5 60 CB E8 2A 83 8A AA 84 CE 95 76 CF 20 1C",
//"03 1D 74 61 95 0C 08 BB E0 3F 8E 23 5A CE 76 CC 53 D7 8B 0B 1E 54 8E 7A 08 60 79 50 3A F8 49 74 A0 21 C0 B2 85 28 7A 4C DB 61 DA E5 97 A8 F9 DB 27 D5 CF FF 0C 5B 27 67 41 19 13 57 90 11 E2 45 93",
//"03 07 D4 93 2A 0B 7E CD 42 65 AD CD 59 0E D5 E2 D7 EE 55 72 29 69 1F 2F 32 B0 19 25 5E 5B 4B DF 95 1F E3 07 44 F2 B6 B3 C1 88 B5 8B 80 D4 A5 07 6B D2 BC 56 96 5E 76 F7 40 A7 85 9B 1E F5 1C 2A EB",
//"02 15 E4 44 0E 95 F0 2C F5 DE 00 D8 46 E4 86 50 CE 34 13 46 E7 EF 50 67 01 E7 E6 93 91 1F 0F 48 C9 23 AC B6 EB 1E CC 64 2F 3C 19 3F F0 5D 24 BF 82 66 F9 CC 6E 93 C4 FA AB 03 96 F8 3D DE AE 74 88",
//"03 04 3B 31 EB 1D 60 5E E5 17 19 95 09 8E 8D 92 9F 4A E4 FF 8A 4F 18 85 21 D1 D2 54 96 78 41 84 65 00 FD F9 A1 10 99 A6 CF F3 48 1D AE A6 92 2B 25 BB 0D 4A E9 31 D2 EF 72 01 19 2B 20 50 2E 1B 23"
//};

//char M[50];
////char M_[50];

u8 Enc_players = Nodes_N;
u8 Enc_Thres_k = 2 * Nodes_f + 1;

//ECP_BN158 Enc_G1;
//ECP2_BN158 Enc_G2;
//ECP_BN158 Enc_g1;
//ECP2_BN158 g2;

//FP_BN158 a[MAX_K];
//u8 S[MAX_SK];
//FP_BN158 SECRET;
BIG_160_28 Enc_SKs[MAX_SK];

//ECP2_BN158 g2;
ECP2_BN158 Enc_VK;
ECP2_BN158 Enc_VKs[MAX_SK];

//ECP_BN158 M_point;
//ECP_BN158 Sigs[MAX_SK];
//ECP_BN158 SIG;

/*************************Threshold Encryption****************************************/
//FP_BN158 a[MAX_K];
//FP_BN158 SKs[MAX_SK];
u8 Enc_players;
//u8 k;
ECP_BN158 Enc_G1;
ECP2_BN158 Enc_G2;
ECP_BN158 Enc_g1;
ECP2_BN158 Enc_g2;
//ECP2_BN158 VK;
//ECP2_BN158 VKs[MAX_SK];
u8 Enc_S[MAX_SK];
BIG_160_28 R_num;
FP_BN158 R;
//FP_BN158 SECRET;

ECP_BN158 RES;

ECP2_BN158 g12;
FP12_BN158 pair1;
ECP_BN158 g21;
FP12_BN158 pair2;
ECP_BN158 U;
ECP_BN158 shares[MAX_SK];
char V[50];
ECP2_BN158 W,H;

char Enc_M[50];
char Enc_M_[50];


void PK_encrypt(char* m);
void PK_verify_ciphertext();
void SK_decrypt_share();
void PK_verify_share();
void PK_combine_share();
void testdemo();
void Enc_ff(BIG_160_28 r,BIG_160_28 x);
void Enc_lagrange(BIG_160_28 r,u8 j);
void Enc_dealer();
BIG_160_28 Enc_g1_num;

/*****************************Single-Hop******************************************************************/

void Tx_PK_encrypt(char* m, char *enc);
void Tx_PK_verify_ciphertext();
void Tx_SK_decrypt_share(u8 tx_id, u8 id);
int Tx_PK_verify_share(u8 tx_id, u8 id);
void Tx_PK_combine_share(u8 tx_id, char * enc, char *dec);

void init_thres_enc(){
	FP_BN158 ONE;
	FP_BN158 ZERO;
	FP_BN158_zero(&ZERO);
	FP_BN158_one(&ONE);
	
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
	BIG_160_28_fromBytes(Enc_g1_num,digest);
	
	if(!ECP_BN158_generator(&Enc_G1)){
		sprintf((char*)Send_Data_buff,"Enc_G1 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	
	ECP_BN158_copy(&Enc_g1,&Enc_G1);
	PAIR_BN158_G1mul(&Enc_g1,Enc_g1_num);
	//ECP_BN158_copy(&g2,&Enc_g1);
	
	if (!ECP2_BN158_generator(&Enc_G2)){
		sprintf((char*)Send_Data_buff,"Enc_G2 generator Fail");
		LoRa_SendData(Send_Data_buff);
	}
	ECP2_BN158_copy(&Enc_g2,&Enc_G2);
	PAIR_BN158_G2mul(&Enc_g2,Enc_g1_num);
	
	
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
		BIG_160_28_fromBytes(Enc_SKs[i],sch[i]);
	}
	for(int i=1;i<=Enc_players;i++){
		ECP2_BN158_fromOctet(&Enc_VKs[i],&RAW[i]);
	}
	ECP2_BN158_fromOctet(&Enc_VK,&RAW[Nodes_N+1]);
	//dealer();
	//while(1);
	//Enc_dealer();
	
	for(u8 i=0;i<20;i++)Enc_S[i] = i;
	
//	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
//	//char digest[50];
//	//hash256 sh256;
//	
//	for(int i=0;i<50;i++)digest[i] = NULL;
//	HASH256_init(&sh256);
//	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
//    HASH256_hash(&sh256,digest);
//	
//	for(int i=0;i<50;i++)Enc_M[i]=digest[i];
//	
//	FP_BN158 M_FP;
//	BIG_160_28 M_num;
//	FP_BN158_fromBytes(&M_FP,M);
//	
//	if(sign_FP(&M_FP)){
//		// negative num
//		FP_BN158 M_FP_neg;
//		FP_BN158_neg(&M_FP_neg,&M_FP);
//		FP_BN158_redc(M_num,&M_FP);
//		ECP_BN158_copy(&M_point,&Enc_G1);
//		PAIR_BN158_G1mul(&M_point,M_num);
//		ECP_BN158_neg(&M_point);
//	}else{
//		FP_BN158_redc(M_num,&M_FP);
//		ECP_BN158_copy(&M_point,&Enc_G1);
//		PAIR_BN158_G1mul(&M_point,M_num);
//	}
	
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
	
	BIG_160_28 modl;
	BIG_160_28_rcopy(modl, CURVE_Order_BN158);
	
	BIG_160_28 r;
	BIG_160_28_randomnum(r,modl,&RNGx);

	
	//	U = g1 ** r
	
	ECP_BN158_copy(&Tx_U[ID],&Enc_g1);
	PAIR_BN158_G1mul(&Tx_U[ID],r);
	
	octet U_oct;				//Res->oct
	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
	U_oct.val = U_ch;
	ECP_BN158_toOctet(&U_oct,&Tx_U[ID],true);
	for(int i=0;i<Share_Size;i++)Tx_U_str[ID][i] = U_ch[i];
	
	BIG_160_28_copy(R_num,r);
	
///////V = xor(m, hashG(self.VK ** r))
	//改成下面的
	//#V = xor(m, hashG(pair(g1, self.VK ** r)))
	
	ECP_BN158 g1_neg;
	ECP_BN158_copy(&g1_neg,&Enc_g1);
	ECP_BN158_neg(&g1_neg);
	
	ECP2_BN158 VKr;				//VK ** r
	ECP2_BN158_copy(&VKr,&Enc_VK);
	PAIR_BN158_G2mul(&VKr,r);
	
	FP12_BN158 pair_r;//pair(g1, self.VK ** r)
	PAIR_BN158_ate(&pair_r,&VKr,&Enc_g1);
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
	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
	pair_r_oct.val = pair_r_ch;
	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
	
	
	hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
	HASH256_hash(&sh256,digest);
	
	for(int i=0;i<32;i++){	//xor m and VKr->oct
		enc[i] = m[i]^digest[i];
	}
	enc[32] = NULL;
	
	
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
//	FP_BN158 r;
//	FP_BN158_from_int(&r,ran);

//	
//	FP_BN158_copy(&R,&r);
//	
//	
//	//	U = g1 ** r
//	
//	BIG_160_28 r_num;
//	ECP_BN158_copy(&Tx_U[ID],&Enc_g1);
//	FP_BN158_redc(r_num,&r);
//	PAIR_BN158_G1mul(&Tx_U[ID],r_num);
//	
//	octet U_oct;				//Res->oct
//	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
//	U_oct.val = U_ch;
//	ECP_BN158_toOctet(&U_oct,&Tx_U[ID],true);
//	for(int i=0;i<Share_Size;i++)Tx_U_str[ID][i] = U_ch[i];
//	
//	BIG_160_28_copy(R_num,r_num);
//	
/////////V = xor(m, hashG(self.VK ** r))
//	//改成下面的
//	//#V = xor(m, hashG(pair(g1, self.VK ** r)))
//	
//	ECP_BN158 g1_neg;
//	ECP_BN158_copy(&g1_neg,&Enc_g1);
//	ECP_BN158_neg(&g1_neg);
//	
//	ECP2_BN158 VKr;				//VK ** r
//	ECP2_BN158_copy(&VKr,&Enc_VK);
//	PAIR_BN158_G2mul(&VKr,r_num);
//	
//	FP12_BN158 pair_r;//pair(g1, self.VK ** r)
//	PAIR_BN158_ate(&pair_r,&VKr,&Enc_g1);
//	PAIR_BN158_fexp(&pair_r);
//	//testdemo();
//	
//	octet pair_r_oct;				//VKr->oct
//	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
//	pair_r_oct.val = pair_r_ch;
//	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
//	
//	
//	hash256 sh256;//hash VKr_oct  
//	HASH256_init(&sh256);
//	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
//	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
//	HASH256_hash(&sh256,digest);
//	
//	for(int i=0;i<PD_Size;i++){	//xor m and VKr->oct
//		enc[i] = m[i]^digest[i];
//	}
	
}











void Tx_PK_verify_ciphertext(){

}

void Tx_SK_decrypt_share(u8 tx_id, u8 id){
		// id -> tx_id

	ECP_BN158_copy(&Tx_shares[tx_id][id-1],&Tx_U[tx_id]);
	PAIR_BN158_G1mul(&Tx_shares[tx_id][id-1],Enc_SKs[id]);
	
	octet S_oct;				//Res->oct
	char S_ch[100];for(int i=0;i<100;i++)S_ch[i]=NULL;
	S_oct.val = S_ch;
	ECP_BN158_toOctet(&S_oct,&Tx_shares[tx_id][id-1],true);
	for(int i=0;i<Share_Size;i++)Tx_shares_str[tx_id][id-1][i] = S_ch[i];
	
	
//	ECP2_BN158 g2_neg;
//	ECP2_BN158_copy(&g2_neg,&Enc_g2);
//	ECP2_BN158_neg(&g2_neg);
//	
//	//pair(U_i, g2) == pair(U, Y_i)
//	FP12_BN158 v;
//	//PAIR_BN158_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//	PAIR_BN158_double_ate(&v,&g2_neg,&Tx_shares[tx_id][id-1],&Enc_VKs[id],&Tx_U[tx_id]);
//	PAIR_BN158_fexp(&v);
	
//	if (FP12_BN158_isunity(&v)){
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

//	// id -> tx_id
//	BIG_160_28 sk_num;
//	ECP_BN158_copy(&Tx_shares[tx_id][id-1],&Tx_U[tx_id]);
//	FP_BN158_redc(sk_num,&Enc_SKs[id]);
//	PAIR_BN158_G1mul(&Tx_shares[tx_id][id-1],sk_num);
//	
//	octet S_oct;				//Res->oct
//	char S_ch[100];for(int i=0;i<100;i++)S_ch[i]=NULL;
//	S_oct.val = S_ch;
//	ECP_BN158_toOctet(&S_oct,&Tx_shares[tx_id][id-1],true);
//	for(int i=0;i<Share_Size;i++)Tx_shares_str[tx_id][id-1][i] = S_ch[i];
//	
////	BIG_160_28 sk_num;
////	for(int i=1;i<=Enc_players;i++){
////		ECP_BN158_copy(&Tx_shares[ID][i-1],&Tx_U[ID]);
////		FP_BN158_redc(sk_num,&Enc_SKs[i]);
////		PAIR_BN158_G1mul(&Tx_shares[ID][i-1],sk_num);
//	}
	
}

int Tx_PK_verify_share(u8 tx_id, u8 id){
	//id -> tx_id
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&Enc_g2);
	ECP2_BN158_neg(&g2_neg);
	
	//pair(U_i, g2) == pair(U, Y_i)
	FP12_BN158 v;
	//PAIR_BN158_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
	PAIR_BN158_double_ate(&v,&g2_neg,&Tx_shares[tx_id][id-1],&Enc_VKs[id],&Tx_U[tx_id]);
	PAIR_BN158_fexp(&v);
	
	if (FP12_BN158_isunity(&v)){
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
	
	
	
//	ECP2_BN158 g2_neg;
//	ECP2_BN158_copy(&g2_neg,&Enc_g2);
//	ECP2_BN158_neg(&g2_neg);
//	
//	//pair(U_i, g2) == pair(U, Y_i)
//	FP12_BN158 v;
//	//PAIR_BN158_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//	PAIR_BN158_double_ate(&v,&g2_neg,&Tx_shares[tx_id][id-1],&Enc_VKs[id],&Tx_U[tx_id]);
//	PAIR_BN158_fexp(&v);
//	
//	if (FP12_BN158_isunity(&v)){
////		sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: Yes",id);
////		LoRa_SendData(Send_Data_buff);
//		return 1;
//	}else{
//		while(1){	
//			sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: No %d",tx_id, id);
//			LoRa_SendData(Send_Data_buff);
//			delay_ms(1000);
//		}
//		return 0;
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
	
	BIG_160_28 larg;
	ECP_BN158 tmp;
	ECP_BN158 r[20];
	ECP_BN158 res;
	
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_Share[tx_id][k] == 0)
			Tx_SK_decrypt_share(tx_id, k);
	}
	
	for(int j=0 ; j<Enc_Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = Enc_S[j];
		Enc_lagrange(larg,jj);
		ECP_BN158_copy(&tmp,&Tx_shares[tx_id][jj]);
		PAIR_BN158_G1mul(&tmp,larg);
		ECP_BN158_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Enc_Thres_k ; j++){
		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
	ECP_BN158_copy(&res,&r[0]);	
	
	
	FP12_BN158 pair_r;//pair(g2, res)
	PAIR_BN158_ate(&pair_r,&Enc_g2,&res);
	PAIR_BN158_fexp(&pair_r);
		
	octet pair_r_oct;				//VKr->oct
	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
	pair_r_oct.val = pair_r_ch;
	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
	
	hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
	HASH256_hash(&sh256,digest);
	
	char m_[100];for(int i=0;i<100;i++)m_[i]=NULL;
	
	for(int i=0;i<32;i++){
		dec[i] = digest[i]^enc[i];
	}	
	
	
	
	
	
	
	
	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	FP_BN158 larg;
//	BIG_160_28 larg_num;
//	ECP_BN158 tmp;
//	ECP_BN158 r[20];
//	ECP_BN158 res;
//	
//	for(int k=1;k<=Nodes_N;k++){
//		if(ACK_Share[tx_id][k] == 0)
//			Tx_SK_decrypt_share(tx_id, k);
//	}

////	u8 idx_ = 0;
////	for(int k=1;k<=Nodes_N;k++){
////		if(ACK_Share[tx_id][k]){
////			Enc_S[idx_] = k-1;
////			idx_++;
////		}
////	}
//	
//	for(int j=0 ; j<Enc_Thres_k ; j++){
////		share ** self.lagrange(S, j)
//		u8 jj = Enc_S[j];
//		Enc_lagrange(&larg,jj);
//		FP_BN158_redc(larg_num,&larg);
//		ECP_BN158_copy(&tmp,&Tx_shares[tx_id][jj]);
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
//	for(int j=1 ; j<Enc_Thres_k ; j++){
//		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
//	}
//	ECP_BN158_copy(&res,&r[0]);	
//	
//	FP12_BN158 pair_r;//pair(g2, res)
//	PAIR_BN158_ate(&pair_r,&Enc_g2,&res);
//	PAIR_BN158_fexp(&pair_r);
//	
//	octet pair_r_oct;				//VKr->oct
//	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
//	pair_r_oct.val = pair_r_ch;
//	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
//	
//	hash256 sh256;//hash VKr_oct  
//	HASH256_init(&sh256);
//	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
//	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
//	HASH256_hash(&sh256,digest);
//	
//	char m_[100];for(int i=0;i<100;i++)m_[i]=NULL;
//	
//	for(int i=0;i<PD_Size;i++){
//		dec[i] = digest[i]^enc[i];
//	}	
//	
//	//while(1);
//	
////	for(int i=0;i<50;i++)	Enc_M_[i] = m_[i];
////	
////	int ress = 1;
////	for(int i=0;i<32;i++){
////		if(Enc_M[i]!=Enc_M_[i])ress=0;
////	}
//	
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_combine_share_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"Combine shares: %d",time);
////	LoRa_SendData(Send_Data_buff);
//	
////	/**************************Combine shares**********************************/
////	
////	if(ress){
////		sprintf((char*)Send_Data_buff,"Combine shares: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"Combine shares: No");
////		LoRa_SendData(Send_Data_buff);
////	}
////		SS = range(PK.l)
////		for i in range(1):
////      random.shuffle(SS)
////      S = set(SS[:PK.k])
////      
////      m_ = PK.combine_shares(C, dict((s,shares[s]) for s in S))
////      assert m_ == m
}



/*******************************************************************/


/*****************************Multi-Hop******************************************************************/

BIG_160_28 Global_Enc_SKs[MAX_SK];
ECP2_BN158 Global_Enc_VK;
ECP2_BN158 Global_Enc_VKs[MAX_SK];


void Global_Tx_PK_encrypt(char* m, char *enc);
void Global_Tx_PK_verify_ciphertext();
void Global_Tx_SK_decrypt_share(u8 tx_id, u8 id);
int Global_Tx_PK_verify_share(u8 tx_id, u8 id);
void Global_Tx_PK_combine_share(u8 tx_id, char * enc, char *dec);

void Global_init_thres_enc(){
	FP_BN158 ONE;
	FP_BN158 ZERO;
	FP_BN158_zero(&ZERO);
	FP_BN158_one(&ONE);
	
	//sha256 init
	char digest[50];
	hash256 sh256;
	int i;
//	u32 lenn = NLEN_256_28;
//	int32_t gg[lenn];
	
	
	char mc[] = "geng1";

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
		for(string_idx = 0; string_idx < sizeof(Global_thres_enc_public_key_string[l-1]);){
			vch[l][key_idx] = Sixteen2Ten(Global_thres_enc_public_key_string[l-1][string_idx])*16+Sixteen2Ten(Global_thres_enc_public_key_string[l-1][string_idx + 1]);
			key_idx = key_idx + 1;
			string_idx = string_idx + 3;
		}
		public_key_idx = key_idx;
		
		key_idx = 0;
		for(string_idx = 0; string_idx < sizeof(Global_thres_enc_private_key_string[l-1]);){
			sch[l][key_idx] = Sixteen2Ten(Global_thres_enc_private_key_string[l-1][string_idx])*16 + Sixteen2Ten(Global_thres_enc_private_key_string[l-1][string_idx + 1]);
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
		BIG_160_28_fromBytes(Global_Enc_SKs[i],sch[i]);
	}
	for(int i=1;i<=Enc_players;i++){
		ECP2_BN158_fromOctet(&Global_Enc_VKs[i],&RAW[i]);
	}
	ECP2_BN158_fromOctet(&Global_Enc_VK,&RAW[Nodes_N+1]);
	//dealer();
	//while(1);
	//Enc_dealer();
	
	for(u8 i=0;i<20;i++)Enc_S[i] = i;
	
//	char test256[]="abcdbcdecdefdefgefghfghighijhijkijkljklmklmnlmnomnopnopq";
//	//char digest[50];
//	//hash256 sh256;
//	
//	for(int i=0;i<50;i++)digest[i] = NULL;
//	HASH256_init(&sh256);
//	for (i=0;test256[i]!=0;i++) HASH256_process(&sh256,test256[i]);
//    HASH256_hash(&sh256,digest);
//	
//	for(int i=0;i<50;i++)Enc_M[i]=digest[i];
//	
//	FP_BN158 M_FP;
//	BIG_160_28 M_num;
//	FP_BN158_fromBytes(&M_FP,M);
//	
//	if(sign_FP(&M_FP)){
//		// negative num
//		FP_BN158 M_FP_neg;
//		FP_BN158_neg(&M_FP_neg,&M_FP);
//		FP_BN158_redc(M_num,&M_FP);
//		ECP_BN158_copy(&M_point,&Enc_G1);
//		PAIR_BN158_G1mul(&M_point,M_num);
//		ECP_BN158_neg(&M_point);
//	}else{
//		FP_BN158_redc(M_num,&M_FP);
//		ECP_BN158_copy(&M_point,&Enc_G1);
//		PAIR_BN158_G1mul(&M_point,M_num);
//	}
	
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




void Global_Tx_PK_encrypt(char* m, char *enc){
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
	
	BIG_160_28 modl;
	BIG_160_28_rcopy(modl, CURVE_Order_BN158);
	
	BIG_160_28 r;
	BIG_160_28_randomnum(r,modl,&RNGx);
	
	//	U = g1 ** r
	ECP_BN158_copy(&Tx_U[Group_ID],&Enc_g1);
	PAIR_BN158_G1mul(&Tx_U[Group_ID],r);
	
	octet U_oct;				//Res->oct
	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
	U_oct.val = U_ch;
	ECP_BN158_toOctet(&U_oct,&Tx_U[Group_ID],true);
	for(int i=0;i<Share_Size;i++)Tx_U_str[Group_ID][i] = U_ch[i];
	
	
///////V = xor(m, hashG(self.VK ** r))
	//改成下面的
	//#V = xor(m, hashG(pair(g1, self.VK ** r)))
	
	ECP_BN158 g1_neg;
	ECP_BN158_copy(&g1_neg,&Enc_g1);
	ECP_BN158_neg(&g1_neg);
	
	ECP2_BN158 VKr;				//VK ** r
	ECP2_BN158_copy(&VKr,&Global_Enc_VK);
	PAIR_BN158_G2mul(&VKr,r);
	
	FP12_BN158 pair_r;//pair(g1, self.VK ** r)
	PAIR_BN158_ate(&pair_r,&VKr,&Enc_g1);
	PAIR_BN158_fexp(&pair_r);

	
	
	octet pair_r_oct;				//VKr->oct
	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
	pair_r_oct.val = pair_r_ch;
	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
	
	
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
//	ECP_BN158_toOctet(&U_oct,&Tx_U[ID],true);
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
//	ECP2_BN158_copy(&Tx_W[ID],&Enc_G2);
//	PAIR_BN158_G2mul(&Tx_W[ID],W_num);
//	ECP2_BN158_copy(&Tx_H[ID],&Tx_W[ID]); //	H
//	PAIR_BN158_G2mul(&Tx_W[ID],r_num);
//	
//	ECP_BN158_copy(&g1_neg,&Enc_g1);
//	ECP_BN158_neg(&g1_neg);
//	
//	FP12_BN158 v;
//	PAIR_BN158_double_ate(&v,&W,&g1_neg,&H,&U);
//	PAIR_BN158_fexp(&v);
//    if (FP12_BN158_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
//	// test single fail 下面的
//	FP12_BN158 r1,r2;
//	
//	PAIR_BN158_ate(&r1,&W,&Enc_g1);
//	PAIR_BN158_ate(&r2,&H,&U);
//	
//	PAIR_BN158_fexp(&r1);
//	PAIR_BN158_fexp(&r2);
//	
//	if(FP12_BN158_equals(&r1,&r2)){
//		sprintf((char*)Send_Data_buff,"Test single verify: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Test single verify: No");
//		LoRa_SendData(Send_Data_buff);
//	}
	
}











void Global_Tx_PK_verify_ciphertext(){

}

void Global_Tx_SK_decrypt_share(u8 tx_id, u8 id){
	// id -> tx_id

	ECP_BN158_copy(&Tx_shares[tx_id][id-1],&Tx_U[tx_id]);
	PAIR_BN158_G1mul(&Tx_shares[tx_id][id-1],Global_Enc_SKs[id]);
	
	octet S_oct;				//Res->oct
	char S_ch[100];for(int i=0;i<100;i++)S_ch[i]=NULL;
	S_oct.val = S_ch;
	ECP_BN158_toOctet(&S_oct,&Tx_shares[tx_id][id-1],true);
	for(int i=0;i<Share_Size;i++)Tx_shares_str[tx_id][id-1][i] = S_ch[i];
	
//	BIG_160_28 sk_num;
//	for(int i=1;i<=Enc_players;i++){
//		ECP_BN158_copy(&Tx_shares[ID][i-1],&Tx_U[ID]);
//		FP_BN158_redc(sk_num,&Enc_SKs[i]);
//		PAIR_BN158_G1mul(&Tx_shares[ID][i-1],sk_num);
//	}
	
}

int Global_Tx_PK_verify_share(u8 tx_id, u8 id){
	//id -> tx_id
	
	ECP2_BN158 g2_neg;
	ECP2_BN158_copy(&g2_neg,&Enc_g2);
	ECP2_BN158_neg(&g2_neg);
	
	//pair(U_i, g2) == pair(U, Y_i)
	FP12_BN158 v;
	//PAIR_BN158_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
	PAIR_BN158_double_ate(&v,&g2_neg,&Tx_shares[tx_id][id-1],&Global_Enc_VKs[id],&Tx_U[tx_id]);
	PAIR_BN158_fexp(&v);
	
	if (FP12_BN158_isunity(&v)){
//		sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: Yes",id);
//		LoRa_SendData(Send_Data_buff);
		return 1;
	}else{
		while(1){	
			sprintf((char*)Send_Data_buff,"Global - Tx  PK_verify_share[%d]: No %d",tx_id, id);
			LoRa_SendData(Send_Data_buff);
			delay_ms(1000);
		}
		return 0;
	}
	
	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	ECP2_BN158 g2_neg;
//	ECP2_BN158_copy(&g2_neg,&Enc_g2);
//	ECP2_BN158_neg(&g2_neg);
//	
//	for(int i=1;i<=Enc_players;i++){
//		//pair(U_i, g2) == pair(U, Y_i)
//		FP12_BN158 v;
//		//PAIR_BN158_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//		PAIR_BN158_double_ate(&v,&g2_neg,&Tx_shares[ID][i-1],&Enc_VKs[i],&Tx_U[ID]);
//		PAIR_BN158_fexp(&v);
//		
//		if (FP12_BN158_isunity(&v)){
//			sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: Yes",i);
//			LoRa_SendData(Send_Data_buff);
//		}else{
//			sprintf((char*)Send_Data_buff,"Tx  PK_verify_share[%d]: No",i);
//			LoRa_SendData(Send_Data_buff);
//		}
//	}
}

void Global_Tx_PK_combine_share(u8 tx_id, char * enc, char *dec){
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
	
	for(int k=1;k<=Nodes_N;k++){
		if(ACK_Share[tx_id][k] == 0)
			Global_Tx_SK_decrypt_share(tx_id, k);
	}
	
	for(int j=0 ; j<Enc_Thres_k ; j++){
//		share ** self.lagrange(S, j)
		u8 jj = Enc_S[j];
		Enc_lagrange(larg,jj);
		ECP_BN158_copy(&tmp,&Tx_shares[tx_id][jj]);
		PAIR_BN158_G1mul(&tmp,larg);
		ECP_BN158_copy(&r[j],&tmp);	
	}
	
	for(int j=1 ; j<Enc_Thres_k ; j++){
		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
	}
	ECP_BN158_copy(&res,&r[0]);	
	
	
	FP12_BN158 pair_r;//pair(g2, res)
	PAIR_BN158_ate(&pair_r,&Enc_g2,&res);
	PAIR_BN158_fexp(&pair_r);
	
	
	octet pair_r_oct;				//VKr->oct
	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
	pair_r_oct.val = pair_r_ch;
	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
	
	hash256 sh256;//hash VKr_oct  
	HASH256_init(&sh256);
	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
	HASH256_hash(&sh256,digest);
	
	
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



/*******************************************************************/









////////////////////////////////////////////////////////////////////////////////////////////////////////////
//void PK_encrypt(char* m){
////	# Only encrypt 32 byte strings
////	assert len(m) == 32
////	r = group.random(ZR)
////	U = g1 ** r
////	V = xor(m, hashG(self.VK ** r))
////	W = hashH(U, V) ** r
////	C = (U, V, W)
////	return C
//	/***************************************************/
//	//	r = group.random(ZR)
//	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
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
//	FP_BN158 r;
//	FP_BN158_from_int(&r,ran);
//	//FP_BN158_rand(&r,&RNGx);
//	
////	while(sign_FP(&r)){
////		FP_BN158_rand(&r,&RNGx);
////	}
//	
//	//u16 xxx = ran;
////	FP_BN158_from_int(&r,ran);
////	while(sign_FP(&r)){
////		ran = 200;//RNG_Get_RandomNum() % 1;
////		FP_BN158_from_int(&r,ran);
////	}
//	
//	FP_BN158_copy(&R,&r);
//	
//	
//	//	U = g1 ** r
//	
//	BIG_160_28 r_num;
//	ECP_BN158_copy(&U,&Enc_g1);
//	FP_BN158_redc(r_num,&r);
//	PAIR_BN158_G1mul(&U,r_num);
//	
//	BIG_160_28_copy(R_num,r_num);
//	
/////////V = xor(m, hashG(self.VK ** r))
//	
//	//#V = xor(m, hashG(pair(g1, self.VK ** r)))
//	
//	ECP_BN158 g1_neg;
//	ECP_BN158_copy(&g1_neg,&Enc_g1);
//	ECP_BN158_neg(&g1_neg);
//	
//	ECP2_BN158 VKr;				//VK ** r
//	ECP2_BN158_copy(&VKr,&Enc_VK);
//	PAIR_BN158_G2mul(&VKr,r_num);
//	
//	FP12_BN158 pair_r;//pair(g1, self.VK ** r)
//	PAIR_BN158_ate(&pair_r,&VKr,&Enc_g1);
//	PAIR_BN158_fexp(&pair_r);
//	//testdemo();
////	if (FP12_BN158_equals(&pair_r,&pair1)){
////		sprintf((char*)Send_Data_buff,"Test Pair 1: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"Test Pair 1: No");
////		LoRa_SendData(Send_Data_buff);
////	}
//	
//	
//	octet pair_r_oct;				//VKr->oct
//	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
//	pair_r_oct.val = pair_r_ch;
//	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
//	
//	
//	hash256 sh256;//hash VKr_oct  
//	HASH256_init(&sh256);
//	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
//	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
//	HASH256_hash(&sh256,digest);
//	
//	for(int i=0;i<32;i++){	//xor m and VKr->oct
//		V[i] = m[i]^digest[i];
//	}
//	V[32] = NULL;
//	
//	//u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_encrypt_time[Run_idx] = time;

//	//	sprintf((char*)Send_Data_buff,"Encrypt Time:%d",time);
////	LoRa_SendData(Send_Data_buff);
//	
////	/**************************Encrypt end**********************************/
////	
//////	W = hashH(U, V) ** r
////	octet U_oct;				//U->oct
////	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
////	U_oct.val = U_ch;
////	ECP_BN158_toOctet(&U_oct,&U,true);
////	
////	octet W_oct;		//U+V
////	char W_ch[100];for(int i=0;i<100;i++)W_ch[i]=NULL;
////	W_oct.val = W_ch;
////	W_oct.max = 100;
////	OCT_copy(&W_oct,&U_oct);
////	OCT_jstring(&W_oct,V);
////	
////	char digest1[50];for(int i=0;i<50;i++)digest1[i]=NULL;
////	HASH256_init(&sh256);
////	for (int i=0;W_oct.val[i]!=0;i++) HASH256_process(&sh256,W_oct.val[i]);
////	HASH256_hash(&sh256,digest1);
////	BIG_160_28 W_num;
////	BIG_160_28_fromBytes(W_num,digest1);
////	
////	
////	ECP2_BN158_copy(&W,&Enc_G2);
////	PAIR_BN158_G2mul(&W,W_num);
////	ECP2_BN158_copy(&H,&W); //	H
////	PAIR_BN158_G2mul(&W,r_num);
////	
////	ECP_BN158_copy(&g1_neg,&Enc_g1);
////	ECP_BN158_neg(&g1_neg);
////	
////	FP12_BN158 v;
////	PAIR_BN158_double_ate(&v,&W,&g1_neg,&H,&U);
////	PAIR_BN158_fexp(&v);
////    if (FP12_BN158_isunity(&v)){
////		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"PK_encrypt verify_ciphertext: No");
////		LoRa_SendData(Send_Data_buff);
////	}
//	
////	// test single fail 下面的
////	FP12_BN158 r1,r2;
////	
////	PAIR_BN158_ate(&r1,&W,&Enc_g1);
////	PAIR_BN158_ate(&r2,&H,&U);
////	
////	PAIR_BN158_fexp(&r1);
////	PAIR_BN158_fexp(&r2);
////	
////	if(FP12_BN158_equals(&r1,&r2)){
////		sprintf((char*)Send_Data_buff,"Test single verify: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"Test single verify: No");
////		LoRa_SendData(Send_Data_buff);
////	}
//		
//}

//void PK_verify_ciphertext(){
////	def verify_ciphertext(self, (U, V, W)):
////	# Check correctness of ciphertext
////	H = hashH(U, V)
////	assert pair(g1, W) == pair(U, H)
////	return True
//	//H = hashH(U+V);
////	ECP2_BN158 H;
//	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	octet U_oct;				//U->oct
//	char U_ch[100];for(int i=0;i<100;i++)U_ch[i]=NULL;
//	U_oct.val = U_ch;
//	ECP_BN158_toOctet(&U_oct,&U,true);
//	
//	octet H_oct;		//U+V
//	char H_ch[100];for(int i=0;i<100;i++)H_ch[i]=NULL;
//	H_oct.val = H_ch;
//	H_oct.max = 100;
//	OCT_copy(&H_oct,&U_oct);
//	OCT_jstring(&H_oct,V);
//	
//	hash256 sh256;
//	char digest1[50];for(int i=0;i<50;i++)digest1[i]=NULL;
//	HASH256_init(&sh256);
//	for (int i=0;H_oct.val[i]!=0;i++) HASH256_process(&sh256,H_oct.val[i]);
//	HASH256_hash(&sh256,digest1);
//	BIG_160_28 H_num;
//	BIG_160_28_fromBytes(H_num,digest1);

//	ECP2_BN158_copy(&H,&Enc_G2);
//	PAIR_BN158_G2mul(&H,H_num);
//	
//	
//	ECP_BN158 g1_neg;
//	ECP_BN158_copy(&g1_neg,&Enc_g1);
//	ECP_BN158_neg(&g1_neg);
//	
//	FP12_BN158 v;
//	PAIR_BN158_double_ate(&v,&W,&g1_neg,&H,&U);
//	PAIR_BN158_fexp(&v);
//	
//	//u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_ciphertext_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"Verify Ciphertext Time:%d",time);
////	LoRa_SendData(Send_Data_buff);
//	
//	/**************************verify end**********************************/
//	
////    if (FP12_BN158_isunity(&v)){
////		sprintf((char*)Send_Data_buff,"PK_verify_ciphertext: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"PK_verify_ciphertext: No");
////		LoRa_SendData(Send_Data_buff);
////	}
//	
//}
//void SK_decrypt_share(){
//	//U_i = U ** self.SK
//	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	BIG_160_28 sk_num;
//	for(int i=1;i<=Enc_players;i++){
//		ECP_BN158_copy(&shares[i-1],&U);
//		FP_BN158_redc(sk_num,&Enc_SKs[i]);
//		PAIR_BN158_G1mul(&shares[i-1],sk_num);
//	}
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	SK_decrypt_share_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"Shares Construction Time:%d",time);
////	LoRa_SendData(Send_Data_buff);
//	
//	/**************************Shares Construction**********************************/
//	
////	sprintf((char*)Send_Data_buff,"Shares Construction: Yes");
////	LoRa_SendData(Send_Data_buff);
//}

//void PK_verify_share(){
////	def verify_share(self, i, U_i, (U,V,W)):
////	assert 0 <= i < self.l
////	Y_i = self.VKs[i]
////	assert pair(U_i, g2) == pair(U, Y_i)
////	return True
//	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	ECP2_BN158 g2_neg;
//	ECP2_BN158_copy(&g2_neg,&Enc_g2);
//	ECP2_BN158_neg(&g2_neg);
//	
//	for(int i=1;i<=Enc_players;i++){
//		//pair(U_i, g2) == pair(U, Y_i)
//		FP12_BN158 v;
//		//PAIR_BN158_double_ate(&v,&shares[i],&g2_neg,&Enc_VKs[i],&U);
//		PAIR_BN158_double_ate(&v,&g2_neg,&shares[i-1],&Enc_VKs[i],&U);
//		PAIR_BN158_fexp(&v);
//		
////		if (FP12_BN158_isunity(&v)){
////			sprintf((char*)Send_Data_buff,"PK_verify_share[%d]: Yes",i);
////			LoRa_SendData(Send_Data_buff);
////		}else{
////			sprintf((char*)Send_Data_buff,"PK_verify_share[%d]: No",i);
////			LoRa_SendData(Send_Data_buff);
////		}
//	}
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_verify_share_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"PK_verify_share%d:%d",Enc_players,time);
////	LoRa_SendData(Send_Data_buff);
//	
//	/**************************PK_verify_share**********************************/

//}

//void PK_combine_share(){
//	//	def combine_shares(self, (U,V,W), shares):
//	//	# sigs: a mapping from idx -> sig
//	//	S = set(shares.keys())
//	//	assert S.issubset(range(self.l))

//	//	# ASSUMPTION
//	//	# assert self.verify_ciphertext((U,V,W))

//	//	mul = lambda a,b: a*b
//	//	res = reduce(mul, [share ** self.lagrange(S, j)for j,share in shares.iteritems()], ONE)
//	//	return xor(hashG(res), V)
//	
////	sprintf((char*)Send_Data_buff,"Combine shares: start");
////	LoRa_SendData(Send_Data_buff);
//	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
//	
//	FP_BN158 larg;
//	BIG_160_28 larg_num;
//	ECP_BN158 tmp;
//	ECP_BN158 r[20];
//	ECP_BN158 res;

//	for(int j=0 ; j<Enc_Thres_k ; j++){
////		share ** self.lagrange(S, j)
//		u8 jj = Enc_S[j];
//		Enc_lagrange(&larg,jj);
//		FP_BN158_redc(larg_num,&larg);
//		ECP_BN158_copy(&tmp,&shares[jj]);
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
//	for(int j=1 ; j<Enc_Thres_k ; j++){
//		ECP_BN158_add(&r[0],&r[j]);//这里可能不是Group元素了！
//	}
//	ECP_BN158_copy(&res,&r[0]);	
//	
////	//PAIR_BN158_G1member
////	if(PAIR_BN158_G1member(&res)){
////		sprintf((char*)Send_Data_buff,"Test Res Enc_G1 member: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"Test Res Enc_G1 member: No");
////		LoRa_SendData(Send_Data_buff);
////	}
////	
////	//PAIR_BN158_G1member
////	if(ECP_BN158_equals(&RES,&res)){
////		sprintf((char*)Send_Data_buff,"RES = res: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"RES = res: No");
////		LoRa_SendData(Send_Data_buff);
////	}
////	
////	if (ECP_BN158_equals(&res,&g21)){
////		sprintf((char*)Send_Data_buff,"Test g21: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"Test g21: No");
////		LoRa_SendData(Send_Data_buff);
////	}
////	ECP2_BN158 g2_neg;
////	ECP2_BN158_copy(&g2_neg,&g2);
////	ECP2_BN158_neg(&g2_neg);
//	
//	FP12_BN158 pair_r;//pair(g2, res)
//	PAIR_BN158_ate(&pair_r,&Enc_g2,&res);
//	PAIR_BN158_fexp(&pair_r);
//	
////	if (FP12_BN158_equals(&pair_r,&pair2)){
////		sprintf((char*)Send_Data_buff,"Test Pair 2: Yes");
////		LoRa_SendData(Send_Data_buff);
////	}else{
////		sprintf((char*)Send_Data_buff,"Test Pair 2: No");
////		LoRa_SendData(Send_Data_buff);
////	}
//	
//	octet pair_r_oct;				//VKr->oct
//	char pair_r_ch[400];for(int i=0;i<400;i++)pair_r_ch[i]=NULL;
//	pair_r_oct.val = pair_r_ch;
//	FP12_BN158_toOctet(&pair_r_oct,&pair_r);
//	
//	hash256 sh256;//hash VKr_oct  
//	HASH256_init(&sh256);
//	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
//	for (int i=0;pair_r_oct.val[i]!=0;i++) HASH256_process(&sh256,pair_r_oct.val[i]);
//	HASH256_hash(&sh256,digest);
//	
//	
//////	octet Res_oct;				//Res->oct
//////	char Res_ch[100];for(int i=0;i<100;i++)Res_ch[i]=NULL;
//////	Res_oct.val = Res_ch;
//////	ECP_BN158_toOctet(&Res_oct,&res,true);
//////	
//////	hash256 sh256;//hash VKr_oct  
//////	HASH256_init(&sh256);
//////	char digest[50];for(int i=0;i<50;i++)digest[i]=NULL;
//////	for (int i=0;Res_oct.val[i]!=0;i++) HASH256_process(&sh256,Res_oct.val[i]);
//////	HASH256_hash(&sh256,digest);
//	
//	char m_[100];for(int i=0;i<100;i++)m_[i]=NULL;
//	
//	for(int i=0;i<32;i++){
//		m_[i] = digest[i]^V[i];
//	}	
//	
//	for(int i=0;i<50;i++)	Enc_M_[i] = m_[i];
//	
//	int ress = 1;
//	for(int i=0;i<32;i++){
//		if(Enc_M[i]!=Enc_M_[i])ress=0;
//	}
//	
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	PK_combine_share_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"Combine shares: %d",time);
////	LoRa_SendData(Send_Data_buff);
//	
////	/**************************Combine shares**********************************/
////	
//	if(ress){
//		sprintf((char*)Send_Data_buff,"Combine shares: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Combine shares: No");
//		LoRa_SendData(Send_Data_buff);
//	}
////		SS = range(PK.l)
////		for i in range(1):
////      random.shuffle(SS)
////      S = set(SS[:PK.k])
////      
////      m_ = PK.combine_shares(C, dict((s,shares[s]) for s in S))
////      assert m_ == m
//	
//}



void testdemo(){
	

}


void Enc_ff(BIG_160_28 r,BIG_160_28 x){
	BIG_160_28 q;
	BIG_160_28_rcopy(q, CURVE_Order_BN158);
	BIG_160_28 y;
	BIG_160_28 xx;
	BIG_160_28_zero(y);
	BIG_160_28_one(xx);
	BIG_160_28 tmp;
	BIG_160_28 tmp1;
	for(u16 i=0;i<Enc_Thres_k;i++){
		
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
void Enc_lagrange(BIG_160_28 r,u8 j){
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
	for(u8 jj=0;jj<Enc_Thres_k;jj++){
		if(Enc_S[jj] == j) continue;
		//num = num * (0 - S[jj] - 1);
		BIG_160_28_zero(S_jj);
		BIG_160_28_inc(S_jj,Enc_S[jj]);
		
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
	for(u8 jj=0;jj<Enc_Thres_k;jj++){
		if(Enc_S[jj] == j) continue;
		//den = den * (j - S[jj]);
		BIG_160_28_zero(S_jj);
		BIG_160_28_inc(S_jj,Enc_S[jj]);
		
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





void Enc_dealer(){
	
//	TIM9_Exceed_Times = 0;
//	TIM9->CNT = 0;
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
//	//FP_BN158_from_int(&secret,ran);
//	FP_BN158_rand(&secret,&RNGx);
//	
//	FP_BN158_copy(&a[0],&secret);
//	FP_BN158_copy(&SECRET,&secret);

//	for(u16 i=1;i<Enc_Thres_k;i++){
//		
//		//FP_BN158_rand(&a[i],&RNGx);
//		ran = RNG_Get_RandomNum();
//		FP_BN158_from_int(&a[i],ran);
//		//FP_BN158_sign(&a[i]);
//		while(FP_BN158_sign(&a[i])){
//			ran = RNG_Get_RandomNum();
//			FP_BN158_from_int(&a[i],ran);
//			//FP_BN158_rand(&a[i],&RNGx);
//		}

//	}
//	
//	for(u16 i=0;i<Enc_players;i++){
//		FP_BN158 x;
//		FP_BN158_from_int(&x,i+1);
//		Enc_ff(&Enc_SKs[i],&x);
//	}
//	
//	//assert ff(0) == secret
//	FP_BN158 secret1;
//	FP_BN158 ZERO;
//	FP_BN158_zero(&ZERO);
//	Enc_ff(&secret1,&ZERO);
//	if(FP_BN158_equals(&secret1,&secret)){
//		sprintf((char*)Send_Data_buff,"hahaha");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"wuwuwu");
//		LoRa_SendData(Send_Data_buff);
//	}
//	
//	//VK = g2 ** secret
//	ECP2_BN158_copy(&Enc_VK,&Enc_g2);
//	BIG_160_28 FP_num;
//	FP_BN158_redc(FP_num,&secret);
//	PAIR_BN158_G2mul(&Enc_VK,FP_num);
//	
//	//VKs = [g2 ** xx for xx in SKs ]
//	for(u16 i=0 ; i < Enc_players ; i++){
//		ECP2_BN158_copy(&Enc_VKs[i],&Enc_g2);
//		FP_BN158_redc(FP_num,&Enc_SKs[i]);
//		PAIR_BN158_G2mul(&Enc_VKs[i],FP_num);	
//	}
//	
////	u32 time = TIM9->CNT + TIM9_Exceed_Times * 5000;	dealer_time[Run_idx] = time;
////	sprintf((char*)Send_Data_buff,"Dealer Time:%d",time);
////	LoRa_SendData(Send_Data_buff);
//	
////	/**************************Dealer end**********************************/

////	# Check reconstruction of 0
////  S = set(range(0,k))[0,1,2]
////  lhs = f(0)
//	FP_BN158 lhs,rhs,tmp,tmp1,tmp2,tmp3,tmp4;
////	FP_BN158 ZERO;
//	FP_BN158_zero(&ZERO);
//	FP_BN158_zero(&rhs);
//	Enc_ff(&lhs,&ZERO);
//	
////  rhs = sum(public_key.lagrange(S,j) * f(j+1) for j in S)
//	for(u8 i=0;i<20;i++)Enc_S[i] = i;
//	
//	for(u16 i=0;i<Enc_Thres_k;i++){
//		FP_BN158_from_int(&tmp2,Enc_S[i]+1);
//		Enc_ff(&tmp,&tmp2);//tmp = ff[S[i]+1]
//		Enc_lagrange(&tmp1,Enc_S[i]);//tmp1 = lagrange
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
//	if(FP_BN158_equals(&lhs,&rhs) && BIG_160_28_comp(lhs_num,rhs_num)==0){
//		sprintf((char*)Send_Data_buff,"Reconstruction: Yes");
//		LoRa_SendData(Send_Data_buff);
//	}else{
//		sprintf((char*)Send_Data_buff,"Reconstruction: NO");
//		LoRa_SendData(Send_Data_buff);
//	}
	
}





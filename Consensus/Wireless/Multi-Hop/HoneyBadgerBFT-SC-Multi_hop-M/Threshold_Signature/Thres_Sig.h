#ifndef THRES_SIG_H
#define THRES_SIG_H

#include "mw1268_app.h"
#include "mw1268_cfg.h"
#include "usart3.h"
#include "string.h"
#include "led.h"
#include "delay.h"
#include "core.h"
#include "bls_BN158.h"
#include "HoneyBadgerBFT.h"
#include "rng.h"
#include "utility.h"

#define MAX_K 10
#define MAX_SK 20
#define key_num1 119
#define key_num2 194

//extern char thres_private_key_string[Nodes_N][key_num1];
//extern char thres_public_key_string[Nodes_N+1][key_num2];

extern u8 players;// = Nodes_N;
extern u8 Thres_k;// = Nodes_f + 1;
extern DMA_HandleTypeDef UART3TxDMA_Handler;
extern char M[50];
//extern char M_[50];

extern ECP_BN158 G1;
extern ECP2_BN158 G2;
extern ECP_BN158 g1;
extern ECP2_BN158 g2;

extern BIG_160_28 a[MAX_K];
extern u8 S[MAX_SK];
extern BIG_160_28 SECRET;
extern BIG_160_28 SKs[MAX_SK];

extern ECP2_BN158 g2;
extern ECP2_BN158 VK;
extern ECP2_BN158 VKs[MAX_SK];

extern ECP_BN158 M_point;
extern ECP_BN158 Sigs[MAX_SK];
extern ECP_BN158 SIG;



void init_thres_sig();

void ff(BIG_160_28 r,BIG_160_28 x);
void lagrange(BIG_160_28 r,u8 j);
void dealer();
void SK_sign();
void PK_verify_sig_share();
void PK_verify_sig();
void PK_sig_combine_share();


void Coin_SK_sign(u8 aba, int round);
void sign_data(u8* hash,u8* sig);
int Coin_PK_verify_sig_share(u8 id,u8 aba, u8 round, u8 *sig);
void Coin_PK_sig_combine_share(u8 aba, u8 round);
#endif /* THRES_SIG_H */
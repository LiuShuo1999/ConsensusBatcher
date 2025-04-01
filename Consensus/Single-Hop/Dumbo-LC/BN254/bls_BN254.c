/*
 * Copyright (c) 2012-2020 MIRACL UK Ltd.
 *
 * This file is part of MIRACL Core
 * (see https://github.com/miracl/core).
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

/* Boneh-Lynn-Shacham signature 128-bit API */

/* Loosely (for now) following https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-bls-signature-02 */

// Minimal-signature-size variant

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bls_BN254.h"


static FP4_BN254 G2_TAB[G2_TABLE_BN254];  // space for precomputation on fixed G2 parameter

#define CEIL(a,b) (((a)-1)/(b)+1)

/* output u[i] \in F_p */
/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field(int hash,int hlen,FP_BN254 *u,octet *DST,octet *M, int ctr)
{
    int i,j,L,nbq;
    BIG_256_28 q,w;
    DBIG_256_28 dx;
    char okm[256],fd[128];
    octet OKM = {0,sizeof(okm),okm};

    BIG_256_28_rcopy(q, Modulus_BN254);
    nbq=BIG_256_28_nbits(q);
    L=CEIL(nbq+CURVE_SECURITY_BN254,8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_256_28_dfromBytesLen(dx,fd,L);
        BIG_256_28_ctdmod(w,dx,q,8*L-nbq);
        FP_BN254_nres(&u[i],w);
    }
}


/* hash a message to an ECP point, using SHA2, random oracle method */
void BLS_HASH_TO_POINT(ECP_BN254 *P, octet *M)
{
    FP_BN254 u[2];
    ECP_BN254 P1;
    char dst[50];
    octet DST = {0,sizeof(dst),dst};

    OCT_jstring(&DST,(char *)"BLS_SIG_BN254G1_XMD:SHA-256_SVDW_RO_NUL_");
    hash_to_field(MC_SHA2,HASH_TYPE_BN254,u,&DST,M,2);

    ECP_BN254_map2point(P,&u[0]);
    ECP_BN254_map2point(&P1,&u[1]);
    ECP_BN254_add(P,&P1);
    ECP_BN254_cfp(P);
    ECP_BN254_affine(P);
}

int BLS_BN254_INIT()
{
    ECP2_BN254 G;
    if (!ECP2_BN254_generator(&G)) return BLS_FAIL;
    PAIR_BN254_precomp(G2_TAB, &G);  // should be done just once on start-up
    return BLS_OK;
}

/* generate key pair, private key S, public key W */
int BLS_BN254_KEY_PAIR_GENERATE(octet *IKM, octet* S, octet *W)
{
    int nbr,L;
    BIG_256_28 r,s;
    DBIG_256_28 dx;
    ECP2_BN254 G;
    char salt[20],prk[HASH_TYPE_BN254],okm[128],aikm[65],len[2];
    octet SALT = {0,sizeof(salt),salt};
    octet PRK = {0,sizeof(prk),prk};
    octet OKM = {0,sizeof(okm),okm};
    octet AIKM = {0,sizeof(aikm),aikm};
    octet LEN = {0,sizeof(len),len};

    OCT_copy(&AIKM,IKM);
    OCT_jbyte(&AIKM,0,1);

    BIG_256_28_rcopy(r, CURVE_Order_BN254);
    nbr=BIG_256_28_nbits(r);
    L=CEIL(3*CEIL(nbr,8),2);
    OCT_jint(&LEN,L,2);

    if (!ECP2_BN254_generator(&G)) return BLS_FAIL;

    OCT_jstring(&SALT,(char *)"BLS-SIG-KEYGEN-SALT-");
    HKDF_Extract(MC_SHA2,HASH_TYPE_BN254,&PRK,&SALT,&AIKM);
    HKDF_Expand(MC_SHA2,HASH_TYPE_BN254,&OKM,L,&PRK,&LEN);

    BIG_256_28_dfromBytesLen(dx,OKM.val,L);
    BIG_256_28_ctdmod(s,dx,r,8*L-nbr);
    BIG_256_28_toBytes(S->val, s);
    S->len = MODBYTES_256_28;

// SkToPk

    PAIR_BN254_G2mul(&G, s);
    ECP2_BN254_toOctet(W, &G, true);
    return BLS_OK;
}

/* Sign message M using private key S to produce signature SIG */
int BLS_BN254_CORE_SIGN(octet *SIG, octet *M, octet *S)
{
    BIG_256_28 s;
    ECP_BN254 D;
    BLS_HASH_TO_POINT(&D, M);
    BIG_256_28_fromBytes(s, S->val);
    PAIR_BN254_G1mul(&D, s);
    ECP_BN254_toOctet(SIG, &D, true); /* compress output */
    return BLS_OK;
}

/* Verify signature of message M, the signature SIG, and the public key W */
int BLS_BN254_CORE_VERIFY(octet *SIG, octet *M, octet *W)
{
    FP12_BN254 v;
    ECP2_BN254 G, PK;
    ECP_BN254 D, HM;
    BLS_HASH_TO_POINT(&HM, M);
    
	ECP_BN254_fromOctet(&D, SIG);
	if (!PAIR_BN254_G1member(&D)) return BLS_FAIL;
    ECP_BN254_neg(&D);

    ECP2_BN254_fromOctet(&PK, W);
	if (!PAIR_BN254_G2member(&PK)) return BLS_FAIL;

// Use new multi-pairing mechanism

//    FP12_BN254 r[ATE_BITS_BN254];
//    PAIR_BN254_initmp(r);
//    PAIR_BN254_another_pc(r, G2_TAB, &D);
//    PAIR_BN254_another(r, &PK, &HM);
//    PAIR_BN254_miller(&v, r);

//.. or alternatively
      if (!ECP2_BN254_generator(&G)) return BLS_FAIL;
      PAIR_BN254_double_ate(&v,&G,&D,&PK,&HM);

    PAIR_BN254_fexp(&v);
    if (FP12_BN254_isunity(&v)) return BLS_OK;
    return BLS_FAIL;
}


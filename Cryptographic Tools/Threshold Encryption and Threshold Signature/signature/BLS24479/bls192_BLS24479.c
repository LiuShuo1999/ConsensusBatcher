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

/* Boneh-Lynn-Shacham signature 192-bit API */

/* Loosely (for now) following https://datatracker.ietf.org/doc/html/draft-irtf-cfrg-bls-signature-02 */

// Minimal-signature-size variant

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bls192_BLS24479.h"

static FP8_BLS24479 G2_TAB[G2_TABLE_BLS24479];  // space for precomputation on fixed G2 parameter

#define CEIL(a,b) (((a)-1)/(b)+1)

/* output u[i] \in F_p */
/* https://datatracker.ietf.org/doc/draft-irtf-cfrg-hash-to-curve/ */
static void hash_to_field(int hash,int hlen,FP_BLS24479 *u,octet *DST,octet *M, int ctr)
{
    int i,j,L,nbq;
    BIG_480_29 q,w;
    DBIG_480_29 dx;
    char okm[256],fd[128];
    octet OKM = {0,sizeof(okm),okm};

    BIG_480_29_rcopy(q, Modulus_BLS24479);
    nbq=BIG_480_29_nbits(q);
    L=CEIL(nbq+CURVE_SECURITY_BLS24479,8);

    XMD_Expand(hash,hlen,&OKM,L*ctr,DST,M);
    for (i=0;i<ctr;i++)
    {
        for (j=0;j<L;j++)
            fd[j]=OKM.val[i*L+j];
        
        BIG_480_29_dfromBytesLen(dx,fd,L);
        BIG_480_29_ctdmod(w,dx,q,8*L-nbq);
        FP_BLS24479_nres(&u[i],w);
    }
}

/* hash a message to an ECP point, using SHA2, random oracle method */
static void BLS_HASH_TO_POINT(ECP_BLS24479 *P, octet *M)
{
    FP_BLS24479 u[2];
    ECP_BLS24479 P1;
    char dst[50];
    octet DST = {0,sizeof(dst),dst};

    OCT_jstring(&DST,(char *)"BLS_SIG_BLS24479G1_XMD:SHA-384_SVDW_RO_NUL_");
    hash_to_field(MC_SHA2,HASH_TYPE_BLS24479,u,&DST,M,2);

    ECP_BLS24479_map2point(P,&u[0]);
    ECP_BLS24479_map2point(&P1,&u[1]);
    ECP_BLS24479_add(P,&P1);
    ECP_BLS24479_cfp(P);
    ECP_BLS24479_affine(P);
}

int BLS_BLS24479_INIT()
{
    ECP4_BLS24479 G;
    if (!ECP4_BLS24479_generator(&G)) return BLS_FAIL;
    PAIR_BLS24479_precomp(G2_TAB, &G);  // should be done just once on start-up
    return BLS_OK;
}

/* generate key pair, private key S, public key W */
int BLS_BLS24479_KEY_PAIR_GENERATE(octet *IKM, octet* S, octet *W)
{
    int nbr,L;
    BIG_480_29 r,s;
    DBIG_480_29 dx;
    ECP4_BLS24479 G;
    char salt[20],prk[HASH_TYPE_BLS24479],okm[128],aikm[65],len[2];
    octet SALT = {0,sizeof(salt),salt};
    octet PRK = {0,sizeof(prk),prk};
    octet OKM = {0,sizeof(okm),okm};
    octet AIKM = {0,sizeof(aikm),aikm};
    octet LEN = {0,sizeof(len),len};

    OCT_copy(&AIKM,IKM);
    OCT_jbyte(&AIKM,0,1);

    BIG_480_29_rcopy(r, CURVE_Order_BLS24479);
    nbr=BIG_480_29_nbits(r);
    L=CEIL(3*CEIL(nbr,8),2);
    OCT_jint(&LEN,L,2);

    if (!ECP4_BLS24479_generator(&G)) return BLS_FAIL;

    OCT_jstring(&SALT,(char *)"BLS-SIG-KEYGEN-SALT-");
    HKDF_Extract(MC_SHA2,HASH_TYPE_BLS24479,&PRK,&SALT,&AIKM);
    HKDF_Expand(MC_SHA2,HASH_TYPE_BLS24479,&OKM,L,&PRK,&LEN);

    BIG_480_29_dfromBytesLen(dx,OKM.val,L);
    BIG_480_29_ctdmod(s,dx,r,8*L-nbr);
    BIG_480_29_toBytes(S->val, s);
    S->len = MODBYTES_480_29;

// SkToPk

    PAIR_BLS24479_G2mul(&G, s);
    ECP4_BLS24479_toOctet(W, &G, true);
    return BLS_OK;
}

/* Sign message M using private key S to produce signature SIG */
int BLS_BLS24479_CORE_SIGN(octet *SIG, octet *M, octet *S)
{
    BIG_480_29 s;
    ECP_BLS24479 D;
    BLS_HASH_TO_POINT(&D, M);
    BIG_480_29_fromBytes(s, S->val);
    PAIR_BLS24479_G1mul(&D, s);
    ECP_BLS24479_toOctet(SIG, &D, true); /* compress output */
    return BLS_OK;
}

/* Verify signature given message M, the signature SIG, and the public key W */
int BLS_BLS24479_CORE_VERIFY(octet *SIG, octet *M, octet *W)
{
    FP24_BLS24479 v;
    ECP4_BLS24479 G, PK;
    ECP_BLS24479 D, HM;
    BLS_HASH_TO_POINT(&HM, M);

    ECP_BLS24479_fromOctet(&D, SIG);
	if (!PAIR_BLS24479_G1member(&D)) return BLS_FAIL;
    ECP_BLS24479_neg(&D);

    ECP4_BLS24479_fromOctet(&PK, W);
	if (!PAIR_BLS24479_G2member(&PK)) return BLS_FAIL;

// Use new multi-pairing mechanism

    FP24_BLS24479 r[ATE_BITS_BLS24479];
    PAIR_BLS24479_initmp(r);
    PAIR_BLS24479_another_pc(r, G2_TAB, &D);
    PAIR_BLS24479_another(r, &PK, &HM);
    PAIR_BLS24479_miller(&v, r);

//.. or alternatively
//    PAIR_BLS24479_double_ate(&v,&G,&D,&PK,&HM);

    PAIR_BLS24479_fexp(&v);
    if (FP24_BLS24479_isunity(&v)) return BLS_OK;
    return BLS_FAIL;
}


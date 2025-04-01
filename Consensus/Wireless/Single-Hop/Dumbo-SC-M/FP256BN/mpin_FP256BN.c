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

/* MPIN 128-bit API Functions */

/* Version 3.0 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "mpin_FP256BN.h"

#define ROUNDUP(a,b) ((a)-1)/(b)+1

void MPIN_FP256BN_ENCODE_TO_CURVE(octet *DST,octet *ID,octet *HCID)
{
    int j,k,m,L;
    char okm[512],fd[256];
    BIG_256_28 q,r,w;
    ECP_FP256BN P;
    FP_FP256BN u;
    DBIG_256_28 dx;
    octet OKM = {0,sizeof(okm),okm};
    BIG_256_28_rcopy(q,Modulus_FP256BN);
    k=BIG_256_28_nbits(q);
    BIG_256_28_rcopy(r, CURVE_Order_FP256BN);
    m=BIG_256_28_nbits(r);
    L=ROUNDUP(k+ROUNDUP(m,2),8);
    XMD_Expand(MC_SHA2,HASH_TYPE_FP256BN,&OKM,L,DST,ID);
    for (j=0;j<L;j++)
        fd[j]=OKM.val[j];
        
    BIG_256_28_dfromBytesLen(dx,fd,L);
    BIG_256_28_dmod(w,dx,q);
    FP_FP256BN_nres(&u,w);
    ECP_FP256BN_map2point(&P,&u);
    ECP_FP256BN_cfp(&P);
    ECP_FP256BN_affine(&P);
    ECP_FP256BN_toOctet(HCID, &P, false);
}

/* create random secret S */
int MPIN_FP256BN_RANDOM_GENERATE(csprng *RNG, octet* S)
{
    BIG_256_28 r, s;
    BIG_256_28_rcopy(r, CURVE_Order_FP256BN);
    BIG_256_28_randtrunc(s, r, 2 * CURVE_SECURITY_FP256BN, RNG);
    BIG_256_28_toBytes(S->val, s);
    S->len = MODBYTES_256_28;
    return 0;
}

/* Extract PIN from TOKEN for identity CID */
int MPIN_FP256BN_EXTRACT_PIN(octet *CID, int pin, octet *TOKEN)
{
    ECP_FP256BN P, R;
    int res = 0;
    pin %= MAXPIN;

    if (!ECP_FP256BN_fromOctet(&P, TOKEN))  res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_FP256BN_fromOctet(&R,CID)) res=MPIN_INVALID_POINT;
    }
    if (res==0)
    {
        ECP_FP256BN_pinmul(&R, pin, PBLEN);
        ECP_FP256BN_sub(&P, &R);
        ECP_FP256BN_toOctet(TOKEN, &P, false);
    }
    return res;
}

/* Implement step 2 on client side of MPin protocol - SEC=-(x+y)*SEC */
int MPIN_FP256BN_CLIENT_2(octet *X, octet *Y, octet *SEC)
{
    BIG_256_28 px, py, r;
    ECP_FP256BN P;
    int res = 0;
    BIG_256_28_rcopy(r, CURVE_Order_FP256BN);
    if (!ECP_FP256BN_fromOctet(&P, SEC)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        BIG_256_28_fromBytes(px, X->val);
        BIG_256_28_fromBytes(py, Y->val);
        BIG_256_28_add(px, px, py);
        BIG_256_28_mod(px, r);
        PAIR_FP256BN_G1mul(&P, px);
        ECP_FP256BN_neg(&P);
        ECP_FP256BN_toOctet(SEC, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Client secret CST=s*IDHTC where IDHTC is client ID hashed to a curve point, and s is the master secret */
int MPIN_FP256BN_GET_CLIENT_SECRET(octet *S, octet *IDHTC, octet *CST)
{
    ECP_FP256BN P;
    BIG_256_28 s;
    int res = 0;
    BIG_256_28_fromBytes(s, S->val);
    if (!ECP_FP256BN_fromOctet(&P, IDHTC)) res = MPIN_INVALID_POINT;
  
    if (res==0)
    {
        PAIR_FP256BN_G1mul(&P, s);
        ECP_FP256BN_toOctet(CST, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Implement step 1 on client side of MPin protocol */
int MPIN_FP256BN_CLIENT_1(octet *CID, csprng *RNG, octet *X, int pin, octet *TOKEN, octet *SEC, octet *xID)
{
    BIG_256_28 r, x;
    ECP_FP256BN P, T, W;
    int res = 0;

    BIG_256_28_rcopy(r, CURVE_Order_FP256BN);
    if (RNG != NULL)
    {
        BIG_256_28_randtrunc(x, r, 2 * CURVE_SECURITY_FP256BN, RNG);
        X->len = MODBYTES_256_28;
        BIG_256_28_toBytes(X->val, x);
    }
    else
        BIG_256_28_fromBytes(x, X->val);

    if (!ECP_FP256BN_fromOctet(&P,CID)) res=MPIN_INVALID_POINT;

    if (res==0)
    {
        if (!ECP_FP256BN_fromOctet(&T, TOKEN)) res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        pin %= MAXPIN;

        ECP_FP256BN_copy(&W, &P);               // W=H(ID)
        ECP_FP256BN_pinmul(&W, pin, PBLEN);     // W=alpha.H(ID)
        ECP_FP256BN_add(&T, &W);                // T=Token+alpha.H(ID) = s.H(ID)
        PAIR_FP256BN_G1mul(&P, x);              // P=x.H(ID)
        ECP_FP256BN_toOctet(xID, &P, false);    // xID     /* change to TRUE for point compression */
    }

    if (res == 0)
    {
        ECP_FP256BN_toOctet(SEC, &T, false); // V    /* change to TRUE for point compression */
    }
    return res;
}

/* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
int MPIN_FP256BN_GET_SERVER_SECRET(octet *S, octet *SST)
{
    BIG_256_28 r, s;
    ECP2_FP256BN Q;
    int res = 0;
    BIG_256_28_rcopy(r, CURVE_Order_FP256BN);
    ECP2_FP256BN_generator(&Q);
    if (res == 0)
    {
        BIG_256_28_fromBytes(s, S->val);
        PAIR_FP256BN_G2mul(&Q, s);
        ECP2_FP256BN_toOctet(SST, &Q, false);
    }
    return res;
}

/* Implement M-Pin on server side */
int MPIN_FP256BN_SERVER(octet *HID, octet *Y, octet *SST, octet *xID, octet *mSEC)
{
    BIG_256_28 y;
    FP12_FP256BN g;
    ECP2_FP256BN Q, sQ;
    ECP_FP256BN P, R;
    int res = 0;

    ECP2_FP256BN_generator(&Q);
    if (!ECP2_FP256BN_fromOctet(&sQ, SST)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_FP256BN_fromOctet(&R, xID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        BIG_256_28_fromBytes(y, Y->val);
        if (!ECP_FP256BN_fromOctet(&P, HID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        PAIR_FP256BN_G1mul(&P, y); 
        ECP_FP256BN_add(&P, &R); 
        if (!ECP_FP256BN_fromOctet(&R, mSEC))  res = MPIN_INVALID_POINT; // V
    }
    if (res == 0)
    {
        PAIR_FP256BN_double_ate(&g, &Q, &R, &sQ, &P);
        PAIR_FP256BN_fexp(&g);

        if (!FP12_FP256BN_isunity(&g))
        {
            res = MPIN_BAD_PIN;
        }
    }
    return res;
}


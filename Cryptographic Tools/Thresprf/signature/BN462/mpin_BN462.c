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
#include "mpin_BN462.h"

#define ROUNDUP(a,b) ((a)-1)/(b)+1

void MPIN_BN462_ENCODE_TO_CURVE(octet *DST,octet *ID,octet *HCID)
{
    int j,k,m,L;
    char okm[512],fd[256];
    BIG_464_28 q,r,w;
    ECP_BN462 P;
    FP_BN462 u;
    DBIG_464_28 dx;
    octet OKM = {0,sizeof(okm),okm};
    BIG_464_28_rcopy(q,Modulus_BN462);
    k=BIG_464_28_nbits(q);
    BIG_464_28_rcopy(r, CURVE_Order_BN462);
    m=BIG_464_28_nbits(r);
    L=ROUNDUP(k+ROUNDUP(m,2),8);
    XMD_Expand(MC_SHA2,HASH_TYPE_BN462,&OKM,L,DST,ID);
    for (j=0;j<L;j++)
        fd[j]=OKM.val[j];
        
    BIG_464_28_dfromBytesLen(dx,fd,L);
    BIG_464_28_dmod(w,dx,q);
    FP_BN462_nres(&u,w);
    ECP_BN462_map2point(&P,&u);
    ECP_BN462_cfp(&P);
    ECP_BN462_affine(&P);
    ECP_BN462_toOctet(HCID, &P, false);
}

/* create random secret S */
int MPIN_BN462_RANDOM_GENERATE(csprng *RNG, octet* S)
{
    BIG_464_28 r, s;
    BIG_464_28_rcopy(r, CURVE_Order_BN462);
    BIG_464_28_randtrunc(s, r, 2 * CURVE_SECURITY_BN462, RNG);
    BIG_464_28_toBytes(S->val, s);
    S->len = MODBYTES_464_28;
    return 0;
}

/* Extract PIN from TOKEN for identity CID */
int MPIN_BN462_EXTRACT_PIN(octet *CID, int pin, octet *TOKEN)
{
    ECP_BN462 P, R;
    int res = 0;
    pin %= MAXPIN;

    if (!ECP_BN462_fromOctet(&P, TOKEN))  res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_BN462_fromOctet(&R,CID)) res=MPIN_INVALID_POINT;
    }
    if (res==0)
    {
        ECP_BN462_pinmul(&R, pin, PBLEN);
        ECP_BN462_sub(&P, &R);
        ECP_BN462_toOctet(TOKEN, &P, false);
    }
    return res;
}

/* Implement step 2 on client side of MPin protocol - SEC=-(x+y)*SEC */
int MPIN_BN462_CLIENT_2(octet *X, octet *Y, octet *SEC)
{
    BIG_464_28 px, py, r;
    ECP_BN462 P;
    int res = 0;
    BIG_464_28_rcopy(r, CURVE_Order_BN462);
    if (!ECP_BN462_fromOctet(&P, SEC)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        BIG_464_28_fromBytes(px, X->val);
        BIG_464_28_fromBytes(py, Y->val);
        BIG_464_28_add(px, px, py);
        BIG_464_28_mod(px, r);
        PAIR_BN462_G1mul(&P, px);
        ECP_BN462_neg(&P);
        ECP_BN462_toOctet(SEC, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Client secret CST=s*IDHTC where IDHTC is client ID hashed to a curve point, and s is the master secret */
int MPIN_BN462_GET_CLIENT_SECRET(octet *S, octet *IDHTC, octet *CST)
{
    ECP_BN462 P;
    BIG_464_28 s;
    int res = 0;
    BIG_464_28_fromBytes(s, S->val);
    if (!ECP_BN462_fromOctet(&P, IDHTC)) res = MPIN_INVALID_POINT;
  
    if (res==0)
    {
        PAIR_BN462_G1mul(&P, s);
        ECP_BN462_toOctet(CST, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Implement step 1 on client side of MPin protocol */
int MPIN_BN462_CLIENT_1(octet *CID, csprng *RNG, octet *X, int pin, octet *TOKEN, octet *SEC, octet *xID)
{
    BIG_464_28 r, x;
    ECP_BN462 P, T, W;
    int res = 0;

    BIG_464_28_rcopy(r, CURVE_Order_BN462);
    if (RNG != NULL)
    {
        BIG_464_28_randtrunc(x, r, 2 * CURVE_SECURITY_BN462, RNG);
        X->len = MODBYTES_464_28;
        BIG_464_28_toBytes(X->val, x);
    }
    else
        BIG_464_28_fromBytes(x, X->val);

    if (!ECP_BN462_fromOctet(&P,CID)) res=MPIN_INVALID_POINT;

    if (res==0)
    {
        if (!ECP_BN462_fromOctet(&T, TOKEN)) res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        pin %= MAXPIN;

        ECP_BN462_copy(&W, &P);               // W=H(ID)
        ECP_BN462_pinmul(&W, pin, PBLEN);     // W=alpha.H(ID)
        ECP_BN462_add(&T, &W);                // T=Token+alpha.H(ID) = s.H(ID)
        PAIR_BN462_G1mul(&P, x);              // P=x.H(ID)
        ECP_BN462_toOctet(xID, &P, false);    // xID     /* change to TRUE for point compression */
    }

    if (res == 0)
    {
        ECP_BN462_toOctet(SEC, &T, false); // V    /* change to TRUE for point compression */
    }
    return res;
}

/* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
int MPIN_BN462_GET_SERVER_SECRET(octet *S, octet *SST)
{
    BIG_464_28 r, s;
    ECP2_BN462 Q;
    int res = 0;
    BIG_464_28_rcopy(r, CURVE_Order_BN462);
    ECP2_BN462_generator(&Q);
    if (res == 0)
    {
        BIG_464_28_fromBytes(s, S->val);
        PAIR_BN462_G2mul(&Q, s);
        ECP2_BN462_toOctet(SST, &Q, false);
    }
    return res;
}

/* Implement M-Pin on server side */
int MPIN_BN462_SERVER(octet *HID, octet *Y, octet *SST, octet *xID, octet *mSEC)
{
    BIG_464_28 y;
    FP12_BN462 g;
    ECP2_BN462 Q, sQ;
    ECP_BN462 P, R;
    int res = 0;

    ECP2_BN462_generator(&Q);
    if (!ECP2_BN462_fromOctet(&sQ, SST)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_BN462_fromOctet(&R, xID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        BIG_464_28_fromBytes(y, Y->val);
        if (!ECP_BN462_fromOctet(&P, HID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        PAIR_BN462_G1mul(&P, y); 
        ECP_BN462_add(&P, &R); 
        if (!ECP_BN462_fromOctet(&R, mSEC))  res = MPIN_INVALID_POINT; // V
    }
    if (res == 0)
    {
        PAIR_BN462_double_ate(&g, &Q, &R, &sQ, &P);
        PAIR_BN462_fexp(&g);

        if (!FP12_BN462_isunity(&g))
        {
            res = MPIN_BAD_PIN;
        }
    }
    return res;
}


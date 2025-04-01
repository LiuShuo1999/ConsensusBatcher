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
#include "mpin_BLS12443.h"

#define ROUNDUP(a,b) ((a)-1)/(b)+1

void MPIN_BLS12443_ENCODE_TO_CURVE(octet *DST,octet *ID,octet *HCID)
{
    int j,k,m,L;
    char okm[512],fd[256];
    BIG_448_29 q,r,w;
    ECP_BLS12443 P;
    FP_BLS12443 u;
    DBIG_448_29 dx;
    octet OKM = {0,sizeof(okm),okm};
    BIG_448_29_rcopy(q,Modulus_BLS12443);
    k=BIG_448_29_nbits(q);
    BIG_448_29_rcopy(r, CURVE_Order_BLS12443);
    m=BIG_448_29_nbits(r);
    L=ROUNDUP(k+ROUNDUP(m,2),8);
    XMD_Expand(MC_SHA2,HASH_TYPE_BLS12443,&OKM,L,DST,ID);
    for (j=0;j<L;j++)
        fd[j]=OKM.val[j];
        
    BIG_448_29_dfromBytesLen(dx,fd,L);
    BIG_448_29_dmod(w,dx,q);
    FP_BLS12443_nres(&u,w);
    ECP_BLS12443_map2point(&P,&u);
    ECP_BLS12443_cfp(&P);
    ECP_BLS12443_affine(&P);
    ECP_BLS12443_toOctet(HCID, &P, false);
}

/* create random secret S */
int MPIN_BLS12443_RANDOM_GENERATE(csprng *RNG, octet* S)
{
    BIG_448_29 r, s;
    BIG_448_29_rcopy(r, CURVE_Order_BLS12443);
    BIG_448_29_randtrunc(s, r, 2 * CURVE_SECURITY_BLS12443, RNG);
    BIG_448_29_toBytes(S->val, s);
    S->len = MODBYTES_448_29;
    return 0;
}

/* Extract PIN from TOKEN for identity CID */
int MPIN_BLS12443_EXTRACT_PIN(octet *CID, int pin, octet *TOKEN)
{
    ECP_BLS12443 P, R;
    int res = 0;
    pin %= MAXPIN;

    if (!ECP_BLS12443_fromOctet(&P, TOKEN))  res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_BLS12443_fromOctet(&R,CID)) res=MPIN_INVALID_POINT;
    }
    if (res==0)
    {
        ECP_BLS12443_pinmul(&R, pin, PBLEN);
        ECP_BLS12443_sub(&P, &R);
        ECP_BLS12443_toOctet(TOKEN, &P, false);
    }
    return res;
}

/* Implement step 2 on client side of MPin protocol - SEC=-(x+y)*SEC */
int MPIN_BLS12443_CLIENT_2(octet *X, octet *Y, octet *SEC)
{
    BIG_448_29 px, py, r;
    ECP_BLS12443 P;
    int res = 0;
    BIG_448_29_rcopy(r, CURVE_Order_BLS12443);
    if (!ECP_BLS12443_fromOctet(&P, SEC)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        BIG_448_29_fromBytes(px, X->val);
        BIG_448_29_fromBytes(py, Y->val);
        BIG_448_29_add(px, px, py);
        BIG_448_29_mod(px, r);
        PAIR_BLS12443_G1mul(&P, px);
        ECP_BLS12443_neg(&P);
        ECP_BLS12443_toOctet(SEC, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Client secret CST=s*IDHTC where IDHTC is client ID hashed to a curve point, and s is the master secret */
int MPIN_BLS12443_GET_CLIENT_SECRET(octet *S, octet *IDHTC, octet *CST)
{
    ECP_BLS12443 P;
    BIG_448_29 s;
    int res = 0;
    BIG_448_29_fromBytes(s, S->val);
    if (!ECP_BLS12443_fromOctet(&P, IDHTC)) res = MPIN_INVALID_POINT;
  
    if (res==0)
    {
        PAIR_BLS12443_G1mul(&P, s);
        ECP_BLS12443_toOctet(CST, &P, false); /* change to TRUE for point compression */
    }
    return res;
}

/* Implement step 1 on client side of MPin protocol */
int MPIN_BLS12443_CLIENT_1(octet *CID, csprng *RNG, octet *X, int pin, octet *TOKEN, octet *SEC, octet *xID)
{
    BIG_448_29 r, x;
    ECP_BLS12443 P, T, W;
    int res = 0;

    BIG_448_29_rcopy(r, CURVE_Order_BLS12443);
    if (RNG != NULL)
    {
        BIG_448_29_randtrunc(x, r, 2 * CURVE_SECURITY_BLS12443, RNG);
        X->len = MODBYTES_448_29;
        BIG_448_29_toBytes(X->val, x);
    }
    else
        BIG_448_29_fromBytes(x, X->val);

    if (!ECP_BLS12443_fromOctet(&P,CID)) res=MPIN_INVALID_POINT;

    if (res==0)
    {
        if (!ECP_BLS12443_fromOctet(&T, TOKEN)) res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        pin %= MAXPIN;

        ECP_BLS12443_copy(&W, &P);               // W=H(ID)
        ECP_BLS12443_pinmul(&W, pin, PBLEN);     // W=alpha.H(ID)
        ECP_BLS12443_add(&T, &W);                // T=Token+alpha.H(ID) = s.H(ID)
        PAIR_BLS12443_G1mul(&P, x);              // P=x.H(ID)
        ECP_BLS12443_toOctet(xID, &P, false);    // xID     /* change to TRUE for point compression */
    }

    if (res == 0)
    {
        ECP_BLS12443_toOctet(SEC, &T, false); // V    /* change to TRUE for point compression */
    }
    return res;
}

/* Extract Server Secret SST=S*Q where Q is fixed generator in G2 and S is master secret */
int MPIN_BLS12443_GET_SERVER_SECRET(octet *S, octet *SST)
{
    BIG_448_29 r, s;
    ECP2_BLS12443 Q;
    int res = 0;
    BIG_448_29_rcopy(r, CURVE_Order_BLS12443);
    ECP2_BLS12443_generator(&Q);
    if (res == 0)
    {
        BIG_448_29_fromBytes(s, S->val);
        PAIR_BLS12443_G2mul(&Q, s);
        ECP2_BLS12443_toOctet(SST, &Q, false);
    }
    return res;
}

/* Implement M-Pin on server side */
int MPIN_BLS12443_SERVER(octet *HID, octet *Y, octet *SST, octet *xID, octet *mSEC)
{
    BIG_448_29 y;
    FP12_BLS12443 g;
    ECP2_BLS12443 Q, sQ;
    ECP_BLS12443 P, R;
    int res = 0;

    ECP2_BLS12443_generator(&Q);
    if (!ECP2_BLS12443_fromOctet(&sQ, SST)) res = MPIN_INVALID_POINT;
    if (res == 0)
    {
        if (!ECP_BLS12443_fromOctet(&R, xID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        BIG_448_29_fromBytes(y, Y->val);
        if (!ECP_BLS12443_fromOctet(&P, HID))  res = MPIN_INVALID_POINT;
    }
    if (res == 0)
    {
        PAIR_BLS12443_G1mul(&P, y); 
        ECP_BLS12443_add(&P, &R); 
        if (!ECP_BLS12443_fromOctet(&R, mSEC))  res = MPIN_INVALID_POINT; // V
    }
    if (res == 0)
    {
        PAIR_BLS12443_double_ate(&g, &Q, &R, &sQ, &P);
        PAIR_BLS12443_fexp(&g);

        if (!FP12_BLS12443_isunity(&g))
        {
            res = MPIN_BAD_PIN;
        }
    }
    return res;
}


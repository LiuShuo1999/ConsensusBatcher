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

/* CORE basic functions for BIG type */
/* SU=m, SU is Stack Usage */

#include "big_464_28.h"

/* test a=0? */
int BIG_464_28_iszilch(BIG_464_28 a)
{
    int i;
    chunk d=0;
    for (i = 0; i < NLEN_464_28; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_464_28));
}

/* test a=1? */
int BIG_464_28_isunity(BIG_464_28 a)
{
    int i;
    chunk d=0;
    for (i = 1; i < NLEN_464_28; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_464_28) & (((a[0]^1)-1)>>BASEBITS_464_28));
}

/* test a=0? */
int BIG_464_28_diszilch(DBIG_464_28 a)
{
    int i;
    chunk d=0;
    for (i = 0; i < DNLEN_464_28; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_464_28));
}

/* SU= 56 */
/* output a */
void BIG_464_28_output(BIG_464_28 a)
{
    BIG_464_28 b;
    int i, len;
    len = BIG_464_28_nbits(a);
    if (len % 4 == 0) len /= 4;
    else
    {
        len /= 4;
        len++;
    }
    if (len < MODBYTES_464_28 * 2) len = MODBYTES_464_28 * 2;

    for (i = len - 1; i >= 0; i--)
    {
        BIG_464_28_copy(b, a);
        BIG_464_28_shr(b, i * 4);
        printf("%01x", (unsigned int) b[0] & 15);
    }
}

/* SU= 16 */
void BIG_464_28_rawoutput(BIG_464_28 a)
{
    int i;
    printf("(");
    for (i = 0; i < NLEN_464_28 - 1; i++)
#if CHUNK==64
        printf("%"PRIxMAX",", (uintmax_t) a[i]);
    printf("%"PRIxMAX")", (uintmax_t) a[NLEN_464_28 - 1]);
#else
        printf("%x,", (unsigned int) a[i]);
    printf("%x)", (unsigned int) a[NLEN_464_28 - 1]);
#endif
}

/* Swap a and b if d=1 */
chunk BIG_464_28_cswap(BIG_464_28 a, BIG_464_28 b, int d)
{
    int i;
    chunk e,r,ra,w,t, c = (chunk) - d;
    w=0; 
    r=a[0]^b[1]; 
    ra=r+r; ra>>=1; // I know this doesn't change r, but the compiler doesn't!
#ifdef DEBUG_NORM
    for (i = 0; i < NLEN_464_28 + 2; i++)
#else
    for (i = 0; i < NLEN_464_28; i++)
#endif
    {
        t = c & (a[i] ^ b[i]);
        t^=r; 
        e=a[i]^t; w^=e;  // to force calculation of e
        a[i] = e^ra;
        e=b[i]^t; w^=e;
        b[i] = e^ra;
    }
    return w;
}

/* Move b to a if d=1 */
chunk BIG_464_28_cmove(BIG_464_28 f, BIG_464_28 g, int d)
{
    int i;
    chunk e,w,r,ra,t,b = (chunk) - d;
    w=0;
    r=f[0]^g[1];
    ra=r+r; ra>>=1; // I know this doesn't change r, but the compiler doesn't!
#ifdef DEBUG_NORM
    for (i = 0; i < NLEN_464_28 + 2; i++)
#else
    for (i = 0; i < NLEN_464_28; i++)
#endif
    {
        t=(f[i] ^ g[i])&b;
        t^=r;
        e=f[i]^t; w^=e;
        f[i] = e^ra;
    }
    return w;
}

/* Move g to f if d=1 */
chunk BIG_464_28_dcmove(DBIG_464_28 f, DBIG_464_28 g, int d)
{
    int i;
    chunk e,w,r,ra,t,b = (chunk) - d;
    w=0;
    r=f[0]+g[1];
    ra=r+r; ra>>=1; // I know this doesn't change r, but the compiler doesn't!
#ifdef DEBUG_NORM
    for (i = 0; i < DNLEN_464_28 + 2; i++)
#else
    for (i = 0; i < DNLEN_464_28; i++)
#endif
    {
        t=(f[i] ^ g[i])&b;
        t^=r;
        e=f[i]^t; w^=e;
        f[i] = e^ra;
    }
    return w;
}

/* convert BIG to/from bytes */
/* SU= 64 */
void BIG_464_28_toBytes(char *b, BIG_464_28 a)
{
    int i;
    BIG_464_28 c;
    BIG_464_28_copy(c, a);
    BIG_464_28_norm(c);
    for (i = MODBYTES_464_28 - 1; i >= 0; i--)
    {
        b[i] = c[0] & 0xff;
        BIG_464_28_fshr(c, 8);
    }
}

/* SU= 16 */
void BIG_464_28_fromBytes(BIG_464_28 a, char *b)
{
    int i;
    BIG_464_28_zero(a);
    for (i = 0; i < MODBYTES_464_28; i++)
    {
        BIG_464_28_fshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[MPV_464_28] = 1;
    a[MNV_464_28] = 0;
#endif
}

void BIG_464_28_fromBytesLen(BIG_464_28 a, char *b, int s)
{
    int i, len = s;
    BIG_464_28_zero(a);

    if (len > MODBYTES_464_28) len = MODBYTES_464_28;
    for (i = 0; i < len; i++)
    {
        BIG_464_28_fshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[MPV_464_28] = 1;
    a[MNV_464_28] = 0;
#endif
}



/* SU= 88 */
void BIG_464_28_doutput(DBIG_464_28 a)
{
    DBIG_464_28 b;
    int i, len;
    BIG_464_28_dnorm(a);
    len = BIG_464_28_dnbits(a);
    if (len % 4 == 0) len /= 4;
    else
    {
        len /= 4;
        len++;
    }

    for (i = len - 1; i >= 0; i--)
    {
        BIG_464_28_dcopy(b, a);
        BIG_464_28_dshr(b, i * 4);
        printf("%01x", (unsigned int) b[0] & 15);
    }
}


void BIG_464_28_drawoutput(DBIG_464_28 a)
{
    int i;
    printf("(");
    for (i = 0; i < DNLEN_464_28 - 1; i++)
#if CHUNK==64
        printf("%"PRIxMAX",", (uintmax_t) a[i]);
    printf("%"PRIxMAX")", (uintmax_t) a[DNLEN_464_28 - 1]);
#else
        printf("%x,", (unsigned int) a[i]);
    printf("%x)", (unsigned int) a[DNLEN_464_28 - 1]);
#endif
}

/* Copy b=a */
void BIG_464_28_copy(BIG_464_28 b, BIG_464_28 a)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_464_28] = a[MPV_464_28];
    b[MNV_464_28] = a[MNV_464_28];
#endif
}

/* Copy from ROM b=a */
void BIG_464_28_rcopy(BIG_464_28 b, const BIG_464_28 a)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_464_28] = 1;
    b[MNV_464_28] = 0;
#endif
}

/* double length DBIG copy b=a */
void BIG_464_28_dcopy(DBIG_464_28 b, DBIG_464_28 a)
{
    int i;
    for (i = 0; i < DNLEN_464_28; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[DMPV_464_28] = a[DMPV_464_28];
    b[DMNV_464_28] = a[DMNV_464_28];
#endif
}

/* Copy BIG to bottom half of DBIG */
void BIG_464_28_dscopy(DBIG_464_28 b, BIG_464_28 a)
{
    int i;
    for (i = 0; i < NLEN_464_28 - 1; i++)
        b[i] = a[i];

    b[NLEN_464_28 - 1] = a[NLEN_464_28 - 1] & BMASK_464_28; /* top word normalized */
    b[NLEN_464_28] = a[NLEN_464_28 - 1] >> BASEBITS_464_28;

    for (i = NLEN_464_28 + 1; i < DNLEN_464_28; i++) b[i] = 0;
#ifdef DEBUG_NORM
    b[DMPV_464_28] = a[MPV_464_28];
    b[DMNV_464_28] = a[MNV_464_28];
#endif
}

/* Copy BIG to top half of DBIG */
void BIG_464_28_dsucopy(DBIG_464_28 b, BIG_464_28 a)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++)
        b[i] = 0;
    for (i = NLEN_464_28; i < DNLEN_464_28; i++)
        b[i] = a[i - NLEN_464_28];
#ifdef DEBUG_NORM
    b[DMPV_464_28] = a[MPV_464_28];
    b[DMNV_464_28] = a[MNV_464_28];
#endif
}

/* Copy bottom half of DBIG to BIG */
void BIG_464_28_sdcopy(BIG_464_28 b, DBIG_464_28 a)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_464_28] = a[DMPV_464_28];
    b[MNV_464_28] = a[DMNV_464_28];
#endif
}

/* Copy top half of DBIG to BIG */
void BIG_464_28_sducopy(BIG_464_28 b, DBIG_464_28 a)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++)
        b[i] = a[i + NLEN_464_28];
#ifdef DEBUG_NORM
    b[MPV_464_28] = a[DMPV_464_28];
    b[MNV_464_28] = a[DMNV_464_28];

#endif
}

/* Set a=0 */
void BIG_464_28_zero(BIG_464_28 a)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[MPV_464_28] = a[MNV_464_28] = 0;
#endif
}

void BIG_464_28_dzero(DBIG_464_28 a)
{
    int i;
    for (i = 0; i < DNLEN_464_28; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[DMPV_464_28] = a[DMNV_464_28] = 0;
#endif
}

/* set a=1 */
void BIG_464_28_one(BIG_464_28 a)
{
    int i;
    a[0] = 1;
    for (i = 1; i < NLEN_464_28; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[MPV_464_28] = 1;
    a[MNV_464_28] = 0;
#endif
}



/* Set c=a+b */
/* SU= 8 */
void BIG_464_28_add(BIG_464_28 c, BIG_464_28 a, BIG_464_28 b)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++)
        c[i] = a[i] + b[i];
#ifdef DEBUG_NORM
    c[MPV_464_28] = a[MPV_464_28] + b[MPV_464_28];
    c[MNV_464_28] = a[MNV_464_28] + b[MNV_464_28];
    if (c[MPV_464_28] > NEXCESS_464_28)  printf("add problem - positive digit overflow %d\n", (int)c[MPV_464_28]);
    if (c[MNV_464_28] > NEXCESS_464_28)  printf("add problem - negative digit overflow %d\n", (int)c[MNV_464_28]);

#endif
}

/* Set c=a or b */
void BIG_464_28_or(BIG_464_28 c, BIG_464_28 a, BIG_464_28 b)
{
    int i;
    BIG_464_28_norm(a);
    BIG_464_28_norm(b);
    for (i = 0; i < NLEN_464_28; i++)
        c[i] = a[i] | b[i];
#ifdef DEBUG_NORM
    c[MPV_464_28] = 1;
    c[MNV_464_28] = 0;
#endif
}


/* Set c=c+d */
void BIG_464_28_inc(BIG_464_28 c, int d)
{
    BIG_464_28_norm(c);
    c[0] += (chunk)d;
#ifdef DEBUG_NORM
    c[MPV_464_28] += 1;
#endif
}

/* Set c=a-b */
/* SU= 8 */
void BIG_464_28_sub(BIG_464_28 c, BIG_464_28 a, BIG_464_28 b)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++)
        c[i] = a[i] - b[i];
#ifdef DEBUG_NORM
    c[MPV_464_28] = a[MPV_464_28] + b[MNV_464_28];
    c[MNV_464_28] = a[MNV_464_28] + b[MPV_464_28];
    if (c[MPV_464_28] > NEXCESS_464_28)  printf("sub problem - positive digit overflow %d\n", (int)c[MPV_464_28]);
    if (c[MNV_464_28] > NEXCESS_464_28)  printf("sub problem - negative digit overflow %d\n", (int)c[MNV_464_28]);

#endif
}

/* SU= 8 */

void BIG_464_28_dsub(DBIG_464_28 c, DBIG_464_28 a, DBIG_464_28 b)
{
    int i;
    for (i = 0; i < DNLEN_464_28; i++)
        c[i] = a[i] - b[i];
#ifdef DEBUG_NORM
    c[DMPV_464_28] = a[DMPV_464_28] + b[DMNV_464_28];
    c[DMNV_464_28] = a[DMNV_464_28] + b[DMPV_464_28];
    if (c[DMPV_464_28] > NEXCESS_464_28)  printf("double sub problem - positive digit overflow %d\n", (int)c[DMPV_464_28]);
    if (c[DMNV_464_28] > NEXCESS_464_28)  printf("double sub problem - negative digit overflow %d\n", (int)c[DMNV_464_28]);
#endif
}

void BIG_464_28_dadd(DBIG_464_28 c, DBIG_464_28 a, DBIG_464_28 b)
{
    int i;
    for (i = 0; i < DNLEN_464_28; i++)
        c[i] = a[i] + b[i];
#ifdef DEBUG_NORM
    c[DMPV_464_28] = a[DMPV_464_28] + b[DMNV_464_28];
    c[DMNV_464_28] = a[DMNV_464_28] + b[DMPV_464_28];
    if (c[DMPV_464_28] > NEXCESS_464_28)  printf("double add problem - positive digit overflow %d\n", (int)c[DMPV_464_28]);
    if (c[DMNV_464_28] > NEXCESS_464_28)  printf("double add problem - negative digit overflow %d\n", (int)c[DMNV_464_28]);
#endif
}

/* Set c=c-1 */
void BIG_464_28_dec(BIG_464_28 c, int d)
{
    BIG_464_28_norm(c);
    c[0] -= (chunk)d;
#ifdef DEBUG_NORM
    c[MNV_464_28] += 1;
#endif
}

/* multiplication r=a*c by c<=NEXCESS_464_28 */
void BIG_464_28_imul(BIG_464_28 r, BIG_464_28 a, int c)
{
    int i;
    for (i = 0; i < NLEN_464_28; i++) r[i] = a[i] * c;
#ifdef DEBUG_NORM
    r[MPV_464_28] = a[MPV_464_28] * c;
    r[MNV_464_28] = a[MNV_464_28] * c;
    if (r[MPV_464_28] > NEXCESS_464_28)  printf("int mul problem - positive digit overflow %d\n", (int)r[MPV_464_28]);
    if (r[MNV_464_28] > NEXCESS_464_28)  printf("int mul problem - negative digit overflow %d\n", (int)r[MNV_464_28]);

#endif
}

/* multiplication r=a*c by larger integer - c<=FEXCESS */
/* SU= 24 */
chunk BIG_464_28_pmul(BIG_464_28 r, BIG_464_28 a, int c)
{
    int i;
    chunk ak, carry = 0;
    for (i = 0; i < NLEN_464_28; i++)
    {
        ak = a[i];
        r[i] = 0;
        carry = muladd_464_28(ak, (chunk)c, carry, &r[i]);
    }
#ifdef DEBUG_NORM
    r[MPV_464_28] = 1;
    r[MNV_464_28] = 0;
#endif
    return carry;
}

/* r/=3 */
/* SU= 16 */
int BIG_464_28_div3(BIG_464_28 r)
{
    int i;
    chunk ak, base, carry = 0;
    BIG_464_28_norm(r);
    base = ((chunk)1 << BASEBITS_464_28);
    for (i = NLEN_464_28 - 1; i >= 0; i--)
    {
        ak = (carry * base + r[i]);
        r[i] = ak / 3;
        carry = ak % 3;
    }
    return (int)carry;
}

/* multiplication c=a*b by even larger integer b>FEXCESS, resulting in DBIG */
/* SU= 24 */
void BIG_464_28_pxmul(DBIG_464_28 c, BIG_464_28 a, int b)
{
    int j;
    chunk carry;
    BIG_464_28_dzero(c);
    carry = 0;
    for (j = 0; j < NLEN_464_28; j++)
        carry = muladd_464_28(a[j], (chunk)b, carry, &c[j]);
    c[NLEN_464_28] = carry;
#ifdef DEBUG_NORM
    c[DMPV_464_28] = 1;
    c[DMNV_464_28] = 0;
#endif
}

/* .. if you know the result will fit in a BIG, c must be distinct from a and b */
/* SU= 40 */
void BIG_464_28_smul(BIG_464_28 c, BIG_464_28 a, BIG_464_28 b)
{
    int i, j;
    chunk carry;

    BIG_464_28_zero(c);
    for (i = 0; i < NLEN_464_28; i++)
    {
        carry = 0;
        for (j = 0; j < NLEN_464_28; j++)
        {
            if (i + j < NLEN_464_28)
                carry = muladd_464_28(a[i], b[j], carry, &c[i + j]);
        }
    }
#ifdef DEBUG_NORM
    c[MPV_464_28] = 1;
    c[MNV_464_28] = 0;
#endif

}

/* Set c=a*b */
/* SU= 72 */
//void BIG_464_28_mul(chunk c[restrict DNLEN_464_28],chunk a[restrict NLEN_464_28],chunk b[restrict NLEN_464_28])
void BIG_464_28_mul(DBIG_464_28 c, BIG_464_28 a, BIG_464_28 b)
{
    int i;
#ifdef dchunk
    dchunk t, co;
    dchunk s;
    dchunk d[NLEN_464_28];
    int k;
#endif

#ifdef DEBUG_NORM
    if ((a[MPV_464_28] != 1 && a[MPV_464_28] != 0) || a[MNV_464_28] != 0) printf("First input to mul not normed\n");
    if ((b[MPV_464_28] != 1 && b[MPV_464_28] != 0) || b[MNV_464_28] != 0) printf("Second input to mul not normed\n");
#endif

    /* Faster to Combafy it.. Let the compiler unroll the loops! */

#ifdef COMBA

    /* faster psuedo-Karatsuba method */
#ifdef UNWOUND

#ifdef USE_KARATSUBA

    	d[0]=(dchunk)a[0]*b[0];
	d[1]=(dchunk)a[1]*b[1];
	d[2]=(dchunk)a[2]*b[2];
	d[3]=(dchunk)a[3]*b[3];
	d[4]=(dchunk)a[4]*b[4];
	d[5]=(dchunk)a[5]*b[5];
	d[6]=(dchunk)a[6]*b[6];
	d[7]=(dchunk)a[7]*b[7];
	d[8]=(dchunk)a[8]*b[8];
	d[9]=(dchunk)a[9]*b[9];
	d[10]=(dchunk)a[10]*b[10];
	d[11]=(dchunk)a[11]*b[11];
	d[12]=(dchunk)a[12]*b[12];
	d[13]=(dchunk)a[13]*b[13];
	d[14]=(dchunk)a[14]*b[14];
	d[15]=(dchunk)a[15]*b[15];
	d[16]=(dchunk)a[16]*b[16];

	s=d[0];
	t = s; c[0]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28;
	s+=d[1]; t=co+s +(dchunk)(a[1]-a[0])*(b[0]-b[1]); c[1]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[2]; t=co+s +(dchunk)(a[2]-a[0])*(b[0]-b[2]); c[2]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[3]; t=co+s +(dchunk)(a[3]-a[0])*(b[0]-b[3])+(dchunk)(a[2]-a[1])*(b[1]-b[2]); c[3]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[4]; t=co+s +(dchunk)(a[4]-a[0])*(b[0]-b[4])+(dchunk)(a[3]-a[1])*(b[1]-b[3]); c[4]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[5]; t=co+s +(dchunk)(a[5]-a[0])*(b[0]-b[5])+(dchunk)(a[4]-a[1])*(b[1]-b[4])+(dchunk)(a[3]-a[2])*(b[2]-b[3]); c[5]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[6]; t=co+s +(dchunk)(a[6]-a[0])*(b[0]-b[6])+(dchunk)(a[5]-a[1])*(b[1]-b[5])+(dchunk)(a[4]-a[2])*(b[2]-b[4]); c[6]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[7]; t=co+s +(dchunk)(a[7]-a[0])*(b[0]-b[7])+(dchunk)(a[6]-a[1])*(b[1]-b[6])+(dchunk)(a[5]-a[2])*(b[2]-b[5])+(dchunk)(a[4]-a[3])*(b[3]-b[4]); c[7]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[8]; t=co+s +(dchunk)(a[8]-a[0])*(b[0]-b[8])+(dchunk)(a[7]-a[1])*(b[1]-b[7])+(dchunk)(a[6]-a[2])*(b[2]-b[6])+(dchunk)(a[5]-a[3])*(b[3]-b[5]); c[8]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[9]; t=co+s +(dchunk)(a[9]-a[0])*(b[0]-b[9])+(dchunk)(a[8]-a[1])*(b[1]-b[8])+(dchunk)(a[7]-a[2])*(b[2]-b[7])+(dchunk)(a[6]-a[3])*(b[3]-b[6])+(dchunk)(a[5]-a[4])*(b[4]-b[5]); c[9]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[10]; t=co+s +(dchunk)(a[10]-a[0])*(b[0]-b[10])+(dchunk)(a[9]-a[1])*(b[1]-b[9])+(dchunk)(a[8]-a[2])*(b[2]-b[8])+(dchunk)(a[7]-a[3])*(b[3]-b[7])+(dchunk)(a[6]-a[4])*(b[4]-b[6]); c[10]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[11]; t=co+s +(dchunk)(a[11]-a[0])*(b[0]-b[11])+(dchunk)(a[10]-a[1])*(b[1]-b[10])+(dchunk)(a[9]-a[2])*(b[2]-b[9])+(dchunk)(a[8]-a[3])*(b[3]-b[8])+(dchunk)(a[7]-a[4])*(b[4]-b[7])+(dchunk)(a[6]-a[5])*(b[5]-b[6]); c[11]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[12]; t=co+s +(dchunk)(a[12]-a[0])*(b[0]-b[12])+(dchunk)(a[11]-a[1])*(b[1]-b[11])+(dchunk)(a[10]-a[2])*(b[2]-b[10])+(dchunk)(a[9]-a[3])*(b[3]-b[9])+(dchunk)(a[8]-a[4])*(b[4]-b[8])+(dchunk)(a[7]-a[5])*(b[5]-b[7]); c[12]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[13]; t=co+s +(dchunk)(a[13]-a[0])*(b[0]-b[13])+(dchunk)(a[12]-a[1])*(b[1]-b[12])+(dchunk)(a[11]-a[2])*(b[2]-b[11])+(dchunk)(a[10]-a[3])*(b[3]-b[10])+(dchunk)(a[9]-a[4])*(b[4]-b[9])+(dchunk)(a[8]-a[5])*(b[5]-b[8])+(dchunk)(a[7]-a[6])*(b[6]-b[7]); c[13]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[14]; t=co+s +(dchunk)(a[14]-a[0])*(b[0]-b[14])+(dchunk)(a[13]-a[1])*(b[1]-b[13])+(dchunk)(a[12]-a[2])*(b[2]-b[12])+(dchunk)(a[11]-a[3])*(b[3]-b[11])+(dchunk)(a[10]-a[4])*(b[4]-b[10])+(dchunk)(a[9]-a[5])*(b[5]-b[9])+(dchunk)(a[8]-a[6])*(b[6]-b[8]); c[14]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[15]; t=co+s +(dchunk)(a[15]-a[0])*(b[0]-b[15])+(dchunk)(a[14]-a[1])*(b[1]-b[14])+(dchunk)(a[13]-a[2])*(b[2]-b[13])+(dchunk)(a[12]-a[3])*(b[3]-b[12])+(dchunk)(a[11]-a[4])*(b[4]-b[11])+(dchunk)(a[10]-a[5])*(b[5]-b[10])+(dchunk)(a[9]-a[6])*(b[6]-b[9])+(dchunk)(a[8]-a[7])*(b[7]-b[8]); c[15]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s+=d[16]; t=co+s +(dchunk)(a[16]-a[0])*(b[0]-b[16])+(dchunk)(a[15]-a[1])*(b[1]-b[15])+(dchunk)(a[14]-a[2])*(b[2]-b[14])+(dchunk)(a[13]-a[3])*(b[3]-b[13])+(dchunk)(a[12]-a[4])*(b[4]-b[12])+(dchunk)(a[11]-a[5])*(b[5]-b[11])+(dchunk)(a[10]-a[6])*(b[6]-b[10])+(dchunk)(a[9]-a[7])*(b[7]-b[9]); c[16]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 

	s-=d[0]; t=co+s +(dchunk)(a[16]-a[1])*(b[1]-b[16])+(dchunk)(a[15]-a[2])*(b[2]-b[15])+(dchunk)(a[14]-a[3])*(b[3]-b[14])+(dchunk)(a[13]-a[4])*(b[4]-b[13])+(dchunk)(a[12]-a[5])*(b[5]-b[12])+(dchunk)(a[11]-a[6])*(b[6]-b[11])+(dchunk)(a[10]-a[7])*(b[7]-b[10])+(dchunk)(a[9]-a[8])*(b[8]-b[9]); c[17]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[1]; t=co+s +(dchunk)(a[16]-a[2])*(b[2]-b[16])+(dchunk)(a[15]-a[3])*(b[3]-b[15])+(dchunk)(a[14]-a[4])*(b[4]-b[14])+(dchunk)(a[13]-a[5])*(b[5]-b[13])+(dchunk)(a[12]-a[6])*(b[6]-b[12])+(dchunk)(a[11]-a[7])*(b[7]-b[11])+(dchunk)(a[10]-a[8])*(b[8]-b[10]); c[18]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[2]; t=co+s +(dchunk)(a[16]-a[3])*(b[3]-b[16])+(dchunk)(a[15]-a[4])*(b[4]-b[15])+(dchunk)(a[14]-a[5])*(b[5]-b[14])+(dchunk)(a[13]-a[6])*(b[6]-b[13])+(dchunk)(a[12]-a[7])*(b[7]-b[12])+(dchunk)(a[11]-a[8])*(b[8]-b[11])+(dchunk)(a[10]-a[9])*(b[9]-b[10]); c[19]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[3]; t=co+s +(dchunk)(a[16]-a[4])*(b[4]-b[16])+(dchunk)(a[15]-a[5])*(b[5]-b[15])+(dchunk)(a[14]-a[6])*(b[6]-b[14])+(dchunk)(a[13]-a[7])*(b[7]-b[13])+(dchunk)(a[12]-a[8])*(b[8]-b[12])+(dchunk)(a[11]-a[9])*(b[9]-b[11]); c[20]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[4]; t=co+s +(dchunk)(a[16]-a[5])*(b[5]-b[16])+(dchunk)(a[15]-a[6])*(b[6]-b[15])+(dchunk)(a[14]-a[7])*(b[7]-b[14])+(dchunk)(a[13]-a[8])*(b[8]-b[13])+(dchunk)(a[12]-a[9])*(b[9]-b[12])+(dchunk)(a[11]-a[10])*(b[10]-b[11]); c[21]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[5]; t=co+s +(dchunk)(a[16]-a[6])*(b[6]-b[16])+(dchunk)(a[15]-a[7])*(b[7]-b[15])+(dchunk)(a[14]-a[8])*(b[8]-b[14])+(dchunk)(a[13]-a[9])*(b[9]-b[13])+(dchunk)(a[12]-a[10])*(b[10]-b[12]); c[22]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[6]; t=co+s +(dchunk)(a[16]-a[7])*(b[7]-b[16])+(dchunk)(a[15]-a[8])*(b[8]-b[15])+(dchunk)(a[14]-a[9])*(b[9]-b[14])+(dchunk)(a[13]-a[10])*(b[10]-b[13])+(dchunk)(a[12]-a[11])*(b[11]-b[12]); c[23]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[7]; t=co+s +(dchunk)(a[16]-a[8])*(b[8]-b[16])+(dchunk)(a[15]-a[9])*(b[9]-b[15])+(dchunk)(a[14]-a[10])*(b[10]-b[14])+(dchunk)(a[13]-a[11])*(b[11]-b[13]); c[24]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[8]; t=co+s +(dchunk)(a[16]-a[9])*(b[9]-b[16])+(dchunk)(a[15]-a[10])*(b[10]-b[15])+(dchunk)(a[14]-a[11])*(b[11]-b[14])+(dchunk)(a[13]-a[12])*(b[12]-b[13]); c[25]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[9]; t=co+s +(dchunk)(a[16]-a[10])*(b[10]-b[16])+(dchunk)(a[15]-a[11])*(b[11]-b[15])+(dchunk)(a[14]-a[12])*(b[12]-b[14]); c[26]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[10]; t=co+s +(dchunk)(a[16]-a[11])*(b[11]-b[16])+(dchunk)(a[15]-a[12])*(b[12]-b[15])+(dchunk)(a[14]-a[13])*(b[13]-b[14]); c[27]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[11]; t=co+s +(dchunk)(a[16]-a[12])*(b[12]-b[16])+(dchunk)(a[15]-a[13])*(b[13]-b[15]); c[28]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[12]; t=co+s +(dchunk)(a[16]-a[13])*(b[13]-b[16])+(dchunk)(a[15]-a[14])*(b[14]-b[15]); c[29]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[13]; t=co+s +(dchunk)(a[16]-a[14])*(b[14]-b[16]); c[30]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[14]; t=co+s +(dchunk)(a[16]-a[15])*(b[15]-b[16]); c[31]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	s-=d[15]; t=co+s ; c[32]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	c[33]=(chunk)co;


#else

    	t=(dchunk)a[0]*b[0]; c[0]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[1]+(dchunk)a[1]*b[0]; c[1]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[2]+(dchunk)a[1]*b[1]+(dchunk)a[2]*b[0]; c[2]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[3]+(dchunk)a[1]*b[2]+(dchunk)a[2]*b[1]+(dchunk)a[3]*b[0]; c[3]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[4]+(dchunk)a[1]*b[3]+(dchunk)a[2]*b[2]+(dchunk)a[3]*b[1]+(dchunk)a[4]*b[0]; c[4]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[5]+(dchunk)a[1]*b[4]+(dchunk)a[2]*b[3]+(dchunk)a[3]*b[2]+(dchunk)a[4]*b[1]+(dchunk)a[5]*b[0]; c[5]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[6]+(dchunk)a[1]*b[5]+(dchunk)a[2]*b[4]+(dchunk)a[3]*b[3]+(dchunk)a[4]*b[2]+(dchunk)a[5]*b[1]+(dchunk)a[6]*b[0]; c[6]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[7]+(dchunk)a[1]*b[6]+(dchunk)a[2]*b[5]+(dchunk)a[3]*b[4]+(dchunk)a[4]*b[3]+(dchunk)a[5]*b[2]+(dchunk)a[6]*b[1]+(dchunk)a[7]*b[0]; c[7]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[8]+(dchunk)a[1]*b[7]+(dchunk)a[2]*b[6]+(dchunk)a[3]*b[5]+(dchunk)a[4]*b[4]+(dchunk)a[5]*b[3]+(dchunk)a[6]*b[2]+(dchunk)a[7]*b[1]+(dchunk)a[8]*b[0]; c[8]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[9]+(dchunk)a[1]*b[8]+(dchunk)a[2]*b[7]+(dchunk)a[3]*b[6]+(dchunk)a[4]*b[5]+(dchunk)a[5]*b[4]+(dchunk)a[6]*b[3]+(dchunk)a[7]*b[2]+(dchunk)a[8]*b[1]+(dchunk)a[9]*b[0]; c[9]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[10]+(dchunk)a[1]*b[9]+(dchunk)a[2]*b[8]+(dchunk)a[3]*b[7]+(dchunk)a[4]*b[6]+(dchunk)a[5]*b[5]+(dchunk)a[6]*b[4]+(dchunk)a[7]*b[3]+(dchunk)a[8]*b[2]+(dchunk)a[9]*b[1]+(dchunk)a[10]*b[0]; c[10]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[11]+(dchunk)a[1]*b[10]+(dchunk)a[2]*b[9]+(dchunk)a[3]*b[8]+(dchunk)a[4]*b[7]+(dchunk)a[5]*b[6]+(dchunk)a[6]*b[5]+(dchunk)a[7]*b[4]+(dchunk)a[8]*b[3]+(dchunk)a[9]*b[2]+(dchunk)a[10]*b[1]+(dchunk)a[11]*b[0]; c[11]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[12]+(dchunk)a[1]*b[11]+(dchunk)a[2]*b[10]+(dchunk)a[3]*b[9]+(dchunk)a[4]*b[8]+(dchunk)a[5]*b[7]+(dchunk)a[6]*b[6]+(dchunk)a[7]*b[5]+(dchunk)a[8]*b[4]+(dchunk)a[9]*b[3]+(dchunk)a[10]*b[2]+(dchunk)a[11]*b[1]+(dchunk)a[12]*b[0]; c[12]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[13]+(dchunk)a[1]*b[12]+(dchunk)a[2]*b[11]+(dchunk)a[3]*b[10]+(dchunk)a[4]*b[9]+(dchunk)a[5]*b[8]+(dchunk)a[6]*b[7]+(dchunk)a[7]*b[6]+(dchunk)a[8]*b[5]+(dchunk)a[9]*b[4]+(dchunk)a[10]*b[3]+(dchunk)a[11]*b[2]+(dchunk)a[12]*b[1]+(dchunk)a[13]*b[0]; c[13]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[14]+(dchunk)a[1]*b[13]+(dchunk)a[2]*b[12]+(dchunk)a[3]*b[11]+(dchunk)a[4]*b[10]+(dchunk)a[5]*b[9]+(dchunk)a[6]*b[8]+(dchunk)a[7]*b[7]+(dchunk)a[8]*b[6]+(dchunk)a[9]*b[5]+(dchunk)a[10]*b[4]+(dchunk)a[11]*b[3]+(dchunk)a[12]*b[2]+(dchunk)a[13]*b[1]+(dchunk)a[14]*b[0]; c[14]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[15]+(dchunk)a[1]*b[14]+(dchunk)a[2]*b[13]+(dchunk)a[3]*b[12]+(dchunk)a[4]*b[11]+(dchunk)a[5]*b[10]+(dchunk)a[6]*b[9]+(dchunk)a[7]*b[8]+(dchunk)a[8]*b[7]+(dchunk)a[9]*b[6]+(dchunk)a[10]*b[5]+(dchunk)a[11]*b[4]+(dchunk)a[12]*b[3]+(dchunk)a[13]*b[2]+(dchunk)a[14]*b[1]+(dchunk)a[15]*b[0]; c[15]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[0]*b[16]+(dchunk)a[1]*b[15]+(dchunk)a[2]*b[14]+(dchunk)a[3]*b[13]+(dchunk)a[4]*b[12]+(dchunk)a[5]*b[11]+(dchunk)a[6]*b[10]+(dchunk)a[7]*b[9]+(dchunk)a[8]*b[8]+(dchunk)a[9]*b[7]+(dchunk)a[10]*b[6]+(dchunk)a[11]*b[5]+(dchunk)a[12]*b[4]+(dchunk)a[13]*b[3]+(dchunk)a[14]*b[2]+(dchunk)a[15]*b[1]+(dchunk)a[16]*b[0]; c[16]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[1]*b[16]+(dchunk)a[2]*b[15]+(dchunk)a[3]*b[14]+(dchunk)a[4]*b[13]+(dchunk)a[5]*b[12]+(dchunk)a[6]*b[11]+(dchunk)a[7]*b[10]+(dchunk)a[8]*b[9]+(dchunk)a[9]*b[8]+(dchunk)a[10]*b[7]+(dchunk)a[11]*b[6]+(dchunk)a[12]*b[5]+(dchunk)a[13]*b[4]+(dchunk)a[14]*b[3]+(dchunk)a[15]*b[2]+(dchunk)a[16]*b[1]; c[17]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[2]*b[16]+(dchunk)a[3]*b[15]+(dchunk)a[4]*b[14]+(dchunk)a[5]*b[13]+(dchunk)a[6]*b[12]+(dchunk)a[7]*b[11]+(dchunk)a[8]*b[10]+(dchunk)a[9]*b[9]+(dchunk)a[10]*b[8]+(dchunk)a[11]*b[7]+(dchunk)a[12]*b[6]+(dchunk)a[13]*b[5]+(dchunk)a[14]*b[4]+(dchunk)a[15]*b[3]+(dchunk)a[16]*b[2]; c[18]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[3]*b[16]+(dchunk)a[4]*b[15]+(dchunk)a[5]*b[14]+(dchunk)a[6]*b[13]+(dchunk)a[7]*b[12]+(dchunk)a[8]*b[11]+(dchunk)a[9]*b[10]+(dchunk)a[10]*b[9]+(dchunk)a[11]*b[8]+(dchunk)a[12]*b[7]+(dchunk)a[13]*b[6]+(dchunk)a[14]*b[5]+(dchunk)a[15]*b[4]+(dchunk)a[16]*b[3]; c[19]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[4]*b[16]+(dchunk)a[5]*b[15]+(dchunk)a[6]*b[14]+(dchunk)a[7]*b[13]+(dchunk)a[8]*b[12]+(dchunk)a[9]*b[11]+(dchunk)a[10]*b[10]+(dchunk)a[11]*b[9]+(dchunk)a[12]*b[8]+(dchunk)a[13]*b[7]+(dchunk)a[14]*b[6]+(dchunk)a[15]*b[5]+(dchunk)a[16]*b[4]; c[20]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[5]*b[16]+(dchunk)a[6]*b[15]+(dchunk)a[7]*b[14]+(dchunk)a[8]*b[13]+(dchunk)a[9]*b[12]+(dchunk)a[10]*b[11]+(dchunk)a[11]*b[10]+(dchunk)a[12]*b[9]+(dchunk)a[13]*b[8]+(dchunk)a[14]*b[7]+(dchunk)a[15]*b[6]+(dchunk)a[16]*b[5]; c[21]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[6]*b[16]+(dchunk)a[7]*b[15]+(dchunk)a[8]*b[14]+(dchunk)a[9]*b[13]+(dchunk)a[10]*b[12]+(dchunk)a[11]*b[11]+(dchunk)a[12]*b[10]+(dchunk)a[13]*b[9]+(dchunk)a[14]*b[8]+(dchunk)a[15]*b[7]+(dchunk)a[16]*b[6]; c[22]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[7]*b[16]+(dchunk)a[8]*b[15]+(dchunk)a[9]*b[14]+(dchunk)a[10]*b[13]+(dchunk)a[11]*b[12]+(dchunk)a[12]*b[11]+(dchunk)a[13]*b[10]+(dchunk)a[14]*b[9]+(dchunk)a[15]*b[8]+(dchunk)a[16]*b[7]; c[23]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[8]*b[16]+(dchunk)a[9]*b[15]+(dchunk)a[10]*b[14]+(dchunk)a[11]*b[13]+(dchunk)a[12]*b[12]+(dchunk)a[13]*b[11]+(dchunk)a[14]*b[10]+(dchunk)a[15]*b[9]+(dchunk)a[16]*b[8]; c[24]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[9]*b[16]+(dchunk)a[10]*b[15]+(dchunk)a[11]*b[14]+(dchunk)a[12]*b[13]+(dchunk)a[13]*b[12]+(dchunk)a[14]*b[11]+(dchunk)a[15]*b[10]+(dchunk)a[16]*b[9]; c[25]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[10]*b[16]+(dchunk)a[11]*b[15]+(dchunk)a[12]*b[14]+(dchunk)a[13]*b[13]+(dchunk)a[14]*b[12]+(dchunk)a[15]*b[11]+(dchunk)a[16]*b[10]; c[26]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[11]*b[16]+(dchunk)a[12]*b[15]+(dchunk)a[13]*b[14]+(dchunk)a[14]*b[13]+(dchunk)a[15]*b[12]+(dchunk)a[16]*b[11]; c[27]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[12]*b[16]+(dchunk)a[13]*b[15]+(dchunk)a[14]*b[14]+(dchunk)a[15]*b[13]+(dchunk)a[16]*b[12]; c[28]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[13]*b[16]+(dchunk)a[14]*b[15]+(dchunk)a[15]*b[14]+(dchunk)a[16]*b[13]; c[29]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[14]*b[16]+(dchunk)a[15]*b[15]+(dchunk)a[16]*b[14]; c[30]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[15]*b[16]+(dchunk)a[16]*b[15]; c[31]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	t=t+(dchunk)a[16]*b[16]; c[32]=(chunk)t & BMASK_464_28; t=t>>BASEBITS_464_28;
	c[33]=(chunk)t;


#endif


#else

#ifndef USE_KARATSUBA

    t=(dchunk)a[0]*b[0];
    c[0]=(chunk)t & BMASK_464_28;
    t = t >> BASEBITS_464_28;
    for (i=1;i<NLEN_464_28;i++)
    {
        k=0; 
        while (k<=i) {t+=(dchunk)a[k]*b[i-k]; k++;}
        c[i]=(chunk)t & BMASK_464_28;
        t = t >> BASEBITS_464_28;
    }

    for (i=NLEN_464_28;i<2*NLEN_464_28-1;i++)
    {
        k=i-(NLEN_464_28-1);
        while (k<=NLEN_464_28-1) {t+=(dchunk)a[k]*b[i-k]; k++;}
        c[i]=(chunk)t & BMASK_464_28;
        t = t >> BASEBITS_464_28;
    }

    c[2 * NLEN_464_28 - 1] = (chunk)t;
#else

    for (i = 0; i < NLEN_464_28; i++)
        d[i] = (dchunk)a[i] * b[i];

    s = d[0];
    t = s;
    c[0] = (chunk)t & BMASK_464_28;
    co = t >> BASEBITS_464_28;

    for (k = 1; k < NLEN_464_28; k++)
    {
        s += d[k];
        t = co + s;
        
        /*for (i = k; i >= 1 + k / 2; i--) This causes a huge slow down! gcc/g++ optimizer problem (I think) */
        for (i=1+k/2;i<=k;i++) t += (dchunk)(a[i] - a[k - i]) * (b[k - i] - b[i]);
        c[k] = (chunk)t & BMASK_464_28;
        co = t >> BASEBITS_464_28;
    }
    for (k = NLEN_464_28; k < 2 * NLEN_464_28 - 1; k++)
    {
        s -= d[k - NLEN_464_28];
        t = co + s;
        for (i=1+k/2;i<NLEN_464_28;i++) t += (dchunk)(a[i] - a[k - i]) * (b[k - i] - b[i]);
        c[k] = (chunk)t & BMASK_464_28;
        co = t >> BASEBITS_464_28;
    }
    c[2 * NLEN_464_28 - 1] = (chunk)co;
#endif
#endif

#else
    int j;
    chunk carry;
    BIG_464_28_dzero(c);
    for (i = 0; i < NLEN_464_28; i++)
    {
        carry = 0;
        for (j = 0; j < NLEN_464_28; j++)
            carry = muladd_464_28(a[i], b[j], carry, &c[i + j]);

        c[NLEN_464_28 + i] = carry;
    }

#endif

#ifdef DEBUG_NORM
    c[DMPV_464_28] = 1;
    c[DMNV_464_28] = 0;
#endif
}

/* Set c=a*a */
/* SU= 80 */

//void BIG_464_28_sqr(chunk c[restrict DNLEN_464_28],chunk a[restrict NLEN_464_28])
void BIG_464_28_sqr(DBIG_464_28 c, BIG_464_28 a)
{
    int i, j;
#ifdef dchunk
    dchunk t, co;
#endif

#ifdef DEBUG_NORM
    if ((a[MPV_464_28] != 1 && a[MPV_464_28] != 0) || a[MNV_464_28] != 0) printf("Input to sqr not normed\n");
#endif
    /* Note 2*a[i] in loop below and extra addition */

#ifdef COMBA

#ifdef UNWOUND

    
	t=(dchunk)a[0]*a[0]; c[0]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28;
	t= +(dchunk)a[1]*a[0]; t+=t; t+=co; c[1]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[2]*a[0]; t+=t; t+=co; t+=(dchunk)a[1]*a[1]; c[2]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[3]*a[0]+(dchunk)a[2]*a[1]; t+=t; t+=co; c[3]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[4]*a[0]+(dchunk)a[3]*a[1]; t+=t; t+=co; t+=(dchunk)a[2]*a[2]; c[4]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[5]*a[0]+(dchunk)a[4]*a[1]+(dchunk)a[3]*a[2]; t+=t; t+=co; c[5]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[6]*a[0]+(dchunk)a[5]*a[1]+(dchunk)a[4]*a[2]; t+=t; t+=co; t+=(dchunk)a[3]*a[3]; c[6]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[7]*a[0]+(dchunk)a[6]*a[1]+(dchunk)a[5]*a[2]+(dchunk)a[4]*a[3]; t+=t; t+=co; c[7]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[8]*a[0]+(dchunk)a[7]*a[1]+(dchunk)a[6]*a[2]+(dchunk)a[5]*a[3]; t+=t; t+=co; t+=(dchunk)a[4]*a[4]; c[8]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[9]*a[0]+(dchunk)a[8]*a[1]+(dchunk)a[7]*a[2]+(dchunk)a[6]*a[3]+(dchunk)a[5]*a[4]; t+=t; t+=co; c[9]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[10]*a[0]+(dchunk)a[9]*a[1]+(dchunk)a[8]*a[2]+(dchunk)a[7]*a[3]+(dchunk)a[6]*a[4]; t+=t; t+=co; t+=(dchunk)a[5]*a[5]; c[10]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[11]*a[0]+(dchunk)a[10]*a[1]+(dchunk)a[9]*a[2]+(dchunk)a[8]*a[3]+(dchunk)a[7]*a[4]+(dchunk)a[6]*a[5]; t+=t; t+=co; c[11]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[12]*a[0]+(dchunk)a[11]*a[1]+(dchunk)a[10]*a[2]+(dchunk)a[9]*a[3]+(dchunk)a[8]*a[4]+(dchunk)a[7]*a[5]; t+=t; t+=co; t+=(dchunk)a[6]*a[6]; c[12]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[13]*a[0]+(dchunk)a[12]*a[1]+(dchunk)a[11]*a[2]+(dchunk)a[10]*a[3]+(dchunk)a[9]*a[4]+(dchunk)a[8]*a[5]+(dchunk)a[7]*a[6]; t+=t; t+=co; c[13]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[14]*a[0]+(dchunk)a[13]*a[1]+(dchunk)a[12]*a[2]+(dchunk)a[11]*a[3]+(dchunk)a[10]*a[4]+(dchunk)a[9]*a[5]+(dchunk)a[8]*a[6]; t+=t; t+=co; t+=(dchunk)a[7]*a[7]; c[14]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[15]*a[0]+(dchunk)a[14]*a[1]+(dchunk)a[13]*a[2]+(dchunk)a[12]*a[3]+(dchunk)a[11]*a[4]+(dchunk)a[10]*a[5]+(dchunk)a[9]*a[6]+(dchunk)a[8]*a[7]; t+=t; t+=co; c[15]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[0]+(dchunk)a[15]*a[1]+(dchunk)a[14]*a[2]+(dchunk)a[13]*a[3]+(dchunk)a[12]*a[4]+(dchunk)a[11]*a[5]+(dchunk)a[10]*a[6]+(dchunk)a[9]*a[7]; t+=t; t+=co; t+=(dchunk)a[8]*a[8]; c[16]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 

	t= +(dchunk)a[16]*a[1]+(dchunk)a[15]*a[2]+(dchunk)a[14]*a[3]+(dchunk)a[13]*a[4]+(dchunk)a[12]*a[5]+(dchunk)a[11]*a[6]+(dchunk)a[10]*a[7]+(dchunk)a[9]*a[8]; t+=t; t+=co; c[17]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[2]+(dchunk)a[15]*a[3]+(dchunk)a[14]*a[4]+(dchunk)a[13]*a[5]+(dchunk)a[12]*a[6]+(dchunk)a[11]*a[7]+(dchunk)a[10]*a[8]; t+=t; t+=co; t+=(dchunk)a[9]*a[9]; c[18]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[3]+(dchunk)a[15]*a[4]+(dchunk)a[14]*a[5]+(dchunk)a[13]*a[6]+(dchunk)a[12]*a[7]+(dchunk)a[11]*a[8]+(dchunk)a[10]*a[9]; t+=t; t+=co; c[19]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[4]+(dchunk)a[15]*a[5]+(dchunk)a[14]*a[6]+(dchunk)a[13]*a[7]+(dchunk)a[12]*a[8]+(dchunk)a[11]*a[9]; t+=t; t+=co; t+=(dchunk)a[10]*a[10]; c[20]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[5]+(dchunk)a[15]*a[6]+(dchunk)a[14]*a[7]+(dchunk)a[13]*a[8]+(dchunk)a[12]*a[9]+(dchunk)a[11]*a[10]; t+=t; t+=co; c[21]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[6]+(dchunk)a[15]*a[7]+(dchunk)a[14]*a[8]+(dchunk)a[13]*a[9]+(dchunk)a[12]*a[10]; t+=t; t+=co; t+=(dchunk)a[11]*a[11]; c[22]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[7]+(dchunk)a[15]*a[8]+(dchunk)a[14]*a[9]+(dchunk)a[13]*a[10]+(dchunk)a[12]*a[11]; t+=t; t+=co; c[23]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[8]+(dchunk)a[15]*a[9]+(dchunk)a[14]*a[10]+(dchunk)a[13]*a[11]; t+=t; t+=co; t+=(dchunk)a[12]*a[12]; c[24]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[9]+(dchunk)a[15]*a[10]+(dchunk)a[14]*a[11]+(dchunk)a[13]*a[12]; t+=t; t+=co; c[25]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[10]+(dchunk)a[15]*a[11]+(dchunk)a[14]*a[12]; t+=t; t+=co; t+=(dchunk)a[13]*a[13]; c[26]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[11]+(dchunk)a[15]*a[12]+(dchunk)a[14]*a[13]; t+=t; t+=co; c[27]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[12]+(dchunk)a[15]*a[13]; t+=t; t+=co; t+=(dchunk)a[14]*a[14]; c[28]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[13]+(dchunk)a[15]*a[14]; t+=t; t+=co; c[29]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[14]; t+=t; t+=co; t+=(dchunk)a[15]*a[15]; c[30]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t= +(dchunk)a[16]*a[15]; t+=t; t+=co; c[31]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
	t=co; t+=(dchunk)a[16]*a[16]; c[32]=(chunk)t&BMASK_464_28; co=t>>BASEBITS_464_28; 
 	c[33]=(chunk)co;


#else


    t = (dchunk)a[0] * a[0];
    c[0] = (chunk)t & BMASK_464_28;
    co = t >> BASEBITS_464_28;

    for (j = 1; j < NLEN_464_28 - 1; )
    {
        t = (dchunk)a[j] * a[0];
        for (i = 1; i < (j + 1) / 2; i++)
        {
            t += (dchunk)a[j - i] * a[i];
        }
        t += t;
        t += co;
        c[j] = (chunk)t & BMASK_464_28;
        co = t >> BASEBITS_464_28;
        j++;
        t = (dchunk)a[j] * a[0];
        for (i = 1; i < (j + 1) / 2; i++)
        {
            t += (dchunk)a[j - i] * a[i];
        }
        t += t;
        t += co;
        t += (dchunk)a[j / 2] * a[j / 2];
        c[j] = (chunk)t & BMASK_464_28;
        co = t >> BASEBITS_464_28;
        j++;
    }

    for (j = NLEN_464_28 - 1 + NLEN_464_28 % 2; j < DNLEN_464_28 - 3; )
    {
        t = (dchunk)a[NLEN_464_28 - 1] * a[j - NLEN_464_28 + 1];
        for (i = j - NLEN_464_28 + 2; i < (j + 1) / 2; i++)
        {
            t += (dchunk)a[j - i] * a[i];
        }
        t += t;
        t += co;
        c[j] = (chunk)t & BMASK_464_28;
        co = t >> BASEBITS_464_28;
        j++;
        t = (dchunk)a[NLEN_464_28 - 1] * a[j - NLEN_464_28 + 1];
        for (i = j - NLEN_464_28 + 2; i < (j + 1) / 2; i++)
        {
            t += (dchunk)a[j - i] * a[i];
        }
        t += t;
        t += co;
        t += (dchunk)a[j / 2] * a[j / 2];
        c[j] = (chunk)t & BMASK_464_28;
        co = t >> BASEBITS_464_28;
        j++;
    }

    t = (dchunk)a[NLEN_464_28 - 2] * a[NLEN_464_28 - 1];
    t += t;
    t += co;
    c[DNLEN_464_28 - 3] = (chunk)t & BMASK_464_28;
    co = t >> BASEBITS_464_28;

    t = (dchunk)a[NLEN_464_28 - 1] * a[NLEN_464_28 - 1] + co;
    c[DNLEN_464_28 - 2] = (chunk)t & BMASK_464_28;
    co = t >> BASEBITS_464_28;
    c[DNLEN_464_28 - 1] = (chunk)co;


#endif

#else
    chunk carry;
    BIG_464_28_dzero(c);
    for (i = 0; i < NLEN_464_28; i++)
    {
        carry = 0;
        for (j = i + 1; j < NLEN_464_28; j++)
            carry = muladd_464_28(a[i], a[j], carry, &c[i + j]);
        c[NLEN_464_28 + i] = carry;
    }

    for (i = 0; i < DNLEN_464_28; i++) c[i] *= 2;

    for (i = 0; i < NLEN_464_28; i++)
        c[2 * i + 1] += muladd_464_28(a[i], a[i], 0, &c[2 * i]);

    BIG_464_28_dnorm(c);
#endif


#ifdef DEBUG_NORM
    c[DMPV_464_28] = 1;
    c[DMNV_464_28] = 0;
#endif

}

/* Montgomery reduction */
//void BIG_464_28_monty(chunk a[restrict NLEN_464_28], chunk md[restrict NLEN_464_28], chunk MC, chunk d[restrict DNLEN_464_28])
void BIG_464_28_monty(BIG_464_28 a, BIG_464_28 md, chunk MC, DBIG_464_28 d)
{
    int i, k;

#ifdef dchunk
    dchunk t, c, s;
    dchunk dd[NLEN_464_28];
    chunk v[NLEN_464_28];
#endif

#ifdef DEBUG_NORM
    if ((d[DMPV_464_28] != 1 && d[DMPV_464_28] != 0) || d[DMNV_464_28] != 0) printf("Input to redc not normed\n");
#endif

#ifdef COMBA

#ifdef UNWOUND

#ifdef USE_KARATSUBA

    	t=d[0]; v[0]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[0]*md[0];  s=0; c=(t>>BASEBITS_464_28);

	t=d[1]+c+s+(dchunk)v[0]*md[1]; v[1]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[1]*md[0];  dd[1]=(dchunk)v[1]*md[1]; s+=dd[1]; c=(t>>BASEBITS_464_28); 
	t=d[2]+c+s+(dchunk)v[0]*md[2]; v[2]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[2]*md[0];  dd[2]=(dchunk)v[2]*md[2]; s+=dd[2]; c=(t>>BASEBITS_464_28); 
	t=d[3]+c+s+(dchunk)v[0]*md[3]+(dchunk)(v[1]-v[2])*(md[2]-md[1]); v[3]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[3]*md[0];  dd[3]=(dchunk)v[3]*md[3]; s+=dd[3]; c=(t>>BASEBITS_464_28); 
	t=d[4]+c+s+(dchunk)v[0]*md[4]+(dchunk)(v[1]-v[3])*(md[3]-md[1]); v[4]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[4]*md[0];  dd[4]=(dchunk)v[4]*md[4]; s+=dd[4]; c=(t>>BASEBITS_464_28); 
	t=d[5]+c+s+(dchunk)v[0]*md[5]+(dchunk)(v[1]-v[4])*(md[4]-md[1])+(dchunk)(v[2]-v[3])*(md[3]-md[2]); v[5]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[5]*md[0];  dd[5]=(dchunk)v[5]*md[5]; s+=dd[5]; c=(t>>BASEBITS_464_28); 
	t=d[6]+c+s+(dchunk)v[0]*md[6]+(dchunk)(v[1]-v[5])*(md[5]-md[1])+(dchunk)(v[2]-v[4])*(md[4]-md[2]); v[6]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[6]*md[0];  dd[6]=(dchunk)v[6]*md[6]; s+=dd[6]; c=(t>>BASEBITS_464_28); 
	t=d[7]+c+s+(dchunk)v[0]*md[7]+(dchunk)(v[1]-v[6])*(md[6]-md[1])+(dchunk)(v[2]-v[5])*(md[5]-md[2])+(dchunk)(v[3]-v[4])*(md[4]-md[3]); v[7]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[7]*md[0];  dd[7]=(dchunk)v[7]*md[7]; s+=dd[7]; c=(t>>BASEBITS_464_28); 
	t=d[8]+c+s+(dchunk)v[0]*md[8]+(dchunk)(v[1]-v[7])*(md[7]-md[1])+(dchunk)(v[2]-v[6])*(md[6]-md[2])+(dchunk)(v[3]-v[5])*(md[5]-md[3]); v[8]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[8]*md[0];  dd[8]=(dchunk)v[8]*md[8]; s+=dd[8]; c=(t>>BASEBITS_464_28); 
	t=d[9]+c+s+(dchunk)v[0]*md[9]+(dchunk)(v[1]-v[8])*(md[8]-md[1])+(dchunk)(v[2]-v[7])*(md[7]-md[2])+(dchunk)(v[3]-v[6])*(md[6]-md[3])+(dchunk)(v[4]-v[5])*(md[5]-md[4]); v[9]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[9]*md[0];  dd[9]=(dchunk)v[9]*md[9]; s+=dd[9]; c=(t>>BASEBITS_464_28); 
	t=d[10]+c+s+(dchunk)v[0]*md[10]+(dchunk)(v[1]-v[9])*(md[9]-md[1])+(dchunk)(v[2]-v[8])*(md[8]-md[2])+(dchunk)(v[3]-v[7])*(md[7]-md[3])+(dchunk)(v[4]-v[6])*(md[6]-md[4]); v[10]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[10]*md[0];  dd[10]=(dchunk)v[10]*md[10]; s+=dd[10]; c=(t>>BASEBITS_464_28); 
	t=d[11]+c+s+(dchunk)v[0]*md[11]+(dchunk)(v[1]-v[10])*(md[10]-md[1])+(dchunk)(v[2]-v[9])*(md[9]-md[2])+(dchunk)(v[3]-v[8])*(md[8]-md[3])+(dchunk)(v[4]-v[7])*(md[7]-md[4])+(dchunk)(v[5]-v[6])*(md[6]-md[5]); v[11]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[11]*md[0];  dd[11]=(dchunk)v[11]*md[11]; s+=dd[11]; c=(t>>BASEBITS_464_28); 
	t=d[12]+c+s+(dchunk)v[0]*md[12]+(dchunk)(v[1]-v[11])*(md[11]-md[1])+(dchunk)(v[2]-v[10])*(md[10]-md[2])+(dchunk)(v[3]-v[9])*(md[9]-md[3])+(dchunk)(v[4]-v[8])*(md[8]-md[4])+(dchunk)(v[5]-v[7])*(md[7]-md[5]); v[12]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[12]*md[0];  dd[12]=(dchunk)v[12]*md[12]; s+=dd[12]; c=(t>>BASEBITS_464_28); 
	t=d[13]+c+s+(dchunk)v[0]*md[13]+(dchunk)(v[1]-v[12])*(md[12]-md[1])+(dchunk)(v[2]-v[11])*(md[11]-md[2])+(dchunk)(v[3]-v[10])*(md[10]-md[3])+(dchunk)(v[4]-v[9])*(md[9]-md[4])+(dchunk)(v[5]-v[8])*(md[8]-md[5])+(dchunk)(v[6]-v[7])*(md[7]-md[6]); v[13]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[13]*md[0];  dd[13]=(dchunk)v[13]*md[13]; s+=dd[13]; c=(t>>BASEBITS_464_28); 
	t=d[14]+c+s+(dchunk)v[0]*md[14]+(dchunk)(v[1]-v[13])*(md[13]-md[1])+(dchunk)(v[2]-v[12])*(md[12]-md[2])+(dchunk)(v[3]-v[11])*(md[11]-md[3])+(dchunk)(v[4]-v[10])*(md[10]-md[4])+(dchunk)(v[5]-v[9])*(md[9]-md[5])+(dchunk)(v[6]-v[8])*(md[8]-md[6]); v[14]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[14]*md[0];  dd[14]=(dchunk)v[14]*md[14]; s+=dd[14]; c=(t>>BASEBITS_464_28); 
	t=d[15]+c+s+(dchunk)v[0]*md[15]+(dchunk)(v[1]-v[14])*(md[14]-md[1])+(dchunk)(v[2]-v[13])*(md[13]-md[2])+(dchunk)(v[3]-v[12])*(md[12]-md[3])+(dchunk)(v[4]-v[11])*(md[11]-md[4])+(dchunk)(v[5]-v[10])*(md[10]-md[5])+(dchunk)(v[6]-v[9])*(md[9]-md[6])+(dchunk)(v[7]-v[8])*(md[8]-md[7]); v[15]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[15]*md[0];  dd[15]=(dchunk)v[15]*md[15]; s+=dd[15]; c=(t>>BASEBITS_464_28); 
	t=d[16]+c+s+(dchunk)v[0]*md[16]+(dchunk)(v[1]-v[15])*(md[15]-md[1])+(dchunk)(v[2]-v[14])*(md[14]-md[2])+(dchunk)(v[3]-v[13])*(md[13]-md[3])+(dchunk)(v[4]-v[12])*(md[12]-md[4])+(dchunk)(v[5]-v[11])*(md[11]-md[5])+(dchunk)(v[6]-v[10])*(md[10]-md[6])+(dchunk)(v[7]-v[9])*(md[9]-md[7]); v[16]=((chunk)t*MC)&BMASK_464_28; t+=(dchunk)v[16]*md[0];  dd[16]=(dchunk)v[16]*md[16]; s+=dd[16]; c=(t>>BASEBITS_464_28); 

	t=d[17]+c+s+(dchunk)(v[1]-v[16])*(md[16]-md[1])+(dchunk)(v[2]-v[15])*(md[15]-md[2])+(dchunk)(v[3]-v[14])*(md[14]-md[3])+(dchunk)(v[4]-v[13])*(md[13]-md[4])+(dchunk)(v[5]-v[12])*(md[12]-md[5])+(dchunk)(v[6]-v[11])*(md[11]-md[6])+(dchunk)(v[7]-v[10])*(md[10]-md[7])+(dchunk)(v[8]-v[9])*(md[9]-md[8]); a[0]=(chunk)t&BMASK_464_28;  s-=dd[1]; c=(t>>BASEBITS_464_28); 
	t=d[18]+c+s+(dchunk)(v[2]-v[16])*(md[16]-md[2])+(dchunk)(v[3]-v[15])*(md[15]-md[3])+(dchunk)(v[4]-v[14])*(md[14]-md[4])+(dchunk)(v[5]-v[13])*(md[13]-md[5])+(dchunk)(v[6]-v[12])*(md[12]-md[6])+(dchunk)(v[7]-v[11])*(md[11]-md[7])+(dchunk)(v[8]-v[10])*(md[10]-md[8]); a[1]=(chunk)t&BMASK_464_28;  s-=dd[2]; c=(t>>BASEBITS_464_28); 
	t=d[19]+c+s+(dchunk)(v[3]-v[16])*(md[16]-md[3])+(dchunk)(v[4]-v[15])*(md[15]-md[4])+(dchunk)(v[5]-v[14])*(md[14]-md[5])+(dchunk)(v[6]-v[13])*(md[13]-md[6])+(dchunk)(v[7]-v[12])*(md[12]-md[7])+(dchunk)(v[8]-v[11])*(md[11]-md[8])+(dchunk)(v[9]-v[10])*(md[10]-md[9]); a[2]=(chunk)t&BMASK_464_28;  s-=dd[3]; c=(t>>BASEBITS_464_28); 
	t=d[20]+c+s+(dchunk)(v[4]-v[16])*(md[16]-md[4])+(dchunk)(v[5]-v[15])*(md[15]-md[5])+(dchunk)(v[6]-v[14])*(md[14]-md[6])+(dchunk)(v[7]-v[13])*(md[13]-md[7])+(dchunk)(v[8]-v[12])*(md[12]-md[8])+(dchunk)(v[9]-v[11])*(md[11]-md[9]); a[3]=(chunk)t&BMASK_464_28;  s-=dd[4]; c=(t>>BASEBITS_464_28); 
	t=d[21]+c+s+(dchunk)(v[5]-v[16])*(md[16]-md[5])+(dchunk)(v[6]-v[15])*(md[15]-md[6])+(dchunk)(v[7]-v[14])*(md[14]-md[7])+(dchunk)(v[8]-v[13])*(md[13]-md[8])+(dchunk)(v[9]-v[12])*(md[12]-md[9])+(dchunk)(v[10]-v[11])*(md[11]-md[10]); a[4]=(chunk)t&BMASK_464_28;  s-=dd[5]; c=(t>>BASEBITS_464_28); 
	t=d[22]+c+s+(dchunk)(v[6]-v[16])*(md[16]-md[6])+(dchunk)(v[7]-v[15])*(md[15]-md[7])+(dchunk)(v[8]-v[14])*(md[14]-md[8])+(dchunk)(v[9]-v[13])*(md[13]-md[9])+(dchunk)(v[10]-v[12])*(md[12]-md[10]); a[5]=(chunk)t&BMASK_464_28;  s-=dd[6]; c=(t>>BASEBITS_464_28); 
	t=d[23]+c+s+(dchunk)(v[7]-v[16])*(md[16]-md[7])+(dchunk)(v[8]-v[15])*(md[15]-md[8])+(dchunk)(v[9]-v[14])*(md[14]-md[9])+(dchunk)(v[10]-v[13])*(md[13]-md[10])+(dchunk)(v[11]-v[12])*(md[12]-md[11]); a[6]=(chunk)t&BMASK_464_28;  s-=dd[7]; c=(t>>BASEBITS_464_28); 
	t=d[24]+c+s+(dchunk)(v[8]-v[16])*(md[16]-md[8])+(dchunk)(v[9]-v[15])*(md[15]-md[9])+(dchunk)(v[10]-v[14])*(md[14]-md[10])+(dchunk)(v[11]-v[13])*(md[13]-md[11]); a[7]=(chunk)t&BMASK_464_28;  s-=dd[8]; c=(t>>BASEBITS_464_28); 
	t=d[25]+c+s+(dchunk)(v[9]-v[16])*(md[16]-md[9])+(dchunk)(v[10]-v[15])*(md[15]-md[10])+(dchunk)(v[11]-v[14])*(md[14]-md[11])+(dchunk)(v[12]-v[13])*(md[13]-md[12]); a[8]=(chunk)t&BMASK_464_28;  s-=dd[9]; c=(t>>BASEBITS_464_28); 
	t=d[26]+c+s+(dchunk)(v[10]-v[16])*(md[16]-md[10])+(dchunk)(v[11]-v[15])*(md[15]-md[11])+(dchunk)(v[12]-v[14])*(md[14]-md[12]); a[9]=(chunk)t&BMASK_464_28;  s-=dd[10]; c=(t>>BASEBITS_464_28); 
	t=d[27]+c+s+(dchunk)(v[11]-v[16])*(md[16]-md[11])+(dchunk)(v[12]-v[15])*(md[15]-md[12])+(dchunk)(v[13]-v[14])*(md[14]-md[13]); a[10]=(chunk)t&BMASK_464_28;  s-=dd[11]; c=(t>>BASEBITS_464_28); 
	t=d[28]+c+s+(dchunk)(v[12]-v[16])*(md[16]-md[12])+(dchunk)(v[13]-v[15])*(md[15]-md[13]); a[11]=(chunk)t&BMASK_464_28;  s-=dd[12]; c=(t>>BASEBITS_464_28); 
	t=d[29]+c+s+(dchunk)(v[13]-v[16])*(md[16]-md[13])+(dchunk)(v[14]-v[15])*(md[15]-md[14]); a[12]=(chunk)t&BMASK_464_28;  s-=dd[13]; c=(t>>BASEBITS_464_28); 
	t=d[30]+c+s+(dchunk)(v[14]-v[16])*(md[16]-md[14]); a[13]=(chunk)t&BMASK_464_28;  s-=dd[14]; c=(t>>BASEBITS_464_28); 
	t=d[31]+c+s+(dchunk)(v[15]-v[16])*(md[16]-md[15]); a[14]=(chunk)t&BMASK_464_28;  s-=dd[15]; c=(t>>BASEBITS_464_28); 
	t=d[32]+c+s; a[15]=(chunk)t&BMASK_464_28;  s-=dd[16]; c=(t>>BASEBITS_464_28); 
	a[16]=d[33]+((chunk)c&BMASK_464_28);


#else

    	t = d[0];
	v[0] = ((chunk)t * MC)&BMASK_464_28;
	t += (dchunk)v[0] * md[0];
	t = (t >> BASEBITS_464_28) + d[1];
	t += (dchunk)v[0] * md[1] ; v[1] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[1] * md[0]; t = (t >> BASEBITS_464_28) + d[2];
	t += (dchunk)v[0] * md[2] + (dchunk)v[1]*md[1]; v[2] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[2] * md[0]; t = (t >> BASEBITS_464_28) + d[3];
	t += (dchunk)v[0] * md[3] + (dchunk)v[1]*md[2]+ (dchunk)v[2]*md[1]; v[3] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[3] * md[0]; t = (t >> BASEBITS_464_28) + d[4];
	t += (dchunk)v[0] * md[4] + (dchunk)v[1]*md[3]+ (dchunk)v[2]*md[2]+ (dchunk)v[3]*md[1]; v[4] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[4] * md[0]; t = (t >> BASEBITS_464_28) + d[5];
	t += (dchunk)v[0] * md[5] + (dchunk)v[1]*md[4]+ (dchunk)v[2]*md[3]+ (dchunk)v[3]*md[2]+ (dchunk)v[4]*md[1]; v[5] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[5] * md[0]; t = (t >> BASEBITS_464_28) + d[6];
	t += (dchunk)v[0] * md[6] + (dchunk)v[1]*md[5]+ (dchunk)v[2]*md[4]+ (dchunk)v[3]*md[3]+ (dchunk)v[4]*md[2]+ (dchunk)v[5]*md[1]; v[6] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[6] * md[0]; t = (t >> BASEBITS_464_28) + d[7];
	t += (dchunk)v[0] * md[7] + (dchunk)v[1]*md[6]+ (dchunk)v[2]*md[5]+ (dchunk)v[3]*md[4]+ (dchunk)v[4]*md[3]+ (dchunk)v[5]*md[2]+ (dchunk)v[6]*md[1]; v[7] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[7] * md[0]; t = (t >> BASEBITS_464_28) + d[8];
	t += (dchunk)v[0] * md[8] + (dchunk)v[1]*md[7]+ (dchunk)v[2]*md[6]+ (dchunk)v[3]*md[5]+ (dchunk)v[4]*md[4]+ (dchunk)v[5]*md[3]+ (dchunk)v[6]*md[2]+ (dchunk)v[7]*md[1]; v[8] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[8] * md[0]; t = (t >> BASEBITS_464_28) + d[9];
	t += (dchunk)v[0] * md[9] + (dchunk)v[1]*md[8]+ (dchunk)v[2]*md[7]+ (dchunk)v[3]*md[6]+ (dchunk)v[4]*md[5]+ (dchunk)v[5]*md[4]+ (dchunk)v[6]*md[3]+ (dchunk)v[7]*md[2]+ (dchunk)v[8]*md[1]; v[9] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[9] * md[0]; t = (t >> BASEBITS_464_28) + d[10];
	t += (dchunk)v[0] * md[10] + (dchunk)v[1]*md[9]+ (dchunk)v[2]*md[8]+ (dchunk)v[3]*md[7]+ (dchunk)v[4]*md[6]+ (dchunk)v[5]*md[5]+ (dchunk)v[6]*md[4]+ (dchunk)v[7]*md[3]+ (dchunk)v[8]*md[2]+ (dchunk)v[9]*md[1]; v[10] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[10] * md[0]; t = (t >> BASEBITS_464_28) + d[11];
	t += (dchunk)v[0] * md[11] + (dchunk)v[1]*md[10]+ (dchunk)v[2]*md[9]+ (dchunk)v[3]*md[8]+ (dchunk)v[4]*md[7]+ (dchunk)v[5]*md[6]+ (dchunk)v[6]*md[5]+ (dchunk)v[7]*md[4]+ (dchunk)v[8]*md[3]+ (dchunk)v[9]*md[2]+ (dchunk)v[10]*md[1]; v[11] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[11] * md[0]; t = (t >> BASEBITS_464_28) + d[12];
	t += (dchunk)v[0] * md[12] + (dchunk)v[1]*md[11]+ (dchunk)v[2]*md[10]+ (dchunk)v[3]*md[9]+ (dchunk)v[4]*md[8]+ (dchunk)v[5]*md[7]+ (dchunk)v[6]*md[6]+ (dchunk)v[7]*md[5]+ (dchunk)v[8]*md[4]+ (dchunk)v[9]*md[3]+ (dchunk)v[10]*md[2]+ (dchunk)v[11]*md[1]; v[12] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[12] * md[0]; t = (t >> BASEBITS_464_28) + d[13];
	t += (dchunk)v[0] * md[13] + (dchunk)v[1]*md[12]+ (dchunk)v[2]*md[11]+ (dchunk)v[3]*md[10]+ (dchunk)v[4]*md[9]+ (dchunk)v[5]*md[8]+ (dchunk)v[6]*md[7]+ (dchunk)v[7]*md[6]+ (dchunk)v[8]*md[5]+ (dchunk)v[9]*md[4]+ (dchunk)v[10]*md[3]+ (dchunk)v[11]*md[2]+ (dchunk)v[12]*md[1]; v[13] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[13] * md[0]; t = (t >> BASEBITS_464_28) + d[14];
	t += (dchunk)v[0] * md[14] + (dchunk)v[1]*md[13]+ (dchunk)v[2]*md[12]+ (dchunk)v[3]*md[11]+ (dchunk)v[4]*md[10]+ (dchunk)v[5]*md[9]+ (dchunk)v[6]*md[8]+ (dchunk)v[7]*md[7]+ (dchunk)v[8]*md[6]+ (dchunk)v[9]*md[5]+ (dchunk)v[10]*md[4]+ (dchunk)v[11]*md[3]+ (dchunk)v[12]*md[2]+ (dchunk)v[13]*md[1]; v[14] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[14] * md[0]; t = (t >> BASEBITS_464_28) + d[15];
	t += (dchunk)v[0] * md[15] + (dchunk)v[1]*md[14]+ (dchunk)v[2]*md[13]+ (dchunk)v[3]*md[12]+ (dchunk)v[4]*md[11]+ (dchunk)v[5]*md[10]+ (dchunk)v[6]*md[9]+ (dchunk)v[7]*md[8]+ (dchunk)v[8]*md[7]+ (dchunk)v[9]*md[6]+ (dchunk)v[10]*md[5]+ (dchunk)v[11]*md[4]+ (dchunk)v[12]*md[3]+ (dchunk)v[13]*md[2]+ (dchunk)v[14]*md[1]; v[15] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[15] * md[0]; t = (t >> BASEBITS_464_28) + d[16];
	t += (dchunk)v[0] * md[16] + (dchunk)v[1]*md[15]+ (dchunk)v[2]*md[14]+ (dchunk)v[3]*md[13]+ (dchunk)v[4]*md[12]+ (dchunk)v[5]*md[11]+ (dchunk)v[6]*md[10]+ (dchunk)v[7]*md[9]+ (dchunk)v[8]*md[8]+ (dchunk)v[9]*md[7]+ (dchunk)v[10]*md[6]+ (dchunk)v[11]*md[5]+ (dchunk)v[12]*md[4]+ (dchunk)v[13]*md[3]+ (dchunk)v[14]*md[2]+ (dchunk)v[15]*md[1]; v[16] = ((chunk)t * MC)&BMASK_464_28; t += (dchunk)v[16] * md[0]; t = (t >> BASEBITS_464_28) + d[17];
	t=t + (dchunk)v[1]*md[16] + (dchunk)v[2]*md[15] + (dchunk)v[3]*md[14] + (dchunk)v[4]*md[13] + (dchunk)v[5]*md[12] + (dchunk)v[6]*md[11] + (dchunk)v[7]*md[10] + (dchunk)v[8]*md[9] + (dchunk)v[9]*md[8] + (dchunk)v[10]*md[7] + (dchunk)v[11]*md[6] + (dchunk)v[12]*md[5] + (dchunk)v[13]*md[4] + (dchunk)v[14]*md[3] + (dchunk)v[15]*md[2] + (dchunk)v[16]*md[1] ; a[0] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[18];
	t=t + (dchunk)v[2]*md[16] + (dchunk)v[3]*md[15] + (dchunk)v[4]*md[14] + (dchunk)v[5]*md[13] + (dchunk)v[6]*md[12] + (dchunk)v[7]*md[11] + (dchunk)v[8]*md[10] + (dchunk)v[9]*md[9] + (dchunk)v[10]*md[8] + (dchunk)v[11]*md[7] + (dchunk)v[12]*md[6] + (dchunk)v[13]*md[5] + (dchunk)v[14]*md[4] + (dchunk)v[15]*md[3] + (dchunk)v[16]*md[2] ; a[1] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[19];
	t=t + (dchunk)v[3]*md[16] + (dchunk)v[4]*md[15] + (dchunk)v[5]*md[14] + (dchunk)v[6]*md[13] + (dchunk)v[7]*md[12] + (dchunk)v[8]*md[11] + (dchunk)v[9]*md[10] + (dchunk)v[10]*md[9] + (dchunk)v[11]*md[8] + (dchunk)v[12]*md[7] + (dchunk)v[13]*md[6] + (dchunk)v[14]*md[5] + (dchunk)v[15]*md[4] + (dchunk)v[16]*md[3] ; a[2] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[20];
	t=t + (dchunk)v[4]*md[16] + (dchunk)v[5]*md[15] + (dchunk)v[6]*md[14] + (dchunk)v[7]*md[13] + (dchunk)v[8]*md[12] + (dchunk)v[9]*md[11] + (dchunk)v[10]*md[10] + (dchunk)v[11]*md[9] + (dchunk)v[12]*md[8] + (dchunk)v[13]*md[7] + (dchunk)v[14]*md[6] + (dchunk)v[15]*md[5] + (dchunk)v[16]*md[4] ; a[3] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[21];
	t=t + (dchunk)v[5]*md[16] + (dchunk)v[6]*md[15] + (dchunk)v[7]*md[14] + (dchunk)v[8]*md[13] + (dchunk)v[9]*md[12] + (dchunk)v[10]*md[11] + (dchunk)v[11]*md[10] + (dchunk)v[12]*md[9] + (dchunk)v[13]*md[8] + (dchunk)v[14]*md[7] + (dchunk)v[15]*md[6] + (dchunk)v[16]*md[5] ; a[4] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[22];
	t=t + (dchunk)v[6]*md[16] + (dchunk)v[7]*md[15] + (dchunk)v[8]*md[14] + (dchunk)v[9]*md[13] + (dchunk)v[10]*md[12] + (dchunk)v[11]*md[11] + (dchunk)v[12]*md[10] + (dchunk)v[13]*md[9] + (dchunk)v[14]*md[8] + (dchunk)v[15]*md[7] + (dchunk)v[16]*md[6] ; a[5] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[23];
	t=t + (dchunk)v[7]*md[16] + (dchunk)v[8]*md[15] + (dchunk)v[9]*md[14] + (dchunk)v[10]*md[13] + (dchunk)v[11]*md[12] + (dchunk)v[12]*md[11] + (dchunk)v[13]*md[10] + (dchunk)v[14]*md[9] + (dchunk)v[15]*md[8] + (dchunk)v[16]*md[7] ; a[6] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[24];
	t=t + (dchunk)v[8]*md[16] + (dchunk)v[9]*md[15] + (dchunk)v[10]*md[14] + (dchunk)v[11]*md[13] + (dchunk)v[12]*md[12] + (dchunk)v[13]*md[11] + (dchunk)v[14]*md[10] + (dchunk)v[15]*md[9] + (dchunk)v[16]*md[8] ; a[7] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[25];
	t=t + (dchunk)v[9]*md[16] + (dchunk)v[10]*md[15] + (dchunk)v[11]*md[14] + (dchunk)v[12]*md[13] + (dchunk)v[13]*md[12] + (dchunk)v[14]*md[11] + (dchunk)v[15]*md[10] + (dchunk)v[16]*md[9] ; a[8] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[26];
	t=t + (dchunk)v[10]*md[16] + (dchunk)v[11]*md[15] + (dchunk)v[12]*md[14] + (dchunk)v[13]*md[13] + (dchunk)v[14]*md[12] + (dchunk)v[15]*md[11] + (dchunk)v[16]*md[10] ; a[9] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[27];
	t=t + (dchunk)v[11]*md[16] + (dchunk)v[12]*md[15] + (dchunk)v[13]*md[14] + (dchunk)v[14]*md[13] + (dchunk)v[15]*md[12] + (dchunk)v[16]*md[11] ; a[10] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[28];
	t=t + (dchunk)v[12]*md[16] + (dchunk)v[13]*md[15] + (dchunk)v[14]*md[14] + (dchunk)v[15]*md[13] + (dchunk)v[16]*md[12] ; a[11] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[29];
	t=t + (dchunk)v[13]*md[16] + (dchunk)v[14]*md[15] + (dchunk)v[15]*md[14] + (dchunk)v[16]*md[13] ; a[12] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[30];
	t=t + (dchunk)v[14]*md[16] + (dchunk)v[15]*md[15] + (dchunk)v[16]*md[14] ; a[13] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[31];
	t=t + (dchunk)v[15]*md[16] + (dchunk)v[16]*md[15] ; a[14] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[32];
	t=t + (dchunk)v[16]*md[16] ; a[15] = (chunk)t & BMASK_464_28; t = (t >> BASEBITS_464_28) + d[33];
	a[16] = (chunk)t & BMASK_464_28;


#endif


#else
#ifndef USE_KARATSUBA 
    t = d[0];
    v[0] = ((chunk)t * MC)&BMASK_464_28;
    t += (dchunk)v[0] * md[0];
    t = (t >> BASEBITS_464_28) + d[1];
   
    for (i = 1; i < NLEN_464_28; i++)
    {
        k=1;
        t += (dchunk)v[0] * md[i];
        while (k<i) {t += (dchunk)v[k]*md[i-k]; k++;}
        v[i] = ((chunk)t * MC)&BMASK_464_28;
        t += (dchunk)v[i] * md[0];
        t = (t >> BASEBITS_464_28) + d[i + 1];
    }
    for (i = NLEN_464_28; i < 2 * NLEN_464_28 - 1; i++)
    {
        k=i-(NLEN_464_28-1);
        while (k<=NLEN_464_28-1) {t += (dchunk)v[k]*md[i-k]; k++;}
        a[i - NLEN_464_28] = (chunk)t & BMASK_464_28;
        t = (t >> BASEBITS_464_28) + d[i + 1];
    }
    a[NLEN_464_28 - 1] = (chunk)t & BMASK_464_28;
#else
    t = d[0];
    v[0] = ((chunk)t * MC)&BMASK_464_28;
    t += (dchunk)v[0] * md[0];
    c = (t >> BASEBITS_464_28) + d[1];
    s = 0;

    for (k = 1; k < NLEN_464_28; k++)
    {
        t = c + s + (dchunk)v[0] * md[k];
        for (i=1+k/2;i<k;i++) t += (dchunk)(v[k - i] - v[i]) * (md[i] - md[k - i]);
        v[k] = ((chunk)t * MC)&BMASK_464_28;
        t += (dchunk)v[k] * md[0];
        c = (t >> BASEBITS_464_28) + d[k + 1];
        dd[k] = (dchunk)v[k] * md[k];
        s += dd[k];
    }
    for (k = NLEN_464_28; k < 2 * NLEN_464_28 - 1; k++)
    {
        t = c + s;
        for (i=1+k/2;i<NLEN_464_28;i++) t += (dchunk)(v[k - i] - v[i]) * (md[i] - md[k - i]);
        a[k - NLEN_464_28] = (chunk)t & BMASK_464_28;
        c = (t >> BASEBITS_464_28) + d[k + 1];
        s -= dd[k - NLEN_464_28 + 1];
    }
    a[NLEN_464_28 - 1] = (chunk)c & BMASK_464_28;

#endif
#endif


#else
    int j;
    chunk m, carry;
    for (i = 0; i < NLEN_464_28; i++)
    {
        if (MC == -1) m = (-d[i])&BMASK_464_28;
        else
        {
            if (MC == 1) m = d[i];
            else m = (MC * d[i])&BMASK_464_28;
        }
        carry = 0;
        for (j = 0; j < NLEN_464_28; j++)
            carry = muladd_464_28(m, md[j], carry, &d[i + j]);
        d[NLEN_464_28 + i] += carry;
    }
    BIG_464_28_sducopy(a, d);
    BIG_464_28_norm(a);

#endif

#ifdef DEBUG_NORM
    a[MPV_464_28] = 1;
    a[MNV_464_28] = 0;
#endif
}

/* General shift left of a by n bits */
/* a MUST be normalised */
/* SU= 32 */
void BIG_464_28_shl(BIG_464_28 a, int k)
{
    int i;
    int n = k % BASEBITS_464_28;
    int m = k / BASEBITS_464_28;

    a[NLEN_464_28 - 1] = ((a[NLEN_464_28 - 1 - m] << n));
    if (NLEN_464_28 >= m + 2) a[NLEN_464_28 - 1] |= (a[NLEN_464_28 - m - 2] >> (BASEBITS_464_28 - n));

    for (i = NLEN_464_28 - 2; i > m; i--)
        a[i] = ((a[i - m] << n)&BMASK_464_28) | (a[i - m - 1] >> (BASEBITS_464_28 - n));
    a[m] = (a[0] << n)&BMASK_464_28;
    for (i = 0; i < m; i++) a[i] = 0;

}

/* Fast shift left of a by n bits, where n less than a word, Return excess (but store it as well) */
/* a MUST be normalised */
/* SU= 16 */
int BIG_464_28_fshl(BIG_464_28 a, int n)
{
    int i;

    a[NLEN_464_28 - 1] = ((a[NLEN_464_28 - 1] << n)) | (a[NLEN_464_28 - 2] >> (BASEBITS_464_28 - n)); /* top word not masked */
    for (i = NLEN_464_28 - 2; i > 0; i--)
        a[i] = ((a[i] << n)&BMASK_464_28) | (a[i - 1] >> (BASEBITS_464_28 - n));
    a[0] = (a[0] << n)&BMASK_464_28;

    return (int)(a[NLEN_464_28 - 1] >> ((8 * MODBYTES_464_28) % BASEBITS_464_28)); /* return excess - only used in ff.c */
}

/* double length left shift of a by k bits - k can be > BASEBITS , a MUST be normalised */
/* SU= 32 */
void BIG_464_28_dshl(DBIG_464_28 a, int k)
{
    int i;
    int n = k % BASEBITS_464_28;
    int m = k / BASEBITS_464_28;

    a[DNLEN_464_28 - 1] = ((a[DNLEN_464_28 - 1 - m] << n)) | (a[DNLEN_464_28 - m - 2] >> (BASEBITS_464_28 - n));

    for (i = DNLEN_464_28 - 2; i > m; i--)
        a[i] = ((a[i - m] << n)&BMASK_464_28) | (a[i - m - 1] >> (BASEBITS_464_28 - n));
    a[m] = (a[0] << n)&BMASK_464_28;
    for (i = 0; i < m; i++) a[i] = 0;

}

/* General shift right of a by k bits */
/* a MUST be normalised */
/* SU= 32 */
void BIG_464_28_shr(BIG_464_28 a, int k)
{
    int i;
    int n = k % BASEBITS_464_28;
    int m = k / BASEBITS_464_28;
    for (i = 0; i < NLEN_464_28 - m - 1; i++)
        a[i] = (a[m + i] >> n) | ((a[m + i + 1] << (BASEBITS_464_28 - n))&BMASK_464_28);
    if (NLEN_464_28 > m)  a[NLEN_464_28 - m - 1] = a[NLEN_464_28 - 1] >> n;
    for (i = NLEN_464_28 - m; i < NLEN_464_28; i++) a[i] = 0;

}

/* Fast combined shift, subtract and norm. Return sign of result */
int BIG_464_28_ssn(BIG_464_28 r, BIG_464_28 a, BIG_464_28 m)
{
    int i, n = NLEN_464_28 - 1;
    chunk carry;
    m[0] = (m[0] >> 1) | ((m[1] << (BASEBITS_464_28 - 1))&BMASK_464_28);
    r[0] = a[0] - m[0];
    carry = r[0] >> BASEBITS_464_28;
    r[0] &= BMASK_464_28;

    for (i = 1; i < n; i++)
    {
        m[i] = (m[i] >> 1) | ((m[i + 1] << (BASEBITS_464_28 - 1))&BMASK_464_28);
        r[i] = a[i] - m[i] + carry;
        carry = r[i] >> BASEBITS_464_28;
        r[i] &= BMASK_464_28;
    }

    m[n] >>= 1;
    r[n] = a[n] - m[n] + carry;
#ifdef DEBUG_NORM
    r[MPV_464_28] = 1;
    r[MNV_464_28] = 0;
#endif
    return ((r[n] >> (CHUNK - 1)) & 1);
}

/* Faster shift right of a by k bits. Return shifted out part */
/* a MUST be normalised */
/* SU= 16 */
int BIG_464_28_fshr(BIG_464_28 a, int k)
{
    int i;
    chunk r = a[0] & (((chunk)1 << k) - 1); /* shifted out part */
    for (i = 0; i < NLEN_464_28 - 1; i++)
        a[i] = (a[i] >> k) | ((a[i + 1] << (BASEBITS_464_28 - k))&BMASK_464_28);
    a[NLEN_464_28 - 1] = a[NLEN_464_28 - 1] >> k;
    return (int)r;
}

/* double length right shift of a by k bits - can be > BASEBITS */
/* SU= 32 */
void BIG_464_28_dshr(DBIG_464_28 a, int k)
{
    int i;
    int n = k % BASEBITS_464_28;
    int m = k / BASEBITS_464_28;
    for (i = 0; i < DNLEN_464_28 - m - 1; i++)
        a[i] = (a[m + i] >> n) | ((a[m + i + 1] << (BASEBITS_464_28 - n))&BMASK_464_28);
    a[DNLEN_464_28 - m - 1] = a[DNLEN_464_28 - 1] >> n;
    for (i = DNLEN_464_28 - m; i < DNLEN_464_28; i++ ) a[i] = 0;
}

/* Split DBIG d into two BIGs t|b. Split happens at n bits, where n falls into NLEN word */
/* d MUST be normalised */
/* SU= 24 */
chunk BIG_464_28_split(BIG_464_28 t, BIG_464_28 b, DBIG_464_28 d, int n)
{
    int i;
    chunk nw, carry = 0;
    int m = n % BASEBITS_464_28;

    if (m == 0)
    {
        for (i = 0; i < NLEN_464_28; i++) b[i] = d[i];
        if (t != b)
        {
            for (i = NLEN_464_28; i < 2 * NLEN_464_28; i++) t[i - NLEN_464_28] = d[i];
            carry = t[NLEN_464_28 - 1] >> BASEBITS_464_28;
            t[NLEN_464_28 - 1] = t[NLEN_464_28 - 1] & BMASK_464_28; /* top word normalized */
        }
        return carry;
    }

    for (i = 0; i < NLEN_464_28 - 1; i++) b[i] = d[i];

    b[NLEN_464_28 - 1] = d[NLEN_464_28 - 1] & (((chunk)1 << m) - 1);

    if (t != b)
    {
        carry = (d[DNLEN_464_28 - 1] << (BASEBITS_464_28 - m));
        for (i = DNLEN_464_28 - 2; i >= NLEN_464_28 - 1; i--)
        {
            nw = (d[i] >> m) | carry;
            carry = (d[i] << (BASEBITS_464_28 - m))&BMASK_464_28;
            t[i - NLEN_464_28 + 1] = nw;
        }
    }
#ifdef DEBUG_NORM
    t[MPV_464_28] = 1;
    t[MNV_464_28] = 0;
    b[MPV_464_28] = 1;
    b[MNV_464_28] = 0;
#endif
    return carry;
}

/* you gotta keep the sign of carry! Look - no branching! */
/* Note that sign bit is needed to disambiguate between +ve and -ve values */
/* normalise BIG - force all digits < 2^BASEBITS */
chunk BIG_464_28_norm(BIG_464_28 a)
{
    int i;
    chunk d, carry;

    carry=a[0]>>BASEBITS_464_28;
    a[0]&=BMASK_464_28;

    for (i = 1; i < NLEN_464_28 - 1; i++)
    {
        d = a[i] + carry;
        a[i] = d & BMASK_464_28;
        carry = d >> BASEBITS_464_28;
    }
    a[NLEN_464_28 - 1] = (a[NLEN_464_28 - 1] + carry);

#ifdef DEBUG_NORM
    a[MPV_464_28] = 1;
    a[MNV_464_28] = 0;
#endif
    return (a[NLEN_464_28 - 1] >> ((8 * MODBYTES_464_28) % BASEBITS_464_28)); /* only used in ff.c */
}

void BIG_464_28_dnorm(DBIG_464_28 a)
{
    int i;
    chunk d, carry;

    carry=a[0]>>BASEBITS_464_28;
    a[0]&=BMASK_464_28;

    for (i = 1; i < DNLEN_464_28 - 1; i++)
    {
        d = a[i] + carry;
        a[i] = d & BMASK_464_28;
        carry = d >> BASEBITS_464_28;
    }
    a[DNLEN_464_28 - 1] = (a[DNLEN_464_28 - 1] + carry);
#ifdef DEBUG_NORM
    a[DMPV_464_28] = 1;
    a[DMNV_464_28] = 0;
#endif
}

/* Compare a and b. Return 1 for a>b, -1 for a<b, 0 for a==b */
/* a and b MUST be normalised before call */
/* sodium constant time implementation */

int BIG_464_28_comp(BIG_464_28 a, BIG_464_28 b)
{
    int i;
    chunk gt=0; chunk eq=1;
    for (i = NLEN_464_28-1; i>=0; i--)
    {
        gt |= ((b[i]-a[i]) >> BASEBITS_464_28) & eq;
        eq &= ((b[i]^a[i])-1) >> BASEBITS_464_28;
    }
    return (int)(gt+gt+eq-1);
}

int BIG_464_28_dcomp(DBIG_464_28 a, DBIG_464_28 b)
{
    int i;
    chunk gt=0; chunk eq=1;
    for (i = DNLEN_464_28-1; i>=0; i--)
    {
        gt |= ((b[i]-a[i]) >> BASEBITS_464_28) & eq;
        eq &= ((b[i]^a[i])-1) >> BASEBITS_464_28;
    }
    return (int)(gt+gt+eq-1);
}

/* return number of bits in a */
/* SU= 8 */
int BIG_464_28_nbits(BIG_464_28 a)
{
    int bts, k = NLEN_464_28 - 1;
    BIG_464_28 t;
    chunk c;
    BIG_464_28_copy(t, a);
    BIG_464_28_norm(t);
    while (k >= 0 && t[k] == 0) k--;
    if (k < 0) return 0;
    bts = BASEBITS_464_28 * k;
    c = t[k];
    while (c != 0)
    {
        c /= 2;
        bts++;
    }
    return bts;
}

/* SU= 8, Calculate number of bits in a DBIG - output normalised */
int BIG_464_28_dnbits(DBIG_464_28 a)
{
    int bts, k = DNLEN_464_28 - 1;
    DBIG_464_28 t;
    chunk c;
    BIG_464_28_dcopy(t, a);
    BIG_464_28_dnorm(t);
    while (k >= 0 && t[k] == 0) k--;
    if (k < 0) return 0;
    bts = BASEBITS_464_28 * k;
    c = t[k];
    while (c != 0)
    {
        c /= 2;
        bts++;
    }
    return bts;
}

void BIG_464_28_ctmod(BIG_464_28 b,BIG_464_28 m,int bd)
{
    int k=bd;
    BIG_464_28 r,c;
    BIG_464_28_copy(c,m);
    BIG_464_28_norm(b);

    BIG_464_28_shl(c,k);
    while (k>=0)
    {
        BIG_464_28_sub(r, b, c);
        BIG_464_28_norm(r);
        BIG_464_28_cmove(b, r, 1 - ((r[NLEN_464_28 - 1] >> (CHUNK - 1)) & 1));
        BIG_464_28_fshr(c, 1);
        k--;
    }
}

/* Set b=b mod c */
/* SU= 16 */
void BIG_464_28_mod(BIG_464_28 b, BIG_464_28 m)
{
    int k=BIG_464_28_nbits(b)-BIG_464_28_nbits(m);
    if (k<0) k=0;
    BIG_464_28_ctmod(b,m,k);
}

// Set a=b mod m in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m
void BIG_464_28_ctdmod(BIG_464_28 a, DBIG_464_28 b, BIG_464_28 m, int bd)
{
    int k=bd;
    DBIG_464_28 c,r;
    BIG_464_28_dscopy(c,m);
    BIG_464_28_dnorm(b);

    BIG_464_28_dshl(c,k);
    while (k>=0)
    {
        BIG_464_28_dsub(r, b, c);
        BIG_464_28_dnorm(r);
        BIG_464_28_dcmove(b, r, 1 - ((r[DNLEN_464_28 - 1] >> (CHUNK - 1)) & 1));
        BIG_464_28_dshr(c, 1);
        k--;
    }
    BIG_464_28_sdcopy(a,b);
}


/* Set a=b mod m, b is destroyed. Slow but rarely used. */
void BIG_464_28_dmod(BIG_464_28 a, DBIG_464_28 b, BIG_464_28 m)
{
    int k=BIG_464_28_dnbits(b)-BIG_464_28_nbits(m);
    if (k<0) k=0;
    BIG_464_28_ctdmod(a,b,m,k);
}

// a=b/m  in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m
void BIG_464_28_ctddiv(BIG_464_28 a,DBIG_464_28 b,BIG_464_28 m,int bd)
{
    int d,k=bd;
    DBIG_464_28 c,dr;
    BIG_464_28 e,r;
    BIG_464_28_dscopy(c,m);
    BIG_464_28_dnorm(b);

    BIG_464_28_zero(a);
    BIG_464_28_zero(e);
    BIG_464_28_inc(e, 1);

    BIG_464_28_shl(e,k);
    BIG_464_28_dshl(c,k);

    while (k >= 0)
    {
        BIG_464_28_dsub(dr, b, c);
        BIG_464_28_dnorm(dr);
        d = 1 - ((dr[DNLEN_464_28 - 1] >> (CHUNK - 1)) & 1);
        BIG_464_28_dcmove(b, dr, d);

        BIG_464_28_add(r, a, e);
        BIG_464_28_norm(r);
        BIG_464_28_cmove(a, r, d);

        BIG_464_28_dshr(c, 1);
        BIG_464_28_fshr(e, 1);
        k--;
    }
}

/* Set a=b/m,  b is destroyed. Slow but rarely used. */
void BIG_464_28_ddiv(BIG_464_28 a, DBIG_464_28 b, BIG_464_28 m)
{
    int k=BIG_464_28_dnbits(b)-BIG_464_28_nbits(m);
    if (k<0) k=0;
    BIG_464_28_ctddiv(a,b,m,k);
}

// a=a/m  in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m
void BIG_464_28_ctsdiv(BIG_464_28 b,BIG_464_28 m,int bd)
{
    int d, k=bd;
    BIG_464_28 e,a,r,c;
    BIG_464_28_norm(b);
    BIG_464_28_copy(a,b);
    BIG_464_28_copy(c,m);
    BIG_464_28_zero(b);
    BIG_464_28_zero(e);
    BIG_464_28_inc(e, 1);

    BIG_464_28_shl(c,k);
    BIG_464_28_shl(e,k);

    while (k >= 0)
    {
        BIG_464_28_sub(r, a, c);
        BIG_464_28_norm(r);
        d = 1 - ((r[NLEN_464_28 - 1] >> (CHUNK - 1)) & 1);
        BIG_464_28_cmove(a, r, d);

        BIG_464_28_add(r, b, e);
        BIG_464_28_norm(r);
        BIG_464_28_cmove(b, r, d);

        BIG_464_28_fshr(c, 1);
        BIG_464_28_fshr(e, 1);

        k--;
    }
}

void BIG_464_28_sdiv(BIG_464_28 b, BIG_464_28 m)
{
    int k=BIG_464_28_nbits(b)-BIG_464_28_nbits(m);
    if (k<0) k=0;
    BIG_464_28_ctsdiv(b,m,k);
}

/* return LSB of a */
int BIG_464_28_parity(BIG_464_28 a)
{
    return a[0] % 2;
}

/* return n-th bit of a */
/* SU= 16 */
int BIG_464_28_bit(BIG_464_28 a, int n)
{
    return (int)((a[n / BASEBITS_464_28] & ((chunk)1 << (n % BASEBITS_464_28))) >> (n%BASEBITS_464_28));
//    if (a[n / BASEBITS_464_28] & ((chunk)1 << (n % BASEBITS_464_28))) return 1;
//    else return 0;
}

/* return last n bits of a, where n is small < BASEBITS */
/* SU= 16 */
int BIG_464_28_lastbits(BIG_464_28 a, int n)
{
    int msk = (1 << n) - 1;
    BIG_464_28_norm(a);
    return ((int)a[0])&msk;
}

/* get 8*MODBYTES size random number */
void BIG_464_28_random(BIG_464_28 m, csprng *rng)
{
    int i, b, j = 0, r = 0;
    int len = 8 * MODBYTES_464_28;

    BIG_464_28_zero(m);
    /* generate random BIG */
    for (i = 0; i < len; i++)
    {
        if (j == 0) r = RAND_byte(rng);
        else r >>= 1;
        b = r & 1;
        BIG_464_28_shl(m, 1);
        m[0] += b;
        j++;
        j &= 7;
    }

#ifdef DEBUG_NORM
    m[MPV_464_28] = 1;
    m[MNV_464_28] = 0;
#endif
}

/* get random BIG from rng, modulo q. Done one bit at a time, so its portable */

void BIG_464_28_randomnum(BIG_464_28 m, BIG_464_28 q, csprng *rng)
{
    int i, b, j = 0, r = 0;
    int n=2 * BIG_464_28_nbits(q);
    DBIG_464_28 d;
    BIG_464_28_dzero(d);
    /* generate random DBIG */
    for (i = 0; i < n; i++)
    {
        if (j == 0) r = RAND_byte(rng);
        else r >>= 1;
        b = r & 1;
        BIG_464_28_dshl(d, 1);
        d[0] += b;
        j++;
        j &= 7;
    }
    /* reduce modulo a BIG. Removes bias */
    BIG_464_28_dmod(m, d, q);
#ifdef DEBUG_NORM
    m[MPV_464_28] = 1;
    m[MNV_464_28] = 0;
#endif
}

/* create randum BIG less than r and less than trunc bits */
void BIG_464_28_randtrunc(BIG_464_28 s, BIG_464_28 r, int trunc, csprng *rng)
{
    BIG_464_28_randomnum(s, r, rng);
    if (BIG_464_28_nbits(r) > trunc)
        BIG_464_28_mod2m(s, trunc);
}


/* Set r=a*b mod m */
/* SU= 96 */
void BIG_464_28_modmul(BIG_464_28 r, BIG_464_28 a1, BIG_464_28 b1, BIG_464_28 m)
{
    DBIG_464_28 d;
    BIG_464_28 a, b;
    BIG_464_28_copy(a, a1);
    BIG_464_28_copy(b, b1);
    BIG_464_28_mod(a, m);
    BIG_464_28_mod(b, m);

    BIG_464_28_mul(d, a, b);
    BIG_464_28_ctdmod(r, d, m, BIG_464_28_nbits(m));
}

/* Set a=a*a mod m */
/* SU= 88 */
void BIG_464_28_modsqr(BIG_464_28 r, BIG_464_28 a1, BIG_464_28 m)
{
    DBIG_464_28 d;
    BIG_464_28 a;
    BIG_464_28_copy(a, a1);
    BIG_464_28_mod(a, m);
    BIG_464_28_sqr(d, a);
    BIG_464_28_ctdmod(r, d, m, BIG_464_28_nbits(m));
}

/* Set r=-a mod m */
/* SU= 16 */
void BIG_464_28_modneg(BIG_464_28 r, BIG_464_28 a1, BIG_464_28 m)
{
    BIG_464_28 a;
    BIG_464_28_copy(a, a1);
    BIG_464_28_mod(a, m);
    BIG_464_28_sub(r, m, a); BIG_464_28_norm(r);
}

/* Set r=a+b mod m */
void BIG_464_28_modadd(BIG_464_28 r, BIG_464_28 a1, BIG_464_28 b1, BIG_464_28 m)
{
    BIG_464_28 a, b;
    BIG_464_28_copy(a, a1);
    BIG_464_28_copy(b, b1);
    BIG_464_28_mod(a, m);
    BIG_464_28_mod(b, m);
    BIG_464_28_add(r,a,b); BIG_464_28_norm(r);
    BIG_464_28_ctmod(r,m,1);
}

/* Set a=a/b mod m */
/* SU= 136 */
void BIG_464_28_moddiv(BIG_464_28 r, BIG_464_28 a1, BIG_464_28 b1, BIG_464_28 m)
{
    DBIG_464_28 d;
    BIG_464_28 z;
    BIG_464_28 a, b;
    BIG_464_28_copy(a, a1);
    BIG_464_28_copy(b, b1);

    BIG_464_28_mod(a, m);
    BIG_464_28_invmodp(z, b, m);

    BIG_464_28_mul(d, a, z);
    BIG_464_28_ctdmod(r, d, m, BIG_464_28_nbits(m));
}

/* Get jacobi Symbol (a/p). Returns 0, 1 or -1 */
/* SU= 216 */
int BIG_464_28_jacobi(BIG_464_28 a, BIG_464_28 p)
{
    int n8, k, m = 0;
    BIG_464_28 t, x, n, zilch, one;
    BIG_464_28_one(one);
    BIG_464_28_zero(zilch);
    if (BIG_464_28_parity(p) == 0 || BIG_464_28_comp(a, zilch) == 0 || BIG_464_28_comp(p, one) <= 0) return 0;
    BIG_464_28_norm(a);
    BIG_464_28_copy(x, a);
    BIG_464_28_copy(n, p);
    BIG_464_28_mod(x, p);

    while (BIG_464_28_comp(n, one) > 0)
    {
        if (BIG_464_28_comp(x, zilch) == 0) return 0;
        n8 = BIG_464_28_lastbits(n, 3);
        k = 0;
        while (BIG_464_28_parity(x) == 0)
        {
            k++;
            BIG_464_28_shr(x, 1);
        }
        if (k % 2 == 1) m += (n8 * n8 - 1) / 8;
        m += (n8 - 1) * (BIG_464_28_lastbits(x, 2) - 1) / 4;
        BIG_464_28_copy(t, n);

        BIG_464_28_mod(t, x);
        BIG_464_28_copy(n, x);
        BIG_464_28_copy(x, t);
        m %= 2;

    }
    if (m == 0) return 1;
    else return -1;
}

/* Arazi and Qi inversion mod 256 */
static int invmod256(int a)
{
    int U, t1, t2, b, c;
    t1 = 0;
    c = (a >> 1) & 1;
    t1 += c;
    t1 &= 1;
    t1 = 2 - t1;
    t1 <<= 1;
    U = t1 + 1;

// i=2
    b = a & 3;
    t1 = U * b;
    t1 >>= 2;
    c = (a >> 2) & 3;
    t2 = (U * c) & 3;
    t1 += t2;
    t1 *= U;
    t1 &= 3;
    t1 = 4 - t1;
    t1 <<= 2;
    U += t1;

// i=4
    b = a & 15;
    t1 = U * b;
    t1 >>= 4;
    c = (a >> 4) & 15;
    t2 = (U * c) & 15;
    t1 += t2;
    t1 *= U;
    t1 &= 15;
    t1 = 16 - t1;
    t1 <<= 4;
    U += t1;

    return U;
}

/* a=1/a mod 2^BIGBITS. This is very fast! */
void BIG_464_28_invmod2m(BIG_464_28 a)
{
    int i;
    BIG_464_28 U, t1, b, c;
    BIG_464_28_zero(U);
    BIG_464_28_inc(U, invmod256(BIG_464_28_lastbits(a, 8)));
    for (i = 8; i < BIGBITS_464_28; i <<= 1)
    {
        BIG_464_28_norm(U);
        BIG_464_28_copy(b, a);
        BIG_464_28_mod2m(b, i);  // bottom i bits of a

        BIG_464_28_smul(t1, U, b);
        BIG_464_28_shr(t1, i); // top i bits of U*b

        BIG_464_28_copy(c, a);
        BIG_464_28_shr(c, i);
        BIG_464_28_mod2m(c, i); // top i bits of a

        BIG_464_28_smul(b, U, c);
        BIG_464_28_mod2m(b, i); // bottom i bits of U*c

        BIG_464_28_add(t1, t1, b);
        BIG_464_28_norm(t1);
        BIG_464_28_smul(b, t1, U);
        BIG_464_28_copy(t1, b); // (t1+b)*U
        BIG_464_28_mod2m(t1, i);               // bottom i bits of (t1+b)*U

        BIG_464_28_one(b);
        BIG_464_28_shl(b, i);
        BIG_464_28_sub(t1, b, t1);
        BIG_464_28_norm(t1);

        BIG_464_28_shl(t1, i);

        BIG_464_28_add(U, U, t1);
    }
    BIG_464_28_copy(a, U);
    BIG_464_28_norm(a);
    BIG_464_28_mod2m(a, BIGBITS_464_28);
}

/* Set r=1/a mod p. Binary method */
/* SU= 240 */
void BIG_464_28_invmodp(BIG_464_28 r, BIG_464_28 a, BIG_464_28 p)
{
    BIG_464_28 u, v, x1, x2, t, one;

    BIG_464_28_mod(a, p);
    if (BIG_464_28_iszilch(a))
    {
        BIG_464_28_zero(r);
        return;
    }

    BIG_464_28_copy(u, a);
    BIG_464_28_copy(v, p);
    BIG_464_28_one(one);
    BIG_464_28_copy(x1, one);
    BIG_464_28_zero(x2);

    while (BIG_464_28_comp(u, one) != 0 && BIG_464_28_comp(v, one) != 0)
    {
        while (BIG_464_28_parity(u) == 0)
        {
            BIG_464_28_fshr(u, 1);
            BIG_464_28_add(t,x1,p);
            BIG_464_28_cmove(x1,t,BIG_464_28_parity(x1));
            BIG_464_28_norm(x1);
            BIG_464_28_fshr(x1,1);
        }
        while (BIG_464_28_parity(v) == 0)
        {
            BIG_464_28_fshr(v, 1);
            BIG_464_28_add(t,x2,p);
            BIG_464_28_cmove(x2,t,BIG_464_28_parity(x2));
            BIG_464_28_norm(x2);
            BIG_464_28_fshr(x2,1);
        }
        if (BIG_464_28_comp(u, v) >= 0)
        {
            BIG_464_28_sub(u, u, v);
            BIG_464_28_norm(u);
            BIG_464_28_add(t,x1,p);
            BIG_464_28_cmove(x1,t,(BIG_464_28_comp(x1,x2)>>1)&1); /* move if x1<x2 */
            BIG_464_28_sub(x1,x1,x2);
            BIG_464_28_norm(x1);
        }
        else
        {
            BIG_464_28_sub(v, v, u);
            BIG_464_28_norm(v);
            BIG_464_28_add(t,x2,p);
            BIG_464_28_cmove(x2,t,(BIG_464_28_comp(x2,x1)>>1)&1); /* move if x2<x1 */
            BIG_464_28_sub(x2,x2,x1);
            BIG_464_28_norm(x2);
        }
    }
    BIG_464_28_copy(r,x1);
    BIG_464_28_cmove(r,x2,BIG_464_28_comp(u,one)&1);
}

/* set x = x mod 2^m */
void BIG_464_28_mod2m(BIG_464_28 x, int m)
{
    int i, wd, bt;
    chunk msk;
    BIG_464_28_norm(x);

    wd = m / BASEBITS_464_28;
    bt = m % BASEBITS_464_28;
    msk = ((chunk)1 << bt) - 1;
    x[wd] &= msk;
    for (i = wd + 1; i < NLEN_464_28; i++) x[i] = 0;
}

// new
/* Convert to DBIG number from byte array of given length */
void BIG_464_28_dfromBytesLen(DBIG_464_28 a, char *b, int s)
{
    int i, len = s;
    BIG_464_28_dzero(a);

    for (i = 0; i < len; i++)
    {
        BIG_464_28_dshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[DMPV_464_28] = 1;
    a[DMNV_464_28] = 0;
#endif
}

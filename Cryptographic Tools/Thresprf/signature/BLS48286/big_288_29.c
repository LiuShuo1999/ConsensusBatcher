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

#include "big_288_29.h"

/* test a=0? */
int BIG_288_29_iszilch(BIG_288_29 a)
{
    int i;
    chunk d=0;
    for (i = 0; i < NLEN_288_29; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_288_29));
}

/* test a=1? */
int BIG_288_29_isunity(BIG_288_29 a)
{
    int i;
    chunk d=0;
    for (i = 1; i < NLEN_288_29; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_288_29) & (((a[0]^1)-1)>>BASEBITS_288_29));
}

/* test a=0? */
int BIG_288_29_diszilch(DBIG_288_29 a)
{
    int i;
    chunk d=0;
    for (i = 0; i < DNLEN_288_29; i++)
        d|=a[i];
    return (1 & ((d-1)>>BASEBITS_288_29));
}

/* SU= 56 */
/* output a */
void BIG_288_29_output(BIG_288_29 a)
{
    BIG_288_29 b;
    int i, len;
    len = BIG_288_29_nbits(a);
    if (len % 4 == 0) len /= 4;
    else
    {
        len /= 4;
        len++;
    }
    if (len < MODBYTES_288_29 * 2) len = MODBYTES_288_29 * 2;

    for (i = len - 1; i >= 0; i--)
    {
        BIG_288_29_copy(b, a);
        BIG_288_29_shr(b, i * 4);
        printf("%01x", (unsigned int) b[0] & 15);
    }
}

/* SU= 16 */
void BIG_288_29_rawoutput(BIG_288_29 a)
{
    int i;
    printf("(");
    for (i = 0; i < NLEN_288_29 - 1; i++)
#if CHUNK==64
        printf("%"PRIxMAX",", (uintmax_t) a[i]);
    printf("%"PRIxMAX")", (uintmax_t) a[NLEN_288_29 - 1]);
#else
        printf("%x,", (unsigned int) a[i]);
    printf("%x)", (unsigned int) a[NLEN_288_29 - 1]);
#endif
}

/* Swap a and b if d=1 */
chunk BIG_288_29_cswap(BIG_288_29 a, BIG_288_29 b, int d)
{
    int i;
    chunk e,r,ra,w,t, c = (chunk) - d;
    w=0; 
    r=a[0]^b[1]; 
    ra=r+r; ra>>=1; // I know this doesn't change r, but the compiler doesn't!
#ifdef DEBUG_NORM
    for (i = 0; i < NLEN_288_29 + 2; i++)
#else
    for (i = 0; i < NLEN_288_29; i++)
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
chunk BIG_288_29_cmove(BIG_288_29 f, BIG_288_29 g, int d)
{
    int i;
    chunk e,w,r,ra,t,b = (chunk) - d;
    w=0;
    r=f[0]^g[1];
    ra=r+r; ra>>=1; // I know this doesn't change r, but the compiler doesn't!
#ifdef DEBUG_NORM
    for (i = 0; i < NLEN_288_29 + 2; i++)
#else
    for (i = 0; i < NLEN_288_29; i++)
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
chunk BIG_288_29_dcmove(DBIG_288_29 f, DBIG_288_29 g, int d)
{
    int i;
    chunk e,w,r,ra,t,b = (chunk) - d;
    w=0;
    r=f[0]+g[1];
    ra=r+r; ra>>=1; // I know this doesn't change r, but the compiler doesn't!
#ifdef DEBUG_NORM
    for (i = 0; i < DNLEN_288_29 + 2; i++)
#else
    for (i = 0; i < DNLEN_288_29; i++)
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
void BIG_288_29_toBytes(char *b, BIG_288_29 a)
{
    int i;
    BIG_288_29 c;
    BIG_288_29_copy(c, a);
    BIG_288_29_norm(c);
    for (i = MODBYTES_288_29 - 1; i >= 0; i--)
    {
        b[i] = c[0] & 0xff;
        BIG_288_29_fshr(c, 8);
    }
}

/* SU= 16 */
void BIG_288_29_fromBytes(BIG_288_29 a, char *b)
{
    int i;
    BIG_288_29_zero(a);
    for (i = 0; i < MODBYTES_288_29; i++)
    {
        BIG_288_29_fshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[MPV_288_29] = 1;
    a[MNV_288_29] = 0;
#endif
}

void BIG_288_29_fromBytesLen(BIG_288_29 a, char *b, int s)
{
    int i, len = s;
    BIG_288_29_zero(a);

    if (len > MODBYTES_288_29) len = MODBYTES_288_29;
    for (i = 0; i < len; i++)
    {
        BIG_288_29_fshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[MPV_288_29] = 1;
    a[MNV_288_29] = 0;
#endif
}



/* SU= 88 */
void BIG_288_29_doutput(DBIG_288_29 a)
{
    DBIG_288_29 b;
    int i, len;
    BIG_288_29_dnorm(a);
    len = BIG_288_29_dnbits(a);
    if (len % 4 == 0) len /= 4;
    else
    {
        len /= 4;
        len++;
    }

    for (i = len - 1; i >= 0; i--)
    {
        BIG_288_29_dcopy(b, a);
        BIG_288_29_dshr(b, i * 4);
        printf("%01x", (unsigned int) b[0] & 15);
    }
}


void BIG_288_29_drawoutput(DBIG_288_29 a)
{
    int i;
    printf("(");
    for (i = 0; i < DNLEN_288_29 - 1; i++)
#if CHUNK==64
        printf("%"PRIxMAX",", (uintmax_t) a[i]);
    printf("%"PRIxMAX")", (uintmax_t) a[DNLEN_288_29 - 1]);
#else
        printf("%x,", (unsigned int) a[i]);
    printf("%x)", (unsigned int) a[DNLEN_288_29 - 1]);
#endif
}

/* Copy b=a */
void BIG_288_29_copy(BIG_288_29 b, BIG_288_29 a)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_288_29] = a[MPV_288_29];
    b[MNV_288_29] = a[MNV_288_29];
#endif
}

/* Copy from ROM b=a */
void BIG_288_29_rcopy(BIG_288_29 b, const BIG_288_29 a)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_288_29] = 1;
    b[MNV_288_29] = 0;
#endif
}

/* double length DBIG copy b=a */
void BIG_288_29_dcopy(DBIG_288_29 b, DBIG_288_29 a)
{
    int i;
    for (i = 0; i < DNLEN_288_29; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[DMPV_288_29] = a[DMPV_288_29];
    b[DMNV_288_29] = a[DMNV_288_29];
#endif
}

/* Copy BIG to bottom half of DBIG */
void BIG_288_29_dscopy(DBIG_288_29 b, BIG_288_29 a)
{
    int i;
    for (i = 0; i < NLEN_288_29 - 1; i++)
        b[i] = a[i];

    b[NLEN_288_29 - 1] = a[NLEN_288_29 - 1] & BMASK_288_29; /* top word normalized */
    b[NLEN_288_29] = a[NLEN_288_29 - 1] >> BASEBITS_288_29;

    for (i = NLEN_288_29 + 1; i < DNLEN_288_29; i++) b[i] = 0;
#ifdef DEBUG_NORM
    b[DMPV_288_29] = a[MPV_288_29];
    b[DMNV_288_29] = a[MNV_288_29];
#endif
}

/* Copy BIG to top half of DBIG */
void BIG_288_29_dsucopy(DBIG_288_29 b, BIG_288_29 a)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++)
        b[i] = 0;
    for (i = NLEN_288_29; i < DNLEN_288_29; i++)
        b[i] = a[i - NLEN_288_29];
#ifdef DEBUG_NORM
    b[DMPV_288_29] = a[MPV_288_29];
    b[DMNV_288_29] = a[MNV_288_29];
#endif
}

/* Copy bottom half of DBIG to BIG */
void BIG_288_29_sdcopy(BIG_288_29 b, DBIG_288_29 a)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++)
        b[i] = a[i];
#ifdef DEBUG_NORM
    b[MPV_288_29] = a[DMPV_288_29];
    b[MNV_288_29] = a[DMNV_288_29];
#endif
}

/* Copy top half of DBIG to BIG */
void BIG_288_29_sducopy(BIG_288_29 b, DBIG_288_29 a)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++)
        b[i] = a[i + NLEN_288_29];
#ifdef DEBUG_NORM
    b[MPV_288_29] = a[DMPV_288_29];
    b[MNV_288_29] = a[DMNV_288_29];

#endif
}

/* Set a=0 */
void BIG_288_29_zero(BIG_288_29 a)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[MPV_288_29] = a[MNV_288_29] = 0;
#endif
}

void BIG_288_29_dzero(DBIG_288_29 a)
{
    int i;
    for (i = 0; i < DNLEN_288_29; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[DMPV_288_29] = a[DMNV_288_29] = 0;
#endif
}

/* set a=1 */
void BIG_288_29_one(BIG_288_29 a)
{
    int i;
    a[0] = 1;
    for (i = 1; i < NLEN_288_29; i++)
        a[i] = 0;
#ifdef DEBUG_NORM
    a[MPV_288_29] = 1;
    a[MNV_288_29] = 0;
#endif
}



/* Set c=a+b */
/* SU= 8 */
void BIG_288_29_add(BIG_288_29 c, BIG_288_29 a, BIG_288_29 b)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++)
        c[i] = a[i] + b[i];
#ifdef DEBUG_NORM
    c[MPV_288_29] = a[MPV_288_29] + b[MPV_288_29];
    c[MNV_288_29] = a[MNV_288_29] + b[MNV_288_29];
    if (c[MPV_288_29] > NEXCESS_288_29)  printf("add problem - positive digit overflow %d\n", (int)c[MPV_288_29]);
    if (c[MNV_288_29] > NEXCESS_288_29)  printf("add problem - negative digit overflow %d\n", (int)c[MNV_288_29]);

#endif
}

/* Set c=a or b */
void BIG_288_29_or(BIG_288_29 c, BIG_288_29 a, BIG_288_29 b)
{
    int i;
    BIG_288_29_norm(a);
    BIG_288_29_norm(b);
    for (i = 0; i < NLEN_288_29; i++)
        c[i] = a[i] | b[i];
#ifdef DEBUG_NORM
    c[MPV_288_29] = 1;
    c[MNV_288_29] = 0;
#endif
}


/* Set c=c+d */
void BIG_288_29_inc(BIG_288_29 c, int d)
{
    BIG_288_29_norm(c);
    c[0] += (chunk)d;
#ifdef DEBUG_NORM
    c[MPV_288_29] += 1;
#endif
}

/* Set c=a-b */
/* SU= 8 */
void BIG_288_29_sub(BIG_288_29 c, BIG_288_29 a, BIG_288_29 b)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++)
        c[i] = a[i] - b[i];
#ifdef DEBUG_NORM
    c[MPV_288_29] = a[MPV_288_29] + b[MNV_288_29];
    c[MNV_288_29] = a[MNV_288_29] + b[MPV_288_29];
    if (c[MPV_288_29] > NEXCESS_288_29)  printf("sub problem - positive digit overflow %d\n", (int)c[MPV_288_29]);
    if (c[MNV_288_29] > NEXCESS_288_29)  printf("sub problem - negative digit overflow %d\n", (int)c[MNV_288_29]);

#endif
}

/* SU= 8 */

void BIG_288_29_dsub(DBIG_288_29 c, DBIG_288_29 a, DBIG_288_29 b)
{
    int i;
    for (i = 0; i < DNLEN_288_29; i++)
        c[i] = a[i] - b[i];
#ifdef DEBUG_NORM
    c[DMPV_288_29] = a[DMPV_288_29] + b[DMNV_288_29];
    c[DMNV_288_29] = a[DMNV_288_29] + b[DMPV_288_29];
    if (c[DMPV_288_29] > NEXCESS_288_29)  printf("double sub problem - positive digit overflow %d\n", (int)c[DMPV_288_29]);
    if (c[DMNV_288_29] > NEXCESS_288_29)  printf("double sub problem - negative digit overflow %d\n", (int)c[DMNV_288_29]);
#endif
}

void BIG_288_29_dadd(DBIG_288_29 c, DBIG_288_29 a, DBIG_288_29 b)
{
    int i;
    for (i = 0; i < DNLEN_288_29; i++)
        c[i] = a[i] + b[i];
#ifdef DEBUG_NORM
    c[DMPV_288_29] = a[DMPV_288_29] + b[DMNV_288_29];
    c[DMNV_288_29] = a[DMNV_288_29] + b[DMPV_288_29];
    if (c[DMPV_288_29] > NEXCESS_288_29)  printf("double add problem - positive digit overflow %d\n", (int)c[DMPV_288_29]);
    if (c[DMNV_288_29] > NEXCESS_288_29)  printf("double add problem - negative digit overflow %d\n", (int)c[DMNV_288_29]);
#endif
}

/* Set c=c-1 */
void BIG_288_29_dec(BIG_288_29 c, int d)
{
    BIG_288_29_norm(c);
    c[0] -= (chunk)d;
#ifdef DEBUG_NORM
    c[MNV_288_29] += 1;
#endif
}

/* multiplication r=a*c by c<=NEXCESS_288_29 */
void BIG_288_29_imul(BIG_288_29 r, BIG_288_29 a, int c)
{
    int i;
    for (i = 0; i < NLEN_288_29; i++) r[i] = a[i] * c;
#ifdef DEBUG_NORM
    r[MPV_288_29] = a[MPV_288_29] * c;
    r[MNV_288_29] = a[MNV_288_29] * c;
    if (r[MPV_288_29] > NEXCESS_288_29)  printf("int mul problem - positive digit overflow %d\n", (int)r[MPV_288_29]);
    if (r[MNV_288_29] > NEXCESS_288_29)  printf("int mul problem - negative digit overflow %d\n", (int)r[MNV_288_29]);

#endif
}

/* multiplication r=a*c by larger integer - c<=FEXCESS */
/* SU= 24 */
chunk BIG_288_29_pmul(BIG_288_29 r, BIG_288_29 a, int c)
{
    int i;
    chunk ak, carry = 0;
    for (i = 0; i < NLEN_288_29; i++)
    {
        ak = a[i];
        r[i] = 0;
        carry = muladd_288_29(ak, (chunk)c, carry, &r[i]);
    }
#ifdef DEBUG_NORM
    r[MPV_288_29] = 1;
    r[MNV_288_29] = 0;
#endif
    return carry;
}

/* r/=3 */
/* SU= 16 */
int BIG_288_29_div3(BIG_288_29 r)
{
    int i;
    chunk ak, base, carry = 0;
    BIG_288_29_norm(r);
    base = ((chunk)1 << BASEBITS_288_29);
    for (i = NLEN_288_29 - 1; i >= 0; i--)
    {
        ak = (carry * base + r[i]);
        r[i] = ak / 3;
        carry = ak % 3;
    }
    return (int)carry;
}

/* multiplication c=a*b by even larger integer b>FEXCESS, resulting in DBIG */
/* SU= 24 */
void BIG_288_29_pxmul(DBIG_288_29 c, BIG_288_29 a, int b)
{
    int j;
    chunk carry;
    BIG_288_29_dzero(c);
    carry = 0;
    for (j = 0; j < NLEN_288_29; j++)
        carry = muladd_288_29(a[j], (chunk)b, carry, &c[j]);
    c[NLEN_288_29] = carry;
#ifdef DEBUG_NORM
    c[DMPV_288_29] = 1;
    c[DMNV_288_29] = 0;
#endif
}

/* .. if you know the result will fit in a BIG, c must be distinct from a and b */
/* SU= 40 */
void BIG_288_29_smul(BIG_288_29 c, BIG_288_29 a, BIG_288_29 b)
{
    int i, j;
    chunk carry;

    BIG_288_29_zero(c);
    for (i = 0; i < NLEN_288_29; i++)
    {
        carry = 0;
        for (j = 0; j < NLEN_288_29; j++)
        {
            if (i + j < NLEN_288_29)
                carry = muladd_288_29(a[i], b[j], carry, &c[i + j]);
        }
    }
#ifdef DEBUG_NORM
    c[MPV_288_29] = 1;
    c[MNV_288_29] = 0;
#endif

}

/* Set c=a*b */
/* SU= 72 */
//void BIG_288_29_mul(chunk c[restrict DNLEN_288_29],chunk a[restrict NLEN_288_29],chunk b[restrict NLEN_288_29])
void BIG_288_29_mul(DBIG_288_29 c, BIG_288_29 a, BIG_288_29 b)
{
    int i;
#ifdef dchunk
    dchunk t, co;
    dchunk s;
    dchunk d[NLEN_288_29];
    int k;
#endif

#ifdef DEBUG_NORM
    if ((a[MPV_288_29] != 1 && a[MPV_288_29] != 0) || a[MNV_288_29] != 0) printf("First input to mul not normed\n");
    if ((b[MPV_288_29] != 1 && b[MPV_288_29] != 0) || b[MNV_288_29] != 0) printf("Second input to mul not normed\n");
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

	s=d[0];
	t = s; c[0]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29;
	s+=d[1]; t=co+s +(dchunk)(a[1]-a[0])*(b[0]-b[1]); c[1]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s+=d[2]; t=co+s +(dchunk)(a[2]-a[0])*(b[0]-b[2]); c[2]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s+=d[3]; t=co+s +(dchunk)(a[3]-a[0])*(b[0]-b[3])+(dchunk)(a[2]-a[1])*(b[1]-b[2]); c[3]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s+=d[4]; t=co+s +(dchunk)(a[4]-a[0])*(b[0]-b[4])+(dchunk)(a[3]-a[1])*(b[1]-b[3]); c[4]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s+=d[5]; t=co+s +(dchunk)(a[5]-a[0])*(b[0]-b[5])+(dchunk)(a[4]-a[1])*(b[1]-b[4])+(dchunk)(a[3]-a[2])*(b[2]-b[3]); c[5]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s+=d[6]; t=co+s +(dchunk)(a[6]-a[0])*(b[0]-b[6])+(dchunk)(a[5]-a[1])*(b[1]-b[5])+(dchunk)(a[4]-a[2])*(b[2]-b[4]); c[6]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s+=d[7]; t=co+s +(dchunk)(a[7]-a[0])*(b[0]-b[7])+(dchunk)(a[6]-a[1])*(b[1]-b[6])+(dchunk)(a[5]-a[2])*(b[2]-b[5])+(dchunk)(a[4]-a[3])*(b[3]-b[4]); c[7]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s+=d[8]; t=co+s +(dchunk)(a[8]-a[0])*(b[0]-b[8])+(dchunk)(a[7]-a[1])*(b[1]-b[7])+(dchunk)(a[6]-a[2])*(b[2]-b[6])+(dchunk)(a[5]-a[3])*(b[3]-b[5]); c[8]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s+=d[9]; t=co+s +(dchunk)(a[9]-a[0])*(b[0]-b[9])+(dchunk)(a[8]-a[1])*(b[1]-b[8])+(dchunk)(a[7]-a[2])*(b[2]-b[7])+(dchunk)(a[6]-a[3])*(b[3]-b[6])+(dchunk)(a[5]-a[4])*(b[4]-b[5]); c[9]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 

	s-=d[0]; t=co+s +(dchunk)(a[9]-a[1])*(b[1]-b[9])+(dchunk)(a[8]-a[2])*(b[2]-b[8])+(dchunk)(a[7]-a[3])*(b[3]-b[7])+(dchunk)(a[6]-a[4])*(b[4]-b[6]); c[10]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s-=d[1]; t=co+s +(dchunk)(a[9]-a[2])*(b[2]-b[9])+(dchunk)(a[8]-a[3])*(b[3]-b[8])+(dchunk)(a[7]-a[4])*(b[4]-b[7])+(dchunk)(a[6]-a[5])*(b[5]-b[6]); c[11]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s-=d[2]; t=co+s +(dchunk)(a[9]-a[3])*(b[3]-b[9])+(dchunk)(a[8]-a[4])*(b[4]-b[8])+(dchunk)(a[7]-a[5])*(b[5]-b[7]); c[12]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s-=d[3]; t=co+s +(dchunk)(a[9]-a[4])*(b[4]-b[9])+(dchunk)(a[8]-a[5])*(b[5]-b[8])+(dchunk)(a[7]-a[6])*(b[6]-b[7]); c[13]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s-=d[4]; t=co+s +(dchunk)(a[9]-a[5])*(b[5]-b[9])+(dchunk)(a[8]-a[6])*(b[6]-b[8]); c[14]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s-=d[5]; t=co+s +(dchunk)(a[9]-a[6])*(b[6]-b[9])+(dchunk)(a[8]-a[7])*(b[7]-b[8]); c[15]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s-=d[6]; t=co+s +(dchunk)(a[9]-a[7])*(b[7]-b[9]); c[16]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s-=d[7]; t=co+s +(dchunk)(a[9]-a[8])*(b[8]-b[9]); c[17]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	s-=d[8]; t=co+s ; c[18]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	c[19]=(chunk)co;


#else

    	t=(dchunk)a[0]*b[0]; c[0]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[1]+(dchunk)a[1]*b[0]; c[1]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[2]+(dchunk)a[1]*b[1]+(dchunk)a[2]*b[0]; c[2]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[3]+(dchunk)a[1]*b[2]+(dchunk)a[2]*b[1]+(dchunk)a[3]*b[0]; c[3]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[4]+(dchunk)a[1]*b[3]+(dchunk)a[2]*b[2]+(dchunk)a[3]*b[1]+(dchunk)a[4]*b[0]; c[4]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[5]+(dchunk)a[1]*b[4]+(dchunk)a[2]*b[3]+(dchunk)a[3]*b[2]+(dchunk)a[4]*b[1]+(dchunk)a[5]*b[0]; c[5]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[6]+(dchunk)a[1]*b[5]+(dchunk)a[2]*b[4]+(dchunk)a[3]*b[3]+(dchunk)a[4]*b[2]+(dchunk)a[5]*b[1]+(dchunk)a[6]*b[0]; c[6]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[7]+(dchunk)a[1]*b[6]+(dchunk)a[2]*b[5]+(dchunk)a[3]*b[4]+(dchunk)a[4]*b[3]+(dchunk)a[5]*b[2]+(dchunk)a[6]*b[1]+(dchunk)a[7]*b[0]; c[7]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[8]+(dchunk)a[1]*b[7]+(dchunk)a[2]*b[6]+(dchunk)a[3]*b[5]+(dchunk)a[4]*b[4]+(dchunk)a[5]*b[3]+(dchunk)a[6]*b[2]+(dchunk)a[7]*b[1]+(dchunk)a[8]*b[0]; c[8]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[0]*b[9]+(dchunk)a[1]*b[8]+(dchunk)a[2]*b[7]+(dchunk)a[3]*b[6]+(dchunk)a[4]*b[5]+(dchunk)a[5]*b[4]+(dchunk)a[6]*b[3]+(dchunk)a[7]*b[2]+(dchunk)a[8]*b[1]+(dchunk)a[9]*b[0]; c[9]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[1]*b[9]+(dchunk)a[2]*b[8]+(dchunk)a[3]*b[7]+(dchunk)a[4]*b[6]+(dchunk)a[5]*b[5]+(dchunk)a[6]*b[4]+(dchunk)a[7]*b[3]+(dchunk)a[8]*b[2]+(dchunk)a[9]*b[1]; c[10]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[2]*b[9]+(dchunk)a[3]*b[8]+(dchunk)a[4]*b[7]+(dchunk)a[5]*b[6]+(dchunk)a[6]*b[5]+(dchunk)a[7]*b[4]+(dchunk)a[8]*b[3]+(dchunk)a[9]*b[2]; c[11]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[3]*b[9]+(dchunk)a[4]*b[8]+(dchunk)a[5]*b[7]+(dchunk)a[6]*b[6]+(dchunk)a[7]*b[5]+(dchunk)a[8]*b[4]+(dchunk)a[9]*b[3]; c[12]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[4]*b[9]+(dchunk)a[5]*b[8]+(dchunk)a[6]*b[7]+(dchunk)a[7]*b[6]+(dchunk)a[8]*b[5]+(dchunk)a[9]*b[4]; c[13]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[5]*b[9]+(dchunk)a[6]*b[8]+(dchunk)a[7]*b[7]+(dchunk)a[8]*b[6]+(dchunk)a[9]*b[5]; c[14]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[6]*b[9]+(dchunk)a[7]*b[8]+(dchunk)a[8]*b[7]+(dchunk)a[9]*b[6]; c[15]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[7]*b[9]+(dchunk)a[8]*b[8]+(dchunk)a[9]*b[7]; c[16]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[8]*b[9]+(dchunk)a[9]*b[8]; c[17]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	t=t+(dchunk)a[9]*b[9]; c[18]=(chunk)t & BMASK_288_29; t=t>>BASEBITS_288_29;
	c[19]=(chunk)t;


#endif


#else

#ifndef USE_KARATSUBA

    t=(dchunk)a[0]*b[0];
    c[0]=(chunk)t & BMASK_288_29;
    t = t >> BASEBITS_288_29;
    for (i=1;i<NLEN_288_29;i++)
    {
        k=0; 
        while (k<=i) {t+=(dchunk)a[k]*b[i-k]; k++;}
        c[i]=(chunk)t & BMASK_288_29;
        t = t >> BASEBITS_288_29;
    }

    for (i=NLEN_288_29;i<2*NLEN_288_29-1;i++)
    {
        k=i-(NLEN_288_29-1);
        while (k<=NLEN_288_29-1) {t+=(dchunk)a[k]*b[i-k]; k++;}
        c[i]=(chunk)t & BMASK_288_29;
        t = t >> BASEBITS_288_29;
    }

    c[2 * NLEN_288_29 - 1] = (chunk)t;
#else

    for (i = 0; i < NLEN_288_29; i++)
        d[i] = (dchunk)a[i] * b[i];

    s = d[0];
    t = s;
    c[0] = (chunk)t & BMASK_288_29;
    co = t >> BASEBITS_288_29;

    for (k = 1; k < NLEN_288_29; k++)
    {
        s += d[k];
        t = co + s;
        
        /*for (i = k; i >= 1 + k / 2; i--) This causes a huge slow down! gcc/g++ optimizer problem (I think) */
        for (i=1+k/2;i<=k;i++) t += (dchunk)(a[i] - a[k - i]) * (b[k - i] - b[i]);
        c[k] = (chunk)t & BMASK_288_29;
        co = t >> BASEBITS_288_29;
    }
    for (k = NLEN_288_29; k < 2 * NLEN_288_29 - 1; k++)
    {
        s -= d[k - NLEN_288_29];
        t = co + s;
        for (i=1+k/2;i<NLEN_288_29;i++) t += (dchunk)(a[i] - a[k - i]) * (b[k - i] - b[i]);
        c[k] = (chunk)t & BMASK_288_29;
        co = t >> BASEBITS_288_29;
    }
    c[2 * NLEN_288_29 - 1] = (chunk)co;
#endif
#endif

#else
    int j;
    chunk carry;
    BIG_288_29_dzero(c);
    for (i = 0; i < NLEN_288_29; i++)
    {
        carry = 0;
        for (j = 0; j < NLEN_288_29; j++)
            carry = muladd_288_29(a[i], b[j], carry, &c[i + j]);

        c[NLEN_288_29 + i] = carry;
    }

#endif

#ifdef DEBUG_NORM
    c[DMPV_288_29] = 1;
    c[DMNV_288_29] = 0;
#endif
}

/* Set c=a*a */
/* SU= 80 */

//void BIG_288_29_sqr(chunk c[restrict DNLEN_288_29],chunk a[restrict NLEN_288_29])
void BIG_288_29_sqr(DBIG_288_29 c, BIG_288_29 a)
{
    int i, j;
#ifdef dchunk
    dchunk t, co;
#endif

#ifdef DEBUG_NORM
    if ((a[MPV_288_29] != 1 && a[MPV_288_29] != 0) || a[MNV_288_29] != 0) printf("Input to sqr not normed\n");
#endif
    /* Note 2*a[i] in loop below and extra addition */

#ifdef COMBA

#ifdef UNWOUND

    
	t=(dchunk)a[0]*a[0]; c[0]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29;
	t= +(dchunk)a[1]*a[0]; t+=t; t+=co; c[1]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[2]*a[0]; t+=t; t+=co; t+=(dchunk)a[1]*a[1]; c[2]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[3]*a[0]+(dchunk)a[2]*a[1]; t+=t; t+=co; c[3]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[4]*a[0]+(dchunk)a[3]*a[1]; t+=t; t+=co; t+=(dchunk)a[2]*a[2]; c[4]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[5]*a[0]+(dchunk)a[4]*a[1]+(dchunk)a[3]*a[2]; t+=t; t+=co; c[5]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[6]*a[0]+(dchunk)a[5]*a[1]+(dchunk)a[4]*a[2]; t+=t; t+=co; t+=(dchunk)a[3]*a[3]; c[6]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[7]*a[0]+(dchunk)a[6]*a[1]+(dchunk)a[5]*a[2]+(dchunk)a[4]*a[3]; t+=t; t+=co; c[7]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[8]*a[0]+(dchunk)a[7]*a[1]+(dchunk)a[6]*a[2]+(dchunk)a[5]*a[3]; t+=t; t+=co; t+=(dchunk)a[4]*a[4]; c[8]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[9]*a[0]+(dchunk)a[8]*a[1]+(dchunk)a[7]*a[2]+(dchunk)a[6]*a[3]+(dchunk)a[5]*a[4]; t+=t; t+=co; c[9]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 

	t= +(dchunk)a[9]*a[1]+(dchunk)a[8]*a[2]+(dchunk)a[7]*a[3]+(dchunk)a[6]*a[4]; t+=t; t+=co; t+=(dchunk)a[5]*a[5]; c[10]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[9]*a[2]+(dchunk)a[8]*a[3]+(dchunk)a[7]*a[4]+(dchunk)a[6]*a[5]; t+=t; t+=co; c[11]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[9]*a[3]+(dchunk)a[8]*a[4]+(dchunk)a[7]*a[5]; t+=t; t+=co; t+=(dchunk)a[6]*a[6]; c[12]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[9]*a[4]+(dchunk)a[8]*a[5]+(dchunk)a[7]*a[6]; t+=t; t+=co; c[13]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[9]*a[5]+(dchunk)a[8]*a[6]; t+=t; t+=co; t+=(dchunk)a[7]*a[7]; c[14]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[9]*a[6]+(dchunk)a[8]*a[7]; t+=t; t+=co; c[15]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[9]*a[7]; t+=t; t+=co; t+=(dchunk)a[8]*a[8]; c[16]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t= +(dchunk)a[9]*a[8]; t+=t; t+=co; c[17]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
	t=co; t+=(dchunk)a[9]*a[9]; c[18]=(chunk)t&BMASK_288_29; co=t>>BASEBITS_288_29; 
 	c[19]=(chunk)co;


#else


    t = (dchunk)a[0] * a[0];
    c[0] = (chunk)t & BMASK_288_29;
    co = t >> BASEBITS_288_29;

    for (j = 1; j < NLEN_288_29 - 1; )
    {
        t = (dchunk)a[j] * a[0];
        for (i = 1; i < (j + 1) / 2; i++)
        {
            t += (dchunk)a[j - i] * a[i];
        }
        t += t;
        t += co;
        c[j] = (chunk)t & BMASK_288_29;
        co = t >> BASEBITS_288_29;
        j++;
        t = (dchunk)a[j] * a[0];
        for (i = 1; i < (j + 1) / 2; i++)
        {
            t += (dchunk)a[j - i] * a[i];
        }
        t += t;
        t += co;
        t += (dchunk)a[j / 2] * a[j / 2];
        c[j] = (chunk)t & BMASK_288_29;
        co = t >> BASEBITS_288_29;
        j++;
    }

    for (j = NLEN_288_29 - 1 + NLEN_288_29 % 2; j < DNLEN_288_29 - 3; )
    {
        t = (dchunk)a[NLEN_288_29 - 1] * a[j - NLEN_288_29 + 1];
        for (i = j - NLEN_288_29 + 2; i < (j + 1) / 2; i++)
        {
            t += (dchunk)a[j - i] * a[i];
        }
        t += t;
        t += co;
        c[j] = (chunk)t & BMASK_288_29;
        co = t >> BASEBITS_288_29;
        j++;
        t = (dchunk)a[NLEN_288_29 - 1] * a[j - NLEN_288_29 + 1];
        for (i = j - NLEN_288_29 + 2; i < (j + 1) / 2; i++)
        {
            t += (dchunk)a[j - i] * a[i];
        }
        t += t;
        t += co;
        t += (dchunk)a[j / 2] * a[j / 2];
        c[j] = (chunk)t & BMASK_288_29;
        co = t >> BASEBITS_288_29;
        j++;
    }

    t = (dchunk)a[NLEN_288_29 - 2] * a[NLEN_288_29 - 1];
    t += t;
    t += co;
    c[DNLEN_288_29 - 3] = (chunk)t & BMASK_288_29;
    co = t >> BASEBITS_288_29;

    t = (dchunk)a[NLEN_288_29 - 1] * a[NLEN_288_29 - 1] + co;
    c[DNLEN_288_29 - 2] = (chunk)t & BMASK_288_29;
    co = t >> BASEBITS_288_29;
    c[DNLEN_288_29 - 1] = (chunk)co;


#endif

#else
    chunk carry;
    BIG_288_29_dzero(c);
    for (i = 0; i < NLEN_288_29; i++)
    {
        carry = 0;
        for (j = i + 1; j < NLEN_288_29; j++)
            carry = muladd_288_29(a[i], a[j], carry, &c[i + j]);
        c[NLEN_288_29 + i] = carry;
    }

    for (i = 0; i < DNLEN_288_29; i++) c[i] *= 2;

    for (i = 0; i < NLEN_288_29; i++)
        c[2 * i + 1] += muladd_288_29(a[i], a[i], 0, &c[2 * i]);

    BIG_288_29_dnorm(c);
#endif


#ifdef DEBUG_NORM
    c[DMPV_288_29] = 1;
    c[DMNV_288_29] = 0;
#endif

}

/* Montgomery reduction */
//void BIG_288_29_monty(chunk a[restrict NLEN_288_29], chunk md[restrict NLEN_288_29], chunk MC, chunk d[restrict DNLEN_288_29])
void BIG_288_29_monty(BIG_288_29 a, BIG_288_29 md, chunk MC, DBIG_288_29 d)
{
    int i, k;

#ifdef dchunk
    dchunk t, c, s;
    dchunk dd[NLEN_288_29];
    chunk v[NLEN_288_29];
#endif

#ifdef DEBUG_NORM
    if ((d[DMPV_288_29] != 1 && d[DMPV_288_29] != 0) || d[DMNV_288_29] != 0) printf("Input to redc not normed\n");
#endif

#ifdef COMBA

#ifdef UNWOUND

#ifdef USE_KARATSUBA

    	t=d[0]; v[0]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[0]*md[0];  s=0; c=(t>>BASEBITS_288_29);

	t=d[1]+c+s+(dchunk)v[0]*md[1]; v[1]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[1]*md[0];  dd[1]=(dchunk)v[1]*md[1]; s+=dd[1]; c=(t>>BASEBITS_288_29); 
	t=d[2]+c+s+(dchunk)v[0]*md[2]; v[2]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[2]*md[0];  dd[2]=(dchunk)v[2]*md[2]; s+=dd[2]; c=(t>>BASEBITS_288_29); 
	t=d[3]+c+s+(dchunk)v[0]*md[3]+(dchunk)(v[1]-v[2])*(md[2]-md[1]); v[3]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[3]*md[0];  dd[3]=(dchunk)v[3]*md[3]; s+=dd[3]; c=(t>>BASEBITS_288_29); 
	t=d[4]+c+s+(dchunk)v[0]*md[4]+(dchunk)(v[1]-v[3])*(md[3]-md[1]); v[4]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[4]*md[0];  dd[4]=(dchunk)v[4]*md[4]; s+=dd[4]; c=(t>>BASEBITS_288_29); 
	t=d[5]+c+s+(dchunk)v[0]*md[5]+(dchunk)(v[1]-v[4])*(md[4]-md[1])+(dchunk)(v[2]-v[3])*(md[3]-md[2]); v[5]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[5]*md[0];  dd[5]=(dchunk)v[5]*md[5]; s+=dd[5]; c=(t>>BASEBITS_288_29); 
	t=d[6]+c+s+(dchunk)v[0]*md[6]+(dchunk)(v[1]-v[5])*(md[5]-md[1])+(dchunk)(v[2]-v[4])*(md[4]-md[2]); v[6]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[6]*md[0];  dd[6]=(dchunk)v[6]*md[6]; s+=dd[6]; c=(t>>BASEBITS_288_29); 
	t=d[7]+c+s+(dchunk)v[0]*md[7]+(dchunk)(v[1]-v[6])*(md[6]-md[1])+(dchunk)(v[2]-v[5])*(md[5]-md[2])+(dchunk)(v[3]-v[4])*(md[4]-md[3]); v[7]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[7]*md[0];  dd[7]=(dchunk)v[7]*md[7]; s+=dd[7]; c=(t>>BASEBITS_288_29); 
	t=d[8]+c+s+(dchunk)v[0]*md[8]+(dchunk)(v[1]-v[7])*(md[7]-md[1])+(dchunk)(v[2]-v[6])*(md[6]-md[2])+(dchunk)(v[3]-v[5])*(md[5]-md[3]); v[8]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[8]*md[0];  dd[8]=(dchunk)v[8]*md[8]; s+=dd[8]; c=(t>>BASEBITS_288_29); 
	t=d[9]+c+s+(dchunk)v[0]*md[9]+(dchunk)(v[1]-v[8])*(md[8]-md[1])+(dchunk)(v[2]-v[7])*(md[7]-md[2])+(dchunk)(v[3]-v[6])*(md[6]-md[3])+(dchunk)(v[4]-v[5])*(md[5]-md[4]); v[9]=((chunk)t*MC)&BMASK_288_29; t+=(dchunk)v[9]*md[0];  dd[9]=(dchunk)v[9]*md[9]; s+=dd[9]; c=(t>>BASEBITS_288_29); 

	t=d[10]+c+s+(dchunk)(v[1]-v[9])*(md[9]-md[1])+(dchunk)(v[2]-v[8])*(md[8]-md[2])+(dchunk)(v[3]-v[7])*(md[7]-md[3])+(dchunk)(v[4]-v[6])*(md[6]-md[4]); a[0]=(chunk)t&BMASK_288_29;  s-=dd[1]; c=(t>>BASEBITS_288_29); 
	t=d[11]+c+s+(dchunk)(v[2]-v[9])*(md[9]-md[2])+(dchunk)(v[3]-v[8])*(md[8]-md[3])+(dchunk)(v[4]-v[7])*(md[7]-md[4])+(dchunk)(v[5]-v[6])*(md[6]-md[5]); a[1]=(chunk)t&BMASK_288_29;  s-=dd[2]; c=(t>>BASEBITS_288_29); 
	t=d[12]+c+s+(dchunk)(v[3]-v[9])*(md[9]-md[3])+(dchunk)(v[4]-v[8])*(md[8]-md[4])+(dchunk)(v[5]-v[7])*(md[7]-md[5]); a[2]=(chunk)t&BMASK_288_29;  s-=dd[3]; c=(t>>BASEBITS_288_29); 
	t=d[13]+c+s+(dchunk)(v[4]-v[9])*(md[9]-md[4])+(dchunk)(v[5]-v[8])*(md[8]-md[5])+(dchunk)(v[6]-v[7])*(md[7]-md[6]); a[3]=(chunk)t&BMASK_288_29;  s-=dd[4]; c=(t>>BASEBITS_288_29); 
	t=d[14]+c+s+(dchunk)(v[5]-v[9])*(md[9]-md[5])+(dchunk)(v[6]-v[8])*(md[8]-md[6]); a[4]=(chunk)t&BMASK_288_29;  s-=dd[5]; c=(t>>BASEBITS_288_29); 
	t=d[15]+c+s+(dchunk)(v[6]-v[9])*(md[9]-md[6])+(dchunk)(v[7]-v[8])*(md[8]-md[7]); a[5]=(chunk)t&BMASK_288_29;  s-=dd[6]; c=(t>>BASEBITS_288_29); 
	t=d[16]+c+s+(dchunk)(v[7]-v[9])*(md[9]-md[7]); a[6]=(chunk)t&BMASK_288_29;  s-=dd[7]; c=(t>>BASEBITS_288_29); 
	t=d[17]+c+s+(dchunk)(v[8]-v[9])*(md[9]-md[8]); a[7]=(chunk)t&BMASK_288_29;  s-=dd[8]; c=(t>>BASEBITS_288_29); 
	t=d[18]+c+s; a[8]=(chunk)t&BMASK_288_29;  s-=dd[9]; c=(t>>BASEBITS_288_29); 
	a[9]=d[19]+((chunk)c&BMASK_288_29);


#else

    	t = d[0];
	v[0] = ((chunk)t * MC)&BMASK_288_29;
	t += (dchunk)v[0] * md[0];
	t = (t >> BASEBITS_288_29) + d[1];
	t += (dchunk)v[0] * md[1] ; v[1] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[1] * md[0]; t = (t >> BASEBITS_288_29) + d[2];
	t += (dchunk)v[0] * md[2] + (dchunk)v[1]*md[1]; v[2] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[2] * md[0]; t = (t >> BASEBITS_288_29) + d[3];
	t += (dchunk)v[0] * md[3] + (dchunk)v[1]*md[2]+ (dchunk)v[2]*md[1]; v[3] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[3] * md[0]; t = (t >> BASEBITS_288_29) + d[4];
	t += (dchunk)v[0] * md[4] + (dchunk)v[1]*md[3]+ (dchunk)v[2]*md[2]+ (dchunk)v[3]*md[1]; v[4] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[4] * md[0]; t = (t >> BASEBITS_288_29) + d[5];
	t += (dchunk)v[0] * md[5] + (dchunk)v[1]*md[4]+ (dchunk)v[2]*md[3]+ (dchunk)v[3]*md[2]+ (dchunk)v[4]*md[1]; v[5] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[5] * md[0]; t = (t >> BASEBITS_288_29) + d[6];
	t += (dchunk)v[0] * md[6] + (dchunk)v[1]*md[5]+ (dchunk)v[2]*md[4]+ (dchunk)v[3]*md[3]+ (dchunk)v[4]*md[2]+ (dchunk)v[5]*md[1]; v[6] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[6] * md[0]; t = (t >> BASEBITS_288_29) + d[7];
	t += (dchunk)v[0] * md[7] + (dchunk)v[1]*md[6]+ (dchunk)v[2]*md[5]+ (dchunk)v[3]*md[4]+ (dchunk)v[4]*md[3]+ (dchunk)v[5]*md[2]+ (dchunk)v[6]*md[1]; v[7] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[7] * md[0]; t = (t >> BASEBITS_288_29) + d[8];
	t += (dchunk)v[0] * md[8] + (dchunk)v[1]*md[7]+ (dchunk)v[2]*md[6]+ (dchunk)v[3]*md[5]+ (dchunk)v[4]*md[4]+ (dchunk)v[5]*md[3]+ (dchunk)v[6]*md[2]+ (dchunk)v[7]*md[1]; v[8] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[8] * md[0]; t = (t >> BASEBITS_288_29) + d[9];
	t += (dchunk)v[0] * md[9] + (dchunk)v[1]*md[8]+ (dchunk)v[2]*md[7]+ (dchunk)v[3]*md[6]+ (dchunk)v[4]*md[5]+ (dchunk)v[5]*md[4]+ (dchunk)v[6]*md[3]+ (dchunk)v[7]*md[2]+ (dchunk)v[8]*md[1]; v[9] = ((chunk)t * MC)&BMASK_288_29; t += (dchunk)v[9] * md[0]; t = (t >> BASEBITS_288_29) + d[10];
	t=t + (dchunk)v[1]*md[9] + (dchunk)v[2]*md[8] + (dchunk)v[3]*md[7] + (dchunk)v[4]*md[6] + (dchunk)v[5]*md[5] + (dchunk)v[6]*md[4] + (dchunk)v[7]*md[3] + (dchunk)v[8]*md[2] + (dchunk)v[9]*md[1] ; a[0] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[11];
	t=t + (dchunk)v[2]*md[9] + (dchunk)v[3]*md[8] + (dchunk)v[4]*md[7] + (dchunk)v[5]*md[6] + (dchunk)v[6]*md[5] + (dchunk)v[7]*md[4] + (dchunk)v[8]*md[3] + (dchunk)v[9]*md[2] ; a[1] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[12];
	t=t + (dchunk)v[3]*md[9] + (dchunk)v[4]*md[8] + (dchunk)v[5]*md[7] + (dchunk)v[6]*md[6] + (dchunk)v[7]*md[5] + (dchunk)v[8]*md[4] + (dchunk)v[9]*md[3] ; a[2] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[13];
	t=t + (dchunk)v[4]*md[9] + (dchunk)v[5]*md[8] + (dchunk)v[6]*md[7] + (dchunk)v[7]*md[6] + (dchunk)v[8]*md[5] + (dchunk)v[9]*md[4] ; a[3] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[14];
	t=t + (dchunk)v[5]*md[9] + (dchunk)v[6]*md[8] + (dchunk)v[7]*md[7] + (dchunk)v[8]*md[6] + (dchunk)v[9]*md[5] ; a[4] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[15];
	t=t + (dchunk)v[6]*md[9] + (dchunk)v[7]*md[8] + (dchunk)v[8]*md[7] + (dchunk)v[9]*md[6] ; a[5] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[16];
	t=t + (dchunk)v[7]*md[9] + (dchunk)v[8]*md[8] + (dchunk)v[9]*md[7] ; a[6] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[17];
	t=t + (dchunk)v[8]*md[9] + (dchunk)v[9]*md[8] ; a[7] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[18];
	t=t + (dchunk)v[9]*md[9] ; a[8] = (chunk)t & BMASK_288_29; t = (t >> BASEBITS_288_29) + d[19];
	a[9] = (chunk)t & BMASK_288_29;


#endif


#else
#ifndef USE_KARATSUBA 
    t = d[0];
    v[0] = ((chunk)t * MC)&BMASK_288_29;
    t += (dchunk)v[0] * md[0];
    t = (t >> BASEBITS_288_29) + d[1];
   
    for (i = 1; i < NLEN_288_29; i++)
    {
        k=1;
        t += (dchunk)v[0] * md[i];
        while (k<i) {t += (dchunk)v[k]*md[i-k]; k++;}
        v[i] = ((chunk)t * MC)&BMASK_288_29;
        t += (dchunk)v[i] * md[0];
        t = (t >> BASEBITS_288_29) + d[i + 1];
    }
    for (i = NLEN_288_29; i < 2 * NLEN_288_29 - 1; i++)
    {
        k=i-(NLEN_288_29-1);
        while (k<=NLEN_288_29-1) {t += (dchunk)v[k]*md[i-k]; k++;}
        a[i - NLEN_288_29] = (chunk)t & BMASK_288_29;
        t = (t >> BASEBITS_288_29) + d[i + 1];
    }
    a[NLEN_288_29 - 1] = (chunk)t & BMASK_288_29;
#else
    t = d[0];
    v[0] = ((chunk)t * MC)&BMASK_288_29;
    t += (dchunk)v[0] * md[0];
    c = (t >> BASEBITS_288_29) + d[1];
    s = 0;

    for (k = 1; k < NLEN_288_29; k++)
    {
        t = c + s + (dchunk)v[0] * md[k];
        for (i=1+k/2;i<k;i++) t += (dchunk)(v[k - i] - v[i]) * (md[i] - md[k - i]);
        v[k] = ((chunk)t * MC)&BMASK_288_29;
        t += (dchunk)v[k] * md[0];
        c = (t >> BASEBITS_288_29) + d[k + 1];
        dd[k] = (dchunk)v[k] * md[k];
        s += dd[k];
    }
    for (k = NLEN_288_29; k < 2 * NLEN_288_29 - 1; k++)
    {
        t = c + s;
        for (i=1+k/2;i<NLEN_288_29;i++) t += (dchunk)(v[k - i] - v[i]) * (md[i] - md[k - i]);
        a[k - NLEN_288_29] = (chunk)t & BMASK_288_29;
        c = (t >> BASEBITS_288_29) + d[k + 1];
        s -= dd[k - NLEN_288_29 + 1];
    }
    a[NLEN_288_29 - 1] = (chunk)c & BMASK_288_29;

#endif
#endif


#else
    int j;
    chunk m, carry;
    for (i = 0; i < NLEN_288_29; i++)
    {
        if (MC == -1) m = (-d[i])&BMASK_288_29;
        else
        {
            if (MC == 1) m = d[i];
            else m = (MC * d[i])&BMASK_288_29;
        }
        carry = 0;
        for (j = 0; j < NLEN_288_29; j++)
            carry = muladd_288_29(m, md[j], carry, &d[i + j]);
        d[NLEN_288_29 + i] += carry;
    }
    BIG_288_29_sducopy(a, d);
    BIG_288_29_norm(a);

#endif

#ifdef DEBUG_NORM
    a[MPV_288_29] = 1;
    a[MNV_288_29] = 0;
#endif
}

/* General shift left of a by n bits */
/* a MUST be normalised */
/* SU= 32 */
void BIG_288_29_shl(BIG_288_29 a, int k)
{
    int i;
    int n = k % BASEBITS_288_29;
    int m = k / BASEBITS_288_29;

    a[NLEN_288_29 - 1] = ((a[NLEN_288_29 - 1 - m] << n));
    if (NLEN_288_29 >= m + 2) a[NLEN_288_29 - 1] |= (a[NLEN_288_29 - m - 2] >> (BASEBITS_288_29 - n));

    for (i = NLEN_288_29 - 2; i > m; i--)
        a[i] = ((a[i - m] << n)&BMASK_288_29) | (a[i - m - 1] >> (BASEBITS_288_29 - n));
    a[m] = (a[0] << n)&BMASK_288_29;
    for (i = 0; i < m; i++) a[i] = 0;

}

/* Fast shift left of a by n bits, where n less than a word, Return excess (but store it as well) */
/* a MUST be normalised */
/* SU= 16 */
int BIG_288_29_fshl(BIG_288_29 a, int n)
{
    int i;

    a[NLEN_288_29 - 1] = ((a[NLEN_288_29 - 1] << n)) | (a[NLEN_288_29 - 2] >> (BASEBITS_288_29 - n)); /* top word not masked */
    for (i = NLEN_288_29 - 2; i > 0; i--)
        a[i] = ((a[i] << n)&BMASK_288_29) | (a[i - 1] >> (BASEBITS_288_29 - n));
    a[0] = (a[0] << n)&BMASK_288_29;

    return (int)(a[NLEN_288_29 - 1] >> ((8 * MODBYTES_288_29) % BASEBITS_288_29)); /* return excess - only used in ff.c */
}

/* double length left shift of a by k bits - k can be > BASEBITS , a MUST be normalised */
/* SU= 32 */
void BIG_288_29_dshl(DBIG_288_29 a, int k)
{
    int i;
    int n = k % BASEBITS_288_29;
    int m = k / BASEBITS_288_29;

    a[DNLEN_288_29 - 1] = ((a[DNLEN_288_29 - 1 - m] << n)) | (a[DNLEN_288_29 - m - 2] >> (BASEBITS_288_29 - n));

    for (i = DNLEN_288_29 - 2; i > m; i--)
        a[i] = ((a[i - m] << n)&BMASK_288_29) | (a[i - m - 1] >> (BASEBITS_288_29 - n));
    a[m] = (a[0] << n)&BMASK_288_29;
    for (i = 0; i < m; i++) a[i] = 0;

}

/* General shift right of a by k bits */
/* a MUST be normalised */
/* SU= 32 */
void BIG_288_29_shr(BIG_288_29 a, int k)
{
    int i;
    int n = k % BASEBITS_288_29;
    int m = k / BASEBITS_288_29;
    for (i = 0; i < NLEN_288_29 - m - 1; i++)
        a[i] = (a[m + i] >> n) | ((a[m + i + 1] << (BASEBITS_288_29 - n))&BMASK_288_29);
    if (NLEN_288_29 > m)  a[NLEN_288_29 - m - 1] = a[NLEN_288_29 - 1] >> n;
    for (i = NLEN_288_29 - m; i < NLEN_288_29; i++) a[i] = 0;

}

/* Fast combined shift, subtract and norm. Return sign of result */
int BIG_288_29_ssn(BIG_288_29 r, BIG_288_29 a, BIG_288_29 m)
{
    int i, n = NLEN_288_29 - 1;
    chunk carry;
    m[0] = (m[0] >> 1) | ((m[1] << (BASEBITS_288_29 - 1))&BMASK_288_29);
    r[0] = a[0] - m[0];
    carry = r[0] >> BASEBITS_288_29;
    r[0] &= BMASK_288_29;

    for (i = 1; i < n; i++)
    {
        m[i] = (m[i] >> 1) | ((m[i + 1] << (BASEBITS_288_29 - 1))&BMASK_288_29);
        r[i] = a[i] - m[i] + carry;
        carry = r[i] >> BASEBITS_288_29;
        r[i] &= BMASK_288_29;
    }

    m[n] >>= 1;
    r[n] = a[n] - m[n] + carry;
#ifdef DEBUG_NORM
    r[MPV_288_29] = 1;
    r[MNV_288_29] = 0;
#endif
    return ((r[n] >> (CHUNK - 1)) & 1);
}

/* Faster shift right of a by k bits. Return shifted out part */
/* a MUST be normalised */
/* SU= 16 */
int BIG_288_29_fshr(BIG_288_29 a, int k)
{
    int i;
    chunk r = a[0] & (((chunk)1 << k) - 1); /* shifted out part */
    for (i = 0; i < NLEN_288_29 - 1; i++)
        a[i] = (a[i] >> k) | ((a[i + 1] << (BASEBITS_288_29 - k))&BMASK_288_29);
    a[NLEN_288_29 - 1] = a[NLEN_288_29 - 1] >> k;
    return (int)r;
}

/* double length right shift of a by k bits - can be > BASEBITS */
/* SU= 32 */
void BIG_288_29_dshr(DBIG_288_29 a, int k)
{
    int i;
    int n = k % BASEBITS_288_29;
    int m = k / BASEBITS_288_29;
    for (i = 0; i < DNLEN_288_29 - m - 1; i++)
        a[i] = (a[m + i] >> n) | ((a[m + i + 1] << (BASEBITS_288_29 - n))&BMASK_288_29);
    a[DNLEN_288_29 - m - 1] = a[DNLEN_288_29 - 1] >> n;
    for (i = DNLEN_288_29 - m; i < DNLEN_288_29; i++ ) a[i] = 0;
}

/* Split DBIG d into two BIGs t|b. Split happens at n bits, where n falls into NLEN word */
/* d MUST be normalised */
/* SU= 24 */
chunk BIG_288_29_split(BIG_288_29 t, BIG_288_29 b, DBIG_288_29 d, int n)
{
    int i;
    chunk nw, carry = 0;
    int m = n % BASEBITS_288_29;

    if (m == 0)
    {
        for (i = 0; i < NLEN_288_29; i++) b[i] = d[i];
        if (t != b)
        {
            for (i = NLEN_288_29; i < 2 * NLEN_288_29; i++) t[i - NLEN_288_29] = d[i];
            carry = t[NLEN_288_29 - 1] >> BASEBITS_288_29;
            t[NLEN_288_29 - 1] = t[NLEN_288_29 - 1] & BMASK_288_29; /* top word normalized */
        }
        return carry;
    }

    for (i = 0; i < NLEN_288_29 - 1; i++) b[i] = d[i];

    b[NLEN_288_29 - 1] = d[NLEN_288_29 - 1] & (((chunk)1 << m) - 1);

    if (t != b)
    {
        carry = (d[DNLEN_288_29 - 1] << (BASEBITS_288_29 - m));
        for (i = DNLEN_288_29 - 2; i >= NLEN_288_29 - 1; i--)
        {
            nw = (d[i] >> m) | carry;
            carry = (d[i] << (BASEBITS_288_29 - m))&BMASK_288_29;
            t[i - NLEN_288_29 + 1] = nw;
        }
    }
#ifdef DEBUG_NORM
    t[MPV_288_29] = 1;
    t[MNV_288_29] = 0;
    b[MPV_288_29] = 1;
    b[MNV_288_29] = 0;
#endif
    return carry;
}

/* you gotta keep the sign of carry! Look - no branching! */
/* Note that sign bit is needed to disambiguate between +ve and -ve values */
/* normalise BIG - force all digits < 2^BASEBITS */
chunk BIG_288_29_norm(BIG_288_29 a)
{
    int i;
    chunk d, carry;

    carry=a[0]>>BASEBITS_288_29;
    a[0]&=BMASK_288_29;

    for (i = 1; i < NLEN_288_29 - 1; i++)
    {
        d = a[i] + carry;
        a[i] = d & BMASK_288_29;
        carry = d >> BASEBITS_288_29;
    }
    a[NLEN_288_29 - 1] = (a[NLEN_288_29 - 1] + carry);

#ifdef DEBUG_NORM
    a[MPV_288_29] = 1;
    a[MNV_288_29] = 0;
#endif
    return (a[NLEN_288_29 - 1] >> ((8 * MODBYTES_288_29) % BASEBITS_288_29)); /* only used in ff.c */
}

void BIG_288_29_dnorm(DBIG_288_29 a)
{
    int i;
    chunk d, carry;

    carry=a[0]>>BASEBITS_288_29;
    a[0]&=BMASK_288_29;

    for (i = 1; i < DNLEN_288_29 - 1; i++)
    {
        d = a[i] + carry;
        a[i] = d & BMASK_288_29;
        carry = d >> BASEBITS_288_29;
    }
    a[DNLEN_288_29 - 1] = (a[DNLEN_288_29 - 1] + carry);
#ifdef DEBUG_NORM
    a[DMPV_288_29] = 1;
    a[DMNV_288_29] = 0;
#endif
}

/* Compare a and b. Return 1 for a>b, -1 for a<b, 0 for a==b */
/* a and b MUST be normalised before call */
/* sodium constant time implementation */

int BIG_288_29_comp(BIG_288_29 a, BIG_288_29 b)
{
    int i;
    chunk gt=0; chunk eq=1;
    for (i = NLEN_288_29-1; i>=0; i--)
    {
        gt |= ((b[i]-a[i]) >> BASEBITS_288_29) & eq;
        eq &= ((b[i]^a[i])-1) >> BASEBITS_288_29;
    }
    return (int)(gt+gt+eq-1);
}

int BIG_288_29_dcomp(DBIG_288_29 a, DBIG_288_29 b)
{
    int i;
    chunk gt=0; chunk eq=1;
    for (i = DNLEN_288_29-1; i>=0; i--)
    {
        gt |= ((b[i]-a[i]) >> BASEBITS_288_29) & eq;
        eq &= ((b[i]^a[i])-1) >> BASEBITS_288_29;
    }
    return (int)(gt+gt+eq-1);
}

/* return number of bits in a */
/* SU= 8 */
int BIG_288_29_nbits(BIG_288_29 a)
{
    int bts, k = NLEN_288_29 - 1;
    BIG_288_29 t;
    chunk c;
    BIG_288_29_copy(t, a);
    BIG_288_29_norm(t);
    while (k >= 0 && t[k] == 0) k--;
    if (k < 0) return 0;
    bts = BASEBITS_288_29 * k;
    c = t[k];
    while (c != 0)
    {
        c /= 2;
        bts++;
    }
    return bts;
}

/* SU= 8, Calculate number of bits in a DBIG - output normalised */
int BIG_288_29_dnbits(DBIG_288_29 a)
{
    int bts, k = DNLEN_288_29 - 1;
    DBIG_288_29 t;
    chunk c;
    BIG_288_29_dcopy(t, a);
    BIG_288_29_dnorm(t);
    while (k >= 0 && t[k] == 0) k--;
    if (k < 0) return 0;
    bts = BASEBITS_288_29 * k;
    c = t[k];
    while (c != 0)
    {
        c /= 2;
        bts++;
    }
    return bts;
}

void BIG_288_29_ctmod(BIG_288_29 b,BIG_288_29 m,int bd)
{
    int k=bd;
    BIG_288_29 r,c;
    BIG_288_29_copy(c,m);
    BIG_288_29_norm(b);

    BIG_288_29_shl(c,k);
    while (k>=0)
    {
        BIG_288_29_sub(r, b, c);
        BIG_288_29_norm(r);
        BIG_288_29_cmove(b, r, 1 - ((r[NLEN_288_29 - 1] >> (CHUNK - 1)) & 1));
        BIG_288_29_fshr(c, 1);
        k--;
    }
}

/* Set b=b mod c */
/* SU= 16 */
void BIG_288_29_mod(BIG_288_29 b, BIG_288_29 m)
{
    int k=BIG_288_29_nbits(b)-BIG_288_29_nbits(m);
    if (k<0) k=0;
    BIG_288_29_ctmod(b,m,k);
}

// Set a=b mod m in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m
void BIG_288_29_ctdmod(BIG_288_29 a, DBIG_288_29 b, BIG_288_29 m, int bd)
{
    int k=bd;
    DBIG_288_29 c,r;
    BIG_288_29_dscopy(c,m);
    BIG_288_29_dnorm(b);

    BIG_288_29_dshl(c,k);
    while (k>=0)
    {
        BIG_288_29_dsub(r, b, c);
        BIG_288_29_dnorm(r);
        BIG_288_29_dcmove(b, r, 1 - ((r[DNLEN_288_29 - 1] >> (CHUNK - 1)) & 1));
        BIG_288_29_dshr(c, 1);
        k--;
    }
    BIG_288_29_sdcopy(a,b);
}


/* Set a=b mod m, b is destroyed. Slow but rarely used. */
void BIG_288_29_dmod(BIG_288_29 a, DBIG_288_29 b, BIG_288_29 m)
{
    int k=BIG_288_29_dnbits(b)-BIG_288_29_nbits(m);
    if (k<0) k=0;
    BIG_288_29_ctdmod(a,b,m,k);
}

// a=b/m  in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m
void BIG_288_29_ctddiv(BIG_288_29 a,DBIG_288_29 b,BIG_288_29 m,int bd)
{
    int d,k=bd;
    DBIG_288_29 c,dr;
    BIG_288_29 e,r;
    BIG_288_29_dscopy(c,m);
    BIG_288_29_dnorm(b);

    BIG_288_29_zero(a);
    BIG_288_29_zero(e);
    BIG_288_29_inc(e, 1);

    BIG_288_29_shl(e,k);
    BIG_288_29_dshl(c,k);

    while (k >= 0)
    {
        BIG_288_29_dsub(dr, b, c);
        BIG_288_29_dnorm(dr);
        d = 1 - ((dr[DNLEN_288_29 - 1] >> (CHUNK - 1)) & 1);
        BIG_288_29_dcmove(b, dr, d);

        BIG_288_29_add(r, a, e);
        BIG_288_29_norm(r);
        BIG_288_29_cmove(a, r, d);

        BIG_288_29_dshr(c, 1);
        BIG_288_29_fshr(e, 1);
        k--;
    }
}

/* Set a=b/m,  b is destroyed. Slow but rarely used. */
void BIG_288_29_ddiv(BIG_288_29 a, DBIG_288_29 b, BIG_288_29 m)
{
    int k=BIG_288_29_dnbits(b)-BIG_288_29_nbits(m);
    if (k<0) k=0;
    BIG_288_29_ctddiv(a,b,m,k);
}

// a=a/m  in constant time (if bd is known at compile time)
// bd is Max number of bits in b - Actual number of bits in m
void BIG_288_29_ctsdiv(BIG_288_29 b,BIG_288_29 m,int bd)
{
    int d, k=bd;
    BIG_288_29 e,a,r,c;
    BIG_288_29_norm(b);
    BIG_288_29_copy(a,b);
    BIG_288_29_copy(c,m);
    BIG_288_29_zero(b);
    BIG_288_29_zero(e);
    BIG_288_29_inc(e, 1);

    BIG_288_29_shl(c,k);
    BIG_288_29_shl(e,k);

    while (k >= 0)
    {
        BIG_288_29_sub(r, a, c);
        BIG_288_29_norm(r);
        d = 1 - ((r[NLEN_288_29 - 1] >> (CHUNK - 1)) & 1);
        BIG_288_29_cmove(a, r, d);

        BIG_288_29_add(r, b, e);
        BIG_288_29_norm(r);
        BIG_288_29_cmove(b, r, d);

        BIG_288_29_fshr(c, 1);
        BIG_288_29_fshr(e, 1);

        k--;
    }
}

void BIG_288_29_sdiv(BIG_288_29 b, BIG_288_29 m)
{
    int k=BIG_288_29_nbits(b)-BIG_288_29_nbits(m);
    if (k<0) k=0;
    BIG_288_29_ctsdiv(b,m,k);
}

/* return LSB of a */
int BIG_288_29_parity(BIG_288_29 a)
{
    return a[0] % 2;
}

/* return n-th bit of a */
/* SU= 16 */
int BIG_288_29_bit(BIG_288_29 a, int n)
{
    return (int)((a[n / BASEBITS_288_29] & ((chunk)1 << (n % BASEBITS_288_29))) >> (n%BASEBITS_288_29));
//    if (a[n / BASEBITS_288_29] & ((chunk)1 << (n % BASEBITS_288_29))) return 1;
//    else return 0;
}

/* return last n bits of a, where n is small < BASEBITS */
/* SU= 16 */
int BIG_288_29_lastbits(BIG_288_29 a, int n)
{
    int msk = (1 << n) - 1;
    BIG_288_29_norm(a);
    return ((int)a[0])&msk;
}

/* get 8*MODBYTES size random number */
void BIG_288_29_random(BIG_288_29 m, csprng *rng)
{
    int i, b, j = 0, r = 0;
    int len = 8 * MODBYTES_288_29;

    BIG_288_29_zero(m);
    /* generate random BIG */
    for (i = 0; i < len; i++)
    {
        if (j == 0) r = RAND_byte(rng);
        else r >>= 1;
        b = r & 1;
        BIG_288_29_shl(m, 1);
        m[0] += b;
        j++;
        j &= 7;
    }

#ifdef DEBUG_NORM
    m[MPV_288_29] = 1;
    m[MNV_288_29] = 0;
#endif
}

/* get random BIG from rng, modulo q. Done one bit at a time, so its portable */

void BIG_288_29_randomnum(BIG_288_29 m, BIG_288_29 q, csprng *rng)
{
    int i, b, j = 0, r = 0;
    int n=2 * BIG_288_29_nbits(q);
    DBIG_288_29 d;
    BIG_288_29_dzero(d);
    /* generate random DBIG */
    for (i = 0; i < n; i++)
    {
        if (j == 0) r = RAND_byte(rng);
        else r >>= 1;
        b = r & 1;
        BIG_288_29_dshl(d, 1);
        d[0] += b;
        j++;
        j &= 7;
    }
    /* reduce modulo a BIG. Removes bias */
    BIG_288_29_dmod(m, d, q);
#ifdef DEBUG_NORM
    m[MPV_288_29] = 1;
    m[MNV_288_29] = 0;
#endif
}

/* create randum BIG less than r and less than trunc bits */
void BIG_288_29_randtrunc(BIG_288_29 s, BIG_288_29 r, int trunc, csprng *rng)
{
    BIG_288_29_randomnum(s, r, rng);
    if (BIG_288_29_nbits(r) > trunc)
        BIG_288_29_mod2m(s, trunc);
}


/* Set r=a*b mod m */
/* SU= 96 */
void BIG_288_29_modmul(BIG_288_29 r, BIG_288_29 a1, BIG_288_29 b1, BIG_288_29 m)
{
    DBIG_288_29 d;
    BIG_288_29 a, b;
    BIG_288_29_copy(a, a1);
    BIG_288_29_copy(b, b1);
    BIG_288_29_mod(a, m);
    BIG_288_29_mod(b, m);

    BIG_288_29_mul(d, a, b);
    BIG_288_29_ctdmod(r, d, m, BIG_288_29_nbits(m));
}

/* Set a=a*a mod m */
/* SU= 88 */
void BIG_288_29_modsqr(BIG_288_29 r, BIG_288_29 a1, BIG_288_29 m)
{
    DBIG_288_29 d;
    BIG_288_29 a;
    BIG_288_29_copy(a, a1);
    BIG_288_29_mod(a, m);
    BIG_288_29_sqr(d, a);
    BIG_288_29_ctdmod(r, d, m, BIG_288_29_nbits(m));
}

/* Set r=-a mod m */
/* SU= 16 */
void BIG_288_29_modneg(BIG_288_29 r, BIG_288_29 a1, BIG_288_29 m)
{
    BIG_288_29 a;
    BIG_288_29_copy(a, a1);
    BIG_288_29_mod(a, m);
    BIG_288_29_sub(r, m, a); BIG_288_29_norm(r);
}

/* Set r=a+b mod m */
void BIG_288_29_modadd(BIG_288_29 r, BIG_288_29 a1, BIG_288_29 b1, BIG_288_29 m)
{
    BIG_288_29 a, b;
    BIG_288_29_copy(a, a1);
    BIG_288_29_copy(b, b1);
    BIG_288_29_mod(a, m);
    BIG_288_29_mod(b, m);
    BIG_288_29_add(r,a,b); BIG_288_29_norm(r);
    BIG_288_29_ctmod(r,m,1);
}

/* Set a=a/b mod m */
/* SU= 136 */
void BIG_288_29_moddiv(BIG_288_29 r, BIG_288_29 a1, BIG_288_29 b1, BIG_288_29 m)
{
    DBIG_288_29 d;
    BIG_288_29 z;
    BIG_288_29 a, b;
    BIG_288_29_copy(a, a1);
    BIG_288_29_copy(b, b1);

    BIG_288_29_mod(a, m);
    BIG_288_29_invmodp(z, b, m);

    BIG_288_29_mul(d, a, z);
    BIG_288_29_ctdmod(r, d, m, BIG_288_29_nbits(m));
}

/* Get jacobi Symbol (a/p). Returns 0, 1 or -1 */
/* SU= 216 */
int BIG_288_29_jacobi(BIG_288_29 a, BIG_288_29 p)
{
    int n8, k, m = 0;
    BIG_288_29 t, x, n, zilch, one;
    BIG_288_29_one(one);
    BIG_288_29_zero(zilch);
    if (BIG_288_29_parity(p) == 0 || BIG_288_29_comp(a, zilch) == 0 || BIG_288_29_comp(p, one) <= 0) return 0;
    BIG_288_29_norm(a);
    BIG_288_29_copy(x, a);
    BIG_288_29_copy(n, p);
    BIG_288_29_mod(x, p);

    while (BIG_288_29_comp(n, one) > 0)
    {
        if (BIG_288_29_comp(x, zilch) == 0) return 0;
        n8 = BIG_288_29_lastbits(n, 3);
        k = 0;
        while (BIG_288_29_parity(x) == 0)
        {
            k++;
            BIG_288_29_shr(x, 1);
        }
        if (k % 2 == 1) m += (n8 * n8 - 1) / 8;
        m += (n8 - 1) * (BIG_288_29_lastbits(x, 2) - 1) / 4;
        BIG_288_29_copy(t, n);

        BIG_288_29_mod(t, x);
        BIG_288_29_copy(n, x);
        BIG_288_29_copy(x, t);
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
void BIG_288_29_invmod2m(BIG_288_29 a)
{
    int i;
    BIG_288_29 U, t1, b, c;
    BIG_288_29_zero(U);
    BIG_288_29_inc(U, invmod256(BIG_288_29_lastbits(a, 8)));
    for (i = 8; i < BIGBITS_288_29; i <<= 1)
    {
        BIG_288_29_norm(U);
        BIG_288_29_copy(b, a);
        BIG_288_29_mod2m(b, i);  // bottom i bits of a

        BIG_288_29_smul(t1, U, b);
        BIG_288_29_shr(t1, i); // top i bits of U*b

        BIG_288_29_copy(c, a);
        BIG_288_29_shr(c, i);
        BIG_288_29_mod2m(c, i); // top i bits of a

        BIG_288_29_smul(b, U, c);
        BIG_288_29_mod2m(b, i); // bottom i bits of U*c

        BIG_288_29_add(t1, t1, b);
        BIG_288_29_norm(t1);
        BIG_288_29_smul(b, t1, U);
        BIG_288_29_copy(t1, b); // (t1+b)*U
        BIG_288_29_mod2m(t1, i);               // bottom i bits of (t1+b)*U

        BIG_288_29_one(b);
        BIG_288_29_shl(b, i);
        BIG_288_29_sub(t1, b, t1);
        BIG_288_29_norm(t1);

        BIG_288_29_shl(t1, i);

        BIG_288_29_add(U, U, t1);
    }
    BIG_288_29_copy(a, U);
    BIG_288_29_norm(a);
    BIG_288_29_mod2m(a, BIGBITS_288_29);
}

/* Set r=1/a mod p. Binary method */
/* SU= 240 */
void BIG_288_29_invmodp(BIG_288_29 r, BIG_288_29 a, BIG_288_29 p)
{
    BIG_288_29 u, v, x1, x2, t, one;

    BIG_288_29_mod(a, p);
    if (BIG_288_29_iszilch(a))
    {
        BIG_288_29_zero(r);
        return;
    }

    BIG_288_29_copy(u, a);
    BIG_288_29_copy(v, p);
    BIG_288_29_one(one);
    BIG_288_29_copy(x1, one);
    BIG_288_29_zero(x2);

    while (BIG_288_29_comp(u, one) != 0 && BIG_288_29_comp(v, one) != 0)
    {
        while (BIG_288_29_parity(u) == 0)
        {
            BIG_288_29_fshr(u, 1);
            BIG_288_29_add(t,x1,p);
            BIG_288_29_cmove(x1,t,BIG_288_29_parity(x1));
            BIG_288_29_norm(x1);
            BIG_288_29_fshr(x1,1);
        }
        while (BIG_288_29_parity(v) == 0)
        {
            BIG_288_29_fshr(v, 1);
            BIG_288_29_add(t,x2,p);
            BIG_288_29_cmove(x2,t,BIG_288_29_parity(x2));
            BIG_288_29_norm(x2);
            BIG_288_29_fshr(x2,1);
        }
        if (BIG_288_29_comp(u, v) >= 0)
        {
            BIG_288_29_sub(u, u, v);
            BIG_288_29_norm(u);
            BIG_288_29_add(t,x1,p);
            BIG_288_29_cmove(x1,t,(BIG_288_29_comp(x1,x2)>>1)&1); /* move if x1<x2 */
            BIG_288_29_sub(x1,x1,x2);
            BIG_288_29_norm(x1);
        }
        else
        {
            BIG_288_29_sub(v, v, u);
            BIG_288_29_norm(v);
            BIG_288_29_add(t,x2,p);
            BIG_288_29_cmove(x2,t,(BIG_288_29_comp(x2,x1)>>1)&1); /* move if x2<x1 */
            BIG_288_29_sub(x2,x2,x1);
            BIG_288_29_norm(x2);
        }
    }
    BIG_288_29_copy(r,x1);
    BIG_288_29_cmove(r,x2,BIG_288_29_comp(u,one)&1);
}

/* set x = x mod 2^m */
void BIG_288_29_mod2m(BIG_288_29 x, int m)
{
    int i, wd, bt;
    chunk msk;
    BIG_288_29_norm(x);

    wd = m / BASEBITS_288_29;
    bt = m % BASEBITS_288_29;
    msk = ((chunk)1 << bt) - 1;
    x[wd] &= msk;
    for (i = wd + 1; i < NLEN_288_29; i++) x[i] = 0;
}

// new
/* Convert to DBIG number from byte array of given length */
void BIG_288_29_dfromBytesLen(DBIG_288_29 a, char *b, int s)
{
    int i, len = s;
    BIG_288_29_dzero(a);

    for (i = 0; i < len; i++)
    {
        BIG_288_29_dshl(a, 8);
        a[0] += (int)(unsigned char)b[i];
    }
#ifdef DEBUG_NORM
    a[DMPV_288_29] = 1;
    a[DMNV_288_29] = 0;
#endif
}

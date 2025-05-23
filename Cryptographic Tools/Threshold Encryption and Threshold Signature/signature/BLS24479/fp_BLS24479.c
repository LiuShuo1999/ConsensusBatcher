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

/* CORE mod p functions */
/* Small Finite Field arithmetic */
/* SU=m, SU is Stack Usage (NOT_SPECIAL Modulus) */

#include "fp_BLS24479.h"

/* Fast Modular Reduction Methods */

/* r=d mod m */
/* d MUST be normalised */
/* Products must be less than pR in all cases !!! */
/* So when multiplying two numbers, their product *must* be less than MODBITS+BASEBITS*NLEN */
/* Results *may* be one bit bigger than MODBITS */

#if MODTYPE_BLS24479 == PSEUDO_MERSENNE
/* r=d mod m */

/* Converts from BIG integer to residue form mod Modulus */
void FP_BLS24479_nres(FP_BLS24479 *y, BIG_480_29 x)
{
    BIG_480_29 mdls;
    BIG_480_29_rcopy(mdls, Modulus_BLS24479);
    BIG_480_29_copy(y->g, x);
    BIG_480_29_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_BLS24479_redc(BIG_480_29 x, FP_BLS24479 *y)
{
    BIG_480_29_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_BLS24479_mod(BIG_480_29 r, DBIG_480_29 d)
{
    BIG_480_29 t, b;
    chunk v, tw;
    BIG_480_29_split(t, b, d, MODBITS_BLS24479);

    /* Note that all of the excess gets pushed into t. So if squaring a value with a 4-bit excess, this results in
       t getting all 8 bits of the excess product! So products must be less than pR which is Montgomery compatible */

    if (MConst_BLS24479 < NEXCESS_480_29)
    {
        BIG_480_29_imul(t, t, MConst_BLS24479);
        BIG_480_29_norm(t);
        BIG_480_29_add(r, t, b);
        BIG_480_29_norm(r);
        tw = r[NLEN_480_29 - 1];
        r[NLEN_480_29 - 1] &= TMASK_BLS24479;
        r[0] += MConst_BLS24479 * ((tw >> TBITS_BLS24479));
    }
    else
    {
        v = BIG_480_29_pmul(t, t, MConst_BLS24479);
        BIG_480_29_add(r, t, b);
        BIG_480_29_norm(r);
        tw = r[NLEN_480_29 - 1];
        r[NLEN_480_29 - 1] &= TMASK_BLS24479;
#if CHUNK == 16
        r[1] += muladd_480_29(MConst_BLS24479, ((tw >> TBITS_BLS24479) + (v << (BASEBITS_480_29 - TBITS_BLS24479))), 0, &r[0]);
#else
        r[0] += MConst_BLS24479 * ((tw >> TBITS_BLS24479) + (v << (BASEBITS_480_29 - TBITS_BLS24479)));
#endif
    }
    BIG_480_29_norm(r);
}
#endif

/* This only applies to Curve C448, so specialised (for now) */
#if MODTYPE_BLS24479 == GENERALISED_MERSENNE

void FP_BLS24479_nres(FP_BLS24479 *y, BIG_480_29 x)
{
    BIG_480_29 mdls;
    BIG_480_29_rcopy(mdls, Modulus_BLS24479);
    BIG_480_29_copy(y->g, x);
    BIG_480_29_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_BLS24479_redc(BIG_480_29 x, FP_BLS24479 *y)
{
    BIG_480_29_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_BLS24479_mod(BIG_480_29 r, DBIG_480_29 d)
{
    BIG_480_29 t, b;
    chunk carry;
    BIG_480_29_split(t, b, d, MBITS_BLS24479);

    BIG_480_29_add(r, t, b);

    BIG_480_29_dscopy(d, t);
    BIG_480_29_dshl(d, MBITS_BLS24479 / 2);

    BIG_480_29_split(t, b, d, MBITS_BLS24479);

    BIG_480_29_add(r, r, t);
    BIG_480_29_add(r, r, b);
    BIG_480_29_norm(r);
    BIG_480_29_shl(t, MBITS_BLS24479 / 2);

    BIG_480_29_add(r, r, t);

    carry = r[NLEN_480_29 - 1] >> TBITS_BLS24479;

    r[NLEN_480_29 - 1] &= TMASK_BLS24479;
    r[0] += carry;

    r[224 / BASEBITS_480_29] += carry << (224 % BASEBITS_480_29); /* need to check that this falls mid-word */
    BIG_480_29_norm(r);
}

#endif

#if MODTYPE_BLS24479 == MONTGOMERY_FRIENDLY

/* convert to Montgomery n-residue form */
void FP_BLS24479_nres(FP_BLS24479 *y, BIG_480_29 x)
{
    DBIG_480_29 d;
    BIG_480_29 r;
    BIG_480_29_rcopy(r, R2modp_BLS24479);
    BIG_480_29_mul(d, x, r);
    FP_BLS24479_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_BLS24479_redc(BIG_480_29 x, FP_BLS24479 *y)
{
    DBIG_480_29 d;
    BIG_480_29_dzero(d);
    BIG_480_29_dscopy(d, y->g);
    FP_BLS24479_mod(x, d);
}

/* fast modular reduction from DBIG to BIG exploiting special form of the modulus */
void FP_BLS24479_mod(BIG_480_29 a, DBIG_480_29 d)
{
    int i;

    for (i = 0; i < NLEN_480_29; i++)
        d[NLEN_480_29 + i] += muladd_480_29(d[i], MConst_BLS24479 - 1, d[i], &d[NLEN_480_29 + i - 1]);

    BIG_480_29_sducopy(a, d);
    BIG_480_29_norm(a);
}

#endif

#if MODTYPE_BLS24479 == NOT_SPECIAL

/* convert to Montgomery n-residue form */
void FP_BLS24479_nres(FP_BLS24479 *y, BIG_480_29 x)
{
    DBIG_480_29 d;
    BIG_480_29 r;
    BIG_480_29_rcopy(r, R2modp_BLS24479);
    BIG_480_29_mul(d, x, r);
    FP_BLS24479_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_BLS24479_redc(BIG_480_29 x, FP_BLS24479 *y)
{
    DBIG_480_29 d;
    BIG_480_29_dzero(d);
    BIG_480_29_dscopy(d, y->g);
    FP_BLS24479_mod(x, d);
}


/* reduce a DBIG to a BIG using Montgomery's no trial division method */
/* d is expected to be dnormed before entry */
/* SU= 112 */
void FP_BLS24479_mod(BIG_480_29 a, DBIG_480_29 d)
{
    BIG_480_29 mdls;
    BIG_480_29_rcopy(mdls, Modulus_BLS24479);
    BIG_480_29_monty(a, mdls, MConst_BLS24479, d);
}

#endif

void FP_BLS24479_from_int(FP_BLS24479 *x,int a)
{
    BIG_480_29 w;
    if (a<0) BIG_480_29_rcopy(w, Modulus_BLS24479);
    else BIG_480_29_zero(w); 
    BIG_480_29_inc(w,a); BIG_480_29_norm(w); 
    FP_BLS24479_nres(x,w);
}

/* test x==0 ? */
/* SU= 48 */
int FP_BLS24479_iszilch(FP_BLS24479 *x)
{
    BIG_480_29 m;
    FP_BLS24479 y;
    FP_BLS24479_copy(&y,x);
    FP_BLS24479_reduce(&y);
    FP_BLS24479_redc(m,&y);
    return BIG_480_29_iszilch(m);
}

int FP_BLS24479_isunity(FP_BLS24479 *x)
{
    BIG_480_29 m;
    FP_BLS24479 y;
    FP_BLS24479_copy(&y,x);
    FP_BLS24479_reduce(&y);
    FP_BLS24479_redc(m,&y);
    return BIG_480_29_isunity(m);
}


void FP_BLS24479_copy(FP_BLS24479 *y, FP_BLS24479 *x)
{
    BIG_480_29_copy(y->g, x->g);
    y->XES = x->XES;
}

void FP_BLS24479_rcopy(FP_BLS24479 *y, const BIG_480_29 c)
{
    BIG_480_29 b;
    BIG_480_29_rcopy(b, c);
    FP_BLS24479_nres(y, b);
}

/* Swap a and b if d=1 */
void FP_BLS24479_cswap(FP_BLS24479 *a, FP_BLS24479 *b, int d)
{
    sign32 t, c = d;
    BIG_480_29_cswap(a->g, b->g, d);

    c = ~(c - 1);
    t = c & ((a->XES) ^ (b->XES));
    a->XES ^= t;
    b->XES ^= t;

}

/* Move b to a if d=1 */
void FP_BLS24479_cmove(FP_BLS24479 *a, FP_BLS24479 *b, int d)
{
    sign32 c = -d;

    BIG_480_29_cmove(a->g, b->g, d);
    a->XES ^= (a->XES ^ b->XES)&c;
}

void FP_BLS24479_zero(FP_BLS24479 *x)
{
    BIG_480_29_zero(x->g);
    x->XES = 1;
}

int FP_BLS24479_equals(FP_BLS24479 *x, FP_BLS24479 *y)
{
    FP_BLS24479 xg, yg;
    FP_BLS24479_copy(&xg, x);
    FP_BLS24479_copy(&yg, y);
    FP_BLS24479_reduce(&xg);
    FP_BLS24479_reduce(&yg);
    if (BIG_480_29_comp(xg.g, yg.g) == 0) return 1;
    return 0;
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP_BLS24479_islarger(FP_BLS24479 *x)
{
    BIG_480_29 p,fx,sx;
    if (FP_BLS24479_iszilch(x)) return 0;
    BIG_480_29_rcopy(p,Modulus_BLS24479);
    FP_BLS24479_redc(fx,x);
    BIG_480_29_sub(sx,p,fx);  BIG_480_29_norm(sx); 
    return BIG_480_29_comp(fx,sx);
}

void FP_BLS24479_toBytes(char *b,FP_BLS24479 *x)
{
    BIG_480_29 t;
    FP_BLS24479_redc(t, x);
    BIG_480_29_toBytes(b, t);
}

void FP_BLS24479_fromBytes(FP_BLS24479 *x,char *b)
{
    BIG_480_29 t;
    BIG_480_29_fromBytes(t, b);
    FP_BLS24479_nres(x, t);
}

/* output FP */
/* SU= 48 */
void FP_BLS24479_output(FP_BLS24479 *r)
{
    BIG_480_29 c;
    FP_BLS24479_reduce(r);
    FP_BLS24479_redc(c, r);
    BIG_480_29_output(c);
}

void FP_BLS24479_rawoutput(FP_BLS24479 *r)
{
    BIG_480_29_rawoutput(r->g);
}

#ifdef GET_STATS
int tsqr = 0, rsqr = 0, tmul = 0, rmul = 0;
int tadd = 0, radd = 0, tneg = 0, rneg = 0;
int tdadd = 0, rdadd = 0, tdneg = 0, rdneg = 0;
#endif

#ifdef FUSED_MODMUL

/* Insert fastest code here */

#endif

/* r=a*b mod Modulus */
/* product must be less that p.R - and we need to know this in advance! */
/* SU= 88 */
void FP_BLS24479_mul(FP_BLS24479 *r, FP_BLS24479 *a, FP_BLS24479 *b)
{
    DBIG_480_29 d;

    if ((sign64)a->XES * b->XES > (sign64)FEXCESS_BLS24479)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_BLS24479_reduce(a);  /* it is sufficient to fully reduce just one of them < p */
    }

#ifdef FUSED_MODMUL
    FP_BLS24479_modmul(r->g, a->g, b->g);
#else
    BIG_480_29_mul(d, a->g, b->g);
    FP_BLS24479_mod(r->g, d);
#endif
    r->XES = 2;
}


/* multiplication by an integer, r=a*c */
/* SU= 136 */
void FP_BLS24479_imul(FP_BLS24479 *r, FP_BLS24479 *a, int c)
{
    int s = 0;

    if (c < 0)
    {
        c = -c;
        s = 1;
    }

#if MODTYPE_BLS24479==PSEUDO_MERSENNE || MODTYPE_BLS24479==GENERALISED_MERSENNE
    DBIG_480_29 d;
    BIG_480_29_pxmul(d, a->g, c);
    FP_BLS24479_mod(r->g, d);
    r->XES = 2;

#else
    //Montgomery
    BIG_480_29 k;
    FP_BLS24479 f;
    if (a->XES * c <= FEXCESS_BLS24479)
    {
        BIG_480_29_pmul(r->g, a->g, c);
        r->XES = a->XES * c; // careful here - XES jumps!
    }
    else
    {
        // don't want to do this - only a problem for Montgomery modulus and larger constants
        BIG_480_29_zero(k);
        BIG_480_29_inc(k, c);
        BIG_480_29_norm(k);
        FP_BLS24479_nres(&f, k);
        FP_BLS24479_mul(r, a, &f);
    }
#endif

    if (s)
    {
        FP_BLS24479_neg(r, r);
        FP_BLS24479_norm(r);
    }
}

/* Set r=a^2 mod m */
/* SU= 88 */
void FP_BLS24479_sqr(FP_BLS24479 *r, FP_BLS24479 *a)
{
    DBIG_480_29 d;

    if ((sign64)a->XES * a->XES > (sign64)FEXCESS_BLS24479)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_BLS24479_reduce(a);
    }

    BIG_480_29_sqr(d, a->g);
    FP_BLS24479_mod(r->g, d);
    r->XES = 2;
}

/* SU= 16 */
/* Set r=a+b */
void FP_BLS24479_add(FP_BLS24479 *r, FP_BLS24479 *a, FP_BLS24479 *b)
{
    BIG_480_29_add(r->g, a->g, b->g);
    r->XES = a->XES + b->XES;
    if (r->XES > FEXCESS_BLS24479)
    {
#ifdef DEBUG_REDUCE
        printf("Sum too large - reducing it \n");
#endif
        FP_BLS24479_reduce(r);
    }
}

/* Set r=a-b mod m */
/* SU= 56 */
void FP_BLS24479_sub(FP_BLS24479 *r, FP_BLS24479 *a, FP_BLS24479 *b)
{
    FP_BLS24479 n;
    FP_BLS24479_neg(&n, b);
    FP_BLS24479_add(r, a, &n);
}

// https://graphics.stanford.edu/~seander/bithacks.html
// constant time log to base 2 (or number of bits in)

static int logb2(unsign32 v)
{
    int r;
    v |= v >> 1;
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;

    v = v - ((v >> 1) & 0x55555555);
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);
    r = (((v + (v >> 4)) & 0xF0F0F0F) * 0x1010101) >> 24;
    return r;
}

// find appoximation to quotient of a/m
// Out by at most 2.
// Note that MAXXES is bounded to be 2-bits less than half a word
static int quo(BIG_480_29 n, BIG_480_29 m)
{
    int sh;
    chunk num, den;
    int hb = CHUNK / 2;
    if (TBITS_BLS24479 < hb)
    {
        sh = hb - TBITS_BLS24479;
        num = (n[NLEN_480_29 - 1] << sh) | (n[NLEN_480_29 - 2] >> (BASEBITS_480_29 - sh));
        den = (m[NLEN_480_29 - 1] << sh) | (m[NLEN_480_29 - 2] >> (BASEBITS_480_29 - sh));
    }
    else
    {
        num = n[NLEN_480_29 - 1];
        den = m[NLEN_480_29 - 1];
    }
    return (int)(num / (den + 1));
}

/* SU= 48 */
/* Fully reduce a mod Modulus */
void FP_BLS24479_reduce(FP_BLS24479 *a)
{
    BIG_480_29 m, r;
    int sr, sb, q;
    chunk carry;

    BIG_480_29_rcopy(m, Modulus_BLS24479);

    BIG_480_29_norm(a->g);

    if (a->XES > 16)
    {
        q = quo(a->g, m);
        carry = BIG_480_29_pmul(r, m, q);
        r[NLEN_480_29 - 1] += (carry << BASEBITS_480_29); // correction - put any carry out back in again
        BIG_480_29_sub(a->g, a->g, r);
        BIG_480_29_norm(a->g);
        sb = 2;
    }
    else sb = logb2(a->XES - 1); // sb does not depend on the actual data

    BIG_480_29_fshl(m, sb);

    while (sb > 0)
    {
// constant time...
        sr = BIG_480_29_ssn(r, a->g, m); // optimized combined shift, subtract and norm
        BIG_480_29_cmove(a->g, r, 1 - sr);
        sb--;
    }

    //BIG_480_29_mod(a->g,m);
    a->XES = 1;
}

void FP_BLS24479_norm(FP_BLS24479 *x)
{
    BIG_480_29_norm(x->g);
}

/* Set r=-a mod Modulus */
/* SU= 64 */
void FP_BLS24479_neg(FP_BLS24479 *r, FP_BLS24479 *a)
{
    int sb;
    BIG_480_29 m;

    BIG_480_29_rcopy(m, Modulus_BLS24479);

    sb = logb2(a->XES - 1);
    BIG_480_29_fshl(m, sb);
    BIG_480_29_sub(r->g, m, a->g);
    r->XES = ((sign32)1 << sb) + 1;

    if (r->XES > FEXCESS_BLS24479)
    {
#ifdef DEBUG_REDUCE
        printf("Negation too large -  reducing it \n");
#endif
        FP_BLS24479_reduce(r);
    }

}

/* Set r=a/2. */
/* SU= 56 */
void FP_BLS24479_div2(FP_BLS24479 *r, FP_BLS24479 *a)
{
    BIG_480_29 m;
    BIG_480_29 w;
    BIG_480_29_rcopy(m, Modulus_BLS24479);
    int pr=BIG_480_29_parity(a->g);

    FP_BLS24479_copy(r, a);
    BIG_480_29_copy(w,r->g);
    BIG_480_29_fshr(r->g,1);
    BIG_480_29_add(w, w, m);
    BIG_480_29_norm(w);
    BIG_480_29_fshr(w, 1);   
    
    BIG_480_29_cmove(r->g,w,pr);

}

// Could leak size of b
// but not used here with secret exponent b
void FP_BLS24479_pow(FP_BLS24479 *r, FP_BLS24479 *a, BIG_480_29 b)
{
    sign8 w[1 + (NLEN_480_29 * BASEBITS_480_29 + 3) / 4];
    FP_BLS24479 tb[16];
    BIG_480_29 t;
    int i, nb;

    FP_BLS24479_copy(r,a);
    FP_BLS24479_norm(r);
    BIG_480_29_copy(t, b);
    BIG_480_29_norm(t);
    nb = 1 + (BIG_480_29_nbits(t) + 3) / 4;
    /* convert exponent to 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_480_29_lastbits(t, 4);
        BIG_480_29_dec(t, w[i]);
        BIG_480_29_norm(t);
        BIG_480_29_fshr(t, 4);
    }

    FP_BLS24479_one(&tb[0]);
    FP_BLS24479_copy(&tb[1], r);
    for (i = 2; i < 16; i++)
        FP_BLS24479_mul(&tb[i], &tb[i - 1], r);

    FP_BLS24479_copy(r, &tb[w[nb - 1]]);
    for (i = nb - 2; i >= 0; i--)
    {
        FP_BLS24479_sqr(r, r);
        FP_BLS24479_sqr(r, r);
        FP_BLS24479_sqr(r, r);
        FP_BLS24479_sqr(r, r);
        FP_BLS24479_mul(r, r, &tb[w[i]]);
    }
    FP_BLS24479_reduce(r);
}


#if MODTYPE_BLS24479 == PSEUDO_MERSENNE || MODTYPE_BLS24479==GENERALISED_MERSENNE

// See eprint paper https://eprint.iacr.org/2018/1038
// If p=3 mod 4 r= x^{(p-3)/4}, if p=5 mod 8 r=x^{(p-5)/8}

static void FP_BLS24479_fpow(FP_BLS24479 *r, FP_BLS24479 *x)
{
    int i, j, k, bw, w, c, nw, lo, m, n, nd, e=PM1D2_BLS24479;
    FP_BLS24479 xp[11], t, key;
    const int ac[] = {1, 2, 3, 6, 12, 15, 30, 60, 120, 240, 255};
// phase 1
    FP_BLS24479_copy(&xp[0], x); // 1
    FP_BLS24479_sqr(&xp[1], x); // 2
    FP_BLS24479_mul(&xp[2], &xp[1], x); //3
    FP_BLS24479_sqr(&xp[3], &xp[2]); // 6
    FP_BLS24479_sqr(&xp[4], &xp[3]); // 12
    FP_BLS24479_mul(&xp[5], &xp[4], &xp[2]); // 15
    FP_BLS24479_sqr(&xp[6], &xp[5]); // 30
    FP_BLS24479_sqr(&xp[7], &xp[6]); // 60
    FP_BLS24479_sqr(&xp[8], &xp[7]); // 120
    FP_BLS24479_sqr(&xp[9], &xp[8]); // 240
    FP_BLS24479_mul(&xp[10], &xp[9], &xp[5]); // 255

#if MODTYPE_BLS24479==PSEUDO_MERSENNE
    n = MODBITS_BLS24479;
#endif
#if MODTYPE_BLS24479==GENERALISED_MERSENNE  // Ed448 ONLY
    n = MODBITS_BLS24479 / 2;
#endif

    n-=(e+1);
    c=(MConst_BLS24479+(1<<e)+1)/(1<<(e+1));

// need c to be odd
    nd=0;
    while (c%2==0)
    {
        c/=2;
        n-=1;
        nd++;
    }

    bw = 0; w = 1; while (w < c) {w *= 2; bw += 1;}
    k = w - c;

    if (k != 0)
    {
        i = 10; while (ac[i] > k) i--;
        FP_BLS24479_copy(&key, &xp[i]);
        k -= ac[i];
    }
    while (k != 0)
    {
        i--;
        if (ac[i] > k) continue;
        FP_BLS24479_mul(&key, &key, &xp[i]);
        k -= ac[i];
    }

// phase 2
    FP_BLS24479_copy(&xp[1], &xp[2]);
    FP_BLS24479_copy(&xp[2], &xp[5]);
    FP_BLS24479_copy(&xp[3], &xp[10]);

    j = 3; m = 8;
    nw = n - bw;
    while (2 * m < nw)
    {
        FP_BLS24479_copy(&t, &xp[j++]);
        for (i = 0; i < m; i++)
            FP_BLS24479_sqr(&t, &t);
        FP_BLS24479_mul(&xp[j], &xp[j - 1], &t);
        m *= 2;
    }

    lo = nw - m;
    FP_BLS24479_copy(r, &xp[j]);

    while (lo != 0)
    {
        m /= 2; j--;
        if (lo < m) continue;
        lo -= m;
        FP_BLS24479_copy(&t, r);
        for (i = 0; i < m; i++)
            FP_BLS24479_sqr(&t, &t);
        FP_BLS24479_mul(r, &t, &xp[j]);
    }
// phase 3

    if (bw != 0)
    {
        for (i = 0; i < bw; i++ )
            FP_BLS24479_sqr(r, r);
        FP_BLS24479_mul(r, r, &key);
    }
#if MODTYPE_BLS24479==GENERALISED_MERSENNE  // Ed448 ONLY
    FP_BLS24479_copy(&key, r);
    FP_BLS24479_sqr(&t, &key);
    FP_BLS24479_mul(r, &t, &xp[0]);
    for (i = 0; i < n + 1; i++)
        FP_BLS24479_sqr(r, r);
    FP_BLS24479_mul(r, r, &key);
#endif

    for (i=0;i<nd;i++)
        FP_BLS24479_sqr(r,r);
}

#endif


// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
void FP_BLS24479_progen(FP_BLS24479 *r,FP_BLS24479 *x)
{
#if MODTYPE_BLS24479==PSEUDO_MERSENNE  || MODTYPE_BLS24479==GENERALISED_MERSENNE
    FP_BLS24479_fpow(r, x);  
#else
    int e=PM1D2_BLS24479;
    BIG_480_29 m;
    BIG_480_29_rcopy(m, Modulus_BLS24479);
    BIG_480_29_dec(m,1);
    BIG_480_29_shr(m,e);
    BIG_480_29_dec(m,1);
    BIG_480_29_fshr(m,1);
    FP_BLS24479_pow(r,x,m);
#endif
}

/* Is x a QR? return optional hint for fast follow-up square root */
int FP_BLS24479_qr(FP_BLS24479 *x,FP_BLS24479 *h)
{
    FP_BLS24479 r;
    int i,e=PM1D2_BLS24479;
    FP_BLS24479_progen(&r,x);
    if (h!=NULL)
        FP_BLS24479_copy(h,&r);

    FP_BLS24479_sqr(&r,&r);
    FP_BLS24479_mul(&r,x,&r);
    for (i=0;i<e-1;i++ )
        FP_BLS24479_sqr(&r,&r);


//    for (i=0;i<e;i++)
//        FP_BLS24479_sqr(&r,&r);
//    FP_BLS24479_copy(&s,x);
//    for (i=0;i<e-1;i++ )
//        FP_BLS24479_sqr(&s,&s);
//    FP_BLS24479_mul(&r,&r,&s);
    
    return FP_BLS24479_isunity(&r);
}

/* Modular inversion */
void FP_BLS24479_inv(FP_BLS24479 *r,FP_BLS24479 *x,FP_BLS24479 *h)
{
    int i,e=PM1D2_BLS24479;
    FP_BLS24479 s,t;
    FP_BLS24479_norm(x);
    FP_BLS24479_copy(&s,x);

    if (h==NULL)
        FP_BLS24479_progen(&t,x);
    else
        FP_BLS24479_copy(&t,h);

    for (i=0;i<e-1;i++)
    {  
        FP_BLS24479_sqr(&s,&s);
        FP_BLS24479_mul(&s,&s,x);
    }
  
    for (i=0;i<=e;i++)
        FP_BLS24479_sqr(&t,&t);
    
    FP_BLS24479_mul(r,&t,&s);
    FP_BLS24479_reduce(r);
}

// Tonelli-Shanks in constant time
void FP_BLS24479_sqrt(FP_BLS24479 *r, FP_BLS24479 *a, FP_BLS24479 *h)
{
    int i,j,k,u,e=PM1D2_BLS24479;
    FP_BLS24479 v,g,t,b;
    BIG_480_29 m;

    if (h==NULL)
        FP_BLS24479_progen(&g,a);
    else
        FP_BLS24479_copy(&g,h);

    BIG_480_29_rcopy(m,ROI_BLS24479);
    FP_BLS24479_nres(&v,m);

    FP_BLS24479_sqr(&t,&g);
    FP_BLS24479_mul(&t,&t,a);
   
    FP_BLS24479_mul(r,&g,a);
    FP_BLS24479_copy(&b,&t);
    for (k=e;k>1;k--)
    {
        for (j=1;j<k-1;j++)
            FP_BLS24479_sqr(&b,&b);
        u=1-FP_BLS24479_isunity(&b);
        FP_BLS24479_mul(&g,r,&v);
        FP_BLS24479_cmove(r,&g,u);
        FP_BLS24479_sqr(&v,&v);
        FP_BLS24479_mul(&g,&t,&v);
        FP_BLS24479_cmove(&t,&g,u);
        FP_BLS24479_copy(&b,&t);
    }
// always return +ve square root
    k=FP_BLS24479_sign(r);
    FP_BLS24479_neg(&v,r); FP_BLS24479_norm(&v);
    FP_BLS24479_cmove(r,&v,k);
}

// Calculate both inverse and square root of x, return QR
int FP_BLS24479_invsqrt(FP_BLS24479 *i, FP_BLS24479 *s, FP_BLS24479 *x)
{
    FP_BLS24479 h;
    int qr=FP_BLS24479_qr(x,&h);
    FP_BLS24479_sqrt(s,x,&h);
    FP_BLS24479_inv(i,x,&h);
    return qr;
}

// Two for Price of One - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
int FP_BLS24479_tpo(FP_BLS24479 *i, FP_BLS24479 *s)
{
    int qr;
    FP_BLS24479 w,t;
    FP_BLS24479_mul(&w,s,i);
    FP_BLS24479_mul(&t,&w,i);
    qr=FP_BLS24479_invsqrt(i,s,&t);
    FP_BLS24479_mul(i,i,&w);
    FP_BLS24479_mul(s,s,i);
    return qr;
}

/* SU=8 */
/* set n=1 */
void FP_BLS24479_one(FP_BLS24479 *n)
{
    BIG_480_29 b;
    BIG_480_29_one(b);
    FP_BLS24479_nres(n, b);
}

int FP_BLS24479_sign(FP_BLS24479 *x)
{
#ifdef BIG_ENDIAN_SIGN_BLS24479
    int cp;
    BIG_480_29 m,pm1d2;
    FP_BLS24479 y;
    BIG_480_29_rcopy(pm1d2, Modulus_BLS24479);
    BIG_480_29_dec(pm1d2,1);
    BIG_480_29_fshr(pm1d2,1); //(p-1)/2
     
    FP_BLS24479_copy(&y,x);
    FP_BLS24479_reduce(&y);
    FP_BLS24479_redc(m,&y);
    cp=BIG_480_29_comp(m,pm1d2);
    return ((cp+1)&2)>>1;

#else
    BIG_480_29 m;
    FP_BLS24479 y;
    FP_BLS24479_copy(&y,x);
    FP_BLS24479_reduce(&y);
    FP_BLS24479_redc(m,&y);
    return BIG_480_29_parity(m);
#endif
}

void FP_BLS24479_rand(FP_BLS24479 *x,csprng *rng)
{
    BIG_480_29 w,m;
    BIG_480_29_rcopy(m,Modulus_BLS24479);
    BIG_480_29_randomnum(w,m,rng);
    FP_BLS24479_nres(x,w);
}



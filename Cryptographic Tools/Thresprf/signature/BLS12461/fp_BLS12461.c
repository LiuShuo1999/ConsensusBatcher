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

#include "fp_BLS12461.h"

/* Fast Modular Reduction Methods */

/* r=d mod m */
/* d MUST be normalised */
/* Products must be less than pR in all cases !!! */
/* So when multiplying two numbers, their product *must* be less than MODBITS+BASEBITS*NLEN */
/* Results *may* be one bit bigger than MODBITS */

#if MODTYPE_BLS12461 == PSEUDO_MERSENNE
/* r=d mod m */

/* Converts from BIG integer to residue form mod Modulus */
void FP_BLS12461_nres(FP_BLS12461 *y, BIG_464_28 x)
{
    BIG_464_28 mdls;
    BIG_464_28_rcopy(mdls, Modulus_BLS12461);
    BIG_464_28_copy(y->g, x);
    BIG_464_28_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_BLS12461_redc(BIG_464_28 x, FP_BLS12461 *y)
{
    BIG_464_28_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_BLS12461_mod(BIG_464_28 r, DBIG_464_28 d)
{
    BIG_464_28 t, b;
    chunk v, tw;
    BIG_464_28_split(t, b, d, MODBITS_BLS12461);

    /* Note that all of the excess gets pushed into t. So if squaring a value with a 4-bit excess, this results in
       t getting all 8 bits of the excess product! So products must be less than pR which is Montgomery compatible */

    if (MConst_BLS12461 < NEXCESS_464_28)
    {
        BIG_464_28_imul(t, t, MConst_BLS12461);
        BIG_464_28_norm(t);
        BIG_464_28_add(r, t, b);
        BIG_464_28_norm(r);
        tw = r[NLEN_464_28 - 1];
        r[NLEN_464_28 - 1] &= TMASK_BLS12461;
        r[0] += MConst_BLS12461 * ((tw >> TBITS_BLS12461));
    }
    else
    {
        v = BIG_464_28_pmul(t, t, MConst_BLS12461);
        BIG_464_28_add(r, t, b);
        BIG_464_28_norm(r);
        tw = r[NLEN_464_28 - 1];
        r[NLEN_464_28 - 1] &= TMASK_BLS12461;
#if CHUNK == 16
        r[1] += muladd_464_28(MConst_BLS12461, ((tw >> TBITS_BLS12461) + (v << (BASEBITS_464_28 - TBITS_BLS12461))), 0, &r[0]);
#else
        r[0] += MConst_BLS12461 * ((tw >> TBITS_BLS12461) + (v << (BASEBITS_464_28 - TBITS_BLS12461)));
#endif
    }
    BIG_464_28_norm(r);
}
#endif

/* This only applies to Curve C448, so specialised (for now) */
#if MODTYPE_BLS12461 == GENERALISED_MERSENNE

void FP_BLS12461_nres(FP_BLS12461 *y, BIG_464_28 x)
{
    BIG_464_28 mdls;
    BIG_464_28_rcopy(mdls, Modulus_BLS12461);
    BIG_464_28_copy(y->g, x);
    BIG_464_28_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_BLS12461_redc(BIG_464_28 x, FP_BLS12461 *y)
{
    BIG_464_28_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_BLS12461_mod(BIG_464_28 r, DBIG_464_28 d)
{
    BIG_464_28 t, b;
    chunk carry;
    BIG_464_28_split(t, b, d, MBITS_BLS12461);

    BIG_464_28_add(r, t, b);

    BIG_464_28_dscopy(d, t);
    BIG_464_28_dshl(d, MBITS_BLS12461 / 2);

    BIG_464_28_split(t, b, d, MBITS_BLS12461);

    BIG_464_28_add(r, r, t);
    BIG_464_28_add(r, r, b);
    BIG_464_28_norm(r);
    BIG_464_28_shl(t, MBITS_BLS12461 / 2);

    BIG_464_28_add(r, r, t);

    carry = r[NLEN_464_28 - 1] >> TBITS_BLS12461;

    r[NLEN_464_28 - 1] &= TMASK_BLS12461;
    r[0] += carry;

    r[224 / BASEBITS_464_28] += carry << (224 % BASEBITS_464_28); /* need to check that this falls mid-word */
    BIG_464_28_norm(r);
}

#endif

#if MODTYPE_BLS12461 == MONTGOMERY_FRIENDLY

/* convert to Montgomery n-residue form */
void FP_BLS12461_nres(FP_BLS12461 *y, BIG_464_28 x)
{
    DBIG_464_28 d;
    BIG_464_28 r;
    BIG_464_28_rcopy(r, R2modp_BLS12461);
    BIG_464_28_mul(d, x, r);
    FP_BLS12461_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_BLS12461_redc(BIG_464_28 x, FP_BLS12461 *y)
{
    DBIG_464_28 d;
    BIG_464_28_dzero(d);
    BIG_464_28_dscopy(d, y->g);
    FP_BLS12461_mod(x, d);
}

/* fast modular reduction from DBIG to BIG exploiting special form of the modulus */
void FP_BLS12461_mod(BIG_464_28 a, DBIG_464_28 d)
{
    int i;

    for (i = 0; i < NLEN_464_28; i++)
        d[NLEN_464_28 + i] += muladd_464_28(d[i], MConst_BLS12461 - 1, d[i], &d[NLEN_464_28 + i - 1]);

    BIG_464_28_sducopy(a, d);
    BIG_464_28_norm(a);
}

#endif

#if MODTYPE_BLS12461 == NOT_SPECIAL

/* convert to Montgomery n-residue form */
void FP_BLS12461_nres(FP_BLS12461 *y, BIG_464_28 x)
{
    DBIG_464_28 d;
    BIG_464_28 r;
    BIG_464_28_rcopy(r, R2modp_BLS12461);
    BIG_464_28_mul(d, x, r);
    FP_BLS12461_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_BLS12461_redc(BIG_464_28 x, FP_BLS12461 *y)
{
    DBIG_464_28 d;
    BIG_464_28_dzero(d);
    BIG_464_28_dscopy(d, y->g);
    FP_BLS12461_mod(x, d);
}


/* reduce a DBIG to a BIG using Montgomery's no trial division method */
/* d is expected to be dnormed before entry */
/* SU= 112 */
void FP_BLS12461_mod(BIG_464_28 a, DBIG_464_28 d)
{
    BIG_464_28 mdls;
    BIG_464_28_rcopy(mdls, Modulus_BLS12461);
    BIG_464_28_monty(a, mdls, MConst_BLS12461, d);
}

#endif

void FP_BLS12461_from_int(FP_BLS12461 *x,int a)
{
    BIG_464_28 w;
    if (a<0) BIG_464_28_rcopy(w, Modulus_BLS12461);
    else BIG_464_28_zero(w); 
    BIG_464_28_inc(w,a); BIG_464_28_norm(w); 
    FP_BLS12461_nres(x,w);
}

/* test x==0 ? */
/* SU= 48 */
int FP_BLS12461_iszilch(FP_BLS12461 *x)
{
    BIG_464_28 m;
    FP_BLS12461 y;
    FP_BLS12461_copy(&y,x);
    FP_BLS12461_reduce(&y);
    FP_BLS12461_redc(m,&y);
    return BIG_464_28_iszilch(m);
}

int FP_BLS12461_isunity(FP_BLS12461 *x)
{
    BIG_464_28 m;
    FP_BLS12461 y;
    FP_BLS12461_copy(&y,x);
    FP_BLS12461_reduce(&y);
    FP_BLS12461_redc(m,&y);
    return BIG_464_28_isunity(m);
}


void FP_BLS12461_copy(FP_BLS12461 *y, FP_BLS12461 *x)
{
    BIG_464_28_copy(y->g, x->g);
    y->XES = x->XES;
}

void FP_BLS12461_rcopy(FP_BLS12461 *y, const BIG_464_28 c)
{
    BIG_464_28 b;
    BIG_464_28_rcopy(b, c);
    FP_BLS12461_nres(y, b);
}

/* Swap a and b if d=1 */
void FP_BLS12461_cswap(FP_BLS12461 *a, FP_BLS12461 *b, int d)
{
    sign32 t, c = d;
    BIG_464_28_cswap(a->g, b->g, d);

    c = ~(c - 1);
    t = c & ((a->XES) ^ (b->XES));
    a->XES ^= t;
    b->XES ^= t;

}

/* Move b to a if d=1 */
void FP_BLS12461_cmove(FP_BLS12461 *a, FP_BLS12461 *b, int d)
{
    sign32 c = -d;

    BIG_464_28_cmove(a->g, b->g, d);
    a->XES ^= (a->XES ^ b->XES)&c;
}

void FP_BLS12461_zero(FP_BLS12461 *x)
{
    BIG_464_28_zero(x->g);
    x->XES = 1;
}

int FP_BLS12461_equals(FP_BLS12461 *x, FP_BLS12461 *y)
{
    FP_BLS12461 xg, yg;
    FP_BLS12461_copy(&xg, x);
    FP_BLS12461_copy(&yg, y);
    FP_BLS12461_reduce(&xg);
    FP_BLS12461_reduce(&yg);
    if (BIG_464_28_comp(xg.g, yg.g) == 0) return 1;
    return 0;
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP_BLS12461_islarger(FP_BLS12461 *x)
{
    BIG_464_28 p,fx,sx;
    if (FP_BLS12461_iszilch(x)) return 0;
    BIG_464_28_rcopy(p,Modulus_BLS12461);
    FP_BLS12461_redc(fx,x);
    BIG_464_28_sub(sx,p,fx);  BIG_464_28_norm(sx); 
    return BIG_464_28_comp(fx,sx);
}

void FP_BLS12461_toBytes(char *b,FP_BLS12461 *x)
{
    BIG_464_28 t;
    FP_BLS12461_redc(t, x);
    BIG_464_28_toBytes(b, t);
}

void FP_BLS12461_fromBytes(FP_BLS12461 *x,char *b)
{
    BIG_464_28 t;
    BIG_464_28_fromBytes(t, b);
    FP_BLS12461_nres(x, t);
}

/* output FP */
/* SU= 48 */
void FP_BLS12461_output(FP_BLS12461 *r)
{
    BIG_464_28 c;
    FP_BLS12461_reduce(r);
    FP_BLS12461_redc(c, r);
    BIG_464_28_output(c);
}

void FP_BLS12461_rawoutput(FP_BLS12461 *r)
{
    BIG_464_28_rawoutput(r->g);
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
void FP_BLS12461_mul(FP_BLS12461 *r, FP_BLS12461 *a, FP_BLS12461 *b)
{
    DBIG_464_28 d;

    if ((sign64)a->XES * b->XES > (sign64)FEXCESS_BLS12461)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_BLS12461_reduce(a);  /* it is sufficient to fully reduce just one of them < p */
    }

#ifdef FUSED_MODMUL
    FP_BLS12461_modmul(r->g, a->g, b->g);
#else
    BIG_464_28_mul(d, a->g, b->g);
    FP_BLS12461_mod(r->g, d);
#endif
    r->XES = 2;
}


/* multiplication by an integer, r=a*c */
/* SU= 136 */
void FP_BLS12461_imul(FP_BLS12461 *r, FP_BLS12461 *a, int c)
{
    int s = 0;

    if (c < 0)
    {
        c = -c;
        s = 1;
    }

#if MODTYPE_BLS12461==PSEUDO_MERSENNE || MODTYPE_BLS12461==GENERALISED_MERSENNE
    DBIG_464_28 d;
    BIG_464_28_pxmul(d, a->g, c);
    FP_BLS12461_mod(r->g, d);
    r->XES = 2;

#else
    //Montgomery
    BIG_464_28 k;
    FP_BLS12461 f;
    if (a->XES * c <= FEXCESS_BLS12461)
    {
        BIG_464_28_pmul(r->g, a->g, c);
        r->XES = a->XES * c; // careful here - XES jumps!
    }
    else
    {
        // don't want to do this - only a problem for Montgomery modulus and larger constants
        BIG_464_28_zero(k);
        BIG_464_28_inc(k, c);
        BIG_464_28_norm(k);
        FP_BLS12461_nres(&f, k);
        FP_BLS12461_mul(r, a, &f);
    }
#endif

    if (s)
    {
        FP_BLS12461_neg(r, r);
        FP_BLS12461_norm(r);
    }
}

/* Set r=a^2 mod m */
/* SU= 88 */
void FP_BLS12461_sqr(FP_BLS12461 *r, FP_BLS12461 *a)
{
    DBIG_464_28 d;

    if ((sign64)a->XES * a->XES > (sign64)FEXCESS_BLS12461)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_BLS12461_reduce(a);
    }

    BIG_464_28_sqr(d, a->g);
    FP_BLS12461_mod(r->g, d);
    r->XES = 2;
}

/* SU= 16 */
/* Set r=a+b */
void FP_BLS12461_add(FP_BLS12461 *r, FP_BLS12461 *a, FP_BLS12461 *b)
{
    BIG_464_28_add(r->g, a->g, b->g);
    r->XES = a->XES + b->XES;
    if (r->XES > FEXCESS_BLS12461)
    {
#ifdef DEBUG_REDUCE
        printf("Sum too large - reducing it \n");
#endif
        FP_BLS12461_reduce(r);
    }
}

/* Set r=a-b mod m */
/* SU= 56 */
void FP_BLS12461_sub(FP_BLS12461 *r, FP_BLS12461 *a, FP_BLS12461 *b)
{
    FP_BLS12461 n;
    FP_BLS12461_neg(&n, b);
    FP_BLS12461_add(r, a, &n);
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
static int quo(BIG_464_28 n, BIG_464_28 m)
{
    int sh;
    chunk num, den;
    int hb = CHUNK / 2;
    if (TBITS_BLS12461 < hb)
    {
        sh = hb - TBITS_BLS12461;
        num = (n[NLEN_464_28 - 1] << sh) | (n[NLEN_464_28 - 2] >> (BASEBITS_464_28 - sh));
        den = (m[NLEN_464_28 - 1] << sh) | (m[NLEN_464_28 - 2] >> (BASEBITS_464_28 - sh));
    }
    else
    {
        num = n[NLEN_464_28 - 1];
        den = m[NLEN_464_28 - 1];
    }
    return (int)(num / (den + 1));
}

/* SU= 48 */
/* Fully reduce a mod Modulus */
void FP_BLS12461_reduce(FP_BLS12461 *a)
{
    BIG_464_28 m, r;
    int sr, sb, q;
    chunk carry;

    BIG_464_28_rcopy(m, Modulus_BLS12461);

    BIG_464_28_norm(a->g);

    if (a->XES > 16)
    {
        q = quo(a->g, m);
        carry = BIG_464_28_pmul(r, m, q);
        r[NLEN_464_28 - 1] += (carry << BASEBITS_464_28); // correction - put any carry out back in again
        BIG_464_28_sub(a->g, a->g, r);
        BIG_464_28_norm(a->g);
        sb = 2;
    }
    else sb = logb2(a->XES - 1); // sb does not depend on the actual data

    BIG_464_28_fshl(m, sb);

    while (sb > 0)
    {
// constant time...
        sr = BIG_464_28_ssn(r, a->g, m); // optimized combined shift, subtract and norm
        BIG_464_28_cmove(a->g, r, 1 - sr);
        sb--;
    }

    //BIG_464_28_mod(a->g,m);
    a->XES = 1;
}

void FP_BLS12461_norm(FP_BLS12461 *x)
{
    BIG_464_28_norm(x->g);
}

/* Set r=-a mod Modulus */
/* SU= 64 */
void FP_BLS12461_neg(FP_BLS12461 *r, FP_BLS12461 *a)
{
    int sb;
    BIG_464_28 m;

    BIG_464_28_rcopy(m, Modulus_BLS12461);

    sb = logb2(a->XES - 1);
    BIG_464_28_fshl(m, sb);
    BIG_464_28_sub(r->g, m, a->g);
    r->XES = ((sign32)1 << sb) + 1;

    if (r->XES > FEXCESS_BLS12461)
    {
#ifdef DEBUG_REDUCE
        printf("Negation too large -  reducing it \n");
#endif
        FP_BLS12461_reduce(r);
    }

}

/* Set r=a/2. */
/* SU= 56 */
void FP_BLS12461_div2(FP_BLS12461 *r, FP_BLS12461 *a)
{
    BIG_464_28 m;
    BIG_464_28 w;
    BIG_464_28_rcopy(m, Modulus_BLS12461);
    int pr=BIG_464_28_parity(a->g);

    FP_BLS12461_copy(r, a);
    BIG_464_28_copy(w,r->g);
    BIG_464_28_fshr(r->g,1);
    BIG_464_28_add(w, w, m);
    BIG_464_28_norm(w);
    BIG_464_28_fshr(w, 1);   
    
    BIG_464_28_cmove(r->g,w,pr);

}

// Could leak size of b
// but not used here with secret exponent b
void FP_BLS12461_pow(FP_BLS12461 *r, FP_BLS12461 *a, BIG_464_28 b)
{
    sign8 w[1 + (NLEN_464_28 * BASEBITS_464_28 + 3) / 4];
    FP_BLS12461 tb[16];
    BIG_464_28 t;
    int i, nb;

    FP_BLS12461_copy(r,a);
    FP_BLS12461_norm(r);
    BIG_464_28_copy(t, b);
    BIG_464_28_norm(t);
    nb = 1 + (BIG_464_28_nbits(t) + 3) / 4;
    /* convert exponent to 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_464_28_lastbits(t, 4);
        BIG_464_28_dec(t, w[i]);
        BIG_464_28_norm(t);
        BIG_464_28_fshr(t, 4);
    }

    FP_BLS12461_one(&tb[0]);
    FP_BLS12461_copy(&tb[1], r);
    for (i = 2; i < 16; i++)
        FP_BLS12461_mul(&tb[i], &tb[i - 1], r);

    FP_BLS12461_copy(r, &tb[w[nb - 1]]);
    for (i = nb - 2; i >= 0; i--)
    {
        FP_BLS12461_sqr(r, r);
        FP_BLS12461_sqr(r, r);
        FP_BLS12461_sqr(r, r);
        FP_BLS12461_sqr(r, r);
        FP_BLS12461_mul(r, r, &tb[w[i]]);
    }
    FP_BLS12461_reduce(r);
}


#if MODTYPE_BLS12461 == PSEUDO_MERSENNE || MODTYPE_BLS12461==GENERALISED_MERSENNE

// See eprint paper https://eprint.iacr.org/2018/1038
// If p=3 mod 4 r= x^{(p-3)/4}, if p=5 mod 8 r=x^{(p-5)/8}

static void FP_BLS12461_fpow(FP_BLS12461 *r, FP_BLS12461 *x)
{
    int i, j, k, bw, w, c, nw, lo, m, n, nd, e=PM1D2_BLS12461;
    FP_BLS12461 xp[11], t, key;
    const int ac[] = {1, 2, 3, 6, 12, 15, 30, 60, 120, 240, 255};
// phase 1
    FP_BLS12461_copy(&xp[0], x); // 1
    FP_BLS12461_sqr(&xp[1], x); // 2
    FP_BLS12461_mul(&xp[2], &xp[1], x); //3
    FP_BLS12461_sqr(&xp[3], &xp[2]); // 6
    FP_BLS12461_sqr(&xp[4], &xp[3]); // 12
    FP_BLS12461_mul(&xp[5], &xp[4], &xp[2]); // 15
    FP_BLS12461_sqr(&xp[6], &xp[5]); // 30
    FP_BLS12461_sqr(&xp[7], &xp[6]); // 60
    FP_BLS12461_sqr(&xp[8], &xp[7]); // 120
    FP_BLS12461_sqr(&xp[9], &xp[8]); // 240
    FP_BLS12461_mul(&xp[10], &xp[9], &xp[5]); // 255

#if MODTYPE_BLS12461==PSEUDO_MERSENNE
    n = MODBITS_BLS12461;
#endif
#if MODTYPE_BLS12461==GENERALISED_MERSENNE  // Ed448 ONLY
    n = MODBITS_BLS12461 / 2;
#endif

    n-=(e+1);
    c=(MConst_BLS12461+(1<<e)+1)/(1<<(e+1));

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
        FP_BLS12461_copy(&key, &xp[i]);
        k -= ac[i];
    }
    while (k != 0)
    {
        i--;
        if (ac[i] > k) continue;
        FP_BLS12461_mul(&key, &key, &xp[i]);
        k -= ac[i];
    }

// phase 2
    FP_BLS12461_copy(&xp[1], &xp[2]);
    FP_BLS12461_copy(&xp[2], &xp[5]);
    FP_BLS12461_copy(&xp[3], &xp[10]);

    j = 3; m = 8;
    nw = n - bw;
    while (2 * m < nw)
    {
        FP_BLS12461_copy(&t, &xp[j++]);
        for (i = 0; i < m; i++)
            FP_BLS12461_sqr(&t, &t);
        FP_BLS12461_mul(&xp[j], &xp[j - 1], &t);
        m *= 2;
    }

    lo = nw - m;
    FP_BLS12461_copy(r, &xp[j]);

    while (lo != 0)
    {
        m /= 2; j--;
        if (lo < m) continue;
        lo -= m;
        FP_BLS12461_copy(&t, r);
        for (i = 0; i < m; i++)
            FP_BLS12461_sqr(&t, &t);
        FP_BLS12461_mul(r, &t, &xp[j]);
    }
// phase 3

    if (bw != 0)
    {
        for (i = 0; i < bw; i++ )
            FP_BLS12461_sqr(r, r);
        FP_BLS12461_mul(r, r, &key);
    }
#if MODTYPE_BLS12461==GENERALISED_MERSENNE  // Ed448 ONLY
    FP_BLS12461_copy(&key, r);
    FP_BLS12461_sqr(&t, &key);
    FP_BLS12461_mul(r, &t, &xp[0]);
    for (i = 0; i < n + 1; i++)
        FP_BLS12461_sqr(r, r);
    FP_BLS12461_mul(r, r, &key);
#endif

    for (i=0;i<nd;i++)
        FP_BLS12461_sqr(r,r);
}

#endif


// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
void FP_BLS12461_progen(FP_BLS12461 *r,FP_BLS12461 *x)
{
#if MODTYPE_BLS12461==PSEUDO_MERSENNE  || MODTYPE_BLS12461==GENERALISED_MERSENNE
    FP_BLS12461_fpow(r, x);  
#else
    int e=PM1D2_BLS12461;
    BIG_464_28 m;
    BIG_464_28_rcopy(m, Modulus_BLS12461);
    BIG_464_28_dec(m,1);
    BIG_464_28_shr(m,e);
    BIG_464_28_dec(m,1);
    BIG_464_28_fshr(m,1);
    FP_BLS12461_pow(r,x,m);
#endif
}

/* Is x a QR? return optional hint for fast follow-up square root */
int FP_BLS12461_qr(FP_BLS12461 *x,FP_BLS12461 *h)
{
    FP_BLS12461 r;
    int i,e=PM1D2_BLS12461;
    FP_BLS12461_progen(&r,x);
    if (h!=NULL)
        FP_BLS12461_copy(h,&r);

    FP_BLS12461_sqr(&r,&r);
    FP_BLS12461_mul(&r,x,&r);
    for (i=0;i<e-1;i++ )
        FP_BLS12461_sqr(&r,&r);


//    for (i=0;i<e;i++)
//        FP_BLS12461_sqr(&r,&r);
//    FP_BLS12461_copy(&s,x);
//    for (i=0;i<e-1;i++ )
//        FP_BLS12461_sqr(&s,&s);
//    FP_BLS12461_mul(&r,&r,&s);
    
    return FP_BLS12461_isunity(&r);
}

/* Modular inversion */
void FP_BLS12461_inv(FP_BLS12461 *r,FP_BLS12461 *x,FP_BLS12461 *h)
{
    int i,e=PM1D2_BLS12461;
    FP_BLS12461 s,t;
    FP_BLS12461_norm(x);
    FP_BLS12461_copy(&s,x);

    if (h==NULL)
        FP_BLS12461_progen(&t,x);
    else
        FP_BLS12461_copy(&t,h);

    for (i=0;i<e-1;i++)
    {  
        FP_BLS12461_sqr(&s,&s);
        FP_BLS12461_mul(&s,&s,x);
    }
  
    for (i=0;i<=e;i++)
        FP_BLS12461_sqr(&t,&t);
    
    FP_BLS12461_mul(r,&t,&s);
    FP_BLS12461_reduce(r);
}

// Tonelli-Shanks in constant time
void FP_BLS12461_sqrt(FP_BLS12461 *r, FP_BLS12461 *a, FP_BLS12461 *h)
{
    int i,j,k,u,e=PM1D2_BLS12461;
    FP_BLS12461 v,g,t,b;
    BIG_464_28 m;

    if (h==NULL)
        FP_BLS12461_progen(&g,a);
    else
        FP_BLS12461_copy(&g,h);

    BIG_464_28_rcopy(m,ROI_BLS12461);
    FP_BLS12461_nres(&v,m);

    FP_BLS12461_sqr(&t,&g);
    FP_BLS12461_mul(&t,&t,a);
   
    FP_BLS12461_mul(r,&g,a);
    FP_BLS12461_copy(&b,&t);
    for (k=e;k>1;k--)
    {
        for (j=1;j<k-1;j++)
            FP_BLS12461_sqr(&b,&b);
        u=1-FP_BLS12461_isunity(&b);
        FP_BLS12461_mul(&g,r,&v);
        FP_BLS12461_cmove(r,&g,u);
        FP_BLS12461_sqr(&v,&v);
        FP_BLS12461_mul(&g,&t,&v);
        FP_BLS12461_cmove(&t,&g,u);
        FP_BLS12461_copy(&b,&t);
    }
// always return +ve square root
    k=FP_BLS12461_sign(r);
    FP_BLS12461_neg(&v,r); FP_BLS12461_norm(&v);
    FP_BLS12461_cmove(r,&v,k);
}

// Calculate both inverse and square root of x, return QR
int FP_BLS12461_invsqrt(FP_BLS12461 *i, FP_BLS12461 *s, FP_BLS12461 *x)
{
    FP_BLS12461 h;
    int qr=FP_BLS12461_qr(x,&h);
    FP_BLS12461_sqrt(s,x,&h);
    FP_BLS12461_inv(i,x,&h);
    return qr;
}

// Two for Price of One - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
int FP_BLS12461_tpo(FP_BLS12461 *i, FP_BLS12461 *s)
{
    int qr;
    FP_BLS12461 w,t;
    FP_BLS12461_mul(&w,s,i);
    FP_BLS12461_mul(&t,&w,i);
    qr=FP_BLS12461_invsqrt(i,s,&t);
    FP_BLS12461_mul(i,i,&w);
    FP_BLS12461_mul(s,s,i);
    return qr;
}

/* SU=8 */
/* set n=1 */
void FP_BLS12461_one(FP_BLS12461 *n)
{
    BIG_464_28 b;
    BIG_464_28_one(b);
    FP_BLS12461_nres(n, b);
}

int FP_BLS12461_sign(FP_BLS12461 *x)
{
#ifdef BIG_ENDIAN_SIGN_BLS12461
    int cp;
    BIG_464_28 m,pm1d2;
    FP_BLS12461 y;
    BIG_464_28_rcopy(pm1d2, Modulus_BLS12461);
    BIG_464_28_dec(pm1d2,1);
    BIG_464_28_fshr(pm1d2,1); //(p-1)/2
     
    FP_BLS12461_copy(&y,x);
    FP_BLS12461_reduce(&y);
    FP_BLS12461_redc(m,&y);
    cp=BIG_464_28_comp(m,pm1d2);
    return ((cp+1)&2)>>1;

#else
    BIG_464_28 m;
    FP_BLS12461 y;
    FP_BLS12461_copy(&y,x);
    FP_BLS12461_reduce(&y);
    FP_BLS12461_redc(m,&y);
    return BIG_464_28_parity(m);
#endif
}

void FP_BLS12461_rand(FP_BLS12461 *x,csprng *rng)
{
    BIG_464_28 w,m;
    BIG_464_28_rcopy(m,Modulus_BLS12461);
    BIG_464_28_randomnum(w,m,rng);
    FP_BLS12461_nres(x,w);
}



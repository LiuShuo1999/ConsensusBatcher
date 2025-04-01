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

#include "fp_FP512BN.h"

/* Fast Modular Reduction Methods */

/* r=d mod m */
/* d MUST be normalised */
/* Products must be less than pR in all cases !!! */
/* So when multiplying two numbers, their product *must* be less than MODBITS+BASEBITS*NLEN */
/* Results *may* be one bit bigger than MODBITS */

#if MODTYPE_FP512BN == PSEUDO_MERSENNE
/* r=d mod m */

/* Converts from BIG integer to residue form mod Modulus */
void FP_FP512BN_nres(FP_FP512BN *y, BIG_512_29 x)
{
    BIG_512_29 mdls;
    BIG_512_29_rcopy(mdls, Modulus_FP512BN);
    BIG_512_29_copy(y->g, x);
    BIG_512_29_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_FP512BN_redc(BIG_512_29 x, FP_FP512BN *y)
{
    BIG_512_29_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_FP512BN_mod(BIG_512_29 r, DBIG_512_29 d)
{
    BIG_512_29 t, b;
    chunk v, tw;
    BIG_512_29_split(t, b, d, MODBITS_FP512BN);

    /* Note that all of the excess gets pushed into t. So if squaring a value with a 4-bit excess, this results in
       t getting all 8 bits of the excess product! So products must be less than pR which is Montgomery compatible */

    if (MConst_FP512BN < NEXCESS_512_29)
    {
        BIG_512_29_imul(t, t, MConst_FP512BN);
        BIG_512_29_norm(t);
        BIG_512_29_add(r, t, b);
        BIG_512_29_norm(r);
        tw = r[NLEN_512_29 - 1];
        r[NLEN_512_29 - 1] &= TMASK_FP512BN;
        r[0] += MConst_FP512BN * ((tw >> TBITS_FP512BN));
    }
    else
    {
        v = BIG_512_29_pmul(t, t, MConst_FP512BN);
        BIG_512_29_add(r, t, b);
        BIG_512_29_norm(r);
        tw = r[NLEN_512_29 - 1];
        r[NLEN_512_29 - 1] &= TMASK_FP512BN;
#if CHUNK == 16
        r[1] += muladd_512_29(MConst_FP512BN, ((tw >> TBITS_FP512BN) + (v << (BASEBITS_512_29 - TBITS_FP512BN))), 0, &r[0]);
#else
        r[0] += MConst_FP512BN * ((tw >> TBITS_FP512BN) + (v << (BASEBITS_512_29 - TBITS_FP512BN)));
#endif
    }
    BIG_512_29_norm(r);
}
#endif

/* This only applies to Curve C448, so specialised (for now) */
#if MODTYPE_FP512BN == GENERALISED_MERSENNE

void FP_FP512BN_nres(FP_FP512BN *y, BIG_512_29 x)
{
    BIG_512_29 mdls;
    BIG_512_29_rcopy(mdls, Modulus_FP512BN);
    BIG_512_29_copy(y->g, x);
    BIG_512_29_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_FP512BN_redc(BIG_512_29 x, FP_FP512BN *y)
{
    BIG_512_29_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_FP512BN_mod(BIG_512_29 r, DBIG_512_29 d)
{
    BIG_512_29 t, b;
    chunk carry;
    BIG_512_29_split(t, b, d, MBITS_FP512BN);

    BIG_512_29_add(r, t, b);

    BIG_512_29_dscopy(d, t);
    BIG_512_29_dshl(d, MBITS_FP512BN / 2);

    BIG_512_29_split(t, b, d, MBITS_FP512BN);

    BIG_512_29_add(r, r, t);
    BIG_512_29_add(r, r, b);
    BIG_512_29_norm(r);
    BIG_512_29_shl(t, MBITS_FP512BN / 2);

    BIG_512_29_add(r, r, t);

    carry = r[NLEN_512_29 - 1] >> TBITS_FP512BN;

    r[NLEN_512_29 - 1] &= TMASK_FP512BN;
    r[0] += carry;

    r[224 / BASEBITS_512_29] += carry << (224 % BASEBITS_512_29); /* need to check that this falls mid-word */
    BIG_512_29_norm(r);
}

#endif

#if MODTYPE_FP512BN == MONTGOMERY_FRIENDLY

/* convert to Montgomery n-residue form */
void FP_FP512BN_nres(FP_FP512BN *y, BIG_512_29 x)
{
    DBIG_512_29 d;
    BIG_512_29 r;
    BIG_512_29_rcopy(r, R2modp_FP512BN);
    BIG_512_29_mul(d, x, r);
    FP_FP512BN_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_FP512BN_redc(BIG_512_29 x, FP_FP512BN *y)
{
    DBIG_512_29 d;
    BIG_512_29_dzero(d);
    BIG_512_29_dscopy(d, y->g);
    FP_FP512BN_mod(x, d);
}

/* fast modular reduction from DBIG to BIG exploiting special form of the modulus */
void FP_FP512BN_mod(BIG_512_29 a, DBIG_512_29 d)
{
    int i;

    for (i = 0; i < NLEN_512_29; i++)
        d[NLEN_512_29 + i] += muladd_512_29(d[i], MConst_FP512BN - 1, d[i], &d[NLEN_512_29 + i - 1]);

    BIG_512_29_sducopy(a, d);
    BIG_512_29_norm(a);
}

#endif

#if MODTYPE_FP512BN == NOT_SPECIAL

/* convert to Montgomery n-residue form */
void FP_FP512BN_nres(FP_FP512BN *y, BIG_512_29 x)
{
    DBIG_512_29 d;
    BIG_512_29 r;
    BIG_512_29_rcopy(r, R2modp_FP512BN);
    BIG_512_29_mul(d, x, r);
    FP_FP512BN_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_FP512BN_redc(BIG_512_29 x, FP_FP512BN *y)
{
    DBIG_512_29 d;
    BIG_512_29_dzero(d);
    BIG_512_29_dscopy(d, y->g);
    FP_FP512BN_mod(x, d);
}


/* reduce a DBIG to a BIG using Montgomery's no trial division method */
/* d is expected to be dnormed before entry */
/* SU= 112 */
void FP_FP512BN_mod(BIG_512_29 a, DBIG_512_29 d)
{
    BIG_512_29 mdls;
    BIG_512_29_rcopy(mdls, Modulus_FP512BN);
    BIG_512_29_monty(a, mdls, MConst_FP512BN, d);
}

#endif

void FP_FP512BN_from_int(FP_FP512BN *x,int a)
{
    BIG_512_29 w;
    if (a<0) BIG_512_29_rcopy(w, Modulus_FP512BN);
    else BIG_512_29_zero(w); 
    BIG_512_29_inc(w,a); BIG_512_29_norm(w); 
    FP_FP512BN_nres(x,w);
}

/* test x==0 ? */
/* SU= 48 */
int FP_FP512BN_iszilch(FP_FP512BN *x)
{
    BIG_512_29 m;
    FP_FP512BN y;
    FP_FP512BN_copy(&y,x);
    FP_FP512BN_reduce(&y);
    FP_FP512BN_redc(m,&y);
    return BIG_512_29_iszilch(m);
}

int FP_FP512BN_isunity(FP_FP512BN *x)
{
    BIG_512_29 m;
    FP_FP512BN y;
    FP_FP512BN_copy(&y,x);
    FP_FP512BN_reduce(&y);
    FP_FP512BN_redc(m,&y);
    return BIG_512_29_isunity(m);
}


void FP_FP512BN_copy(FP_FP512BN *y, FP_FP512BN *x)
{
    BIG_512_29_copy(y->g, x->g);
    y->XES = x->XES;
}

void FP_FP512BN_rcopy(FP_FP512BN *y, const BIG_512_29 c)
{
    BIG_512_29 b;
    BIG_512_29_rcopy(b, c);
    FP_FP512BN_nres(y, b);
}

/* Swap a and b if d=1 */
void FP_FP512BN_cswap(FP_FP512BN *a, FP_FP512BN *b, int d)
{
    sign32 t, c = d;
    BIG_512_29_cswap(a->g, b->g, d);

    c = ~(c - 1);
    t = c & ((a->XES) ^ (b->XES));
    a->XES ^= t;
    b->XES ^= t;

}

/* Move b to a if d=1 */
void FP_FP512BN_cmove(FP_FP512BN *a, FP_FP512BN *b, int d)
{
    sign32 c = -d;

    BIG_512_29_cmove(a->g, b->g, d);
    a->XES ^= (a->XES ^ b->XES)&c;
}

void FP_FP512BN_zero(FP_FP512BN *x)
{
    BIG_512_29_zero(x->g);
    x->XES = 1;
}

int FP_FP512BN_equals(FP_FP512BN *x, FP_FP512BN *y)
{
    FP_FP512BN xg, yg;
    FP_FP512BN_copy(&xg, x);
    FP_FP512BN_copy(&yg, y);
    FP_FP512BN_reduce(&xg);
    FP_FP512BN_reduce(&yg);
    if (BIG_512_29_comp(xg.g, yg.g) == 0) return 1;
    return 0;
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP_FP512BN_islarger(FP_FP512BN *x)
{
    BIG_512_29 p,fx,sx;
    if (FP_FP512BN_iszilch(x)) return 0;
    BIG_512_29_rcopy(p,Modulus_FP512BN);
    FP_FP512BN_redc(fx,x);
    BIG_512_29_sub(sx,p,fx);  BIG_512_29_norm(sx); 
    return BIG_512_29_comp(fx,sx);
}

void FP_FP512BN_toBytes(char *b,FP_FP512BN *x)
{
    BIG_512_29 t;
    FP_FP512BN_redc(t, x);
    BIG_512_29_toBytes(b, t);
}

void FP_FP512BN_fromBytes(FP_FP512BN *x,char *b)
{
    BIG_512_29 t;
    BIG_512_29_fromBytes(t, b);
    FP_FP512BN_nres(x, t);
}

/* output FP */
/* SU= 48 */
void FP_FP512BN_output(FP_FP512BN *r)
{
    BIG_512_29 c;
    FP_FP512BN_reduce(r);
    FP_FP512BN_redc(c, r);
    BIG_512_29_output(c);
}

void FP_FP512BN_rawoutput(FP_FP512BN *r)
{
    BIG_512_29_rawoutput(r->g);
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
void FP_FP512BN_mul(FP_FP512BN *r, FP_FP512BN *a, FP_FP512BN *b)
{
    DBIG_512_29 d;

    if ((sign64)a->XES * b->XES > (sign64)FEXCESS_FP512BN)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_FP512BN_reduce(a);  /* it is sufficient to fully reduce just one of them < p */
    }

#ifdef FUSED_MODMUL
    FP_FP512BN_modmul(r->g, a->g, b->g);
#else
    BIG_512_29_mul(d, a->g, b->g);
    FP_FP512BN_mod(r->g, d);
#endif
    r->XES = 2;
}


/* multiplication by an integer, r=a*c */
/* SU= 136 */
void FP_FP512BN_imul(FP_FP512BN *r, FP_FP512BN *a, int c)
{
    int s = 0;

    if (c < 0)
    {
        c = -c;
        s = 1;
    }

#if MODTYPE_FP512BN==PSEUDO_MERSENNE || MODTYPE_FP512BN==GENERALISED_MERSENNE
    DBIG_512_29 d;
    BIG_512_29_pxmul(d, a->g, c);
    FP_FP512BN_mod(r->g, d);
    r->XES = 2;

#else
    //Montgomery
    BIG_512_29 k;
    FP_FP512BN f;
    if (a->XES * c <= FEXCESS_FP512BN)
    {
        BIG_512_29_pmul(r->g, a->g, c);
        r->XES = a->XES * c; // careful here - XES jumps!
    }
    else
    {
        // don't want to do this - only a problem for Montgomery modulus and larger constants
        BIG_512_29_zero(k);
        BIG_512_29_inc(k, c);
        BIG_512_29_norm(k);
        FP_FP512BN_nres(&f, k);
        FP_FP512BN_mul(r, a, &f);
    }
#endif

    if (s)
    {
        FP_FP512BN_neg(r, r);
        FP_FP512BN_norm(r);
    }
}

/* Set r=a^2 mod m */
/* SU= 88 */
void FP_FP512BN_sqr(FP_FP512BN *r, FP_FP512BN *a)
{
    DBIG_512_29 d;

    if ((sign64)a->XES * a->XES > (sign64)FEXCESS_FP512BN)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_FP512BN_reduce(a);
    }

    BIG_512_29_sqr(d, a->g);
    FP_FP512BN_mod(r->g, d);
    r->XES = 2;
}

/* SU= 16 */
/* Set r=a+b */
void FP_FP512BN_add(FP_FP512BN *r, FP_FP512BN *a, FP_FP512BN *b)
{
    BIG_512_29_add(r->g, a->g, b->g);
    r->XES = a->XES + b->XES;
    if (r->XES > FEXCESS_FP512BN)
    {
#ifdef DEBUG_REDUCE
        printf("Sum too large - reducing it \n");
#endif
        FP_FP512BN_reduce(r);
    }
}

/* Set r=a-b mod m */
/* SU= 56 */
void FP_FP512BN_sub(FP_FP512BN *r, FP_FP512BN *a, FP_FP512BN *b)
{
    FP_FP512BN n;
    FP_FP512BN_neg(&n, b);
    FP_FP512BN_add(r, a, &n);
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
static int quo(BIG_512_29 n, BIG_512_29 m)
{
    int sh;
    chunk num, den;
    int hb = CHUNK / 2;
    if (TBITS_FP512BN < hb)
    {
        sh = hb - TBITS_FP512BN;
        num = (n[NLEN_512_29 - 1] << sh) | (n[NLEN_512_29 - 2] >> (BASEBITS_512_29 - sh));
        den = (m[NLEN_512_29 - 1] << sh) | (m[NLEN_512_29 - 2] >> (BASEBITS_512_29 - sh));
    }
    else
    {
        num = n[NLEN_512_29 - 1];
        den = m[NLEN_512_29 - 1];
    }
    return (int)(num / (den + 1));
}

/* SU= 48 */
/* Fully reduce a mod Modulus */
void FP_FP512BN_reduce(FP_FP512BN *a)
{
    BIG_512_29 m, r;
    int sr, sb, q;
    chunk carry;

    BIG_512_29_rcopy(m, Modulus_FP512BN);

    BIG_512_29_norm(a->g);

    if (a->XES > 16)
    {
        q = quo(a->g, m);
        carry = BIG_512_29_pmul(r, m, q);
        r[NLEN_512_29 - 1] += (carry << BASEBITS_512_29); // correction - put any carry out back in again
        BIG_512_29_sub(a->g, a->g, r);
        BIG_512_29_norm(a->g);
        sb = 2;
    }
    else sb = logb2(a->XES - 1); // sb does not depend on the actual data

    BIG_512_29_fshl(m, sb);

    while (sb > 0)
    {
// constant time...
        sr = BIG_512_29_ssn(r, a->g, m); // optimized combined shift, subtract and norm
        BIG_512_29_cmove(a->g, r, 1 - sr);
        sb--;
    }

    //BIG_512_29_mod(a->g,m);
    a->XES = 1;
}

void FP_FP512BN_norm(FP_FP512BN *x)
{
    BIG_512_29_norm(x->g);
}

/* Set r=-a mod Modulus */
/* SU= 64 */
void FP_FP512BN_neg(FP_FP512BN *r, FP_FP512BN *a)
{
    int sb;
    BIG_512_29 m;

    BIG_512_29_rcopy(m, Modulus_FP512BN);

    sb = logb2(a->XES - 1);
    BIG_512_29_fshl(m, sb);
    BIG_512_29_sub(r->g, m, a->g);
    r->XES = ((sign32)1 << sb) + 1;

    if (r->XES > FEXCESS_FP512BN)
    {
#ifdef DEBUG_REDUCE
        printf("Negation too large -  reducing it \n");
#endif
        FP_FP512BN_reduce(r);
    }

}

/* Set r=a/2. */
/* SU= 56 */
void FP_FP512BN_div2(FP_FP512BN *r, FP_FP512BN *a)
{
    BIG_512_29 m;
    BIG_512_29 w;
    BIG_512_29_rcopy(m, Modulus_FP512BN);
    int pr=BIG_512_29_parity(a->g);

    FP_FP512BN_copy(r, a);
    BIG_512_29_copy(w,r->g);
    BIG_512_29_fshr(r->g,1);
    BIG_512_29_add(w, w, m);
    BIG_512_29_norm(w);
    BIG_512_29_fshr(w, 1);   
    
    BIG_512_29_cmove(r->g,w,pr);

}

// Could leak size of b
// but not used here with secret exponent b
void FP_FP512BN_pow(FP_FP512BN *r, FP_FP512BN *a, BIG_512_29 b)
{
    sign8 w[1 + (NLEN_512_29 * BASEBITS_512_29 + 3) / 4];
    FP_FP512BN tb[16];
    BIG_512_29 t;
    int i, nb;

    FP_FP512BN_copy(r,a);
    FP_FP512BN_norm(r);
    BIG_512_29_copy(t, b);
    BIG_512_29_norm(t);
    nb = 1 + (BIG_512_29_nbits(t) + 3) / 4;
    /* convert exponent to 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_512_29_lastbits(t, 4);
        BIG_512_29_dec(t, w[i]);
        BIG_512_29_norm(t);
        BIG_512_29_fshr(t, 4);
    }

    FP_FP512BN_one(&tb[0]);
    FP_FP512BN_copy(&tb[1], r);
    for (i = 2; i < 16; i++)
        FP_FP512BN_mul(&tb[i], &tb[i - 1], r);

    FP_FP512BN_copy(r, &tb[w[nb - 1]]);
    for (i = nb - 2; i >= 0; i--)
    {
        FP_FP512BN_sqr(r, r);
        FP_FP512BN_sqr(r, r);
        FP_FP512BN_sqr(r, r);
        FP_FP512BN_sqr(r, r);
        FP_FP512BN_mul(r, r, &tb[w[i]]);
    }
    FP_FP512BN_reduce(r);
}


#if MODTYPE_FP512BN == PSEUDO_MERSENNE || MODTYPE_FP512BN==GENERALISED_MERSENNE

// See eprint paper https://eprint.iacr.org/2018/1038
// If p=3 mod 4 r= x^{(p-3)/4}, if p=5 mod 8 r=x^{(p-5)/8}

static void FP_FP512BN_fpow(FP_FP512BN *r, FP_FP512BN *x)
{
    int i, j, k, bw, w, c, nw, lo, m, n, nd, e=PM1D2_FP512BN;
    FP_FP512BN xp[11], t, key;
    const int ac[] = {1, 2, 3, 6, 12, 15, 30, 60, 120, 240, 255};
// phase 1
    FP_FP512BN_copy(&xp[0], x); // 1
    FP_FP512BN_sqr(&xp[1], x); // 2
    FP_FP512BN_mul(&xp[2], &xp[1], x); //3
    FP_FP512BN_sqr(&xp[3], &xp[2]); // 6
    FP_FP512BN_sqr(&xp[4], &xp[3]); // 12
    FP_FP512BN_mul(&xp[5], &xp[4], &xp[2]); // 15
    FP_FP512BN_sqr(&xp[6], &xp[5]); // 30
    FP_FP512BN_sqr(&xp[7], &xp[6]); // 60
    FP_FP512BN_sqr(&xp[8], &xp[7]); // 120
    FP_FP512BN_sqr(&xp[9], &xp[8]); // 240
    FP_FP512BN_mul(&xp[10], &xp[9], &xp[5]); // 255

#if MODTYPE_FP512BN==PSEUDO_MERSENNE
    n = MODBITS_FP512BN;
#endif
#if MODTYPE_FP512BN==GENERALISED_MERSENNE  // Ed448 ONLY
    n = MODBITS_FP512BN / 2;
#endif

    n-=(e+1);
    c=(MConst_FP512BN+(1<<e)+1)/(1<<(e+1));

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
        FP_FP512BN_copy(&key, &xp[i]);
        k -= ac[i];
    }
    while (k != 0)
    {
        i--;
        if (ac[i] > k) continue;
        FP_FP512BN_mul(&key, &key, &xp[i]);
        k -= ac[i];
    }

// phase 2
    FP_FP512BN_copy(&xp[1], &xp[2]);
    FP_FP512BN_copy(&xp[2], &xp[5]);
    FP_FP512BN_copy(&xp[3], &xp[10]);

    j = 3; m = 8;
    nw = n - bw;
    while (2 * m < nw)
    {
        FP_FP512BN_copy(&t, &xp[j++]);
        for (i = 0; i < m; i++)
            FP_FP512BN_sqr(&t, &t);
        FP_FP512BN_mul(&xp[j], &xp[j - 1], &t);
        m *= 2;
    }

    lo = nw - m;
    FP_FP512BN_copy(r, &xp[j]);

    while (lo != 0)
    {
        m /= 2; j--;
        if (lo < m) continue;
        lo -= m;
        FP_FP512BN_copy(&t, r);
        for (i = 0; i < m; i++)
            FP_FP512BN_sqr(&t, &t);
        FP_FP512BN_mul(r, &t, &xp[j]);
    }
// phase 3

    if (bw != 0)
    {
        for (i = 0; i < bw; i++ )
            FP_FP512BN_sqr(r, r);
        FP_FP512BN_mul(r, r, &key);
    }
#if MODTYPE_FP512BN==GENERALISED_MERSENNE  // Ed448 ONLY
    FP_FP512BN_copy(&key, r);
    FP_FP512BN_sqr(&t, &key);
    FP_FP512BN_mul(r, &t, &xp[0]);
    for (i = 0; i < n + 1; i++)
        FP_FP512BN_sqr(r, r);
    FP_FP512BN_mul(r, r, &key);
#endif

    for (i=0;i<nd;i++)
        FP_FP512BN_sqr(r,r);
}

#endif


// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
void FP_FP512BN_progen(FP_FP512BN *r,FP_FP512BN *x)
{
#if MODTYPE_FP512BN==PSEUDO_MERSENNE  || MODTYPE_FP512BN==GENERALISED_MERSENNE
    FP_FP512BN_fpow(r, x);  
#else
    int e=PM1D2_FP512BN;
    BIG_512_29 m;
    BIG_512_29_rcopy(m, Modulus_FP512BN);
    BIG_512_29_dec(m,1);
    BIG_512_29_shr(m,e);
    BIG_512_29_dec(m,1);
    BIG_512_29_fshr(m,1);
    FP_FP512BN_pow(r,x,m);
#endif
}

/* Is x a QR? return optional hint for fast follow-up square root */
int FP_FP512BN_qr(FP_FP512BN *x,FP_FP512BN *h)
{
    FP_FP512BN r;
    int i,e=PM1D2_FP512BN;
    FP_FP512BN_progen(&r,x);
    if (h!=NULL)
        FP_FP512BN_copy(h,&r);

    FP_FP512BN_sqr(&r,&r);
    FP_FP512BN_mul(&r,x,&r);
    for (i=0;i<e-1;i++ )
        FP_FP512BN_sqr(&r,&r);


//    for (i=0;i<e;i++)
//        FP_FP512BN_sqr(&r,&r);
//    FP_FP512BN_copy(&s,x);
//    for (i=0;i<e-1;i++ )
//        FP_FP512BN_sqr(&s,&s);
//    FP_FP512BN_mul(&r,&r,&s);
    
    return FP_FP512BN_isunity(&r);
}

/* Modular inversion */
void FP_FP512BN_inv(FP_FP512BN *r,FP_FP512BN *x,FP_FP512BN *h)
{
    int i,e=PM1D2_FP512BN;
    FP_FP512BN s,t;
    FP_FP512BN_norm(x);
    FP_FP512BN_copy(&s,x);

    if (h==NULL)
        FP_FP512BN_progen(&t,x);
    else
        FP_FP512BN_copy(&t,h);

    for (i=0;i<e-1;i++)
    {  
        FP_FP512BN_sqr(&s,&s);
        FP_FP512BN_mul(&s,&s,x);
    }
  
    for (i=0;i<=e;i++)
        FP_FP512BN_sqr(&t,&t);
    
    FP_FP512BN_mul(r,&t,&s);
    FP_FP512BN_reduce(r);
}

// Tonelli-Shanks in constant time
void FP_FP512BN_sqrt(FP_FP512BN *r, FP_FP512BN *a, FP_FP512BN *h)
{
    int i,j,k,u,e=PM1D2_FP512BN;
    FP_FP512BN v,g,t,b;
    BIG_512_29 m;

    if (h==NULL)
        FP_FP512BN_progen(&g,a);
    else
        FP_FP512BN_copy(&g,h);

    BIG_512_29_rcopy(m,ROI_FP512BN);
    FP_FP512BN_nres(&v,m);

    FP_FP512BN_sqr(&t,&g);
    FP_FP512BN_mul(&t,&t,a);
   
    FP_FP512BN_mul(r,&g,a);
    FP_FP512BN_copy(&b,&t);
    for (k=e;k>1;k--)
    {
        for (j=1;j<k-1;j++)
            FP_FP512BN_sqr(&b,&b);
        u=1-FP_FP512BN_isunity(&b);
        FP_FP512BN_mul(&g,r,&v);
        FP_FP512BN_cmove(r,&g,u);
        FP_FP512BN_sqr(&v,&v);
        FP_FP512BN_mul(&g,&t,&v);
        FP_FP512BN_cmove(&t,&g,u);
        FP_FP512BN_copy(&b,&t);
    }
// always return +ve square root
    k=FP_FP512BN_sign(r);
    FP_FP512BN_neg(&v,r); FP_FP512BN_norm(&v);
    FP_FP512BN_cmove(r,&v,k);
}

// Calculate both inverse and square root of x, return QR
int FP_FP512BN_invsqrt(FP_FP512BN *i, FP_FP512BN *s, FP_FP512BN *x)
{
    FP_FP512BN h;
    int qr=FP_FP512BN_qr(x,&h);
    FP_FP512BN_sqrt(s,x,&h);
    FP_FP512BN_inv(i,x,&h);
    return qr;
}

// Two for Price of One - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
int FP_FP512BN_tpo(FP_FP512BN *i, FP_FP512BN *s)
{
    int qr;
    FP_FP512BN w,t;
    FP_FP512BN_mul(&w,s,i);
    FP_FP512BN_mul(&t,&w,i);
    qr=FP_FP512BN_invsqrt(i,s,&t);
    FP_FP512BN_mul(i,i,&w);
    FP_FP512BN_mul(s,s,i);
    return qr;
}

/* SU=8 */
/* set n=1 */
void FP_FP512BN_one(FP_FP512BN *n)
{
    BIG_512_29 b;
    BIG_512_29_one(b);
    FP_FP512BN_nres(n, b);
}

int FP_FP512BN_sign(FP_FP512BN *x)
{
#ifdef BIG_ENDIAN_SIGN_FP512BN
    int cp;
    BIG_512_29 m,pm1d2;
    FP_FP512BN y;
    BIG_512_29_rcopy(pm1d2, Modulus_FP512BN);
    BIG_512_29_dec(pm1d2,1);
    BIG_512_29_fshr(pm1d2,1); //(p-1)/2
     
    FP_FP512BN_copy(&y,x);
    FP_FP512BN_reduce(&y);
    FP_FP512BN_redc(m,&y);
    cp=BIG_512_29_comp(m,pm1d2);
    return ((cp+1)&2)>>1;

#else
    BIG_512_29 m;
    FP_FP512BN y;
    FP_FP512BN_copy(&y,x);
    FP_FP512BN_reduce(&y);
    FP_FP512BN_redc(m,&y);
    return BIG_512_29_parity(m);
#endif
}

void FP_FP512BN_rand(FP_FP512BN *x,csprng *rng)
{
    BIG_512_29 w,m;
    BIG_512_29_rcopy(m,Modulus_FP512BN);
    BIG_512_29_randomnum(w,m,rng);
    FP_FP512BN_nres(x,w);
}



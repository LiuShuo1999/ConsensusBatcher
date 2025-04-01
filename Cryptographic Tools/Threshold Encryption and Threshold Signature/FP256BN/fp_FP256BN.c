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

#include "fp_FP256BN.h"

/* Fast Modular Reduction Methods */

/* r=d mod m */
/* d MUST be normalised */
/* Products must be less than pR in all cases !!! */
/* So when multiplying two numbers, their product *must* be less than MODBITS+BASEBITS*NLEN */
/* Results *may* be one bit bigger than MODBITS */

#if MODTYPE_FP256BN == PSEUDO_MERSENNE
/* r=d mod m */

/* Converts from BIG integer to residue form mod Modulus */
void FP_FP256BN_nres(FP_FP256BN *y, BIG_256_28 x)
{
    BIG_256_28 mdls;
    BIG_256_28_rcopy(mdls, Modulus_FP256BN);
    BIG_256_28_copy(y->g, x);
    BIG_256_28_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_FP256BN_redc(BIG_256_28 x, FP_FP256BN *y)
{
    BIG_256_28_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_FP256BN_mod(BIG_256_28 r, DBIG_256_28 d)
{
    BIG_256_28 t, b;
    chunk v, tw;
    BIG_256_28_split(t, b, d, MODBITS_FP256BN);

    /* Note that all of the excess gets pushed into t. So if squaring a value with a 4-bit excess, this results in
       t getting all 8 bits of the excess product! So products must be less than pR which is Montgomery compatible */

    if (MConst_FP256BN < NEXCESS_256_28)
    {
        BIG_256_28_imul(t, t, MConst_FP256BN);
        BIG_256_28_norm(t);
        BIG_256_28_add(r, t, b);
        BIG_256_28_norm(r);
        tw = r[NLEN_256_28 - 1];
        r[NLEN_256_28 - 1] &= TMASK_FP256BN;
        r[0] += MConst_FP256BN * ((tw >> TBITS_FP256BN));
    }
    else
    {
        v = BIG_256_28_pmul(t, t, MConst_FP256BN);
        BIG_256_28_add(r, t, b);
        BIG_256_28_norm(r);
        tw = r[NLEN_256_28 - 1];
        r[NLEN_256_28 - 1] &= TMASK_FP256BN;
#if CHUNK == 16
        r[1] += muladd_256_28(MConst_FP256BN, ((tw >> TBITS_FP256BN) + (v << (BASEBITS_256_28 - TBITS_FP256BN))), 0, &r[0]);
#else
        r[0] += MConst_FP256BN * ((tw >> TBITS_FP256BN) + (v << (BASEBITS_256_28 - TBITS_FP256BN)));
#endif
    }
    BIG_256_28_norm(r);
}
#endif

/* This only applies to Curve C448, so specialised (for now) */
#if MODTYPE_FP256BN == GENERALISED_MERSENNE

void FP_FP256BN_nres(FP_FP256BN *y, BIG_256_28 x)
{
    BIG_256_28 mdls;
    BIG_256_28_rcopy(mdls, Modulus_FP256BN);
    BIG_256_28_copy(y->g, x);
    BIG_256_28_mod(y->g,mdls);
    y->XES = 1;
}

/* Converts from residue form back to BIG integer form */
void FP_FP256BN_redc(BIG_256_28 x, FP_FP256BN *y)
{
    BIG_256_28_copy(x, y->g);
}

/* reduce a DBIG to a BIG exploiting the special form of the modulus */
void FP_FP256BN_mod(BIG_256_28 r, DBIG_256_28 d)
{
    BIG_256_28 t, b;
    chunk carry;
    BIG_256_28_split(t, b, d, MBITS_FP256BN);

    BIG_256_28_add(r, t, b);

    BIG_256_28_dscopy(d, t);
    BIG_256_28_dshl(d, MBITS_FP256BN / 2);

    BIG_256_28_split(t, b, d, MBITS_FP256BN);

    BIG_256_28_add(r, r, t);
    BIG_256_28_add(r, r, b);
    BIG_256_28_norm(r);
    BIG_256_28_shl(t, MBITS_FP256BN / 2);

    BIG_256_28_add(r, r, t);

    carry = r[NLEN_256_28 - 1] >> TBITS_FP256BN;

    r[NLEN_256_28 - 1] &= TMASK_FP256BN;
    r[0] += carry;

    r[224 / BASEBITS_256_28] += carry << (224 % BASEBITS_256_28); /* need to check that this falls mid-word */
    BIG_256_28_norm(r);
}

#endif

#if MODTYPE_FP256BN == MONTGOMERY_FRIENDLY

/* convert to Montgomery n-residue form */
void FP_FP256BN_nres(FP_FP256BN *y, BIG_256_28 x)
{
    DBIG_256_28 d;
    BIG_256_28 r;
    BIG_256_28_rcopy(r, R2modp_FP256BN);
    BIG_256_28_mul(d, x, r);
    FP_FP256BN_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_FP256BN_redc(BIG_256_28 x, FP_FP256BN *y)
{
    DBIG_256_28 d;
    BIG_256_28_dzero(d);
    BIG_256_28_dscopy(d, y->g);
    FP_FP256BN_mod(x, d);
}

/* fast modular reduction from DBIG to BIG exploiting special form of the modulus */
void FP_FP256BN_mod(BIG_256_28 a, DBIG_256_28 d)
{
    int i;

    for (i = 0; i < NLEN_256_28; i++)
        d[NLEN_256_28 + i] += muladd_256_28(d[i], MConst_FP256BN - 1, d[i], &d[NLEN_256_28 + i - 1]);

    BIG_256_28_sducopy(a, d);
    BIG_256_28_norm(a);
}

#endif

#if MODTYPE_FP256BN == NOT_SPECIAL

/* convert to Montgomery n-residue form */
void FP_FP256BN_nres(FP_FP256BN *y, BIG_256_28 x)
{
    DBIG_256_28 d;
    BIG_256_28 r;
    BIG_256_28_rcopy(r, R2modp_FP256BN);
    BIG_256_28_mul(d, x, r);
    FP_FP256BN_mod(y->g, d);
    y->XES = 2;
}

/* convert back to regular form */
void FP_FP256BN_redc(BIG_256_28 x, FP_FP256BN *y)
{
    DBIG_256_28 d;
    BIG_256_28_dzero(d);
    BIG_256_28_dscopy(d, y->g);
    FP_FP256BN_mod(x, d);
}


/* reduce a DBIG to a BIG using Montgomery's no trial division method */
/* d is expected to be dnormed before entry */
/* SU= 112 */
void FP_FP256BN_mod(BIG_256_28 a, DBIG_256_28 d)
{
    BIG_256_28 mdls;
    BIG_256_28_rcopy(mdls, Modulus_FP256BN);
    BIG_256_28_monty(a, mdls, MConst_FP256BN, d);
}

#endif

void FP_FP256BN_from_int(FP_FP256BN *x,int a)
{
    BIG_256_28 w;
    if (a<0) BIG_256_28_rcopy(w, Modulus_FP256BN);
    else BIG_256_28_zero(w); 
    BIG_256_28_inc(w,a); BIG_256_28_norm(w); 
    FP_FP256BN_nres(x,w);
}

/* test x==0 ? */
/* SU= 48 */
int FP_FP256BN_iszilch(FP_FP256BN *x)
{
    BIG_256_28 m;
    FP_FP256BN y;
    FP_FP256BN_copy(&y,x);
    FP_FP256BN_reduce(&y);
    FP_FP256BN_redc(m,&y);
    return BIG_256_28_iszilch(m);
}

int FP_FP256BN_isunity(FP_FP256BN *x)
{
    BIG_256_28 m;
    FP_FP256BN y;
    FP_FP256BN_copy(&y,x);
    FP_FP256BN_reduce(&y);
    FP_FP256BN_redc(m,&y);
    return BIG_256_28_isunity(m);
}


void FP_FP256BN_copy(FP_FP256BN *y, FP_FP256BN *x)
{
    BIG_256_28_copy(y->g, x->g);
    y->XES = x->XES;
}

void FP_FP256BN_rcopy(FP_FP256BN *y, const BIG_256_28 c)
{
    BIG_256_28 b;
    BIG_256_28_rcopy(b, c);
    FP_FP256BN_nres(y, b);
}

/* Swap a and b if d=1 */
void FP_FP256BN_cswap(FP_FP256BN *a, FP_FP256BN *b, int d)
{
    sign32 t, c = d;
    BIG_256_28_cswap(a->g, b->g, d);

    c = ~(c - 1);
    t = c & ((a->XES) ^ (b->XES));
    a->XES ^= t;
    b->XES ^= t;

}

/* Move b to a if d=1 */
void FP_FP256BN_cmove(FP_FP256BN *a, FP_FP256BN *b, int d)
{
    sign32 c = -d;

    BIG_256_28_cmove(a->g, b->g, d);
    a->XES ^= (a->XES ^ b->XES)&c;
}

void FP_FP256BN_zero(FP_FP256BN *x)
{
    BIG_256_28_zero(x->g);
    x->XES = 1;
}

int FP_FP256BN_equals(FP_FP256BN *x, FP_FP256BN *y)
{
    FP_FP256BN xg, yg;
    FP_FP256BN_copy(&xg, x);
    FP_FP256BN_copy(&yg, y);
    FP_FP256BN_reduce(&xg);
    FP_FP256BN_reduce(&yg);
    if (BIG_256_28_comp(xg.g, yg.g) == 0) return 1;
    return 0;
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP_FP256BN_islarger(FP_FP256BN *x)
{
    BIG_256_28 p,fx,sx;
    if (FP_FP256BN_iszilch(x)) return 0;
    BIG_256_28_rcopy(p,Modulus_FP256BN);
    FP_FP256BN_redc(fx,x);
    BIG_256_28_sub(sx,p,fx);  BIG_256_28_norm(sx); 
    return BIG_256_28_comp(fx,sx);
}

void FP_FP256BN_toBytes(char *b,FP_FP256BN *x)
{
    BIG_256_28 t;
    FP_FP256BN_redc(t, x);
    BIG_256_28_toBytes(b, t);
}

void FP_FP256BN_fromBytes(FP_FP256BN *x,char *b)
{
    BIG_256_28 t;
    BIG_256_28_fromBytes(t, b);
    FP_FP256BN_nres(x, t);
}

/* output FP */
/* SU= 48 */
void FP_FP256BN_output(FP_FP256BN *r)
{
    BIG_256_28 c;
    FP_FP256BN_reduce(r);
    FP_FP256BN_redc(c, r);
    BIG_256_28_output(c);
}

void FP_FP256BN_rawoutput(FP_FP256BN *r)
{
    BIG_256_28_rawoutput(r->g);
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
void FP_FP256BN_mul(FP_FP256BN *r, FP_FP256BN *a, FP_FP256BN *b)
{
    DBIG_256_28 d;

    if ((sign64)a->XES * b->XES > (sign64)FEXCESS_FP256BN)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_FP256BN_reduce(a);  /* it is sufficient to fully reduce just one of them < p */
    }

#ifdef FUSED_MODMUL
    FP_FP256BN_modmul(r->g, a->g, b->g);
#else
    BIG_256_28_mul(d, a->g, b->g);
    FP_FP256BN_mod(r->g, d);
#endif
    r->XES = 2;
}


/* multiplication by an integer, r=a*c */
/* SU= 136 */
void FP_FP256BN_imul(FP_FP256BN *r, FP_FP256BN *a, int c)
{
    int s = 0;

    if (c < 0)
    {
        c = -c;
        s = 1;
    }

#if MODTYPE_FP256BN==PSEUDO_MERSENNE || MODTYPE_FP256BN==GENERALISED_MERSENNE
    DBIG_256_28 d;
    BIG_256_28_pxmul(d, a->g, c);
    FP_FP256BN_mod(r->g, d);
    r->XES = 2;

#else
    //Montgomery
    BIG_256_28 k;
    FP_FP256BN f;
    if (a->XES * c <= FEXCESS_FP256BN)
    {
        BIG_256_28_pmul(r->g, a->g, c);
        r->XES = a->XES * c; // careful here - XES jumps!
    }
    else
    {
        // don't want to do this - only a problem for Montgomery modulus and larger constants
        BIG_256_28_zero(k);
        BIG_256_28_inc(k, c);
        BIG_256_28_norm(k);
        FP_FP256BN_nres(&f, k);
        FP_FP256BN_mul(r, a, &f);
    }
#endif

    if (s)
    {
        FP_FP256BN_neg(r, r);
        FP_FP256BN_norm(r);
    }
}

/* Set r=a^2 mod m */
/* SU= 88 */
void FP_FP256BN_sqr(FP_FP256BN *r, FP_FP256BN *a)
{
    DBIG_256_28 d;

    if ((sign64)a->XES * a->XES > (sign64)FEXCESS_FP256BN)
    {
#ifdef DEBUG_REDUCE
        printf("Product too large - reducing it\n");
#endif
        FP_FP256BN_reduce(a);
    }

    BIG_256_28_sqr(d, a->g);
    FP_FP256BN_mod(r->g, d);
    r->XES = 2;
}

/* SU= 16 */
/* Set r=a+b */
void FP_FP256BN_add(FP_FP256BN *r, FP_FP256BN *a, FP_FP256BN *b)
{
    BIG_256_28_add(r->g, a->g, b->g);
    r->XES = a->XES + b->XES;
    if (r->XES > FEXCESS_FP256BN)
    {
#ifdef DEBUG_REDUCE
        printf("Sum too large - reducing it \n");
#endif
        FP_FP256BN_reduce(r);
    }
}

/* Set r=a-b mod m */
/* SU= 56 */
void FP_FP256BN_sub(FP_FP256BN *r, FP_FP256BN *a, FP_FP256BN *b)
{
    FP_FP256BN n;
    FP_FP256BN_neg(&n, b);
    FP_FP256BN_add(r, a, &n);
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
static int quo(BIG_256_28 n, BIG_256_28 m)
{
    int sh;
    chunk num, den;
    int hb = CHUNK / 2;
    if (TBITS_FP256BN < hb)
    {
        sh = hb - TBITS_FP256BN;
        num = (n[NLEN_256_28 - 1] << sh) | (n[NLEN_256_28 - 2] >> (BASEBITS_256_28 - sh));
        den = (m[NLEN_256_28 - 1] << sh) | (m[NLEN_256_28 - 2] >> (BASEBITS_256_28 - sh));
    }
    else
    {
        num = n[NLEN_256_28 - 1];
        den = m[NLEN_256_28 - 1];
    }
    return (int)(num / (den + 1));
}

/* SU= 48 */
/* Fully reduce a mod Modulus */
void FP_FP256BN_reduce(FP_FP256BN *a)
{
    BIG_256_28 m, r;
    int sr, sb, q;
    chunk carry;

    BIG_256_28_rcopy(m, Modulus_FP256BN);

    BIG_256_28_norm(a->g);

    if (a->XES > 16)
    {
        q = quo(a->g, m);
        carry = BIG_256_28_pmul(r, m, q);
        r[NLEN_256_28 - 1] += (carry << BASEBITS_256_28); // correction - put any carry out back in again
        BIG_256_28_sub(a->g, a->g, r);
        BIG_256_28_norm(a->g);
        sb = 2;
    }
    else sb = logb2(a->XES - 1); // sb does not depend on the actual data

    BIG_256_28_fshl(m, sb);

    while (sb > 0)
    {
// constant time...
        sr = BIG_256_28_ssn(r, a->g, m); // optimized combined shift, subtract and norm
        BIG_256_28_cmove(a->g, r, 1 - sr);
        sb--;
    }

    //BIG_256_28_mod(a->g,m);
    a->XES = 1;
}

void FP_FP256BN_norm(FP_FP256BN *x)
{
    BIG_256_28_norm(x->g);
}

/* Set r=-a mod Modulus */
/* SU= 64 */
void FP_FP256BN_neg(FP_FP256BN *r, FP_FP256BN *a)
{
    int sb;
    BIG_256_28 m;

    BIG_256_28_rcopy(m, Modulus_FP256BN);

    sb = logb2(a->XES - 1);
    BIG_256_28_fshl(m, sb);
    BIG_256_28_sub(r->g, m, a->g);
    r->XES = ((sign32)1 << sb) + 1;

    if (r->XES > FEXCESS_FP256BN)
    {
#ifdef DEBUG_REDUCE
        printf("Negation too large -  reducing it \n");
#endif
        FP_FP256BN_reduce(r);
    }

}

/* Set r=a/2. */
/* SU= 56 */
void FP_FP256BN_div2(FP_FP256BN *r, FP_FP256BN *a)
{
    BIG_256_28 m;
    BIG_256_28 w;
    BIG_256_28_rcopy(m, Modulus_FP256BN);
    int pr=BIG_256_28_parity(a->g);

    FP_FP256BN_copy(r, a);
    BIG_256_28_copy(w,r->g);
    BIG_256_28_fshr(r->g,1);
    BIG_256_28_add(w, w, m);
    BIG_256_28_norm(w);
    BIG_256_28_fshr(w, 1);   
    
    BIG_256_28_cmove(r->g,w,pr);

}

// Could leak size of b
// but not used here with secret exponent b
void FP_FP256BN_pow(FP_FP256BN *r, FP_FP256BN *a, BIG_256_28 b)
{
    sign8 w[1 + (NLEN_256_28 * BASEBITS_256_28 + 3) / 4];
    FP_FP256BN tb[16];
    BIG_256_28 t;
    int i, nb;

    FP_FP256BN_copy(r,a);
    FP_FP256BN_norm(r);
    BIG_256_28_copy(t, b);
    BIG_256_28_norm(t);
    nb = 1 + (BIG_256_28_nbits(t) + 3) / 4;
    /* convert exponent to 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_256_28_lastbits(t, 4);
        BIG_256_28_dec(t, w[i]);
        BIG_256_28_norm(t);
        BIG_256_28_fshr(t, 4);
    }

    FP_FP256BN_one(&tb[0]);
    FP_FP256BN_copy(&tb[1], r);
    for (i = 2; i < 16; i++)
        FP_FP256BN_mul(&tb[i], &tb[i - 1], r);

    FP_FP256BN_copy(r, &tb[w[nb - 1]]);
    for (i = nb - 2; i >= 0; i--)
    {
        FP_FP256BN_sqr(r, r);
        FP_FP256BN_sqr(r, r);
        FP_FP256BN_sqr(r, r);
        FP_FP256BN_sqr(r, r);
        FP_FP256BN_mul(r, r, &tb[w[i]]);
    }
    FP_FP256BN_reduce(r);
}


#if MODTYPE_FP256BN == PSEUDO_MERSENNE || MODTYPE_FP256BN==GENERALISED_MERSENNE

// See eprint paper https://eprint.iacr.org/2018/1038
// If p=3 mod 4 r= x^{(p-3)/4}, if p=5 mod 8 r=x^{(p-5)/8}

static void FP_FP256BN_fpow(FP_FP256BN *r, FP_FP256BN *x)
{
    int i, j, k, bw, w, c, nw, lo, m, n, nd, e=PM1D2_FP256BN;
    FP_FP256BN xp[11], t, key;
    const int ac[] = {1, 2, 3, 6, 12, 15, 30, 60, 120, 240, 255};
// phase 1
    FP_FP256BN_copy(&xp[0], x); // 1
    FP_FP256BN_sqr(&xp[1], x); // 2
    FP_FP256BN_mul(&xp[2], &xp[1], x); //3
    FP_FP256BN_sqr(&xp[3], &xp[2]); // 6
    FP_FP256BN_sqr(&xp[4], &xp[3]); // 12
    FP_FP256BN_mul(&xp[5], &xp[4], &xp[2]); // 15
    FP_FP256BN_sqr(&xp[6], &xp[5]); // 30
    FP_FP256BN_sqr(&xp[7], &xp[6]); // 60
    FP_FP256BN_sqr(&xp[8], &xp[7]); // 120
    FP_FP256BN_sqr(&xp[9], &xp[8]); // 240
    FP_FP256BN_mul(&xp[10], &xp[9], &xp[5]); // 255

#if MODTYPE_FP256BN==PSEUDO_MERSENNE
    n = MODBITS_FP256BN;
#endif
#if MODTYPE_FP256BN==GENERALISED_MERSENNE  // Ed448 ONLY
    n = MODBITS_FP256BN / 2;
#endif

    n-=(e+1);
    c=(MConst_FP256BN+(1<<e)+1)/(1<<(e+1));

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
        FP_FP256BN_copy(&key, &xp[i]);
        k -= ac[i];
    }
    while (k != 0)
    {
        i--;
        if (ac[i] > k) continue;
        FP_FP256BN_mul(&key, &key, &xp[i]);
        k -= ac[i];
    }

// phase 2
    FP_FP256BN_copy(&xp[1], &xp[2]);
    FP_FP256BN_copy(&xp[2], &xp[5]);
    FP_FP256BN_copy(&xp[3], &xp[10]);

    j = 3; m = 8;
    nw = n - bw;
    while (2 * m < nw)
    {
        FP_FP256BN_copy(&t, &xp[j++]);
        for (i = 0; i < m; i++)
            FP_FP256BN_sqr(&t, &t);
        FP_FP256BN_mul(&xp[j], &xp[j - 1], &t);
        m *= 2;
    }

    lo = nw - m;
    FP_FP256BN_copy(r, &xp[j]);

    while (lo != 0)
    {
        m /= 2; j--;
        if (lo < m) continue;
        lo -= m;
        FP_FP256BN_copy(&t, r);
        for (i = 0; i < m; i++)
            FP_FP256BN_sqr(&t, &t);
        FP_FP256BN_mul(r, &t, &xp[j]);
    }
// phase 3

    if (bw != 0)
    {
        for (i = 0; i < bw; i++ )
            FP_FP256BN_sqr(r, r);
        FP_FP256BN_mul(r, r, &key);
    }
#if MODTYPE_FP256BN==GENERALISED_MERSENNE  // Ed448 ONLY
    FP_FP256BN_copy(&key, r);
    FP_FP256BN_sqr(&t, &key);
    FP_FP256BN_mul(r, &t, &xp[0]);
    for (i = 0; i < n + 1; i++)
        FP_FP256BN_sqr(r, r);
    FP_FP256BN_mul(r, r, &key);
#endif

    for (i=0;i<nd;i++)
        FP_FP256BN_sqr(r,r);
}

#endif


// calculates r=x^(p-1-2^e)/2^{e+1) where 2^e|p-1
void FP_FP256BN_progen(FP_FP256BN *r,FP_FP256BN *x)
{
#if MODTYPE_FP256BN==PSEUDO_MERSENNE  || MODTYPE_FP256BN==GENERALISED_MERSENNE
    FP_FP256BN_fpow(r, x);  
#else
    int e=PM1D2_FP256BN;
    BIG_256_28 m;
    BIG_256_28_rcopy(m, Modulus_FP256BN);
    BIG_256_28_dec(m,1);
    BIG_256_28_shr(m,e);
    BIG_256_28_dec(m,1);
    BIG_256_28_fshr(m,1);
    FP_FP256BN_pow(r,x,m);
#endif
}

/* Is x a QR? return optional hint for fast follow-up square root */
int FP_FP256BN_qr(FP_FP256BN *x,FP_FP256BN *h)
{
    FP_FP256BN r;
    int i,e=PM1D2_FP256BN;
    FP_FP256BN_progen(&r,x);
    if (h!=NULL)
        FP_FP256BN_copy(h,&r);

    FP_FP256BN_sqr(&r,&r);
    FP_FP256BN_mul(&r,x,&r);
    for (i=0;i<e-1;i++ )
        FP_FP256BN_sqr(&r,&r);


//    for (i=0;i<e;i++)
//        FP_FP256BN_sqr(&r,&r);
//    FP_FP256BN_copy(&s,x);
//    for (i=0;i<e-1;i++ )
//        FP_FP256BN_sqr(&s,&s);
//    FP_FP256BN_mul(&r,&r,&s);
    
    return FP_FP256BN_isunity(&r);
}

/* Modular inversion */
void FP_FP256BN_inv(FP_FP256BN *r,FP_FP256BN *x,FP_FP256BN *h)
{
    int i,e=PM1D2_FP256BN;
    FP_FP256BN s,t;
    FP_FP256BN_norm(x);
    FP_FP256BN_copy(&s,x);

    if (h==NULL)
        FP_FP256BN_progen(&t,x);
    else
        FP_FP256BN_copy(&t,h);

    for (i=0;i<e-1;i++)
    {  
        FP_FP256BN_sqr(&s,&s);
        FP_FP256BN_mul(&s,&s,x);
    }
  
    for (i=0;i<=e;i++)
        FP_FP256BN_sqr(&t,&t);
    
    FP_FP256BN_mul(r,&t,&s);
    FP_FP256BN_reduce(r);
}

// Tonelli-Shanks in constant time
void FP_FP256BN_sqrt(FP_FP256BN *r, FP_FP256BN *a, FP_FP256BN *h)
{
    int i,j,k,u,e=PM1D2_FP256BN;
    FP_FP256BN v,g,t,b;
    BIG_256_28 m;

    if (h==NULL)
        FP_FP256BN_progen(&g,a);
    else
        FP_FP256BN_copy(&g,h);

    BIG_256_28_rcopy(m,ROI_FP256BN);
    FP_FP256BN_nres(&v,m);

    FP_FP256BN_sqr(&t,&g);
    FP_FP256BN_mul(&t,&t,a);
   
    FP_FP256BN_mul(r,&g,a);
    FP_FP256BN_copy(&b,&t);
    for (k=e;k>1;k--)
    {
        for (j=1;j<k-1;j++)
            FP_FP256BN_sqr(&b,&b);
        u=1-FP_FP256BN_isunity(&b);
        FP_FP256BN_mul(&g,r,&v);
        FP_FP256BN_cmove(r,&g,u);
        FP_FP256BN_sqr(&v,&v);
        FP_FP256BN_mul(&g,&t,&v);
        FP_FP256BN_cmove(&t,&g,u);
        FP_FP256BN_copy(&b,&t);
    }
// always return +ve square root
    k=FP_FP256BN_sign(r);
    FP_FP256BN_neg(&v,r); FP_FP256BN_norm(&v);
    FP_FP256BN_cmove(r,&v,k);
}

// Calculate both inverse and square root of x, return QR
int FP_FP256BN_invsqrt(FP_FP256BN *i, FP_FP256BN *s, FP_FP256BN *x)
{
    FP_FP256BN h;
    int qr=FP_FP256BN_qr(x,&h);
    FP_FP256BN_sqrt(s,x,&h);
    FP_FP256BN_inv(i,x,&h);
    return qr;
}

// Two for Price of One - See Hamburg https://eprint.iacr.org/2012/309.pdf
// Calculate inverse of i and square root of s, return QR
int FP_FP256BN_tpo(FP_FP256BN *i, FP_FP256BN *s)
{
    int qr;
    FP_FP256BN w,t;
    FP_FP256BN_mul(&w,s,i);
    FP_FP256BN_mul(&t,&w,i);
    qr=FP_FP256BN_invsqrt(i,s,&t);
    FP_FP256BN_mul(i,i,&w);
    FP_FP256BN_mul(s,s,i);
    return qr;
}

/* SU=8 */
/* set n=1 */
void FP_FP256BN_one(FP_FP256BN *n)
{
    BIG_256_28 b;
    BIG_256_28_one(b);
    FP_FP256BN_nres(n, b);
}

int FP_FP256BN_sign(FP_FP256BN *x)
{
#ifdef BIG_ENDIAN_SIGN_FP256BN
    int cp;
    BIG_256_28 m,pm1d2;
    FP_FP256BN y;
    BIG_256_28_rcopy(pm1d2, Modulus_FP256BN);
    BIG_256_28_dec(pm1d2,1);
    BIG_256_28_fshr(pm1d2,1); //(p-1)/2
     
    FP_FP256BN_copy(&y,x);
    FP_FP256BN_reduce(&y);
    FP_FP256BN_redc(m,&y);
    cp=BIG_256_28_comp(m,pm1d2);
    return ((cp+1)&2)>>1;

#else
    BIG_256_28 m;
    FP_FP256BN y;
    FP_FP256BN_copy(&y,x);
    FP_FP256BN_reduce(&y);
    FP_FP256BN_redc(m,&y);
    return BIG_256_28_parity(m);
#endif
}

void FP_FP256BN_rand(FP_FP256BN *x,csprng *rng)
{
    BIG_256_28 w,m;
    BIG_256_28_rcopy(m,Modulus_FP256BN);
    BIG_256_28_randomnum(w,m,rng);
    FP_FP256BN_nres(x,w);
}



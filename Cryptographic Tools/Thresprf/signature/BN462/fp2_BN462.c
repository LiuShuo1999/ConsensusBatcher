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

/* CORE Fp^2 functions */
/* SU=m, m is Stack Usage (no lazy )*/

/* FP2 elements are of the form a+ib, where i is sqrt(-1) */

#include "fp2_BN462.h"

/* test x==0 ? */
/* SU= 8 */
int FP2_BN462_iszilch(FP2_BN462 *x)
{
    return (FP_BN462_iszilch(&(x->a)) & FP_BN462_iszilch(&(x->b)));
}

/* Move b to a if d=1 */
void FP2_BN462_cmove(FP2_BN462 *f, FP2_BN462 *g, int d)
{
    FP_BN462_cmove(&(f->a), &(g->a), d);
    FP_BN462_cmove(&(f->b), &(g->b), d);
}

/* test x==1 ? */
/* SU= 48 */
int FP2_BN462_isunity(FP2_BN462 *x)
{
    FP_BN462 one;
    FP_BN462_one(&one);
    return (FP_BN462_equals(&(x->a), &one) & FP_BN462_iszilch(&(x->b)));
}

/* SU= 8 */
/* Fully reduce a and b mod Modulus */
void FP2_BN462_reduce(FP2_BN462 *w)
{
    FP_BN462_reduce(&(w->a));
    FP_BN462_reduce(&(w->b));
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int FP2_BN462_equals(FP2_BN462 *x, FP2_BN462 *y)
{
    return (FP_BN462_equals(&(x->a), &(y->a)) & FP_BN462_equals(&(x->b), &(y->b)));
}


// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP2_BN462_islarger(FP2_BN462 *x)
{
    int cmp;
    if (FP2_BN462_iszilch(x)) return 0;
    cmp=FP_BN462_islarger(&(x->b));
    if (cmp!=0) return cmp;
    return FP_BN462_islarger(&(x->a));
}

void FP2_BN462_toBytes(char *b,FP2_BN462 *x)
{
    FP_BN462_toBytes(b,&(x->b));
    FP_BN462_toBytes(&b[MODBYTES_464_28],&(x->a));
}

void FP2_BN462_fromBytes(FP2_BN462 *x,char *b)
{
    FP_BN462_fromBytes(&(x->b),b);
    FP_BN462_fromBytes(&(x->a),&b[MODBYTES_464_28]);
}


/* Create FP2 from two FPs */
/* SU= 16 */
void FP2_BN462_from_FPs(FP2_BN462 *w, FP_BN462 *x, FP_BN462 *y)
{
    FP_BN462_copy(&(w->a), x);
    FP_BN462_copy(&(w->b), y);
}

/* Create FP2 from two BIGS */
/* SU= 16 */
void FP2_BN462_from_BIGs(FP2_BN462 *w, BIG_464_28 x, BIG_464_28 y)
{
    FP_BN462_nres(&(w->a), x);
    FP_BN462_nres(&(w->b), y);
}

/* Create FP2 from two ints */
void FP2_BN462_from_ints(FP2_BN462 *w, int xa, int xb)
{
    FP_BN462 a,b;
    FP_BN462_from_int(&a,xa);
    FP_BN462_from_int(&b,xb);
    FP2_BN462_from_FPs(w,&a,&b);
//    BIG_464_28 a, b;
//    BIG_464_28_zero(a); BIG_464_28_inc(a, xa); BIG_464_28_norm(a);
//    BIG_464_28_zero(b); BIG_464_28_inc(b, xb); BIG_464_28_norm(b);
//    FP2_BN462_from_BIGs(w, a, b);
}

/* Create FP2 from FP */
/* SU= 8 */
void FP2_BN462_from_FP(FP2_BN462 *w, FP_BN462 *x)
{
    FP_BN462_copy(&(w->a), x);
    FP_BN462_zero(&(w->b));
}

/* Create FP2 from BIG */
/* SU= 8 */
void FP2_BN462_from_BIG(FP2_BN462 *w, BIG_464_28 x)
{
    FP_BN462_nres(&(w->a), x);
    FP_BN462_zero(&(w->b));
}

/* FP2 copy w=x */
/* SU= 16 */
void FP2_BN462_copy(FP2_BN462 *w, FP2_BN462 *x)
{
    if (w == x) return;
    FP_BN462_copy(&(w->a), &(x->a));
    FP_BN462_copy(&(w->b), &(x->b));
}

/* FP2 set w=0 */
/* SU= 8 */
void FP2_BN462_zero(FP2_BN462 *w)
{
    FP_BN462_zero(&(w->a));
    FP_BN462_zero(&(w->b));
}

/* FP2 set w=1 */
/* SU= 48 */
void FP2_BN462_one(FP2_BN462 *w)
{
    FP_BN462 one;
    FP_BN462_one(&one);
    FP2_BN462_from_FP(w, &one);
}

void FP2_BN462_rcopy(FP2_BN462 *w,const BIG_464_28 a,const BIG_464_28 b)
{
    FP_BN462_rcopy(&(w->a),a);
    FP_BN462_rcopy(&(w->b),b);
}

int FP2_BN462_sign(FP2_BN462 *w)
{
    int p1,p2;
    p1=FP_BN462_sign(&(w->a));
    p2=FP_BN462_sign(&(w->b));
#ifdef BIG_ENDIAN_SIGN_BN462
    p2 ^= (p1 ^ p2)&FP_BN462_iszilch(&(w->b));
    return p2;
#else
    p1 ^= (p1 ^ p2)&FP_BN462_iszilch(&(w->a));
    return p1;
#endif
}

/* Set w=-x */
/* SU= 88 */
void FP2_BN462_neg(FP2_BN462 *w, FP2_BN462 *x)
{
    /* Just one neg! */
    FP_BN462 m, t;
    FP_BN462_add(&m, &(x->a), &(x->b));
    FP_BN462_neg(&m, &m);
    FP_BN462_add(&t, &m, &(x->b));
    FP_BN462_add(&(w->b), &m, &(x->a));
    FP_BN462_copy(&(w->a), &t);

}

/* Set w=conj(x) */
/* SU= 16 */
void FP2_BN462_conj(FP2_BN462 *w, FP2_BN462 *x)
{
    FP_BN462_copy(&(w->a), &(x->a));
    FP_BN462_neg(&(w->b), &(x->b));
    FP_BN462_norm(&(w->b));
}

/* Set w=x+y */
/* SU= 16 */
void FP2_BN462_add(FP2_BN462 *w, FP2_BN462 *x, FP2_BN462 *y)
{
    FP_BN462_add(&(w->a), &(x->a), &(y->a));
    FP_BN462_add(&(w->b), &(x->b), &(y->b));
}

/* Set w=x-y */
/* Input y MUST be normed */
void FP2_BN462_sub(FP2_BN462 *w, FP2_BN462 *x, FP2_BN462 *y)
{
    FP2_BN462 m;
    FP2_BN462_neg(&m, y);
    FP2_BN462_add(w, x, &m);
}

/* Set w=s*x, where s is FP */
/* SU= 16 */
void FP2_BN462_pmul(FP2_BN462 *w, FP2_BN462 *x, FP_BN462 *s)
{
    FP_BN462_mul(&(w->a), &(x->a), s);
    FP_BN462_mul(&(w->b), &(x->b), s);
}

/* SU= 16 */
/* Set w=s*x, where s is int */
void FP2_BN462_imul(FP2_BN462 *w, FP2_BN462 *x, int s)
{
    FP_BN462_imul(&(w->a), &(x->a), s);
    FP_BN462_imul(&(w->b), &(x->b), s);
}

/* Set w=x^2 */
/* SU= 128 */
void FP2_BN462_sqr(FP2_BN462 *w, FP2_BN462 *x)
{
    FP_BN462 w1, w3, mb;

    FP_BN462_add(&w1, &(x->a), &(x->b));
    FP_BN462_neg(&mb, &(x->b));

    FP_BN462_add(&w3, &(x->a), &(x->a));
    FP_BN462_norm(&w3);
    FP_BN462_mul(&(w->b), &w3, &(x->b));

    FP_BN462_add(&(w->a), &(x->a), &mb);

    FP_BN462_norm(&w1);
    FP_BN462_norm(&(w->a));

    FP_BN462_mul(&(w->a), &w1, &(w->a));   /* w->a#2 w->a=1 w1&w2=6 w1*w2=2 */
}

/* Set w=x*y */
/* Inputs MUST be normed  */
/* Now uses Lazy reduction */
void FP2_BN462_mul(FP2_BN462 *w, FP2_BN462 *x, FP2_BN462 *y)
{
    DBIG_464_28 A, B, E, F, pR;
    BIG_464_28 C, D, p;

    BIG_464_28_rcopy(p, Modulus_BN462);
    BIG_464_28_dsucopy(pR, p);

// reduce excesses of a and b as required (so product < pR)

    if ((sign64)(x->a.XES + x->b.XES) * (y->a.XES + y->b.XES) > (sign64)FEXCESS_BN462)
    {
#ifdef DEBUG_REDUCE
        printf("FP2 Product too large - reducing it\n");
#endif
        if (x->a.XES > 1) FP_BN462_reduce(&(x->a));
        if (x->b.XES > 1) FP_BN462_reduce(&(x->b));
    }

    BIG_464_28_mul(A, x->a.g, y->a.g);
    BIG_464_28_mul(B, x->b.g, y->b.g);

    BIG_464_28_add(C, x->a.g, x->b.g);
    BIG_464_28_norm(C);
    BIG_464_28_add(D, y->a.g, y->b.g);
    BIG_464_28_norm(D);

    BIG_464_28_mul(E, C, D);
    BIG_464_28_dadd(F, A, B);
    BIG_464_28_dsub(B, pR, B); //

    BIG_464_28_dadd(A, A, B);  // A<pR? Not necessarily, but <2pR
    BIG_464_28_dsub(E, E, F);  // E<pR ? Yes

    BIG_464_28_dnorm(A);
    FP_BN462_mod(w->a.g, A);
    w->a.XES = 3; // may drift above 2p...
    BIG_464_28_dnorm(E);
    FP_BN462_mod(w->b.g, E);
    w->b.XES = 2;

}

/* output FP2 in hex format [a,b] */
/* SU= 16 */
void FP2_BN462_output(FP2_BN462 *w)
{
    BIG_464_28 bx, by;
    FP2_BN462_reduce(w);
    FP_BN462_redc(bx, &(w->a));
    FP_BN462_redc(by, &(w->b));
    printf("[");
    BIG_464_28_output(bx);
    printf(",");
    BIG_464_28_output(by);
    printf("]");
    FP_BN462_nres(&(w->a), bx);
    FP_BN462_nres(&(w->b), by);
}

/* SU= 8 */
void FP2_BN462_rawoutput(FP2_BN462 *w)
{
    printf("[");
    BIG_464_28_rawoutput(w->a.g);
    printf(",");
    BIG_464_28_rawoutput(w->b.g);
    printf("]");
}


/* Set w=1/x */
/* SU= 128 */
void FP2_BN462_inv(FP2_BN462 *w, FP2_BN462 *x, FP_BN462 *h)
{
    BIG_464_28 m, b;
    FP_BN462 w1, w2;

    FP2_BN462_norm(x);
    FP_BN462_sqr(&w1, &(x->a));
    FP_BN462_sqr(&w2, &(x->b));
    FP_BN462_add(&w1, &w1, &w2);

    FP_BN462_inv(&w1, &w1, h);

    FP_BN462_mul(&(w->a), &(x->a), &w1);
    FP_BN462_neg(&w1, &w1);
    FP_BN462_norm(&w1);
    FP_BN462_mul(&(w->b), &(x->b), &w1);
}


/* Set w=x/2 */
/* SU= 16 */
void FP2_BN462_div2(FP2_BN462 *w, FP2_BN462 *x)
{
    FP_BN462_div2(&(w->a), &(x->a));
    FP_BN462_div2(&(w->b), &(x->b));
}

/* Set w*=(1+sqrt(-1)) */
/* where X^2-(1+sqrt(-1)) is irreducible for FP4, assumes p=3 mod 8 */

/* Input MUST be normed */
void FP2_BN462_mul_ip(FP2_BN462 *w)
{
    FP2_BN462 t;

    int i = QNRI_BN462;

    FP2_BN462_copy(&t, w);
    FP2_BN462_times_i(w);

// add 2^i.t
    while (i > 0)
    {
        FP2_BN462_add(&t, &t, &t);
        FP2_BN462_norm(&t);
        i--;
    }
    FP2_BN462_add(w, &t, w);

#if TOWER_BN462 == POSITOWER
    FP2_BN462_norm(w);
    FP2_BN462_neg(w, w);  // ***
#endif
//    Output NOT normed, so use with care
}

/* Set w/=(1+sqrt(-1)) */
/* SU= 88 */
void FP2_BN462_div_ip(FP2_BN462 *w)
{
    FP2_BN462 z;
    FP2_BN462_norm(w);
    FP2_BN462_from_ints(&z, (1 << QNRI_BN462), 1);
    FP2_BN462_inv(&z, &z, NULL);
    FP2_BN462_mul(w, &z, w);
#if TOWER_BN462 == POSITOWER
    FP2_BN462_neg(w, w);  // ***
#endif
}

/* SU= 8 */
/* normalise a and b components of w */
void FP2_BN462_norm(FP2_BN462 *w)
{
    FP_BN462_norm(&(w->a));
    FP_BN462_norm(&(w->b));
}

/* Set w=a^b mod m */
/* SU= 208 */
/*
void FP2_BN462_pow(FP2_BN462 *r, FP2_BN462* a, BIG_464_28 b)
{
    FP2_BN462 w;
    FP_BN462 one;
    BIG_464_28 z, zilch;
    int bt;

    BIG_464_28_norm(b);
    BIG_464_28_copy(z, b);
    FP2_BN462_copy(&w, a);
    FP_BN462_one(&one);
    BIG_464_28_zero(zilch);
    FP2_BN462_from_FP(r, &one);
    while (1)
    {
        bt = BIG_464_28_parity(z);
        BIG_464_28_shr(z, 1);
        if (bt) FP2_BN462_mul(r, r, &w);
        if (BIG_464_28_comp(z, zilch) == 0) break;
        FP2_BN462_sqr(&w, &w);
    }
    FP2_BN462_reduce(r);
}
*/
/* test for x a QR */

int FP2_BN462_qr(FP2_BN462 *x, FP_BN462 *h)
{ /* test x^(p^2-1)/2 = 1 */
    FP2_BN462 c;
    FP2_BN462_conj(&c,x);
    FP2_BN462_mul(&c,&c,x);

    return FP_BN462_qr(&(c.a),h);
}

/* sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b)/2)+ib/(2*sqrt(a+sqrt(a*a-n*b*b)/2)) */

void FP2_BN462_sqrt(FP2_BN462 *w, FP2_BN462 *u, FP_BN462 *h)
{
    FP_BN462 w1, w2, w3, w4, hint;
    FP2_BN462 nw;
    int sgn,qr;

    FP2_BN462_copy(w, u);
    if (FP2_BN462_iszilch(w)) return;

    FP_BN462_sqr(&w1, &(w->b));
    FP_BN462_sqr(&w2, &(w->a));
    FP_BN462_add(&w1, &w1, &w2);
    FP_BN462_norm(&w1);
    FP_BN462_sqrt(&w1, &w1,h);

    FP_BN462_add(&w2, &(w->a), &w1);
    FP_BN462_norm(&w2);
    FP_BN462_div2(&w2, &w2);

    FP_BN462_div2(&w1,&(w->b));                   // w1=b/2
    qr=FP_BN462_qr(&w2,&hint);                    // only exp!

// tweak hint
    FP_BN462_neg(&w3,&hint); FP_BN462_norm(&w3);    // QNR = -1
    FP_BN462_neg(&w4,&w2); FP_BN462_norm(&w4);

    FP_BN462_cmove(&w2,&w4,1-qr);
    FP_BN462_cmove(&hint,&w3,1-qr);

    FP_BN462_sqrt(&(w->a),&w2,&hint);             // a=sqrt(w2)
    FP_BN462_inv(&w3,&w2,&hint);                  // w3=1/w2
    FP_BN462_mul(&w3,&w3,&(w->a));                // w3=1/sqrt(w2)
    FP_BN462_mul(&(w->b),&w3,&w1);                // b=(b/2)*1/sqrt(w2)
    FP_BN462_copy(&w4,&(w->a));

    FP_BN462_cmove(&(w->a),&(w->b),1-qr);
    FP_BN462_cmove(&(w->b),&w4,1-qr);


/*

    FP_BN462_sqrt(&(w->a),&w2,&hint);             // a=sqrt(w2)
    FP_BN462_inv(&w3,&w2,&hint);                  // w3=1/w2
    FP_BN462_mul(&w3,&w3,&(w->a));                // w3=1/sqrt(w2)
    FP_BN462_mul(&(w->b),&w3,&w1);                // b=(b/2)*1/sqrt(w2)

// tweak hint
    FP_BN462_neg(&hint,&hint); FP_BN462_norm(&hint);    // QNR = -1
    FP_BN462_neg(&w2,&w2); FP_BN462_norm(&w2);

    FP_BN462_sqrt(&w4,&w2,&hint);                 // w4=sqrt(w2)
    FP_BN462_inv(&w3,&w2,&hint);                  // w3=1/w2    
    FP_BN462_mul(&w3,&w3,&w4);                    // w3=1/sqrt(w2)
    FP_BN462_mul(&w3,&w3,&w1);                    // w3=(b/2)*1/sqrt(w2)

    FP_BN462_cmove(&(w->a),&w3,1-qr);
    FP_BN462_cmove(&(w->b),&w4,1-qr);
*/
    sgn=FP2_BN462_sign(w);
    FP2_BN462_neg(&nw,w); FP2_BN462_norm(&nw);
    FP2_BN462_cmove(w,&nw,sgn);

/*
    FP_BN462_sub(&w3, &(w->a), &w1);
    FP_BN462_norm(&w3);
    FP_BN462_div2(&w3, &w3);

    FP_BN462_cmove(&w2,&w3,FP_BN462_qr(&w3,NULL)); // one or the other will be a QR

    FP_BN462_invsqrt(&w3,&(w->a),&w2);
    FP_BN462_mul(&w3,&w3,&(w->a));
    FP_BN462_div2(&w2,&w3);

    FP_BN462_mul(&(w->b), &(w->b), &w2);

    sgn=FP2_BN462_sign(w);
    FP2_BN462_neg(&nw,w); FP2_BN462_norm(&nw);
    FP2_BN462_cmove(w,&nw,sgn); */
}

/* New stuff for ECp4 support */

/* Input MUST be normed */
void FP2_BN462_times_i(FP2_BN462 *w)
{
    FP_BN462 z;
    FP_BN462_copy(&z, &(w->a));
    FP_BN462_neg(&(w->a), &(w->b));
    FP_BN462_copy(&(w->b), &z);

//    Output NOT normed, so use with care
}

void FP2_BN462_rand(FP2_BN462 *x,csprng *rng)
{
    FP_BN462_rand(&(x->a),rng);
    FP_BN462_rand(&(x->b),rng);
}


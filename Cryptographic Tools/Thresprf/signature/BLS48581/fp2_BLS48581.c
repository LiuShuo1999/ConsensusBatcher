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

#include "fp2_BLS48581.h"

/* test x==0 ? */
/* SU= 8 */
int FP2_BLS48581_iszilch(FP2_BLS48581 *x)
{
    return (FP_BLS48581_iszilch(&(x->a)) & FP_BLS48581_iszilch(&(x->b)));
}

/* Move b to a if d=1 */
void FP2_BLS48581_cmove(FP2_BLS48581 *f, FP2_BLS48581 *g, int d)
{
    FP_BLS48581_cmove(&(f->a), &(g->a), d);
    FP_BLS48581_cmove(&(f->b), &(g->b), d);
}

/* test x==1 ? */
/* SU= 48 */
int FP2_BLS48581_isunity(FP2_BLS48581 *x)
{
    FP_BLS48581 one;
    FP_BLS48581_one(&one);
    return (FP_BLS48581_equals(&(x->a), &one) & FP_BLS48581_iszilch(&(x->b)));
}

/* SU= 8 */
/* Fully reduce a and b mod Modulus */
void FP2_BLS48581_reduce(FP2_BLS48581 *w)
{
    FP_BLS48581_reduce(&(w->a));
    FP_BLS48581_reduce(&(w->b));
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int FP2_BLS48581_equals(FP2_BLS48581 *x, FP2_BLS48581 *y)
{
    return (FP_BLS48581_equals(&(x->a), &(y->a)) & FP_BLS48581_equals(&(x->b), &(y->b)));
}


// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP2_BLS48581_islarger(FP2_BLS48581 *x)
{
    int cmp;
    if (FP2_BLS48581_iszilch(x)) return 0;
    cmp=FP_BLS48581_islarger(&(x->b));
    if (cmp!=0) return cmp;
    return FP_BLS48581_islarger(&(x->a));
}

void FP2_BLS48581_toBytes(char *b,FP2_BLS48581 *x)
{
    FP_BLS48581_toBytes(b,&(x->b));
    FP_BLS48581_toBytes(&b[MODBYTES_584_29],&(x->a));
}

void FP2_BLS48581_fromBytes(FP2_BLS48581 *x,char *b)
{
    FP_BLS48581_fromBytes(&(x->b),b);
    FP_BLS48581_fromBytes(&(x->a),&b[MODBYTES_584_29]);
}


/* Create FP2 from two FPs */
/* SU= 16 */
void FP2_BLS48581_from_FPs(FP2_BLS48581 *w, FP_BLS48581 *x, FP_BLS48581 *y)
{
    FP_BLS48581_copy(&(w->a), x);
    FP_BLS48581_copy(&(w->b), y);
}

/* Create FP2 from two BIGS */
/* SU= 16 */
void FP2_BLS48581_from_BIGs(FP2_BLS48581 *w, BIG_584_29 x, BIG_584_29 y)
{
    FP_BLS48581_nres(&(w->a), x);
    FP_BLS48581_nres(&(w->b), y);
}

/* Create FP2 from two ints */
void FP2_BLS48581_from_ints(FP2_BLS48581 *w, int xa, int xb)
{
    FP_BLS48581 a,b;
    FP_BLS48581_from_int(&a,xa);
    FP_BLS48581_from_int(&b,xb);
    FP2_BLS48581_from_FPs(w,&a,&b);
//    BIG_584_29 a, b;
//    BIG_584_29_zero(a); BIG_584_29_inc(a, xa); BIG_584_29_norm(a);
//    BIG_584_29_zero(b); BIG_584_29_inc(b, xb); BIG_584_29_norm(b);
//    FP2_BLS48581_from_BIGs(w, a, b);
}

/* Create FP2 from FP */
/* SU= 8 */
void FP2_BLS48581_from_FP(FP2_BLS48581 *w, FP_BLS48581 *x)
{
    FP_BLS48581_copy(&(w->a), x);
    FP_BLS48581_zero(&(w->b));
}

/* Create FP2 from BIG */
/* SU= 8 */
void FP2_BLS48581_from_BIG(FP2_BLS48581 *w, BIG_584_29 x)
{
    FP_BLS48581_nres(&(w->a), x);
    FP_BLS48581_zero(&(w->b));
}

/* FP2 copy w=x */
/* SU= 16 */
void FP2_BLS48581_copy(FP2_BLS48581 *w, FP2_BLS48581 *x)
{
    if (w == x) return;
    FP_BLS48581_copy(&(w->a), &(x->a));
    FP_BLS48581_copy(&(w->b), &(x->b));
}

/* FP2 set w=0 */
/* SU= 8 */
void FP2_BLS48581_zero(FP2_BLS48581 *w)
{
    FP_BLS48581_zero(&(w->a));
    FP_BLS48581_zero(&(w->b));
}

/* FP2 set w=1 */
/* SU= 48 */
void FP2_BLS48581_one(FP2_BLS48581 *w)
{
    FP_BLS48581 one;
    FP_BLS48581_one(&one);
    FP2_BLS48581_from_FP(w, &one);
}

void FP2_BLS48581_rcopy(FP2_BLS48581 *w,const BIG_584_29 a,const BIG_584_29 b)
{
    FP_BLS48581_rcopy(&(w->a),a);
    FP_BLS48581_rcopy(&(w->b),b);
}

int FP2_BLS48581_sign(FP2_BLS48581 *w)
{
    int p1,p2;
    p1=FP_BLS48581_sign(&(w->a));
    p2=FP_BLS48581_sign(&(w->b));
#ifdef BIG_ENDIAN_SIGN_BLS48581
    p2 ^= (p1 ^ p2)&FP_BLS48581_iszilch(&(w->b));
    return p2;
#else
    p1 ^= (p1 ^ p2)&FP_BLS48581_iszilch(&(w->a));
    return p1;
#endif
}

/* Set w=-x */
/* SU= 88 */
void FP2_BLS48581_neg(FP2_BLS48581 *w, FP2_BLS48581 *x)
{
    /* Just one neg! */
    FP_BLS48581 m, t;
    FP_BLS48581_add(&m, &(x->a), &(x->b));
    FP_BLS48581_neg(&m, &m);
    FP_BLS48581_add(&t, &m, &(x->b));
    FP_BLS48581_add(&(w->b), &m, &(x->a));
    FP_BLS48581_copy(&(w->a), &t);

}

/* Set w=conj(x) */
/* SU= 16 */
void FP2_BLS48581_conj(FP2_BLS48581 *w, FP2_BLS48581 *x)
{
    FP_BLS48581_copy(&(w->a), &(x->a));
    FP_BLS48581_neg(&(w->b), &(x->b));
    FP_BLS48581_norm(&(w->b));
}

/* Set w=x+y */
/* SU= 16 */
void FP2_BLS48581_add(FP2_BLS48581 *w, FP2_BLS48581 *x, FP2_BLS48581 *y)
{
    FP_BLS48581_add(&(w->a), &(x->a), &(y->a));
    FP_BLS48581_add(&(w->b), &(x->b), &(y->b));
}

/* Set w=x-y */
/* Input y MUST be normed */
void FP2_BLS48581_sub(FP2_BLS48581 *w, FP2_BLS48581 *x, FP2_BLS48581 *y)
{
    FP2_BLS48581 m;
    FP2_BLS48581_neg(&m, y);
    FP2_BLS48581_add(w, x, &m);
}

/* Set w=s*x, where s is FP */
/* SU= 16 */
void FP2_BLS48581_pmul(FP2_BLS48581 *w, FP2_BLS48581 *x, FP_BLS48581 *s)
{
    FP_BLS48581_mul(&(w->a), &(x->a), s);
    FP_BLS48581_mul(&(w->b), &(x->b), s);
}

/* SU= 16 */
/* Set w=s*x, where s is int */
void FP2_BLS48581_imul(FP2_BLS48581 *w, FP2_BLS48581 *x, int s)
{
    FP_BLS48581_imul(&(w->a), &(x->a), s);
    FP_BLS48581_imul(&(w->b), &(x->b), s);
}

/* Set w=x^2 */
/* SU= 128 */
void FP2_BLS48581_sqr(FP2_BLS48581 *w, FP2_BLS48581 *x)
{
    FP_BLS48581 w1, w3, mb;

    FP_BLS48581_add(&w1, &(x->a), &(x->b));
    FP_BLS48581_neg(&mb, &(x->b));

    FP_BLS48581_add(&w3, &(x->a), &(x->a));
    FP_BLS48581_norm(&w3);
    FP_BLS48581_mul(&(w->b), &w3, &(x->b));

    FP_BLS48581_add(&(w->a), &(x->a), &mb);

    FP_BLS48581_norm(&w1);
    FP_BLS48581_norm(&(w->a));

    FP_BLS48581_mul(&(w->a), &w1, &(w->a));   /* w->a#2 w->a=1 w1&w2=6 w1*w2=2 */
}

/* Set w=x*y */
/* Inputs MUST be normed  */
/* Now uses Lazy reduction */
void FP2_BLS48581_mul(FP2_BLS48581 *w, FP2_BLS48581 *x, FP2_BLS48581 *y)
{
    DBIG_584_29 A, B, E, F, pR;
    BIG_584_29 C, D, p;

    BIG_584_29_rcopy(p, Modulus_BLS48581);
    BIG_584_29_dsucopy(pR, p);

// reduce excesses of a and b as required (so product < pR)

    if ((sign64)(x->a.XES + x->b.XES) * (y->a.XES + y->b.XES) > (sign64)FEXCESS_BLS48581)
    {
#ifdef DEBUG_REDUCE
        printf("FP2 Product too large - reducing it\n");
#endif
        if (x->a.XES > 1) FP_BLS48581_reduce(&(x->a));
        if (x->b.XES > 1) FP_BLS48581_reduce(&(x->b));
    }

    BIG_584_29_mul(A, x->a.g, y->a.g);
    BIG_584_29_mul(B, x->b.g, y->b.g);

    BIG_584_29_add(C, x->a.g, x->b.g);
    BIG_584_29_norm(C);
    BIG_584_29_add(D, y->a.g, y->b.g);
    BIG_584_29_norm(D);

    BIG_584_29_mul(E, C, D);
    BIG_584_29_dadd(F, A, B);
    BIG_584_29_dsub(B, pR, B); //

    BIG_584_29_dadd(A, A, B);  // A<pR? Not necessarily, but <2pR
    BIG_584_29_dsub(E, E, F);  // E<pR ? Yes

    BIG_584_29_dnorm(A);
    FP_BLS48581_mod(w->a.g, A);
    w->a.XES = 3; // may drift above 2p...
    BIG_584_29_dnorm(E);
    FP_BLS48581_mod(w->b.g, E);
    w->b.XES = 2;

}

/* output FP2 in hex format [a,b] */
/* SU= 16 */
void FP2_BLS48581_output(FP2_BLS48581 *w)
{
    BIG_584_29 bx, by;
    FP2_BLS48581_reduce(w);
    FP_BLS48581_redc(bx, &(w->a));
    FP_BLS48581_redc(by, &(w->b));
    printf("[");
    BIG_584_29_output(bx);
    printf(",");
    BIG_584_29_output(by);
    printf("]");
    FP_BLS48581_nres(&(w->a), bx);
    FP_BLS48581_nres(&(w->b), by);
}

/* SU= 8 */
void FP2_BLS48581_rawoutput(FP2_BLS48581 *w)
{
    printf("[");
    BIG_584_29_rawoutput(w->a.g);
    printf(",");
    BIG_584_29_rawoutput(w->b.g);
    printf("]");
}


/* Set w=1/x */
/* SU= 128 */
void FP2_BLS48581_inv(FP2_BLS48581 *w, FP2_BLS48581 *x, FP_BLS48581 *h)
{
    BIG_584_29 m, b;
    FP_BLS48581 w1, w2;

    FP2_BLS48581_norm(x);
    FP_BLS48581_sqr(&w1, &(x->a));
    FP_BLS48581_sqr(&w2, &(x->b));
    FP_BLS48581_add(&w1, &w1, &w2);

    FP_BLS48581_inv(&w1, &w1, h);

    FP_BLS48581_mul(&(w->a), &(x->a), &w1);
    FP_BLS48581_neg(&w1, &w1);
    FP_BLS48581_norm(&w1);
    FP_BLS48581_mul(&(w->b), &(x->b), &w1);
}


/* Set w=x/2 */
/* SU= 16 */
void FP2_BLS48581_div2(FP2_BLS48581 *w, FP2_BLS48581 *x)
{
    FP_BLS48581_div2(&(w->a), &(x->a));
    FP_BLS48581_div2(&(w->b), &(x->b));
}

/* Set w*=(1+sqrt(-1)) */
/* where X^2-(1+sqrt(-1)) is irreducible for FP4, assumes p=3 mod 8 */

/* Input MUST be normed */
void FP2_BLS48581_mul_ip(FP2_BLS48581 *w)
{
    FP2_BLS48581 t;

    int i = QNRI_BLS48581;

    FP2_BLS48581_copy(&t, w);
    FP2_BLS48581_times_i(w);

// add 2^i.t
    while (i > 0)
    {
        FP2_BLS48581_add(&t, &t, &t);
        FP2_BLS48581_norm(&t);
        i--;
    }
    FP2_BLS48581_add(w, &t, w);

#if TOWER_BLS48581 == POSITOWER
    FP2_BLS48581_norm(w);
    FP2_BLS48581_neg(w, w);  // ***
#endif
//    Output NOT normed, so use with care
}

/* Set w/=(1+sqrt(-1)) */
/* SU= 88 */
void FP2_BLS48581_div_ip(FP2_BLS48581 *w)
{
    FP2_BLS48581 z;
    FP2_BLS48581_norm(w);
    FP2_BLS48581_from_ints(&z, (1 << QNRI_BLS48581), 1);
    FP2_BLS48581_inv(&z, &z, NULL);
    FP2_BLS48581_mul(w, &z, w);
#if TOWER_BLS48581 == POSITOWER
    FP2_BLS48581_neg(w, w);  // ***
#endif
}

/* SU= 8 */
/* normalise a and b components of w */
void FP2_BLS48581_norm(FP2_BLS48581 *w)
{
    FP_BLS48581_norm(&(w->a));
    FP_BLS48581_norm(&(w->b));
}

/* Set w=a^b mod m */
/* SU= 208 */
/*
void FP2_BLS48581_pow(FP2_BLS48581 *r, FP2_BLS48581* a, BIG_584_29 b)
{
    FP2_BLS48581 w;
    FP_BLS48581 one;
    BIG_584_29 z, zilch;
    int bt;

    BIG_584_29_norm(b);
    BIG_584_29_copy(z, b);
    FP2_BLS48581_copy(&w, a);
    FP_BLS48581_one(&one);
    BIG_584_29_zero(zilch);
    FP2_BLS48581_from_FP(r, &one);
    while (1)
    {
        bt = BIG_584_29_parity(z);
        BIG_584_29_shr(z, 1);
        if (bt) FP2_BLS48581_mul(r, r, &w);
        if (BIG_584_29_comp(z, zilch) == 0) break;
        FP2_BLS48581_sqr(&w, &w);
    }
    FP2_BLS48581_reduce(r);
}
*/
/* test for x a QR */

int FP2_BLS48581_qr(FP2_BLS48581 *x, FP_BLS48581 *h)
{ /* test x^(p^2-1)/2 = 1 */
    FP2_BLS48581 c;
    FP2_BLS48581_conj(&c,x);
    FP2_BLS48581_mul(&c,&c,x);

    return FP_BLS48581_qr(&(c.a),h);
}

/* sqrt(a+ib) = sqrt(a+sqrt(a*a-n*b*b)/2)+ib/(2*sqrt(a+sqrt(a*a-n*b*b)/2)) */

void FP2_BLS48581_sqrt(FP2_BLS48581 *w, FP2_BLS48581 *u, FP_BLS48581 *h)
{
    FP_BLS48581 w1, w2, w3, w4, hint;
    FP2_BLS48581 nw;
    int sgn,qr;

    FP2_BLS48581_copy(w, u);
    if (FP2_BLS48581_iszilch(w)) return;

    FP_BLS48581_sqr(&w1, &(w->b));
    FP_BLS48581_sqr(&w2, &(w->a));
    FP_BLS48581_add(&w1, &w1, &w2);
    FP_BLS48581_norm(&w1);
    FP_BLS48581_sqrt(&w1, &w1,h);

    FP_BLS48581_add(&w2, &(w->a), &w1);
    FP_BLS48581_norm(&w2);
    FP_BLS48581_div2(&w2, &w2);

    FP_BLS48581_div2(&w1,&(w->b));                   // w1=b/2
    qr=FP_BLS48581_qr(&w2,&hint);                    // only exp!

// tweak hint
    FP_BLS48581_neg(&w3,&hint); FP_BLS48581_norm(&w3);    // QNR = -1
    FP_BLS48581_neg(&w4,&w2); FP_BLS48581_norm(&w4);

    FP_BLS48581_cmove(&w2,&w4,1-qr);
    FP_BLS48581_cmove(&hint,&w3,1-qr);

    FP_BLS48581_sqrt(&(w->a),&w2,&hint);             // a=sqrt(w2)
    FP_BLS48581_inv(&w3,&w2,&hint);                  // w3=1/w2
    FP_BLS48581_mul(&w3,&w3,&(w->a));                // w3=1/sqrt(w2)
    FP_BLS48581_mul(&(w->b),&w3,&w1);                // b=(b/2)*1/sqrt(w2)
    FP_BLS48581_copy(&w4,&(w->a));

    FP_BLS48581_cmove(&(w->a),&(w->b),1-qr);
    FP_BLS48581_cmove(&(w->b),&w4,1-qr);


/*

    FP_BLS48581_sqrt(&(w->a),&w2,&hint);             // a=sqrt(w2)
    FP_BLS48581_inv(&w3,&w2,&hint);                  // w3=1/w2
    FP_BLS48581_mul(&w3,&w3,&(w->a));                // w3=1/sqrt(w2)
    FP_BLS48581_mul(&(w->b),&w3,&w1);                // b=(b/2)*1/sqrt(w2)

// tweak hint
    FP_BLS48581_neg(&hint,&hint); FP_BLS48581_norm(&hint);    // QNR = -1
    FP_BLS48581_neg(&w2,&w2); FP_BLS48581_norm(&w2);

    FP_BLS48581_sqrt(&w4,&w2,&hint);                 // w4=sqrt(w2)
    FP_BLS48581_inv(&w3,&w2,&hint);                  // w3=1/w2    
    FP_BLS48581_mul(&w3,&w3,&w4);                    // w3=1/sqrt(w2)
    FP_BLS48581_mul(&w3,&w3,&w1);                    // w3=(b/2)*1/sqrt(w2)

    FP_BLS48581_cmove(&(w->a),&w3,1-qr);
    FP_BLS48581_cmove(&(w->b),&w4,1-qr);
*/
    sgn=FP2_BLS48581_sign(w);
    FP2_BLS48581_neg(&nw,w); FP2_BLS48581_norm(&nw);
    FP2_BLS48581_cmove(w,&nw,sgn);

/*
    FP_BLS48581_sub(&w3, &(w->a), &w1);
    FP_BLS48581_norm(&w3);
    FP_BLS48581_div2(&w3, &w3);

    FP_BLS48581_cmove(&w2,&w3,FP_BLS48581_qr(&w3,NULL)); // one or the other will be a QR

    FP_BLS48581_invsqrt(&w3,&(w->a),&w2);
    FP_BLS48581_mul(&w3,&w3,&(w->a));
    FP_BLS48581_div2(&w2,&w3);

    FP_BLS48581_mul(&(w->b), &(w->b), &w2);

    sgn=FP2_BLS48581_sign(w);
    FP2_BLS48581_neg(&nw,w); FP2_BLS48581_norm(&nw);
    FP2_BLS48581_cmove(w,&nw,sgn); */
}

/* New stuff for ECp4 support */

/* Input MUST be normed */
void FP2_BLS48581_times_i(FP2_BLS48581 *w)
{
    FP_BLS48581 z;
    FP_BLS48581_copy(&z, &(w->a));
    FP_BLS48581_neg(&(w->a), &(w->b));
    FP_BLS48581_copy(&(w->b), &z);

//    Output NOT normed, so use with care
}

void FP2_BLS48581_rand(FP2_BLS48581 *x,csprng *rng)
{
    FP_BLS48581_rand(&(x->a),rng);
    FP_BLS48581_rand(&(x->b),rng);
}


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

/* CORE Fp^4 functions */
/* SU=m, m is Stack Usage (no lazy )*/

/* FP4 elements are of the form a+ib, where i is sqrt(-1+sqrt(-1)) */

#include "fp4_BN462.h"

/* test x==0 ? */
/* SU= 8 */
int FP4_BN462_iszilch(FP4_BN462 *x)
{
    return (FP2_BN462_iszilch(&(x->a)) & FP2_BN462_iszilch(&(x->b)));
}

/* test x==1 ? */
/* SU= 8 */
int FP4_BN462_isunity(FP4_BN462 *x)
{
    return (FP2_BN462_isunity(&(x->a)) & FP2_BN462_iszilch(&(x->b)));
}

/* test is w real? That is in a+ib test b is zero */
int FP4_BN462_isreal(FP4_BN462 *w)
{
    return FP2_BN462_iszilch(&(w->b));
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int FP4_BN462_equals(FP4_BN462 *x, FP4_BN462 *y)
{
    return (FP2_BN462_equals(&(x->a), &(y->a)) & FP2_BN462_equals(&(x->b), &(y->b)));
}

// Is x lexically larger than p-x?
// return -1 for no, 0 if x=0, 1 for yes
int FP4_BN462_islarger(FP4_BN462 *x)
{
    int cmp;
    if (FP4_BN462_iszilch(x)) return 0;
    cmp=FP2_BN462_islarger(&(x->b));
    if (cmp!=0) return cmp;
    return FP2_BN462_islarger(&(x->a));
}

void FP4_BN462_toBytes(char *b,FP4_BN462 *x)
{
    FP2_BN462_toBytes(b,&(x->b));
    FP2_BN462_toBytes(&b[2*MODBYTES_464_28],&(x->a));
}

void FP4_BN462_fromBytes(FP4_BN462 *x,char *b)
{
    FP2_BN462_fromBytes(&(x->b),b);
    FP2_BN462_fromBytes(&(x->a),&b[2*MODBYTES_464_28]);
}


/* set FP4 from two FP2s */
/* SU= 16 */
void FP4_BN462_from_FP2s(FP4_BN462 *w, FP2_BN462 * x, FP2_BN462* y)
{
    FP2_BN462_copy(&(w->a), x);
    FP2_BN462_copy(&(w->b), y);
}

/* set FP4 from FP2 */
/* SU= 8 */
void FP4_BN462_from_FP2(FP4_BN462 *w, FP2_BN462 *x)
{
    FP2_BN462_copy(&(w->a), x);
    FP2_BN462_zero(&(w->b));
}

/* set high part of FP4 from FP2 */
/* SU= 8 */
void FP4_BN462_from_FP2H(FP4_BN462 *w, FP2_BN462 *x)
{
    FP2_BN462_copy(&(w->b), x);
    FP2_BN462_zero(&(w->a));
}

/* set FP4 from FP */
void FP4_BN462_from_FP(FP4_BN462 *w, FP_BN462 *x)
{
    FP2_BN462 t;
    FP2_BN462_from_FP(&t, x);
    FP4_BN462_from_FP2(w, &t);
}

/* FP4 copy w=x */
/* SU= 16 */
void FP4_BN462_copy(FP4_BN462 *w, FP4_BN462 *x)
{
    if (w == x) return;
    FP2_BN462_copy(&(w->a), &(x->a));
    FP2_BN462_copy(&(w->b), &(x->b));
}

/* FP4 w=0 */
/* SU= 8 */
void FP4_BN462_zero(FP4_BN462 *w)
{
    FP2_BN462_zero(&(w->a));
    FP2_BN462_zero(&(w->b));
}

/* FP4 w=1 */
/* SU= 8 */
void FP4_BN462_one(FP4_BN462 *w)
{
    FP2_BN462_one(&(w->a));
    FP2_BN462_zero(&(w->b));
}

int FP4_BN462_sign(FP4_BN462 *w)
{
    int p1,p2;
    p1=FP2_BN462_sign(&(w->a));
    p2=FP2_BN462_sign(&(w->b));
#ifdef BIG_ENDIAN_SIGN_BN462
    p2 ^= (p1 ^ p2)&FP2_BN462_iszilch(&(w->b));
    return p2;
#else
    p1 ^= (p1 ^ p2)&FP2_BN462_iszilch(&(w->a));
    return p1;
#endif
}

/* Set w=-x */
/* SU= 160 */
void FP4_BN462_neg(FP4_BN462 *w, FP4_BN462 *x)
{
    /* Just one field neg */
    FP2_BN462 m, t;
    FP4_BN462_norm(x);
    FP2_BN462_add(&m, &(x->a), &(x->b));
    FP2_BN462_neg(&m, &m);
    FP2_BN462_add(&t, &m, &(x->b));
    FP2_BN462_add(&(w->b), &m, &(x->a));
    FP2_BN462_copy(&(w->a), &t);
    FP4_BN462_norm(w);
}

/* Set w=conj(x) */
/* SU= 16 */
void FP4_BN462_conj(FP4_BN462 *w, FP4_BN462 *x)
{
    FP2_BN462_copy(&(w->a), &(x->a));
    FP2_BN462_neg(&(w->b), &(x->b));
    FP4_BN462_norm(w);
}

/* Set w=-conj(x) */
/* SU= 16 */
void FP4_BN462_nconj(FP4_BN462 *w, FP4_BN462 *x)
{
    FP2_BN462_copy(&(w->b), &(x->b));
    FP2_BN462_neg(&(w->a), &(x->a));
    FP4_BN462_norm(w);
}

/* Set w=x+y */
/* SU= 16 */
void FP4_BN462_add(FP4_BN462 *w, FP4_BN462 *x, FP4_BN462 *y)
{
    FP2_BN462_add(&(w->a), &(x->a), &(y->a));
    FP2_BN462_add(&(w->b), &(x->b), &(y->b));
}

/* Set w=x-y */
/* Input y MUST be normed */
void FP4_BN462_sub(FP4_BN462 *w, FP4_BN462 *x, FP4_BN462 *y)
{
    FP4_BN462 my;
    FP4_BN462_neg(&my, y);
    FP4_BN462_add(w, x, &my);
}
/* SU= 8 */
/* reduce all components of w mod Modulus */
void FP4_BN462_reduce(FP4_BN462 *w)
{
    FP2_BN462_reduce(&(w->a));
    FP2_BN462_reduce(&(w->b));
}

/* SU= 8 */
/* normalise all elements of w */
void FP4_BN462_norm(FP4_BN462 *w)
{
    FP2_BN462_norm(&(w->a));
    FP2_BN462_norm(&(w->b));
}

/* Set w=s*x, where s is FP2 */
/* SU= 16 */
void FP4_BN462_pmul(FP4_BN462 *w, FP4_BN462 *x, FP2_BN462 *s)
{
    FP2_BN462_mul(&(w->a), &(x->a), s);
    FP2_BN462_mul(&(w->b), &(x->b), s);
}

/* Set w=s*x, where s is FP */
void FP4_BN462_qmul(FP4_BN462 *w, FP4_BN462 *x, FP_BN462 *s)
{
    FP2_BN462_pmul(&(w->a), &(x->a), s);
    FP2_BN462_pmul(&(w->b), &(x->b), s);
}

/* SU= 16 */
/* Set w=s*x, where s is int */
void FP4_BN462_imul(FP4_BN462 *w, FP4_BN462 *x, int s)
{
    FP2_BN462_imul(&(w->a), &(x->a), s);
    FP2_BN462_imul(&(w->b), &(x->b), s);
}

/* Set w=x^2 */
/* Input MUST be normed  */
void FP4_BN462_sqr(FP4_BN462 *w, FP4_BN462 *x)
{
    FP2_BN462 t1, t2, t3;

    FP2_BN462_mul(&t3, &(x->a), &(x->b)); /* norms x */
    FP2_BN462_copy(&t2, &(x->b));
    FP2_BN462_add(&t1, &(x->a), &(x->b));
    FP2_BN462_mul_ip(&t2);

    FP2_BN462_add(&t2, &(x->a), &t2);

    FP2_BN462_norm(&t1);  // 2
    FP2_BN462_norm(&t2);  // 2

    FP2_BN462_mul(&(w->a), &t1, &t2);

    FP2_BN462_copy(&t2, &t3);
    FP2_BN462_mul_ip(&t2);

    FP2_BN462_add(&t2, &t2, &t3);

    FP2_BN462_norm(&t2);  // 2
    FP2_BN462_neg(&t2, &t2);
    FP2_BN462_add(&(w->a), &(w->a), &t2); /* a=(a+b)(a+i^2.b)-i^2.ab-ab = a*a+ib*ib */
    FP2_BN462_add(&(w->b), &t3, &t3); /* b=2ab */

    FP4_BN462_norm(w);
}

/* Set w=x*y */
/* Inputs MUST be normed  */
void FP4_BN462_mul(FP4_BN462 *w, FP4_BN462 *x, FP4_BN462 *y)
{

    FP2_BN462 t1, t2, t3, t4;
    FP2_BN462_mul(&t1, &(x->a), &(y->a));
    FP2_BN462_mul(&t2, &(x->b), &(y->b));

    FP2_BN462_add(&t3, &(y->b), &(y->a));
    FP2_BN462_add(&t4, &(x->b), &(x->a));

    FP2_BN462_norm(&t4); // 2
    FP2_BN462_norm(&t3); // 2

    FP2_BN462_mul(&t4, &t4, &t3); /* (xa+xb)(ya+yb) */

    FP2_BN462_neg(&t3, &t1); // 1
    FP2_BN462_add(&t4, &t4, &t3); //t4E=3
    FP2_BN462_norm(&t4);

    FP2_BN462_neg(&t3, &t2); // 1
    FP2_BN462_add(&(w->b), &t4, &t3); //wbE=3

    FP2_BN462_mul_ip(&t2);
    FP2_BN462_add(&(w->a), &t2, &t1);

    FP4_BN462_norm(w);
}

/* output FP4 in format [a,b] */
/* SU= 8 */
void FP4_BN462_output(FP4_BN462 *w)
{
    printf("[");
    FP2_BN462_output(&(w->a));
    printf(",");
    FP2_BN462_output(&(w->b));
    printf("]");
}

/* SU= 8 */
void FP4_BN462_rawoutput(FP4_BN462 *w)
{
    printf("[");
    FP2_BN462_rawoutput(&(w->a));
    printf(",");
    FP2_BN462_rawoutput(&(w->b));
    printf("]");
}

/* Set w=1/x */
/* SU= 160 */
void FP4_BN462_inv(FP4_BN462 *w, FP4_BN462 *x, FP_BN462 *h)
{
    FP2_BN462 t1, t2;
    FP2_BN462_sqr(&t1, &(x->a));
    FP2_BN462_sqr(&t2, &(x->b));
    FP2_BN462_mul_ip(&t2);
    FP2_BN462_norm(&t2);
    FP2_BN462_sub(&t1, &t1, &t2);
    FP2_BN462_inv(&t1, &t1, h);
    FP2_BN462_mul(&(w->a), &t1, &(x->a));
    FP2_BN462_neg(&t1, &t1);
    FP2_BN462_norm(&t1);
    FP2_BN462_mul(&(w->b), &t1, &(x->b));
}

/* w*=i where i = sqrt(1+sqrt(-1)) */
/* SU= 200 */
void FP4_BN462_times_i(FP4_BN462 *w)
{
    FP2_BN462 t;
    FP2_BN462_copy(&t, &(w->b));
    FP2_BN462_copy(&(w->b), &(w->a));
    FP2_BN462_mul_ip(&t);
    FP2_BN462_copy(&(w->a), &t);
    FP4_BN462_norm(w);
#if TOWER_BN462 == POSITOWER
    FP4_BN462_neg(w, w);  // ***
    FP4_BN462_norm(w);
#endif
}

/* Set w=w^p using Frobenius */
/* SU= 16 */
void FP4_BN462_frob(FP4_BN462 *w, FP2_BN462 *f)
{
    FP2_BN462_conj(&(w->a), &(w->a));
    FP2_BN462_conj(&(w->b), &(w->b));
    FP2_BN462_mul( &(w->b), f, &(w->b));
}

/* Set r=a^b mod m */
/* SU= 240 */
/*
void FP4_BN462_pow(FP4_BN462 *r, FP4_BN462* a, BIG_464_28 b)
{
    FP4_BN462 w;
    BIG_464_28 z, zilch;
    int bt;

    BIG_464_28_zero(zilch);

    BIG_464_28_copy(z, b);
    BIG_464_28_norm(z);
    FP4_BN462_copy(&w, a);
    FP4_BN462_norm(&w);
    FP4_BN462_one(r);

    while (1)
    {
        bt = BIG_464_28_parity(z);
        BIG_464_28_shr(z, 1);
        if (bt) FP4_BN462_mul(r, r, &w);
        if (BIG_464_28_comp(z, zilch) == 0) break;
        FP4_BN462_sqr(&w, &w);
    }
    FP4_BN462_reduce(r);
} */

/* SU= 304 */
/* XTR xtr_a function */
void FP4_BN462_xtr_A(FP4_BN462 *r, FP4_BN462 *w, FP4_BN462 *x, FP4_BN462 *y, FP4_BN462 *z)
{
    FP4_BN462 t1, t2;
    FP4_BN462_copy(r, x);
    FP4_BN462_sub(&t1, w, y);
    FP4_BN462_norm(&t1);
    FP4_BN462_pmul(&t1, &t1, &(r->a));
    FP4_BN462_add(&t2, w, y);
    FP4_BN462_norm(&t2);
    FP4_BN462_pmul(&t2, &t2, &(r->b));
    FP4_BN462_times_i(&t2);

    FP4_BN462_add(r, &t1, &t2);
    FP4_BN462_add(r, r, z);

    FP4_BN462_reduce(r);
}

/* SU= 152 */
/* XTR xtr_d function */
void FP4_BN462_xtr_D(FP4_BN462 *r, FP4_BN462 *x)
{
    FP4_BN462 w;
    FP4_BN462_copy(r, x);
    FP4_BN462_conj(&w, r);
    FP4_BN462_add(&w, &w, &w);
    FP4_BN462_sqr(r, r);
    FP4_BN462_norm(&w);
    FP4_BN462_sub(r, r, &w);
    FP4_BN462_reduce(r);    /* reduce here as multiple calls trigger automatic reductions */
}

/* SU= 728 */
/* r=x^n using XTR method on traces of FP12s */
void FP4_BN462_xtr_pow(FP4_BN462 *r, FP4_BN462 *x, BIG_464_28 n)
{
    int i, par, nb;
    BIG_464_28 v;
    FP2_BN462 w;
    FP4_BN462 t, a, b, c, sf;

    BIG_464_28_zero(v);
    BIG_464_28_inc(v, 3);
    BIG_464_28_norm(v);
    FP2_BN462_from_BIG(&w, v);
    FP4_BN462_from_FP2(&a, &w);

    FP4_BN462_copy(&sf, x);
    FP4_BN462_norm(&sf);
    FP4_BN462_copy(&b, &sf);
    FP4_BN462_xtr_D(&c, &sf);

    par = BIG_464_28_parity(n);
    BIG_464_28_copy(v, n);
    BIG_464_28_norm(v);
    BIG_464_28_shr(v, 1);
    if (par == 0)
    {
        BIG_464_28_dec(v, 1);
        BIG_464_28_norm(v);
    }

    nb = BIG_464_28_nbits(v);
    for (i = nb - 1; i >= 0; i--)
    {
        if (!BIG_464_28_bit(v, i))
        {
            FP4_BN462_copy(&t, &b);
            FP4_BN462_conj(&sf, &sf);
            FP4_BN462_conj(&c, &c);
            FP4_BN462_xtr_A(&b, &a, &b, &sf, &c);
            FP4_BN462_conj(&sf, &sf);
            FP4_BN462_xtr_D(&c, &t);
            FP4_BN462_xtr_D(&a, &a);
        }
        else
        {
            FP4_BN462_conj(&t, &a);
            FP4_BN462_xtr_D(&a, &b);
            FP4_BN462_xtr_A(&b, &c, &b, &sf, &t);
            FP4_BN462_xtr_D(&c, &c);
        }
    }

    if (par == 0) FP4_BN462_copy(r, &c);
    else FP4_BN462_copy(r, &b);
    FP4_BN462_reduce(r);
}

/* SU= 872 */
/* r=ck^a.cl^n using XTR double exponentiation method on traces of FP12s. See Stam thesis. */
void FP4_BN462_xtr_pow2(FP4_BN462 *r, FP4_BN462 *ck, FP4_BN462 *cl, FP4_BN462 *ckml, FP4_BN462 *ckm2l, BIG_464_28 a, BIG_464_28 b)
{
    int i, f2;
    BIG_464_28 d, e, w;
    FP4_BN462 t, cu, cv, cumv, cum2v;


    BIG_464_28_copy(e, a);
    BIG_464_28_copy(d, b);
    BIG_464_28_norm(e);
    BIG_464_28_norm(d);
    FP4_BN462_copy(&cu, ck);
    FP4_BN462_copy(&cv, cl);
    FP4_BN462_copy(&cumv, ckml);
    FP4_BN462_copy(&cum2v, ckm2l);

    f2 = 0;
    while (BIG_464_28_parity(d) == 0 && BIG_464_28_parity(e) == 0)
    {
        BIG_464_28_shr(d, 1);
        BIG_464_28_shr(e, 1);
        f2++;
    }
    while (BIG_464_28_comp(d, e) != 0)
    {
        if (BIG_464_28_comp(d, e) > 0)
        {
            BIG_464_28_imul(w, e, 4);
            BIG_464_28_norm(w);
            if (BIG_464_28_comp(d, w) <= 0)
            {
                BIG_464_28_copy(w, d);
                BIG_464_28_copy(d, e);
                BIG_464_28_sub(e, w, e);
                BIG_464_28_norm(e);
                FP4_BN462_xtr_A(&t, &cu, &cv, &cumv, &cum2v);
                FP4_BN462_conj(&cum2v, &cumv);
                FP4_BN462_copy(&cumv, &cv);
                FP4_BN462_copy(&cv, &cu);
                FP4_BN462_copy(&cu, &t);
            }
            else if (BIG_464_28_parity(d) == 0)
            {
                BIG_464_28_shr(d, 1);
                FP4_BN462_conj(r, &cum2v);
                FP4_BN462_xtr_A(&t, &cu, &cumv, &cv, r);
                FP4_BN462_xtr_D(&cum2v, &cumv);
                FP4_BN462_copy(&cumv, &t);
                FP4_BN462_xtr_D(&cu, &cu);
            }
            else if (BIG_464_28_parity(e) == 1)
            {
                BIG_464_28_sub(d, d, e);
                BIG_464_28_norm(d);
                BIG_464_28_shr(d, 1);
                FP4_BN462_xtr_A(&t, &cu, &cv, &cumv, &cum2v);
                FP4_BN462_xtr_D(&cu, &cu);
                FP4_BN462_xtr_D(&cum2v, &cv);
                FP4_BN462_conj(&cum2v, &cum2v);
                FP4_BN462_copy(&cv, &t);
            }
            else
            {
                BIG_464_28_copy(w, d);
                BIG_464_28_copy(d, e);
                BIG_464_28_shr(d, 1);
                BIG_464_28_copy(e, w);
                FP4_BN462_xtr_D(&t, &cumv);
                FP4_BN462_conj(&cumv, &cum2v);
                FP4_BN462_conj(&cum2v, &t);
                FP4_BN462_xtr_D(&t, &cv);
                FP4_BN462_copy(&cv, &cu);
                FP4_BN462_copy(&cu, &t);
            }
        }
        if (BIG_464_28_comp(d, e) < 0)
        {
            BIG_464_28_imul(w, d, 4);
            BIG_464_28_norm(w);
            if (BIG_464_28_comp(e, w) <= 0)
            {
                BIG_464_28_sub(e, e, d);
                BIG_464_28_norm(e);
                FP4_BN462_xtr_A(&t, &cu, &cv, &cumv, &cum2v);
                FP4_BN462_copy(&cum2v, &cumv);
                FP4_BN462_copy(&cumv, &cu);
                FP4_BN462_copy(&cu, &t);
            }
            else if (BIG_464_28_parity(e) == 0)
            {
                BIG_464_28_copy(w, d);
                BIG_464_28_copy(d, e);
                BIG_464_28_shr(d, 1);
                BIG_464_28_copy(e, w);
                FP4_BN462_xtr_D(&t, &cumv);
                FP4_BN462_conj(&cumv, &cum2v);
                FP4_BN462_conj(&cum2v, &t);
                FP4_BN462_xtr_D(&t, &cv);
                FP4_BN462_copy(&cv, &cu);
                FP4_BN462_copy(&cu, &t);
            }
            else if (BIG_464_28_parity(d) == 1)
            {
                BIG_464_28_copy(w, e);
                BIG_464_28_copy(e, d);
                BIG_464_28_sub(w, w, d);
                BIG_464_28_norm(w);
                BIG_464_28_copy(d, w);
                BIG_464_28_shr(d, 1);
                FP4_BN462_xtr_A(&t, &cu, &cv, &cumv, &cum2v);
                FP4_BN462_conj(&cumv, &cumv);
                FP4_BN462_xtr_D(&cum2v, &cu);
                FP4_BN462_conj(&cum2v, &cum2v);
                FP4_BN462_xtr_D(&cu, &cv);
                FP4_BN462_copy(&cv, &t);
            }
            else
            {
                BIG_464_28_shr(d, 1);
                FP4_BN462_conj(r, &cum2v);
                FP4_BN462_xtr_A(&t, &cu, &cumv, &cv, r);
                FP4_BN462_xtr_D(&cum2v, &cumv);
                FP4_BN462_copy(&cumv, &t);
                FP4_BN462_xtr_D(&cu, &cu);
            }
        }
    }
    FP4_BN462_xtr_A(r, &cu, &cv, &cumv, &cum2v);
    for (i = 0; i < f2; i++)    FP4_BN462_xtr_D(r, r);
    FP4_BN462_xtr_pow(r, r, d);
}

/* Move b to a if d=1 */
void FP4_BN462_cmove(FP4_BN462 *f, FP4_BN462 *g, int d)
{
    FP2_BN462_cmove(&(f->a), &(g->a), d);
    FP2_BN462_cmove(&(f->b), &(g->b), d);
}

/* New stuff for ECp4 support */

/* Set w=x/2 */
void FP4_BN462_div2(FP4_BN462 *w, FP4_BN462 *x)
{
    FP2_BN462_div2(&(w->a), &(x->a));
    FP2_BN462_div2(&(w->b), &(x->b));
}

void FP4_BN462_rand(FP4_BN462 *x,csprng *rng)
{
    FP2_BN462_rand(&(x->a),rng);
    FP2_BN462_rand(&(x->b),rng);
}

#if PAIRING_FRIENDLY_BN462 >= BLS24_CURVE

/* test for x a QR */
int FP4_BN462_qr(FP4_BN462 *x, FP_BN462 *h)
{ /* test x^(p^4-1)/2 = 1 */
    FP4_BN462 c;
    FP4_BN462_conj(&c,x);
    FP4_BN462_mul(&c,&c,x);

    return FP2_BN462_qr(&(c.a),h);
}

/* sqrt(a+xb) = sqrt((a+sqrt(a*a-n*b*b))/2)+x.b/(2*sqrt((a+sqrt(a*a-n*b*b))/2)) */

void FP4_BN462_sqrt(FP4_BN462 *r, FP4_BN462* x, FP_BN462 *h)
{
    FP2_BN462 a, b, s, t;
    FP4_BN462 nr;
    FP_BN462 hint,twk;
    int sgn,qr;

    FP4_BN462_copy(r, x);
    if (FP4_BN462_iszilch(x)) return;

    FP2_BN462_copy(&a, &(x->a));
    FP2_BN462_copy(&s, &(x->b));

    FP2_BN462_sqr(&s, &s); // s*=s
    FP2_BN462_sqr(&a, &a); // a*=a
    FP2_BN462_mul_ip(&s);
    FP2_BN462_norm(&s);
    FP2_BN462_sub(&a, &a, &s); // a-=txx(s)
    FP2_BN462_norm(&a);
    FP2_BN462_sqrt(&s, &a, h);

    FP2_BN462_copy(&t, &(x->a));

    FP2_BN462_add(&a, &t, &s);
    FP2_BN462_norm(&a);
    FP2_BN462_div2(&a, &a);

    FP2_BN462_div2(&b,&(r->b));                   // w1=b/2
    qr=FP2_BN462_qr(&a,&hint);                    // only exp! Cost=+1

// tweak hint - multiply old hint by Norm(1/Beta)^e where Beta is irreducible polynomial
    FP2_BN462_copy(&s,&a);
    FP_BN462_rcopy(&twk,TWK_BN462);
    FP_BN462_mul(&twk,&twk,&hint);
    FP2_BN462_div_ip(&s); FP2_BN462_norm(&s); // switch to other candidate

    FP2_BN462_cmove(&a,&s,1-qr);
    FP_BN462_cmove(&hint,&twk,1-qr);

    FP2_BN462_sqrt(&(r->a),&a,&hint);             // a=sqrt(w2)  Cost=+1
    FP2_BN462_inv(&s,&a,&hint);                  // w3=1/w2
    FP2_BN462_mul(&s,&s,&(r->a));                // w3=1/sqrt(w2)
    FP2_BN462_mul(&(r->b),&s,&b);                // b=(b/2)*1/sqrt(w2)
    FP2_BN462_copy(&t,&(r->a));

    FP2_BN462_cmove(&(r->a),&(r->b),1-qr);
    FP2_BN462_cmove(&(r->b),&t,1-qr);

    sgn=FP4_BN462_sign(r);
    FP4_BN462_neg(&nr,r); FP4_BN462_norm(&nr);
    FP4_BN462_cmove(r,&nr,sgn);
}

void FP4_BN462_div_i(FP4_BN462 *f)
{
    FP2_BN462 u, v;
    FP2_BN462_copy(&u, &(f->a));
    FP2_BN462_copy(&v, &(f->b));
    FP2_BN462_div_ip(&u);
    FP2_BN462_copy(&(f->a), &v);
    FP2_BN462_copy(&(f->b), &u);
#if TOWER_BN462 == POSITOWER
    FP4_BN462_neg(f, f);  // ***
    FP4_BN462_norm(f);
#endif
}
/*
void FP4_BN462_div_2i(FP4_BN462 *f)
{
    FP2_BN462 u,v;
    FP2_BN462_copy(&u,&(f->a));
    FP2_BN462_copy(&v,&(f->b));
    FP2_BN462_div_ip2(&u);
    FP2_BN462_add(&v,&v,&v);
    FP2_BN462_norm(&v);
    FP2_BN462_copy(&(f->a),&v);
    FP2_BN462_copy(&(f->b),&u);
}
*/
#endif

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

/* CORE Fp^12 functions */
/* SU=m, m is Stack Usage (no lazy )*/
/* FP24 elements are of the form a+i.b+i^2.c */

#include "fp24_BLS24479.h"

/* return 1 if b==c, no branching */
static int teq(sign32 b, sign32 c)
{
    sign32 x = b ^ c;
    x -= 1; // if x=0, x now -1
    return (int)((x >> 31) & 1);
}


/* Constant time select from pre-computed table */
static void FP24_BLS24479_select(FP24_BLS24479 *f, FP24_BLS24479 g[], sign32 b)
{
    FP24_BLS24479 invf;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    FP24_BLS24479_cmove(f, &g[0], teq(babs, 0)); // conditional move
    FP24_BLS24479_cmove(f, &g[1], teq(babs, 1));
    FP24_BLS24479_cmove(f, &g[2], teq(babs, 2));
    FP24_BLS24479_cmove(f, &g[3], teq(babs, 3));
    FP24_BLS24479_cmove(f, &g[4], teq(babs, 4));
    FP24_BLS24479_cmove(f, &g[5], teq(babs, 5));
    FP24_BLS24479_cmove(f, &g[6], teq(babs, 6));
    FP24_BLS24479_cmove(f, &g[7], teq(babs, 7));

    FP24_BLS24479_copy(&invf, f);
    FP24_BLS24479_conj(&invf, &invf); // 1/f
    FP24_BLS24479_cmove(f, &invf, (int)(m & 1));
}

/* test x==0 ? */
/* SU= 8 */
int FP24_BLS24479_iszilch(FP24_BLS24479 *x)
{
    if (FP8_BLS24479_iszilch(&(x->a)) && FP8_BLS24479_iszilch(&(x->b)) && FP8_BLS24479_iszilch(&(x->c))) return 1;
    return 0;
}

/* test x==1 ? */
/* SU= 8 */
int FP24_BLS24479_isunity(FP24_BLS24479 *x)
{
    if (FP8_BLS24479_isunity(&(x->a)) && FP8_BLS24479_iszilch(&(x->b)) && FP8_BLS24479_iszilch(&(x->c))) return 1;
    return 0;
}

/* FP24 copy w=x */
/* SU= 16 */
void FP24_BLS24479_copy(FP24_BLS24479 *w, FP24_BLS24479 *x)
{
    if (x == w) return;
    FP8_BLS24479_copy(&(w->a), &(x->a));
    FP8_BLS24479_copy(&(w->b), &(x->b));
    FP8_BLS24479_copy(&(w->c), &(x->c));
    w->type = x->type;
}

/* FP24 w=1 */
/* SU= 8 */
void FP24_BLS24479_one(FP24_BLS24479 *w)
{
    FP8_BLS24479_one(&(w->a));
    FP8_BLS24479_zero(&(w->b));
    FP8_BLS24479_zero(&(w->c));
    w->type = FP_UNITY;
}

void FP24_BLS24479_zero(FP24_BLS24479 *w)
{
    FP8_BLS24479_zero(&(w->a));
    FP8_BLS24479_zero(&(w->b));
    FP8_BLS24479_zero(&(w->c));
    w->type = FP_ZILCH;
}

/* return 1 if x==y, else 0 */
/* SU= 16 */
int FP24_BLS24479_equals(FP24_BLS24479 *x, FP24_BLS24479 *y)
{
    if (FP8_BLS24479_equals(&(x->a), &(y->a)) && FP8_BLS24479_equals(&(x->b), &(y->b)) && FP8_BLS24479_equals(&(x->c), &(y->c)))
        return 1;
    return 0;
}

/* Set w=conj(x) */
/* SU= 8 */
void FP24_BLS24479_conj(FP24_BLS24479 *w, FP24_BLS24479 *x)
{
    FP24_BLS24479_copy(w, x);
    FP8_BLS24479_conj(&(w->a), &(w->a));
    FP8_BLS24479_nconj(&(w->b), &(w->b));
    FP8_BLS24479_conj(&(w->c), &(w->c));
}

/* Create FP24 from FP8 */
/* SU= 8 */
void FP24_BLS24479_from_FP8(FP24_BLS24479 *w, FP8_BLS24479 *a)
{
    FP8_BLS24479_copy(&(w->a), a);
    FP8_BLS24479_zero(&(w->b));
    FP8_BLS24479_zero(&(w->c));
    w->type = FP_SPARSEST;
}

/* Create FP24 from 3 FP8's */
/* SU= 16 */
void FP24_BLS24479_from_FP8s(FP24_BLS24479 *w, FP8_BLS24479 *a, FP8_BLS24479 *b, FP8_BLS24479 *c)
{
    FP8_BLS24479_copy(&(w->a), a);
    FP8_BLS24479_copy(&(w->b), b);
    FP8_BLS24479_copy(&(w->c), c);
    w->type = FP_DENSE;
}

/* Granger-Scott Unitary Squaring. This does not benefit from lazy reduction */
/* SU= 600 */
void FP24_BLS24479_usqr(FP24_BLS24479 *w, FP24_BLS24479 *x)
{
    FP8_BLS24479 A, B, C, D;

    FP8_BLS24479_copy(&A, &(x->a));

    FP8_BLS24479_sqr(&(w->a), &(x->a));
    FP8_BLS24479_add(&D, &(w->a), &(w->a));
    FP8_BLS24479_add(&(w->a), &D, &(w->a));

    FP8_BLS24479_norm(&(w->a));
    FP8_BLS24479_nconj(&A, &A);

    FP8_BLS24479_add(&A, &A, &A);
    FP8_BLS24479_add(&(w->a), &(w->a), &A);
    FP8_BLS24479_sqr(&B, &(x->c));
    FP8_BLS24479_times_i(&B);

    FP8_BLS24479_add(&D, &B, &B);
    FP8_BLS24479_add(&B, &B, &D);
    FP8_BLS24479_norm(&B);

    FP8_BLS24479_sqr(&C, &(x->b));

    FP8_BLS24479_add(&D, &C, &C);
    FP8_BLS24479_add(&C, &C, &D);

    FP8_BLS24479_norm(&C);
    FP8_BLS24479_conj(&(w->b), &(x->b));
    FP8_BLS24479_add(&(w->b), &(w->b), &(w->b));
    FP8_BLS24479_nconj(&(w->c), &(x->c));

    FP8_BLS24479_add(&(w->c), &(w->c), &(w->c));
    FP8_BLS24479_add(&(w->b), &B, &(w->b));
    FP8_BLS24479_add(&(w->c), &C, &(w->c));
    w->type = FP_DENSE;
    FP24_BLS24479_reduce(w);     /* reduce here as in pow function repeated squarings would trigger multiple reductions */
}

/* FP24 squaring w=x^2 */
/* SU= 600 */
void FP24_BLS24479_sqr(FP24_BLS24479 *w, FP24_BLS24479 *x)
{
    /* Use Chung-Hasan SQR2 method from http://cacr.uwaterloo.ca/techreports/2006/cacr2006-24.pdf */

    FP8_BLS24479 A, B, C, D;

    if (x->type <= FP_UNITY)
    {
        FP24_BLS24479_copy(w, x);
        return;
    }

    FP8_BLS24479_sqr(&A, &(x->a));
    FP8_BLS24479_mul(&B, &(x->b), &(x->c));
    FP8_BLS24479_add(&B, &B, &B);
    FP8_BLS24479_norm(&B);
    FP8_BLS24479_sqr(&C, &(x->c));

    FP8_BLS24479_mul(&D, &(x->a), &(x->b));
    FP8_BLS24479_add(&D, &D, &D);

    FP8_BLS24479_add(&(w->c), &(x->a), &(x->c));
    FP8_BLS24479_add(&(w->c), &(x->b), &(w->c));
    FP8_BLS24479_norm(&(w->c));

    FP8_BLS24479_sqr(&(w->c), &(w->c));

    FP8_BLS24479_copy(&(w->a), &A);
    FP8_BLS24479_add(&A, &A, &B);

    FP8_BLS24479_norm(&A);

    FP8_BLS24479_add(&A, &A, &C);
    FP8_BLS24479_add(&A, &A, &D);

    FP8_BLS24479_norm(&A);

    FP8_BLS24479_neg(&A, &A);
    FP8_BLS24479_times_i(&B);
    FP8_BLS24479_times_i(&C);

    FP8_BLS24479_add(&(w->a), &(w->a), &B);
    FP8_BLS24479_add(&(w->b), &C, &D);
    FP8_BLS24479_add(&(w->c), &(w->c), &A);

    if (x->type == FP_SPARSER || x->type == FP_SPARSEST)
        w->type = FP_SPARSE;
    else
        w->type = FP_DENSE;

    FP24_BLS24479_norm(w);
}

/* FP24 full multiplication w=w*y */


/* SU= 896 */
/* FP24 full multiplication w=w*y */
void FP24_BLS24479_mul(FP24_BLS24479 *w, FP24_BLS24479 *y)
{
    FP8_BLS24479 z0, z1, z2, z3, t0, t1;

    FP8_BLS24479_mul(&z0, &(w->a), &(y->a));
    FP8_BLS24479_mul(&z2, &(w->b), &(y->b)); //

    FP8_BLS24479_add(&t0, &(w->a), &(w->b));
    FP8_BLS24479_add(&t1, &(y->a), &(y->b)); //

    FP8_BLS24479_norm(&t0);
    FP8_BLS24479_norm(&t1);

    FP8_BLS24479_mul(&z1, &t0, &t1);
    FP8_BLS24479_add(&t0, &(w->b), &(w->c));
    FP8_BLS24479_add(&t1, &(y->b), &(y->c)); //

    FP8_BLS24479_norm(&t0);
    FP8_BLS24479_norm(&t1);

    FP8_BLS24479_mul(&z3, &t0, &t1);

    FP8_BLS24479_neg(&t0, &z0);
    FP8_BLS24479_neg(&t1, &z2);

    FP8_BLS24479_add(&z1, &z1, &t0); // z1=z1-z0
    FP8_BLS24479_add(&(w->b), &z1, &t1); // z1=z1-z2
    FP8_BLS24479_add(&z3, &z3, &t1);      // z3=z3-z2
    FP8_BLS24479_add(&z2, &z2, &t0);      // z2=z2-z0

    FP8_BLS24479_add(&t0, &(w->a), &(w->c));
    FP8_BLS24479_add(&t1, &(y->a), &(y->c));

    FP8_BLS24479_norm(&t0);
    FP8_BLS24479_norm(&t1);

    FP8_BLS24479_mul(&t0, &t1, &t0);
    FP8_BLS24479_add(&z2, &z2, &t0);

    FP8_BLS24479_mul(&t0, &(w->c), &(y->c));
    FP8_BLS24479_neg(&t1, &t0);

    FP8_BLS24479_add(&(w->c), &z2, &t1);
    FP8_BLS24479_add(&z3, &z3, &t1);
    FP8_BLS24479_times_i(&t0);
    FP8_BLS24479_add(&(w->b), &(w->b), &t0);
    FP8_BLS24479_norm(&z3);
    FP8_BLS24479_times_i(&z3);
    FP8_BLS24479_add(&(w->a), &z0, &z3);
    w->type = FP_DENSE;
    FP24_BLS24479_norm(w);
}

/* FP24 full multiplication w=w*y */
/* Supports sparse multiplicands */
/* Usually w is denser than y */
void FP24_BLS24479_ssmul(FP24_BLS24479 *w, FP24_BLS24479 *y)
{
    FP8_BLS24479 z0, z1, z2, z3, t0, t1;
    if (w->type == FP_UNITY)
    {
        FP24_BLS24479_copy(w, y);
        return;
    }
    if (y->type == FP_UNITY)
        return;

    if (y->type >= FP_SPARSE)
    {
        FP8_BLS24479_mul(&z0, &(w->a), &(y->a)); // xa.ya   always 11x11

#if SEXTIC_TWIST_BLS24479 == M_TYPE
        if (y->type == FP_SPARSE || w->type == FP_SPARSE)
        {
            FP4_BLS24479_mul(&z2.b, &(w->b).b, &(y->b).b);
            FP4_BLS24479_zero(&z2.a);
            if (y->type != FP_SPARSE)
                FP4_BLS24479_mul(&z2.a, &(w->b).b, &(y->b).a);
            if (w->type != FP_SPARSE)
                FP4_BLS24479_mul(&z2.a, &(w->b).a, &(y->b).b);
            FP8_BLS24479_times_i(&z2);
        }
        else
#endif
            FP8_BLS24479_mul(&z2, &(w->b), &(y->b)); // xb.yb  could be 00x00 or 01x01 or or 10x10 or 11x00 or 11x10 or 11x01 or 11x11

        FP8_BLS24479_add(&t0, &(w->a), &(w->b)); // (xa+xb)
        FP8_BLS24479_add(&t1, &(y->a), &(y->b)); // (ya+yb)

        FP8_BLS24479_norm(&t0);
        FP8_BLS24479_norm(&t1);

        FP8_BLS24479_mul(&z1, &t0, &t1); // (xa+xb)(ya+yb)  always 11x11
        FP8_BLS24479_add(&t0, &(w->b), &(w->c)); // (xb+xc)
        FP8_BLS24479_add(&t1, &(y->b), &(y->c)); // (yb+yc)

        FP8_BLS24479_norm(&t0);
        FP8_BLS24479_norm(&t1);

        FP8_BLS24479_mul(&z3, &t0, &t1); // (xb+xc)(yb+yc)   could be anything...
        FP8_BLS24479_neg(&t0, &z0);      // -(xa.ya)
        FP8_BLS24479_neg(&t1, &z2);      // -(xb.yb)

        FP8_BLS24479_add(&z1, &z1, &t0);
        FP8_BLS24479_add(&(w->b), &z1, &t1); // /wb = (xa+xb)(ya+yb) -(xa.ya) -(xb.yb)                     = xa.yb + xb.ya

        FP8_BLS24479_add(&z3, &z3, &t1);      // (xb+xc)(yb+yc) -(xb.yb)
        FP8_BLS24479_add(&z2, &z2, &t0);      // (xb.yb) - (xa.ya)

        FP8_BLS24479_add(&t0, &(w->a), &(w->c)); // (xa+xc)
        FP8_BLS24479_add(&t1, &(y->a), &(y->c)); // (ya+yc)

        FP8_BLS24479_norm(&t0);
        FP8_BLS24479_norm(&t1);

        FP8_BLS24479_mul(&t0, &t1, &t0); // (xa+xc)(ya+yc)    always 11x11
        FP8_BLS24479_add(&z2, &z2, &t0); // (xb.yb) - (xa.ya) + (xa+xc)(ya+yc)

#if SEXTIC_TWIST_BLS24479 == D_TYPE
        if (y->type == FP_SPARSE || w->type == FP_SPARSE)
        {
            FP4_BLS24479_mul(&t0.a, &(w->c).a, &(y->c).a);
            FP4_BLS24479_zero(&t0.b);
            if (y->type != FP_SPARSE)
                FP4_BLS24479_mul(&t0.b, &(w->c).a, &(y->c).b);
            if (w->type != FP_SPARSE)
                FP4_BLS24479_mul(&t0.b, &(w->c).b, &(y->c).a);
        }
        else
#endif
            FP8_BLS24479_mul(&t0, &(w->c), &(y->c)); // (xc.yc)  could be anything

        FP8_BLS24479_neg(&t1, &t0);            // -(xc.yc)

        FP8_BLS24479_add(&(w->c), &z2, &t1);     // wc = (xb.yb) - (xa.ya) + (xa+xc)(ya+yc) - (xc.yc)    =  xb.yb + xc.ya + xa.yc
        FP8_BLS24479_add(&z3, &z3, &t1);         // (xb+xc)(yb+yc) -(xb.yb) - (xc.yc)                    =  xb.yc + xc.yb
        FP8_BLS24479_times_i(&t0);               // i.(xc.yc)
        FP8_BLS24479_add(&(w->b), &(w->b), &t0); // wb = (xa+xb)(ya+yb) -(xa.ya) -(xb.yb) +i(xc.yc)
        FP8_BLS24479_norm(&z3);
        FP8_BLS24479_times_i(&z3);               // i[(xb+xc)(yb+yc) -(xb.yb) - (xc.yc)]                 = i(xb.yc + xc.yb)
        FP8_BLS24479_add(&(w->a), &z0, &z3);     // wa = xa.ya + i(xb.yc + xc.yb)
    } else {
        if (w->type == FP_SPARSER || w->type == FP_SPARSEST)
        {
            FP24_BLS24479_smul(w, y);
            return;
        }
// dense by sparser - 13m
#if SEXTIC_TWIST_BLS24479 == D_TYPE
        FP8_BLS24479_copy(&z3, &(w->b));
        FP8_BLS24479_mul(&z0, &(w->a), &(y->a));

        if (y->type == FP_SPARSEST)
            FP8_BLS24479_tmul(&z2, &(w->b), &(y->b).a.a.a);
        else
            FP8_BLS24479_pmul(&z2, &(w->b), &(y->b).a);

        FP8_BLS24479_add(&(w->b), &(w->a), &(w->b));
        FP8_BLS24479_copy(&t1, &(y->a));
        FP4_BLS24479_add(&t1.a, &t1.a, &(y->b).a);

        FP8_BLS24479_norm(&t1);
        FP8_BLS24479_norm(&(w->b));

        FP8_BLS24479_mul(&(w->b), &(w->b), &t1);
        FP8_BLS24479_add(&z3, &z3, &(w->c));
        FP8_BLS24479_norm(&z3);

        if (y->type == FP_SPARSEST)
            FP8_BLS24479_tmul(&z3, &z3, &(y->b).a.a.a);
        else
            FP8_BLS24479_pmul(&z3, &z3, &(y->b).a);

        FP8_BLS24479_neg(&t0, &z0);
        FP8_BLS24479_neg(&t1, &z2);

        FP8_BLS24479_add(&(w->b), &(w->b), &t0); // z1=z1-z0
        FP8_BLS24479_add(&(w->b), &(w->b), &t1); // z1=z1-z2

        FP8_BLS24479_add(&z3, &z3, &t1);      // z3=z3-z2
        FP8_BLS24479_add(&z2, &z2, &t0);      // z2=z2-z0

        FP8_BLS24479_add(&t0, &(w->a), &(w->c));
        FP8_BLS24479_norm(&t0);
        FP8_BLS24479_norm(&z3);

        FP8_BLS24479_mul(&t0, &(y->a), &t0);
        FP8_BLS24479_add(&(w->c), &z2, &t0);

        FP8_BLS24479_times_i(&z3);
        FP8_BLS24479_add(&(w->a), &z0, &z3);
#endif
#if SEXTIC_TWIST_BLS24479 == M_TYPE
        FP8_BLS24479_mul(&z0, &(w->a), &(y->a));
        FP8_BLS24479_add(&t0, &(w->a), &(w->b));
        FP8_BLS24479_norm(&t0);

        FP8_BLS24479_mul(&z1, &t0, &(y->a));
        FP8_BLS24479_add(&t0, &(w->b), &(w->c));
        FP8_BLS24479_norm(&t0);

        if (y->type == FP_SPARSEST)
            FP8_BLS24479_tmul(&z3, &t0, &(y->c).b.a.a);
        else
            FP8_BLS24479_pmul(&z3, &t0, &(y->c).b);

        FP8_BLS24479_times_i(&z3);

        FP8_BLS24479_neg(&t0, &z0);
        FP8_BLS24479_add(&z1, &z1, &t0); // z1=z1-z0

        FP8_BLS24479_copy(&(w->b), &z1);
        FP8_BLS24479_copy(&z2, &t0);

        FP8_BLS24479_add(&t0, &(w->a), &(w->c));
        FP8_BLS24479_add(&t1, &(y->a), &(y->c));

        FP8_BLS24479_norm(&t0);
        FP8_BLS24479_norm(&t1);

        FP8_BLS24479_mul(&t0, &t1, &t0);
        FP8_BLS24479_add(&z2, &z2, &t0);

        if (y->type == FP_SPARSEST)
            FP8_BLS24479_tmul(&t0, &(w->c), &(y->c).b.a.a);
        else
            FP8_BLS24479_pmul(&t0, &(w->c), &(y->c).b);

        FP8_BLS24479_times_i(&t0);
        FP8_BLS24479_neg(&t1, &t0);
        FP8_BLS24479_times_i(&t0);

        FP8_BLS24479_add(&(w->c), &z2, &t1);
        FP8_BLS24479_add(&z3, &z3, &t1);

        FP8_BLS24479_add(&(w->b), &(w->b), &t0);
        FP8_BLS24479_norm(&z3);
        FP8_BLS24479_times_i(&z3);
        FP8_BLS24479_add(&(w->a), &z0, &z3);
#endif
    }
    w->type = FP_DENSE;
    FP24_BLS24479_norm(w);
}

/* FP24 multiplication w=w*y */
/* catering for special case that arises from special form of ATE pairing line function */
/* w and y are both sparser line functions - cost = 6m */
void FP24_BLS24479_smul(FP24_BLS24479 *w, FP24_BLS24479 *y)
{
    FP4_BLS24479 w1, w2, w3, ta, tb, tc, td, te, t;


#if SEXTIC_TWIST_BLS24479 == D_TYPE
    FP4_BLS24479_mul(&w1, &(w->a).a, &(y->a).a); // A1.A2
    FP4_BLS24479_mul(&w2, &(w->a).b, &(y->a).b); // B1.B2
    if (y->type == FP_SPARSEST || w->type == FP_SPARSEST)
    {
        if (y->type == FP_SPARSEST && w->type == FP_SPARSEST)
        {
            FP4_BLS24479_zero(&w3);
            FP_BLS24479_mul(&w3.a.a, &(w->b).a.a.a, &(y->b).a.a.a);
        } else {
            if (y->type != FP_SPARSEST)
                FP4_BLS24479_qmul(&w3, &(y->b).a, &(w->b).a.a.a);
            if (w->type != FP_SPARSEST)
                FP4_BLS24479_qmul(&w3, &(w->b).a, &(y->b).a.a.a);
        }
    }
    else
        FP4_BLS24479_mul(&w3, &(w->b).a, &(y->b).a); // C1.C2

    FP4_BLS24479_add(&ta, &(w->a).a, &(w->a).b); // A1+B1
    FP4_BLS24479_add(&tb, &(y->a).a, &(y->a).b); // A2+B2
    FP4_BLS24479_norm(&ta);
    FP4_BLS24479_norm(&tb);
    FP4_BLS24479_mul(&tc, &ta, &tb);         // (A1+B1)(A2+B2)
    FP4_BLS24479_add(&t, &w1, &w2);
    FP4_BLS24479_neg(&t, &t);
    FP4_BLS24479_add(&tc, &tc, &t);          // (A1+B1)(A2+B2)-A1.A2-B1*B2 =  (A1.B2+A2.B1)

    FP4_BLS24479_add(&ta, &(w->a).a, &(w->b).a); // A1+C1
    FP4_BLS24479_add(&tb, &(y->a).a, &(y->b).a); // A2+C2
    FP4_BLS24479_norm(&ta);
    FP4_BLS24479_norm(&tb);
    FP4_BLS24479_mul(&td, &ta, &tb);         // (A1+C1)(A2+C2)
    FP4_BLS24479_add(&t, &w1, &w3);
    FP4_BLS24479_neg(&t, &t);
    FP4_BLS24479_add(&td, &td, &t);          // (A1+C1)(A2+C2)-A1.A2-C1*C2 =  (A1.C2+A2.C1)

    FP4_BLS24479_add(&ta, &(w->a).b, &(w->b).a); // B1+C1
    FP4_BLS24479_add(&tb, &(y->a).b, &(y->b).a); // B2+C2
    FP4_BLS24479_norm(&ta);
    FP4_BLS24479_norm(&tb);
    FP4_BLS24479_mul(&te, &ta, &tb);         // (B1+C1)(B2+C2)
    FP4_BLS24479_add(&t, &w2, &w3);
    FP4_BLS24479_neg(&t, &t);
    FP4_BLS24479_add(&te, &te, &t);          // (B1+C1)(B2+C2)-B1.B2-C1*C2 =  (B1.C2+B2.C1)

    FP4_BLS24479_times_i(&w2);
    FP4_BLS24479_add(&w1, &w1, &w2);
    FP8_BLS24479_from_FP4s(&(w->a), &w1, &tc);
    FP8_BLS24479_from_FP4s(&(w->b), &td, &te); // only norm these 2
    FP8_BLS24479_from_FP4(&(w->c), &w3);

    FP8_BLS24479_norm(&(w->a));
    FP8_BLS24479_norm(&(w->b));
#endif
#if SEXTIC_TWIST_BLS24479 == M_TYPE
    FP4_BLS24479_mul(&w1, &(w->a).a, &(y->a).a); // A1.A2
    FP4_BLS24479_mul(&w2, &(w->a).b, &(y->a).b); // B1.B2
    if (y->type == FP_SPARSEST || w->type == FP_SPARSEST)
    {
        if (y->type == FP_SPARSEST && w->type == FP_SPARSEST)
        {
            FP4_BLS24479_zero(&w3);
            FP_BLS24479_mul(&w3.a.a, &(w->c).b.a.a, &(y->c).b.a.a);
        } else {
            if (y->type != FP_SPARSEST)
                FP4_BLS24479_qmul(&w3, &(y->c).b, &(w->c).b.a.a);
            if (w->type != FP_SPARSEST)
                FP4_BLS24479_qmul(&w3, &(w->c).b, &(y->c).b.a.a);
        }
    }
    else
        FP4_BLS24479_mul(&w3, &(w->c).b, &(y->c).b); // F1.F2

    FP4_BLS24479_add(&ta, &(w->a).a, &(w->a).b); // A1+B1
    FP4_BLS24479_add(&tb, &(y->a).a, &(y->a).b); // A2+B2
    FP4_BLS24479_norm(&ta);
    FP4_BLS24479_norm(&tb);
    FP4_BLS24479_mul(&tc, &ta, &tb);         // (A1+B1)(A2+B2)
    FP4_BLS24479_add(&t, &w1, &w2);
    FP4_BLS24479_neg(&t, &t);
    FP4_BLS24479_add(&tc, &tc, &t);          // (A1+B1)(A2+B2)-A1.A2-B1*B2 =  (A1.B2+A2.B1)

    FP4_BLS24479_add(&ta, &(w->a).a, &(w->c).b); // A1+F1
    FP4_BLS24479_add(&tb, &(y->a).a, &(y->c).b); // A2+F2
    FP4_BLS24479_norm(&ta);
    FP4_BLS24479_norm(&tb);
    FP4_BLS24479_mul(&td, &ta, &tb);         // (A1+F1)(A2+F2)
    FP4_BLS24479_add(&t, &w1, &w3);
    FP4_BLS24479_neg(&t, &t);
    FP4_BLS24479_add(&td, &td, &t);          // (A1+F1)(A2+F2)-A1.A2-F1*F2 =  (A1.F2+A2.F1)

    FP4_BLS24479_add(&ta, &(w->a).b, &(w->c).b); // B1+F1
    FP4_BLS24479_add(&tb, &(y->a).b, &(y->c).b); // B2+F2
    FP4_BLS24479_norm(&ta);
    FP4_BLS24479_norm(&tb);
    FP4_BLS24479_mul(&te, &ta, &tb);         // (B1+F1)(B2+F2)
    FP4_BLS24479_add(&t, &w2, &w3);
    FP4_BLS24479_neg(&t, &t);
    FP4_BLS24479_add(&te, &te, &t);          // (B1+F1)(B2+F2)-B1.B2-F1*F2 =  (B1.F2+B2.F1)

    FP4_BLS24479_times_i(&w2);
    FP4_BLS24479_add(&w1, &w1, &w2);
    FP8_BLS24479_from_FP4s(&(w->a), &w1, &tc);

    FP4_BLS24479_times_i(&w3);
    FP4_BLS24479_norm(&w3);
    FP8_BLS24479_from_FP4H(&(w->b), &w3);

    FP4_BLS24479_norm(&te);
    FP4_BLS24479_times_i(&te);
    FP8_BLS24479_from_FP4s(&(w->c), &te, &td);

    FP8_BLS24479_norm(&(w->a));
    FP8_BLS24479_norm(&(w->c));
#endif

    w->type = FP_SPARSE;
}

/* Set w=1/x */
/* SU= 600 */
void FP24_BLS24479_inv(FP24_BLS24479 *w, FP24_BLS24479 *x)
{
    FP8_BLS24479 f0, f1, f2, f3;

    FP8_BLS24479_sqr(&f0, &(x->a));
    FP8_BLS24479_mul(&f1, &(x->b), &(x->c));
    FP8_BLS24479_times_i(&f1);
    FP8_BLS24479_sub(&f0, &f0, &f1); /* y.a */
    FP8_BLS24479_norm(&f0);

    FP8_BLS24479_sqr(&f1, &(x->c));
    FP8_BLS24479_times_i(&f1);
    FP8_BLS24479_mul(&f2, &(x->a), &(x->b));
    FP8_BLS24479_sub(&f1, &f1, &f2); /* y.b */
    FP8_BLS24479_norm(&f1);

    FP8_BLS24479_sqr(&f2, &(x->b));
    FP8_BLS24479_mul(&f3, &(x->a), &(x->c));
    FP8_BLS24479_sub(&f2, &f2, &f3); /* y.c */
    FP8_BLS24479_norm(&f2);

    FP8_BLS24479_mul(&f3, &(x->b), &f2);
    FP8_BLS24479_times_i(&f3);
    FP8_BLS24479_mul(&(w->a), &f0, &(x->a));
    FP8_BLS24479_add(&f3, &(w->a), &f3);
    FP8_BLS24479_mul(&(w->c), &f1, &(x->c));
    FP8_BLS24479_times_i(&(w->c));

    FP8_BLS24479_add(&f3, &(w->c), &f3);
    FP8_BLS24479_norm(&f3);

    FP8_BLS24479_inv(&f3, &f3, NULL);
    FP8_BLS24479_mul(&(w->a), &f0, &f3);
    FP8_BLS24479_mul(&(w->b), &f1, &f3);
    FP8_BLS24479_mul(&(w->c), &f2, &f3);
    w->type = FP_DENSE;
}

/* constant time powering by small integer of max length bts */

void FP24_BLS24479_pinpow(FP24_BLS24479 *r, int e, int bts)
{
    int i, b;
    FP24_BLS24479 R[2];

    FP24_BLS24479_one(&R[0]);
    FP24_BLS24479_copy(&R[1], r);

    for (i = bts - 1; i >= 0; i--)
    {
        b = (e >> i) & 1;
        FP24_BLS24479_mul(&R[1 - b], &R[b]);
        FP24_BLS24479_usqr(&R[b], &R[b]);
    }
    FP24_BLS24479_copy(r, &R[0]);
}

/* Compressed powering of unitary elements y=x^(e mod r) */
/*
void FP24_BLS24479_compow(FP8_BLS24479 *c, FP24_BLS24479 *x, BIG_480_29 e, BIG_480_29 r)
{
    FP24_BLS24479 g1, g2;
    FP8_BLS24479 cp, cpm1, cpm2;
    FP2_BLS24479 f;
    BIG_480_29 q, a, b, m;

    BIG_480_29_rcopy(a, Fra_BLS24479);
    BIG_480_29_rcopy(b, Frb_BLS24479);
    FP2_BLS24479_from_BIGs(&f, a, b);

    BIG_480_29_rcopy(q, Modulus_BLS24479);

    FP24_BLS24479_copy(&g1, x);
    FP24_BLS24479_copy(&g2, x);

    BIG_480_29_copy(m, q);
    BIG_480_29_mod(m, r);

    BIG_480_29_copy(a, e);
    BIG_480_29_mod(a, m);

    BIG_480_29_copy(b, e);
    BIG_480_29_sdiv(b, m);

    FP24_BLS24479_trace(c, &g1);

    if (BIG_480_29_iszilch(b))
    {
        FP8_BLS24479_xtr_pow(c, c, e);
        return;
    }

    FP24_BLS24479_frob(&g2, &f, 1);
    FP24_BLS24479_trace(&cp, &g2);
    FP24_BLS24479_conj(&g1, &g1);
    FP24_BLS24479_mul(&g2, &g1);
    FP24_BLS24479_trace(&cpm1, &g2);
    FP24_BLS24479_mul(&g2, &g1);

    FP24_BLS24479_trace(&cpm2, &g2);

    FP8_BLS24479_xtr_pow2(c, &cp, c, &cpm1, &cpm2, a, b);

}
*/
/* Note this is simple square and multiply, so not side-channel safe */

void FP24_BLS24479_pow(FP24_BLS24479 *r, FP24_BLS24479 *a, BIG_480_29 b)
{
    FP24_BLS24479 w, sf;
    BIG_480_29 b1, b3;
    int i, nb, bt;
    BIG_480_29_copy(b1, b);
    BIG_480_29_norm(b1);
    BIG_480_29_pmul(b3, b1, 3);
    BIG_480_29_norm(b3);

    FP24_BLS24479_copy(&sf, a);
    FP24_BLS24479_norm(&sf);
    FP24_BLS24479_copy(&w, &sf);

    if (BIG_480_29_iszilch(b3))
    {
        FP24_BLS24479_one(r);
        return;
    }

    nb = BIG_480_29_nbits(b3);
    for (i = nb - 2; i >= 1; i--)
    {
        FP24_BLS24479_usqr(&w, &w);
        bt = BIG_480_29_bit(b3, i) - BIG_480_29_bit(b1, i);
        if (bt == 1)
            FP24_BLS24479_mul(&w, &sf);
        if (bt == -1)
        {
            FP24_BLS24479_conj(&sf, &sf);
            FP24_BLS24479_mul(&w, &sf);
            FP24_BLS24479_conj(&sf, &sf);
        }
    }

    FP24_BLS24479_copy(r, &w);
    FP24_BLS24479_reduce(r);
}

/* p=q0^u0.q1^u1.q2^u2.q3^u3... */
/* Side channel attack secure */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf

void FP24_BLS24479_pow8(FP24_BLS24479 *p, FP24_BLS24479 *q, BIG_480_29 u[8])
{
    int i, j, k, nb, pb1, pb2, bt;
    FP24_BLS24479 g1[8], g2[8], r;
    BIG_480_29 t[8], mt;
    sign8 w1[NLEN_480_29 * BASEBITS_480_29 + 1];
    sign8 s1[NLEN_480_29 * BASEBITS_480_29 + 1];
    sign8 w2[NLEN_480_29 * BASEBITS_480_29 + 1];
    sign8 s2[NLEN_480_29 * BASEBITS_480_29 + 1];

    for (i = 0; i < 8; i++)
        BIG_480_29_copy(t[i], u[i]);

// Precomputed table
    FP24_BLS24479_copy(&g1[0], &q[0]); // q[0]
    FP24_BLS24479_copy(&g1[1], &g1[0]);
    FP24_BLS24479_mul(&g1[1], &q[1]); // q[0].q[1]
    FP24_BLS24479_copy(&g1[2], &g1[0]);
    FP24_BLS24479_mul(&g1[2], &q[2]); // q[0].q[2]
    FP24_BLS24479_copy(&g1[3], &g1[1]);
    FP24_BLS24479_mul(&g1[3], &q[2]); // q[0].q[1].q[2]
    FP24_BLS24479_copy(&g1[4], &g1[0]);
    FP24_BLS24479_mul(&g1[4], &q[3]); // q[0].q[3]
    FP24_BLS24479_copy(&g1[5], &g1[1]);
    FP24_BLS24479_mul(&g1[5], &q[3]); // q[0].q[1].q[3]
    FP24_BLS24479_copy(&g1[6], &g1[2]);
    FP24_BLS24479_mul(&g1[6], &q[3]); // q[0].q[2].q[3]
    FP24_BLS24479_copy(&g1[7], &g1[3]);
    FP24_BLS24479_mul(&g1[7], &q[3]); // q[0].q[1].q[2].q[3]

    FP24_BLS24479_copy(&g2[0], &q[4]); // q[0]
    FP24_BLS24479_copy(&g2[1], &g2[0]);
    FP24_BLS24479_mul(&g2[1], &q[5]); // q[0].q[1]
    FP24_BLS24479_copy(&g2[2], &g2[0]);
    FP24_BLS24479_mul(&g2[2], &q[6]); // q[0].q[2]
    FP24_BLS24479_copy(&g2[3], &g2[1]);
    FP24_BLS24479_mul(&g2[3], &q[6]); // q[0].q[1].q[2]
    FP24_BLS24479_copy(&g2[4], &g2[0]);
    FP24_BLS24479_mul(&g2[4], &q[7]); // q[0].q[3]
    FP24_BLS24479_copy(&g2[5], &g2[1]);
    FP24_BLS24479_mul(&g2[5], &q[7]); // q[0].q[1].q[3]
    FP24_BLS24479_copy(&g2[6], &g2[2]);
    FP24_BLS24479_mul(&g2[6], &q[7]); // q[0].q[2].q[3]
    FP24_BLS24479_copy(&g2[7], &g2[3]);
    FP24_BLS24479_mul(&g2[7], &q[7]); // q[0].q[1].q[2].q[3]

// Make it odd
    pb1 = 1 - BIG_480_29_parity(t[0]);
    BIG_480_29_inc(t[0], pb1);
    BIG_480_29_norm(t[0]);

    pb2 = 1 - BIG_480_29_parity(t[4]);
    BIG_480_29_inc(t[4], pb2);
    BIG_480_29_norm(t[4]);

// Number of bits
    BIG_480_29_zero(mt);
    for (i = 0; i < 8; i++)
    {
        BIG_480_29_or(mt, mt, t[i]);
    }
    nb = 1 + BIG_480_29_nbits(mt);

// Sign pivot
    s1[nb - 1] = 1;
    s2[nb - 1] = 1;
    for (i = 0; i < nb - 1; i++)
    {
        BIG_480_29_fshr(t[0], 1);
        s1[i] = 2 * BIG_480_29_parity(t[0]) - 1;
        BIG_480_29_fshr(t[4], 1);
        s2[i] = 2 * BIG_480_29_parity(t[4]) - 1;
    }

// Recoded exponents
    for (i = 0; i < nb; i++)
    {
        w1[i] = 0;
        k = 1;
        for (j = 1; j < 4; j++)
        {
            bt = s1[i] * BIG_480_29_parity(t[j]);
            BIG_480_29_fshr(t[j], 1);

            BIG_480_29_dec(t[j], (bt >> 1));
            BIG_480_29_norm(t[j]);
            w1[i] += bt * k;
            k *= 2;
        }

        w2[i] = 0;
        k = 1;
        for (j = 5; j < 8; j++)
        {
            bt = s2[i] * BIG_480_29_parity(t[j]);
            BIG_480_29_fshr(t[j], 1);

            BIG_480_29_dec(t[j], (bt >> 1));
            BIG_480_29_norm(t[j]);
            w2[i] += bt * k;
            k *= 2;
        }
    }

// Main loop
    FP24_BLS24479_select(p, g1, 2 * w1[nb - 1] + 1);
    FP24_BLS24479_select(&r, g2, 2 * w2[nb - 1] + 1);
    FP24_BLS24479_mul(p, &r);
    for (i = nb - 2; i >= 0; i--)
    {
        FP24_BLS24479_usqr(p, p);
        FP24_BLS24479_select(&r, g1, 2 * w1[i] + s1[i]);
        FP24_BLS24479_mul(p, &r);
        FP24_BLS24479_select(&r, g2, 2 * w2[i] + s2[i]);
        FP24_BLS24479_mul(p, &r);
    }

// apply correction
    FP24_BLS24479_conj(&r, &q[0]);
    FP24_BLS24479_mul(&r, p);
    FP24_BLS24479_cmove(p, &r, pb1);
    FP24_BLS24479_conj(&r, &q[4]);
    FP24_BLS24479_mul(&r, p);
    FP24_BLS24479_cmove(p, &r, pb2);

    FP24_BLS24479_reduce(p);
}

/* Set w=w^p using Frobenius */
/* SU= 160 */
void FP24_BLS24479_frob(FP24_BLS24479 *w, FP2_BLS24479 *f, int n)
{
    int i;
    FP4_BLS24479 X2, X4;
    FP2_BLS24479 f3, f2;             // f=(1+i)^(p-7)/12
    FP2_BLS24479_sqr(&f2, f);    //
    FP2_BLS24479_mul(&f3, &f2, f); // f3=f^3=(1+i)^(p-7)/4

    FP2_BLS24479_mul_ip(&f3);    // f3 = (1+i).f3 = (1+i)^(p-3)/4
    FP2_BLS24479_norm(&f3);

    for (i = 0; i < n; i++)
    {
        FP8_BLS24479_frob(&(w->a), &f3);  // a=a^p
        FP8_BLS24479_frob(&(w->b), &f3);  // b=b^p
        FP8_BLS24479_frob(&(w->c), &f3);  // c=c^p

        FP8_BLS24479_qmul(&(w->b), &(w->b), f); FP8_BLS24479_times_i2(&(w->b));
        FP8_BLS24479_qmul(&(w->c), &(w->c), &f2); FP8_BLS24479_times_i2(&(w->c)); FP8_BLS24479_times_i2(&(w->c));
    }
    w->type = FP_DENSE;
}

/* SU= 8 */
/* normalise all components of w */
void FP24_BLS24479_norm(FP24_BLS24479 *w)
{
    FP8_BLS24479_norm(&(w->a));
    FP8_BLS24479_norm(&(w->b));
    FP8_BLS24479_norm(&(w->c));
}

/* SU= 8 */
/* reduce all components of w */
void FP24_BLS24479_reduce(FP24_BLS24479 *w)
{
    FP8_BLS24479_reduce(&(w->a));
    FP8_BLS24479_reduce(&(w->b));
    FP8_BLS24479_reduce(&(w->c));
}

/* trace function w=trace(x) */
/* SU= 8 */
void FP24_BLS24479_trace(FP8_BLS24479 *w, FP24_BLS24479 *x)
{
    FP8_BLS24479_imul(w, &(x->a), 3);
    FP8_BLS24479_reduce(w);
}

/* SU= 8 */
/* Output w in hex */
void FP24_BLS24479_output(FP24_BLS24479 *w)
{
    printf("[");
    FP8_BLS24479_output(&(w->a));
    printf(",");
    FP8_BLS24479_output(&(w->b));
    printf(",");
    FP8_BLS24479_output(&(w->c));
    printf("]");
}

/* SU= 64 */
/* Convert g to octet string w */
void FP24_BLS24479_toOctet(octet *W, FP24_BLS24479 *g)
{
    W->len = 24 * MODBYTES_480_29;
    FP8_BLS24479_toBytes(&(W->val[0]),&(g->c));
    FP8_BLS24479_toBytes(&(W->val[8 * MODBYTES_480_29]),&(g->b));
    FP8_BLS24479_toBytes(&(W->val[16 * MODBYTES_480_29]),&(g->a));
}

/* SU= 24 */
/* Restore g from octet string w */
void FP24_BLS24479_fromOctet(FP24_BLS24479 *g, octet *W)
{
    FP8_BLS24479_fromBytes(&(g->c),&(W->val[0]));
    FP8_BLS24479_fromBytes(&(g->b),&(W->val[8 * MODBYTES_480_29]));
    FP8_BLS24479_fromBytes(&(g->a),&(W->val[16 * MODBYTES_480_29]));
}

/* Move b to a if d=1 */
void FP24_BLS24479_cmove(FP24_BLS24479 *f, FP24_BLS24479 *g, int d)
{
    FP8_BLS24479_cmove(&(f->a), &(g->a), d);
    FP8_BLS24479_cmove(&(f->b), &(g->b), d);
    FP8_BLS24479_cmove(&(f->c), &(g->c), d);
    d = ~(d - 1);
    f->type ^= (f->type ^ g->type)&d;
}


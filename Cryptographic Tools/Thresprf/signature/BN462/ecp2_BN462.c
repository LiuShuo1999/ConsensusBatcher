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

/* CORE Weierstrass elliptic curve functions over FP2 */
/* SU=m, m is Stack Usage */

#include "ecp2_BN462.h"
#include "ecp_BN462.h"

int ECP2_BN462_isinf(ECP2_BN462 *P)
{
    return (FP2_BN462_iszilch(&(P->x)) & FP2_BN462_iszilch(&(P->z)));
}

/* Set P=Q */
/* SU= 16 */
void ECP2_BN462_copy(ECP2_BN462 *P, ECP2_BN462 *Q)
{
    FP2_BN462_copy(&(P->x), &(Q->x));
    FP2_BN462_copy(&(P->y), &(Q->y));
    FP2_BN462_copy(&(P->z), &(Q->z));
}

/* set P to Infinity */
/* SU= 8 */
void ECP2_BN462_inf(ECP2_BN462 *P)
{
    FP2_BN462_zero(&(P->x));
    FP2_BN462_one(&(P->y));
    FP2_BN462_zero(&(P->z));
}

/* Conditional move Q to P dependant on d */
static void ECP2_BN462_cmove(ECP2_BN462 *P, ECP2_BN462 *Q, int d)
{
    FP2_BN462_cmove(&(P->x), &(Q->x), d);
    FP2_BN462_cmove(&(P->y), &(Q->y), d);
    FP2_BN462_cmove(&(P->z), &(Q->z), d);
}

/* return 1 if b==c, no branching */
static int teq(sign32 b, sign32 c)
{
    sign32 x = b ^ c;
    x -= 1; // if x=0, x now -1
    return (int)((x >> 31) & 1);
}

/* Constant time select from pre-computed table */
static void ECP2_BN462_select(ECP2_BN462 *P, ECP2_BN462 W[], sign32 b)
{
    ECP2_BN462 MP;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    ECP2_BN462_cmove(P, &W[0], teq(babs, 0)); // conditional move
    ECP2_BN462_cmove(P, &W[1], teq(babs, 1));
    ECP2_BN462_cmove(P, &W[2], teq(babs, 2));
    ECP2_BN462_cmove(P, &W[3], teq(babs, 3));
    ECP2_BN462_cmove(P, &W[4], teq(babs, 4));
    ECP2_BN462_cmove(P, &W[5], teq(babs, 5));
    ECP2_BN462_cmove(P, &W[6], teq(babs, 6));
    ECP2_BN462_cmove(P, &W[7], teq(babs, 7));

    ECP2_BN462_copy(&MP, P);
    ECP2_BN462_neg(&MP);  // minus P
    ECP2_BN462_cmove(P, &MP, (int)(m & 1));
}

/* return 1 if P==Q, else 0 */
/* SU= 312 */
int ECP2_BN462_equals(ECP2_BN462 *P, ECP2_BN462 *Q)
{
    FP2_BN462 a, b;

    FP2_BN462_mul(&a, &(P->x), &(Q->z));
    FP2_BN462_mul(&b, &(Q->x), &(P->z));
    if (!FP2_BN462_equals(&a, &b)) return 0;

    FP2_BN462_mul(&a, &(P->y), &(Q->z));
    FP2_BN462_mul(&b, &(Q->y), &(P->z));
    if (!FP2_BN462_equals(&a, &b)) return 0;
    return 1;
}

/* Make P affine (so z=1) */
/* SU= 232 */
void ECP2_BN462_affine(ECP2_BN462 *P)
{
    FP2_BN462 one, iz;
    if (ECP2_BN462_isinf(P)) return;

    FP2_BN462_one(&one);
    if (FP2_BN462_isunity(&(P->z)))
    {
        FP2_BN462_reduce(&(P->x));
        FP2_BN462_reduce(&(P->y));
        return;
    }

    FP2_BN462_inv(&iz, &(P->z),NULL);
    FP2_BN462_mul(&(P->x), &(P->x), &iz);
    FP2_BN462_mul(&(P->y), &(P->y), &iz);

    FP2_BN462_reduce(&(P->x));
    FP2_BN462_reduce(&(P->y));
    FP2_BN462_copy(&(P->z), &one);
}

/* extract x, y from point P */
/* SU= 16 */
int ECP2_BN462_get(FP2_BN462 *x, FP2_BN462 *y, ECP2_BN462 *P)
{
    ECP2_BN462 W;
    ECP2_BN462_copy(&W, P);
    ECP2_BN462_affine(&W);
    if (ECP2_BN462_isinf(&W)) return -1;
    FP2_BN462_copy(y, &(W.y));
    FP2_BN462_copy(x, &(W.x));
    return 0;
}

/* SU= 152 */
/* Output point P */
void ECP2_BN462_output(ECP2_BN462 *P)
{
    FP2_BN462 x, y;
    if (ECP2_BN462_isinf(P))
    {
        printf("Infinity\n");
        return;
    }
    ECP2_BN462_get(&x, &y, P);
    printf("(");
    FP2_BN462_output(&x);
    printf(",");
    FP2_BN462_output(&y);
    printf(")\n");
}

/* SU= 232 */
void ECP2_BN462_outputxyz(ECP2_BN462 *P)
{
    ECP2_BN462 Q;
    if (ECP2_BN462_isinf(P))
    {
        printf("Infinity\n");
        return;
    }
    ECP2_BN462_copy(&Q, P);
    printf("(");
    FP2_BN462_output(&(Q.x));
    printf(",");
    FP2_BN462_output(&(Q.y));
    printf(",");
    FP2_BN462_output(&(Q.z));
    printf(")\n");
}

/* SU= 168 */
/* Convert Q to octet string */
void ECP2_BN462_toOctet(octet *W, ECP2_BN462 *Q, bool compress)
{
    FP2_BN462 qx, qy;
    bool alt=false;
    ECP2_BN462_get(&qx, &qy, Q);

#if (MBITS-1)%8 <= 4
#ifdef ALLOW_ALT_COMPRESS_BN462
    alt=true;
#endif
#endif

    if (alt)
    {
        FP2_BN462_toBytes(&(W->val[0]),&qx);
        if (!compress)
        {
            W->len=4*MODBYTES_464_28;
            FP2_BN462_toBytes(&(W->val[2*MODBYTES_464_28]), &qy);
        } else {
            W->val[0]|=0x80;
            if (FP2_BN462_islarger(&qy)==1) W->val[0]|=0x20;
            W->len=2*MODBYTES_464_28;
        }
    } else {
        FP2_BN462_toBytes(&(W->val[1]),&qx);
        if (!compress)
        {
            W->val[0] = 0x04;
            FP2_BN462_toBytes(&(W->val[2 * MODBYTES_464_28+1]), &qy);
            W->len = 4 * MODBYTES_464_28 + 1;
        } else {
            W->val[0]=0x02;
            if (FP2_BN462_sign(&qy)==1) W->val[0] = 0x03;
            W->len = 2 * MODBYTES_464_28 + 1;
        }
    }
}

/* SU= 176 */
/* restore Q from octet string */
int ECP2_BN462_fromOctet(ECP2_BN462 *Q, octet *W)
{
    FP2_BN462 qx, qy;
    bool alt=false;
    int sgn,cmp,typ = W->val[0];

#if (MBITS-1)%8 <= 4
#ifdef ALLOW_ALT_COMPRESS_BN462
    alt=true;
#endif
#endif

    if (alt)
    {
        W->val[0]&=0x1f;
        FP2_BN462_fromBytes(&qx,&(W->val[0]));
        W->val[0]=typ;
        if ((typ&0x80)==0)
        {
            FP2_BN462_fromBytes(&qy,&(W->val[2*MODBYTES_464_28]));
            if (ECP2_BN462_set(Q, &qx, &qy)) return 1;
            return 0;
        } else {
            if (!ECP2_BN462_setx(Q,&qx,0)) return 0;
            sgn=(typ&0x20)>>5;
            cmp=FP2_BN462_islarger(&(Q->y));
            if ((sgn==1 && cmp!=1) || (sgn==0 && cmp==1)) ECP2_BN462_neg(Q);
            return 1;
        }

    } else {
        FP2_BN462_fromBytes(&qx,&(W->val[1]));
        if (typ == 0x04)
        {
            FP2_BN462_fromBytes(&qy,&(W->val[2 * MODBYTES_464_28+1]));
            if (ECP2_BN462_set(Q, &qx, &qy)) return 1;
        } else {
            if (ECP2_BN462_setx(Q, &qx, typ&1)) return 1;
        }
    }
    return 0;
}

/* SU= 128 */
/* Calculate RHS of twisted curve equation x^3+B/i or x^3+Bi*/
void ECP2_BN462_rhs(FP2_BN462 *rhs, FP2_BN462 *x)
{
    /* calculate RHS of elliptic curve equation */
    FP2_BN462 t;
    BIG_464_28 b;
    FP2_BN462_sqr(&t, x);

    FP2_BN462_mul(rhs, &t, x);

    /* Assuming CURVE_A=0 */

    BIG_464_28_rcopy(b, CURVE_B_BN462);

    FP2_BN462_from_BIG(&t, b);

#if SEXTIC_TWIST_BN462 == D_TYPE
    FP2_BN462_div_ip(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

#if SEXTIC_TWIST_BN462 == M_TYPE
    FP2_BN462_norm(&t);
    FP2_BN462_mul_ip(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
    FP2_BN462_norm(&t);

#endif


    FP2_BN462_add(rhs, &t, rhs);
    FP2_BN462_reduce(rhs);
}


/* Set P=(x,y). Return 1 if (x,y) is on the curve, else return 0*/
/* SU= 232 */
int ECP2_BN462_set(ECP2_BN462 *P, FP2_BN462 *x, FP2_BN462 *y)
{
    FP2_BN462 rhs, y2;

    FP2_BN462_sqr(&y2, y);
    ECP2_BN462_rhs(&rhs, x);

    if (!FP2_BN462_equals(&y2, &rhs))
    {
        ECP2_BN462_inf(P);
        return 0;
    }

    FP2_BN462_copy(&(P->x), x);
    FP2_BN462_copy(&(P->y), y);

    FP2_BN462_one(&(P->z));
    return 1;
}

/* Set P=(x,y). Return 1 if (x,.) is on the curve, else return 0 */
/* SU= 232 */
int ECP2_BN462_setx(ECP2_BN462 *P, FP2_BN462 *x, int s)
{
    FP2_BN462 y;
    FP_BN462 h;
    ECP2_BN462_rhs(&y, x);

    if (!FP2_BN462_qr(&y,&h))
    {
        ECP2_BN462_inf(P);
        return 0;
    }
    FP2_BN462_sqrt(&y, &y, &h);

    FP2_BN462_copy(&(P->x), x);
    FP2_BN462_copy(&(P->y), &y);
    FP2_BN462_one(&(P->z));

    if (FP2_BN462_sign(&(P->y)) != s)
        FP2_BN462_neg(&(P->y),&(P->y));
    FP2_BN462_reduce(&(P->y));
    return 1;
}

/* Set P=-P */
/* SU= 8 */
void ECP2_BN462_neg(ECP2_BN462 *P)
{
    FP2_BN462_norm(&(P->y));
    FP2_BN462_neg(&(P->y), &(P->y));
    FP2_BN462_norm(&(P->y));
}

/* R+=R */
/* return -1 for Infinity, 0 for addition, 1 for doubling */
/* SU= 448 */
int ECP2_BN462_dbl(ECP2_BN462 *P)
{
    FP2_BN462 t0, t1, t2, iy, x3, y3;

    FP2_BN462_copy(&iy, &(P->y));     //FP2 iy=new FP2(y);
#if SEXTIC_TWIST_BN462==D_TYPE
    FP2_BN462_mul_ip(&iy);            //iy.mul_ip();
    FP2_BN462_norm(&iy);              //iy.norm();
#endif
    FP2_BN462_sqr(&t0, &(P->y));          //t0.sqr();
#if SEXTIC_TWIST_BN462==D_TYPE
    FP2_BN462_mul_ip(&t0);            //t0.mul_ip();
#endif
    FP2_BN462_mul(&t1, &iy, &(P->z)); //t1.mul(z);
    FP2_BN462_sqr(&t2, &(P->z));              //t2.sqr();

    FP2_BN462_add(&(P->z), &t0, &t0); //z.add(t0);
    FP2_BN462_norm(&(P->z));              //z.norm();
    FP2_BN462_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP2_BN462_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP2_BN462_norm(&(P->z));          //z.norm();

    FP2_BN462_imul(&t2, &t2, 3 * CURVE_B_I_BN462); //t2.imul(3*ROM.CURVE_B_I);
#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_mul_ip(&t2);
    FP2_BN462_norm(&t2);
#endif

    FP2_BN462_mul(&x3, &t2, &(P->z)); //x3.mul(z);

    FP2_BN462_add(&y3, &t0, &t2);     //y3.add(t2);
    FP2_BN462_norm(&y3);              //y3.norm();
    FP2_BN462_mul(&(P->z), &(P->z), &t1); //z.mul(t1);

    FP2_BN462_add(&t1, &t2, &t2);     //t1.add(t2);
    FP2_BN462_add(&t2, &t2, &t1);     //t2.add(t1);
    FP2_BN462_norm(&t2);              //t2.norm();
    FP2_BN462_sub(&t0, &t0, &t2);     //t0.sub(t2);
    FP2_BN462_norm(&t0);              //t0.norm();                           //y^2-9bz^2
    FP2_BN462_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP2_BN462_add(&(P->y), &y3, &x3);     //y3.add(x3);                          //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2
    FP2_BN462_mul(&t1, &(P->x), &iy);     //t1.mul(iy);                       //
    FP2_BN462_norm(&t0);          //x.norm();
    FP2_BN462_mul(&(P->x), &t0, &t1); //x.mul(t1);
    FP2_BN462_add(&(P->x), &(P->x), &(P->x)); //x.add(x);       //(y^2-9bz^2)xy2

    FP2_BN462_norm(&(P->x));          //x.norm();
    FP2_BN462_norm(&(P->y));          //y.norm();

    return 1;
}

/* Set P+=Q */
/* SU= 400 */
int ECP2_BN462_add(ECP2_BN462 *P, ECP2_BN462 *Q)
{
    FP2_BN462 t0, t1, t2, t3, t4, x3, y3, z3;
    int b3 = 3 * CURVE_B_I_BN462;

    FP2_BN462_mul(&t0, &(P->x), &(Q->x)); //t0.mul(Q.x);         // x.Q.x
    FP2_BN462_mul(&t1, &(P->y), &(Q->y)); //t1.mul(Q.y);       // y.Q.y

    FP2_BN462_mul(&t2, &(P->z), &(Q->z)); //t2.mul(Q.z);
    FP2_BN462_add(&t3, &(P->x), &(P->y)); //t3.add(y);
    FP2_BN462_norm(&t3);              //t3.norm();          //t3=X1+Y1

    FP2_BN462_add(&t4, &(Q->x), &(Q->y)); //t4.add(Q.y);
    FP2_BN462_norm(&t4);              //t4.norm();            //t4=X2+Y2
    FP2_BN462_mul(&t3, &t3, &t4);     //t3.mul(t4);                       //t3=(X1+Y1)(X2+Y2)
    FP2_BN462_add(&t4, &t0, &t1);     //t4.add(t1);       //t4=X1.X2+Y1.Y2

    FP2_BN462_sub(&t3, &t3, &t4);     //t3.sub(t4);
    FP2_BN462_norm(&t3);              //t3.norm();
#if SEXTIC_TWIST_BN462==D_TYPE
    FP2_BN462_mul_ip(&t3);            //t3.mul_ip();
    FP2_BN462_norm(&t3);              //t3.norm();         //t3=(X1+Y1)(X2+Y2)-(X1.X2+Y1.Y2) = X1.Y2+X2.Y1
#endif
    FP2_BN462_add(&t4, &(P->y), &(P->z)); //t4.add(z);
    FP2_BN462_norm(&t4);              //t4.norm();            //t4=Y1+Z1
    FP2_BN462_add(&x3, &(Q->y), &(Q->z)); //x3.add(Q.z);
    FP2_BN462_norm(&x3);              //x3.norm();            //x3=Y2+Z2

    FP2_BN462_mul(&t4, &t4, &x3);     //t4.mul(x3);                       //t4=(Y1+Z1)(Y2+Z2)
    FP2_BN462_add(&x3, &t1, &t2);     //x3.add(t2);                       //X3=Y1.Y2+Z1.Z2

    FP2_BN462_sub(&t4, &t4, &x3);     //t4.sub(x3);
    FP2_BN462_norm(&t4);              //t4.norm();
#if SEXTIC_TWIST_BN462==D_TYPE
    FP2_BN462_mul_ip(&t4);            //t4.mul_ip();
    FP2_BN462_norm(&t4);              //t4.norm();          //t4=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1
#endif
    FP2_BN462_add(&x3, &(P->x), &(P->z)); //x3.add(z);
    FP2_BN462_norm(&x3);              //x3.norm();    // x3=X1+Z1
    FP2_BN462_add(&y3, &(Q->x), &(Q->z)); //y3.add(Q.z);
    FP2_BN462_norm(&y3);              //y3.norm();                // y3=X2+Z2
    FP2_BN462_mul(&x3, &x3, &y3);     //x3.mul(y3);                           // x3=(X1+Z1)(X2+Z2)
    FP2_BN462_add(&y3, &t0, &t2);     //y3.add(t2);                           // y3=X1.X2+Z1+Z2
    FP2_BN462_sub(&y3, &x3, &y3);     //y3.rsub(x3);
    FP2_BN462_norm(&y3);              //y3.norm();                // y3=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1
#if SEXTIC_TWIST_BN462==D_TYPE
    FP2_BN462_mul_ip(&t0);            //t0.mul_ip();
    FP2_BN462_norm(&t0);              //t0.norm(); // x.Q.x
    FP2_BN462_mul_ip(&t1);            //t1.mul_ip();
    FP2_BN462_norm(&t1);              //t1.norm(); // y.Q.y
#endif
    FP2_BN462_add(&x3, &t0, &t0);     //x3.add(t0);
    FP2_BN462_add(&t0, &t0, &x3);     //t0.add(x3);
    FP2_BN462_norm(&t0);              //t0.norm();
    FP2_BN462_imul(&t2, &t2, b3);     //t2.imul(b);
#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_mul_ip(&t2);
    FP2_BN462_norm(&t2);
#endif
    FP2_BN462_add(&z3, &t1, &t2);     //z3.add(t2);
    FP2_BN462_norm(&z3);              //z3.norm();
    FP2_BN462_sub(&t1, &t1, &t2);     //t1.sub(t2);
    FP2_BN462_norm(&t1);              //t1.norm();
    FP2_BN462_imul(&y3, &y3, b3);     //y3.imul(b);
#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_mul_ip(&y3);
    FP2_BN462_norm(&y3);
#endif
    FP2_BN462_mul(&x3, &y3, &t4);     //x3.mul(t4);
    FP2_BN462_mul(&t2, &t3, &t1);     //t2.mul(t1);
    FP2_BN462_sub(&(P->x), &t2, &x3);     //x3.rsub(t2);
    FP2_BN462_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP2_BN462_mul(&t1, &t1, &z3);     //t1.mul(z3);
    FP2_BN462_add(&(P->y), &y3, &t1);     //y3.add(t1);
    FP2_BN462_mul(&t0, &t0, &t3);     //t0.mul(t3);
    FP2_BN462_mul(&z3, &z3, &t4);     //z3.mul(t4);
    FP2_BN462_add(&(P->z), &z3, &t0);     //z3.add(t0);

    FP2_BN462_norm(&(P->x));          //x.norm();
    FP2_BN462_norm(&(P->y));          //y.norm();
    FP2_BN462_norm(&(P->z));          //z.norm();

    return 0;
}

/* Set P-=Q */
/* SU= 16 */
void ECP2_BN462_sub(ECP2_BN462 *P, ECP2_BN462 *Q)
{
    ECP2_BN462 NQ;
    ECP2_BN462_copy(&NQ, Q);
    ECP2_BN462_neg(&NQ);
    ECP2_BN462_add(P, &NQ);
}

/* P*=e */
/* SU= 280 */
void ECP2_BN462_mul(ECP2_BN462 *P, BIG_464_28 e)
{
    /* fixed size windows */
    int i, nb, s, ns;
    BIG_464_28 mt, t;
    ECP2_BN462 Q, W[8], C;
    sign8 w[1 + (NLEN_464_28 * BASEBITS_464_28 + 3) / 4];

    if (ECP2_BN462_isinf(P)) return;

    /* precompute table */

    ECP2_BN462_copy(&Q, P);
    ECP2_BN462_dbl(&Q);
    ECP2_BN462_copy(&W[0], P);

    for (i = 1; i < 8; i++)
    {
        ECP2_BN462_copy(&W[i], &W[i - 1]);
        ECP2_BN462_add(&W[i], &Q);
    }

    /* make exponent odd - add 2P if even, P if odd */
    BIG_464_28_copy(t, e);
    s = BIG_464_28_parity(t);
    BIG_464_28_inc(t, 1);
    BIG_464_28_norm(t);
    ns = BIG_464_28_parity(t);
    BIG_464_28_copy(mt, t);
    BIG_464_28_inc(mt, 1);
    BIG_464_28_norm(mt);
    BIG_464_28_cmove(t, mt, s);
    ECP2_BN462_cmove(&Q, P, ns);
    ECP2_BN462_copy(&C, &Q);

    nb = 1 + (BIG_464_28_nbits(t) + 3) / 4;

    /* convert exponent to signed 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_464_28_lastbits(t, 5) - 16;
        BIG_464_28_dec(t, w[i]);
        BIG_464_28_norm(t);
        BIG_464_28_fshr(t, 4);
    }
    w[nb] = BIG_464_28_lastbits(t, 5);

    //ECP2_BN462_copy(P, &W[(w[nb] - 1) / 2]);
    ECP2_BN462_select(P, W, w[i]);
    for (i = nb - 1; i >= 0; i--)
    {
        ECP2_BN462_select(&Q, W, w[i]);
        ECP2_BN462_dbl(P);
        ECP2_BN462_dbl(P);
        ECP2_BN462_dbl(P);
        ECP2_BN462_dbl(P);
        ECP2_BN462_add(P, &Q);
    }
    ECP2_BN462_sub(P, &C); /* apply correction */
}

/* Calculates q.P using Frobenius constant X */
/* SU= 96 */
void ECP2_BN462_frob(ECP2_BN462 *P, FP2_BN462 *X)
{
    FP2_BN462 X2;

    FP2_BN462_sqr(&X2, X);
    FP2_BN462_conj(&(P->x), &(P->x));
    FP2_BN462_conj(&(P->y), &(P->y));
    FP2_BN462_conj(&(P->z), &(P->z));
    FP2_BN462_reduce(&(P->z));

    FP2_BN462_mul(&(P->x), &X2, &(P->x));
    FP2_BN462_mul(&(P->y), &X2, &(P->y));
    FP2_BN462_mul(&(P->y), X, &(P->y));

}


// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf
// Side channel attack secure

void ECP2_BN462_mul4(ECP2_BN462 *P, ECP2_BN462 Q[4], BIG_464_28 u[4])
{
    int i, j, k, nb, pb, bt;
    ECP2_BN462 T[8], W;
    BIG_464_28 t[4], mt;
    sign8 w[NLEN_464_28 * BASEBITS_464_28 + 1];
    sign8 s[NLEN_464_28 * BASEBITS_464_28 + 1];

    for (i = 0; i < 4; i++)
    {
        BIG_464_28_copy(t[i], u[i]);
    }

// Precomputed table
    ECP2_BN462_copy(&T[0], &Q[0]); // Q[0]
    ECP2_BN462_copy(&T[1], &T[0]);
    ECP2_BN462_add(&T[1], &Q[1]); // Q[0]+Q[1]
    ECP2_BN462_copy(&T[2], &T[0]);
    ECP2_BN462_add(&T[2], &Q[2]); // Q[0]+Q[2]
    ECP2_BN462_copy(&T[3], &T[1]);
    ECP2_BN462_add(&T[3], &Q[2]); // Q[0]+Q[1]+Q[2]
    ECP2_BN462_copy(&T[4], &T[0]);
    ECP2_BN462_add(&T[4], &Q[3]); // Q[0]+Q[3]
    ECP2_BN462_copy(&T[5], &T[1]);
    ECP2_BN462_add(&T[5], &Q[3]); // Q[0]+Q[1]+Q[3]
    ECP2_BN462_copy(&T[6], &T[2]);
    ECP2_BN462_add(&T[6], &Q[3]); // Q[0]+Q[2]+Q[3]
    ECP2_BN462_copy(&T[7], &T[3]);
    ECP2_BN462_add(&T[7], &Q[3]); // Q[0]+Q[1]+Q[2]+Q[3]

// Make it odd
    pb = 1 - BIG_464_28_parity(t[0]);
    BIG_464_28_inc(t[0], pb);
    BIG_464_28_norm(t[0]);

// Number of bits
    BIG_464_28_zero(mt);
    for (i = 0; i < 4; i++)
    {
        BIG_464_28_or(mt, mt, t[i]);
    }
    nb = 1 + BIG_464_28_nbits(mt);

// Sign pivot
    s[nb - 1] = 1;
    for (i = 0; i < nb - 1; i++)
    {
        BIG_464_28_fshr(t[0], 1);
        s[i] = 2 * BIG_464_28_parity(t[0]) - 1;
    }

// Recoded exponent
    for (i = 0; i < nb; i++)
    {
        w[i] = 0;
        k = 1;
        for (j = 1; j < 4; j++)
        {
            bt = s[i] * BIG_464_28_parity(t[j]);
            BIG_464_28_fshr(t[j], 1);

            BIG_464_28_dec(t[j], (bt >> 1));
            BIG_464_28_norm(t[j]);
            w[i] += bt * k;
            k *= 2;
        }
    }

// Main loop
    ECP2_BN462_select(P, T, 2 * w[nb - 1] + 1);
    for (i = nb - 2; i >= 0; i--)
    {
        ECP2_BN462_select(&W, T, 2 * w[i] + s[i]);
        ECP2_BN462_dbl(P);
        ECP2_BN462_add(P, &W);
    }

// apply correction
    ECP2_BN462_copy(&W, P);
    ECP2_BN462_sub(&W, &Q[0]);
    ECP2_BN462_cmove(P, &W, pb);
}

/* Hunt and Peck a BIG to a curve point */
/*
void ECP2_BN462_hap2point(ECP2_BN462 *Q,BIG_464_28 h)
{
    BIG_464_28 one,hv;
    FP2_BN462 X;
    BIG_464_28_one(one);
    BIG_464_28_copy(hv,h);
    for (;;)
    {
        FP2_BN462_from_BIGs(&X,one,hv);
        if (ECP2_BN462_setx(Q,&X,0)) break;
        BIG_464_28_inc(hv,1);
        BIG_464_28_norm(hv);
    }
}
*/
/* Constant time Map to Point in G2 */
void ECP2_BN462_map2point(ECP2_BN462 *Q,FP2_BN462 *H)
{ // SSWU plus isogenies method

    int i,k,sgn,ne,isox,isoy,iso=HTC_ISO_G2_BN462;
    FP2_BN462 X1,X2,X3,W,Y,T,A,NY;
    FP_BN462 s;
#if HTC_ISO_G2_BN462 != 0
    FP_BN462 hint;
    FP2_BN462 ZZ,Ad,Bd,D,D2,GX1;
    FP2_BN462 xnum,xden,ynum,yden;
    FP2_BN462_from_ints(&ZZ,RIADZG2A_BN462,RIADZG2B_BN462);

    FP2_BN462_rcopy(&Ad,CURVE_Adr_BN462,CURVE_Adi_BN462);
    FP2_BN462_rcopy(&Bd,CURVE_Bdr_BN462,CURVE_Bdi_BN462);

    FP2_BN462_one(&NY);
    FP2_BN462_copy(&T,H);
    sgn=FP2_BN462_sign(&T);

    FP2_BN462_sqr(&T,&T);
    FP2_BN462_mul(&T,&T,&ZZ);
    FP2_BN462_add(&W,&T,&NY);
    FP2_BN462_norm(&W);

    FP2_BN462_mul(&W,&W,&T);
    FP2_BN462_mul(&D,&Ad,&W);

    FP2_BN462_add(&W,&W,&NY);
    FP2_BN462_norm(&W);
    FP2_BN462_mul(&W,&W,&Bd);
    FP2_BN462_neg(&W,&W);
    FP2_BN462_norm(&W);

    FP2_BN462_copy(&X2,&W);
    FP2_BN462_mul(&X3,&T,&X2);

// x^3+Ad^2x+Bd^3
    FP2_BN462_sqr(&GX1,&X2);
    FP2_BN462_sqr(&D2,&D); FP2_BN462_mul(&W,&Ad,&D2); FP2_BN462_add(&GX1,&GX1,&W); FP2_BN462_norm(&GX1); FP2_BN462_mul(&GX1,&GX1,&X2); FP2_BN462_mul(&D2,&D2,&D); FP2_BN462_mul(&W,&Bd,&D2); FP2_BN462_add(&GX1,&GX1,&W); FP2_BN462_norm(&GX1);

    FP2_BN462_mul(&W,&GX1,&D);
    int qr=FP2_BN462_qr(&W,&hint);   // qr(ad) - only exp happens here
    FP2_BN462_inv(&D,&W,&hint);     // d=1/(ad)
    FP2_BN462_mul(&D,&D,&GX1);     // 1/d
    FP2_BN462_mul(&X2,&X2,&D);     // X2/=D
    FP2_BN462_mul(&X3,&X3,&D);     // X3/=D
    FP2_BN462_mul(&T,&T,H);        // t=Z.u^3
    FP2_BN462_sqr(&D2,&D);

// first solution - X2, W, hint, D2

    FP2_BN462_mul(&D,&D2,&T);     // second candidate if X3 is correct
    FP2_BN462_mul(&T,&W,&ZZ); 

    FP_BN462_rcopy(&s,CURVE_HTPC2_BN462);     
    FP_BN462_mul(&s,&s,&hint); // modify hint

    FP2_BN462_cmove(&X2,&X3,1-qr); 
    FP2_BN462_cmove(&W,&T,1-qr);
    FP2_BN462_cmove(&D2,&D,1-qr);
    FP_BN462_cmove(&hint,&s,1-qr);

    FP2_BN462_sqrt(&Y,&W,&hint);  // first candidate if X2 is correct
    FP2_BN462_mul(&Y,&Y,&D2);

    ne=FP2_BN462_sign(&Y)^sgn;
    FP2_BN462_neg(&NY,&Y); FP2_BN462_norm(&NY);
    FP2_BN462_cmove(&Y,&NY,ne);

// (X2,Y) is on isogenous curve

    k=0;
    isox=iso;
    isoy=3*(iso-1)/2;

// xnum
    FP2_BN462_rcopy(&xnum,PCR_BN462[k],PCI_BN462[k]); k++;
    for (i=0;i<isox;i++)
    {
        FP2_BN462_mul(&xnum,&xnum,&X2); 
        FP2_BN462_rcopy(&W,PCR_BN462[k],PCI_BN462[k]); k++;
        FP2_BN462_add(&xnum,&xnum,&W); FP2_BN462_norm(&xnum);
    }

// xden
    FP2_BN462_copy(&xden,&X2);
    FP2_BN462_rcopy(&W,PCR_BN462[k],PCI_BN462[k]); k++;
    FP2_BN462_add(&xden,&xden,&W); FP2_BN462_norm(&xden);
    for (i=0;i<isox-2;i++)
    {
        FP2_BN462_mul(&xden,&xden,&X2);
        FP2_BN462_rcopy(&W,PCR_BN462[k],PCI_BN462[k]); k++;
        FP2_BN462_add(&xden,&xden,&W); FP2_BN462_norm(&xden);
    }

// ynum
        FP2_BN462_rcopy(&ynum,PCR_BN462[k],PCI_BN462[k]); k++;
        for (i=0;i<isoy;i++)
        {
            FP2_BN462_mul(&ynum,&ynum,&X2); 
            FP2_BN462_rcopy(&W,PCR_BN462[k],PCI_BN462[k]); k++;
            FP2_BN462_add(&ynum,&ynum,&W); FP2_BN462_norm(&ynum);
        }

// yden 
        FP2_BN462_copy(&yden,&X2);
        FP2_BN462_rcopy(&W,PCR_BN462[k],PCI_BN462[k]); k++;
        FP2_BN462_add(&yden,&yden,&W); FP2_BN462_norm(&yden);
      
        for (i=0;i<isoy-1;i++)
        {
            FP2_BN462_mul(&yden,&yden,&X2); 
            FP2_BN462_rcopy(&W,PCR_BN462[k],PCI_BN462[k]); k++;
            FP2_BN462_add(&yden,&yden,&W); FP2_BN462_norm(&yden);
        }

        FP2_BN462_mul(&ynum,&ynum,&Y);



        FP2_BN462_mul(&T,&xnum,&yden);
        FP2_BN462_copy(&(Q->x),&T);

        FP2_BN462_mul(&T,&ynum,&xden);
        FP2_BN462_copy(&(Q->y),&T);

        FP2_BN462_mul(&T,&xden,&yden);
        FP2_BN462_copy(&(Q->z),&T);

#else
    FP_BN462 Z;
    FP2_BN462_one(&NY);
    FP2_BN462_copy(&T,H);
    sgn=FP2_BN462_sign(&T);

    FP_BN462_from_int(&Z,RIADZG2A_BN462);
    FP2_BN462_from_FP(&A,&Z);
    ECP2_BN462_rhs(&A,&A);  // A=g(Z)

   if (CURVE_B_I_BN462==4 && SEXTIC_TWIST_BN462==M_TYPE && RIADZG2A_BN462==-1 && RIADZG2B_BN462==0)
    { // special case for BLS12381
        FP2_BN462_from_ints(&W,2,1);
    } else {
        FP2_BN462_sqrt(&W,&A,NULL);   // sqrt(g(Z))
    }

    FP_BN462_rcopy(&s,SQRTm3_BN462);

    FP_BN462_mul(&Z,&Z,&s);     // Z.sqrt(-3)

    FP2_BN462_sqr(&T,&T);
    FP2_BN462_mul(&Y,&A,&T);   // tv1=u^2*g(Z)
    FP2_BN462_add(&T,&NY,&Y); FP2_BN462_norm(&T); // tv2=1+tv1
    FP2_BN462_sub(&Y,&NY,&Y); FP2_BN462_norm(&Y); // tv1=1-tv1
    FP2_BN462_mul(&NY,&T,&Y);

    FP2_BN462_pmul(&NY,&NY,&Z);

    FP2_BN462_inv(&NY,&NY,NULL);     // tv3=inv0(tv1*tv2)

    FP2_BN462_pmul(&W,&W,&Z); // tv4=Z*sqrt(-3).sqrt(g(Z))
    if (FP2_BN462_sign(&W)==1)
    {
        FP2_BN462_neg(&W,&W);
        FP2_BN462_norm(&W);
    }
    FP2_BN462_pmul(&W,&W,&Z);
    FP2_BN462_mul(&W,&W,H);
    FP2_BN462_mul(&W,&W,&Y);
    FP2_BN462_mul(&W,&W,&NY);     // tv5=u*tv1*tv3*tv4*Z*sqrt(-3)

    FP2_BN462_from_ints(&X1,RIADZG2A_BN462,RIADZG2B_BN462);
    FP2_BN462_copy(&X3,&X1);
    FP2_BN462_neg(&X1,&X1); FP2_BN462_norm(&X1); FP2_BN462_div2(&X1,&X1); // -Z/2
    FP2_BN462_copy(&X2,&X1);
    FP2_BN462_sub(&X1,&X1,&W); FP2_BN462_norm(&X1);
    FP2_BN462_add(&X2,&X2,&W); FP2_BN462_norm(&X2);
    FP2_BN462_add(&A,&A,&A);
    FP2_BN462_add(&A,&A,&A);
    FP2_BN462_norm(&A);      // 4*g(Z)
    FP2_BN462_sqr(&T,&T);
    FP2_BN462_mul(&T,&T,&NY);
    FP2_BN462_sqr(&T,&T);    // (tv2^2*tv3)^2
    FP2_BN462_mul(&A,&A,&T); // 4*g(Z)*(tv2^2*tv3)^2
    FP2_BN462_add(&X3,&X3,&A); FP2_BN462_norm(&X3);


    ECP2_BN462_rhs(&W,&X2);
    FP2_BN462_cmove(&X3,&X2,FP2_BN462_qr(&W,NULL));
    ECP2_BN462_rhs(&W,&X1);
    FP2_BN462_cmove(&X3,&X1,FP2_BN462_qr(&W,NULL));
    ECP2_BN462_rhs(&W,&X3);
    FP2_BN462_sqrt(&Y,&W,NULL);

    ne=FP2_BN462_sign(&Y)^sgn;
    FP2_BN462_neg(&W,&Y); FP2_BN462_norm(&W);
    FP2_BN462_cmove(&Y,&W,ne);

    ECP2_BN462_set(Q,&X3,&Y);
#endif
}

/* Map octet to point */
/*
void ECP2_BN462_mapit(ECP2_BN462 *Q, octet *W)
{
    BIG_464_28 q, x;
    DBIG_464_28 dx;
    BIG_464_28_rcopy(q, Modulus_BN462);

    BIG_464_28_dfromBytesLen(dx,W->val,W->len);
    BIG_464_28_dmod(x,dx,q);


    ECP2_BN462_hap2point(Q,x);
    ECP2_BN462_cfp(Q);
}
*/
/* cofactor product */
void ECP2_BN462_cfp(ECP2_BN462 *Q)
{
    FP_BN462 Fx, Fy;
    FP2_BN462 X;
    BIG_464_28 x;
#if (PAIRING_FRIENDLY_BN462 == BN_CURVE)
    ECP2_BN462 T, K;
#elif (PAIRING_FRIENDLY_BN462 > BN_CURVE)
    ECP2_BN462 xQ, x2Q;
#endif
    FP_BN462_rcopy(&Fx, Fra_BN462);
    FP_BN462_rcopy(&Fy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &Fx, &Fy);

#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_inv(&X, &X,NULL);
    FP2_BN462_norm(&X);
#endif

    BIG_464_28_rcopy(x, CURVE_Bnx_BN462);

#if (PAIRING_FRIENDLY_BN462 == BN_CURVE)

    // Faster Hashing to G2 - Fuentes-Castaneda, Knapp and Rodriguez-Henriquez
    // Q -> xQ + F(3xQ) + F(F(xQ)) + F(F(F(Q))).
    ECP2_BN462_copy(&T, Q);
    ECP2_BN462_mul(&T, x);
#if SIGN_OF_X_BN462==NEGATIVEX
    ECP2_BN462_neg(&T);   // our x is negative
#endif
    ECP2_BN462_copy(&K, &T);
    ECP2_BN462_dbl(&K);
    ECP2_BN462_add(&K, &T);

    ECP2_BN462_frob(&K, &X);
    ECP2_BN462_frob(Q, &X);
    ECP2_BN462_frob(Q, &X);
    ECP2_BN462_frob(Q, &X);
    ECP2_BN462_add(Q, &T);
    ECP2_BN462_add(Q, &K);
    ECP2_BN462_frob(&T, &X);
    ECP2_BN462_frob(&T, &X);
    ECP2_BN462_add(Q, &T);

#elif (PAIRING_FRIENDLY_BN462 > BN_CURVE)

    // Efficient hash maps to G2 on BLS curves - Budroni, Pintore
    // Q -> x2Q -xQ -Q +F(xQ -Q) +F(F(2Q))

    ECP2_BN462_copy(&xQ, Q);
    ECP2_BN462_mul(&xQ, x);
    ECP2_BN462_copy(&x2Q, &xQ);
    ECP2_BN462_mul(&x2Q, x);

#if SIGN_OF_X_BN462==NEGATIVEX
    ECP2_BN462_neg(&xQ);
#endif

    ECP2_BN462_sub(&x2Q, &xQ);
    ECP2_BN462_sub(&x2Q, Q);

    ECP2_BN462_sub(&xQ, Q);
    ECP2_BN462_frob(&xQ, &X);

    ECP2_BN462_dbl(Q);
    ECP2_BN462_frob(Q, &X);
    ECP2_BN462_frob(Q, &X);

    ECP2_BN462_add(Q, &x2Q);
    ECP2_BN462_add(Q, &xQ);

#endif
}

int ECP2_BN462_generator(ECP2_BN462 *G)
{
    FP2_BN462 wx, wy;

    FP2_BN462_rcopy(&wx,CURVE_Pxa_BN462,CURVE_Pxb_BN462);
    FP2_BN462_rcopy(&wy,CURVE_Pya_BN462,CURVE_Pyb_BN462);

    return ECP2_BN462_set(G, &wx, &wy);
}

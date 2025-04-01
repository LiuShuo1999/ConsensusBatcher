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

#include "ecp4_BLS24479.h"

int ECP4_BLS24479_isinf(ECP4_BLS24479 *P)
{
    return (FP4_BLS24479_iszilch(&(P->x)) & FP4_BLS24479_iszilch(&(P->z)));
}

/* Set P=Q */
void ECP4_BLS24479_copy(ECP4_BLS24479 *P, ECP4_BLS24479 *Q)
{
    FP4_BLS24479_copy(&(P->x), &(Q->x));
    FP4_BLS24479_copy(&(P->y), &(Q->y));
    FP4_BLS24479_copy(&(P->z), &(Q->z));
}

/* set P to Infinity */
void ECP4_BLS24479_inf(ECP4_BLS24479 *P)
{
    FP4_BLS24479_zero(&(P->x));
    FP4_BLS24479_one(&(P->y));
    FP4_BLS24479_zero(&(P->z));
}

/* Conditional move Q to P dependant on d */
static void ECP4_BLS24479_cmove(ECP4_BLS24479 *P, ECP4_BLS24479 *Q, int d)
{
    FP4_BLS24479_cmove(&(P->x), &(Q->x), d);
    FP4_BLS24479_cmove(&(P->y), &(Q->y), d);
    FP4_BLS24479_cmove(&(P->z), &(Q->z), d);

}

/* return 1 if b==c, no branching */
static int teq(sign32 b, sign32 c)
{
    sign32 x = b ^ c;
    x -= 1; // if x=0, x now -1
    return (int)((x >> 31) & 1);
}

/* Constant time select from pre-computed table */
static void ECP4_BLS24479_select(ECP4_BLS24479 *P, ECP4_BLS24479 W[], sign32 b)
{
    ECP4_BLS24479 MP;
    sign32 m = b >> 31;
    sign32 babs = (b ^ m) - m;

    babs = (babs - 1) / 2;

    ECP4_BLS24479_cmove(P, &W[0], teq(babs, 0)); // conditional move
    ECP4_BLS24479_cmove(P, &W[1], teq(babs, 1));
    ECP4_BLS24479_cmove(P, &W[2], teq(babs, 2));
    ECP4_BLS24479_cmove(P, &W[3], teq(babs, 3));
    ECP4_BLS24479_cmove(P, &W[4], teq(babs, 4));
    ECP4_BLS24479_cmove(P, &W[5], teq(babs, 5));
    ECP4_BLS24479_cmove(P, &W[6], teq(babs, 6));
    ECP4_BLS24479_cmove(P, &W[7], teq(babs, 7));

    ECP4_BLS24479_copy(&MP, P);
    ECP4_BLS24479_neg(&MP);  // minus P
    ECP4_BLS24479_cmove(P, &MP, (int)(m & 1));
}

/* Make P affine (so z=1) */
void ECP4_BLS24479_affine(ECP4_BLS24479 *P)
{
    FP4_BLS24479 one, iz;
    if (ECP4_BLS24479_isinf(P)) return;

    FP4_BLS24479_one(&one);
    if (FP4_BLS24479_isunity(&(P->z)))
    {
        FP4_BLS24479_reduce(&(P->x));
        FP4_BLS24479_reduce(&(P->y));
        return;
    }

    FP4_BLS24479_inv(&iz, &(P->z),NULL);
    FP4_BLS24479_mul(&(P->x), &(P->x), &iz);
    FP4_BLS24479_mul(&(P->y), &(P->y), &iz);

    FP4_BLS24479_reduce(&(P->x));
    FP4_BLS24479_reduce(&(P->y));
    FP4_BLS24479_copy(&(P->z), &one);
}

/* return 1 if P==Q, else 0 */
/* SU= 312 */
int ECP4_BLS24479_equals(ECP4_BLS24479 *P, ECP4_BLS24479 *Q)
{
    FP4_BLS24479 a, b;

    FP4_BLS24479_mul(&a, &(P->x), &(Q->z));
    FP4_BLS24479_mul(&b, &(Q->x), &(P->z));
    if (!FP4_BLS24479_equals(&a, &b)) return 0;

    FP4_BLS24479_mul(&a, &(P->y), &(Q->z));
    FP4_BLS24479_mul(&b, &(Q->y), &(P->z));
    if (!FP4_BLS24479_equals(&a, &b)) return 0;
    return 1;

}

/* extract x, y from point P */
int ECP4_BLS24479_get(FP4_BLS24479 *x, FP4_BLS24479 *y, ECP4_BLS24479 *P)
{
    ECP4_BLS24479 W;
    ECP4_BLS24479_copy(&W, P);
    ECP4_BLS24479_affine(&W);
    if (ECP4_BLS24479_isinf(&W)) return -1;
    FP4_BLS24479_copy(y, &(W.y));
    FP4_BLS24479_copy(x, &(W.x));
    return 0;
}

/* Output point P */
void ECP4_BLS24479_output(ECP4_BLS24479 *P)
{
    FP4_BLS24479 x, y;
    if (ECP4_BLS24479_isinf(P))
    {
        printf("Infinity\n");
        return;
    }
    ECP4_BLS24479_get(&x, &y, P);
    printf("(");
    FP4_BLS24479_output(&x);
    printf(",");
    FP4_BLS24479_output(&y);
    printf(")\n");
}

/* Convert Q to octet string */
void ECP4_BLS24479_toOctet(octet *W, ECP4_BLS24479 *Q,bool compress)
{
    FP4_BLS24479 qx, qy;
    bool alt=false;
    ECP4_BLS24479_get(&qx, &qy, Q);
  
#if (MBITS-1)%8 <= 4
#ifdef ALLOW_ALT_COMPRESS_BLS24479
    alt=true;
#endif
#endif

    if (alt)
    {
        FP4_BLS24479_toBytes(&(W->val[0]),&qx);
        if (!compress)
        {
            W->len=8*MODBYTES_480_29;
            FP4_BLS24479_toBytes(&(W->val[4*MODBYTES_480_29]), &qy);
        } else {
            W->val[0]|=0x80;
            if (FP4_BLS24479_islarger(&qy)==1) W->val[0]|=0x20;
            W->len=4*MODBYTES_480_29;
        }
    } else {
        FP4_BLS24479_toBytes(&(W->val[1]),&qx);
        if (!compress)
        {
            W->val[0] = 0x04;
            FP4_BLS24479_toBytes(&(W->val[4 * MODBYTES_480_29+1]), &qy);
            W->len = 8 * MODBYTES_480_29+1;
        } else {
            W->val[0]=0x02;
            if (FP4_BLS24479_sign(&qy)==1) W->val[0] = 0x03;
            W->len = 4 * MODBYTES_480_29+1;
        }
    }
}

/* restore Q from octet string */
int ECP4_BLS24479_fromOctet(ECP4_BLS24479 *Q, octet *W)
{
    FP4_BLS24479 qx, qy;
    bool alt=false;
    int sgn,cmp,typ = W->val[0];

#if (MBITS-1)%8 <= 4
#ifdef ALLOW_ALT_COMPRESS_BLS24479
    alt=true;
#endif
#endif

    if (alt)
    {
        W->val[0]&=0x1f;
        FP4_BLS24479_fromBytes(&qx,&(W->val[0]));
        W->val[0]=typ;
        if ((typ&0x80)==0)
        {
            FP4_BLS24479_fromBytes(&qy,&(W->val[4*MODBYTES_480_29]));
            if (ECP4_BLS24479_set(Q, &qx, &qy)) return 1;
            return 0;
        } else {
            if (!ECP4_BLS24479_setx(Q,&qx,0)) return 0;
            sgn=(typ&0x20)>>5;
            cmp=FP4_BLS24479_islarger(&(Q->y));
            if ((sgn==1 && cmp!=1) || (sgn==0 && cmp==1)) ECP4_BLS24479_neg(Q);
            return 1;
        }
    } else {
        FP4_BLS24479_fromBytes(&qx,&(W->val[1]));
        if (typ == 0x04)
        {
            FP4_BLS24479_fromBytes(&qy,&(W->val[4 * MODBYTES_480_29+1]));
            if (ECP4_BLS24479_set(Q, &qx, &qy)) return 1;
        } else {
            if (ECP4_BLS24479_setx(Q, &qx, typ&1)) return 1;
        }
    }
    return 0;
}

/* Calculate RHS of twisted curve equation x^3+B/i or x^3+Bi*/
void ECP4_BLS24479_rhs(FP4_BLS24479 *rhs, FP4_BLS24479 *x)
{
    /* calculate RHS of elliptic curve equation */
    FP4_BLS24479 t;
    FP2_BLS24479 t2;
    BIG_480_29 b;
    FP4_BLS24479_sqr(&t, x);

    FP4_BLS24479_mul(rhs, &t, x);

    /* Assuming CURVE_A=0 */

    BIG_480_29_rcopy(b, CURVE_B_BLS24479);

    FP2_BLS24479_from_BIG(&t2, b);
    FP4_BLS24479_from_FP2(&t, &t2);

#if SEXTIC_TWIST_BLS24479 == D_TYPE
    FP4_BLS24479_div_i(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

#if SEXTIC_TWIST_BLS24479 == M_TYPE
    FP4_BLS24479_times_i(&t);   /* IMPORTANT - here we use the correct SEXTIC twist of the curve */
#endif

    FP4_BLS24479_add(rhs, &t, rhs);
    FP4_BLS24479_reduce(rhs);
}

/* Set P=(x,y). Return 1 if (x,y) is on the curve, else return 0*/
/* SU= 232 */
int ECP4_BLS24479_set(ECP4_BLS24479 *P, FP4_BLS24479 *x, FP4_BLS24479 *y)
{
    FP4_BLS24479 rhs, y2;

    FP4_BLS24479_sqr(&y2, y);
    ECP4_BLS24479_rhs(&rhs, x);

    if (!FP4_BLS24479_equals(&y2, &rhs))
    {
        ECP4_BLS24479_inf(P);
        return 0;
    }

//   P->inf=0;
    FP4_BLS24479_copy(&(P->x), x);
    FP4_BLS24479_copy(&(P->y), y);

    FP4_BLS24479_one(&(P->z));
    return 1;
}

/* Set P=(x,y). Return 1 if (x,.) is on the curve, else return 0 */
/* SU= 232 */
int ECP4_BLS24479_setx(ECP4_BLS24479 *P, FP4_BLS24479 *x,int s)
{
    FP4_BLS24479 y;
    FP_BLS24479 h;
    ECP4_BLS24479_rhs(&y, x);

    if (!FP4_BLS24479_qr(&y,&h))
    {
        ECP4_BLS24479_inf(P);
        return 0;
    }
    FP4_BLS24479_sqrt(&y, &y, &h);
    FP4_BLS24479_copy(&(P->x), x);
    FP4_BLS24479_copy(&(P->y), &y);
    FP4_BLS24479_one(&(P->z));
    if (FP4_BLS24479_sign(&(P->y)) != s)
        FP4_BLS24479_neg(&(P->y),&(P->y));
    FP4_BLS24479_reduce(&(P->y));
    return 1;
}

/* Set P=-P */
/* SU= 8 */
void ECP4_BLS24479_neg(ECP4_BLS24479 *P)
{
    FP4_BLS24479_norm(&(P->y));
    FP4_BLS24479_neg(&(P->y), &(P->y));
    FP4_BLS24479_norm(&(P->y));
}


/* R+=R */
/* return -1 for Infinity, 0 for addition, 1 for doubling */
int ECP4_BLS24479_dbl(ECP4_BLS24479 *P)
{
    FP4_BLS24479 t0, t1, t2, t3, iy, x3, y3;

    FP4_BLS24479_copy(&iy, &(P->y));     //FP4_BLS24479 iy=new FP4_BLS24479(y);
#if SEXTIC_TWIST_BLS24479==D_TYPE
    FP4_BLS24479_times_i(&iy);           //iy.mul_ip();
#endif

    FP4_BLS24479_sqr(&t0, &(P->y));          //t0.sqr();
#if SEXTIC_TWIST_BLS24479==D_TYPE
    FP4_BLS24479_times_i(&t0);           //t0.mul_ip();
#endif

    FP4_BLS24479_mul(&t1, &iy, &(P->z)); //t1.mul(z);
    FP4_BLS24479_sqr(&t2, &(P->z));              //t2.sqr();

    FP4_BLS24479_add(&(P->z), &t0, &t0); //z.add(t0);
    FP4_BLS24479_norm(&(P->z));              //z.norm();
    FP4_BLS24479_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP4_BLS24479_add(&(P->z), &(P->z), &(P->z)); //z.add(z);
    FP4_BLS24479_norm(&(P->z));          //z.norm();

    FP4_BLS24479_imul(&t2, &t2, 3 * CURVE_B_I_BLS24479); //t2.imul(3*ROM.CURVE_B_I);
#if SEXTIC_TWIST_BLS24479==M_TYPE
    FP4_BLS24479_times_i(&t2);
#endif

    FP4_BLS24479_mul(&x3, &t2, &(P->z)); //x3.mul(z);

    FP4_BLS24479_add(&y3, &t0, &t2);     //y3.add(t2);
    FP4_BLS24479_norm(&y3);              //y3.norm();
    FP4_BLS24479_mul(&(P->z), &(P->z), &t1); //z.mul(t1);

    FP4_BLS24479_add(&t1, &t2, &t2);     //t1.add(t2);
    FP4_BLS24479_add(&t2, &t2, &t1);     //t2.add(t1);
    FP4_BLS24479_norm(&t2);              //t2.norm();
    FP4_BLS24479_sub(&t0, &t0, &t2);     //t0.sub(t2);
    FP4_BLS24479_norm(&t0);              //t0.norm();                           //y^2-9bz^2
    FP4_BLS24479_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP4_BLS24479_add(&(P->y), &y3, &x3);     //y3.add(x3);                          //(y^2+3z*2)(y^2-9z^2)+3b.z^2.8y^2

    FP4_BLS24479_mul(&t1, &(P->x), &iy);     //t1.mul(iy);                       //

    FP4_BLS24479_norm(&t0);              //x.norm();
    FP4_BLS24479_mul(&(P->x), &t0, &t1); //x.mul(t1);
    FP4_BLS24479_add(&(P->x), &(P->x), &(P->x)); //x.add(x);       //(y^2-9bz^2)xy2

    FP4_BLS24479_norm(&(P->x));          //x.norm();

    FP4_BLS24479_norm(&(P->y));          //y.norm();

    return 1;
}

/* Set P+=Q */

int ECP4_BLS24479_add(ECP4_BLS24479 *P, ECP4_BLS24479 *Q)
{
    FP4_BLS24479 t0, t1, t2, t3, t4, x3, y3, z3;
    int b3 = 3 * CURVE_B_I_BLS24479;

    FP4_BLS24479_mul(&t0, &(P->x), &(Q->x)); //t0.mul(Q.x);         // x.Q.x
    FP4_BLS24479_mul(&t1, &(P->y), &(Q->y)); //t1.mul(Q.y);       // y.Q.y

    FP4_BLS24479_mul(&t2, &(P->z), &(Q->z)); //t2.mul(Q.z);
    FP4_BLS24479_add(&t3, &(P->x), &(P->y)); //t3.add(y);
    FP4_BLS24479_norm(&t3);              //t3.norm();          //t3=X1+Y1
    FP4_BLS24479_add(&t4, &(Q->x), &(Q->y)); //t4.add(Q.y);
    FP4_BLS24479_norm(&t4);              //t4.norm();            //t4=X2+Y2
    FP4_BLS24479_mul(&t3, &t3, &t4);     //t3.mul(t4);                       //t3=(X1+Y1)(X2+Y2)
    FP4_BLS24479_add(&t4, &t0, &t1);     //t4.add(t1);       //t4=X1.X2+Y1.Y2

    FP4_BLS24479_sub(&t3, &t3, &t4);     //t3.sub(t4);
    FP4_BLS24479_norm(&t3);              //t3.norm();
#if SEXTIC_TWIST_BLS24479==D_TYPE
    FP4_BLS24479_times_i(&t3);           //t3.mul_ip();  //t3=(X1+Y1)(X2+Y2)-(X1.X2+Y1.Y2) = X1.Y2+X2.Y1
#endif

    FP4_BLS24479_add(&t4, &(P->y), &(P->z)); //t4.add(z);
    FP4_BLS24479_norm(&t4);              //t4.norm();            //t4=Y1+Z1

    FP4_BLS24479_add(&x3, &(Q->y), &(Q->z)); //x3.add(Q.z);
    FP4_BLS24479_norm(&x3);              //x3.norm();            //x3=Y2+Z2

    FP4_BLS24479_mul(&t4, &t4, &x3);     //t4.mul(x3);                       //t4=(Y1+Z1)(Y2+Z2)

    FP4_BLS24479_add(&x3, &t1, &t2);     //x3.add(t2);                       //X3=Y1.Y2+Z1.Z2

    FP4_BLS24479_sub(&t4, &t4, &x3);     //t4.sub(x3);
    FP4_BLS24479_norm(&t4);              //t4.norm();
#if SEXTIC_TWIST_BLS24479==D_TYPE
    FP4_BLS24479_times_i(&t4);           //t4.mul_ip(); //t4=(Y1+Z1)(Y2+Z2) - (Y1.Y2+Z1.Z2) = Y1.Z2+Y2.Z1
#endif

    FP4_BLS24479_add(&x3, &(P->x), &(P->z)); //x3.add(z);
    FP4_BLS24479_norm(&x3);              //x3.norm();    // x3=X1+Z1

    FP4_BLS24479_add(&y3, &(Q->x), &(Q->z)); //y3.add(Q.z);
    FP4_BLS24479_norm(&y3);              //y3.norm();                // y3=X2+Z2
    FP4_BLS24479_mul(&x3, &x3, &y3);     //x3.mul(y3);                           // x3=(X1+Z1)(X2+Z2)

    FP4_BLS24479_add(&y3, &t0, &t2);     //y3.add(t2);                           // y3=X1.X2+Z1+Z2
    FP4_BLS24479_sub(&y3, &x3, &y3);     //y3.rsub(x3);
    FP4_BLS24479_norm(&y3);              //y3.norm();                // y3=(X1+Z1)(X2+Z2) - (X1.X2+Z1.Z2) = X1.Z2+X2.Z1
#if SEXTIC_TWIST_BLS24479==D_TYPE
    FP4_BLS24479_times_i(&t0);           //t0.mul_ip();
    FP4_BLS24479_times_i(&t1);           //t1.mul_ip();
#endif

    FP4_BLS24479_add(&x3, &t0, &t0);     //x3.add(t0);
    FP4_BLS24479_add(&t0, &t0, &x3);     //t0.add(x3);
    FP4_BLS24479_norm(&t0);              //t0.norm();
    FP4_BLS24479_imul(&t2, &t2, b3);     //t2.imul(b);
#if SEXTIC_TWIST_BLS24479==M_TYPE
    FP4_BLS24479_times_i(&t2);
#endif

    FP4_BLS24479_add(&z3, &t1, &t2);     //z3.add(t2);
    FP4_BLS24479_norm(&z3);              //z3.norm();
    FP4_BLS24479_sub(&t1, &t1, &t2);     //t1.sub(t2);
    FP4_BLS24479_norm(&t1);              //t1.norm();
    FP4_BLS24479_imul(&y3, &y3, b3);     //y3.imul(b);
#if SEXTIC_TWIST_BLS24479==M_TYPE
    FP4_BLS24479_times_i(&y3);
#endif

    FP4_BLS24479_mul(&x3, &y3, &t4);     //x3.mul(t4);

    FP4_BLS24479_mul(&t2, &t3, &t1);     //t2.mul(t1);
    FP4_BLS24479_sub(&(P->x), &t2, &x3);     //x3.rsub(t2);
    FP4_BLS24479_mul(&y3, &y3, &t0);     //y3.mul(t0);
    FP4_BLS24479_mul(&t1, &t1, &z3);     //t1.mul(z3);
    FP4_BLS24479_add(&(P->y), &y3, &t1);     //y3.add(t1);
    FP4_BLS24479_mul(&t0, &t0, &t3);     //t0.mul(t3);
    FP4_BLS24479_mul(&z3, &z3, &t4);     //z3.mul(t4);
    FP4_BLS24479_add(&(P->z), &z3, &t0);     //z3.add(t0);


    FP4_BLS24479_norm(&(P->x));          //x.norm();
    FP4_BLS24479_norm(&(P->y));          //y.norm();
    FP4_BLS24479_norm(&(P->z));          //z.norm();

    return 0;
}

/* Set P-=Q */
/* SU= 16 */
void ECP4_BLS24479_sub(ECP4_BLS24479 *P, ECP4_BLS24479 *Q)
{
    ECP4_BLS24479 NQ;
    ECP4_BLS24479_copy(&NQ, Q);
    ECP4_BLS24479_neg(&NQ);
    ECP4_BLS24479_add(P, &NQ);
}


void ECP4_BLS24479_reduce(ECP4_BLS24479 *P)
{
    FP4_BLS24479_reduce(&(P->x));
    FP4_BLS24479_reduce(&(P->y));
    FP4_BLS24479_reduce(&(P->z));
}

/* P*=e */
/* SU= 280 */
void ECP4_BLS24479_mul(ECP4_BLS24479 *P, BIG_480_29 e)
{
    /* fixed size windows */
    int i, nb, s, ns;
    BIG_480_29 mt, t;
    ECP4_BLS24479 Q, W[8], C;
    sign8 w[1 + (NLEN_480_29 * BASEBITS_480_29 + 3) / 4];

    if (ECP4_BLS24479_isinf(P)) return;

    /* precompute table */

    ECP4_BLS24479_copy(&Q, P);
    ECP4_BLS24479_dbl(&Q);
    ECP4_BLS24479_copy(&W[0], P);

    for (i = 1; i < 8; i++)
    {
        ECP4_BLS24479_copy(&W[i], &W[i - 1]);
        ECP4_BLS24479_add(&W[i], &Q);
    }

    /* make exponent odd - add 2P if even, P if odd */
    BIG_480_29_copy(t, e);
    s = BIG_480_29_parity(t);
    BIG_480_29_inc(t, 1);
    BIG_480_29_norm(t);
    ns = BIG_480_29_parity(t);
    BIG_480_29_copy(mt, t);
    BIG_480_29_inc(mt, 1);
    BIG_480_29_norm(mt);
    BIG_480_29_cmove(t, mt, s);
    ECP4_BLS24479_cmove(&Q, P, ns);
    ECP4_BLS24479_copy(&C, &Q);

    nb = 1 + (BIG_480_29_nbits(t) + 3) / 4;

    /* convert exponent to signed 4-bit window */
    for (i = 0; i < nb; i++)
    {
        w[i] = BIG_480_29_lastbits(t, 5) - 16;
        BIG_480_29_dec(t, w[i]);
        BIG_480_29_norm(t);
        BIG_480_29_fshr(t, 4);
    }
    w[nb] = BIG_480_29_lastbits(t, 5);

    //ECP4_BLS24479_copy(P, &W[(w[nb] - 1) / 2]);
    ECP4_BLS24479_select(P, W, w[i]);
    for (i = nb - 1; i >= 0; i--)
    {
        ECP4_BLS24479_select(&Q, W, w[i]);
        ECP4_BLS24479_dbl(P);
        ECP4_BLS24479_dbl(P);
        ECP4_BLS24479_dbl(P);
        ECP4_BLS24479_dbl(P);
        ECP4_BLS24479_add(P, &Q);
    }
    ECP4_BLS24479_sub(P, &C); /* apply correction */
    ECP4_BLS24479_affine(P);
}

// calculate frobenius constants
void ECP4_BLS24479_frob_constants(FP2_BLS24479 F[3])
{
    FP_BLS24479 fx, fy;
    FP2_BLS24479 X;

    FP_BLS24479_rcopy(&fx, Fra_BLS24479);
    FP_BLS24479_rcopy(&fy, Frb_BLS24479);
    FP2_BLS24479_from_FPs(&X, &fx, &fy);

    FP2_BLS24479_sqr(&F[0], &X);     // FF=F^2=(1+i)^(p-7)/6
    FP2_BLS24479_copy(&F[2], &F[0]);
    FP2_BLS24479_mul_ip(&F[2]);      // W=(1+i)^6/6.(1+i)^(p-7)/6 = (1+i)^(p-1)/6
    FP2_BLS24479_norm(&F[2]);
    FP2_BLS24479_sqr(&F[1], &F[2]);
    FP2_BLS24479_mul(&F[2], &F[2], &F[1]); // W=(1+i)^(p-1)/2

    FP2_BLS24479_copy(&F[1], &X);

#if SEXTIC_TWIST_BLS24479 == M_TYPE
    FP2_BLS24479_mul_ip(&F[1]);      // (1+i)^12/12.(1+i)^(p-7)/12 = (1+i)^(p+5)/12
    FP2_BLS24479_inv(&F[1], &F[1],NULL);      // (1+i)^-(p+5)/12
    FP2_BLS24479_sqr(&F[0], &F[1]);      // (1+i)^-(p+5)/6
#endif

    FP2_BLS24479_mul_ip(&F[0]);      // FF=(1+i)^(p-7)/6.(1+i) = (1+i)^(p-1)/6                   // (1+i)^6/6.(1+i)^-(p+5)/6 = (1+i)^-(p-1)/6
    FP2_BLS24479_norm(&F[0]);
    FP2_BLS24479_mul(&F[1], &F[1], &F[0]); // FFF = (1+i)^(p-7)/12 . (1+i)^(p-1)/6 = (1+i)^(p-3)/4    // (1+i)^-(p+5)/12 . (1+i)^-(p-1)/6 = (1+i)^-(p+1)/4

}

/* Calculates q^n.P using Frobenius constants */
void ECP4_BLS24479_frob(ECP4_BLS24479 *P, FP2_BLS24479 F[3], int n)
{
    int i;
    FP4_BLS24479 X, Y, Z;

    FP4_BLS24479_copy(&X, &(P->x));
    FP4_BLS24479_copy(&Y, &(P->y));
    FP4_BLS24479_copy(&Z, &(P->z));

    for (i = 0; i < n; i++)
    {
        FP4_BLS24479_frob(&X, &F[2]);    // X^p
        FP4_BLS24479_pmul(&X, &X, &F[0]); // X^p.(1+i)^(p-1)/6                                    // X^p.(1+i)^-(p-1)/6

        FP4_BLS24479_frob(&Y, &F[2]);    // Y^p
        FP4_BLS24479_pmul(&Y, &Y, &F[1]);
        FP4_BLS24479_times_i(&Y);        // Y.p.(1+i)^(p-3)/4.(1+i)^(2/4) = Y^p.(1+i)^(p-1)/4    // (1+i)^-(p+1)/4 .(1+i)^2/4 = Y^p.(1+i)^-(p-1)/4

        FP4_BLS24479_frob(&Z, &F[2]);
    }

    FP4_BLS24479_copy(&(P->x), &X);
    FP4_BLS24479_copy(&(P->y), &Y);
    FP4_BLS24479_copy(&(P->z), &Z);
}

/* Side channel attack secure */
// Bos & Costello https://eprint.iacr.org/2013/458.pdf
// Faz-Hernandez & Longa & Sanchez  https://eprint.iacr.org/2013/158.pdf

void ECP4_BLS24479_mul8(ECP4_BLS24479 *P, ECP4_BLS24479 Q[8], BIG_480_29 u[8])
{
    int i, j, k, nb, pb1, pb2, bt;
    ECP4_BLS24479 T1[8], T2[8], W;
    BIG_480_29 mt, t[8];
    sign8 w1[NLEN_480_29 * BASEBITS_480_29 + 1];
    sign8 s1[NLEN_480_29 * BASEBITS_480_29 + 1];
    sign8 w2[NLEN_480_29 * BASEBITS_480_29 + 1];
    sign8 s2[NLEN_480_29 * BASEBITS_480_29 + 1];

    for (i = 0; i < 8; i++)
    {
        BIG_480_29_copy(t[i], u[i]);
    }

// Precomputed table
    ECP4_BLS24479_copy(&T1[0], &Q[0]); // Q[0]
    ECP4_BLS24479_copy(&T1[1], &T1[0]);
    ECP4_BLS24479_add(&T1[1], &Q[1]); // Q[0]+Q[1]
    ECP4_BLS24479_copy(&T1[2], &T1[0]);
    ECP4_BLS24479_add(&T1[2], &Q[2]); // Q[0]+Q[2]
    ECP4_BLS24479_copy(&T1[3], &T1[1]);
    ECP4_BLS24479_add(&T1[3], &Q[2]); // Q[0]+Q[1]+Q[2]
    ECP4_BLS24479_copy(&T1[4], &T1[0]);
    ECP4_BLS24479_add(&T1[4], &Q[3]); // Q[0]+Q[3]
    ECP4_BLS24479_copy(&T1[5], &T1[1]);
    ECP4_BLS24479_add(&T1[5], &Q[3]); // Q[0]+Q[1]+Q[3]
    ECP4_BLS24479_copy(&T1[6], &T1[2]);
    ECP4_BLS24479_add(&T1[6], &Q[3]); // Q[0]+Q[2]+Q[3]
    ECP4_BLS24479_copy(&T1[7], &T1[3]);
    ECP4_BLS24479_add(&T1[7], &Q[3]); // Q[0]+Q[1]+Q[2]+Q[3]

    ECP4_BLS24479_copy(&T2[0], &Q[4]); // Q[0]
    ECP4_BLS24479_copy(&T2[1], &T2[0]);
    ECP4_BLS24479_add(&T2[1], &Q[5]); // Q[0]+Q[1]
    ECP4_BLS24479_copy(&T2[2], &T2[0]);
    ECP4_BLS24479_add(&T2[2], &Q[6]); // Q[0]+Q[2]
    ECP4_BLS24479_copy(&T2[3], &T2[1]);
    ECP4_BLS24479_add(&T2[3], &Q[6]); // Q[0]+Q[1]+Q[2]
    ECP4_BLS24479_copy(&T2[4], &T2[0]);
    ECP4_BLS24479_add(&T2[4], &Q[7]); // Q[0]+Q[3]
    ECP4_BLS24479_copy(&T2[5], &T2[1]);
    ECP4_BLS24479_add(&T2[5], &Q[7]); // Q[0]+Q[1]+Q[3]
    ECP4_BLS24479_copy(&T2[6], &T2[2]);
    ECP4_BLS24479_add(&T2[6], &Q[7]); // Q[0]+Q[2]+Q[3]
    ECP4_BLS24479_copy(&T2[7], &T2[3]);
    ECP4_BLS24479_add(&T2[7], &Q[7]); // Q[0]+Q[1]+Q[2]+Q[3]

// Make them odd
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
    ECP4_BLS24479_select(P, T1, 2 * w1[nb - 1] + 1);
    ECP4_BLS24479_select(&W, T2, 2 * w2[nb - 1] + 1);
    ECP4_BLS24479_add(P, &W);
    for (i = nb - 2; i >= 0; i--)
    {
        ECP4_BLS24479_dbl(P);
        ECP4_BLS24479_select(&W, T1, 2 * w1[i] + s1[i]);
        ECP4_BLS24479_add(P, &W);
        ECP4_BLS24479_select(&W, T2, 2 * w2[i] + s2[i]);
        ECP4_BLS24479_add(P, &W);
    }

// apply corrections
    ECP4_BLS24479_copy(&W, P);
    ECP4_BLS24479_sub(&W, &Q[0]);
    ECP4_BLS24479_cmove(P, &W, pb1);
    ECP4_BLS24479_copy(&W, P);
    ECP4_BLS24479_sub(&W, &Q[4]);
    ECP4_BLS24479_cmove(P, &W, pb2);

    ECP4_BLS24479_affine(P);
}

/* Hunt and Peck a BIG to a curve point */
/*
void ECP4_BLS24479_hap2point(ECP4_BLS24479 *Q,BIG_480_29 h)
{
    BIG_480_29 one,hv;
    FP2_BLS24479 X2;
    FP4_BLS24479 X4;
    BIG_480_29_one(one);
    BIG_480_29_copy(hv,h);

    for (;;)
    {
        FP2_BLS24479_from_BIGs(&X2,one,hv); 
		FP4_BLS24479_from_FP2(&X4,&X2);
        if (ECP4_BLS24479_setx(Q,&X4,0)) break;
        BIG_480_29_inc(hv,1);
        BIG_480_29_norm(hv);
    }
}
*/
/* Constant time Map to Point in G2 */
void ECP4_BLS24479_map2point(ECP4_BLS24479 *Q,FP4_BLS24479 *H)
{
    int sgn,ne;
    FP4_BLS24479 X1,X2,X3,W,Y,T,A,NY;
    FP_BLS24479 Z,s;

    FP4_BLS24479_one(&NY);
    FP4_BLS24479_copy(&T,H);
    sgn=FP4_BLS24479_sign(&T);

    FP_BLS24479_from_int(&Z,RIADZG2A_BLS24479);
    FP4_BLS24479_from_FP(&A,&Z);
    ECP4_BLS24479_rhs(&A,&A);  // A=g(Z)

    FP4_BLS24479_sqrt(&W,&A,NULL);
    FP_BLS24479_rcopy(&s,SQRTm3_BLS24479);

    FP_BLS24479_mul(&Z,&Z,&s);

    FP4_BLS24479_sqr(&T,&T);   
    FP4_BLS24479_mul(&Y,&A,&T);   // tv1=u^2*g(Z)
    FP4_BLS24479_add(&T,&NY,&Y); FP4_BLS24479_norm(&T); // tv2=1+tv1
    FP4_BLS24479_sub(&Y,&NY,&Y); FP4_BLS24479_norm(&Y); // tv1=1-tv1 
    FP4_BLS24479_mul(&NY,&T,&Y);

    FP4_BLS24479_qmul(&NY,&NY,&Z);

    FP4_BLS24479_inv(&NY,&NY,NULL);     // tv3=inv0(tv1*tv2*Z*sqrt(-3))
    FP4_BLS24479_qmul(&W,&W,&Z); // tv4=Z*sqrt(-3).sqrt(g(Z))
    if (FP4_BLS24479_sign(&W)==1)
    {
        FP4_BLS24479_neg(&W,&W);
        FP4_BLS24479_norm(&W);
    }
    FP4_BLS24479_qmul(&W,&W,&Z); // tv4=Z*sqrt(-3g(Z))
    FP4_BLS24479_mul(&W,&W,H);
    FP4_BLS24479_mul(&W,&W,&Y);
    FP4_BLS24479_mul(&W,&W,&NY);     // tv5=u*tv1*tv3*tv4*Z*sqrt(-3)

    FP_BLS24479_from_int(&s,RIADZG2A_BLS24479);
    FP4_BLS24479_from_FP(&X1,&s);

    FP4_BLS24479_copy(&X3,&X1);
    FP4_BLS24479_neg(&X1,&X1); FP4_BLS24479_norm(&X1); FP4_BLS24479_div2(&X1,&X1); // -Z/2
    FP4_BLS24479_copy(&X2,&X1);
    FP4_BLS24479_sub(&X1,&X1,&W); FP4_BLS24479_norm(&X1);
    FP4_BLS24479_add(&X2,&X2,&W); FP4_BLS24479_norm(&X2);
    FP4_BLS24479_add(&A,&A,&A);
    FP4_BLS24479_add(&A,&A,&A); 
    FP4_BLS24479_norm(&A);      // 4*g(Z)
    FP4_BLS24479_sqr(&T,&T);
    FP4_BLS24479_mul(&T,&T,&NY);
    FP4_BLS24479_sqr(&T,&T);    // (tv2^2*tv3)^2
    FP4_BLS24479_mul(&A,&A,&T); // 4*g(Z)*(tv2^2*tv3)^2

    FP4_BLS24479_add(&X3,&X3,&A); FP4_BLS24479_norm(&X3);

    ECP4_BLS24479_rhs(&W,&X2);
    FP4_BLS24479_cmove(&X3,&X2,FP4_BLS24479_qr(&W,NULL));
    ECP4_BLS24479_rhs(&W,&X1);
    FP4_BLS24479_cmove(&X3,&X1,FP4_BLS24479_qr(&W,NULL));
    ECP4_BLS24479_rhs(&W,&X3);
    FP4_BLS24479_sqrt(&Y,&W,NULL);
    
    ne=FP4_BLS24479_sign(&Y)^sgn;
    FP4_BLS24479_neg(&W,&Y); FP4_BLS24479_norm(&W);
    FP4_BLS24479_cmove(&Y,&W,ne);
 
    ECP4_BLS24479_set(Q,&X3,&Y);
}

/* Map octet to point */
/*
void ECP4_BLS24479_mapit(ECP4_BLS24479 *Q, octet *W)
{
    BIG_480_29 q, x;
    DBIG_480_29 dx;
    BIG_480_29_rcopy(q, Modulus_BLS24479);

    BIG_480_29_dfromBytesLen(dx,W->val,W->len);
    BIG_480_29_dmod(x,dx,q);

    ECP4_BLS24479_hap2point(Q,x);   
    ECP4_BLS24479_cfp(Q);
}
*/
/* cofactor product */
void ECP4_BLS24479_cfp(ECP4_BLS24479 *Q)
{
    FP2_BLS24479 X[3];
    ECP4_BLS24479 xQ, x2Q, x3Q, x4Q;
    BIG_480_29 x;

    ECP4_BLS24479_frob_constants(X);

    BIG_480_29_rcopy(x, CURVE_Bnx_BLS24479);

    // Efficient hash maps to G2 on BLS24 curves - Budroni, Pintore
    // Q -> x4Q -x3Q -Q + F(x3Q-x2Q) + F(F(x2Q-xQ)) + F(F(F(xQ-Q))) +F(F(F(F(2Q))))

    ECP4_BLS24479_copy(&xQ, Q);
    ECP4_BLS24479_mul(&xQ, x);
    ECP4_BLS24479_copy(&x2Q, &xQ);
    ECP4_BLS24479_mul(&x2Q, x);
    ECP4_BLS24479_copy(&x3Q, &x2Q);
    ECP4_BLS24479_mul(&x3Q, x);
    ECP4_BLS24479_copy(&x4Q, &x3Q);
    ECP4_BLS24479_mul(&x4Q, x);

#if SIGN_OF_X_BLS24479==NEGATIVEX
    ECP4_BLS24479_neg(&xQ);
    ECP4_BLS24479_neg(&x3Q);
#endif

    ECP4_BLS24479_sub(&x4Q, &x3Q);
    ECP4_BLS24479_sub(&x4Q, Q);

    ECP4_BLS24479_sub(&x3Q, &x2Q);
    ECP4_BLS24479_frob(&x3Q, X, 1);

    ECP4_BLS24479_sub(&x2Q, &xQ);
    ECP4_BLS24479_frob(&x2Q, X, 2);

    ECP4_BLS24479_sub(&xQ, Q);
    ECP4_BLS24479_frob(&xQ, X, 3);

    ECP4_BLS24479_dbl(Q);
    ECP4_BLS24479_frob(Q, X, 4);

    ECP4_BLS24479_add(Q, &x4Q);
    ECP4_BLS24479_add(Q, &x3Q);
    ECP4_BLS24479_add(Q, &x2Q);
    ECP4_BLS24479_add(Q, &xQ);

    ECP4_BLS24479_affine(Q);

}

// ECP$ Get Group Generator

int ECP4_BLS24479_generator(ECP4_BLS24479 *G)
{
    BIG_480_29 a, b;
    FP2_BLS24479 Aa, Bb;
    FP4_BLS24479 X, Y;

    BIG_480_29_rcopy(a, CURVE_Pxaa_BLS24479);
    BIG_480_29_rcopy(b, CURVE_Pxab_BLS24479);
    FP2_BLS24479_from_BIGs(&Aa, a, b);

    BIG_480_29_rcopy(a, CURVE_Pxba_BLS24479);
    BIG_480_29_rcopy(b, CURVE_Pxbb_BLS24479);
    FP2_BLS24479_from_BIGs(&Bb, a, b);

    FP4_BLS24479_from_FP2s(&X, &Aa, &Bb);

    BIG_480_29_rcopy(a, CURVE_Pyaa_BLS24479);
    BIG_480_29_rcopy(b, CURVE_Pyab_BLS24479);
    FP2_BLS24479_from_BIGs(&Aa, a, b);

    BIG_480_29_rcopy(a, CURVE_Pyba_BLS24479);
    BIG_480_29_rcopy(b, CURVE_Pybb_BLS24479);
    FP2_BLS24479_from_BIGs(&Bb, a, b);

    FP4_BLS24479_from_FP2s(&Y, &Aa, &Bb);

    return ECP4_BLS24479_set(G, &X, &Y);
}


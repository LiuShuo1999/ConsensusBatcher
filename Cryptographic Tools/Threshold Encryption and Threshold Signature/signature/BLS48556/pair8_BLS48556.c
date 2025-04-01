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

/* CORE BLS Curve pairing functions */

//#define HAS_MAIN

#include "pair8_BLS48556.h"


// Point doubling for pairings
static void PAIR_BLS48556_double(ECP8_BLS48556 *A, FP8_BLS48556 *AA, FP8_BLS48556 *BB, FP8_BLS48556 *CC)
{
    FP8_BLS48556 YY;
    FP8_BLS48556_copy(CC, &(A->x));  //FP8 XX=new FP8(A.getx());  //X
    FP8_BLS48556_copy(&YY, &(A->y)); //FP8 YY=new FP8(A.gety());  //Y
    FP8_BLS48556_copy(BB, &(A->z));  //FP8 ZZ=new FP8(A.getz());  //Z

    FP8_BLS48556_copy(AA, &YY);      //FP8 YZ=new FP8(YY);        //Y
    FP8_BLS48556_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP8_BLS48556_sqr(CC, CC);        //XX.sqr();                //X^2
    FP8_BLS48556_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP8_BLS48556_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP8_BLS48556_add(AA, AA, AA);
    FP8_BLS48556_neg(AA, AA);
    FP8_BLS48556_norm(AA);           // -2YZ
    FP8_BLS48556_times_i(AA);        // -2YZi

    FP8_BLS48556_imul(BB, BB, 3 * CURVE_B_I_BLS48556); //3Bz^2
    FP8_BLS48556_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_BLS48556==D_TYPE
    FP8_BLS48556_times_i(&YY);                   // Y^2.i
    FP8_BLS48556_times_i(CC);                    // 3X^2.i
#endif

#if SEXTIC_TWIST_BLS48556==M_TYPE
    FP8_BLS48556_times_i(BB);                    // 3Bz^2.i
#endif

    FP8_BLS48556_sub(BB, BB, &YY);
    FP8_BLS48556_norm(BB);

    ECP8_BLS48556_dbl(A);            //A.dbl();
}

// Point addition for pairings
static void PAIR_BLS48556_add(ECP8_BLS48556 *A, ECP8_BLS48556 *B, FP8_BLS48556 *AA, FP8_BLS48556 *BB, FP8_BLS48556 *CC)
{
    FP8_BLS48556 T1;
    FP8_BLS48556_copy(AA, &(A->x));      //FP8 X1=new FP8(A.getx());    // X1
    FP8_BLS48556_copy(CC, &(A->y));      //FP8 Y1=new FP8(A.gety());    // Y1
    FP8_BLS48556_copy(&T1, &(A->z));     //FP8 T1=new FP8(A.getz());    // Z1

    FP8_BLS48556_copy(BB, &T1);          //FP8 T2=new FP8(A.getz());    // Z1

    FP8_BLS48556_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP8_BLS48556_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP8_BLS48556_sub(AA, AA, BB);        //X1.sub(T2);
    FP8_BLS48556_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP8_BLS48556_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP8_BLS48556_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP8_BLS48556_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_BLS48556==M_TYPE
    FP8_BLS48556_times_i(AA);
    FP8_BLS48556_norm(AA);
#endif

    FP8_BLS48556_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP8_BLS48556_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP8_BLS48556_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP8_BLS48556_sub(BB, BB, &T1);       //T2.sub(T1);
    FP8_BLS48556_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP8_BLS48556_neg(CC, CC);            //Y1.neg();
    FP8_BLS48556_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP8_BLS48556_add(A, B);             //A.add(B);
}


/* Line function */
static void PAIR_BLS48556_line(FP48_BLS48556 *v, ECP8_BLS48556 *A, ECP8_BLS48556 *B, FP_BLS48556 *Qx, FP_BLS48556 *Qy)
{
    FP8_BLS48556 AA, BB, CC;
    FP16_BLS48556 a, b, c;

    if (A == B)
        PAIR_BLS48556_double(A, &AA, &BB, &CC);
    else
        PAIR_BLS48556_add(A, B, &AA, &BB, &CC);

    FP8_BLS48556_tmul(&CC, &CC, Qx);
    FP8_BLS48556_tmul(&AA, &AA, Qy);

    FP16_BLS48556_from_FP8s(&a, &AA, &BB);
#if SEXTIC_TWIST_BLS48556==D_TYPE
    FP16_BLS48556_from_FP8(&b, &CC);
    FP16_BLS48556_zero(&c);
#endif
#if SEXTIC_TWIST_BLS48556==M_TYPE
    FP16_BLS48556_zero(&b);
    FP16_BLS48556_from_FP8H(&c, &CC);
#endif

    FP48_BLS48556_from_FP16s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int PAIR_BLS48556_nbits(BIG_560_29 n3, BIG_560_29 n)
{
    BIG_560_29 x;
    BIG_560_29_rcopy(x, CURVE_Bnx_BLS48556);

    BIG_560_29_copy(n, x);
    BIG_560_29_norm(n);
    BIG_560_29_pmul(n3, n, 3);
    BIG_560_29_norm(n3);

    return BIG_560_29_nbits(n3);
}

/*
    For multi-pairing, product of n pairings
    1. Declare FP24 array of length number of bits in Ate parameter
    2. Initialise this array by calling PAIR_initmp()
    3. Accumulate each pairing by calling PAIR_another() n times
    4. Call PAIR_miller()
    5. Call final exponentiation PAIR_fexp()
*/

/* prepare for multi-pairing */
void PAIR_BLS48556_initmp(FP48_BLS48556 r[])
{
    int i;
    for (i = ATE_BITS_BLS48556 - 1; i >= 0; i--)
        FP48_BLS48556_one(&r[i]);
    return;
}

// Store precomputed line details in an FP4
static void PAIR_BLS48556_pack(FP16_BLS48556 *T, FP8_BLS48556* AA, FP8_BLS48556* BB, FP8_BLS48556 *CC)
{
    FP8_BLS48556 I, A, B;
    FP8_BLS48556_inv(&I, CC, NULL);
    FP8_BLS48556_mul(&A, AA, &I);
    FP8_BLS48556_mul(&B, BB, &I);
    FP16_BLS48556_from_FP8s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void PAIR_BLS48556_unpack(FP48_BLS48556 *v, FP16_BLS48556* T, FP_BLS48556 *Qx, FP_BLS48556 *Qy)
{
    FP16_BLS48556 a, b, c;
    FP8_BLS48556 t;
    FP16_BLS48556_copy(&a, T);
    FP8_BLS48556_tmul(&a.a, &a.a, Qy);
    FP8_BLS48556_from_FP(&t, Qx);

#if SEXTIC_TWIST_BLS48556==D_TYPE
    FP16_BLS48556_from_FP8(&b, &t);
    FP16_BLS48556_zero(&c);
#endif
#if SEXTIC_TWIST_BLS48556==M_TYPE
    FP16_BLS48556_zero(&b);
    FP16_BLS48556_from_FP8H(&c, &t);
#endif

    FP48_BLS48556_from_FP16s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}

/* basic Miller loop */
void PAIR_BLS48556_miller(FP48_BLS48556 *res, FP48_BLS48556 r[])
{
    int i;
    FP48_BLS48556_one(res);
    for (i = ATE_BITS_BLS48556 - 1; i >= 1; i--)
    {
        FP48_BLS48556_sqr(res, res);
        FP48_BLS48556_ssmul(res, &r[i]);
        FP48_BLS48556_zero(&r[i]);
    }

#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(res, res);
#endif
    FP48_BLS48556_ssmul(res, &r[0]);
    FP48_BLS48556_zero(&r[0]);
    return;
}

// Precompute table of line functions for fixed G2 value
void PAIR_BLS48556_precomp(FP16_BLS48556 T[], ECP8_BLS48556* GV)
{
    int i, j, nb, bt;
    BIG_560_29 n, n3;
    FP8_BLS48556 AA, BB, CC;
    ECP8_BLS48556 A, G, NG;

    ECP8_BLS48556_copy(&A, GV);
    ECP8_BLS48556_copy(&G, GV);
    ECP8_BLS48556_copy(&NG, GV);
    ECP8_BLS48556_neg(&NG);

    nb = PAIR_BLS48556_nbits(n3, n);
    j = 0;

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS48556_double(&A, &AA, &BB, &CC);
        PAIR_BLS48556_pack(&T[j++], &AA, &BB, &CC);

        bt = BIG_560_29_bit(n3, i) - BIG_560_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48556_add(&A, &G, &AA, &BB, &CC);
            PAIR_BLS48556_pack(&T[j++], &AA, &BB, &CC);
        }
        if (bt == -1)
        {
            PAIR_BLS48556_add(&A, &NG, &AA, &BB, &CC);
            PAIR_BLS48556_pack(&T[j++], &AA, &BB, &CC);
        }
    }
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void PAIR_BLS48556_another_pc(FP48_BLS48556 r[], FP16_BLS48556 T[], ECP_BLS48556 *QV)
{
    int i, j, nb, bt;
    BIG_560_29 x, n, n3;
    FP48_BLS48556 lv, lv2;
    ECP_BLS48556 Q;
    FP_BLS48556 Qx, Qy;

    if (ECP_BLS48556_isinf(QV)) return;

    nb = PAIR_BLS48556_nbits(n3, n);

    ECP_BLS48556_copy(&Q, QV);
    ECP_BLS48556_affine(&Q);

    FP_BLS48556_copy(&Qx, &(Q.x));
    FP_BLS48556_copy(&Qy, &(Q.y));

    j = 0;
    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS48556_unpack(&lv, &T[j++], &Qx, &Qy);

        bt = BIG_560_29_bit(n3, i) - BIG_560_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48556_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP48_BLS48556_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS48556_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP48_BLS48556_smul(&lv, &lv2);
        }
        FP48_BLS48556_ssmul(&r[i], &lv);
    }
}



/* Accumulate another set of line functions for n-pairing */
void PAIR_BLS48556_another(FP48_BLS48556 r[], ECP8_BLS48556* PV, ECP_BLS48556* QV)
{
    int i, j, nb, bt;
    BIG_560_29 x, n, n3;
    FP48_BLS48556 lv, lv2;
    ECP8_BLS48556 A, NP, P;
    ECP_BLS48556 Q;
    FP_BLS48556 Qx, Qy;

    if (ECP_BLS48556_isinf(QV)) return;

    nb = PAIR_BLS48556_nbits(n3, n);

    ECP8_BLS48556_copy(&P, PV);
    ECP_BLS48556_copy(&Q, QV);

    ECP8_BLS48556_affine(&P);
    ECP_BLS48556_affine(&Q);

    FP_BLS48556_copy(&Qx, &(Q.x));
    FP_BLS48556_copy(&Qy, &(Q.y));

    ECP8_BLS48556_copy(&A, &P);
    ECP8_BLS48556_copy(&NP, &P); ECP8_BLS48556_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS48556_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_560_29_bit(n3, i) - BIG_560_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48556_line(&lv2, &A, &P, &Qx, &Qy);
            FP48_BLS48556_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS48556_line(&lv2, &A, &NP, &Qx, &Qy);
            FP48_BLS48556_smul(&lv, &lv2);
        }
        FP48_BLS48556_ssmul(&r[i], &lv);
    }
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_BLS48556_ate(FP48_BLS48556 *r, ECP8_BLS48556 *P1, ECP_BLS48556 *Q1)
{
    BIG_560_29 x, n, n3;
    FP_BLS48556 Qx, Qy;
    int i, nb, bt;
    ECP8_BLS48556 A, NP, P;
    ECP_BLS48556 Q;
    FP48_BLS48556 lv, lv2;

    FP48_BLS48556_one(r);

    if (ECP_BLS48556_isinf(Q1)) return;

    nb = PAIR_BLS48556_nbits(n3, n);

    ECP8_BLS48556_copy(&P, P1);
    ECP_BLS48556_copy(&Q, Q1);

    ECP8_BLS48556_affine(&P);
    ECP_BLS48556_affine(&Q);


    FP_BLS48556_copy(&Qx, &(Q.x));
    FP_BLS48556_copy(&Qy, &(Q.y));

    ECP8_BLS48556_copy(&A, &P);
    ECP8_BLS48556_copy(&NP, &P); ECP8_BLS48556_neg(&NP);



    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP48_BLS48556_sqr(r, r);
        PAIR_BLS48556_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_560_29_bit(n3, i) - BIG_560_29_bit(n, i); // BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48556_line(&lv2, &A, &P, &Qx, &Qy);
            FP48_BLS48556_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS48556_line(&lv2, &A, &NP, &Qx, &Qy);
            FP48_BLS48556_smul(&lv, &lv2);
        }
        FP48_BLS48556_ssmul(r, &lv);
    }

#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(r, r);
#endif

}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_BLS48556_double_ate(FP48_BLS48556 *r, ECP8_BLS48556 *P1, ECP_BLS48556 *Q1, ECP8_BLS48556 *R1, ECP_BLS48556 *S1)
{
    BIG_560_29 x, n, n3;
    FP_BLS48556 Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP8_BLS48556 A, B, NP, NR, P, R;
    ECP_BLS48556 Q, S;
    FP48_BLS48556 lv, lv2;

    if (ECP_BLS48556_isinf(Q1))
    {
        PAIR_BLS48556_ate(r, R1, S1);
        return;
    }
    if (ECP_BLS48556_isinf(S1))
    {
        PAIR_BLS48556_ate(r, P1, Q1);
        return;
    }
    nb = PAIR_BLS48556_nbits(n3, n);

    ECP8_BLS48556_copy(&P, P1);
    ECP_BLS48556_copy(&Q, Q1);

    ECP8_BLS48556_affine(&P);
    ECP_BLS48556_affine(&Q);

    ECP8_BLS48556_copy(&R, R1);
    ECP_BLS48556_copy(&S, S1);

    ECP8_BLS48556_affine(&R);
    ECP_BLS48556_affine(&S);

    FP_BLS48556_copy(&Qx, &(Q.x));
    FP_BLS48556_copy(&Qy, &(Q.y));

    FP_BLS48556_copy(&Sx, &(S.x));
    FP_BLS48556_copy(&Sy, &(S.y));

    ECP8_BLS48556_copy(&A, &P);
    ECP8_BLS48556_copy(&B, &R);
    ECP8_BLS48556_copy(&NP, &P); ECP8_BLS48556_neg(&NP);
    ECP8_BLS48556_copy(&NR, &R); ECP8_BLS48556_neg(&NR);

    FP48_BLS48556_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP48_BLS48556_sqr(r, r);
        PAIR_BLS48556_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_BLS48556_line(&lv2, &B, &B, &Sx, &Sy);
        FP48_BLS48556_smul(&lv, &lv2);
        FP48_BLS48556_ssmul(r, &lv);

        bt = BIG_560_29_bit(n3, i) - BIG_560_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48556_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_BLS48556_line(&lv2, &B, &R, &Sx, &Sy);
            FP48_BLS48556_smul(&lv, &lv2);
            FP48_BLS48556_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_BLS48556_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_BLS48556_line(&lv2, &B, &NR, &Sx, &Sy);
            FP48_BLS48556_smul(&lv, &lv2);
            FP48_BLS48556_ssmul(r, &lv);
        }
    }

#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(r, r);
#endif

}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */

void PAIR_BLS48556_fexp(FP48_BLS48556 *r)
{
    FP2_BLS48556 X;
    BIG_560_29 x;
    FP_BLS48556 a, b;
    FP48_BLS48556 t0, t1, t2;//, t3;

    BIG_560_29_rcopy(x, CURVE_Bnx_BLS48556);
    FP_BLS48556_rcopy(&a, Fra_BLS48556);
    FP_BLS48556_rcopy(&b, Frb_BLS48556);
    FP2_BLS48556_from_FPs(&X, &a, &b);

    /* Easy part of final exp - r^(p^24-1)(p^8+1)*/

    FP48_BLS48556_inv(&t0, r);
    FP48_BLS48556_conj(r, r);

    FP48_BLS48556_mul(r, &t0);
    FP48_BLS48556_copy(&t0, r);

    FP48_BLS48556_frob(r, &X, 8);

    FP48_BLS48556_mul(r, &t0);

// See https://eprint.iacr.org/2020/875.pdf
    FP48_BLS48556_usqr(&t2,r);
    FP48_BLS48556_mul(&t2,r);     // t2=r^3

    FP48_BLS48556_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_conj(&t0,r);    // t0=r^-1
    FP48_BLS48556_copy(r,&t1);
    FP48_BLS48556_mul(r,&t0);    // r=r^(x-1)

    FP48_BLS48556_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_conj(&t0,r);    // t0=r^-1
    FP48_BLS48556_copy(r,&t1);
    FP48_BLS48556_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP48_BLS48556_pow(&t1,r,x);  // t1=r^x
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t0,r);   
    FP48_BLS48556_frob(&t0,&X,1); // t0=r^p
    FP48_BLS48556_copy(r,&t1);
    FP48_BLS48556_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2)
    FP48_BLS48556_pow(&t1,r,x);  
    FP48_BLS48556_pow(&t1,&t1,x); // t1=r^x^2
    FP48_BLS48556_copy(&t0,r);    
    FP48_BLS48556_frob(&t0,&X,2);   // t0=r^p^2
    FP48_BLS48556_mul(&t1,&t0);   // t1=r^x^2.r^p^2
    FP48_BLS48556_copy(r,&t1);

// ^(x^4+p^4)
    FP48_BLS48556_pow(&t1,r,x);  
    FP48_BLS48556_pow(&t1,&t1,x);
    FP48_BLS48556_pow(&t1,&t1,x); 
    FP48_BLS48556_pow(&t1,&t1,x); // t1=r^x^4
    FP48_BLS48556_copy(&t0,r);    
    FP48_BLS48556_frob(&t0,&X,4);   // t0=r^p^4
    FP48_BLS48556_mul(&t1,&t0);   // t1=r^x^4.r^p^4
    FP48_BLS48556_copy(r,&t1);

// ^(x^8+p^8-1)
    FP48_BLS48556_pow(&t1,r,x);  
    FP48_BLS48556_pow(&t1,&t1,x); 
    FP48_BLS48556_pow(&t1,&t1,x); 
    FP48_BLS48556_pow(&t1,&t1,x); 
    FP48_BLS48556_pow(&t1,&t1,x);
    FP48_BLS48556_pow(&t1,&t1,x);
    FP48_BLS48556_pow(&t1,&t1,x); 
    FP48_BLS48556_pow(&t1,&t1,x); // t1=r^x^8
    FP48_BLS48556_copy(&t0,r);    
    FP48_BLS48556_frob(&t0,&X,8); // t0=r^p^8
    FP48_BLS48556_mul(&t1,&t0);   // t1=r^x^8.r^p^8
    FP48_BLS48556_conj(&t0,r);    // t0=r^-1
    FP48_BLS48556_copy(r,&t1);    
    FP48_BLS48556_mul(r,&t0);     // r=r^x^4.r^p^4.r^-1

    FP48_BLS48556_mul(r,&t2);    
    FP48_BLS48556_reduce(r);

/*
// f^e0.f^e1^p.f^e2^p^2.. .. f^e14^p^14.f^e15^p^15

    FP48_BLS48556_usqr(&t7, r);          // t7=f^2

    if (BIG_560_29_parity(x) == 1)
    {
        FP48_BLS48556_pow(&t2, r, x);    // f^u
        FP48_BLS48556_usqr(&t1, &t2);    // f^(2u)
        FP48_BLS48556_pow(&t2, &t2, x);  // f^u^2
    } else {
        FP48_BLS48556_pow(&t1, &t7, x);      // t1=f^2u
        BIG_560_29_fshr(x, 1);
        FP48_BLS48556_pow(&t2, &t1, x);      // t2=f^2u^(u/2) =  f^u^2
        BIG_560_29_fshl(x, 1);             // x must be even
    }

#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif

    FP48_BLS48556_conj(&t3, &t1);    // t3=f^-2u
    FP48_BLS48556_mul(&t2, &t3);     // t2=f^u^2.f^-2u
    FP48_BLS48556_mul(&t2, r);       // t2=f^u^2.f^-2u.f = f^(u^2-2u+1) = f^e15

    FP48_BLS48556_mul(r, &t7);       // f^3

    FP48_BLS48556_pow(&t1, &t2, x);  // f^e15^u = f^(u.e15) = f^(u^3-2u^2+u) = f^(e14)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 14); // f^(u^3-2u^2+u)^p^14
    FP48_BLS48556_mul(r, &t3);       // f^3.f^(u^3-2u^2+u)^p^14

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e14) = f^(u^4-2u^3+u^2) =  f^(e13)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 13); // f^(e13)^p^13
    FP48_BLS48556_mul(r, &t3);       // f^3.f^(u^3-2u^2+u)^p^14.f^(u^4-2u^3+u^2)^p^13

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e13)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 12); // f^(e12)^p^12
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e12)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 11); // f^(e11)^p^11
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e11)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 10); // f^(e10)^p^10
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e10)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 9);  // f^(e9)^p^9
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e9)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 8);  // f^(e8)^p^8
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e8)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_conj(&t3, &t2);
    FP48_BLS48556_mul(&t1, &t3); // f^(u.e8).f^-e15
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 7);  // f^(e7)^p^7
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e7)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 6);  // f^(e6)^p^6
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e6)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 5);  // f^(e5)^p^5
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e5)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 4);  // f^(e4)^p^4
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e4)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 3);  // f^(e3)^p^3
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e3)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 2);  // f^(e2)^p^2
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e2)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_copy(&t3, &t1);
    FP48_BLS48556_frob(&t3, &X, 1);  // f^(e1)^p^1
    FP48_BLS48556_mul(r, &t3);

    FP48_BLS48556_pow(&t1, &t1, x);  // f^(u.e1)
#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t1, &t1);
#endif
    FP48_BLS48556_mul(r, &t1);       // r.f^e0

    FP48_BLS48556_frob(&t2, &X, 15); // f^(e15.p^15)
    FP48_BLS48556_mul(r, &t2);

    FP48_BLS48556_reduce(r);
*/
}

#ifdef USE_GLV_BLS48556
/* GLV method */
static void glv(BIG_560_29 u[2], BIG_560_29 ee)
{

    int bd;
    BIG_560_29 q,x,x2;
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
// -(x^8).P = (Beta.x,y)

    BIG_560_29_rcopy(x, CURVE_Bnx_BLS48556);

    BIG_560_29_smul(x2, x, x);
    BIG_560_29_smul(x, x2, x2);
    BIG_560_29_smul(x2, x, x);
    bd=BIG_560_29_nbits(q)-BIG_560_29_nbits(x2); // fixed x^8

    BIG_560_29_copy(u[0], ee);
    BIG_560_29_ctmod(u[0], x2, bd);
    BIG_560_29_copy(u[1], ee);
    BIG_560_29_ctsdiv(u[1], x2, bd);

    BIG_560_29_sub(u[1], q, u[1]);

    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void gs(BIG_560_29 u[16], BIG_560_29 ee)
{
    int i,bd;
    BIG_560_29 q,x,w;
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);

    BIG_560_29_rcopy(x, CURVE_Bnx_BLS48556);
    BIG_560_29_copy(w, ee);
    bd=BIG_560_29_nbits(q)-BIG_560_29_nbits(x); // fixed

    for (i = 0; i < 15; i++)
    {
        BIG_560_29_copy(u[i], w);
        BIG_560_29_ctmod(u[i], x, bd);
        BIG_560_29_ctsdiv(w, x, bd);
    }
    BIG_560_29_copy(u[15], w);

    /*  */
#if SIGN_OF_X_BLS48556==NEGATIVEX
    BIG_560_29_modneg(u[1], u[1], q);
    BIG_560_29_modneg(u[3], u[3], q);
    BIG_560_29_modneg(u[5], u[5], q);
    BIG_560_29_modneg(u[7], u[7], q);
    BIG_560_29_modneg(u[9], u[9], q);
    BIG_560_29_modneg(u[11], u[11], q);
    BIG_560_29_modneg(u[13], u[13], q);
    BIG_560_29_modneg(u[15], u[15], q);
#endif

    return;
}

/* Multiply P by e in group G1 */
void PAIR_BLS48556_G1mul(ECP_BLS48556 *P, BIG_560_29 e)
{
    BIG_560_29 ee,q;
#ifdef USE_GLV_BLS48556   /* Note this method is patented */
    int np, nn;
    ECP_BLS48556 Q;
    FP_BLS48556 cru;
    BIG_560_29 t;
    BIG_560_29 u[2];

    BIG_560_29_copy(ee,e);
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
    BIG_560_29_mod(ee,q);

    glv(u, ee);

    ECP_BLS48556_copy(&Q, P); ECP_BLS48556_affine(&Q);
    FP_BLS48556_rcopy(&cru, CRu_BLS48556);
    FP_BLS48556_mul(&(Q.x), &(Q.x), &cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np = BIG_560_29_nbits(u[0]);
    BIG_560_29_modneg(t, u[0], q);
    nn = BIG_560_29_nbits(t);
    if (nn < np)
    {
        BIG_560_29_copy(u[0], t);
        ECP_BLS48556_neg(P);
    }

    np = BIG_560_29_nbits(u[1]);
    BIG_560_29_modneg(t, u[1], q);
    nn = BIG_560_29_nbits(t);
    if (nn < np)
    {
        BIG_560_29_copy(u[1], t);
        ECP_BLS48556_neg(&Q);
    }
    BIG_560_29_norm(u[0]);
    BIG_560_29_norm(u[1]);
    ECP_BLS48556_mul2(P, &Q, u[0], u[1]);

#else
    BIG_560_29_copy(ee,e);
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
    BIG_560_29_mod(ee,q);
    ECP_BLS48556_clmul(P, ee, q);
#endif
}

/* Multiply P by e in group G2 */
void PAIR_BLS48556_G2mul(ECP8_BLS48556 *P, BIG_560_29 e)
{
    BIG_560_29 ee,q;
#ifdef USE_GS_G2_BLS48556   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP8_BLS48556 Q[16];
    FP2_BLS48556 X[3];
    BIG_560_29 x, u[16];

    BIG_560_29_copy(ee,e);
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
    BIG_560_29_mod(ee,q);

    ECP8_BLS48556_frob_constants(X);

    gs(u, ee);

    ECP8_BLS48556_copy(&Q[0], P);
    for (i = 1; i < 16; i++)
    {
        ECP8_BLS48556_copy(&Q[i], &Q[i - 1]);
        ECP8_BLS48556_frob(&Q[i], X, 1);
    }

    for (i = 0; i < 16; i++)
    {
        np = BIG_560_29_nbits(u[i]);
        BIG_560_29_modneg(x, u[i], q);
        nn = BIG_560_29_nbits(x);
        if (nn < np)
        {
            BIG_560_29_copy(u[i], x);
            ECP8_BLS48556_neg(&Q[i]);
        }
        BIG_560_29_norm(u[i]);
    }

    ECP8_BLS48556_mul16(P, Q, u);

#else
    BIG_560_29_copy(ee,e);
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
    BIG_560_29_mod(ee,q);
    ECP8_BLS48556_mul(P, ee);
#endif
}

/* f=f^e */
void PAIR_BLS48556_GTpow(FP48_BLS48556 *f, BIG_560_29 e)
{
    BIG_560_29 ee,q;
#ifdef USE_GS_GT_BLS48556   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see FP16.cpp */
    int i, np, nn;
    FP48_BLS48556 g[16];
    FP2_BLS48556 X;
    BIG_560_29 t;
    FP_BLS48556 fx, fy;
    BIG_560_29 u[16];

    FP_BLS48556_rcopy(&fx, Fra_BLS48556);
    FP_BLS48556_rcopy(&fy, Frb_BLS48556);
    FP2_BLS48556_from_FPs(&X, &fx, &fy);

    BIG_560_29_copy(ee,e);
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
    BIG_560_29_mod(ee,q);
    gs(u, ee);

    FP48_BLS48556_copy(&g[0], f);
    for (i = 1; i < 16; i++)
    {
        FP48_BLS48556_copy(&g[i], &g[i - 1]);
        FP48_BLS48556_frob(&g[i], &X, 1);
    }

    for (i = 0; i < 16; i++)
    {
        np = BIG_560_29_nbits(u[i]);
        BIG_560_29_modneg(t, u[i], q);
        nn = BIG_560_29_nbits(t);
        if (nn < np)
        {
            BIG_560_29_copy(u[i], t);
            FP48_BLS48556_conj(&g[i], &g[i]);
        }
        BIG_560_29_norm(u[i]);
    }
    FP48_BLS48556_pow16(f, g, u);

#else
    BIG_560_29_copy(ee,e);
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
    BIG_560_29_mod(ee,q);
    FP48_BLS48556_pow(f, f, ee);
#endif
}

/* test G1 group membership */

int PAIR_BLS48556_G1member(ECP_BLS48556 *P)
{
    ECP_BLS48556 W,T;
    BIG_560_29 x;
    FP_BLS48556 cru;
    if (ECP_BLS48556_isinf(P)) return 0;

    BIG_560_29_rcopy(x, CURVE_Bnx_BLS48556);
    ECP_BLS48556_copy(&W,P);
    ECP_BLS48556_copy(&T,P);
    ECP_BLS48556_mul(&T,x); 
    if (ECP_BLS48556_equals(P,&T)) return 0;    // P is of low order        
    ECP_BLS48556_mul(&T,x);
    ECP_BLS48556_mul(&T,x); ECP_BLS48556_mul(&T,x);
    ECP_BLS48556_mul(&T,x); ECP_BLS48556_mul(&T,x);
    ECP_BLS48556_mul(&T,x); ECP_BLS48556_mul(&T,x);
    ECP_BLS48556_neg(&T);

    FP_BLS48556_rcopy(&cru, CRu_BLS48556);
    FP_BLS48556_mul(&(W.x), &(W.x), &cru);
    if (!ECP_BLS48556_equals(&W,&T)) return 0;  // check that Endomorphism works

// Not needed
//    ECP_BLS48556_add(&W,P);
//    FP_BLS48556_mul(&(T.x), &(T.x), &cru);
//    ECP_BLS48556_add(&W,&T);
//    if (!ECP_BLS48556_isinf(&W)) return 0;      // use it to check order
/*	BIG_560_29 q;
	ECP_BLS48556 W;
    if (ECP_BLS48556_isinf(P)) return 0;
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
	ECP_BLS48556_copy(&W,P);
	ECP_BLS48556_mul(&W,q);
	if (!ECP_BLS48556_isinf(&W)) return 0; */
	return 1;
}

/* test G2 group membership */

int PAIR_BLS48556_G2member(ECP8_BLS48556 *P)
{
    ECP8_BLS48556 W,T;
    BIG_560_29 x;
    FP2_BLS48556 X[3];

    if (ECP8_BLS48556_isinf(P)) return 0;
    ECP8_BLS48556_frob_constants(X);
    BIG_560_29_rcopy(x, CURVE_Bnx_BLS48556);

    ECP8_BLS48556_copy(&W,P);
    ECP8_BLS48556_frob(&W, X, 1);

    ECP8_BLS48556_copy(&T,P);
    ECP8_BLS48556_mul(&T,x);

#if SIGN_OF_X_BLS48556==NEGATIVEX
    ECP8_BLS48556_neg(&T);
#endif
/*
    ECP8_BLS48556_copy(&R,&W);
    ECP8_BLS48556_frob(&R,X,1);    // R=\psi^2(P)
    ECP8_BLS48556_sub(&W,&R);
    ECP8_BLS48556_copy(&R,&T);    // R=xP
    ECP8_BLS48556_frob(&R,X,1);
    ECP8_BLS48556_add(&W,&R);     // W=\psi(P)-\psi^2(P)+\psi(xP)
*/
    if (ECP8_BLS48556_equals(&W,&T)) return 1;
    return 0;
/*
	BIG_560_29 q;
	ECP8_BLS48556 W;
    if (ECP8_BLS48556_isinf(P)) return 0;
    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
	ECP8_BLS48556_copy(&W,P);
	ECP8_BLS48556_mul(&W,q);
	if (!ECP8_BLS48556_isinf(&W)) return 0;
	return 1;
*/
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^16}=m^{p^8} */
int PAIR_BLS48556_GTcyclotomic(FP48_BLS48556 *m)
{
	FP_BLS48556 fx,fy;
	FP2_BLS48556 X;
	FP48_BLS48556 r,w;
	if (FP48_BLS48556_isunity(m)) return 0;
	FP48_BLS48556_conj(&r,m);
	FP48_BLS48556_mul(&r,m);
	if (!FP48_BLS48556_isunity(&r)) return 0;

	FP_BLS48556_rcopy(&fx,Fra_BLS48556);
	FP_BLS48556_rcopy(&fy,Frb_BLS48556);
	FP2_BLS48556_from_FPs(&X,&fx,&fy);

	FP48_BLS48556_copy(&r,m); FP48_BLS48556_frob(&r,&X,8); 
	FP48_BLS48556_copy(&w,&r); FP48_BLS48556_frob(&w,&X,8); 
	FP48_BLS48556_mul(&w,m);

	if (!FP48_BLS48556_equals(&w,&r)) return 0;
    return 1;
}

/* test for full GT group membership */
int PAIR_BLS48556_GTmember(FP48_BLS48556 *m)
{
    BIG_560_29 x;
    FP2_BLS48556 X;
    FP_BLS48556 fx, fy;
    FP48_BLS48556 r,t;
    if (!PAIR_BLS48556_GTcyclotomic(m)) return 0;

    FP_BLS48556_rcopy(&fx, Fra_BLS48556);
    FP_BLS48556_rcopy(&fy, Frb_BLS48556);
    FP2_BLS48556_from_FPs(&X, &fx, &fy);
    BIG_560_29_rcopy(x, CURVE_Bnx_BLS48556);

    FP48_BLS48556_copy(&r,m);
    FP48_BLS48556_frob(&r, &X, 1);

    FP48_BLS48556_pow(&t,m,x);

#if SIGN_OF_X_BLS48556==NEGATIVEX
    FP48_BLS48556_conj(&t,&t);
#endif

    if (FP48_BLS48556_equals(&r,&t)) return 1;
    return 0;
/*
	BIG_560_29 q;
    FP48_BLS48556 r;
    if (!PAIR_BLS48556_GTcyclotomic(m)) return 0;

    BIG_560_29_rcopy(q, CURVE_Order_BLS48556);
	FP48_BLS48556_pow(&r,m,q);
	if (!FP48_BLS48556_isunity(&r)) return 0;
	return 1;
*/
}

#ifdef HAS_MAIN

using namespace std;
using namespace BLS48556;


// g++ -O2 pair8_BLS48.cpp ecp8_BLS48.cpp fp48_BLS48.cpp fp16_BLS48.cpp fp8_BLS48.cpp fp4_BLS48.cpp fp2_BLS48.cpp ecp_BLS48.cpp fp_BLS48.cpp big_B560_29.cpp rom_curve_BLS48.cpp rom_field_BLS48.cpp rand.cpp hash.cpp oct.cpp -o pair8_BLS48.exe

int main()
{
    int i;
    char byt[32];
    csprng rng;
    BIG xa, xb, ya, yb, w, a, b, t1, q, u[2], v[4], m, r, xx, x2, x4, p;
    ECP8 P, G;
    ECP Q, R;
    FP48 g, gp;
    FP16 t, c, cp, cpm1, cpm2;
    FP8 X, Y;
    FP2 x, y, f, Aa, Bb;
    FP cru;

    for (i = 0; i < 32; i++)
        byt[i] = i + 9;
    RAND_seed(&rng, 32, byt);

    BIG_rcopy(r, CURVE_Order);
    BIG_rcopy(p, Modulus);


    BIG_rcopy(xa, CURVE_Gx_BLS48556);
    BIG_rcopy(ya, CURVE_Gy_BLS48556);

    ECP_BLS48556_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_BLS48556_output(&Q);
    printf("\n");

    ECP8_generator(&P);

    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    BIG_rcopy(a, Fra);
    BIG_rcopy(b, Frb);
    FP2_from_BIGs(&f, a, b);

    PAIR_BLS48556_ate(&g, &P, &Q);

    printf("gb= ");
    FP48_output(&g);
    printf("\n");
    PAIR_BLS48556_fexp(&g);

    printf("g= ");
    FP48_output(&g);
    printf("\n");

    ECP_BLS48556_copy(&R, &Q);
    ECP8_copy(&G, &P);

    ECP8_dbl(&G);
    ECP_dbl(&R);
    ECP_affine(&R);

    PAIR_BLS48556_ate(&g, &G, &Q);
    PAIR_BLS48556_fexp(&g);

    printf("g1= ");
    FP48_output(&g);
    printf("\n");

    PAIR_BLS48556_ate(&g, &P, &R);
    PAIR_BLS48556_fexp(&g);

    printf("g2= ");
    FP48_output(&g);
    printf("\n");


    PAIR_BLS48556_G1mul(&Q, r);
    printf("rQ= "); ECP_output(&Q); printf("\n");

    PAIR_BLS48556_G2mul(&P, r);
    printf("rP= "); ECP8_output(&P); printf("\n");

    PAIR_BLS48556_GTpow(&g, r);
    printf("g^r= "); FP48_output(&g); printf("\n");


    BIG_randomnum(w, r, &rng);

    FP48_copy(&gp, &g);

    PAIR_BLS48556_GTpow(&g, w);

    FP48_trace(&t, &g);

    printf("g^r=  "); FP16_output(&t); printf("\n");

    FP48_compow(&t, &gp, w, r);

    printf("t(g)= "); FP16_output(&t); printf("\n");

}

#endif

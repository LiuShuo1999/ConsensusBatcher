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

/* CORE BN Curve pairing functions */

//#define HAS_MAIN

#include "pair_BLS12461.h"

// Point doubling for pairings
static void PAIR_BLS12461_double(ECP2_BLS12461 *A, FP2_BLS12461 *AA, FP2_BLS12461 *BB, FP2_BLS12461 *CC)
{
    FP2_BLS12461 YY;
    FP2_BLS12461_copy(CC, &(A->x));  //FP2 XX=new FP2(A.getx());  //X
    FP2_BLS12461_copy(&YY, &(A->y)); //FP2 YY=new FP2(A.gety());  //Y
    FP2_BLS12461_copy(BB, &(A->z));  //FP2 ZZ=new FP2(A.getz());  //Z

    FP2_BLS12461_copy(AA, &YY);      //FP2 YZ=new FP2(YY);        //Y
    FP2_BLS12461_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP2_BLS12461_sqr(CC, CC);        //XX.sqr();                //X^2
    FP2_BLS12461_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP2_BLS12461_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP2_BLS12461_add(AA, AA, AA);
    FP2_BLS12461_neg(AA, AA);
    FP2_BLS12461_norm(AA);           // -2YZ
    FP2_BLS12461_mul_ip(AA);
    FP2_BLS12461_norm(AA);           // -2YZi

    FP2_BLS12461_imul(BB, BB, 3 * CURVE_B_I_BLS12461); //3Bz^2
    FP2_BLS12461_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_BLS12461==D_TYPE
    FP2_BLS12461_mul_ip(&YY);                    // Y^2.i
    FP2_BLS12461_norm(&YY);
    FP2_BLS12461_mul_ip(CC);                 // 3X^2.i
    FP2_BLS12461_norm(CC);
#endif

#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP2_BLS12461_mul_ip(BB);                 // 3Bz^2.i
    FP2_BLS12461_norm(BB);
#endif

    FP2_BLS12461_sub(BB, BB, &YY);
    FP2_BLS12461_norm(BB);

    ECP2_BLS12461_dbl(A);            //A.dbl();
}

// Point addition for pairings
static void PAIR_BLS12461_add(ECP2_BLS12461 *A, ECP2_BLS12461 *B, FP2_BLS12461 *AA, FP2_BLS12461 *BB, FP2_BLS12461 *CC)
{
    FP2_BLS12461 T1;
    FP2_BLS12461_copy(AA, &(A->x));      //FP2 X1=new FP2(A.getx());    // X1
    FP2_BLS12461_copy(CC, &(A->y));      //FP2 Y1=new FP2(A.gety());    // Y1
    FP2_BLS12461_copy(&T1, &(A->z));     //FP2 T1=new FP2(A.getz());    // Z1

    FP2_BLS12461_copy(BB, &T1);          //FP2 T2=new FP2(A.getz());    // Z1

    FP2_BLS12461_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP2_BLS12461_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP2_BLS12461_sub(AA, AA, BB);        //X1.sub(T2);
    FP2_BLS12461_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP2_BLS12461_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP2_BLS12461_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP2_BLS12461_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP2_BLS12461_mul_ip(AA);
    FP2_BLS12461_norm(AA);
#endif

    FP2_BLS12461_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP2_BLS12461_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP2_BLS12461_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP2_BLS12461_sub(BB, BB, &T1);       //T2.sub(T1);
    FP2_BLS12461_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP2_BLS12461_neg(CC, CC);            //Y1.neg();
    FP2_BLS12461_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP2_BLS12461_add(A, B);             //A.add(B);
}

/* Line function */
static void PAIR_BLS12461_line(FP12_BLS12461 *v, ECP2_BLS12461 *A, ECP2_BLS12461 *B, FP_BLS12461 *Qx, FP_BLS12461 *Qy)
{
    FP2_BLS12461 AA, BB, CC;
    FP4_BLS12461 a, b, c;

    if (A == B)
        PAIR_BLS12461_double(A, &AA, &BB, &CC);
    else
        PAIR_BLS12461_add(A, B, &AA, &BB, &CC);

    FP2_BLS12461_pmul(&CC, &CC, Qx);
    FP2_BLS12461_pmul(&AA, &AA, Qy);

    FP4_BLS12461_from_FP2s(&a, &AA, &BB);
#if SEXTIC_TWIST_BLS12461==D_TYPE
    FP4_BLS12461_from_FP2(&b, &CC);
    FP4_BLS12461_zero(&c);
#endif
#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP4_BLS12461_zero(&b);
    FP4_BLS12461_from_FP2H(&c, &CC);
#endif

    FP12_BLS12461_from_FP4s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int PAIR_BLS12461_nbits(BIG_464_28 n3, BIG_464_28 n)
{
    BIG_464_28 x;
    BIG_464_28_rcopy(x, CURVE_Bnx_BLS12461);

#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    BIG_464_28_pmul(n, x, 6);
#if SIGN_OF_X_BLS12461==POSITIVEX
    BIG_464_28_inc(n, 2);
#else
    BIG_464_28_dec(n, 2);
#endif

#else
    BIG_464_28_copy(n, x);
#endif

    BIG_464_28_norm(n);
    BIG_464_28_pmul(n3, n, 3);
    BIG_464_28_norm(n3);

    return BIG_464_28_nbits(n3);
}

/*
    For multi-pairing, product of n pairings
    1. Declare FP12 array of length number of bits in Ate parameter
    2. Initialise this array by calling PAIR_initmp()
    3. Accumulate each pairing by calling PAIR_another() n times
    4. Call PAIR_miller()
    5. Call final exponentiation PAIR_fexp()
*/

/* prepare for multi-pairing */
void PAIR_BLS12461_initmp(FP12_BLS12461 r[])
{
    int i;
    for (i = ATE_BITS_BLS12461 - 1; i >= 0; i--)
        FP12_BLS12461_one(&r[i]);
    return;
}

// Store precomputed line details in an FP4
static void PAIR_BLS12461_pack(FP4_BLS12461 *T, FP2_BLS12461* AA, FP2_BLS12461* BB, FP2_BLS12461 *CC)
{
    FP2_BLS12461 I, A, B;
    FP2_BLS12461_inv(&I, CC, NULL);
    FP2_BLS12461_mul(&A, AA, &I);
    FP2_BLS12461_mul(&B, BB, &I);
    FP4_BLS12461_from_FP2s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void PAIR_BLS12461_unpack(FP12_BLS12461 *v, FP4_BLS12461* T, FP_BLS12461 *Qx, FP_BLS12461 *Qy)
{
    FP4_BLS12461 a, b, c;
    FP2_BLS12461 t;
    FP4_BLS12461_copy(&a, T);
    FP2_BLS12461_pmul(&a.a, &a.a, Qy);
    FP2_BLS12461_from_FP(&t, Qx);

#if SEXTIC_TWIST_BLS12461==D_TYPE
    FP4_BLS12461_from_FP2(&b, &t);
    FP4_BLS12461_zero(&c);
#endif
#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP4_BLS12461_zero(&b);
    FP4_BLS12461_from_FP2H(&c, &t);
#endif

    FP12_BLS12461_from_FP4s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}


/* basic Miller loop */
void PAIR_BLS12461_miller(FP12_BLS12461 *res, FP12_BLS12461 r[])
{
    int i;
    FP12_BLS12461_one(res);
    for (i = ATE_BITS_BLS12461 - 1; i >= 1; i--)
    {
        FP12_BLS12461_sqr(res, res);
        FP12_BLS12461_ssmul(res, &r[i]);
        FP12_BLS12461_zero(&r[i]);
    }

#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(res, res);
#endif
    FP12_BLS12461_ssmul(res, &r[0]);
    FP12_BLS12461_zero(&r[0]);
    return;
}


// Precompute table of line functions for fixed G2 value
void PAIR_BLS12461_precomp(FP4_BLS12461 T[], ECP2_BLS12461* GV)
{
    int i, j, nb, bt;
    BIG_464_28 n, n3;
    FP2_BLS12461 AA, BB, CC;
    ECP2_BLS12461 A, G, NG;
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    ECP2_BLS12461 K;
    FP2_BLS12461 X;
    FP_BLS12461 Qx, Qy;
    FP_BLS12461_rcopy(&Qx, Fra_BLS12461);
    FP_BLS12461_rcopy(&Qy, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &Qx, &Qy);
#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP2_BLS12461_inv(&X, &X, NULL);
    FP2_BLS12461_norm(&X);
#endif
#endif

    ECP2_BLS12461_copy(&A, GV);
    ECP2_BLS12461_copy(&G, GV);
    ECP2_BLS12461_copy(&NG, GV);
    ECP2_BLS12461_neg(&NG);

    nb = PAIR_BLS12461_nbits(n3, n);
    j = 0;

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS12461_double(&A, &AA, &BB, &CC);
        PAIR_BLS12461_pack(&T[j++], &AA, &BB, &CC);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS12461_add(&A, &G, &AA, &BB, &CC);
            PAIR_BLS12461_pack(&T[j++], &AA, &BB, &CC);
        }
        if (bt == -1)
        {
            PAIR_BLS12461_add(&A, &NG, &AA, &BB, &CC);
            PAIR_BLS12461_pack(&T[j++], &AA, &BB, &CC);
        }
    }
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE

#if SIGN_OF_X_BLS12461==NEGATIVEX
    ECP2_BLS12461_neg(&A);
#endif

    ECP2_BLS12461_copy(&K, &G);
    ECP2_BLS12461_frob(&K, &X);
    PAIR_BLS12461_add(&A, &K, &AA, &BB, &CC);
    PAIR_BLS12461_pack(&T[j++], &AA, &BB, &CC);
    ECP2_BLS12461_frob(&K, &X);
    ECP2_BLS12461_neg(&K);
    PAIR_BLS12461_add(&A, &K, &AA, &BB, &CC);
    PAIR_BLS12461_pack(&T[j++], &AA, &BB, &CC);

#endif
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void PAIR_BLS12461_another_pc(FP12_BLS12461 r[], FP4_BLS12461 T[], ECP_BLS12461 *QV)
{
    int i, j, nb, bt;
    BIG_464_28 x, n, n3;
    FP12_BLS12461 lv, lv2;
    ECP_BLS12461 Q;
    FP_BLS12461 Qx, Qy;

    if (ECP_BLS12461_isinf(QV)) return;

    nb = PAIR_BLS12461_nbits(n3, n);

    ECP_BLS12461_copy(&Q, QV);
    ECP_BLS12461_affine(&Q);

    FP_BLS12461_copy(&Qx, &(Q.x));
    FP_BLS12461_copy(&Qy, &(Q.y));

    j = 0;
    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS12461_unpack(&lv, &T[j++], &Qx, &Qy);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS12461_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP12_BLS12461_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS12461_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP12_BLS12461_smul(&lv, &lv2);
        }
        FP12_BLS12461_ssmul(&r[i], &lv);
    }

#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    PAIR_BLS12461_unpack(&lv, &T[j++], &Qx, &Qy);
    PAIR_BLS12461_unpack(&lv2, &T[j++], &Qx, &Qy);
    FP12_BLS12461_smul(&lv, &lv2);
    FP12_BLS12461_ssmul(&r[0], &lv);
#endif
}

/* Accumulate another set of line functions for n-pairing */
void PAIR_BLS12461_another(FP12_BLS12461 r[], ECP2_BLS12461* PV, ECP_BLS12461* QV)
{
    int i, j, nb, bt;
    BIG_464_28 x, n, n3;
    FP12_BLS12461 lv, lv2;
    ECP2_BLS12461 A, NP, P;
    ECP_BLS12461 Q;
    FP_BLS12461 Qx, Qy;
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    ECP2_BLS12461 K;
    FP2_BLS12461 X;
    FP_BLS12461_rcopy(&Qx, Fra_BLS12461);
    FP_BLS12461_rcopy(&Qy, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &Qx, &Qy);
#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP2_BLS12461_inv(&X, &X, NULL);
    FP2_BLS12461_norm(&X);
#endif
#endif

    if (ECP_BLS12461_isinf(QV)) return;

    nb = PAIR_BLS12461_nbits(n3, n);

    ECP2_BLS12461_copy(&P, PV);
    ECP_BLS12461_copy(&Q, QV);

    ECP2_BLS12461_affine(&P);
    ECP_BLS12461_affine(&Q);

    FP_BLS12461_copy(&Qx, &(Q.x));
    FP_BLS12461_copy(&Qy, &(Q.y));

    ECP2_BLS12461_copy(&A, &P);
    ECP2_BLS12461_copy(&NP, &P); ECP2_BLS12461_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS12461_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS12461_line(&lv2, &A, &P, &Qx, &Qy);
            FP12_BLS12461_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS12461_line(&lv2, &A, &NP, &Qx, &Qy);
            FP12_BLS12461_smul(&lv, &lv2);
        }
        FP12_BLS12461_ssmul(&r[i], &lv);
    }

#if PAIRING_FRIENDLY_BLS12461==BN_CURVE

#if SIGN_OF_X_BLS12461==NEGATIVEX
    ECP2_BLS12461_neg(&A);
#endif

    ECP2_BLS12461_copy(&K, &P);
    ECP2_BLS12461_frob(&K, &X);
    PAIR_BLS12461_line(&lv, &A, &K, &Qx, &Qy);
    ECP2_BLS12461_frob(&K, &X);
    ECP2_BLS12461_neg(&K);
    PAIR_BLS12461_line(&lv2, &A, &K, &Qx, &Qy);
    FP12_BLS12461_smul(&lv, &lv2);
    FP12_BLS12461_ssmul(&r[0], &lv);

#endif
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_BLS12461_ate(FP12_BLS12461 *r, ECP2_BLS12461 *P1, ECP_BLS12461 *Q1)
{

    BIG_464_28 x, n, n3;
    FP_BLS12461 Qx, Qy;
    int i, nb, bt;
    ECP2_BLS12461 A, NP, P;
    ECP_BLS12461 Q;
    FP12_BLS12461 lv, lv2;
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    ECP2_BLS12461 KA;
    FP2_BLS12461 X;

    FP_BLS12461_rcopy(&Qx, Fra_BLS12461);
    FP_BLS12461_rcopy(&Qy, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &Qx, &Qy);

#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP2_BLS12461_inv(&X, &X, NULL);
    FP2_BLS12461_norm(&X);
#endif
#endif

    FP12_BLS12461_one(r);
    if (ECP_BLS12461_isinf(Q1)) return;

    nb = PAIR_BLS12461_nbits(n3, n);

    ECP2_BLS12461_copy(&P, P1);
    ECP_BLS12461_copy(&Q, Q1);

    ECP2_BLS12461_affine(&P);
    ECP_BLS12461_affine(&Q);

    FP_BLS12461_copy(&Qx, &(Q.x));
    FP_BLS12461_copy(&Qy, &(Q.y));

    ECP2_BLS12461_copy(&A, &P);
    ECP2_BLS12461_copy(&NP, &P); ECP2_BLS12461_neg(&NP);



    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--) //0
    {
        FP12_BLS12461_sqr(r, r);
        PAIR_BLS12461_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS12461_line(&lv2, &A, &P, &Qx, &Qy);
            FP12_BLS12461_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS12461_line(&lv2, &A, &NP, &Qx, &Qy);
            FP12_BLS12461_smul(&lv, &lv2);
        }
        FP12_BLS12461_ssmul(r, &lv);

    }


#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(r, r);
#endif

    /* R-ate fixup required for BN curves */
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE

#if SIGN_OF_X_BLS12461==NEGATIVEX
    ECP2_BLS12461_neg(&A);
#endif

    ECP2_BLS12461_copy(&KA, &P);
    ECP2_BLS12461_frob(&KA, &X);
    PAIR_BLS12461_line(&lv, &A, &KA, &Qx, &Qy);
    ECP2_BLS12461_frob(&KA, &X);
    ECP2_BLS12461_neg(&KA);
    PAIR_BLS12461_line(&lv2, &A, &KA, &Qx, &Qy);
    FP12_BLS12461_smul(&lv, &lv2);
    FP12_BLS12461_ssmul(r, &lv);
#endif
}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_BLS12461_double_ate(FP12_BLS12461 *r, ECP2_BLS12461 *P1, ECP_BLS12461 *Q1, ECP2_BLS12461 *R1, ECP_BLS12461 *S1)
{
    BIG_464_28 x, n, n3;
    FP_BLS12461 Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP2_BLS12461 A, B, NP, NR, P, R;
    ECP_BLS12461 Q, S;
    FP12_BLS12461 lv, lv2;
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    FP2_BLS12461 X;
    ECP2_BLS12461 K;

    FP_BLS12461_rcopy(&Qx, Fra_BLS12461);
    FP_BLS12461_rcopy(&Qy, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &Qx, &Qy);

#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP2_BLS12461_inv(&X, &X, NULL);
    FP2_BLS12461_norm(&X);
#endif
#endif

    if (ECP_BLS12461_isinf(Q1))
    {
        PAIR_BLS12461_ate(r, R1, S1);
        return;
    }
    if (ECP_BLS12461_isinf(S1))
    {
        PAIR_BLS12461_ate(r, P1, Q1);
        return;
    }
    nb = PAIR_BLS12461_nbits(n3, n);

    ECP2_BLS12461_copy(&P, P1);
    ECP_BLS12461_copy(&Q, Q1);

    ECP2_BLS12461_affine(&P);
    ECP_BLS12461_affine(&Q);

    ECP2_BLS12461_copy(&R, R1);
    ECP_BLS12461_copy(&S, S1);

    ECP2_BLS12461_affine(&R);
    ECP_BLS12461_affine(&S);

    FP_BLS12461_copy(&Qx, &(Q.x));
    FP_BLS12461_copy(&Qy, &(Q.y));

    FP_BLS12461_copy(&Sx, &(S.x));
    FP_BLS12461_copy(&Sy, &(S.y));

    ECP2_BLS12461_copy(&A, &P);
    ECP2_BLS12461_copy(&B, &R);

    ECP2_BLS12461_copy(&NP, &P); ECP2_BLS12461_neg(&NP);
    ECP2_BLS12461_copy(&NR, &R); ECP2_BLS12461_neg(&NR);

    FP12_BLS12461_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP12_BLS12461_sqr(r, r);
        PAIR_BLS12461_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_BLS12461_line(&lv2, &B, &B, &Sx, &Sy);
        FP12_BLS12461_smul(&lv, &lv2);
        FP12_BLS12461_ssmul(r, &lv);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS12461_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_BLS12461_line(&lv2, &B, &R, &Sx, &Sy);
            FP12_BLS12461_smul(&lv, &lv2);
            FP12_BLS12461_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_BLS12461_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_BLS12461_line(&lv2, &B, &NR, &Sx, &Sy);
            FP12_BLS12461_smul(&lv, &lv2);
            FP12_BLS12461_ssmul(r, &lv);
        }

    }


    /* R-ate fixup required for BN curves */

#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(r, r);
#endif

#if PAIRING_FRIENDLY_BLS12461==BN_CURVE

#if SIGN_OF_X_BLS12461==NEGATIVEX
    ECP2_BLS12461_neg(&A);
    ECP2_BLS12461_neg(&B);
#endif

    ECP2_BLS12461_copy(&K, &P);
    ECP2_BLS12461_frob(&K, &X);
    PAIR_BLS12461_line(&lv, &A, &K, &Qx, &Qy);
    ECP2_BLS12461_frob(&K, &X);
    ECP2_BLS12461_neg(&K);
    PAIR_BLS12461_line(&lv2, &A, &K, &Qx, &Qy);
    FP12_BLS12461_smul(&lv, &lv2);
    FP12_BLS12461_ssmul(r, &lv);

    ECP2_BLS12461_copy(&K, &R);
    ECP2_BLS12461_frob(&K, &X);
    PAIR_BLS12461_line(&lv, &B, &K, &Sx, &Sy);
    ECP2_BLS12461_frob(&K, &X);
    ECP2_BLS12461_neg(&K);
    PAIR_BLS12461_line(&lv2, &B, &K, &Sx, &Sy);
    FP12_BLS12461_smul(&lv, &lv2);
    FP12_BLS12461_ssmul(r, &lv);
#endif
}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
void PAIR_BLS12461_fexp(FP12_BLS12461 *r)
{
    FP2_BLS12461 X;
    BIG_464_28 x;
    FP_BLS12461 a, b;
    FP12_BLS12461 t0, y0, y1;
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    FP12_BLS12461 y2, y3;
#endif

    BIG_464_28_rcopy(x, CURVE_Bnx_BLS12461);
    FP_BLS12461_rcopy(&a, Fra_BLS12461);
    FP_BLS12461_rcopy(&b, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &a, &b);

    /* Easy part of final exp */

    FP12_BLS12461_inv(&t0, r);
    FP12_BLS12461_conj(r, r);

    FP12_BLS12461_mul(r, &t0);
    FP12_BLS12461_copy(&t0, r);

    FP12_BLS12461_frob(r, &X);
    FP12_BLS12461_frob(r, &X);
    FP12_BLS12461_mul(r, &t0);

    /* Hard part of final exp - see Duquesne & Ghamman eprint 2015/192.pdf */
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    FP12_BLS12461_pow(&t0, r, x); // t0=f^-u
#if SIGN_OF_X_BLS12461==POSITIVEX
    FP12_BLS12461_conj(&t0, &t0);
#endif
    FP12_BLS12461_usqr(&y3, &t0); // y3=t0^2
    FP12_BLS12461_copy(&y0, &t0);
    FP12_BLS12461_mul(&y0, &y3); // y0=t0*y3
    FP12_BLS12461_copy(&y2, &y3);
    FP12_BLS12461_frob(&y2, &X); // y2=y3^p
    FP12_BLS12461_mul(&y2, &y3); //y2=y2*y3
    FP12_BLS12461_usqr(&y2, &y2); //y2=y2^2
    FP12_BLS12461_mul(&y2, &y3); // y2=y2*y3

    FP12_BLS12461_pow(&t0, &y0, x); //t0=y0^-u
#if SIGN_OF_X_BLS12461==POSITIVEX
    FP12_BLS12461_conj(&t0, &t0);
#endif
    FP12_BLS12461_conj(&y0, r);    //y0=~r
    FP12_BLS12461_copy(&y1, &t0);
    FP12_BLS12461_frob(&y1, &X);
    FP12_BLS12461_frob(&y1, &X); //y1=t0^p^2
    FP12_BLS12461_mul(&y1, &y0); // y1=y0*y1
    FP12_BLS12461_conj(&t0, &t0); // t0=~t0
    FP12_BLS12461_copy(&y3, &t0);
    FP12_BLS12461_frob(&y3, &X); //y3=t0^p
    FP12_BLS12461_mul(&y3, &t0); // y3=t0*y3
    FP12_BLS12461_usqr(&t0, &t0); // t0=t0^2
    FP12_BLS12461_mul(&y1, &t0); // y1=t0*y1

    FP12_BLS12461_pow(&t0, &y3, x); // t0=y3^-u
#if SIGN_OF_X_BLS12461==POSITIVEX
    FP12_BLS12461_conj(&t0, &t0);
#endif
    FP12_BLS12461_usqr(&t0, &t0); //t0=t0^2
    FP12_BLS12461_conj(&t0, &t0); //t0=~t0
    FP12_BLS12461_mul(&y3, &t0); // y3=t0*y3

    FP12_BLS12461_frob(r, &X);
    FP12_BLS12461_copy(&y0, r);
    FP12_BLS12461_frob(r, &X);
    FP12_BLS12461_mul(&y0, r);
    FP12_BLS12461_frob(r, &X);
    FP12_BLS12461_mul(&y0, r);

    FP12_BLS12461_usqr(r, &y3); //r=y3^2
    FP12_BLS12461_mul(r, &y2);  //r=y2*r
    FP12_BLS12461_copy(&y3, r);
    FP12_BLS12461_mul(&y3, &y0); // y3=r*y0
    FP12_BLS12461_mul(r, &y1); // r=r*y1
    FP12_BLS12461_usqr(r, r); // r=r^2
    FP12_BLS12461_mul(r, &y3); // r=r*y3
    FP12_BLS12461_reduce(r);
#else

// See https://eprint.iacr.org/2020/875.pdf
    FP12_BLS12461_usqr(&y1,r);
    FP12_BLS12461_mul(&y1,r);     // y1=r^3

    FP12_BLS12461_pow(&y0,r,x);   // y0=r^x
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&y0, &y0);
#endif
    FP12_BLS12461_conj(&t0,r);    // t0=r^-1
    FP12_BLS12461_copy(r,&y0);
    FP12_BLS12461_mul(r,&t0);    // r=r^(x-1)

    FP12_BLS12461_pow(&y0,r,x);   // y0=r^x
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&y0, &y0);
#endif
    FP12_BLS12461_conj(&t0,r);    // t0=r^-1
    FP12_BLS12461_copy(r,&y0);
    FP12_BLS12461_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP12_BLS12461_pow(&y0,r,x);  // y0=r^x
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&y0, &y0);
#endif
    FP12_BLS12461_copy(&t0,r);   
    FP12_BLS12461_frob(&t0,&X); // t0=r^p
    FP12_BLS12461_copy(r,&y0);
    FP12_BLS12461_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2-1)
    FP12_BLS12461_pow(&y0,r,x);  
    FP12_BLS12461_pow(&y0,&y0,x); // y0=r^x^2
    FP12_BLS12461_copy(&t0,r);    
    FP12_BLS12461_frob(&t0,&X);
    FP12_BLS12461_frob(&t0,&X);   // t0=r^p^2
    FP12_BLS12461_mul(&y0,&t0);   // y0=r^x^2.r^p^2
    FP12_BLS12461_conj(&t0,r);    // t0=r^-1
    FP12_BLS12461_copy(r,&y0);    // 
    FP12_BLS12461_mul(r,&t0);     // r=r^x^2.r^p^2.r^-1

    FP12_BLS12461_mul(r,&y1);    
    FP12_BLS12461_reduce(r);

// Ghamman & Fouotsa Method
/*
    FP12_BLS12461_usqr(&y0, r);
    FP12_BLS12461_pow(&y1, &y0, x);
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&y1, &y1);
#endif

    BIG_464_28_fshr(x, 1);
    FP12_BLS12461_pow(&y2, &y1, x);
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&y2, &y2);
#endif

    BIG_464_28_fshl(x, 1); // x must be even
    FP12_BLS12461_conj(&y3, r);
    FP12_BLS12461_mul(&y1, &y3);

    FP12_BLS12461_conj(&y1, &y1);
    FP12_BLS12461_mul(&y1, &y2);

    FP12_BLS12461_pow(&y2, &y1, x);
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&y2, &y2);
#endif

    FP12_BLS12461_pow(&y3, &y2, x);
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&y3, &y3);
#endif
    FP12_BLS12461_conj(&y1, &y1);
    FP12_BLS12461_mul(&y3, &y1);

    FP12_BLS12461_conj(&y1, &y1);
    FP12_BLS12461_frob(&y1, &X);
    FP12_BLS12461_frob(&y1, &X);
    FP12_BLS12461_frob(&y1, &X);
    FP12_BLS12461_frob(&y2, &X);
    FP12_BLS12461_frob(&y2, &X);
    FP12_BLS12461_mul(&y1, &y2);

    FP12_BLS12461_pow(&y2, &y3, x);
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&y2, &y2);
#endif
    FP12_BLS12461_mul(&y2, &y0);
    FP12_BLS12461_mul(&y2, r);

    FP12_BLS12461_mul(&y1, &y2);
    FP12_BLS12461_copy(&y2, &y3);
    FP12_BLS12461_frob(&y2, &X);
    FP12_BLS12461_mul(&y1, &y2);
    FP12_BLS12461_copy(r, &y1);
    FP12_BLS12461_reduce(r);
*/
#endif
}

#ifdef USE_GLV_BLS12461
/* GLV method */
static void glv(BIG_464_28 u[2], BIG_464_28 ee)
{
    BIG_464_28 q;
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    int i, j;
    BIG_464_28 v[2], t;
    DBIG_464_28 d;
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);

    for (i = 0; i < 2; i++)
    {
        BIG_464_28_rcopy(t, CURVE_W_BLS12461[i]);
        BIG_464_28_mul(d, t, ee);
        BIG_464_28_ctddiv(v[i],d,q,BIG_464_28_nbits(t));
        BIG_464_28_zero(u[i]);
    }
    BIG_464_28_copy(u[0], ee);
    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
        {
            BIG_464_28_rcopy(t, CURVE_SB_BLS12461[j][i]);
            BIG_464_28_modmul(t, v[j], t, q);
            BIG_464_28_add(u[i], u[i], q);
            BIG_464_28_sub(u[i], u[i], t);
            BIG_464_28_ctmod(u[i],q,1);
        }

#else
// -(x^2).P = (Beta.x,y)
    int bd;
    BIG_464_28 x, x2;
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
    BIG_464_28_rcopy(x, CURVE_Bnx_BLS12461);

    BIG_464_28_smul(x2, x, x);
    bd=BIG_464_28_nbits(q)-BIG_464_28_nbits(x2); // fixed
    BIG_464_28_copy(u[0], ee);
    BIG_464_28_ctmod(u[0], x2, bd);
    BIG_464_28_copy(u[1], ee);
    BIG_464_28_ctsdiv(u[1], x2, bd);

    BIG_464_28_sub(u[1], q, u[1]);

#endif

    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void gs(BIG_464_28 u[4], BIG_464_28 ee)
{
    int i;
    BIG_464_28 q;
#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    int j;
    BIG_464_28 v[4], t;
    DBIG_464_28 d;
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);

    for (i = 0; i < 4; i++)
    {
        BIG_464_28_rcopy(t, CURVE_WB_BLS12461[i]);
        BIG_464_28_mul(d, t, ee);
        BIG_464_28_ctddiv(v[i],d,q,BIG_464_28_nbits(t));
        BIG_464_28_zero(u[i]);
    }

    BIG_464_28_copy(u[0], ee);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            BIG_464_28_rcopy(t, CURVE_BB_BLS12461[j][i]);
            BIG_464_28_modmul(t, v[j], t, q);
            BIG_464_28_add(u[i], u[i], q);
            BIG_464_28_sub(u[i], u[i], t);
            BIG_464_28_ctmod(u[i],q,1);
        }
#else
    int bd;
    BIG_464_28 x, w;
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);

    BIG_464_28_rcopy(x, CURVE_Bnx_BLS12461);
    BIG_464_28_copy(w, ee);
    bd=BIG_464_28_nbits(q)-BIG_464_28_nbits(x); // fixed

    for (i = 0; i < 3; i++)
    {
        BIG_464_28_copy(u[i], w);
        BIG_464_28_ctmod(u[i],x,bd);
        BIG_464_28_ctsdiv(w,x,bd);
    }
    BIG_464_28_copy(u[3], w);

    /*  */
#if SIGN_OF_X_BLS12461==NEGATIVEX
    BIG_464_28_modneg(u[1], u[1], q);
    BIG_464_28_modneg(u[3], u[3], q);
#endif

#endif
    return;
}

/* Multiply P by e in group G1 */
void PAIR_BLS12461_G1mul(ECP_BLS12461 *P, BIG_464_28 e)
{
    BIG_464_28 ee,q;
#ifdef USE_GLV_BLS12461   /* Note this method is patented */
    int np, nn;
    ECP_BLS12461 Q;
    FP_BLS12461 cru;
    BIG_464_28 t;
    BIG_464_28 u[2];

    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
    BIG_464_28_mod(ee,q);

    glv(u, ee);

    ECP_BLS12461_copy(&Q, P); ECP_BLS12461_affine(&Q);
    FP_BLS12461_rcopy(&cru, CRu_BLS12461);
    FP_BLS12461_mul(&(Q.x), &(Q.x), &cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np = BIG_464_28_nbits(u[0]);
    BIG_464_28_modneg(t, u[0], q);
    nn = BIG_464_28_nbits(t);
    if (nn < np)
    {
        BIG_464_28_copy(u[0], t);
        ECP_BLS12461_neg(P);
    }

    np = BIG_464_28_nbits(u[1]);
    BIG_464_28_modneg(t, u[1], q);
    nn = BIG_464_28_nbits(t);
    if (nn < np)
    {
        BIG_464_28_copy(u[1], t);
        ECP_BLS12461_neg(&Q);
    }
    BIG_464_28_norm(u[0]);
    BIG_464_28_norm(u[1]);
    ECP_BLS12461_mul2(P, &Q, u[0], u[1]);

#else
    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
    BIG_464_28_mod(ee,q);
    ECP_BLS12461_clmul(P, ee, q);
#endif
}

/* Multiply P by e in group G2 */
void PAIR_BLS12461_G2mul(ECP2_BLS12461 *P, BIG_464_28 e)
{
    BIG_464_28 ee,q;
#ifdef USE_GS_G2_BLS12461   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP2_BLS12461 Q[4];
    FP2_BLS12461 X;
    FP_BLS12461 fx, fy;
    BIG_464_28 x, u[4];

    FP_BLS12461_rcopy(&fx, Fra_BLS12461);
    FP_BLS12461_rcopy(&fy, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &fx, &fy);

#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP2_BLS12461_inv(&X, &X, NULL);
    FP2_BLS12461_norm(&X);
#endif

    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
    BIG_464_28_mod(ee,q);

    gs(u, ee);

    ECP2_BLS12461_copy(&Q[0], P);
    for (i = 1; i < 4; i++)
    {
        ECP2_BLS12461_copy(&Q[i], &Q[i - 1]);
        ECP2_BLS12461_frob(&Q[i], &X);
    }

    for (i = 0; i < 4; i++)
    {
        np = BIG_464_28_nbits(u[i]);
        BIG_464_28_modneg(x, u[i], q);
        nn = BIG_464_28_nbits(x);
        if (nn < np)
        {
            BIG_464_28_copy(u[i], x);
            ECP2_BLS12461_neg(&Q[i]);
        }
        BIG_464_28_norm(u[i]);
    }

    ECP2_BLS12461_mul4(P, Q, u);

#else
    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
    BIG_464_28_mod(ee,q);
    ECP2_BLS12461_mul(P, ee);
#endif
}

/* f=f^e */
void PAIR_BLS12461_GTpow(FP12_BLS12461 *f, BIG_464_28 e)
{
    BIG_464_28 ee,q;
#ifdef USE_GS_GT_BLS12461   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see fp4.c */
    int i, np, nn;
    FP12_BLS12461 g[4];
    FP2_BLS12461 X;
    BIG_464_28 t;
    FP_BLS12461 fx, fy;
    BIG_464_28 u[4];

    FP_BLS12461_rcopy(&fx, Fra_BLS12461);
    FP_BLS12461_rcopy(&fy, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &fx, &fy);

    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
    BIG_464_28_mod(ee,q);

    gs(u, ee);

    FP12_BLS12461_copy(&g[0], f);
    for (i = 1; i < 4; i++)
    {
        FP12_BLS12461_copy(&g[i], &g[i - 1]);
        FP12_BLS12461_frob(&g[i], &X);
    }

    for (i = 0; i < 4; i++)
    {
        np = BIG_464_28_nbits(u[i]);
        BIG_464_28_modneg(t, u[i], q);
        nn = BIG_464_28_nbits(t);
        if (nn < np)
        {
            BIG_464_28_copy(u[i], t);
            FP12_BLS12461_conj(&g[i], &g[i]);
        }
        BIG_464_28_norm(u[i]);
    }
    FP12_BLS12461_pow4(f, g, u);

#else
    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
    BIG_464_28_mod(ee,q);
    FP12_BLS12461_pow(f, f, ee);
#endif
}


/* test G1 group membership */

int PAIR_BLS12461_G1member(ECP_BLS12461 *P)
{
    ECP_BLS12461 W,T;
    BIG_464_28 x;
    FP_BLS12461 cru;
    if (ECP_BLS12461_isinf(P)) return 0;
#if PAIRING_FRIENDLY_BLS12461!=BN_CURVE
    BIG_464_28_rcopy(x, CURVE_Bnx_BLS12461);
    ECP_BLS12461_copy(&W,P);
    ECP_BLS12461_copy(&T,P);
    ECP_BLS12461_mul(&T,x);
    if (ECP_BLS12461_equals(P,&T)) return 0;    // P is of low order   
    ECP_BLS12461_mul(&T,x);
    ECP_BLS12461_neg(&T);

    FP_BLS12461_rcopy(&cru, CRu_BLS12461);
    FP_BLS12461_mul(&(W.x), &(W.x), &cru);
    if (!ECP_BLS12461_equals(&W,&T)) return 0;  // check that Endomorphism works

// Not needed
//    ECP_BLS12461_add(&W,P);
//    FP_BLS12461_mul(&(T.x), &(T.x), &cru);
//    ECP_BLS12461_add(&W,&T);
//    if (!ECP_BLS12461_isinf(&W)) return 0;      // use it to check order

/*	BIG_464_28 q;
	ECP_BLS12461 W;
    if (ECP_BLS12461_isinf(P)) return 0;
#if PAIRING_FRIENDLY_BLS12461!=BN_CURVE
    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
	ECP_BLS12461_copy(&W,P);
	ECP_BLS12461_mul(&W,q);
	if (!ECP_BLS12461_isinf(&W)) return 0; */
#endif 
	return 1;
}

/* test G2 group membership */

int PAIR_BLS12461_G2member(ECP2_BLS12461 *P)
{
    ECP2_BLS12461 W,T;
    BIG_464_28 x;
    FP2_BLS12461 X;
    FP_BLS12461 fx, fy;

    if (ECP2_BLS12461_isinf(P)) return 0;
    FP_BLS12461_rcopy(&fx, Fra_BLS12461);
    FP_BLS12461_rcopy(&fy, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &fx, &fy);
#if SEXTIC_TWIST_BLS12461==M_TYPE
    FP2_BLS12461_inv(&X, &X, NULL);
    FP2_BLS12461_norm(&X);
#endif
    BIG_464_28_rcopy(x, CURVE_Bnx_BLS12461);

    ECP2_BLS12461_copy(&T,P);
    ECP2_BLS12461_mul(&T,x);

#if SIGN_OF_X_BLS12461==NEGATIVEX
    ECP2_BLS12461_neg(&T);
#endif

#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
//https://eprint.iacr.org/2022/348.pdf
    ECP2_BLS12461_copy(&W,&T);
    ECP2_BLS12461_frob(&W,&X); // W=\psi(xP)
    ECP2_BLS12461_add(&T,P); // T=xP+P
    ECP2_BLS12461_add(&T,&W); // T=xP+P+\psi(xP)
    ECP2_BLS12461_frob(&W,&X); // W=\psi^2(xP)
    ECP2_BLS12461_add(&T,&W); // T=xp+P+\psi(xP)+\psi^2(xP)
    ECP2_BLS12461_frob(&W,&X); // W=\psi^3(xP)
    ECP2_BLS12461_dbl(&W); // W=\psi^3(2xP)
#else
//https://eprint.iacr.org/2021/1130
    ECP2_BLS12461_copy(&W,P);
    ECP2_BLS12461_frob(&W, &X);    // W=\psi(P)    
#endif

    if (ECP2_BLS12461_equals(&W,&T)) return 1;
    return 0;
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^4}=m^{p^2} */
int PAIR_BLS12461_GTcyclotomic(FP12_BLS12461 *m)
{
	FP_BLS12461 fx,fy;
	FP2_BLS12461 X;
	FP12_BLS12461 r,w;
	if (FP12_BLS12461_isunity(m)) return 0;
	FP12_BLS12461_conj(&r,m);
	FP12_BLS12461_mul(&r,m);
	if (!FP12_BLS12461_isunity(&r)) return 0;

	FP_BLS12461_rcopy(&fx,Fra_BLS12461);
	FP_BLS12461_rcopy(&fy,Frb_BLS12461);
	FP2_BLS12461_from_FPs(&X,&fx,&fy);

	FP12_BLS12461_copy(&r,m); FP12_BLS12461_frob(&r,&X); FP12_BLS12461_frob(&r,&X);
	FP12_BLS12461_copy(&w,&r); FP12_BLS12461_frob(&w,&X); FP12_BLS12461_frob(&w,&X);
	FP12_BLS12461_mul(&w,m);

	if (!FP12_BLS12461_equals(&w,&r)) return 0;
    return 1;
}

/* test for full GT group membership */
int PAIR_BLS12461_GTmember(FP12_BLS12461 *m)
{
    BIG_464_28 x;
    FP2_BLS12461 X;
    FP_BLS12461 fx, fy;
    FP12_BLS12461 r,t;
    if (!PAIR_BLS12461_GTcyclotomic(m)) return 0;

    FP_BLS12461_rcopy(&fx, Fra_BLS12461);
    FP_BLS12461_rcopy(&fy, Frb_BLS12461);
    FP2_BLS12461_from_FPs(&X, &fx, &fy);
    BIG_464_28_rcopy(x, CURVE_Bnx_BLS12461);

    FP12_BLS12461_copy(&r,m);
    FP12_BLS12461_frob(&r, &X);

    FP12_BLS12461_pow(&t,m,x);

#if PAIRING_FRIENDLY_BLS12461==BN_CURVE
    FP12_BLS12461_pow(&t,&t,x);
    BIG_464_28_zero(x); BIG_464_28_inc(x,6);
    FP12_BLS12461_pow(&t,&t,x);
#else
#if SIGN_OF_X_BLS12461==NEGATIVEX
    FP12_BLS12461_conj(&t,&t);
#endif
#endif

    if (FP12_BLS12461_equals(&r,&t)) return 1;
    return 0;

/*	BIG_464_28 q;
	FP12_BLS12461 r;
    if (!PAIR_BLS12461_GTcyclotomic(m)) return 0;

    BIG_464_28_rcopy(q, CURVE_Order_BLS12461);
    FP12_BLS12461_pow(&r,m,q);
	if (!FP12_BLS12461_isunity(&r)) return 0;
	return 1;
*/
}

#ifdef HAS_MAIN

int main()
{
    int i;
    char byt[32];
    csprng rng;
    BIG_464_28 xa, xb, ya, yb, w, a, b, t1, q, u[2], v[4], m, r;
    ECP2_BLS12461 P, G;
    ECP_BLS12461 Q, R;
    FP12_BLS12461 g, gp;
    FP4_BLS12461 t, c, cp, cpm1, cpm2;
    FP2_BLS12461 x, y, X;


    BIG_464_28_rcopy(a, CURVE_Fra);
    BIG_464_28_rcopy(b, CURVE_Frb);
    FP2_BLS12461_from_BIGs(&X, a, b);

    BIG_464_28_rcopy(xa, CURVE_Gx);
    BIG_464_28_rcopy(ya, CURVE_Gy);

    ECP_BLS12461_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_BLS12461_output(&Q);
    printf("\n");

    BIG_464_28_rcopy(xa, CURVE_Pxa);
    BIG_464_28_rcopy(xb, CURVE_Pxb);
    BIG_464_28_rcopy(ya, CURVE_Pya);
    BIG_464_28_rcopy(yb, CURVE_Pyb);

    FP2_BLS12461_from_BIGs(&x, xa, xb);
    FP2_BLS12461_from_BIGs(&y, ya, yb);

    ECP2_BLS12461_set(&P, &x, &y);
    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    printf("P= ");
    ECP2_BLS12461_output(&P);
    printf("\n");

    for (i = 0; i < 1000; i++ )
    {
        PAIR_BLS12461_ate(&g, &P, &Q);
        PAIR_BLS12461_fexp(&g);
    }
    printf("g= ");
    FP12_BLS12461_output(&g);
    printf("\n");
}

#endif

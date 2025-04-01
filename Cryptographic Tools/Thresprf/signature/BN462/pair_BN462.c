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

#include "pair_BN462.h"

// Point doubling for pairings
static void PAIR_BN462_double(ECP2_BN462 *A, FP2_BN462 *AA, FP2_BN462 *BB, FP2_BN462 *CC)
{
    FP2_BN462 YY;
    FP2_BN462_copy(CC, &(A->x));  //FP2 XX=new FP2(A.getx());  //X
    FP2_BN462_copy(&YY, &(A->y)); //FP2 YY=new FP2(A.gety());  //Y
    FP2_BN462_copy(BB, &(A->z));  //FP2 ZZ=new FP2(A.getz());  //Z

    FP2_BN462_copy(AA, &YY);      //FP2 YZ=new FP2(YY);        //Y
    FP2_BN462_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP2_BN462_sqr(CC, CC);        //XX.sqr();                //X^2
    FP2_BN462_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP2_BN462_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP2_BN462_add(AA, AA, AA);
    FP2_BN462_neg(AA, AA);
    FP2_BN462_norm(AA);           // -2YZ
    FP2_BN462_mul_ip(AA);
    FP2_BN462_norm(AA);           // -2YZi

    FP2_BN462_imul(BB, BB, 3 * CURVE_B_I_BN462); //3Bz^2
    FP2_BN462_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_BN462==D_TYPE
    FP2_BN462_mul_ip(&YY);                    // Y^2.i
    FP2_BN462_norm(&YY);
    FP2_BN462_mul_ip(CC);                 // 3X^2.i
    FP2_BN462_norm(CC);
#endif

#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_mul_ip(BB);                 // 3Bz^2.i
    FP2_BN462_norm(BB);
#endif

    FP2_BN462_sub(BB, BB, &YY);
    FP2_BN462_norm(BB);

    ECP2_BN462_dbl(A);            //A.dbl();
}

// Point addition for pairings
static void PAIR_BN462_add(ECP2_BN462 *A, ECP2_BN462 *B, FP2_BN462 *AA, FP2_BN462 *BB, FP2_BN462 *CC)
{
    FP2_BN462 T1;
    FP2_BN462_copy(AA, &(A->x));      //FP2 X1=new FP2(A.getx());    // X1
    FP2_BN462_copy(CC, &(A->y));      //FP2 Y1=new FP2(A.gety());    // Y1
    FP2_BN462_copy(&T1, &(A->z));     //FP2 T1=new FP2(A.getz());    // Z1

    FP2_BN462_copy(BB, &T1);          //FP2 T2=new FP2(A.getz());    // Z1

    FP2_BN462_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP2_BN462_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP2_BN462_sub(AA, AA, BB);        //X1.sub(T2);
    FP2_BN462_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP2_BN462_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP2_BN462_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP2_BN462_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_mul_ip(AA);
    FP2_BN462_norm(AA);
#endif

    FP2_BN462_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP2_BN462_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP2_BN462_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP2_BN462_sub(BB, BB, &T1);       //T2.sub(T1);
    FP2_BN462_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP2_BN462_neg(CC, CC);            //Y1.neg();
    FP2_BN462_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP2_BN462_add(A, B);             //A.add(B);
}

/* Line function */
static void PAIR_BN462_line(FP12_BN462 *v, ECP2_BN462 *A, ECP2_BN462 *B, FP_BN462 *Qx, FP_BN462 *Qy)
{
    FP2_BN462 AA, BB, CC;
    FP4_BN462 a, b, c;

    if (A == B)
        PAIR_BN462_double(A, &AA, &BB, &CC);
    else
        PAIR_BN462_add(A, B, &AA, &BB, &CC);

    FP2_BN462_pmul(&CC, &CC, Qx);
    FP2_BN462_pmul(&AA, &AA, Qy);

    FP4_BN462_from_FP2s(&a, &AA, &BB);
#if SEXTIC_TWIST_BN462==D_TYPE
    FP4_BN462_from_FP2(&b, &CC);
    FP4_BN462_zero(&c);
#endif
#if SEXTIC_TWIST_BN462==M_TYPE
    FP4_BN462_zero(&b);
    FP4_BN462_from_FP2H(&c, &CC);
#endif

    FP12_BN462_from_FP4s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int PAIR_BN462_nbits(BIG_464_28 n3, BIG_464_28 n)
{
    BIG_464_28 x;
    BIG_464_28_rcopy(x, CURVE_Bnx_BN462);

#if PAIRING_FRIENDLY_BN462==BN_CURVE
    BIG_464_28_pmul(n, x, 6);
#if SIGN_OF_X_BN462==POSITIVEX
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
void PAIR_BN462_initmp(FP12_BN462 r[])
{
    int i;
    for (i = ATE_BITS_BN462 - 1; i >= 0; i--)
        FP12_BN462_one(&r[i]);
    return;
}

// Store precomputed line details in an FP4
static void PAIR_BN462_pack(FP4_BN462 *T, FP2_BN462* AA, FP2_BN462* BB, FP2_BN462 *CC)
{
    FP2_BN462 I, A, B;
    FP2_BN462_inv(&I, CC, NULL);
    FP2_BN462_mul(&A, AA, &I);
    FP2_BN462_mul(&B, BB, &I);
    FP4_BN462_from_FP2s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void PAIR_BN462_unpack(FP12_BN462 *v, FP4_BN462* T, FP_BN462 *Qx, FP_BN462 *Qy)
{
    FP4_BN462 a, b, c;
    FP2_BN462 t;
    FP4_BN462_copy(&a, T);
    FP2_BN462_pmul(&a.a, &a.a, Qy);
    FP2_BN462_from_FP(&t, Qx);

#if SEXTIC_TWIST_BN462==D_TYPE
    FP4_BN462_from_FP2(&b, &t);
    FP4_BN462_zero(&c);
#endif
#if SEXTIC_TWIST_BN462==M_TYPE
    FP4_BN462_zero(&b);
    FP4_BN462_from_FP2H(&c, &t);
#endif

    FP12_BN462_from_FP4s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}


/* basic Miller loop */
void PAIR_BN462_miller(FP12_BN462 *res, FP12_BN462 r[])
{
    int i;
    FP12_BN462_one(res);
    for (i = ATE_BITS_BN462 - 1; i >= 1; i--)
    {
        FP12_BN462_sqr(res, res);
        FP12_BN462_ssmul(res, &r[i]);
        FP12_BN462_zero(&r[i]);
    }

#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(res, res);
#endif
    FP12_BN462_ssmul(res, &r[0]);
    FP12_BN462_zero(&r[0]);
    return;
}


// Precompute table of line functions for fixed G2 value
void PAIR_BN462_precomp(FP4_BN462 T[], ECP2_BN462* GV)
{
    int i, j, nb, bt;
    BIG_464_28 n, n3;
    FP2_BN462 AA, BB, CC;
    ECP2_BN462 A, G, NG;
#if PAIRING_FRIENDLY_BN462==BN_CURVE
    ECP2_BN462 K;
    FP2_BN462 X;
    FP_BN462 Qx, Qy;
    FP_BN462_rcopy(&Qx, Fra_BN462);
    FP_BN462_rcopy(&Qy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &Qx, &Qy);
#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_inv(&X, &X, NULL);
    FP2_BN462_norm(&X);
#endif
#endif

    ECP2_BN462_copy(&A, GV);
    ECP2_BN462_copy(&G, GV);
    ECP2_BN462_copy(&NG, GV);
    ECP2_BN462_neg(&NG);

    nb = PAIR_BN462_nbits(n3, n);
    j = 0;

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BN462_double(&A, &AA, &BB, &CC);
        PAIR_BN462_pack(&T[j++], &AA, &BB, &CC);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BN462_add(&A, &G, &AA, &BB, &CC);
            PAIR_BN462_pack(&T[j++], &AA, &BB, &CC);
        }
        if (bt == -1)
        {
            PAIR_BN462_add(&A, &NG, &AA, &BB, &CC);
            PAIR_BN462_pack(&T[j++], &AA, &BB, &CC);
        }
    }
#if PAIRING_FRIENDLY_BN462==BN_CURVE

#if SIGN_OF_X_BN462==NEGATIVEX
    ECP2_BN462_neg(&A);
#endif

    ECP2_BN462_copy(&K, &G);
    ECP2_BN462_frob(&K, &X);
    PAIR_BN462_add(&A, &K, &AA, &BB, &CC);
    PAIR_BN462_pack(&T[j++], &AA, &BB, &CC);
    ECP2_BN462_frob(&K, &X);
    ECP2_BN462_neg(&K);
    PAIR_BN462_add(&A, &K, &AA, &BB, &CC);
    PAIR_BN462_pack(&T[j++], &AA, &BB, &CC);

#endif
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void PAIR_BN462_another_pc(FP12_BN462 r[], FP4_BN462 T[], ECP_BN462 *QV)
{
    int i, j, nb, bt;
    BIG_464_28 x, n, n3;
    FP12_BN462 lv, lv2;
    ECP_BN462 Q;
    FP_BN462 Qx, Qy;

    if (ECP_BN462_isinf(QV)) return;

    nb = PAIR_BN462_nbits(n3, n);

    ECP_BN462_copy(&Q, QV);
    ECP_BN462_affine(&Q);

    FP_BN462_copy(&Qx, &(Q.x));
    FP_BN462_copy(&Qy, &(Q.y));

    j = 0;
    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BN462_unpack(&lv, &T[j++], &Qx, &Qy);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BN462_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP12_BN462_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BN462_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP12_BN462_smul(&lv, &lv2);
        }
        FP12_BN462_ssmul(&r[i], &lv);
    }

#if PAIRING_FRIENDLY_BN462==BN_CURVE
    PAIR_BN462_unpack(&lv, &T[j++], &Qx, &Qy);
    PAIR_BN462_unpack(&lv2, &T[j++], &Qx, &Qy);
    FP12_BN462_smul(&lv, &lv2);
    FP12_BN462_ssmul(&r[0], &lv);
#endif
}

/* Accumulate another set of line functions for n-pairing */
void PAIR_BN462_another(FP12_BN462 r[], ECP2_BN462* PV, ECP_BN462* QV)
{
    int i, j, nb, bt;
    BIG_464_28 x, n, n3;
    FP12_BN462 lv, lv2;
    ECP2_BN462 A, NP, P;
    ECP_BN462 Q;
    FP_BN462 Qx, Qy;
#if PAIRING_FRIENDLY_BN462==BN_CURVE
    ECP2_BN462 K;
    FP2_BN462 X;
    FP_BN462_rcopy(&Qx, Fra_BN462);
    FP_BN462_rcopy(&Qy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &Qx, &Qy);
#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_inv(&X, &X, NULL);
    FP2_BN462_norm(&X);
#endif
#endif

    if (ECP_BN462_isinf(QV)) return;

    nb = PAIR_BN462_nbits(n3, n);

    ECP2_BN462_copy(&P, PV);
    ECP_BN462_copy(&Q, QV);

    ECP2_BN462_affine(&P);
    ECP_BN462_affine(&Q);

    FP_BN462_copy(&Qx, &(Q.x));
    FP_BN462_copy(&Qy, &(Q.y));

    ECP2_BN462_copy(&A, &P);
    ECP2_BN462_copy(&NP, &P); ECP2_BN462_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BN462_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BN462_line(&lv2, &A, &P, &Qx, &Qy);
            FP12_BN462_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BN462_line(&lv2, &A, &NP, &Qx, &Qy);
            FP12_BN462_smul(&lv, &lv2);
        }
        FP12_BN462_ssmul(&r[i], &lv);
    }

#if PAIRING_FRIENDLY_BN462==BN_CURVE

#if SIGN_OF_X_BN462==NEGATIVEX
    ECP2_BN462_neg(&A);
#endif

    ECP2_BN462_copy(&K, &P);
    ECP2_BN462_frob(&K, &X);
    PAIR_BN462_line(&lv, &A, &K, &Qx, &Qy);
    ECP2_BN462_frob(&K, &X);
    ECP2_BN462_neg(&K);
    PAIR_BN462_line(&lv2, &A, &K, &Qx, &Qy);
    FP12_BN462_smul(&lv, &lv2);
    FP12_BN462_ssmul(&r[0], &lv);

#endif
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_BN462_ate(FP12_BN462 *r, ECP2_BN462 *P1, ECP_BN462 *Q1)
{

    BIG_464_28 x, n, n3;
    FP_BN462 Qx, Qy;
    int i, nb, bt;
    ECP2_BN462 A, NP, P;
    ECP_BN462 Q;
    FP12_BN462 lv, lv2;
#if PAIRING_FRIENDLY_BN462==BN_CURVE
    ECP2_BN462 KA;
    FP2_BN462 X;

    FP_BN462_rcopy(&Qx, Fra_BN462);
    FP_BN462_rcopy(&Qy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &Qx, &Qy);

#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_inv(&X, &X, NULL);
    FP2_BN462_norm(&X);
#endif
#endif

    FP12_BN462_one(r);
    if (ECP_BN462_isinf(Q1)) return;

    nb = PAIR_BN462_nbits(n3, n);

    ECP2_BN462_copy(&P, P1);
    ECP_BN462_copy(&Q, Q1);

    ECP2_BN462_affine(&P);
    ECP_BN462_affine(&Q);

    FP_BN462_copy(&Qx, &(Q.x));
    FP_BN462_copy(&Qy, &(Q.y));

    ECP2_BN462_copy(&A, &P);
    ECP2_BN462_copy(&NP, &P); ECP2_BN462_neg(&NP);



    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--) //0
    {
        FP12_BN462_sqr(r, r);
        PAIR_BN462_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BN462_line(&lv2, &A, &P, &Qx, &Qy);
            FP12_BN462_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BN462_line(&lv2, &A, &NP, &Qx, &Qy);
            FP12_BN462_smul(&lv, &lv2);
        }
        FP12_BN462_ssmul(r, &lv);

    }


#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(r, r);
#endif

    /* R-ate fixup required for BN curves */
#if PAIRING_FRIENDLY_BN462==BN_CURVE

#if SIGN_OF_X_BN462==NEGATIVEX
    ECP2_BN462_neg(&A);
#endif

    ECP2_BN462_copy(&KA, &P);
    ECP2_BN462_frob(&KA, &X);
    PAIR_BN462_line(&lv, &A, &KA, &Qx, &Qy);
    ECP2_BN462_frob(&KA, &X);
    ECP2_BN462_neg(&KA);
    PAIR_BN462_line(&lv2, &A, &KA, &Qx, &Qy);
    FP12_BN462_smul(&lv, &lv2);
    FP12_BN462_ssmul(r, &lv);
#endif
}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_BN462_double_ate(FP12_BN462 *r, ECP2_BN462 *P1, ECP_BN462 *Q1, ECP2_BN462 *R1, ECP_BN462 *S1)
{
    BIG_464_28 x, n, n3;
    FP_BN462 Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP2_BN462 A, B, NP, NR, P, R;
    ECP_BN462 Q, S;
    FP12_BN462 lv, lv2;
#if PAIRING_FRIENDLY_BN462==BN_CURVE
    FP2_BN462 X;
    ECP2_BN462 K;

    FP_BN462_rcopy(&Qx, Fra_BN462);
    FP_BN462_rcopy(&Qy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &Qx, &Qy);

#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_inv(&X, &X, NULL);
    FP2_BN462_norm(&X);
#endif
#endif

    if (ECP_BN462_isinf(Q1))
    {
        PAIR_BN462_ate(r, R1, S1);
        return;
    }
    if (ECP_BN462_isinf(S1))
    {
        PAIR_BN462_ate(r, P1, Q1);
        return;
    }
    nb = PAIR_BN462_nbits(n3, n);

    ECP2_BN462_copy(&P, P1);
    ECP_BN462_copy(&Q, Q1);

    ECP2_BN462_affine(&P);
    ECP_BN462_affine(&Q);

    ECP2_BN462_copy(&R, R1);
    ECP_BN462_copy(&S, S1);

    ECP2_BN462_affine(&R);
    ECP_BN462_affine(&S);

    FP_BN462_copy(&Qx, &(Q.x));
    FP_BN462_copy(&Qy, &(Q.y));

    FP_BN462_copy(&Sx, &(S.x));
    FP_BN462_copy(&Sy, &(S.y));

    ECP2_BN462_copy(&A, &P);
    ECP2_BN462_copy(&B, &R);

    ECP2_BN462_copy(&NP, &P); ECP2_BN462_neg(&NP);
    ECP2_BN462_copy(&NR, &R); ECP2_BN462_neg(&NR);

    FP12_BN462_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP12_BN462_sqr(r, r);
        PAIR_BN462_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_BN462_line(&lv2, &B, &B, &Sx, &Sy);
        FP12_BN462_smul(&lv, &lv2);
        FP12_BN462_ssmul(r, &lv);

        bt = BIG_464_28_bit(n3, i) - BIG_464_28_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BN462_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_BN462_line(&lv2, &B, &R, &Sx, &Sy);
            FP12_BN462_smul(&lv, &lv2);
            FP12_BN462_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_BN462_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_BN462_line(&lv2, &B, &NR, &Sx, &Sy);
            FP12_BN462_smul(&lv, &lv2);
            FP12_BN462_ssmul(r, &lv);
        }

    }


    /* R-ate fixup required for BN curves */

#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(r, r);
#endif

#if PAIRING_FRIENDLY_BN462==BN_CURVE

#if SIGN_OF_X_BN462==NEGATIVEX
    ECP2_BN462_neg(&A);
    ECP2_BN462_neg(&B);
#endif

    ECP2_BN462_copy(&K, &P);
    ECP2_BN462_frob(&K, &X);
    PAIR_BN462_line(&lv, &A, &K, &Qx, &Qy);
    ECP2_BN462_frob(&K, &X);
    ECP2_BN462_neg(&K);
    PAIR_BN462_line(&lv2, &A, &K, &Qx, &Qy);
    FP12_BN462_smul(&lv, &lv2);
    FP12_BN462_ssmul(r, &lv);

    ECP2_BN462_copy(&K, &R);
    ECP2_BN462_frob(&K, &X);
    PAIR_BN462_line(&lv, &B, &K, &Sx, &Sy);
    ECP2_BN462_frob(&K, &X);
    ECP2_BN462_neg(&K);
    PAIR_BN462_line(&lv2, &B, &K, &Sx, &Sy);
    FP12_BN462_smul(&lv, &lv2);
    FP12_BN462_ssmul(r, &lv);
#endif
}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */
void PAIR_BN462_fexp(FP12_BN462 *r)
{
    FP2_BN462 X;
    BIG_464_28 x;
    FP_BN462 a, b;
    FP12_BN462 t0, y0, y1;
#if PAIRING_FRIENDLY_BN462==BN_CURVE
    FP12_BN462 y2, y3;
#endif

    BIG_464_28_rcopy(x, CURVE_Bnx_BN462);
    FP_BN462_rcopy(&a, Fra_BN462);
    FP_BN462_rcopy(&b, Frb_BN462);
    FP2_BN462_from_FPs(&X, &a, &b);

    /* Easy part of final exp */

    FP12_BN462_inv(&t0, r);
    FP12_BN462_conj(r, r);

    FP12_BN462_mul(r, &t0);
    FP12_BN462_copy(&t0, r);

    FP12_BN462_frob(r, &X);
    FP12_BN462_frob(r, &X);
    FP12_BN462_mul(r, &t0);

    /* Hard part of final exp - see Duquesne & Ghamman eprint 2015/192.pdf */
#if PAIRING_FRIENDLY_BN462==BN_CURVE
    FP12_BN462_pow(&t0, r, x); // t0=f^-u
#if SIGN_OF_X_BN462==POSITIVEX
    FP12_BN462_conj(&t0, &t0);
#endif
    FP12_BN462_usqr(&y3, &t0); // y3=t0^2
    FP12_BN462_copy(&y0, &t0);
    FP12_BN462_mul(&y0, &y3); // y0=t0*y3
    FP12_BN462_copy(&y2, &y3);
    FP12_BN462_frob(&y2, &X); // y2=y3^p
    FP12_BN462_mul(&y2, &y3); //y2=y2*y3
    FP12_BN462_usqr(&y2, &y2); //y2=y2^2
    FP12_BN462_mul(&y2, &y3); // y2=y2*y3

    FP12_BN462_pow(&t0, &y0, x); //t0=y0^-u
#if SIGN_OF_X_BN462==POSITIVEX
    FP12_BN462_conj(&t0, &t0);
#endif
    FP12_BN462_conj(&y0, r);    //y0=~r
    FP12_BN462_copy(&y1, &t0);
    FP12_BN462_frob(&y1, &X);
    FP12_BN462_frob(&y1, &X); //y1=t0^p^2
    FP12_BN462_mul(&y1, &y0); // y1=y0*y1
    FP12_BN462_conj(&t0, &t0); // t0=~t0
    FP12_BN462_copy(&y3, &t0);
    FP12_BN462_frob(&y3, &X); //y3=t0^p
    FP12_BN462_mul(&y3, &t0); // y3=t0*y3
    FP12_BN462_usqr(&t0, &t0); // t0=t0^2
    FP12_BN462_mul(&y1, &t0); // y1=t0*y1

    FP12_BN462_pow(&t0, &y3, x); // t0=y3^-u
#if SIGN_OF_X_BN462==POSITIVEX
    FP12_BN462_conj(&t0, &t0);
#endif
    FP12_BN462_usqr(&t0, &t0); //t0=t0^2
    FP12_BN462_conj(&t0, &t0); //t0=~t0
    FP12_BN462_mul(&y3, &t0); // y3=t0*y3

    FP12_BN462_frob(r, &X);
    FP12_BN462_copy(&y0, r);
    FP12_BN462_frob(r, &X);
    FP12_BN462_mul(&y0, r);
    FP12_BN462_frob(r, &X);
    FP12_BN462_mul(&y0, r);

    FP12_BN462_usqr(r, &y3); //r=y3^2
    FP12_BN462_mul(r, &y2);  //r=y2*r
    FP12_BN462_copy(&y3, r);
    FP12_BN462_mul(&y3, &y0); // y3=r*y0
    FP12_BN462_mul(r, &y1); // r=r*y1
    FP12_BN462_usqr(r, r); // r=r^2
    FP12_BN462_mul(r, &y3); // r=r*y3
    FP12_BN462_reduce(r);
#else

// See https://eprint.iacr.org/2020/875.pdf
    FP12_BN462_usqr(&y1,r);
    FP12_BN462_mul(&y1,r);     // y1=r^3

    FP12_BN462_pow(&y0,r,x);   // y0=r^x
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&y0, &y0);
#endif
    FP12_BN462_conj(&t0,r);    // t0=r^-1
    FP12_BN462_copy(r,&y0);
    FP12_BN462_mul(r,&t0);    // r=r^(x-1)

    FP12_BN462_pow(&y0,r,x);   // y0=r^x
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&y0, &y0);
#endif
    FP12_BN462_conj(&t0,r);    // t0=r^-1
    FP12_BN462_copy(r,&y0);
    FP12_BN462_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP12_BN462_pow(&y0,r,x);  // y0=r^x
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&y0, &y0);
#endif
    FP12_BN462_copy(&t0,r);   
    FP12_BN462_frob(&t0,&X); // t0=r^p
    FP12_BN462_copy(r,&y0);
    FP12_BN462_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2-1)
    FP12_BN462_pow(&y0,r,x);  
    FP12_BN462_pow(&y0,&y0,x); // y0=r^x^2
    FP12_BN462_copy(&t0,r);    
    FP12_BN462_frob(&t0,&X);
    FP12_BN462_frob(&t0,&X);   // t0=r^p^2
    FP12_BN462_mul(&y0,&t0);   // y0=r^x^2.r^p^2
    FP12_BN462_conj(&t0,r);    // t0=r^-1
    FP12_BN462_copy(r,&y0);    // 
    FP12_BN462_mul(r,&t0);     // r=r^x^2.r^p^2.r^-1

    FP12_BN462_mul(r,&y1);    
    FP12_BN462_reduce(r);

// Ghamman & Fouotsa Method
/*
    FP12_BN462_usqr(&y0, r);
    FP12_BN462_pow(&y1, &y0, x);
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&y1, &y1);
#endif

    BIG_464_28_fshr(x, 1);
    FP12_BN462_pow(&y2, &y1, x);
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&y2, &y2);
#endif

    BIG_464_28_fshl(x, 1); // x must be even
    FP12_BN462_conj(&y3, r);
    FP12_BN462_mul(&y1, &y3);

    FP12_BN462_conj(&y1, &y1);
    FP12_BN462_mul(&y1, &y2);

    FP12_BN462_pow(&y2, &y1, x);
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&y2, &y2);
#endif

    FP12_BN462_pow(&y3, &y2, x);
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&y3, &y3);
#endif
    FP12_BN462_conj(&y1, &y1);
    FP12_BN462_mul(&y3, &y1);

    FP12_BN462_conj(&y1, &y1);
    FP12_BN462_frob(&y1, &X);
    FP12_BN462_frob(&y1, &X);
    FP12_BN462_frob(&y1, &X);
    FP12_BN462_frob(&y2, &X);
    FP12_BN462_frob(&y2, &X);
    FP12_BN462_mul(&y1, &y2);

    FP12_BN462_pow(&y2, &y3, x);
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&y2, &y2);
#endif
    FP12_BN462_mul(&y2, &y0);
    FP12_BN462_mul(&y2, r);

    FP12_BN462_mul(&y1, &y2);
    FP12_BN462_copy(&y2, &y3);
    FP12_BN462_frob(&y2, &X);
    FP12_BN462_mul(&y1, &y2);
    FP12_BN462_copy(r, &y1);
    FP12_BN462_reduce(r);
*/
#endif
}

#ifdef USE_GLV_BN462
/* GLV method */
static void glv(BIG_464_28 u[2], BIG_464_28 ee)
{
    BIG_464_28 q;
#if PAIRING_FRIENDLY_BN462==BN_CURVE
    int i, j;
    BIG_464_28 v[2], t;
    DBIG_464_28 d;
    BIG_464_28_rcopy(q, CURVE_Order_BN462);

    for (i = 0; i < 2; i++)
    {
        BIG_464_28_rcopy(t, CURVE_W_BN462[i]);
        BIG_464_28_mul(d, t, ee);
        BIG_464_28_ctddiv(v[i],d,q,BIG_464_28_nbits(t));
        BIG_464_28_zero(u[i]);
    }
    BIG_464_28_copy(u[0], ee);
    for (i = 0; i < 2; i++)
        for (j = 0; j < 2; j++)
        {
            BIG_464_28_rcopy(t, CURVE_SB_BN462[j][i]);
            BIG_464_28_modmul(t, v[j], t, q);
            BIG_464_28_add(u[i], u[i], q);
            BIG_464_28_sub(u[i], u[i], t);
            BIG_464_28_ctmod(u[i],q,1);
        }

#else
// -(x^2).P = (Beta.x,y)
    int bd;
    BIG_464_28 x, x2;
    BIG_464_28_rcopy(q, CURVE_Order_BN462);
    BIG_464_28_rcopy(x, CURVE_Bnx_BN462);

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
#if PAIRING_FRIENDLY_BN462==BN_CURVE
    int j;
    BIG_464_28 v[4], t;
    DBIG_464_28 d;
    BIG_464_28_rcopy(q, CURVE_Order_BN462);

    for (i = 0; i < 4; i++)
    {
        BIG_464_28_rcopy(t, CURVE_WB_BN462[i]);
        BIG_464_28_mul(d, t, ee);
        BIG_464_28_ctddiv(v[i],d,q,BIG_464_28_nbits(t));
        BIG_464_28_zero(u[i]);
    }

    BIG_464_28_copy(u[0], ee);
    for (i = 0; i < 4; i++)
        for (j = 0; j < 4; j++)
        {
            BIG_464_28_rcopy(t, CURVE_BB_BN462[j][i]);
            BIG_464_28_modmul(t, v[j], t, q);
            BIG_464_28_add(u[i], u[i], q);
            BIG_464_28_sub(u[i], u[i], t);
            BIG_464_28_ctmod(u[i],q,1);
        }
#else
    int bd;
    BIG_464_28 x, w;
    BIG_464_28_rcopy(q, CURVE_Order_BN462);

    BIG_464_28_rcopy(x, CURVE_Bnx_BN462);
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
#if SIGN_OF_X_BN462==NEGATIVEX
    BIG_464_28_modneg(u[1], u[1], q);
    BIG_464_28_modneg(u[3], u[3], q);
#endif

#endif
    return;
}

/* Multiply P by e in group G1 */
void PAIR_BN462_G1mul(ECP_BN462 *P, BIG_464_28 e)
{
    BIG_464_28 ee,q;
#ifdef USE_GLV_BN462   /* Note this method is patented */
    int np, nn;
    ECP_BN462 Q;
    FP_BN462 cru;
    BIG_464_28 t;
    BIG_464_28 u[2];

    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BN462);
    BIG_464_28_mod(ee,q);

    glv(u, ee);

    ECP_BN462_copy(&Q, P); ECP_BN462_affine(&Q);
    FP_BN462_rcopy(&cru, CRu_BN462);
    FP_BN462_mul(&(Q.x), &(Q.x), &cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np = BIG_464_28_nbits(u[0]);
    BIG_464_28_modneg(t, u[0], q);
    nn = BIG_464_28_nbits(t);
    if (nn < np)
    {
        BIG_464_28_copy(u[0], t);
        ECP_BN462_neg(P);
    }

    np = BIG_464_28_nbits(u[1]);
    BIG_464_28_modneg(t, u[1], q);
    nn = BIG_464_28_nbits(t);
    if (nn < np)
    {
        BIG_464_28_copy(u[1], t);
        ECP_BN462_neg(&Q);
    }
    BIG_464_28_norm(u[0]);
    BIG_464_28_norm(u[1]);
    ECP_BN462_mul2(P, &Q, u[0], u[1]);

#else
    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BN462);
    BIG_464_28_mod(ee,q);
    ECP_BN462_clmul(P, ee, q);
#endif
}

/* Multiply P by e in group G2 */
void PAIR_BN462_G2mul(ECP2_BN462 *P, BIG_464_28 e)
{
    BIG_464_28 ee,q;
#ifdef USE_GS_G2_BN462   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP2_BN462 Q[4];
    FP2_BN462 X;
    FP_BN462 fx, fy;
    BIG_464_28 x, u[4];

    FP_BN462_rcopy(&fx, Fra_BN462);
    FP_BN462_rcopy(&fy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &fx, &fy);

#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_inv(&X, &X, NULL);
    FP2_BN462_norm(&X);
#endif

    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BN462);
    BIG_464_28_mod(ee,q);

    gs(u, ee);

    ECP2_BN462_copy(&Q[0], P);
    for (i = 1; i < 4; i++)
    {
        ECP2_BN462_copy(&Q[i], &Q[i - 1]);
        ECP2_BN462_frob(&Q[i], &X);
    }

    for (i = 0; i < 4; i++)
    {
        np = BIG_464_28_nbits(u[i]);
        BIG_464_28_modneg(x, u[i], q);
        nn = BIG_464_28_nbits(x);
        if (nn < np)
        {
            BIG_464_28_copy(u[i], x);
            ECP2_BN462_neg(&Q[i]);
        }
        BIG_464_28_norm(u[i]);
    }

    ECP2_BN462_mul4(P, Q, u);

#else
    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BN462);
    BIG_464_28_mod(ee,q);
    ECP2_BN462_mul(P, ee);
#endif
}

/* f=f^e */
void PAIR_BN462_GTpow(FP12_BN462 *f, BIG_464_28 e)
{
    BIG_464_28 ee,q;
#ifdef USE_GS_GT_BN462   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see fp4.c */
    int i, np, nn;
    FP12_BN462 g[4];
    FP2_BN462 X;
    BIG_464_28 t;
    FP_BN462 fx, fy;
    BIG_464_28 u[4];

    FP_BN462_rcopy(&fx, Fra_BN462);
    FP_BN462_rcopy(&fy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &fx, &fy);

    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BN462);
    BIG_464_28_mod(ee,q);

    gs(u, ee);

    FP12_BN462_copy(&g[0], f);
    for (i = 1; i < 4; i++)
    {
        FP12_BN462_copy(&g[i], &g[i - 1]);
        FP12_BN462_frob(&g[i], &X);
    }

    for (i = 0; i < 4; i++)
    {
        np = BIG_464_28_nbits(u[i]);
        BIG_464_28_modneg(t, u[i], q);
        nn = BIG_464_28_nbits(t);
        if (nn < np)
        {
            BIG_464_28_copy(u[i], t);
            FP12_BN462_conj(&g[i], &g[i]);
        }
        BIG_464_28_norm(u[i]);
    }
    FP12_BN462_pow4(f, g, u);

#else
    BIG_464_28_copy(ee,e);
    BIG_464_28_rcopy(q, CURVE_Order_BN462);
    BIG_464_28_mod(ee,q);
    FP12_BN462_pow(f, f, ee);
#endif
}


/* test G1 group membership */

int PAIR_BN462_G1member(ECP_BN462 *P)
{
    ECP_BN462 W,T;
    BIG_464_28 x;
    FP_BN462 cru;
    if (ECP_BN462_isinf(P)) return 0;
#if PAIRING_FRIENDLY_BN462!=BN_CURVE
    BIG_464_28_rcopy(x, CURVE_Bnx_BN462);
    ECP_BN462_copy(&W,P);
    ECP_BN462_copy(&T,P);
    ECP_BN462_mul(&T,x);
    if (ECP_BN462_equals(P,&T)) return 0;    // P is of low order   
    ECP_BN462_mul(&T,x);
    ECP_BN462_neg(&T);

    FP_BN462_rcopy(&cru, CRu_BN462);
    FP_BN462_mul(&(W.x), &(W.x), &cru);
    if (!ECP_BN462_equals(&W,&T)) return 0;  // check that Endomorphism works

// Not needed
//    ECP_BN462_add(&W,P);
//    FP_BN462_mul(&(T.x), &(T.x), &cru);
//    ECP_BN462_add(&W,&T);
//    if (!ECP_BN462_isinf(&W)) return 0;      // use it to check order

/*	BIG_464_28 q;
	ECP_BN462 W;
    if (ECP_BN462_isinf(P)) return 0;
#if PAIRING_FRIENDLY_BN462!=BN_CURVE
    BIG_464_28_rcopy(q, CURVE_Order_BN462);
	ECP_BN462_copy(&W,P);
	ECP_BN462_mul(&W,q);
	if (!ECP_BN462_isinf(&W)) return 0; */
#endif 
	return 1;
}

/* test G2 group membership */

int PAIR_BN462_G2member(ECP2_BN462 *P)
{
    ECP2_BN462 W,T;
    BIG_464_28 x;
    FP2_BN462 X;
    FP_BN462 fx, fy;

    if (ECP2_BN462_isinf(P)) return 0;
    FP_BN462_rcopy(&fx, Fra_BN462);
    FP_BN462_rcopy(&fy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &fx, &fy);
#if SEXTIC_TWIST_BN462==M_TYPE
    FP2_BN462_inv(&X, &X, NULL);
    FP2_BN462_norm(&X);
#endif
    BIG_464_28_rcopy(x, CURVE_Bnx_BN462);

    ECP2_BN462_copy(&T,P);
    ECP2_BN462_mul(&T,x);

#if SIGN_OF_X_BN462==NEGATIVEX
    ECP2_BN462_neg(&T);
#endif

#if PAIRING_FRIENDLY_BN462==BN_CURVE
//https://eprint.iacr.org/2022/348.pdf
    ECP2_BN462_copy(&W,&T);
    ECP2_BN462_frob(&W,&X); // W=\psi(xP)
    ECP2_BN462_add(&T,P); // T=xP+P
    ECP2_BN462_add(&T,&W); // T=xP+P+\psi(xP)
    ECP2_BN462_frob(&W,&X); // W=\psi^2(xP)
    ECP2_BN462_add(&T,&W); // T=xp+P+\psi(xP)+\psi^2(xP)
    ECP2_BN462_frob(&W,&X); // W=\psi^3(xP)
    ECP2_BN462_dbl(&W); // W=\psi^3(2xP)
#else
//https://eprint.iacr.org/2021/1130
    ECP2_BN462_copy(&W,P);
    ECP2_BN462_frob(&W, &X);    // W=\psi(P)    
#endif

    if (ECP2_BN462_equals(&W,&T)) return 1;
    return 0;
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^4}=m^{p^2} */
int PAIR_BN462_GTcyclotomic(FP12_BN462 *m)
{
	FP_BN462 fx,fy;
	FP2_BN462 X;
	FP12_BN462 r,w;
	if (FP12_BN462_isunity(m)) return 0;
	FP12_BN462_conj(&r,m);
	FP12_BN462_mul(&r,m);
	if (!FP12_BN462_isunity(&r)) return 0;

	FP_BN462_rcopy(&fx,Fra_BN462);
	FP_BN462_rcopy(&fy,Frb_BN462);
	FP2_BN462_from_FPs(&X,&fx,&fy);

	FP12_BN462_copy(&r,m); FP12_BN462_frob(&r,&X); FP12_BN462_frob(&r,&X);
	FP12_BN462_copy(&w,&r); FP12_BN462_frob(&w,&X); FP12_BN462_frob(&w,&X);
	FP12_BN462_mul(&w,m);

	if (!FP12_BN462_equals(&w,&r)) return 0;
    return 1;
}

/* test for full GT group membership */
int PAIR_BN462_GTmember(FP12_BN462 *m)
{
    BIG_464_28 x;
    FP2_BN462 X;
    FP_BN462 fx, fy;
    FP12_BN462 r,t;
    if (!PAIR_BN462_GTcyclotomic(m)) return 0;

    FP_BN462_rcopy(&fx, Fra_BN462);
    FP_BN462_rcopy(&fy, Frb_BN462);
    FP2_BN462_from_FPs(&X, &fx, &fy);
    BIG_464_28_rcopy(x, CURVE_Bnx_BN462);

    FP12_BN462_copy(&r,m);
    FP12_BN462_frob(&r, &X);

    FP12_BN462_pow(&t,m,x);

#if PAIRING_FRIENDLY_BN462==BN_CURVE
    FP12_BN462_pow(&t,&t,x);
    BIG_464_28_zero(x); BIG_464_28_inc(x,6);
    FP12_BN462_pow(&t,&t,x);
#else
#if SIGN_OF_X_BN462==NEGATIVEX
    FP12_BN462_conj(&t,&t);
#endif
#endif

    if (FP12_BN462_equals(&r,&t)) return 1;
    return 0;

/*	BIG_464_28 q;
	FP12_BN462 r;
    if (!PAIR_BN462_GTcyclotomic(m)) return 0;

    BIG_464_28_rcopy(q, CURVE_Order_BN462);
    FP12_BN462_pow(&r,m,q);
	if (!FP12_BN462_isunity(&r)) return 0;
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
    ECP2_BN462 P, G;
    ECP_BN462 Q, R;
    FP12_BN462 g, gp;
    FP4_BN462 t, c, cp, cpm1, cpm2;
    FP2_BN462 x, y, X;


    BIG_464_28_rcopy(a, CURVE_Fra);
    BIG_464_28_rcopy(b, CURVE_Frb);
    FP2_BN462_from_BIGs(&X, a, b);

    BIG_464_28_rcopy(xa, CURVE_Gx);
    BIG_464_28_rcopy(ya, CURVE_Gy);

    ECP_BN462_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_BN462_output(&Q);
    printf("\n");

    BIG_464_28_rcopy(xa, CURVE_Pxa);
    BIG_464_28_rcopy(xb, CURVE_Pxb);
    BIG_464_28_rcopy(ya, CURVE_Pya);
    BIG_464_28_rcopy(yb, CURVE_Pyb);

    FP2_BN462_from_BIGs(&x, xa, xb);
    FP2_BN462_from_BIGs(&y, ya, yb);

    ECP2_BN462_set(&P, &x, &y);
    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    printf("P= ");
    ECP2_BN462_output(&P);
    printf("\n");

    for (i = 0; i < 1000; i++ )
    {
        PAIR_BN462_ate(&g, &P, &Q);
        PAIR_BN462_fexp(&g);
    }
    printf("g= ");
    FP12_BN462_output(&g);
    printf("\n");
}

#endif

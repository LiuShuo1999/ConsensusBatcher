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

#include "pair4_BLS24479.h"

// Point doubling for pairings
static void PAIR_BLS24479_double(ECP4_BLS24479 *A, FP4_BLS24479 *AA, FP4_BLS24479 *BB, FP4_BLS24479 *CC)
{
    FP4_BLS24479 YY;
    FP4_BLS24479_copy(CC, &(A->x));  //FP4 XX=new FP4(A.getx());  //X
    FP4_BLS24479_copy(&YY, &(A->y)); //FP4 YY=new FP4(A.gety());  //Y
    FP4_BLS24479_copy(BB, &(A->z));  //FP4 ZZ=new FP4(A.getz());  //Z

    FP4_BLS24479_copy(AA, &YY);      //FP4 YZ=new FP4(YY);        //Y
    FP4_BLS24479_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP4_BLS24479_sqr(CC, CC);        //XX.sqr();                //X^2
    FP4_BLS24479_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP4_BLS24479_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP4_BLS24479_add(AA, AA, AA);
    FP4_BLS24479_neg(AA, AA);
    FP4_BLS24479_norm(AA);           // -2YZ
    FP4_BLS24479_times_i(AA);        // -2YZi

    FP4_BLS24479_imul(BB, BB, 3 * CURVE_B_I_BLS24479); //3Bz^2
    FP4_BLS24479_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_BLS24479==D_TYPE
    FP4_BLS24479_times_i(&YY);                   // Y^2.i
    FP4_BLS24479_times_i(CC);                    // 3X^2.i
#endif

#if SEXTIC_TWIST_BLS24479==M_TYPE
    FP4_BLS24479_times_i(BB);                    // 3Bz^2.i
#endif

    FP4_BLS24479_sub(BB, BB, &YY);
    FP4_BLS24479_norm(BB);

    ECP4_BLS24479_dbl(A);            //A.dbl();
}

// Point addition for pairings
static void PAIR_BLS24479_add(ECP4_BLS24479 *A, ECP4_BLS24479 *B, FP4_BLS24479 *AA, FP4_BLS24479 *BB, FP4_BLS24479 *CC)
{
    FP4_BLS24479 T1;
    FP4_BLS24479_copy(AA, &(A->x));      //FP4 X1=new FP4(A.getx());    // X1
    FP4_BLS24479_copy(CC, &(A->y));      //FP4 Y1=new FP4(A.gety());    // Y1
    FP4_BLS24479_copy(&T1, &(A->z));     //FP4 T1=new FP4(A.getz());    // Z1

    FP4_BLS24479_copy(BB, &T1);          //FP4 T2=new FP4(A.getz());    // Z1

    FP4_BLS24479_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP4_BLS24479_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP4_BLS24479_sub(AA, AA, BB);        //X1.sub(T2);
    FP4_BLS24479_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP4_BLS24479_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP4_BLS24479_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP4_BLS24479_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_BLS24479==M_TYPE
    FP4_BLS24479_times_i(AA);
    FP4_BLS24479_norm(AA);
#endif

    FP4_BLS24479_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP4_BLS24479_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP4_BLS24479_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP4_BLS24479_sub(BB, BB, &T1);       //T2.sub(T1);
    FP4_BLS24479_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP4_BLS24479_neg(CC, CC);            //Y1.neg();
    FP4_BLS24479_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP4_BLS24479_add(A, B);             //A.add(B);
}

/* Line function */
static void PAIR_BLS24479_line(FP24_BLS24479 *v, ECP4_BLS24479 *A, ECP4_BLS24479 *B, FP_BLS24479 *Qx, FP_BLS24479 *Qy)
{
    FP4_BLS24479 AA, BB, CC;
    FP8_BLS24479 a, b, c;

    if (A == B)
        PAIR_BLS24479_double(A, &AA, &BB, &CC);
    else
        PAIR_BLS24479_add(A, B, &AA, &BB, &CC);

    FP4_BLS24479_qmul(&CC, &CC, Qx);
    FP4_BLS24479_qmul(&AA, &AA, Qy);

    FP8_BLS24479_from_FP4s(&a, &AA, &BB);
#if SEXTIC_TWIST_BLS24479==D_TYPE
    FP8_BLS24479_from_FP4(&b, &CC);
    FP8_BLS24479_zero(&c);
#endif
#if SEXTIC_TWIST_BLS24479==M_TYPE
    FP8_BLS24479_zero(&b);
    FP8_BLS24479_from_FP4H(&c, &CC);
#endif

    FP24_BLS24479_from_FP8s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}


/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int PAIR_BLS24479_nbits(BIG_480_29 n3, BIG_480_29 n)
{
    BIG_480_29 x;
    BIG_480_29_rcopy(x, CURVE_Bnx_BLS24479);

    BIG_480_29_copy(n, x);
    BIG_480_29_norm(n);
    BIG_480_29_pmul(n3, n, 3);
    BIG_480_29_norm(n3);

    return BIG_480_29_nbits(n3);
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
void PAIR_BLS24479_initmp(FP24_BLS24479 r[])
{
    int i;
    for (i = ATE_BITS_BLS24479 - 1; i >= 0; i--)
        FP24_BLS24479_one(&r[i]);
    return;
}

/* basic Miller loop */
void PAIR_BLS24479_miller(FP24_BLS24479 *res, FP24_BLS24479 r[])
{
    int i;
    FP24_BLS24479_one(res);
    for (i = ATE_BITS_BLS24479 - 1; i >= 1; i--)
    {
        FP24_BLS24479_sqr(res, res);
        FP24_BLS24479_ssmul(res, &r[i]);
        FP24_BLS24479_zero(&r[i]);
    }

#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(res, res);
#endif
    FP24_BLS24479_ssmul(res, &r[0]);
    FP24_BLS24479_zero(&r[0]);
    return;
}

// Store precomputed line details in an FP4
static void PAIR_BLS24479_pack(FP8_BLS24479 *T, FP4_BLS24479* AA, FP4_BLS24479* BB, FP4_BLS24479 *CC)
{
    FP4_BLS24479 I, A, B;
    FP4_BLS24479_inv(&I, CC, NULL);
    FP4_BLS24479_mul(&A, AA, &I);
    FP4_BLS24479_mul(&B, BB, &I);
    FP8_BLS24479_from_FP4s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void PAIR_BLS24479_unpack(FP24_BLS24479 *v, FP8_BLS24479* T, FP_BLS24479 *Qx, FP_BLS24479 *Qy)
{
    FP8_BLS24479 a, b, c;
    FP4_BLS24479 t;
    FP8_BLS24479_copy(&a, T);
    FP4_BLS24479_qmul(&a.a, &a.a, Qy);
    FP4_BLS24479_from_FP(&t, Qx);

#if SEXTIC_TWIST_BLS24479==D_TYPE
    FP8_BLS24479_from_FP4(&b, &t);
    FP8_BLS24479_zero(&c);
#endif
#if SEXTIC_TWIST_BLS24479==M_TYPE
    FP8_BLS24479_zero(&b);
    FP8_BLS24479_from_FP4H(&c, &t);
#endif

    FP24_BLS24479_from_FP8s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}

// Precompute table of line functions for fixed G2 value
void PAIR_BLS24479_precomp(FP8_BLS24479 T[], ECP4_BLS24479* GV)
{
    int i, j, nb, bt;
    BIG_480_29 n, n3;
    FP4_BLS24479 AA, BB, CC;
    ECP4_BLS24479 A, G, NG;

    ECP4_BLS24479_copy(&A, GV);
    ECP4_BLS24479_copy(&G, GV);
    ECP4_BLS24479_copy(&NG, GV);
    ECP4_BLS24479_neg(&NG);

    nb = PAIR_BLS24479_nbits(n3, n);
    j = 0;

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS24479_double(&A, &AA, &BB, &CC);
        PAIR_BLS24479_pack(&T[j++], &AA, &BB, &CC);

        bt = BIG_480_29_bit(n3, i) - BIG_480_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS24479_add(&A, &G, &AA, &BB, &CC);
            PAIR_BLS24479_pack(&T[j++], &AA, &BB, &CC);
        }
        if (bt == -1)
        {
            PAIR_BLS24479_add(&A, &NG, &AA, &BB, &CC);
            PAIR_BLS24479_pack(&T[j++], &AA, &BB, &CC);
        }
    }
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void PAIR_BLS24479_another_pc(FP24_BLS24479 r[], FP8_BLS24479 T[], ECP_BLS24479 *QV)
{
    int i, j, nb, bt;
    BIG_480_29 x, n, n3;
    FP24_BLS24479 lv, lv2;
    ECP_BLS24479 Q;
    FP_BLS24479 Qx, Qy;

    if (ECP_BLS24479_isinf(QV)) return;

    nb = PAIR_BLS24479_nbits(n3, n);

    ECP_BLS24479_copy(&Q, QV);
    ECP_BLS24479_affine(&Q);

    FP_BLS24479_copy(&Qx, &(Q.x));
    FP_BLS24479_copy(&Qy, &(Q.y));

    j = 0;
    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS24479_unpack(&lv, &T[j++], &Qx, &Qy);

        bt = BIG_480_29_bit(n3, i) - BIG_480_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS24479_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP24_BLS24479_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS24479_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP24_BLS24479_smul(&lv, &lv2);
        }
        FP24_BLS24479_ssmul(&r[i], &lv);
    }
}

/* Accumulate another set of line functions for n-pairing */
void PAIR_BLS24479_another(FP24_BLS24479 r[], ECP4_BLS24479* PV, ECP_BLS24479* QV)
{
    int i, j, nb, bt;
    BIG_480_29 x, n, n3;
    FP24_BLS24479 lv, lv2;
    ECP4_BLS24479 A, NP, P;
    ECP_BLS24479 Q;
    FP_BLS24479 Qx, Qy;

    if (ECP_BLS24479_isinf(QV)) return;

    nb = PAIR_BLS24479_nbits(n3, n);

    ECP4_BLS24479_copy(&P, PV);
    ECP_BLS24479_copy(&Q, QV);

    ECP4_BLS24479_affine(&P);
    ECP_BLS24479_affine(&Q);

    FP_BLS24479_copy(&Qx, &(Q.x));
    FP_BLS24479_copy(&Qy, &(Q.y));

    ECP4_BLS24479_copy(&A, &P);
    ECP4_BLS24479_copy(&NP, &P); ECP4_BLS24479_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS24479_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_480_29_bit(n3, i) - BIG_480_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS24479_line(&lv2, &A, &P, &Qx, &Qy);
            FP24_BLS24479_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS24479_line(&lv2, &A, &NP, &Qx, &Qy);
            FP24_BLS24479_smul(&lv, &lv2);
        }
        FP24_BLS24479_ssmul(&r[i], &lv);
    }
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_BLS24479_ate(FP24_BLS24479 *r, ECP4_BLS24479 *P1, ECP_BLS24479 *Q1)
{
    BIG_480_29 x, n, n3;
    FP_BLS24479 Qx, Qy;
    int i, nb, bt;
    ECP4_BLS24479 A, NP, P;
    ECP_BLS24479 Q;
    FP24_BLS24479 lv, lv2;

    FP24_BLS24479_one(r);

    if (ECP_BLS24479_isinf(Q1)) return;

    nb = PAIR_BLS24479_nbits(n3, n);

    ECP4_BLS24479_copy(&P, P1);
    ECP_BLS24479_copy(&Q, Q1);

    ECP4_BLS24479_affine(&P);
    ECP_BLS24479_affine(&Q);


    FP_BLS24479_copy(&Qx, &(Q.x));
    FP_BLS24479_copy(&Qy, &(Q.y));

    ECP4_BLS24479_copy(&A, &P);
    ECP4_BLS24479_copy(&NP, &P); ECP4_BLS24479_neg(&NP);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP24_BLS24479_sqr(r, r);
        PAIR_BLS24479_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_480_29_bit(n3, i) - BIG_480_29_bit(n, i); // BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS24479_line(&lv2, &A, &P, &Qx, &Qy);
            FP24_BLS24479_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS24479_line(&lv2, &A, &NP, &Qx, &Qy);
            FP24_BLS24479_smul(&lv, &lv2);
        }
        FP24_BLS24479_ssmul(r, &lv);
    }

#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(r, r);
#endif

}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_BLS24479_double_ate(FP24_BLS24479 *r, ECP4_BLS24479 *P1, ECP_BLS24479 *Q1, ECP4_BLS24479 *R1, ECP_BLS24479 *S1)
{
    BIG_480_29 x, n, n3;
    FP_BLS24479 Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP4_BLS24479 A, B, NP, NR, P, R;
    ECP_BLS24479 Q, S;
    FP24_BLS24479 lv, lv2;

    if (ECP_BLS24479_isinf(Q1))
    {
        PAIR_BLS24479_ate(r, R1, S1);
        return;
    }
    if (ECP_BLS24479_isinf(S1))
    {
        PAIR_BLS24479_ate(r, P1, Q1);
        return;
    }
    nb = PAIR_BLS24479_nbits(n3, n);

    ECP4_BLS24479_copy(&P, P1);
    ECP_BLS24479_copy(&Q, Q1);

    ECP4_BLS24479_affine(&P);
    ECP_BLS24479_affine(&Q);

    ECP4_BLS24479_copy(&R, R1);
    ECP_BLS24479_copy(&S, S1);

    ECP4_BLS24479_affine(&R);
    ECP_BLS24479_affine(&S);

    FP_BLS24479_copy(&Qx, &(Q.x));
    FP_BLS24479_copy(&Qy, &(Q.y));

    FP_BLS24479_copy(&Sx, &(S.x));
    FP_BLS24479_copy(&Sy, &(S.y));

    ECP4_BLS24479_copy(&A, &P);
    ECP4_BLS24479_copy(&B, &R);
    ECP4_BLS24479_copy(&NP, &P); ECP4_BLS24479_neg(&NP);
    ECP4_BLS24479_copy(&NR, &R); ECP4_BLS24479_neg(&NR);

    FP24_BLS24479_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP24_BLS24479_sqr(r, r);
        PAIR_BLS24479_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_BLS24479_line(&lv2, &B, &B, &Sx, &Sy);
        FP24_BLS24479_smul(&lv, &lv2);
        FP24_BLS24479_ssmul(r, &lv);

        bt = BIG_480_29_bit(n3, i) - BIG_480_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS24479_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_BLS24479_line(&lv2, &B, &R, &Sx, &Sy);
            FP24_BLS24479_smul(&lv, &lv2);
            FP24_BLS24479_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_BLS24479_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_BLS24479_line(&lv2, &B, &NR, &Sx, &Sy);
            FP24_BLS24479_smul(&lv, &lv2);
            FP24_BLS24479_ssmul(r, &lv);
        }
    }



#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(r, r);
#endif

}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */

void PAIR_BLS24479_fexp(FP24_BLS24479 *r)
{
    FP2_BLS24479 X;
    BIG_480_29 x;
    FP_BLS24479 a, b;
    FP24_BLS24479 t0, t1, t2; //, t3, t4, t5, t6, t7; // could lose one of these - r=t3

    BIG_480_29_rcopy(x, CURVE_Bnx_BLS24479);
    FP_BLS24479_rcopy(&a, Fra_BLS24479);
    FP_BLS24479_rcopy(&b, Frb_BLS24479);
    FP2_BLS24479_from_FPs(&X, &a, &b);

    /* Easy part of final exp - r^(p^12-1)(p^4+1)*/

    FP24_BLS24479_inv(&t0, r);
    FP24_BLS24479_conj(r, r);

    FP24_BLS24479_mul(r, &t0);
    FP24_BLS24479_copy(&t0, r);

    FP24_BLS24479_frob(r, &X, 4);

    FP24_BLS24479_mul(r, &t0);

// See https://eprint.iacr.org/2020/875.pdf
    FP24_BLS24479_usqr(&t2,r);
    FP24_BLS24479_mul(&t2,r);     // t2=r^3

    FP24_BLS24479_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t1, &t1);
#endif
    FP24_BLS24479_conj(&t0,r);    // t0=r^-1
    FP24_BLS24479_copy(r,&t1);
    FP24_BLS24479_mul(r,&t0);    // r=r^(x-1)

    FP24_BLS24479_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t1, &t1);
#endif
    FP24_BLS24479_conj(&t0,r);    // t0=r^-1
    FP24_BLS24479_copy(r,&t1);
    FP24_BLS24479_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP24_BLS24479_pow(&t1,r,x);  // t1=r^x
#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t1, &t1);
#endif
    FP24_BLS24479_copy(&t0,r);   
    FP24_BLS24479_frob(&t0,&X,1); // t0=r^p
    FP24_BLS24479_copy(r,&t1);
    FP24_BLS24479_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2)
    FP24_BLS24479_pow(&t1,r,x);  
    FP24_BLS24479_pow(&t1,&t1,x); // t1=r^x^2
    FP24_BLS24479_copy(&t0,r);    
    FP24_BLS24479_frob(&t0,&X,2);   // t0=r^p^2
    FP24_BLS24479_mul(&t1,&t0);   // t1=r^x^2.r^p^2
    FP24_BLS24479_copy(r,&t1);

// ^(x^4+p^4-1)
    FP24_BLS24479_pow(&t1,r,x);  
    FP24_BLS24479_pow(&t1,&t1,x); 
    FP24_BLS24479_pow(&t1,&t1,x); 
    FP24_BLS24479_pow(&t1,&t1,x); // t1=r^x^4
    FP24_BLS24479_copy(&t0,r);    
    FP24_BLS24479_frob(&t0,&X,4); // t0=r^p^4
    FP24_BLS24479_mul(&t1,&t0);   // t1=r^x^4.r^p^4
    FP24_BLS24479_conj(&t0,r);    // t0=r^-1
    FP24_BLS24479_copy(r,&t1);    
    FP24_BLS24479_mul(r,&t0);     // r=r^x^4.r^p^4.r^-1

    FP24_BLS24479_mul(r,&t2);    
    FP24_BLS24479_reduce(r);

/*
// Ghamman & Fouotsa Method - (completely garbled in  https://eprint.iacr.org/2016/130)

    FP24_BLS24479_usqr(&t7, r);          // t7=f^2
    FP24_BLS24479_pow(&t1, &t7, x);      // t1=t7^u

    BIG_480_29_fshr(x, 1);
    FP24_BLS24479_pow(&t2, &t1, x);      // t2=t1^(u/2)
    BIG_480_29_fshl(x, 1); // x must be even

#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t1, &t1);
#endif

    FP24_BLS24479_conj(&t3, &t1);    // t3=1/t1
    FP24_BLS24479_mul(&t2, &t3);     // t2=t1*t3
    FP24_BLS24479_mul(&t2, r);       // t2=t2*f


    FP24_BLS24479_pow(&t3, &t2, x);      // t3=t2^u
    FP24_BLS24479_pow(&t4, &t3, x);      // t4=t3^u
    FP24_BLS24479_pow(&t5, &t4, x);      // t5=t4^u

#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t3, &t3);
    FP24_BLS24479_conj(&t5, &t5);
#endif

    FP24_BLS24479_frob(&t3, &X, 6);
    FP24_BLS24479_frob(&t4, &X, 5);

    FP24_BLS24479_mul(&t3, &t4);     // t3=t3.t4


    FP24_BLS24479_pow(&t6, &t5, x);      // t6=t5^u
#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t6, &t6);
#endif


    FP24_BLS24479_frob(&t5, &X, 4);
    FP24_BLS24479_mul(&t3, &t5); // ??


    FP24_BLS24479_conj(&t0, &t2);        // t0=1/t2
    FP24_BLS24479_mul(&t6, &t0);     // t6=t6*t0

    FP24_BLS24479_copy(&t5, &t6);
    FP24_BLS24479_frob(&t5, &X, 3);

    FP24_BLS24479_mul(&t3, &t5);     // t3=t3*t5
    FP24_BLS24479_pow(&t5, &t6, x);  // t5=t6^x
    FP24_BLS24479_pow(&t6, &t5, x);  // t6=t5^x

#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t5, &t5);
#endif

    FP24_BLS24479_copy(&t0, &t5);
    FP24_BLS24479_frob(&t0, &X, 2);
    FP24_BLS24479_mul(&t3, &t0);     // t3=t3*t0
    FP24_BLS24479_copy(&t0, &t6);    //
    FP24_BLS24479_frob(&t0, &X, 1);

    FP24_BLS24479_mul(&t3, &t0);     // t3=t3*t0
    FP24_BLS24479_pow(&t5, &t6, x);  // t5=t6*x

#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t5, &t5);
#endif

    FP24_BLS24479_frob(&t2, &X, 7);

    FP24_BLS24479_mul(&t5, &t7);     // t5=t5*t7
    FP24_BLS24479_mul(&t3, &t2);     // t3=t3*t2
    FP24_BLS24479_mul(&t3, &t5);     // t3=t3*t5

    FP24_BLS24479_mul(r, &t3);
    FP24_BLS24479_reduce(r);
*/
}

#ifdef USE_GLV_BLS24479
/* GLV method */
static void glv(BIG_480_29 u[2], BIG_480_29 ee)
{
    int bd;
    BIG_480_29 q,x,x2;
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);

// -(x^4).P = (Beta.x,y)

    BIG_480_29_rcopy(x, CURVE_Bnx_BLS24479);

    BIG_480_29_smul(x2, x, x);
    BIG_480_29_smul(x, x2, x2);
    bd=BIG_480_29_nbits(q)-BIG_480_29_nbits(x); // fixed x^4

    BIG_480_29_copy(u[0], ee);
    BIG_480_29_ctmod(u[0], x, bd);
    BIG_480_29_copy(u[1], ee);
    BIG_480_29_ctsdiv(u[1], x, bd);
    BIG_480_29_sub(u[1], q, u[1]);

    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void gs(BIG_480_29 u[8], BIG_480_29 ee)
{
    int i,bd;
    BIG_480_29 q,x,w;
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);

    BIG_480_29_rcopy(x, CURVE_Bnx_BLS24479);
    BIG_480_29_copy(w, ee);
    bd=BIG_480_29_nbits(q)-BIG_480_29_nbits(x); // fixed

    for (i = 0; i < 7; i++)
    {
        BIG_480_29_copy(u[i], w);
        BIG_480_29_ctmod(u[i], x, bd);
        BIG_480_29_ctsdiv(w, x, bd);
    }
    BIG_480_29_copy(u[7], w);

    /*  */
#if SIGN_OF_X_BLS24479==NEGATIVEX
    BIG_480_29_modneg(u[1], u[1], q);
    BIG_480_29_modneg(u[3], u[3], q);
    BIG_480_29_modneg(u[5], u[5], q);
    BIG_480_29_modneg(u[7], u[7], q);
#endif
    return;
}

/* Multiply P by e in group G1 */
void PAIR_BLS24479_G1mul(ECP_BLS24479 *P, BIG_480_29 e)
{
    BIG_480_29 ee,q;
#ifdef USE_GLV_BLS24479   /* Note this method is patented */
    int np, nn;
    ECP_BLS24479 Q;
    FP_BLS24479 cru;
    BIG_480_29 t;
    BIG_480_29 u[2];
    BIG_480_29_copy(ee,e);
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
    BIG_480_29_mod(ee,q);

    glv(u, ee);

    ECP_BLS24479_copy(&Q, P); ECP_BLS24479_affine(&Q);
    FP_BLS24479_rcopy(&cru, CRu_BLS24479);
    FP_BLS24479_mul(&(Q.x), &(Q.x), &cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np = BIG_480_29_nbits(u[0]);
    BIG_480_29_modneg(t, u[0], q);
    nn = BIG_480_29_nbits(t);
    if (nn < np)
    {
        BIG_480_29_copy(u[0], t);
        ECP_BLS24479_neg(P);
    }

    np = BIG_480_29_nbits(u[1]);
    BIG_480_29_modneg(t, u[1], q);
    nn = BIG_480_29_nbits(t);
    if (nn < np)
    {
        BIG_480_29_copy(u[1], t);
        ECP_BLS24479_neg(&Q);
    }
    BIG_480_29_norm(u[0]);
    BIG_480_29_norm(u[1]);
    ECP_BLS24479_mul2(P, &Q, u[0], u[1]);

#else
    BIG_480_29_copy(ee,e);
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
    BIG_480_29_mod(ee,q);
    ECP_BLS24479_clmul(P, ee, q);
#endif
}

/* Multiply P by e in group G2 */
void PAIR_BLS24479_G2mul(ECP4_BLS24479 *P, BIG_480_29 e)
{
    BIG_480_29 ee,q;
#ifdef USE_GS_G2_BLS24479   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP4_BLS24479 Q[8];
    FP2_BLS24479 X[3];
    BIG_480_29 x, y, u[8];

    BIG_480_29_copy(ee,e);
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
    BIG_480_29_mod(ee,q);

    ECP4_BLS24479_frob_constants(X);
    gs(u, ee);

    ECP4_BLS24479_copy(&Q[0], P);
    for (i = 1; i < 8; i++)
    {
        ECP4_BLS24479_copy(&Q[i], &Q[i - 1]);
        ECP4_BLS24479_frob(&Q[i], X, 1);
    }

    for (i = 0; i < 8; i++)
    {
        np = BIG_480_29_nbits(u[i]);
        BIG_480_29_modneg(x, u[i], q);
        nn = BIG_480_29_nbits(x);
        if (nn < np)
        {
            BIG_480_29_copy(u[i], x);
            ECP4_BLS24479_neg(&Q[i]);
        }
        BIG_480_29_norm(u[i]);
    }

    ECP4_BLS24479_mul8(P, Q, u);

#else
    BIG_480_29_copy(ee,e);
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
    BIG_480_29_mod(ee,q);
    ECP4_BLS24479_mul(P, ee);
#endif
}

/* f=f^e */
void PAIR_BLS24479_GTpow(FP24_BLS24479 *f, BIG_480_29 e)
{
    BIG_480_29 ee,q;
#ifdef USE_GS_GT_BLS24479   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see FP8.c */
    int i, np, nn;
    FP24_BLS24479 g[8];
    FP2_BLS24479 X;
    BIG_480_29 t;
    FP_BLS24479 fx, fy;
    BIG_480_29 u[8];

    FP_BLS24479_rcopy(&fx, Fra_BLS24479);
    FP_BLS24479_rcopy(&fy, Frb_BLS24479);
    FP2_BLS24479_from_FPs(&X, &fx, &fy);

    BIG_480_29_copy(ee,e);
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
    BIG_480_29_mod(ee,q);
    gs(u, ee);

    FP24_BLS24479_copy(&g[0], f);
    for (i = 1; i < 8; i++)
    {
        FP24_BLS24479_copy(&g[i], &g[i - 1]);
        FP24_BLS24479_frob(&g[i], &X, 1);
    }

    for (i = 0; i < 8; i++)
    {
        np = BIG_480_29_nbits(u[i]);
        BIG_480_29_modneg(t, u[i], q);
        nn = BIG_480_29_nbits(t);
        if (nn < np)
        {
            BIG_480_29_copy(u[i], t);
            FP24_BLS24479_conj(&g[i], &g[i]);
        }
        BIG_480_29_norm(u[i]);
    }
    FP24_BLS24479_pow8(f, g, u);

#else
    BIG_480_29_copy(ee,e);
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
    BIG_480_29_mod(ee,q);
    FP24_BLS24479_pow(f, f, ee);
#endif
}

/* test G1 group membership */

int PAIR_BLS24479_G1member(ECP_BLS24479 *P)
{
    ECP_BLS24479 W,T;
    BIG_480_29 x;
    FP_BLS24479 cru;
    if (ECP_BLS24479_isinf(P)) return 0;

    BIG_480_29_rcopy(x, CURVE_Bnx_BLS24479);
    ECP_BLS24479_copy(&W,P);
    ECP_BLS24479_copy(&T,P);
    ECP_BLS24479_mul(&T,x); 
    if (ECP_BLS24479_equals(P,&T)) return 0;    // P is of low order       
    ECP_BLS24479_mul(&T,x);
    ECP_BLS24479_mul(&T,x); ECP_BLS24479_mul(&T,x);
    ECP_BLS24479_neg(&T);

    FP_BLS24479_rcopy(&cru, CRu_BLS24479);
    FP_BLS24479_mul(&(W.x), &(W.x), &cru);
    if (!ECP_BLS24479_equals(&W,&T)) return 0;  // check that Endomorphism works

// Not needed
//    ECP_BLS24479_add(&W,P);
//    FP_BLS24479_mul(&(T.x), &(T.x), &cru);
//    ECP_BLS24479_add(&W,&T);
//    if (!ECP_BLS24479_isinf(&W)) return 0;      // use it to check order

/*	BIG_480_29 q;
	ECP_BLS24479 W;
    if (ECP_BLS24479_isinf(P)) return 0;
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
	ECP_BLS24479_copy(&W,P);
	ECP_BLS24479_mul(&W,q);
	if (!ECP_BLS24479_isinf(&W)) return 0; */
	return 1;
}

/* test G2 group membership */

int PAIR_BLS24479_G2member(ECP4_BLS24479 *P)
{
    ECP4_BLS24479 W,T;
    BIG_480_29 x;
    FP2_BLS24479 X[3];

    if (ECP4_BLS24479_isinf(P)) return 0;
    ECP4_BLS24479_frob_constants(X);
    BIG_480_29_rcopy(x, CURVE_Bnx_BLS24479);

    ECP4_BLS24479_copy(&W,P);
    ECP4_BLS24479_frob(&W, X, 1);

    ECP4_BLS24479_copy(&T,P);
    ECP4_BLS24479_mul(&T,x);

#if SIGN_OF_X_BLS24479==NEGATIVEX
    ECP4_BLS24479_neg(&T);
#endif
/*
    ECP4_BLS24479_copy(&R,&W);
    ECP4_BLS24479_frob(&R,X,1);    // R=\psi^2(P)
    ECP4_BLS24479_sub(&W,&R);
    ECP4_BLS24479_copy(&R,&T);    // R=xP
    ECP4_BLS24479_frob(&R,X,1);
    ECP4_BLS24479_add(&W,&R);     // W=\psi(P)-\psi^2(P)+\psi(xP)
*/
    if (ECP4_BLS24479_equals(&W,&T)) return 1;
    return 0;
/*
	BIG_480_29 q;
	ECP4_BLS24479 W;
    if (ECP4_BLS24479_isinf(P)) return 0;
    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
	ECP4_BLS24479_copy(&W,P);
	ECP4_BLS24479_mul(&W,q);
	if (!ECP4_BLS24479_isinf(&W)) return 0;
	return 1; */
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^8}=m^{p^4} */
int PAIR_BLS24479_GTcyclotomic(FP24_BLS24479 *m)
{
	FP_BLS24479 fx,fy;
	FP2_BLS24479 X;
	FP24_BLS24479 r,w;
	if (FP24_BLS24479_isunity(m)) return 0;
	FP24_BLS24479_conj(&r,m);
	FP24_BLS24479_mul(&r,m);
	if (!FP24_BLS24479_isunity(&r)) return 0;

	FP_BLS24479_rcopy(&fx,Fra_BLS24479);
	FP_BLS24479_rcopy(&fy,Frb_BLS24479);
	FP2_BLS24479_from_FPs(&X,&fx,&fy);

	FP24_BLS24479_copy(&r,m); FP24_BLS24479_frob(&r,&X,4); 
	FP24_BLS24479_copy(&w,&r); FP24_BLS24479_frob(&w,&X,4); 
	FP24_BLS24479_mul(&w,m);

	if (!FP24_BLS24479_equals(&w,&r)) return 0;
    return 1;
}


/* test for full GT group membership */
int PAIR_BLS24479_GTmember(FP24_BLS24479 *m)
{
    BIG_480_29 x;
    FP2_BLS24479 X;
    FP_BLS24479 fx, fy;
    FP24_BLS24479 r,t;
    if (!PAIR_BLS24479_GTcyclotomic(m)) return 0;

    FP_BLS24479_rcopy(&fx, Fra_BLS24479);
    FP_BLS24479_rcopy(&fy, Frb_BLS24479);
    FP2_BLS24479_from_FPs(&X, &fx, &fy);
    BIG_480_29_rcopy(x, CURVE_Bnx_BLS24479);

    FP24_BLS24479_copy(&r,m);
    FP24_BLS24479_frob(&r, &X, 1);

    FP24_BLS24479_pow(&t,m,x);

#if SIGN_OF_X_BLS24479==NEGATIVEX
    FP24_BLS24479_conj(&t,&t);
#endif

    if (FP24_BLS24479_equals(&r,&t)) return 1;
    return 0;

/*
	BIG_480_29 q;
    FP24_BLS24479 r;
    if (!PAIR_BLS24479_GTcyclotomic(m)) return 0;

    BIG_480_29_rcopy(q, CURVE_Order_BLS24479);
	FP24_BLS24479_pow(&r,m,q);
	if (!FP24_BLS24479_isunity(&r)) return 0;
	return 1;
*/
}


#ifdef HAS_MAIN

using namespace std;
using namespace BLS24479;


// g++ -O2 pair4_BLS24.cpp ecp4_BLS24.cpp fp24_BLS24.cpp fp8_BLS24.cpp fp4_BLS24.cpp fp2_BLS24.cpp ecp_BLS24.cpp fp_BLS24.cpp big_480_29.cpp rom_curve_BLS24.cpp rom_field_BLS24.cpp rand.cpp hash.cpp oct.cpp -o pair4_BLS24.exe

int main()
{
    int i;
    char byt[32];
    csprng rng;
    BIG_480_29 xa, xb, ya, yb, w, a, b, t1, q, u[2], v[4], m, r, xx, x2, x4, p;
    ECP4_BLS24479 P, G;
    ECP_BLS24479 Q, R;
    FP24 g, gp;
    FP8_BLS24479 t, c, cp, cpm1, cpm2;
    FP4_BLS24479 X, Y;
    FP2_BLS24479 x, y, f, Aa, Bb;
    FP_BLS24479 cru;

    for (i = 0; i < 32; i++)
        byt[i] = i + 9;
    RAND_seed(&rng, 32, byt);

    BIG_480_29_rcopy(r, CURVE_Order);
    BIG_480_29_rcopy(p, Modulus);


    BIG_480_29_rcopy(xa, CURVE_Gx);
    BIG_480_29_rcopy(ya, CURVE_Gy);

    ECP_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_output(&Q);
    printf("\n");

    ECP4_BLS24479_generator(&P);

    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    BIG_480_29_rcopy(a, Fra);
    BIG_480_29_rcopy(b, Frb);
    FP2_BLS24479 from_BIGs(&f, a, b);

    PAIR_ate(&g, &P, &Q);

    printf("gb= ");
    FP24_output(&g);
    printf("\n");
    PAIR_fexp(&g);

    printf("g= ");
    FP24_output(&g);
    printf("\n");

    ECP_copy(&R, &Q);
    ECP4_BLS24479_copy(&G, &P);

    ECP4_BLS24479_dbl(&G);
    ECP_dbl(&R);
    ECP_affine(&R);

    PAIR_ate(&g, &G, &Q);
    PAIR_fexp(&g);

    printf("g1= ");
    FP24_output(&g);
    printf("\n");

    PAIR_ate(&g, &P, &R);
    PAIR_fexp(&g);

    printf("g2= ");
    FP24_output(&g);
    printf("\n");


    PAIR_G1mul(&Q, r);
    printf("rQ= "); ECP_output(&Q); printf("\n");

    PAIR_G2mul(&P, r);
    printf("rP= "); ECP4_BLS24479_output(&P); printf("\n");

    BIG_480_29_randomnum(w, r, &rng);

    FP24_copy(&gp, &g);

    PAIR_GTpow(&g, w);

    FP24_trace(&t, &g);

    printf("g^r=  "); FP8_output(&t); printf("\n");

    FP24_compow(&t, &gp, w, r);

    printf("t(g)= "); FP8_output(&t); printf("\n");

}

#endif

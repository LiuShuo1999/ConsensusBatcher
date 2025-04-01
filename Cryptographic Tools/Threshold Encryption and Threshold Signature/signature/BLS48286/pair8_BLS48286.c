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

#include "pair8_BLS48286.h"


// Point doubling for pairings
static void PAIR_BLS48286_double(ECP8_BLS48286 *A, FP8_BLS48286 *AA, FP8_BLS48286 *BB, FP8_BLS48286 *CC)
{
    FP8_BLS48286 YY;
    FP8_BLS48286_copy(CC, &(A->x));  //FP8 XX=new FP8(A.getx());  //X
    FP8_BLS48286_copy(&YY, &(A->y)); //FP8 YY=new FP8(A.gety());  //Y
    FP8_BLS48286_copy(BB, &(A->z));  //FP8 ZZ=new FP8(A.getz());  //Z

    FP8_BLS48286_copy(AA, &YY);      //FP8 YZ=new FP8(YY);        //Y
    FP8_BLS48286_mul(AA, AA, BB);    //YZ.mul(ZZ);                //YZ
    FP8_BLS48286_sqr(CC, CC);        //XX.sqr();                //X^2
    FP8_BLS48286_sqr(&YY, &YY);      //YY.sqr();                //Y^2
    FP8_BLS48286_sqr(BB, BB);        //ZZ.sqr();                //Z^2

    FP8_BLS48286_add(AA, AA, AA);
    FP8_BLS48286_neg(AA, AA);
    FP8_BLS48286_norm(AA);           // -2YZ
    FP8_BLS48286_times_i(AA);        // -2YZi

    FP8_BLS48286_imul(BB, BB, 3 * CURVE_B_I_BLS48286); //3Bz^2
    FP8_BLS48286_imul(CC, CC, 3);            //3X^2

#if SEXTIC_TWIST_BLS48286==D_TYPE
    FP8_BLS48286_times_i(&YY);                   // Y^2.i
    FP8_BLS48286_times_i(CC);                    // 3X^2.i
#endif

#if SEXTIC_TWIST_BLS48286==M_TYPE
    FP8_BLS48286_times_i(BB);                    // 3Bz^2.i
#endif

    FP8_BLS48286_sub(BB, BB, &YY);
    FP8_BLS48286_norm(BB);

    ECP8_BLS48286_dbl(A);            //A.dbl();
}

// Point addition for pairings
static void PAIR_BLS48286_add(ECP8_BLS48286 *A, ECP8_BLS48286 *B, FP8_BLS48286 *AA, FP8_BLS48286 *BB, FP8_BLS48286 *CC)
{
    FP8_BLS48286 T1;
    FP8_BLS48286_copy(AA, &(A->x));      //FP8 X1=new FP8(A.getx());    // X1
    FP8_BLS48286_copy(CC, &(A->y));      //FP8 Y1=new FP8(A.gety());    // Y1
    FP8_BLS48286_copy(&T1, &(A->z));     //FP8 T1=new FP8(A.getz());    // Z1

    FP8_BLS48286_copy(BB, &T1);          //FP8 T2=new FP8(A.getz());    // Z1

    FP8_BLS48286_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());    // T1=Z1.Y2
    FP8_BLS48286_mul(BB, BB, &(B->x));   //T2.mul(B.getx());    // T2=Z1.X2

    FP8_BLS48286_sub(AA, AA, BB);        //X1.sub(T2);
    FP8_BLS48286_norm(AA);               //X1.norm();  // X1=X1-Z1.X2
    FP8_BLS48286_sub(CC, CC, &T1);       //Y1.sub(T1);
    FP8_BLS48286_norm(CC);               //Y1.norm();  // Y1=Y1-Z1.Y2

    FP8_BLS48286_copy(&T1, AA);          //T1.copy(X1);            // T1=X1-Z1.X2

#if SEXTIC_TWIST_BLS48286==M_TYPE
    FP8_BLS48286_times_i(AA);
    FP8_BLS48286_norm(AA);
#endif

    FP8_BLS48286_mul(&T1, &T1, &(B->y)); //T1.mul(B.gety());       // T1=(X1-Z1.X2).Y2

    FP8_BLS48286_copy(BB, CC);           //T2.copy(Y1);            // T2=Y1-Z1.Y2
    FP8_BLS48286_mul(BB, BB, &(B->x));   //T2.mul(B.getx());       // T2=(Y1-Z1.Y2).X2
    FP8_BLS48286_sub(BB, BB, &T1);       //T2.sub(T1);
    FP8_BLS48286_norm(BB);               //T2.norm();          // T2=(Y1-Z1.Y2).X2 - (X1-Z1.X2).Y2

    FP8_BLS48286_neg(CC, CC);            //Y1.neg();
    FP8_BLS48286_norm(CC);               //Y1.norm(); // Y1=-(Y1-Z1.Y2).Xs                - ***

    ECP8_BLS48286_add(A, B);             //A.add(B);
}


/* Line function */
static void PAIR_BLS48286_line(FP48_BLS48286 *v, ECP8_BLS48286 *A, ECP8_BLS48286 *B, FP_BLS48286 *Qx, FP_BLS48286 *Qy)
{
    FP8_BLS48286 AA, BB, CC;
    FP16_BLS48286 a, b, c;

    if (A == B)
        PAIR_BLS48286_double(A, &AA, &BB, &CC);
    else
        PAIR_BLS48286_add(A, B, &AA, &BB, &CC);

    FP8_BLS48286_tmul(&CC, &CC, Qx);
    FP8_BLS48286_tmul(&AA, &AA, Qy);

    FP16_BLS48286_from_FP8s(&a, &AA, &BB);
#if SEXTIC_TWIST_BLS48286==D_TYPE
    FP16_BLS48286_from_FP8(&b, &CC);
    FP16_BLS48286_zero(&c);
#endif
#if SEXTIC_TWIST_BLS48286==M_TYPE
    FP16_BLS48286_zero(&b);
    FP16_BLS48286_from_FP8H(&c, &CC);
#endif

    FP48_BLS48286_from_FP16s(v, &a, &b, &c);
    v->type = FP_SPARSER;
}

/* prepare ate parameter, n=6u+2 (BN) or n=u (BLS), n3=3*n */
int PAIR_BLS48286_nbits(BIG_288_29 n3, BIG_288_29 n)
{
    BIG_288_29 x;
    BIG_288_29_rcopy(x, CURVE_Bnx_BLS48286);

    BIG_288_29_copy(n, x);
    BIG_288_29_norm(n);
    BIG_288_29_pmul(n3, n, 3);
    BIG_288_29_norm(n3);

    return BIG_288_29_nbits(n3);
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
void PAIR_BLS48286_initmp(FP48_BLS48286 r[])
{
    int i;
    for (i = ATE_BITS_BLS48286 - 1; i >= 0; i--)
        FP48_BLS48286_one(&r[i]);
    return;
}

// Store precomputed line details in an FP4
static void PAIR_BLS48286_pack(FP16_BLS48286 *T, FP8_BLS48286* AA, FP8_BLS48286* BB, FP8_BLS48286 *CC)
{
    FP8_BLS48286 I, A, B;
    FP8_BLS48286_inv(&I, CC, NULL);
    FP8_BLS48286_mul(&A, AA, &I);
    FP8_BLS48286_mul(&B, BB, &I);
    FP16_BLS48286_from_FP8s(T, &A, &B);
}

// Unpack G2 line function details and include G1
static void PAIR_BLS48286_unpack(FP48_BLS48286 *v, FP16_BLS48286* T, FP_BLS48286 *Qx, FP_BLS48286 *Qy)
{
    FP16_BLS48286 a, b, c;
    FP8_BLS48286 t;
    FP16_BLS48286_copy(&a, T);
    FP8_BLS48286_tmul(&a.a, &a.a, Qy);
    FP8_BLS48286_from_FP(&t, Qx);

#if SEXTIC_TWIST_BLS48286==D_TYPE
    FP16_BLS48286_from_FP8(&b, &t);
    FP16_BLS48286_zero(&c);
#endif
#if SEXTIC_TWIST_BLS48286==M_TYPE
    FP16_BLS48286_zero(&b);
    FP16_BLS48286_from_FP8H(&c, &t);
#endif

    FP48_BLS48286_from_FP16s(v, &a, &b, &c);
    v->type = FP_SPARSEST;
}

/* basic Miller loop */
void PAIR_BLS48286_miller(FP48_BLS48286 *res, FP48_BLS48286 r[])
{
    int i;
    FP48_BLS48286_one(res);
    for (i = ATE_BITS_BLS48286 - 1; i >= 1; i--)
    {
        FP48_BLS48286_sqr(res, res);
        FP48_BLS48286_ssmul(res, &r[i]);
        FP48_BLS48286_zero(&r[i]);
    }

#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(res, res);
#endif
    FP48_BLS48286_ssmul(res, &r[0]);
    FP48_BLS48286_zero(&r[0]);
    return;
}

// Precompute table of line functions for fixed G2 value
void PAIR_BLS48286_precomp(FP16_BLS48286 T[], ECP8_BLS48286* GV)
{
    int i, j, nb, bt;
    BIG_288_29 n, n3;
    FP8_BLS48286 AA, BB, CC;
    ECP8_BLS48286 A, G, NG;

    ECP8_BLS48286_copy(&A, GV);
    ECP8_BLS48286_copy(&G, GV);
    ECP8_BLS48286_copy(&NG, GV);
    ECP8_BLS48286_neg(&NG);

    nb = PAIR_BLS48286_nbits(n3, n);
    j = 0;

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS48286_double(&A, &AA, &BB, &CC);
        PAIR_BLS48286_pack(&T[j++], &AA, &BB, &CC);

        bt = BIG_288_29_bit(n3, i) - BIG_288_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48286_add(&A, &G, &AA, &BB, &CC);
            PAIR_BLS48286_pack(&T[j++], &AA, &BB, &CC);
        }
        if (bt == -1)
        {
            PAIR_BLS48286_add(&A, &NG, &AA, &BB, &CC);
            PAIR_BLS48286_pack(&T[j++], &AA, &BB, &CC);
        }
    }
}

/* Accumulate another set of line functions for n-pairing, assuming precomputation on G2 */
void PAIR_BLS48286_another_pc(FP48_BLS48286 r[], FP16_BLS48286 T[], ECP_BLS48286 *QV)
{
    int i, j, nb, bt;
    BIG_288_29 x, n, n3;
    FP48_BLS48286 lv, lv2;
    ECP_BLS48286 Q;
    FP_BLS48286 Qx, Qy;

    if (ECP_BLS48286_isinf(QV)) return;

    nb = PAIR_BLS48286_nbits(n3, n);

    ECP_BLS48286_copy(&Q, QV);
    ECP_BLS48286_affine(&Q);

    FP_BLS48286_copy(&Qx, &(Q.x));
    FP_BLS48286_copy(&Qy, &(Q.y));

    j = 0;
    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS48286_unpack(&lv, &T[j++], &Qx, &Qy);

        bt = BIG_288_29_bit(n3, i) - BIG_288_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48286_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP48_BLS48286_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS48286_unpack(&lv2, &T[j++], &Qx, &Qy);
            FP48_BLS48286_smul(&lv, &lv2);
        }
        FP48_BLS48286_ssmul(&r[i], &lv);
    }
}



/* Accumulate another set of line functions for n-pairing */
void PAIR_BLS48286_another(FP48_BLS48286 r[], ECP8_BLS48286* PV, ECP_BLS48286* QV)
{
    int i, j, nb, bt;
    BIG_288_29 x, n, n3;
    FP48_BLS48286 lv, lv2;
    ECP8_BLS48286 A, NP, P;
    ECP_BLS48286 Q;
    FP_BLS48286 Qx, Qy;

    if (ECP_BLS48286_isinf(QV)) return;

    nb = PAIR_BLS48286_nbits(n3, n);

    ECP8_BLS48286_copy(&P, PV);
    ECP_BLS48286_copy(&Q, QV);

    ECP8_BLS48286_affine(&P);
    ECP_BLS48286_affine(&Q);

    FP_BLS48286_copy(&Qx, &(Q.x));
    FP_BLS48286_copy(&Qy, &(Q.y));

    ECP8_BLS48286_copy(&A, &P);
    ECP8_BLS48286_copy(&NP, &P); ECP8_BLS48286_neg(&NP);

    for (i = nb - 2; i >= 1; i--)
    {
        PAIR_BLS48286_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_288_29_bit(n3, i) - BIG_288_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48286_line(&lv2, &A, &P, &Qx, &Qy);
            FP48_BLS48286_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS48286_line(&lv2, &A, &NP, &Qx, &Qy);
            FP48_BLS48286_smul(&lv, &lv2);
        }
        FP48_BLS48286_ssmul(&r[i], &lv);
    }
}

/* Optimal R-ate pairing r=e(P,Q) */
void PAIR_BLS48286_ate(FP48_BLS48286 *r, ECP8_BLS48286 *P1, ECP_BLS48286 *Q1)
{
    BIG_288_29 x, n, n3;
    FP_BLS48286 Qx, Qy;
    int i, nb, bt;
    ECP8_BLS48286 A, NP, P;
    ECP_BLS48286 Q;
    FP48_BLS48286 lv, lv2;

    FP48_BLS48286_one(r);

    if (ECP_BLS48286_isinf(Q1)) return;

    nb = PAIR_BLS48286_nbits(n3, n);

    ECP8_BLS48286_copy(&P, P1);
    ECP_BLS48286_copy(&Q, Q1);

    ECP8_BLS48286_affine(&P);
    ECP_BLS48286_affine(&Q);


    FP_BLS48286_copy(&Qx, &(Q.x));
    FP_BLS48286_copy(&Qy, &(Q.y));

    ECP8_BLS48286_copy(&A, &P);
    ECP8_BLS48286_copy(&NP, &P); ECP8_BLS48286_neg(&NP);



    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP48_BLS48286_sqr(r, r);
        PAIR_BLS48286_line(&lv, &A, &A, &Qx, &Qy);

        bt = BIG_288_29_bit(n3, i) - BIG_288_29_bit(n, i); // BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48286_line(&lv2, &A, &P, &Qx, &Qy);
            FP48_BLS48286_smul(&lv, &lv2);
        }
        if (bt == -1)
        {
            PAIR_BLS48286_line(&lv2, &A, &NP, &Qx, &Qy);
            FP48_BLS48286_smul(&lv, &lv2);
        }
        FP48_BLS48286_ssmul(r, &lv);
    }

#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(r, r);
#endif

}

/* Optimal R-ate double pairing e(P,Q).e(R,S) */
void PAIR_BLS48286_double_ate(FP48_BLS48286 *r, ECP8_BLS48286 *P1, ECP_BLS48286 *Q1, ECP8_BLS48286 *R1, ECP_BLS48286 *S1)
{
    BIG_288_29 x, n, n3;
    FP_BLS48286 Qx, Qy, Sx, Sy;
    int i, nb, bt;
    ECP8_BLS48286 A, B, NP, NR, P, R;
    ECP_BLS48286 Q, S;
    FP48_BLS48286 lv, lv2;

    if (ECP_BLS48286_isinf(Q1))
    {
        PAIR_BLS48286_ate(r, R1, S1);
        return;
    }
    if (ECP_BLS48286_isinf(S1))
    {
        PAIR_BLS48286_ate(r, P1, Q1);
        return;
    }
    nb = PAIR_BLS48286_nbits(n3, n);

    ECP8_BLS48286_copy(&P, P1);
    ECP_BLS48286_copy(&Q, Q1);

    ECP8_BLS48286_affine(&P);
    ECP_BLS48286_affine(&Q);

    ECP8_BLS48286_copy(&R, R1);
    ECP_BLS48286_copy(&S, S1);

    ECP8_BLS48286_affine(&R);
    ECP_BLS48286_affine(&S);

    FP_BLS48286_copy(&Qx, &(Q.x));
    FP_BLS48286_copy(&Qy, &(Q.y));

    FP_BLS48286_copy(&Sx, &(S.x));
    FP_BLS48286_copy(&Sy, &(S.y));

    ECP8_BLS48286_copy(&A, &P);
    ECP8_BLS48286_copy(&B, &R);
    ECP8_BLS48286_copy(&NP, &P); ECP8_BLS48286_neg(&NP);
    ECP8_BLS48286_copy(&NR, &R); ECP8_BLS48286_neg(&NR);

    FP48_BLS48286_one(r);

    /* Main Miller Loop */
    for (i = nb - 2; i >= 1; i--)
    {
        FP48_BLS48286_sqr(r, r);
        PAIR_BLS48286_line(&lv, &A, &A, &Qx, &Qy);
        PAIR_BLS48286_line(&lv2, &B, &B, &Sx, &Sy);
        FP48_BLS48286_smul(&lv, &lv2);
        FP48_BLS48286_ssmul(r, &lv);

        bt = BIG_288_29_bit(n3, i) - BIG_288_29_bit(n, i); // bt=BIG_bit(n,i);
        if (bt == 1)
        {
            PAIR_BLS48286_line(&lv, &A, &P, &Qx, &Qy);
            PAIR_BLS48286_line(&lv2, &B, &R, &Sx, &Sy);
            FP48_BLS48286_smul(&lv, &lv2);
            FP48_BLS48286_ssmul(r, &lv);
        }
        if (bt == -1)
        {
            PAIR_BLS48286_line(&lv, &A, &NP, &Qx, &Qy);
            PAIR_BLS48286_line(&lv2, &B, &NR, &Sx, &Sy);
            FP48_BLS48286_smul(&lv, &lv2);
            FP48_BLS48286_ssmul(r, &lv);
        }
    }

#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(r, r);
#endif

}

/* final exponentiation - keep separate for multi-pairings and to avoid thrashing stack */

void PAIR_BLS48286_fexp(FP48_BLS48286 *r)
{
    FP2_BLS48286 X;
    BIG_288_29 x;
    FP_BLS48286 a, b;
    FP48_BLS48286 t0, t1, t2;//, t3;

    BIG_288_29_rcopy(x, CURVE_Bnx_BLS48286);
    FP_BLS48286_rcopy(&a, Fra_BLS48286);
    FP_BLS48286_rcopy(&b, Frb_BLS48286);
    FP2_BLS48286_from_FPs(&X, &a, &b);

    /* Easy part of final exp - r^(p^24-1)(p^8+1)*/

    FP48_BLS48286_inv(&t0, r);
    FP48_BLS48286_conj(r, r);

    FP48_BLS48286_mul(r, &t0);
    FP48_BLS48286_copy(&t0, r);

    FP48_BLS48286_frob(r, &X, 8);

    FP48_BLS48286_mul(r, &t0);

// See https://eprint.iacr.org/2020/875.pdf
    FP48_BLS48286_usqr(&t2,r);
    FP48_BLS48286_mul(&t2,r);     // t2=r^3

    FP48_BLS48286_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_conj(&t0,r);    // t0=r^-1
    FP48_BLS48286_copy(r,&t1);
    FP48_BLS48286_mul(r,&t0);    // r=r^(x-1)

    FP48_BLS48286_pow(&t1,r,x);   // t1=r^x
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_conj(&t0,r);    // t0=r^-1
    FP48_BLS48286_copy(r,&t1);
    FP48_BLS48286_mul(r,&t0);    // r=r^(x-1)

// ^(x+p)
    FP48_BLS48286_pow(&t1,r,x);  // t1=r^x
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t0,r);   
    FP48_BLS48286_frob(&t0,&X,1); // t0=r^p
    FP48_BLS48286_copy(r,&t1);
    FP48_BLS48286_mul(r,&t0); // r=r^x.r^p

// ^(x^2+p^2)
    FP48_BLS48286_pow(&t1,r,x);  
    FP48_BLS48286_pow(&t1,&t1,x); // t1=r^x^2
    FP48_BLS48286_copy(&t0,r);    
    FP48_BLS48286_frob(&t0,&X,2);   // t0=r^p^2
    FP48_BLS48286_mul(&t1,&t0);   // t1=r^x^2.r^p^2
    FP48_BLS48286_copy(r,&t1);

// ^(x^4+p^4)
    FP48_BLS48286_pow(&t1,r,x);  
    FP48_BLS48286_pow(&t1,&t1,x);
    FP48_BLS48286_pow(&t1,&t1,x); 
    FP48_BLS48286_pow(&t1,&t1,x); // t1=r^x^4
    FP48_BLS48286_copy(&t0,r);    
    FP48_BLS48286_frob(&t0,&X,4);   // t0=r^p^4
    FP48_BLS48286_mul(&t1,&t0);   // t1=r^x^4.r^p^4
    FP48_BLS48286_copy(r,&t1);

// ^(x^8+p^8-1)
    FP48_BLS48286_pow(&t1,r,x);  
    FP48_BLS48286_pow(&t1,&t1,x); 
    FP48_BLS48286_pow(&t1,&t1,x); 
    FP48_BLS48286_pow(&t1,&t1,x); 
    FP48_BLS48286_pow(&t1,&t1,x);
    FP48_BLS48286_pow(&t1,&t1,x);
    FP48_BLS48286_pow(&t1,&t1,x); 
    FP48_BLS48286_pow(&t1,&t1,x); // t1=r^x^8
    FP48_BLS48286_copy(&t0,r);    
    FP48_BLS48286_frob(&t0,&X,8); // t0=r^p^8
    FP48_BLS48286_mul(&t1,&t0);   // t1=r^x^8.r^p^8
    FP48_BLS48286_conj(&t0,r);    // t0=r^-1
    FP48_BLS48286_copy(r,&t1);    
    FP48_BLS48286_mul(r,&t0);     // r=r^x^4.r^p^4.r^-1

    FP48_BLS48286_mul(r,&t2);    
    FP48_BLS48286_reduce(r);

/*
// f^e0.f^e1^p.f^e2^p^2.. .. f^e14^p^14.f^e15^p^15

    FP48_BLS48286_usqr(&t7, r);          // t7=f^2

    if (BIG_288_29_parity(x) == 1)
    {
        FP48_BLS48286_pow(&t2, r, x);    // f^u
        FP48_BLS48286_usqr(&t1, &t2);    // f^(2u)
        FP48_BLS48286_pow(&t2, &t2, x);  // f^u^2
    } else {
        FP48_BLS48286_pow(&t1, &t7, x);      // t1=f^2u
        BIG_288_29_fshr(x, 1);
        FP48_BLS48286_pow(&t2, &t1, x);      // t2=f^2u^(u/2) =  f^u^2
        BIG_288_29_fshl(x, 1);             // x must be even
    }

#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif

    FP48_BLS48286_conj(&t3, &t1);    // t3=f^-2u
    FP48_BLS48286_mul(&t2, &t3);     // t2=f^u^2.f^-2u
    FP48_BLS48286_mul(&t2, r);       // t2=f^u^2.f^-2u.f = f^(u^2-2u+1) = f^e15

    FP48_BLS48286_mul(r, &t7);       // f^3

    FP48_BLS48286_pow(&t1, &t2, x);  // f^e15^u = f^(u.e15) = f^(u^3-2u^2+u) = f^(e14)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 14); // f^(u^3-2u^2+u)^p^14
    FP48_BLS48286_mul(r, &t3);       // f^3.f^(u^3-2u^2+u)^p^14

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e14) = f^(u^4-2u^3+u^2) =  f^(e13)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 13); // f^(e13)^p^13
    FP48_BLS48286_mul(r, &t3);       // f^3.f^(u^3-2u^2+u)^p^14.f^(u^4-2u^3+u^2)^p^13

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e13)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 12); // f^(e12)^p^12
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e12)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 11); // f^(e11)^p^11
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e11)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 10); // f^(e10)^p^10
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e10)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 9);  // f^(e9)^p^9
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e9)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 8);  // f^(e8)^p^8
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e8)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_conj(&t3, &t2);
    FP48_BLS48286_mul(&t1, &t3); // f^(u.e8).f^-e15
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 7);  // f^(e7)^p^7
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e7)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 6);  // f^(e6)^p^6
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e6)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 5);  // f^(e5)^p^5
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e5)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 4);  // f^(e4)^p^4
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e4)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 3);  // f^(e3)^p^3
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e3)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 2);  // f^(e2)^p^2
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e2)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_copy(&t3, &t1);
    FP48_BLS48286_frob(&t3, &X, 1);  // f^(e1)^p^1
    FP48_BLS48286_mul(r, &t3);

    FP48_BLS48286_pow(&t1, &t1, x);  // f^(u.e1)
#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t1, &t1);
#endif
    FP48_BLS48286_mul(r, &t1);       // r.f^e0

    FP48_BLS48286_frob(&t2, &X, 15); // f^(e15.p^15)
    FP48_BLS48286_mul(r, &t2);

    FP48_BLS48286_reduce(r);
*/
}

#ifdef USE_GLV_BLS48286
/* GLV method */
static void glv(BIG_288_29 u[2], BIG_288_29 ee)
{

    int bd;
    BIG_288_29 q,x,x2;
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
// -(x^8).P = (Beta.x,y)

    BIG_288_29_rcopy(x, CURVE_Bnx_BLS48286);

    BIG_288_29_smul(x2, x, x);
    BIG_288_29_smul(x, x2, x2);
    BIG_288_29_smul(x2, x, x);
    bd=BIG_288_29_nbits(q)-BIG_288_29_nbits(x2); // fixed x^8

    BIG_288_29_copy(u[0], ee);
    BIG_288_29_ctmod(u[0], x2, bd);
    BIG_288_29_copy(u[1], ee);
    BIG_288_29_ctsdiv(u[1], x2, bd);

    BIG_288_29_sub(u[1], q, u[1]);

    return;
}
#endif // USE_GLV

/* Galbraith & Scott Method */
static void gs(BIG_288_29 u[16], BIG_288_29 ee)
{
    int i,bd;
    BIG_288_29 q,x,w;
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);

    BIG_288_29_rcopy(x, CURVE_Bnx_BLS48286);
    BIG_288_29_copy(w, ee);
    bd=BIG_288_29_nbits(q)-BIG_288_29_nbits(x); // fixed

    for (i = 0; i < 15; i++)
    {
        BIG_288_29_copy(u[i], w);
        BIG_288_29_ctmod(u[i], x, bd);
        BIG_288_29_ctsdiv(w, x, bd);
    }
    BIG_288_29_copy(u[15], w);

    /*  */
#if SIGN_OF_X_BLS48286==NEGATIVEX
    BIG_288_29_modneg(u[1], u[1], q);
    BIG_288_29_modneg(u[3], u[3], q);
    BIG_288_29_modneg(u[5], u[5], q);
    BIG_288_29_modneg(u[7], u[7], q);
    BIG_288_29_modneg(u[9], u[9], q);
    BIG_288_29_modneg(u[11], u[11], q);
    BIG_288_29_modneg(u[13], u[13], q);
    BIG_288_29_modneg(u[15], u[15], q);
#endif

    return;
}

/* Multiply P by e in group G1 */
void PAIR_BLS48286_G1mul(ECP_BLS48286 *P, BIG_288_29 e)
{
    BIG_288_29 ee,q;
#ifdef USE_GLV_BLS48286   /* Note this method is patented */
    int np, nn;
    ECP_BLS48286 Q;
    FP_BLS48286 cru;
    BIG_288_29 t;
    BIG_288_29 u[2];

    BIG_288_29_copy(ee,e);
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
    BIG_288_29_mod(ee,q);

    glv(u, ee);

    ECP_BLS48286_copy(&Q, P); ECP_BLS48286_affine(&Q);
    FP_BLS48286_rcopy(&cru, CRu_BLS48286);
    FP_BLS48286_mul(&(Q.x), &(Q.x), &cru);

    /* note that -a.B = a.(-B). Use a or -a depending on which is smaller */

    np = BIG_288_29_nbits(u[0]);
    BIG_288_29_modneg(t, u[0], q);
    nn = BIG_288_29_nbits(t);
    if (nn < np)
    {
        BIG_288_29_copy(u[0], t);
        ECP_BLS48286_neg(P);
    }

    np = BIG_288_29_nbits(u[1]);
    BIG_288_29_modneg(t, u[1], q);
    nn = BIG_288_29_nbits(t);
    if (nn < np)
    {
        BIG_288_29_copy(u[1], t);
        ECP_BLS48286_neg(&Q);
    }
    BIG_288_29_norm(u[0]);
    BIG_288_29_norm(u[1]);
    ECP_BLS48286_mul2(P, &Q, u[0], u[1]);

#else
    BIG_288_29_copy(ee,e);
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
    BIG_288_29_mod(ee,q);
    ECP_BLS48286_clmul(P, ee, q);
#endif
}

/* Multiply P by e in group G2 */
void PAIR_BLS48286_G2mul(ECP8_BLS48286 *P, BIG_288_29 e)
{
    BIG_288_29 ee,q;
#ifdef USE_GS_G2_BLS48286   /* Well I didn't patent it :) */
    int i, np, nn;
    ECP8_BLS48286 Q[16];
    FP2_BLS48286 X[3];
    BIG_288_29 x, u[16];

    BIG_288_29_copy(ee,e);
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
    BIG_288_29_mod(ee,q);

    ECP8_BLS48286_frob_constants(X);

    gs(u, ee);

    ECP8_BLS48286_copy(&Q[0], P);
    for (i = 1; i < 16; i++)
    {
        ECP8_BLS48286_copy(&Q[i], &Q[i - 1]);
        ECP8_BLS48286_frob(&Q[i], X, 1);
    }

    for (i = 0; i < 16; i++)
    {
        np = BIG_288_29_nbits(u[i]);
        BIG_288_29_modneg(x, u[i], q);
        nn = BIG_288_29_nbits(x);
        if (nn < np)
        {
            BIG_288_29_copy(u[i], x);
            ECP8_BLS48286_neg(&Q[i]);
        }
        BIG_288_29_norm(u[i]);
    }

    ECP8_BLS48286_mul16(P, Q, u);

#else
    BIG_288_29_copy(ee,e);
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
    BIG_288_29_mod(ee,q);
    ECP8_BLS48286_mul(P, ee);
#endif
}

/* f=f^e */
void PAIR_BLS48286_GTpow(FP48_BLS48286 *f, BIG_288_29 e)
{
    BIG_288_29 ee,q;
#ifdef USE_GS_GT_BLS48286   /* Note that this option requires a lot of RAM! Maybe better to use compressed XTR method, see FP16.cpp */
    int i, np, nn;
    FP48_BLS48286 g[16];
    FP2_BLS48286 X;
    BIG_288_29 t;
    FP_BLS48286 fx, fy;
    BIG_288_29 u[16];

    FP_BLS48286_rcopy(&fx, Fra_BLS48286);
    FP_BLS48286_rcopy(&fy, Frb_BLS48286);
    FP2_BLS48286_from_FPs(&X, &fx, &fy);

    BIG_288_29_copy(ee,e);
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
    BIG_288_29_mod(ee,q);
    gs(u, ee);

    FP48_BLS48286_copy(&g[0], f);
    for (i = 1; i < 16; i++)
    {
        FP48_BLS48286_copy(&g[i], &g[i - 1]);
        FP48_BLS48286_frob(&g[i], &X, 1);
    }

    for (i = 0; i < 16; i++)
    {
        np = BIG_288_29_nbits(u[i]);
        BIG_288_29_modneg(t, u[i], q);
        nn = BIG_288_29_nbits(t);
        if (nn < np)
        {
            BIG_288_29_copy(u[i], t);
            FP48_BLS48286_conj(&g[i], &g[i]);
        }
        BIG_288_29_norm(u[i]);
    }
    FP48_BLS48286_pow16(f, g, u);

#else
    BIG_288_29_copy(ee,e);
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
    BIG_288_29_mod(ee,q);
    FP48_BLS48286_pow(f, f, ee);
#endif
}

/* test G1 group membership */

int PAIR_BLS48286_G1member(ECP_BLS48286 *P)
{
    ECP_BLS48286 W,T;
    BIG_288_29 x;
    FP_BLS48286 cru;
    if (ECP_BLS48286_isinf(P)) return 0;

    BIG_288_29_rcopy(x, CURVE_Bnx_BLS48286);
    ECP_BLS48286_copy(&W,P);
    ECP_BLS48286_copy(&T,P);
    ECP_BLS48286_mul(&T,x); 
    if (ECP_BLS48286_equals(P,&T)) return 0;    // P is of low order        
    ECP_BLS48286_mul(&T,x);
    ECP_BLS48286_mul(&T,x); ECP_BLS48286_mul(&T,x);
    ECP_BLS48286_mul(&T,x); ECP_BLS48286_mul(&T,x);
    ECP_BLS48286_mul(&T,x); ECP_BLS48286_mul(&T,x);
    ECP_BLS48286_neg(&T);

    FP_BLS48286_rcopy(&cru, CRu_BLS48286);
    FP_BLS48286_mul(&(W.x), &(W.x), &cru);
    if (!ECP_BLS48286_equals(&W,&T)) return 0;  // check that Endomorphism works

// Not needed
//    ECP_BLS48286_add(&W,P);
//    FP_BLS48286_mul(&(T.x), &(T.x), &cru);
//    ECP_BLS48286_add(&W,&T);
//    if (!ECP_BLS48286_isinf(&W)) return 0;      // use it to check order
/*	BIG_288_29 q;
	ECP_BLS48286 W;
    if (ECP_BLS48286_isinf(P)) return 0;
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
	ECP_BLS48286_copy(&W,P);
	ECP_BLS48286_mul(&W,q);
	if (!ECP_BLS48286_isinf(&W)) return 0; */
	return 1;
}

/* test G2 group membership */

int PAIR_BLS48286_G2member(ECP8_BLS48286 *P)
{
    ECP8_BLS48286 W,T;
    BIG_288_29 x;
    FP2_BLS48286 X[3];

    if (ECP8_BLS48286_isinf(P)) return 0;
    ECP8_BLS48286_frob_constants(X);
    BIG_288_29_rcopy(x, CURVE_Bnx_BLS48286);

    ECP8_BLS48286_copy(&W,P);
    ECP8_BLS48286_frob(&W, X, 1);

    ECP8_BLS48286_copy(&T,P);
    ECP8_BLS48286_mul(&T,x);

#if SIGN_OF_X_BLS48286==NEGATIVEX
    ECP8_BLS48286_neg(&T);
#endif
/*
    ECP8_BLS48286_copy(&R,&W);
    ECP8_BLS48286_frob(&R,X,1);    // R=\psi^2(P)
    ECP8_BLS48286_sub(&W,&R);
    ECP8_BLS48286_copy(&R,&T);    // R=xP
    ECP8_BLS48286_frob(&R,X,1);
    ECP8_BLS48286_add(&W,&R);     // W=\psi(P)-\psi^2(P)+\psi(xP)
*/
    if (ECP8_BLS48286_equals(&W,&T)) return 1;
    return 0;
/*
	BIG_288_29 q;
	ECP8_BLS48286 W;
    if (ECP8_BLS48286_isinf(P)) return 0;
    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
	ECP8_BLS48286_copy(&W,P);
	ECP8_BLS48286_mul(&W,q);
	if (!ECP8_BLS48286_isinf(&W)) return 0;
	return 1;
*/
}

/* Check that m is in cyclotomic sub-group */
/* Check that m!=1, conj(m)*m==1, and m.m^{p^16}=m^{p^8} */
int PAIR_BLS48286_GTcyclotomic(FP48_BLS48286 *m)
{
	FP_BLS48286 fx,fy;
	FP2_BLS48286 X;
	FP48_BLS48286 r,w;
	if (FP48_BLS48286_isunity(m)) return 0;
	FP48_BLS48286_conj(&r,m);
	FP48_BLS48286_mul(&r,m);
	if (!FP48_BLS48286_isunity(&r)) return 0;

	FP_BLS48286_rcopy(&fx,Fra_BLS48286);
	FP_BLS48286_rcopy(&fy,Frb_BLS48286);
	FP2_BLS48286_from_FPs(&X,&fx,&fy);

	FP48_BLS48286_copy(&r,m); FP48_BLS48286_frob(&r,&X,8); 
	FP48_BLS48286_copy(&w,&r); FP48_BLS48286_frob(&w,&X,8); 
	FP48_BLS48286_mul(&w,m);

	if (!FP48_BLS48286_equals(&w,&r)) return 0;
    return 1;
}

/* test for full GT group membership */
int PAIR_BLS48286_GTmember(FP48_BLS48286 *m)
{
    BIG_288_29 x;
    FP2_BLS48286 X;
    FP_BLS48286 fx, fy;
    FP48_BLS48286 r,t;
    if (!PAIR_BLS48286_GTcyclotomic(m)) return 0;

    FP_BLS48286_rcopy(&fx, Fra_BLS48286);
    FP_BLS48286_rcopy(&fy, Frb_BLS48286);
    FP2_BLS48286_from_FPs(&X, &fx, &fy);
    BIG_288_29_rcopy(x, CURVE_Bnx_BLS48286);

    FP48_BLS48286_copy(&r,m);
    FP48_BLS48286_frob(&r, &X, 1);

    FP48_BLS48286_pow(&t,m,x);

#if SIGN_OF_X_BLS48286==NEGATIVEX
    FP48_BLS48286_conj(&t,&t);
#endif

    if (FP48_BLS48286_equals(&r,&t)) return 1;
    return 0;
/*
	BIG_288_29 q;
    FP48_BLS48286 r;
    if (!PAIR_BLS48286_GTcyclotomic(m)) return 0;

    BIG_288_29_rcopy(q, CURVE_Order_BLS48286);
	FP48_BLS48286_pow(&r,m,q);
	if (!FP48_BLS48286_isunity(&r)) return 0;
	return 1;
*/
}

#ifdef HAS_MAIN

using namespace std;
using namespace BLS48286;


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


    BIG_rcopy(xa, CURVE_Gx_BLS48286);
    BIG_rcopy(ya, CURVE_Gy_BLS48286);

    ECP_BLS48286_set(&Q, xa, ya);
    if (Q.inf) printf("Failed to set - point not on curve\n");
    else printf("G1 set success\n");

    printf("Q= ");
    ECP_BLS48286_output(&Q);
    printf("\n");

    ECP8_generator(&P);

    if (P.inf) printf("Failed to set - point not on curve\n");
    else printf("G2 set success\n");

    BIG_rcopy(a, Fra);
    BIG_rcopy(b, Frb);
    FP2_from_BIGs(&f, a, b);

    PAIR_BLS48286_ate(&g, &P, &Q);

    printf("gb= ");
    FP48_output(&g);
    printf("\n");
    PAIR_BLS48286_fexp(&g);

    printf("g= ");
    FP48_output(&g);
    printf("\n");

    ECP_BLS48286_copy(&R, &Q);
    ECP8_copy(&G, &P);

    ECP8_dbl(&G);
    ECP_dbl(&R);
    ECP_affine(&R);

    PAIR_BLS48286_ate(&g, &G, &Q);
    PAIR_BLS48286_fexp(&g);

    printf("g1= ");
    FP48_output(&g);
    printf("\n");

    PAIR_BLS48286_ate(&g, &P, &R);
    PAIR_BLS48286_fexp(&g);

    printf("g2= ");
    FP48_output(&g);
    printf("\n");


    PAIR_BLS48286_G1mul(&Q, r);
    printf("rQ= "); ECP_output(&Q); printf("\n");

    PAIR_BLS48286_G2mul(&P, r);
    printf("rP= "); ECP8_output(&P); printf("\n");

    PAIR_BLS48286_GTpow(&g, r);
    printf("g^r= "); FP48_output(&g); printf("\n");


    BIG_randomnum(w, r, &rng);

    FP48_copy(&gp, &g);

    PAIR_BLS48286_GTpow(&g, w);

    FP48_trace(&t, &g);

    printf("g^r=  "); FP16_output(&t); printf("\n");

    FP48_compow(&t, &gp, w, r);

    printf("t(g)= "); FP16_output(&t); printf("\n");

}

#endif

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

/**
 * @file pair.h
 * @author Mike Scott
 * @brief PAIR Header File
 *
 */

#ifndef PAIR_FP256BN_H
#define PAIR_FP256BN_H

#include "fp12_FP256BN.h"
#include "ecp2_FP256BN.h"
#include "ecp_FP256BN.h"

/* Pairing constants */

extern const BIG_256_28 CURVE_Bnx_FP256BN; /**< BN curve x parameter */
extern const BIG_256_28 CURVE_Cru_FP256BN; /**< BN curve Cube Root of Unity */

extern const BIG_256_28 CURVE_W_FP256BN[2];	 /**< BN curve constant for GLV decomposition */
extern const BIG_256_28 CURVE_SB_FP256BN[2][2]; /**< BN curve constant for GLV decomposition */
extern const BIG_256_28 CURVE_WB_FP256BN[4];	 /**< BN curve constant for GS decomposition */
extern const BIG_256_28 CURVE_BB_FP256BN[4][4]; /**< BN curve constant for GS decomposition */

/* Pairing function prototypes */

/**	@brief Precompute line functions details for fixed G2 value
 *
	@param T array of precomputed FP4 partial line functions
	@param GV a fixed ECP2 instance
 */
extern void PAIR_FP256BN_precomp(FP4_FP256BN T[], ECP2_FP256BN* GV);



/**	@brief Precompute line functions for n-pairing
 *
	@param r array of precomputed FP12 products of line functions
	@param PV ECP2 instance, an element of G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_FP256BN_another(FP12_FP256BN r[], ECP2_FP256BN* PV, ECP_FP256BN* QV);


/**	@brief Compute line functions for n-pairing, assuming precomputation on G2
 *
	@param r array of precomputed FP12 products of line functions
	@param T array contains precomputed partial line fucntions from G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_FP256BN_another_pc(FP12_FP256BN r[], FP4_FP256BN T[], ECP_FP256BN *QV);


/**	@brief Calculate Miller loop for Optimal ATE pairing e(P,Q)
 *
	@param r FP12 result of the pairing calculation e(P,Q)
	@param P ECP2 instance, an element of G2
	@param Q ECP instance, an element of G1

 */
extern void PAIR_FP256BN_ate(FP12_FP256BN *r, ECP2_FP256BN *P, ECP_FP256BN *Q);
/**	@brief Calculate Miller loop for Optimal ATE double-pairing e(P,Q).e(R,S)
 *
	Faster than calculating two separate pairings
	@param r FP12 result of the pairing calculation e(P,Q).e(R,S), an element of GT
	@param P ECP2 instance, an element of G2
	@param Q ECP instance, an element of G1
	@param R ECP2 instance, an element of G2
	@param S ECP instance, an element of G1
 */
extern void PAIR_FP256BN_double_ate(FP12_FP256BN *r, ECP2_FP256BN *P, ECP_FP256BN *Q, ECP2_FP256BN *R, ECP_FP256BN *S);
/**	@brief Final exponentiation of pairing, converts output of Miller loop to element in GT
 *
	Here p is the internal modulus, and r is the group order
	@param x FP12, on exit = x^((p^12-1)/r)
 */
extern void PAIR_FP256BN_fexp(FP12_FP256BN *x);
/**	@brief Fast point multiplication of a member of the group G1 by a BIG number
 *
	May exploit endomorphism for speed.
	@param Q ECP member of G1.
	@param b BIG multiplier

 */
extern void PAIR_FP256BN_G1mul(ECP_FP256BN *Q, BIG_256_28 b);
/**	@brief Fast point multiplication of a member of the group G2 by a BIG number
 *
	May exploit endomorphism for speed.
	@param P ECP2 member of G1.
	@param b BIG multiplier

 */
extern void PAIR_FP256BN_G2mul(ECP2_FP256BN *P, BIG_256_28 b);
/**	@brief Fast raising of a member of GT to a BIG power
 *
	May exploit endomorphism for speed.
	@param x FP12 member of GT.
	@param b BIG exponent

 */
extern void PAIR_FP256BN_GTpow(FP12_FP256BN *x, BIG_256_28 b);

/**	@brief Tests ECP for membership of G1
 *
	@param P ECP member of G1
	@return true or false

 */
extern int PAIR_FP256BN_G1member(ECP_FP256BN *P);

/**	@brief Tests ECP2 for membership of G2
 *
	@param P ECP2 member of G2
	@return true or false

 */
extern int PAIR_FP256BN_G2member(ECP2_FP256BN *P);

/**	@brief Tests FP12 for membership of cyclotomic sub-group
 *
	@param x FP12 instance
	@return 1 if x is cyclotomic, else return 0

 */
extern int PAIR_FP256BN_GTcyclotomic(FP12_FP256BN *x);

/**	@brief Tests FP12 for full membership of GT
 *
	@param x FP12 instance
	@return 1 if x is in GT, else return 0

 */
extern int PAIR_FP256BN_GTmember(FP12_FP256BN *x);

/**	@brief Prepare Ate parameter
 *
	@param n BIG parameter
	@param n3 BIG paramter = 3*n
	@return number of nits in n3

 */
extern int PAIR_FP256BN_nbits(BIG_256_28 n3, BIG_256_28 n);

/**	@brief Initialise structure for multi-pairing
 *
	@param r FP12 array, to be initialised to 1

 */
extern void PAIR_FP256BN_initmp(FP12_FP256BN r[]);


/**	@brief Miller loop
 *
 	@param res FP12 result
	@param r FP12 precomputed array of accumulated line functions

 */
extern void PAIR_FP256BN_miller(FP12_FP256BN *res, FP12_FP256BN r[]);

#endif

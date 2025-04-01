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
 * @file pair8.h
 * @author Mike Scott
 * @brief PAIR Header File
 *
 */

#ifndef PAIR8_BLS48556_H
#define PAIR8_BLS48556_H

#include "fp48_BLS48556.h"
#include "ecp8_BLS48556.h"
#include "ecp_BLS48556.h"


/* Pairing constants */

extern const BIG_560_29 CURVE_Bnx_BLS48556; /**< BN curve x parameter */
extern const BIG_560_29 CURVE_Cru_BLS48556; /**< BN curve Cube Root of Unity */

extern const BIG_560_29 CURVE_W_BLS48556[2];	 /**< BN curve constant for GLV decomposition */
extern const BIG_560_29 CURVE_SB_BLS48556[2][2]; /**< BN curve constant for GLV decomposition */
extern const BIG_560_29 CURVE_WB_BLS48556[4];	 /**< BN curve constant for GS decomposition */
extern const BIG_560_29 CURVE_BB_BLS48556[4][4]; /**< BN curve constant for GS decomposition */

/* Pairing function prototypes */

/**	@brief Precompute line functions details for fixed G2 value
 *
	@param T array of precomputed FP16 partial line functions
	@param GV a fixed ECP8 instance
 */
extern void PAIR_BLS48556_precomp(FP16_BLS48556 T[], ECP8_BLS48556* GV);


/**	@brief Precompute line functions for n-pairing
 *
	@param r array of precomputed FP48 products of line functions
	@param PV ECP8 instance, an element of G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_BLS48556_another(FP48_BLS48556 r[], ECP8_BLS48556* PV, ECP_BLS48556* QV);


/**	@brief Compute line functions for n-pairing, assuming precomputation on G2
 *
	@param r array of precomputed FP48 products of line functions
	@param T array contains precomputed partial line fucntions from G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_BLS48556_another_pc(FP48_BLS48556 r[], FP16_BLS48556 T[], ECP_BLS48556 *QV);

/**	@brief Calculate Miller loop for Optimal ATE pairing e(P,Q)
 *
	@param r FP48 result of the pairing calculation e(P,Q)
	@param P ECP8 instance, an element of G2
	@param Q ECP instance, an element of G1

 */
extern void PAIR_BLS48556_ate(FP48_BLS48556 *r, ECP8_BLS48556 *P, ECP_BLS48556 *Q);
/**	@brief Calculate Miller loop for Optimal ATE double-pairing e(P,Q).e(R,S)
 *
	Faster than calculating two separate pairings
	@param r FP48 result of the pairing calculation e(P,Q).e(R,S), an element of GT
	@param P ECP8 instance, an element of G2
	@param Q ECP instance, an element of G1
	@param R ECP8 instance, an element of G2
	@param S ECP instance, an element of G1
 */
extern void PAIR_BLS48556_double_ate(FP48_BLS48556 *r, ECP8_BLS48556 *P, ECP_BLS48556 *Q, ECP8_BLS48556 *R, ECP_BLS48556 *S);
/**	@brief Final exponentiation of pairing, converts output of Miller loop to element in GT
 *
	Here p is the internal modulus, and r is the group order
	@param x FP48, on exit = x^((p^12-1)/r)
 */
extern void PAIR_BLS48556_fexp(FP48_BLS48556 *x);
/**	@brief Fast point multiplication of a member of the group G1 by a BIG number
 *
	May exploit endomorphism for speed.
	@param Q ECP member of G1.
	@param b BIG multiplier

 */
extern void PAIR_BLS48556_G1mul(ECP_BLS48556 *Q, BIG_560_29 b);
/**	@brief Fast point multiplication of a member of the group G2 by a BIG number
 *
	May exploit endomorphism for speed.
	@param P ECP8 member of G1.
	@param b BIG multiplier

 */
extern void PAIR_BLS48556_G2mul(ECP8_BLS48556 *P, BIG_560_29 b);
/**	@brief Fast raising of a member of GT to a BIG power
 *
	May exploit endomorphism for speed.
	@param x FP48 member of GT.
	@param b BIG exponent

 */
extern void PAIR_BLS48556_GTpow(FP48_BLS48556 *x, BIG_560_29 b);

/**	@brief Tests ECP for membership of G1
 *
	@param P ECP member of G1
	@return true or false

 */
extern int PAIR_BLS48556_G1member(ECP_BLS48556 *P);

/**	@brief Tests ECP8 for membership of G2
 *
	@param P ECP8 member of G2
	@return true or false

 */
extern int PAIR_BLS48556_G2member(ECP8_BLS48556 *P);

/**	@brief Tests FP48 for membership of cyclotomic sub-group
 *
	@param x FP48 instance
	@return 1 if x is cyclotomic, else return 0

 */
extern int PAIR_BLS48556_GTcyclotomic(FP48_BLS48556 *x);

/**	@brief Tests FP48 for full membership of GT
 *
	@param x FP48 instance
	@return 1 if x is in GT, else return 0

 */
extern int PAIR_BLS48556_GTmember(FP48_BLS48556 *x);

/**	@brief Prepare Ate parameter
 *
	@param n BIG parameter
	@param n3 BIG paramter = 3*n
	@return number of nits in n3

 */
extern int PAIR_BLS48556_nbits(BIG_560_29 n3, BIG_560_29 n);

/**	@brief Initialise structure for multi-pairing
 *
	@param r FP48 array, to be initialised to 1

 */
extern void PAIR_BLS48556_initmp(FP48_BLS48556 r[]);


/**	@brief Miller loop
 *
 	@param res FP48 result
	@param r FP48 precomputed array of accumulated line functions

 */
extern void PAIR_BLS48556_miller(FP48_BLS48556 *res, FP48_BLS48556 r[]);
#endif

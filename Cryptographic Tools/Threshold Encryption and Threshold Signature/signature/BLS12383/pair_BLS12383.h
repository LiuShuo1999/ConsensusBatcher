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

#ifndef PAIR_BLS12383_H
#define PAIR_BLS12383_H

#include "fp12_BLS12383.h"
#include "ecp2_BLS12383.h"
#include "ecp_BLS12383.h"

/* Pairing constants */

extern const BIG_384_29 CURVE_Bnx_BLS12383; /**< BN curve x parameter */
extern const BIG_384_29 CURVE_Cru_BLS12383; /**< BN curve Cube Root of Unity */

extern const BIG_384_29 CURVE_W_BLS12383[2];	 /**< BN curve constant for GLV decomposition */
extern const BIG_384_29 CURVE_SB_BLS12383[2][2]; /**< BN curve constant for GLV decomposition */
extern const BIG_384_29 CURVE_WB_BLS12383[4];	 /**< BN curve constant for GS decomposition */
extern const BIG_384_29 CURVE_BB_BLS12383[4][4]; /**< BN curve constant for GS decomposition */

/* Pairing function prototypes */

/**	@brief Precompute line functions details for fixed G2 value
 *
	@param T array of precomputed FP4 partial line functions
	@param GV a fixed ECP2 instance
 */
extern void PAIR_BLS12383_precomp(FP4_BLS12383 T[], ECP2_BLS12383* GV);



/**	@brief Precompute line functions for n-pairing
 *
	@param r array of precomputed FP12 products of line functions
	@param PV ECP2 instance, an element of G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_BLS12383_another(FP12_BLS12383 r[], ECP2_BLS12383* PV, ECP_BLS12383* QV);


/**	@brief Compute line functions for n-pairing, assuming precomputation on G2
 *
	@param r array of precomputed FP12 products of line functions
	@param T array contains precomputed partial line fucntions from G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_BLS12383_another_pc(FP12_BLS12383 r[], FP4_BLS12383 T[], ECP_BLS12383 *QV);


/**	@brief Calculate Miller loop for Optimal ATE pairing e(P,Q)
 *
	@param r FP12 result of the pairing calculation e(P,Q)
	@param P ECP2 instance, an element of G2
	@param Q ECP instance, an element of G1

 */
extern void PAIR_BLS12383_ate(FP12_BLS12383 *r, ECP2_BLS12383 *P, ECP_BLS12383 *Q);
/**	@brief Calculate Miller loop for Optimal ATE double-pairing e(P,Q).e(R,S)
 *
	Faster than calculating two separate pairings
	@param r FP12 result of the pairing calculation e(P,Q).e(R,S), an element of GT
	@param P ECP2 instance, an element of G2
	@param Q ECP instance, an element of G1
	@param R ECP2 instance, an element of G2
	@param S ECP instance, an element of G1
 */
extern void PAIR_BLS12383_double_ate(FP12_BLS12383 *r, ECP2_BLS12383 *P, ECP_BLS12383 *Q, ECP2_BLS12383 *R, ECP_BLS12383 *S);
/**	@brief Final exponentiation of pairing, converts output of Miller loop to element in GT
 *
	Here p is the internal modulus, and r is the group order
	@param x FP12, on exit = x^((p^12-1)/r)
 */
extern void PAIR_BLS12383_fexp(FP12_BLS12383 *x);
/**	@brief Fast point multiplication of a member of the group G1 by a BIG number
 *
	May exploit endomorphism for speed.
	@param Q ECP member of G1.
	@param b BIG multiplier

 */
extern void PAIR_BLS12383_G1mul(ECP_BLS12383 *Q, BIG_384_29 b);
/**	@brief Fast point multiplication of a member of the group G2 by a BIG number
 *
	May exploit endomorphism for speed.
	@param P ECP2 member of G1.
	@param b BIG multiplier

 */
extern void PAIR_BLS12383_G2mul(ECP2_BLS12383 *P, BIG_384_29 b);
/**	@brief Fast raising of a member of GT to a BIG power
 *
	May exploit endomorphism for speed.
	@param x FP12 member of GT.
	@param b BIG exponent

 */
extern void PAIR_BLS12383_GTpow(FP12_BLS12383 *x, BIG_384_29 b);

/**	@brief Tests ECP for membership of G1
 *
	@param P ECP member of G1
	@return true or false

 */
extern int PAIR_BLS12383_G1member(ECP_BLS12383 *P);

/**	@brief Tests ECP2 for membership of G2
 *
	@param P ECP2 member of G2
	@return true or false

 */
extern int PAIR_BLS12383_G2member(ECP2_BLS12383 *P);

/**	@brief Tests FP12 for membership of cyclotomic sub-group
 *
	@param x FP12 instance
	@return 1 if x is cyclotomic, else return 0

 */
extern int PAIR_BLS12383_GTcyclotomic(FP12_BLS12383 *x);

/**	@brief Tests FP12 for full membership of GT
 *
	@param x FP12 instance
	@return 1 if x is in GT, else return 0

 */
extern int PAIR_BLS12383_GTmember(FP12_BLS12383 *x);

/**	@brief Prepare Ate parameter
 *
	@param n BIG parameter
	@param n3 BIG paramter = 3*n
	@return number of nits in n3

 */
extern int PAIR_BLS12383_nbits(BIG_384_29 n3, BIG_384_29 n);

/**	@brief Initialise structure for multi-pairing
 *
	@param r FP12 array, to be initialised to 1

 */
extern void PAIR_BLS12383_initmp(FP12_BLS12383 r[]);


/**	@brief Miller loop
 *
 	@param res FP12 result
	@param r FP12 precomputed array of accumulated line functions

 */
extern void PAIR_BLS12383_miller(FP12_BLS12383 *res, FP12_BLS12383 r[]);

#endif

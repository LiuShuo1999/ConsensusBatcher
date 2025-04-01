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
 * @file pair4.h
 * @author Mike Scott
 * @brief PAIR Header File
 *
 */

#ifndef PAIR4_BLS24479_H
#define PAIR4_BLS24479_H

#include "fp24_BLS24479.h"
#include "ecp4_BLS24479.h"
#include "ecp_BLS24479.h"


/* Pairing constants */

extern const BIG_480_29 CURVE_Bnx_BLS24479; /**< BN curve x parameter */
extern const BIG_480_29 CURVE_Cru_BLS24479; /**< BN curve Cube Root of Unity */

extern const BIG_480_29 CURVE_W_BLS24479[2];	 /**< BN curve constant for GLV decomposition */
extern const BIG_480_29 CURVE_SB_BLS24479[2][2]; /**< BN curve constant for GLV decomposition */
extern const BIG_480_29 CURVE_WB_BLS24479[4];	 /**< BN curve constant for GS decomposition */
extern const BIG_480_29 CURVE_BB_BLS24479[4][4]; /**< BN curve constant for GS decomposition */

/* Pairing function prototypes */

/**	@brief Precompute line functions details for fixed G2 value
 *
	@param T array of precomputed FP8 partial line functions
	@param GV a fixed ECP4 instance
 */
extern void PAIR_BLS24479_precomp(FP8_BLS24479 T[], ECP4_BLS24479* GV);


/**	@brief Precompute line functions for n-pairing
 *
	@param r array of precomputed FP24 products of line functions
	@param PV ECP4 instance, an element of G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_BLS24479_another(FP24_BLS24479 r[], ECP4_BLS24479* PV, ECP_BLS24479* QV);

/**	@brief Compute line functions for n-pairing, assuming precomputation on G2
 *
	@param r array of precomputed FP24 products of line functions
	@param T array contains precomputed partial line fucntions from G2
	@param QV ECP instance, an element of G1

 */
extern void PAIR_BLS24479_another_pc(FP24_BLS24479 r[], FP8_BLS24479 T[], ECP_BLS24479 *QV);


/**	@brief Calculate Miller loop for Optimal ATE pairing e(P,Q)
 *
	@param r FP24 result of the pairing calculation e(P,Q)
	@param P ECP4 instance, an element of G2
	@param Q ECP instance, an element of G1

 */
extern void PAIR_BLS24479_ate(FP24_BLS24479 *r, ECP4_BLS24479 *P, ECP_BLS24479 *Q);
/**	@brief Calculate Miller loop for Optimal ATE double-pairing e(P,Q).e(R,S)
 *
	Faster than calculating two separate pairings
	@param r FP24 result of the pairing calculation e(P,Q).e(R,S), an element of GT
	@param P ECP4 instance, an element of G2
	@param Q ECP instance, an element of G1
	@param R ECP4 instance, an element of G2
	@param S ECP instance, an element of G1
 */
extern void PAIR_BLS24479_double_ate(FP24_BLS24479 *r, ECP4_BLS24479 *P, ECP_BLS24479 *Q, ECP4_BLS24479 *R, ECP_BLS24479 *S);
/**	@brief Final exponentiation of pairing, converts output of Miller loop to element in GT
 *
	Here p is the internal modulus, and r is the group order
	@param x FP24, on exit = x^((p^12-1)/r)
 */
extern void PAIR_BLS24479_fexp(FP24_BLS24479 *x);
/**	@brief Fast point multiplication of a member of the group G1 by a BIG number
 *
	May exploit endomorphism for speed.
	@param Q ECP member of G1.
	@param b BIG multiplier

 */
extern void PAIR_BLS24479_G1mul(ECP_BLS24479 *Q, BIG_480_29 b);
/**	@brief Fast point multiplication of a member of the group G2 by a BIG number
 *
	May exploit endomorphism for speed.
	@param P ECP4 member of G1.
	@param b BIG multiplier

 */
extern void PAIR_BLS24479_G2mul(ECP4_BLS24479 *P, BIG_480_29 b);
/**	@brief Fast raising of a member of GT to a BIG power
 *
	May exploit endomorphism for speed.
	@param x FP24 member of GT.
	@param b BIG exponent

 */
extern void PAIR_BLS24479_GTpow(FP24_BLS24479 *x, BIG_480_29 b);

/**	@brief Tests ECP for membership of G1
 *
	@param P ECP member of G1
	@return true or false

 */
extern int PAIR_BLS24479_G1member(ECP_BLS24479 *P);

/**	@brief Tests ECP4 for membership of G2
 *
	@param P ECP4 member of G2
	@return true or false

 */
extern int PAIR_BLS24479_G2member(ECP4_BLS24479 *P);

/**	@brief Tests FP24 for membership of cyclotomic sub-group
 *
	@param x FP24 instance
	@return 1 if x is cyclotomic, else return 0

 */
extern int PAIR_BLS24479_GTcyclotomic(FP24_BLS24479 *x);


/**	@brief Tests FP24 for full membership of GT
 *
	@param x FP24 instance
	@return 1 if x is in GT, else return 0

 */
extern int PAIR_BLS24479_GTmember(FP24_BLS24479 *x);

/**	@brief Prepare Ate parameter
 *
	@param n BIG parameter
	@param n3 BIG paramter = 3*n
	@return number of nits in n3

 */
extern int PAIR_BLS24479_nbits(BIG_480_29 n3, BIG_480_29 n);

/**	@brief Initialise structure for multi-pairing
 *
	@param r FP24 array, to be initialised to 1

 */
extern void PAIR_BLS24479_initmp(FP24_BLS24479 r[]);


/**	@brief Miller loop
 *
 	@param res FP24 result
	@param r FP24 precomputed array of accumulated line functions

 */
extern void PAIR_BLS24479_miller(FP24_BLS24479 *res, FP24_BLS24479 r[]);


#endif

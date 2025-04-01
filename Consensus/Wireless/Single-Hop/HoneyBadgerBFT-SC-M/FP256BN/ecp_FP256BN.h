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
 * @file ecp.h
 * @author Mike Scott
 * @brief ECP Header File
 *
 */

#ifndef ECP_FP256BN_H
#define ECP_FP256BN_H

#include "fp_FP256BN.h"
#include "config_curve_FP256BN.h"

/* Curve Params - see rom_zzz.c */
extern const int CURVE_Cof_I_FP256BN;     /**< Elliptic curve cofactor */
extern const int CURVE_B_I_FP256BN;       /**< Elliptic curve B_i parameter */
extern const BIG_256_28 CURVE_B_FP256BN;     /**< Elliptic curve B parameter */
extern const BIG_256_28 CURVE_Order_FP256BN; /**< Elliptic curve group order */
extern const BIG_256_28 CURVE_Cof_FP256BN;   /**< Elliptic curve cofactor */
extern const BIG_256_28 CURVE_HTPC_FP256BN;  /**< Hash to Point precomputation */
extern const BIG_256_28 CURVE_HTPC2_FP256BN;  /**< Hash to Point precomputation for G2 */ 

extern const BIG_256_28 CURVE_Ad_FP256BN;      /**< A parameter of isogenous curve */
extern const BIG_256_28 CURVE_Bd_FP256BN;      /**< B parameter of isogenous curve */
extern const BIG_256_28 PC_FP256BN[];          /**< Precomputed isogenies  */

extern const BIG_256_28 CURVE_Adr_FP256BN;     /**< Real part of A parameter of isogenous curve in G2 */
extern const BIG_256_28 CURVE_Adi_FP256BN;     /**< Imaginary part of A parameter of isogenous curve in G2 */
extern const BIG_256_28 CURVE_Bdr_FP256BN;     /**< Real part of B parameter of isogenous curve in G2 */
extern const BIG_256_28 CURVE_Bdi_FP256BN;     /**< Imaginary part of B parameter of isogenous curve in G2 */
extern const BIG_256_28 PCR_FP256BN[];         /**< Real parts of precomputed isogenies */
extern const BIG_256_28 PCI_FP256BN[];         /**< Imaginary parts of precomputed isogenies */

/* Generator point on G1 */
extern const BIG_256_28 CURVE_Gx_FP256BN; /**< x-coordinate of generator point in group G1  */
extern const BIG_256_28 CURVE_Gy_FP256BN; /**< y-coordinate of generator point in group G1  */


/* For Pairings only */

/* Generator point on G2 */
extern const BIG_256_28 CURVE_Pxa_FP256BN; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxb_FP256BN; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pya_FP256BN; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pyb_FP256BN; /**< imaginary part of y-coordinate of generator point in group G2 */


/*** needed for BLS24 curves ***/

extern const BIG_256_28 CURVE_Pxaa_FP256BN; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxab_FP256BN; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxba_FP256BN; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxbb_FP256BN; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pyaa_FP256BN; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pyab_FP256BN; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pyba_FP256BN; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pybb_FP256BN; /**< imaginary part of y-coordinate of generator point in group G2 */

/*** needed for BLS48 curves ***/

extern const BIG_256_28 CURVE_Pxaaa_FP256BN; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxaab_FP256BN; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxaba_FP256BN; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxabb_FP256BN; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxbaa_FP256BN; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxbab_FP256BN; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxbba_FP256BN; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pxbbb_FP256BN; /**< imaginary part of x-coordinate of generator point in group G2 */

extern const BIG_256_28 CURVE_Pyaaa_FP256BN; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pyaab_FP256BN; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pyaba_FP256BN; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pyabb_FP256BN; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pybaa_FP256BN; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pybab_FP256BN; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pybba_FP256BN; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_256_28 CURVE_Pybbb_FP256BN; /**< imaginary part of y-coordinate of generator point in group G2 */


extern const BIG_256_28 CURVE_Bnx_FP256BN; /**< BN curve x parameter */



extern const BIG_256_28 Fra_FP256BN; /**< real part of BN curve Frobenius Constant */
extern const BIG_256_28 Frb_FP256BN; /**< imaginary part of BN curve Frobenius Constant */


extern const BIG_256_28 CURVE_W_FP256BN[2];	 /**< BN curve constant for GLV decomposition */
extern const BIG_256_28 CURVE_SB_FP256BN[2][2]; /**< BN curve constant for GLV decomposition */
extern const BIG_256_28 CURVE_WB_FP256BN[4];	 /**< BN curve constant for GS decomposition */
extern const BIG_256_28 CURVE_BB_FP256BN[4][4]; /**< BN curve constant for GS decomposition */


/**
	@brief ECP structure - Elliptic Curve Point over base field
*/

typedef struct
{
//    int inf; /**< Infinity Flag - not needed for Edwards representation */

    FP_FP256BN x; /**< x-coordinate of point */
#if CURVETYPE_FP256BN!=MONTGOMERY
    FP_FP256BN y; /**< y-coordinate of point. Not needed for Montgomery representation */
#endif
    FP_FP256BN z;/**< z-coordinate of point */
} ECP_FP256BN;


/* ECP E(Fp) prototypes */
/**	@brief Tests for ECP point equal to infinity
 *
	@param P ECP point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP_FP256BN_isinf(ECP_FP256BN *P);
/**	@brief Tests for equality of two ECPs
 *
	@param P ECP instance to be compared
	@param Q ECP instance to be compared
	@return 1 if P=Q, else returns 0
 */
extern int ECP_FP256BN_equals(ECP_FP256BN *P, ECP_FP256BN *Q);
/**	@brief Copy ECP point to another ECP point
 *
	@param P ECP instance, on exit = Q
	@param Q ECP instance to be copied
 */
extern void ECP_FP256BN_copy(ECP_FP256BN *P, ECP_FP256BN *Q);
/**	@brief Negation of an ECP point
 *
	@param P ECP instance, on exit = -P
 */
extern void ECP_FP256BN_neg(ECP_FP256BN *P);
/**	@brief Set ECP to point-at-infinity
 *
	@param P ECP instance to be set to infinity
 */
extern void ECP_FP256BN_inf(ECP_FP256BN *P);
/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x) depends on form of elliptic curve, Weierstrass, Edwards or Montgomery.
	Used internally.
	@param r BIG n-residue value of f(x)
	@param x BIG n-residue x
 */
extern void ECP_FP256BN_rhs(FP_FP256BN *r, FP_FP256BN *x);

#if CURVETYPE_FP256BN==MONTGOMERY
/**	@brief Set ECP to point(x,[y]) given x
 *
	Point P set to infinity if no such point on the curve. Note that y coordinate is not needed.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP_FP256BN_set(ECP_FP256BN *P, BIG_256_28 x);
/**	@brief Extract x coordinate of an ECP point P
 *
	@param x BIG on exit = x coordinate of point
	@param P ECP instance (x,[y])
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP_FP256BN_get(BIG_256_28 x, ECP_FP256BN *P);
/**	@brief Adds ECP instance Q to ECP instance P, given difference D=P-Q
 *
	Differential addition of points on a Montgomery curve
	@param P ECP instance, on exit =P+Q
	@param Q ECP instance to be added to P
	@param D Difference between P and Q
 */
extern void ECP_FP256BN_add(ECP_FP256BN *P, ECP_FP256BN *Q, ECP_FP256BN *D);
#else
/**	@brief Set ECP to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP instance to be set (x,y)
	@param x BIG x coordinate of point
	@param y BIG y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP_FP256BN_set(ECP_FP256BN *P, BIG_256_28 x, BIG_256_28 y);
/**	@brief Extract x and y coordinates of an ECP point P
 *
	If x=y, returns only x
	@param x BIG on exit = x coordinate of point
	@param y BIG on exit = y coordinate of point (unless x=y)
	@param P ECP instance (x,y)
	@return sign of y, or -1 if P is point-at-infinity
 */
extern int ECP_FP256BN_get(BIG_256_28 x, BIG_256_28 y, ECP_FP256BN *P);
/**	@brief Adds ECP instance Q to ECP instance P
 *
	@param P ECP instance, on exit =P+Q
	@param Q ECP instance to be added to P
 */
extern void ECP_FP256BN_add(ECP_FP256BN *P, ECP_FP256BN *Q);
/**	@brief Subtracts ECP instance Q from ECP instance P
 *
	@param P ECP instance, on exit =P-Q
	@param Q ECP instance to be subtracted from P
 */
extern void ECP_FP256BN_sub(ECP_FP256BN *P, ECP_FP256BN *Q);
/**	@brief Set ECP to point(x,y) given just x and sign of y
 *
	Point P set to infinity if no such point on the curve. If x is on the curve then y is calculated from the curve equation.
	The correct y value (plus or minus) is selected given its sign s.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
	@param s an integer representing the "sign" of y, in fact its least significant bit.
 */
extern int ECP_FP256BN_setx(ECP_FP256BN *P, BIG_256_28 x, int s);

#endif

/**	@brief Multiplies Point by curve co-factor
 *
	@param Q ECP instance
 */
extern void ECP_FP256BN_cfp(ECP_FP256BN *Q);


/**	@brief Maps random BIG to curve point in constant time
 *
	@param Q ECP instance 
	@param x FP derived from hash
 */
extern void ECP_FP256BN_map2point(ECP_FP256BN *Q, FP_FP256BN *x);

/**	@brief Maps random BIG to curve point using hunt-and-peck
 *
	@param Q ECP instance 
	@param x Fp derived from hash
 */
extern void ECP_FP256BN_hap2point(ECP_FP256BN *Q, BIG_256_28  x);


/**	@brief Maps random octet to curve point of correct order
 *
	@param Q ECP instance of correct order
	@param w OCTET byte array to be mapped
 */
extern void ECP_FP256BN_mapit(ECP_FP256BN *Q, octet *w);

/**	@brief Converts an ECP point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP instance to be converted to affine form
 */
extern void ECP_FP256BN_affine(ECP_FP256BN *P);
/**	@brief Formats and outputs an ECP point to the console, in projective coordinates
 *
	@param P ECP instance to be printed
 */
extern void ECP_FP256BN_outputxyz(ECP_FP256BN *P);
/**	@brief Formats and outputs an ECP point to the console, converted to affine coordinates
 *
	@param P ECP instance to be printed
 */
extern void ECP_FP256BN_output(ECP_FP256BN * P);

/**	@brief Formats and outputs an ECP point to the console
 *
	@param P ECP instance to be printed
 */
extern void ECP_FP256BN_rawoutput(ECP_FP256BN * P);

/**	@brief Formats and outputs an ECP point to an octet string
	The octet string is normally in the standard form 0x04|x|y
	Here x (and y) are the x and y coordinates in left justified big-endian base 256 form.
	For Montgomery curve it is 0x06|x
	If c is true, only the x coordinate is provided as in 0x2|x if y is even, or 0x3|x if y is odd
	@param c compression required, true or false
	@param S output octet string
	@param P ECP instance to be converted to an octet string
 */
extern void ECP_FP256BN_toOctet(octet *S, ECP_FP256BN *P, bool c);
/**	@brief Creates an ECP point from an octet string
 *
	The octet string is normally in the standard form 0x04|x|y
	Here x (and y) are the x and y coordinates in left justified big-endian base 256 form.
	For Montgomery curve it is 0x06|x
	If in compressed form only the x coordinate is provided as in 0x2|x if y is even, or 0x3|x if y is odd
	@param P ECP instance to be created from the octet string
	@param S input octet string
	return 1 if octet string corresponds to a point on the curve, else 0
 */
extern int ECP_FP256BN_fromOctet(ECP_FP256BN *P, octet *S);
/**	@brief Doubles an ECP instance P
 *
	@param P ECP instance, on exit =2*P
 */
extern void ECP_FP256BN_dbl(ECP_FP256BN *P);
/**	@brief Multiplies an ECP instance P by a small integer, side-channel resistant
 *
	@param P ECP instance, on exit =i*P
	@param i small integer multiplier
	@param b maximum number of bits in multiplier
 */
extern void ECP_FP256BN_pinmul(ECP_FP256BN *P, int i, int b);

/**	@brief Multiplies an ECP instance P by a BIG, side-channel resistant
 *
	Uses Montgomery ladder for Montgomery curves, otherwise fixed sized windows.
	@param P ECP instance, on exit =b*P
	@param e BIG number multiplier
    @param maxe maximum e

 */
extern void ECP_FP256BN_clmul(ECP_FP256BN *P, BIG_256_28 e, BIG_256_28 maxe);

/**	@brief Multiplies an ECP instance P by a BIG
 *
	Uses Montgomery ladder for Montgomery curves, otherwise fixed sized windows.
	@param P ECP instance, on exit =b*P
	@param b BIG number multiplier

 */
extern void ECP_FP256BN_mul(ECP_FP256BN *P, BIG_256_28 b);
/**	@brief Calculates double multiplication P=e*P+f*Q, side-channel resistant
 *
	@param P ECP instance, on exit =e*P+f*Q
	@param Q ECP instance
	@param e BIG number multiplier
	@param f BIG number multiplier
 */
extern void ECP_FP256BN_mul2(ECP_FP256BN *P, ECP_FP256BN *Q, BIG_256_28 e, BIG_256_28 f);

/**	@brief Calculates multi-multiplication P=Sigma e_i*X_i, side-channel resistant
 *
	@param P ECP instance, on exit = Sigma e_i*X_i
    @param n Number of multiplications
	@param X array of n ECPs
	@param e array of n BIG multipliers
 */
extern void ECP_FP256BN_muln(ECP_FP256BN *P,int n,ECP_FP256BN X[],BIG_256_28 e[]);


/**	@brief Get Group Generator from ROM
 *
	@param G ECP instance
    @return success
 */
extern int ECP_FP256BN_generator(ECP_FP256BN *G);


#endif

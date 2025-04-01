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

#ifndef ECP_BLS24479_H
#define ECP_BLS24479_H

#include "fp_BLS24479.h"
#include "config_curve_BLS24479.h"

/* Curve Params - see rom_zzz.c */
extern const int CURVE_Cof_I_BLS24479;     /**< Elliptic curve cofactor */
extern const int CURVE_B_I_BLS24479;       /**< Elliptic curve B_i parameter */
extern const BIG_480_29 CURVE_B_BLS24479;     /**< Elliptic curve B parameter */
extern const BIG_480_29 CURVE_Order_BLS24479; /**< Elliptic curve group order */
extern const BIG_480_29 CURVE_Cof_BLS24479;   /**< Elliptic curve cofactor */
extern const BIG_480_29 CURVE_HTPC_BLS24479;  /**< Hash to Point precomputation */
extern const BIG_480_29 CURVE_HTPC2_BLS24479;  /**< Hash to Point precomputation for G2 */ 

extern const BIG_480_29 CURVE_Ad_BLS24479;      /**< A parameter of isogenous curve */
extern const BIG_480_29 CURVE_Bd_BLS24479;      /**< B parameter of isogenous curve */
extern const BIG_480_29 PC_BLS24479[];          /**< Precomputed isogenies  */

extern const BIG_480_29 CURVE_Adr_BLS24479;     /**< Real part of A parameter of isogenous curve in G2 */
extern const BIG_480_29 CURVE_Adi_BLS24479;     /**< Imaginary part of A parameter of isogenous curve in G2 */
extern const BIG_480_29 CURVE_Bdr_BLS24479;     /**< Real part of B parameter of isogenous curve in G2 */
extern const BIG_480_29 CURVE_Bdi_BLS24479;     /**< Imaginary part of B parameter of isogenous curve in G2 */
extern const BIG_480_29 PCR_BLS24479[];         /**< Real parts of precomputed isogenies */
extern const BIG_480_29 PCI_BLS24479[];         /**< Imaginary parts of precomputed isogenies */

/* Generator point on G1 */
extern const BIG_480_29 CURVE_Gx_BLS24479; /**< x-coordinate of generator point in group G1  */
extern const BIG_480_29 CURVE_Gy_BLS24479; /**< y-coordinate of generator point in group G1  */


/* For Pairings only */

/* Generator point on G2 */
extern const BIG_480_29 CURVE_Pxa_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxb_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pya_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyb_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */


/*** needed for BLS24 curves ***/

extern const BIG_480_29 CURVE_Pxaa_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxab_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxba_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxbb_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyaa_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyab_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyba_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pybb_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */

/*** needed for BLS48 curves ***/

extern const BIG_480_29 CURVE_Pxaaa_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxaab_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxaba_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxabb_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxbaa_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxbab_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxbba_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxbbb_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */

extern const BIG_480_29 CURVE_Pyaaa_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyaab_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyaba_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyabb_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pybaa_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pybab_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pybba_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pybbb_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */


extern const BIG_480_29 CURVE_Bnx_BLS24479; /**< BN curve x parameter */



extern const BIG_480_29 Fra_BLS24479; /**< real part of BN curve Frobenius Constant */
extern const BIG_480_29 Frb_BLS24479; /**< imaginary part of BN curve Frobenius Constant */


extern const BIG_480_29 CURVE_W_BLS24479[2];	 /**< BN curve constant for GLV decomposition */
extern const BIG_480_29 CURVE_SB_BLS24479[2][2]; /**< BN curve constant for GLV decomposition */
extern const BIG_480_29 CURVE_WB_BLS24479[4];	 /**< BN curve constant for GS decomposition */
extern const BIG_480_29 CURVE_BB_BLS24479[4][4]; /**< BN curve constant for GS decomposition */


/**
	@brief ECP structure - Elliptic Curve Point over base field
*/

typedef struct
{
//    int inf; /**< Infinity Flag - not needed for Edwards representation */

    FP_BLS24479 x; /**< x-coordinate of point */
#if CURVETYPE_BLS24479!=MONTGOMERY
    FP_BLS24479 y; /**< y-coordinate of point. Not needed for Montgomery representation */
#endif
    FP_BLS24479 z;/**< z-coordinate of point */
} ECP_BLS24479;


/* ECP E(Fp) prototypes */
/**	@brief Tests for ECP point equal to infinity
 *
	@param P ECP point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP_BLS24479_isinf(ECP_BLS24479 *P);
/**	@brief Tests for equality of two ECPs
 *
	@param P ECP instance to be compared
	@param Q ECP instance to be compared
	@return 1 if P=Q, else returns 0
 */
extern int ECP_BLS24479_equals(ECP_BLS24479 *P, ECP_BLS24479 *Q);
/**	@brief Copy ECP point to another ECP point
 *
	@param P ECP instance, on exit = Q
	@param Q ECP instance to be copied
 */
extern void ECP_BLS24479_copy(ECP_BLS24479 *P, ECP_BLS24479 *Q);
/**	@brief Negation of an ECP point
 *
	@param P ECP instance, on exit = -P
 */
extern void ECP_BLS24479_neg(ECP_BLS24479 *P);
/**	@brief Set ECP to point-at-infinity
 *
	@param P ECP instance to be set to infinity
 */
extern void ECP_BLS24479_inf(ECP_BLS24479 *P);
/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x) depends on form of elliptic curve, Weierstrass, Edwards or Montgomery.
	Used internally.
	@param r BIG n-residue value of f(x)
	@param x BIG n-residue x
 */
extern void ECP_BLS24479_rhs(FP_BLS24479 *r, FP_BLS24479 *x);

#if CURVETYPE_BLS24479==MONTGOMERY
/**	@brief Set ECP to point(x,[y]) given x
 *
	Point P set to infinity if no such point on the curve. Note that y coordinate is not needed.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP_BLS24479_set(ECP_BLS24479 *P, BIG_480_29 x);
/**	@brief Extract x coordinate of an ECP point P
 *
	@param x BIG on exit = x coordinate of point
	@param P ECP instance (x,[y])
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP_BLS24479_get(BIG_480_29 x, ECP_BLS24479 *P);
/**	@brief Adds ECP instance Q to ECP instance P, given difference D=P-Q
 *
	Differential addition of points on a Montgomery curve
	@param P ECP instance, on exit =P+Q
	@param Q ECP instance to be added to P
	@param D Difference between P and Q
 */
extern void ECP_BLS24479_add(ECP_BLS24479 *P, ECP_BLS24479 *Q, ECP_BLS24479 *D);
#else
/**	@brief Set ECP to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP instance to be set (x,y)
	@param x BIG x coordinate of point
	@param y BIG y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP_BLS24479_set(ECP_BLS24479 *P, BIG_480_29 x, BIG_480_29 y);
/**	@brief Extract x and y coordinates of an ECP point P
 *
	If x=y, returns only x
	@param x BIG on exit = x coordinate of point
	@param y BIG on exit = y coordinate of point (unless x=y)
	@param P ECP instance (x,y)
	@return sign of y, or -1 if P is point-at-infinity
 */
extern int ECP_BLS24479_get(BIG_480_29 x, BIG_480_29 y, ECP_BLS24479 *P);
/**	@brief Adds ECP instance Q to ECP instance P
 *
	@param P ECP instance, on exit =P+Q
	@param Q ECP instance to be added to P
 */
extern void ECP_BLS24479_add(ECP_BLS24479 *P, ECP_BLS24479 *Q);
/**	@brief Subtracts ECP instance Q from ECP instance P
 *
	@param P ECP instance, on exit =P-Q
	@param Q ECP instance to be subtracted from P
 */
extern void ECP_BLS24479_sub(ECP_BLS24479 *P, ECP_BLS24479 *Q);
/**	@brief Set ECP to point(x,y) given just x and sign of y
 *
	Point P set to infinity if no such point on the curve. If x is on the curve then y is calculated from the curve equation.
	The correct y value (plus or minus) is selected given its sign s.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
	@param s an integer representing the "sign" of y, in fact its least significant bit.
 */
extern int ECP_BLS24479_setx(ECP_BLS24479 *P, BIG_480_29 x, int s);

#endif

/**	@brief Multiplies Point by curve co-factor
 *
	@param Q ECP instance
 */
extern void ECP_BLS24479_cfp(ECP_BLS24479 *Q);


/**	@brief Maps random BIG to curve point in constant time
 *
	@param Q ECP instance 
	@param x FP derived from hash
 */
extern void ECP_BLS24479_map2point(ECP_BLS24479 *Q, FP_BLS24479 *x);

/**	@brief Maps random BIG to curve point using hunt-and-peck
 *
	@param Q ECP instance 
	@param x Fp derived from hash
 */
extern void ECP_BLS24479_hap2point(ECP_BLS24479 *Q, BIG_480_29  x);


/**	@brief Maps random octet to curve point of correct order
 *
	@param Q ECP instance of correct order
	@param w OCTET byte array to be mapped
 */
extern void ECP_BLS24479_mapit(ECP_BLS24479 *Q, octet *w);

/**	@brief Converts an ECP point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP instance to be converted to affine form
 */
extern void ECP_BLS24479_affine(ECP_BLS24479 *P);
/**	@brief Formats and outputs an ECP point to the console, in projective coordinates
 *
	@param P ECP instance to be printed
 */
extern void ECP_BLS24479_outputxyz(ECP_BLS24479 *P);
/**	@brief Formats and outputs an ECP point to the console, converted to affine coordinates
 *
	@param P ECP instance to be printed
 */
extern void ECP_BLS24479_output(ECP_BLS24479 * P);

/**	@brief Formats and outputs an ECP point to the console
 *
	@param P ECP instance to be printed
 */
extern void ECP_BLS24479_rawoutput(ECP_BLS24479 * P);

/**	@brief Formats and outputs an ECP point to an octet string
	The octet string is normally in the standard form 0x04|x|y
	Here x (and y) are the x and y coordinates in left justified big-endian base 256 form.
	For Montgomery curve it is 0x06|x
	If c is true, only the x coordinate is provided as in 0x2|x if y is even, or 0x3|x if y is odd
	@param c compression required, true or false
	@param S output octet string
	@param P ECP instance to be converted to an octet string
 */
extern void ECP_BLS24479_toOctet(octet *S, ECP_BLS24479 *P, bool c);
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
extern int ECP_BLS24479_fromOctet(ECP_BLS24479 *P, octet *S);
/**	@brief Doubles an ECP instance P
 *
	@param P ECP instance, on exit =2*P
 */
extern void ECP_BLS24479_dbl(ECP_BLS24479 *P);
/**	@brief Multiplies an ECP instance P by a small integer, side-channel resistant
 *
	@param P ECP instance, on exit =i*P
	@param i small integer multiplier
	@param b maximum number of bits in multiplier
 */
extern void ECP_BLS24479_pinmul(ECP_BLS24479 *P, int i, int b);

/**	@brief Multiplies an ECP instance P by a BIG, side-channel resistant
 *
	Uses Montgomery ladder for Montgomery curves, otherwise fixed sized windows.
	@param P ECP instance, on exit =b*P
	@param e BIG number multiplier
    @param maxe maximum e

 */
extern void ECP_BLS24479_clmul(ECP_BLS24479 *P, BIG_480_29 e, BIG_480_29 maxe);

/**	@brief Multiplies an ECP instance P by a BIG
 *
	Uses Montgomery ladder for Montgomery curves, otherwise fixed sized windows.
	@param P ECP instance, on exit =b*P
	@param b BIG number multiplier

 */
extern void ECP_BLS24479_mul(ECP_BLS24479 *P, BIG_480_29 b);
/**	@brief Calculates double multiplication P=e*P+f*Q, side-channel resistant
 *
	@param P ECP instance, on exit =e*P+f*Q
	@param Q ECP instance
	@param e BIG number multiplier
	@param f BIG number multiplier
 */
extern void ECP_BLS24479_mul2(ECP_BLS24479 *P, ECP_BLS24479 *Q, BIG_480_29 e, BIG_480_29 f);

/**	@brief Calculates multi-multiplication P=Sigma e_i*X_i, side-channel resistant
 *
	@param P ECP instance, on exit = Sigma e_i*X_i
    @param n Number of multiplications
	@param X array of n ECPs
	@param e array of n BIG multipliers
 */
extern void ECP_BLS24479_muln(ECP_BLS24479 *P,int n,ECP_BLS24479 X[],BIG_480_29 e[]);


/**	@brief Get Group Generator from ROM
 *
	@param G ECP instance
    @return success
 */
extern int ECP_BLS24479_generator(ECP_BLS24479 *G);


#endif

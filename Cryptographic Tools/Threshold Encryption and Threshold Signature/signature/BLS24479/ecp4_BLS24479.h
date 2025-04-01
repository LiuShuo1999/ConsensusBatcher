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
 * @file ecp4.h
 * @author Mike Scott
 * @brief ECP2 Header File
 *
 */

#ifndef ECP4_BLS24479_H
#define ECP4_BLS24479_H

#include "fp4_BLS24479.h"
#include "config_curve_BLS24479.h"


/**
	@brief ECP4 Structure - Elliptic Curve Point over quadratic extension field
*/

typedef struct
{
//   int inf; /**< Infinity Flag */
    FP4_BLS24479 x;  /**< x-coordinate of point */
    FP4_BLS24479 y;  /**< y-coordinate of point */
    FP4_BLS24479 z;  /**< z-coordinate of point */
} ECP4_BLS24479;


/* Curve Params - see rom.c */

extern const int CURVE_B_I_BLS24479;		/**< Elliptic curve B parameter */
extern const BIG_480_29 CURVE_B_BLS24479;     /**< Elliptic curve B parameter */
extern const BIG_480_29 CURVE_Order_BLS24479; /**< Elliptic curve group order */
extern const BIG_480_29 CURVE_Cof_BLS24479;   /**< Elliptic curve cofactor */
extern const BIG_480_29 CURVE_Bnx_BLS24479;   /**< Elliptic curve parameter */
extern const BIG_480_29 CURVE_HTPC_BLS24479;  /**< Hash to Point precomputation */

extern const BIG_480_29 Fra_BLS24479; /**< real part of curve Frobenius Constant */
extern const BIG_480_29 Frb_BLS24479; /**< imaginary part of curve Frobenius Constant */

/* Generator point on G1 */
extern const BIG_480_29 CURVE_Gx_BLS24479; /**< x-coordinate of generator point in group G1  */
extern const BIG_480_29 CURVE_Gy_BLS24479; /**< y-coordinate of generator point in group G1  */

/* For Pairings only */

/* Generator point on G2 */
extern const BIG_480_29 CURVE_Pxaa_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxab_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxba_BLS24479; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pxbb_BLS24479; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyaa_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyab_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pyba_BLS24479; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_480_29 CURVE_Pybb_BLS24479; /**< imaginary part of y-coordinate of generator point in group G2 */

/* ECP4 E(FP4) prototypes */
/**	@brief Tests for ECP4 point equal to infinity
 *
	@param P ECP4 point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP4_BLS24479_isinf(ECP4_BLS24479 *P);
/**	@brief Copy ECP4 point to another ECP4 point
 *
	@param P ECP4 instance, on exit = Q
	@param Q ECP4 instance to be copied
 */
extern void ECP4_BLS24479_copy(ECP4_BLS24479 *P, ECP4_BLS24479 *Q);
/**	@brief Set ECP4 to point-at-infinity
 *
	@param P ECP4 instance to be set to infinity
 */
extern void ECP4_BLS24479_inf(ECP4_BLS24479 *P);
/**	@brief Tests for equality of two ECP4s
 *
	@param P ECP4 instance to be compared
	@param Q ECP4 instance to be compared
	@return 1 if P=Q, else returns 0
 */
extern int ECP4_BLS24479_equals(ECP4_BLS24479 *P, ECP4_BLS24479 *Q);

/**	@brief Converts an ECP4 point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP4 instance to be converted to affine form
 */
extern void ECP4_BLS24479_affine(ECP4_BLS24479 *P);

/**	@brief Extract x and y coordinates of an ECP4 point P
 *
	If x=y, returns only x
	@param x FP4 on exit = x coordinate of point
	@param y FP4 on exit = y coordinate of point (unless x=y)
	@param P ECP4 instance (x,y)
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP4_BLS24479_get(FP4_BLS24479 *x, FP4_BLS24479 *y, ECP4_BLS24479 *P);
/**	@brief Formats and outputs an ECP4 point to the console, converted to affine coordinates
 *
	@param P ECP4 instance to be printed
 */
extern void ECP4_BLS24479_output(ECP4_BLS24479 *P);

/**	@brief Formats and outputs an ECP4 point to an octet string
 *
	The octet string is created in the form x|y.
	Convert the real and imaginary parts of the x and y coordinates to big-endian base 256 form.
	@param S output octet string
	@param P ECP4 instance to be converted to an octet string
    @param c true for compression
 */
extern void ECP4_BLS24479_toOctet(octet *S, ECP4_BLS24479 *P, bool c);
/**	@brief Creates an ECP4 point from an octet string
 *
	The octet string is in the form x|y
	The real and imaginary parts of the x and y coordinates are in big-endian base 256 form.
	@param P ECP4 instance to be created from the octet string
	@param S input octet string
	return 1 if octet string corresponds to a point on the curve, else 0
 */
extern int ECP4_BLS24479_fromOctet(ECP4_BLS24479 *P, octet *S);
/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x)=x^3+Ax+B
	Used internally.
	@param r FP4 value of f(x)
	@param x FP4 instance
 */
extern void ECP4_BLS24479_rhs(FP4_BLS24479 *r, FP4_BLS24479 *x);
/**	@brief Set ECP4 to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP4 instance to be set (x,y)
	@param x FP4 x coordinate of point
	@param y FP4 y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP4_BLS24479_set(ECP4_BLS24479 *P, FP4_BLS24479 *x, FP4_BLS24479 *y);
/**	@brief Set ECP to point(x,[y]) given x
 *
	Point P set to infinity if no such point on the curve. Otherwise y coordinate is calculated from x.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
    @param s sign of y
	@return 1 if point exists, else 0
 */
extern int ECP4_BLS24479_setx(ECP4_BLS24479 *P, FP4_BLS24479 *x, int s);
/**	@brief Negation of an ECP4 point
 *
	@param P ECP4 instance, on exit = -P
 */
extern void ECP4_BLS24479_neg(ECP4_BLS24479 *P);

/**	@brief Reduction of an ECP4 point
 *
	@param P ECP4 instance, on exit (x,y) are reduced wrt the modulus
 */
extern void ECP4_BLS24479_reduce(ECP4_BLS24479 *P);


/**	@brief Doubles an ECP4 instance P
 *
	@param P ECP4 instance, on exit =2*P
 */
extern int ECP4_BLS24479_dbl(ECP4_BLS24479 *P);
/**	@brief Adds ECP4 instance Q to ECP4 instance P
 *
	@param P ECP4 instance, on exit =P+Q
	@param Q ECP4 instance to be added to P
 */
extern int ECP4_BLS24479_add(ECP4_BLS24479 *P, ECP4_BLS24479 *Q);
/**	@brief Subtracts ECP instance Q from ECP4 instance P
 *
	@param P ECP4 instance, on exit =P-Q
	@param Q ECP4 instance to be subtracted from P
 */
extern void ECP4_BLS24479_sub(ECP4_BLS24479 *P, ECP4_BLS24479 *Q);
/**	@brief Multiplies an ECP4 instance P by a BIG, side-channel resistant
 *
	Uses fixed sized windows.
	@param P ECP4 instance, on exit =b*P
	@param b BIG number multiplier

 */
extern void ECP4_BLS24479_mul(ECP4_BLS24479 *P, BIG_480_29 b);

/**	@brief Calculates required Frobenius constants
 *
	Calculate Frobenius constants
	@param F array of FP2 precalculated constants

 */
extern void ECP4_BLS24479_frob_constants(FP2_BLS24479 F[3]);

/**	@brief Multiplies an ECP4 instance P by the internal modulus p^n, using precalculated Frobenius constants
 *
	Fast point multiplication using Frobenius
	@param P ECP4 instance, on exit = p^n*P
	@param F array of FP2 precalculated Frobenius constant
	@param n power of prime

 */
extern void ECP4_BLS24479_frob(ECP4_BLS24479 *P, FP2_BLS24479 F[3], int n);

/**	@brief Calculates P=Sigma b[i]*Q[i] for i=0 to 7
 *
	@param P ECP4 instance, on exit = Sigma b[i]*Q[i] for i=0 to 7
	@param Q ECP4 array of 4 points
	@param b BIG array of 4 multipliers
 */
extern void ECP4_BLS24479_mul8(ECP4_BLS24479 *P, ECP4_BLS24479 *Q, BIG_480_29 *b);

/**	@brief Multiplies random point by co-factor
 *
	@param Q ECP4 multiplied by co-factor
 */
extern void ECP4_BLS24479_cfp(ECP4_BLS24479 *Q);

/**	@brief Maps random BIG to curve point in constant time
 *
	@param Q ECP4 instance 
	@param x FP4 derived from hash
 */
extern void ECP4_BLS24479_map2point(ECP4_BLS24479 *Q, FP4_BLS24479 *x);

/**	@brief Maps random BIG to curve point using hunt-and-peck
 *
	@param Q ECP4 instance 
	@param x Fp derived from hash
 */
extern void ECP4_BLS24479_hap2point(ECP4_BLS24479 *Q, BIG_480_29  x);

/**	@brief Maps random BIG to curve point of correct order
 *
	@param P ECP4 instance of correct order
	@param W OCTET byte array to be mapped
 */
extern void ECP4_BLS24479_mapit(ECP4_BLS24479 *P, octet *W);

/**	@brief Get Group Generator from ROM
 *
	@param G ECP4 instance
	@return 1 if point exists, else 0
 */
extern int ECP4_BLS24479_generator(ECP4_BLS24479 *G);


#endif

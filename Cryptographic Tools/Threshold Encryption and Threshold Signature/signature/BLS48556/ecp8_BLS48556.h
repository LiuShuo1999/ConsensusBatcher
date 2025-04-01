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
 * @file ecp8.h
 * @author Mike Scott
 * @brief ECP2 Header File
 *
 */

#ifndef ECP8_BLS48556_H
#define ECP8_BLS48556_H

#include "fp8_BLS48556.h"
#include "config_curve_BLS48556.h"


extern const BIG_560_29 Fra_BLS48556; /**< real part of BN curve Frobenius Constant */
extern const BIG_560_29 Frb_BLS48556; /**< imaginary part of BN curve Frobenius Constant */


/**
	@brief ECP8 Structure - Elliptic Curve Point over quadratic extension field
*/

typedef struct
{
//    int inf; /**< Infinity Flag */
    FP8_BLS48556 x;   /**< x-coordinate of point */
    FP8_BLS48556 y;   /**< y-coordinate of point */
    FP8_BLS48556 z;	/**< z-coordinate of point */
} ECP8_BLS48556;


/* Curve Params - see rom.c */
extern const int CURVE_B_I_BLS48556;		/**< Elliptic curve B parameter */
extern const BIG_560_29 CURVE_B_BLS48556;     /**< Elliptic curve B parameter */
extern const BIG_560_29 CURVE_Order_BLS48556; /**< Elliptic curve group order */
extern const BIG_560_29 CURVE_Cof_BLS48556;   /**< Elliptic curve cofactor */
extern const BIG_560_29 CURVE_Bnx_BLS48556;   /**< Elliptic curve parameter */
extern const BIG_560_29 CURVE_HTPC_BLS48556;  /**< Hash to Point precomputation */


/* Generator point on G1 */
extern const BIG_560_29 CURVE_Gx; /**< x-coordinate of generator point in group G1  */
extern const BIG_560_29 CURVE_Gy; /**< y-coordinate of generator point in group G1  */

/* For Pairings only */

/* Generator point on G2 */
extern const BIG_560_29 CURVE_Pxaaa_BLS48556; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pxaab_BLS48556; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pxaba_BLS48556; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pxabb_BLS48556; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pxbaa_BLS48556; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pxbab_BLS48556; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pxbba_BLS48556; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pxbbb_BLS48556; /**< imaginary part of x-coordinate of generator point in group G2 */

extern const BIG_560_29 CURVE_Pyaaa_BLS48556; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pyaab_BLS48556; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pyaba_BLS48556; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pyabb_BLS48556; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pybaa_BLS48556; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pybab_BLS48556; /**< imaginary part of y-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pybba_BLS48556; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_560_29 CURVE_Pybbb_BLS48556; /**< imaginary part of y-coordinate of generator point in group G2 */


/* ECP8 E(FP8) prototypes */
/**	@brief Tests for ECP8 point equal to infinity
 *
	@param P ECP8 point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP8_BLS48556_isinf(ECP8_BLS48556 *P);
/**	@brief Copy ECP8 point to another ECP8 point
 *
	@param P ECP8 instance, on exit = Q
	@param Q ECP8 instance to be copied
 */
extern void ECP8_BLS48556_copy(ECP8_BLS48556 *P, ECP8_BLS48556 *Q);
/**	@brief Set ECP8 to point-at-infinity
 *
	@param P ECP8 instance to be set to infinity
 */
extern void ECP8_BLS48556_inf(ECP8_BLS48556 *P);
/**	@brief Tests for equality of two ECP8s
 *
	@param P ECP8 instance to be compared
	@param Q ECP8 instance to be compared
	@return 1 if P=Q, else returns 0
 */
extern int ECP8_BLS48556_equals(ECP8_BLS48556 *P, ECP8_BLS48556 *Q);


/**	@brief Converts an ECP8 point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP8 instance to be converted to affine form
 */
extern void ECP8_BLS48556_affine(ECP8_BLS48556 *P);


/**	@brief Extract x and y coordinates of an ECP8 point P
 *
	If x=y, returns only x
	@param x FP8 on exit = x coordinate of point
	@param y FP8 on exit = y coordinate of point (unless x=y)
	@param P ECP8 instance (x,y)
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP8_BLS48556_get(FP8_BLS48556 *x, FP8_BLS48556 *y, ECP8_BLS48556 *P);
/**	@brief Formats and outputs an ECP8 point to the console, converted to affine coordinates
 *
	@param P ECP8 instance to be printed
 */
extern void ECP8_BLS48556_output(ECP8_BLS48556 *P);

/**	@brief Formats and outputs an ECP8 point to an octet string
 *
	The octet string is created in the form x|y.
	Convert the real and imaginary parts of the x and y coordinates to big-endian base 256 form.
	@param S output octet string
	@param P ECP8 instance to be converted to an octet string
    @param c true for compression
 */
extern void ECP8_BLS48556_toOctet(octet *S, ECP8_BLS48556 *P, bool c);
/**	@brief Creates an ECP8 point from an octet string
 *
	The octet string is in the form x|y
	The real and imaginary parts of the x and y coordinates are in big-endian base 256 form.
	@param P ECP8 instance to be created from the octet string
	@param S input octet string
	return 1 if octet string corresponds to a point on the curve, else 0
 */
extern int ECP8_BLS48556_fromOctet(ECP8_BLS48556 *P, octet *S);
/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x)=x^3+Ax+B
	Used internally.
	@param r FP8 value of f(x)
	@param x FP8 instance
 */
extern void ECP8_BLS48556_rhs(FP8_BLS48556 *r, FP8_BLS48556 *x);
/**	@brief Set ECP8 to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP8 instance to be set (x,y)
	@param x FP8 x coordinate of point
	@param y FP8 y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP8_BLS48556_set(ECP8_BLS48556 *P, FP8_BLS48556 *x, FP8_BLS48556 *y);
/**	@brief Set ECP to point(x,[y]) given x
 *
	Point P set to infinity if no such point on the curve. Otherwise y coordinate is calculated from x.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
    @param s sign of y
	@return 1 if point exists, else 0
 */
extern int ECP8_BLS48556_setx(ECP8_BLS48556 *P, FP8_BLS48556 *x, int s);
/**	@brief Negation of an ECP8 point
 *
	@param P ECP8 instance, on exit = -P
 */
extern void ECP8_BLS48556_neg(ECP8_BLS48556 *P);

/**	@brief Reduction of an ECP8 point
 *
	@param P ECP8 instance, on exit (x,y) are reduced wrt the modulus
 */
extern void ECP8_BLS48556_reduce(ECP8_BLS48556 *P);

/**	@brief Doubles an ECP8 instance P
 *
	@param P ECP8 instance, on exit =2*P
 */
extern int ECP8_BLS48556_dbl(ECP8_BLS48556 *P);
/**	@brief Adds ECP8 instance Q to ECP8 instance P
 *
	@param P ECP8 instance, on exit =P+Q
	@param Q ECP8 instance to be added to P
 */
extern int ECP8_BLS48556_add(ECP8_BLS48556 *P, ECP8_BLS48556 *Q);
/**	@brief Subtracts ECP instance Q from ECP8 instance P
 *
	@param P ECP8 instance, on exit =P-Q
	@param Q ECP8 instance to be subtracted from P
 */
extern void ECP8_BLS48556_sub(ECP8_BLS48556 *P, ECP8_BLS48556 *Q);
/**	@brief Multiplies an ECP8 instance P by a BIG, side-channel resistant
 *
	Uses fixed sized windows.
	@param P ECP8 instance, on exit =b*P
	@param b BIG number multiplier

 */
extern void ECP8_BLS48556_mul(ECP8_BLS48556 *P, BIG_560_29 b);

/**	@brief Calculates required Frobenius constants
 *
	Calculate Frobenius constants
	@param F array of FP2 precalculated constants

 */
extern void ECP8_BLS48556_frob_constants(FP2_BLS48556 F[3]);

/**	@brief Multiplies an ECP8 instance P by the internal modulus p^n, using precalculated Frobenius constants
 *
	Fast point multiplication using Frobenius
	@param P ECP8 instance, on exit = p^n*P
	@param F array of FP2 precalculated Frobenius constant
	@param n power of prime

 */
extern void ECP8_BLS48556_frob(ECP8_BLS48556 *P, FP2_BLS48556 F[3], int n);

/**	@brief Calculates P=Sigma b[i]*Q[i] for i=0 to 7
 *
	@param P ECP8 instance, on exit = Sigma b[i]*Q[i] for i=0 to 7
	@param Q ECP8 array of 4 points
	@param b BIG array of 4 multipliers
 */
extern void ECP8_BLS48556_mul16(ECP8_BLS48556 *P, ECP8_BLS48556 *Q, BIG_560_29 *b);

/**	@brief Multiplies random point by co-factor
 *
	@param Q ECP8 multiplied by co-factor
 */
extern void ECP8_BLS48556_cfp(ECP8_BLS48556 *Q);

/**	@brief Hashes random BIG to curve point using hunt-and-peck
 *
	@param Q ECP8 instance 
	@param x Fp derived from hash
 */
extern void ECP8_BLS48556_hap2point(ECP8_BLS48556 *Q, BIG_560_29  x);

/**	@brief Hashes random BIG to curve point in constant time
 *
	@param Q ECP8 instance 
	@param x FP8 derived from hash
 */
extern void ECP8_BLS48556_map2point(ECP8_BLS48556 *Q, FP8_BLS48556 *x);


/**	@brief Maps random BIG to curve point of correct order
 *
	@param P ECP8 instance of correct order
	@param W OCTET byte array to be mapped
 */
extern void ECP8_BLS48556_mapit(ECP8_BLS48556 *P, octet *W);

/**	@brief Get Group Generator from ROM
 *
	@param G ECP8 instance
	@return 1 if point exists, else 0
 */
extern int ECP8_BLS48556_generator(ECP8_BLS48556 *G);


#endif

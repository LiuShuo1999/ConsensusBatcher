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
 * @file ecp2.h
 * @author Mike Scott
 * @brief ECP2 Header File
 *
 */

#ifndef ECP2_BN158_H
#define ECP2_BN158_H

#include "fp2_BN158.h"
#include "config_curve_BN158.h"

/**
	@brief ECP2 Structure - Elliptic Curve Point over quadratic extension field
*/

typedef struct
{
//    int inf; /**< Infinity Flag */
    FP2_BN158 x;   /**< x-coordinate of point */
    FP2_BN158 y;   /**< y-coordinate of point */
    FP2_BN158 z;   /**< z-coordinate of point */
} ECP2_BN158;


/* Curve Params - see rom_zzz.c */

extern const int CURVE_B_I_BN158;		/**< Elliptic curve B parameter */
extern const BIG_160_28 CURVE_B_BN158;     /**< Elliptic curve B parameter */
extern const BIG_160_28 CURVE_Order_BN158; /**< Elliptic curve group order */
extern const BIG_160_28 CURVE_Cof_BN158;   /**< Elliptic curve cofactor */
extern const BIG_160_28 CURVE_Bnx_BN158;   /**< Elliptic curve parameter */
extern const BIG_160_28 CURVE_HTPC_BN158;  /**< Hash to Point precomputation */

extern const BIG_160_28 Fra_BN158; /**< real part of BN curve Frobenius Constant */
extern const BIG_160_28 Frb_BN158; /**< imaginary part of BN curve Frobenius Constant */


/* Generator point on G1 */
extern const BIG_160_28 CURVE_Gx_BN158; /**< x-coordinate of generator point in group G1  */
extern const BIG_160_28 CURVE_Gy_BN158; /**< y-coordinate of generator point in group G1  */

/* For Pairings only */

/* Generator point on G2 */
extern const BIG_160_28 CURVE_Pxa_BN158; /**< real part of x-coordinate of generator point in group G2 */
extern const BIG_160_28 CURVE_Pxb_BN158; /**< imaginary part of x-coordinate of generator point in group G2 */
extern const BIG_160_28 CURVE_Pya_BN158; /**< real part of y-coordinate of generator point in group G2 */
extern const BIG_160_28 CURVE_Pyb_BN158; /**< imaginary part of y-coordinate of generator point in group G2 */

/* ECP2 E(Fp2) prototypes */
/**	@brief Tests for ECP2 point equal to infinity
 *
	@param P ECP2 point to be tested
	@return 1 if infinity, else returns 0
 */
extern int ECP2_BN158_isinf(ECP2_BN158 *P);
/**	@brief Copy ECP2 point to another ECP2 point
 *
	@param P ECP2 instance, on exit = Q
	@param Q ECP2 instance to be copied
 */
extern void ECP2_BN158_copy(ECP2_BN158 *P, ECP2_BN158 *Q);
/**	@brief Set ECP2 to point-at-infinity
 *
	@param P ECP2 instance to be set to infinity
 */
extern void ECP2_BN158_inf(ECP2_BN158 *P);
/**	@brief Tests for equality of two ECP2s
 *
	@param P ECP2 instance to be compared
	@param Q ECP2 instance to be compared
	@return 1 if P=Q, else returns 0
 */
extern int ECP2_BN158_equals(ECP2_BN158 *P, ECP2_BN158 *Q);
/**	@brief Converts an ECP2 point from Projective (x,y,z) coordinates to affine (x,y) coordinates
 *
	@param P ECP2 instance to be converted to affine form
 */
extern void ECP2_BN158_affine(ECP2_BN158 *P);
/**	@brief Extract x and y coordinates of an ECP2 point P
 *
	If x=y, returns only x
	@param x FP2 on exit = x coordinate of point
	@param y FP2 on exit = y coordinate of point (unless x=y)
	@param P ECP2 instance (x,y)
	@return -1 if P is point-at-infinity, else 0
 */
extern int ECP2_BN158_get(FP2_BN158 *x, FP2_BN158 *y, ECP2_BN158 *P);
/**	@brief Formats and outputs an ECP2 point to the console, converted to affine coordinates
 *
	@param P ECP2 instance to be printed
 */
extern void ECP2_BN158_output(ECP2_BN158 *P);
/**	@brief Formats and outputs an ECP2 point to the console, in projective coordinates
 *
	@param P ECP2 instance to be printed
 */
extern void ECP2_BN158_outputxyz(ECP2_BN158 *P);
/**	@brief Formats and outputs an ECP2 point to an octet string
 *
	The octet string is created in the form x|y or just x if compressed
	Convert the real and imaginary parts of the x and y coordinates to big-endian base 256 form.
	@param S output octet string
	@param P ECP2 instance to be converted to an octet string
    @param c true for compression
 */
extern void ECP2_BN158_toOctet(octet *S, ECP2_BN158 *P, bool c);
/**	@brief Creates an ECP2 point from an octet string
 *
	The octet string is in the form x|y
	The real and imaginary parts of the x and y coordinates are in big-endian base 256 form.
    If in compressed form only the x coordinate is provided as in 0x2|x if y is even, or 0x3|x if y is odd
	@param P ECP2 instance to be created from the octet string
	@param S input octet string
	return 1 if octet string corresponds to a point on the curve, else 0
 */
extern int ECP2_BN158_fromOctet(ECP2_BN158 *P, octet *S);
/**	@brief Calculate Right Hand Side of curve equation y^2=f(x)
 *
	Function f(x)=x^3+Ax+B
	Used internally.
	@param r FP2 value of f(x)
	@param x FP2 instance
 */
extern void ECP2_BN158_rhs(FP2_BN158 *r, FP2_BN158 *x);
/**	@brief Set ECP2 to point(x,y) given x and y
 *
	Point P set to infinity if no such point on the curve.
	@param P ECP2 instance to be set (x,y)
	@param x FP2 x coordinate of point
	@param y FP2 y coordinate of point
	@return 1 if point exists, else 0
 */
extern int ECP2_BN158_set(ECP2_BN158 *P, FP2_BN158 *x, FP2_BN158 *y);
/**	@brief Set ECP to point(x,[y]) given x and sign of y
 *
	Point P set to infinity if no such point on the curve. Otherwise y coordinate is calculated from x.
	@param P ECP instance to be set (x,[y])
	@param x BIG x coordinate of point
    @param s sign of y
	@return 1 if point exists, else 0
 */
extern int ECP2_BN158_setx(ECP2_BN158 *P, FP2_BN158 *x, int s);
/**	@brief Negation of an ECP2 point
 *
	@param P ECP2 instance, on exit = -P
 */
extern void ECP2_BN158_neg(ECP2_BN158 *P);
/**	@brief Doubles an ECP2 instance P
 *
	@param P ECP2 instance, on exit =2*P
 */
extern int ECP2_BN158_dbl(ECP2_BN158 *P);
/**	@brief Adds ECP2 instance Q to ECP2 instance P
 *
	@param P ECP2 instance, on exit =P+Q
	@param Q ECP2 instance to be added to P
 */
extern int ECP2_BN158_add(ECP2_BN158 *P, ECP2_BN158 *Q);
/**	@brief Subtracts ECP instance Q from ECP2 instance P
 *
	@param P ECP2 instance, on exit =P-Q
	@param Q ECP2 instance to be subtracted from P
 */
extern void ECP2_BN158_sub(ECP2_BN158 *P, ECP2_BN158 *Q);
/**	@brief Multiplies an ECP2 instance P by a BIG, side-channel resistant
 *
	Uses fixed sized windows.
	@param P ECP2 instance, on exit =b*P
	@param b BIG number multiplier

 */
extern void ECP2_BN158_mul(ECP2_BN158 *P, BIG_160_28 b);
/**	@brief Multiplies an ECP2 instance P by the internal modulus p, using precalculated Frobenius constant f
 *
	Fast point multiplication using Frobenius
	@param P ECP2 instance, on exit = p*P
	@param f FP2 precalculated Frobenius constant

 */
extern void ECP2_BN158_frob(ECP2_BN158 *P, FP2_BN158 *f);
/**	@brief Calculates P=b[0]*Q[0]+b[1]*Q[1]+b[2]*Q[2]+b[3]*Q[3]
 *
	@param P ECP2 instance, on exit = b[0]*Q[0]+b[1]*Q[1]+b[2]*Q[2]+b[3]*Q[3]
	@param Q ECP2 array of 4 points
	@param b BIG array of 4 multipliers
 */
extern void ECP2_BN158_mul4(ECP2_BN158 *P, ECP2_BN158 *Q, BIG_160_28 *b);

/**	@brief Multiplies random point by co-factor
 *
	@param Q ECP2 multiplied by co-factor
 */
extern void ECP2_BN158_cfp(ECP2_BN158 *Q);

/**	@brief Maps random BIG to curve point in constant time
 *
	@param Q ECP2 instance 
	@param x FP2 derived from hash
 */
extern void ECP2_BN158_map2point(ECP2_BN158 *Q, FP2_BN158 *x);


/**	@brief Maps random BIG to curve point using hunt-and-peck
 *
	@param Q ECP2 instance 
	@param x Fp derived from hash
 */
extern void ECP2_BN158_hap2point(ECP2_BN158 *Q, BIG_160_28  x);

/**	@brief Maps random BIG to curve point of correct order
 *
	@param P ECP2 instance of correct order
	@param w OCTET byte array to be mapped
 */
extern void ECP2_BN158_mapit(ECP2_BN158 *P, octet *w);

/**	@brief Get Group Generator from ROM
 *
	@param G ECP2 instance
	@return 1 if point exists, else 0
 */
extern int ECP2_BN158_generator(ECP2_BN158 *G);

#endif

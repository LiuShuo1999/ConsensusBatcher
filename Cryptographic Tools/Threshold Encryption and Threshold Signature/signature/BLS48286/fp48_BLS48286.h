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
 * @file fp48.h
 * @author Mike Scott
 * @brief FP48 Header File
 *
 */

#ifndef FP48_BLS48286_H
#define FP48_BLS48286_H

#include "fp16_BLS48286.h"

/**
	@brief FP12 Structure - towered over three FP16
*/

typedef struct
{
    FP16_BLS48286 a; /**< first part of FP12 */
    FP16_BLS48286 b; /**< second part of FP12 */
    FP16_BLS48286 c; /**< third part of FP12 */
    int type;   /**< record sparseness */
} FP48_BLS48286;

extern const BIG_288_29 Fra_BLS48286; /**< real part of BN curve Frobenius Constant */
extern const BIG_288_29 Frb_BLS48286; /**< imaginary part of BN curve Frobenius Constant */

/* FP48 prototypes */
/**	@brief Tests for FP48 equal to zero
 *
	@param x FP48 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP48_BLS48286_iszilch(FP48_BLS48286 *x);
/**	@brief Tests for FP48 equal to unity
 *
	@param x FP48 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP48_BLS48286_isunity(FP48_BLS48286 *x);
/**	@brief Copy FP48 to another FP48
 *
	@param x FP48 instance, on exit = y
	@param y FP48 instance to be copied
 */
extern void FP48_BLS48286_copy(FP48_BLS48286 *x, FP48_BLS48286 *y);
/**	@brief Set FP48 to unity
 *
	@param x FP48 instance to be set to one
 */
extern void FP48_BLS48286_one(FP48_BLS48286 *x);

/**	@brief Set FP48 to zero
 *
	@param x FP48 instance to be set to zero
 */
extern void FP48_BLS48286_zero(FP48_BLS48286 *x);

/**	@brief Tests for equality of two FP48s
 *
	@param x FP48 instance to be compared
	@param y FP48 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP48_BLS48286_equals(FP48_BLS48286 *x, FP48_BLS48286 *y);
/**	@brief Conjugation of FP48
 *
	If y=(a,b,c) (where a,b,c are its three FP16 components) on exit x=(conj(a),-conj(b),conj(c))
	@param x FP48 instance, on exit = conj(y)
	@param y FP48 instance
 */
extern void FP48_BLS48286_conj(FP48_BLS48286 *x, FP48_BLS48286 *y);
/**	@brief Initialise FP48 from single FP16
 *
	Sets first FP16 component of an FP48, other components set to zero
	@param x FP48 instance to be initialised
	@param a FP16 to form first part of FP48
 */
extern void FP48_BLS48286_from_FP16(FP48_BLS48286 *x, FP16_BLS48286 *a);
/**	@brief Initialise FP48 from three FP16s
 *
	@param x FP48 instance to be initialised
	@param a FP16 to form first part of FP48
	@param b FP16 to form second part of FP48
	@param c FP16 to form third part of FP48
 */
extern void FP48_BLS48286_from_FP16s(FP48_BLS48286 *x, FP16_BLS48286 *a, FP16_BLS48286* b, FP16_BLS48286 *c);
/**	@brief Fast Squaring of an FP48 in "unitary" form
 *
	@param x FP48 instance, on exit = y^2
	@param y FP16 instance, must be unitary
 */
extern void FP48_BLS48286_usqr(FP48_BLS48286 *x, FP48_BLS48286 *y);
/**	@brief Squaring an FP48
 *
	@param x FP48 instance, on exit = y^2
	@param y FP48 instance
 */
extern void FP48_BLS48286_sqr(FP48_BLS48286 *x, FP48_BLS48286 *y);
/**	@brief Fast multiplication of two sparse FP24s that arises from ATE pairing line functions
 *
	@param x FP48 instance, on exit = x*y
	@param y FP48 instance, of special form
 */
extern void FP48_BLS48286_smul(FP48_BLS48286 *x, FP48_BLS48286 *y);

/**	@brief Fast multiplication of what may be sparse multiplicands
 *
	@param x FP48 instance, on exit = x*y
	@param y FP48 instance, of special form
 */
extern void FP48_BLS48286_ssmul(FP48_BLS48286 *x, FP48_BLS48286 *y);
/**	@brief Full unconditional Multiplication of two FP24s
 *
	@param x FP48 instance, on exit = x*y
	@param y FP48 instance, the multiplier
 */
extern void FP48_BLS48286_mul(FP48_BLS48286 *x, FP48_BLS48286 *y);
/**	@brief Inverting an FP48
 *
	@param x FP48 instance, on exit = 1/y
	@param y FP48 instance
 */
extern void FP48_BLS48286_inv(FP48_BLS48286 *x, FP48_BLS48286 *y);
/**	@brief Raises an FP48 to the power of a BIG
 *
	@param r FP48 instance, on exit = y^b
	@param x FP48 instance
	@param b BIG number
 */
extern void FP48_BLS48286_pow(FP48_BLS48286 *r, FP48_BLS48286 *x, BIG_288_29 b);

//extern void FP48_ppow(FP48 *r,FP48 *x,BIG b);

/**	@brief Raises an FP48 instance x to a small integer power, side-channel resistant
 *
	@param x FP48 instance, on exit = x^i
	@param i small integer exponent
	@param b maximum number of bits in exponent
 */
extern void FP48_BLS48286_pinpow(FP48_BLS48286 *x, int i, int b);

/**	@brief Raises an FP48 instance x to a BIG_288_29 power, compressed to FP16
 *
	@param c FP16 instance, on exit = x^(e mod r) as FP16
	@param x FP48 input
	@param e BIG exponent
	@param r BIG group order
 */
extern void FP48_BLS48286_compow(FP16_BLS48286 *c, FP48_BLS48286 *x, BIG_288_29 e, BIG_288_29 r);

/**	@brief Calculate Pi x[i]^b[i] for i=0 to 15, side-channel resistant
 *
	@param r FP48 instance, on exit = Pi x[i]^b[i] for i=0 to 15
	@param x FP48 array with 16 FP48s
	@param b BIG array of 16 exponents
 */
extern void FP48_BLS48286_pow16(FP48_BLS48286 *r, FP48_BLS48286 *x, BIG_288_29 *b);


/**	@brief Raises an FP48 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP48 instance, on exit = x^p^n
	@param f FP2 precalculated Frobenius constant
	@param n power of p
 */
extern void FP48_BLS48286_frob(FP48_BLS48286 *x, FP2_BLS48286 *f, int n);

/**	@brief Reduces all components of possibly unreduced FP48 mod Modulus
 *
	@param x FP48 instance, on exit reduced mod Modulus
 */
extern void FP48_BLS48286_reduce(FP48_BLS48286 *x);
/**	@brief Normalises the components of an FP48
 *
	@param x FP48 instance to be normalised
 */
extern void FP48_BLS48286_norm(FP48_BLS48286 *x);
/**	@brief Formats and outputs an FP48 to the console
 *
	@param x FP48 instance to be printed
 */
extern void FP48_BLS48286_output(FP48_BLS48286 *x);
/**	@brief Formats and outputs an FP48 instance to an octet string
 *
	Serializes the components of an FP48 to big-endian base 256 form.
	@param S output octet string
	@param x FP48 instance to be converted to an octet string
 */
extern void FP48_BLS48286_toOctet(octet *S, FP48_BLS48286 *x);
/**	@brief Creates an FP48 instance from an octet string
 *
	De-serializes the components of an FP48 to create an FP48 from big-endian base 256 components.
	@param x FP48 instance to be created from an octet string
	@param S input octet string

 */
extern void FP48_BLS48286_fromOctet(FP48_BLS48286 *x, octet *S);
/**	@brief Calculate the trace of an FP48
 *
	@param t FP16 trace of x, on exit = tr(x)
	@param x FP48 instance

 */
extern void FP48_BLS48286_trace(FP16_BLS48286 *t, FP48_BLS48286 *x);

/**	@brief Conditional copy of FP48 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP48 instance, set to y if s!=0
	@param y another FP48 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP48_BLS48286_cmove(FP48_BLS48286 *x, FP48_BLS48286 *y, int s);

#endif

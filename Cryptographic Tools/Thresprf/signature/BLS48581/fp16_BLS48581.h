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
 * @file fp16.h
 * @author Mike Scott
 * @brief FP16 Header File
 *
 */

#ifndef FP16_BLS48581_H
#define FP16_BLS48581_H

#include "fp8_BLS48581.h"
#include "config_curve_BLS48581.h"


/**
	@brief FP16 Structure - towered over two FP8
*/

typedef struct
{
    FP8_BLS48581 a; /**< real part of FP16 */
    FP8_BLS48581 b; /**< imaginary part of FP16 */
} FP16_BLS48581;


/* FP16 prototypes */
/**	@brief Tests for FP16 equal to zero
 *
	@param x FP16 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP16_BLS48581_iszilch(FP16_BLS48581 *x);


/**	@brief Serialize in FP16  
 *
    @param b buffer for output
	@param x FP16 number to be serialized
 */
extern void FP16_BLS48581_toBytes(char *b,FP16_BLS48581 *x);
/**	@brief Serialize out FP16  
 *
	@param x FP16 number to be serialized
    @param b buffer for input
 */
extern void FP16_BLS48581_fromBytes(FP16_BLS48581 *x,char *b);

/**	@brief Tests for FP16 equal to unity
 *
	@param x FP16 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP16_BLS48581_isunity(FP16_BLS48581 *x);
/**	@brief Tests for equality of two FP16s
 *
	@param x FP16 instance to be compared
	@param y FP16 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP16_BLS48581_equals(FP16_BLS48581 *x, FP16_BLS48581 *y);
/**	@brief Tests for FP16 having only a real part and no imaginary part
 *
	@param x FP16 number to be tested
	@return 1 if real, else returns 0
 */
extern int FP16_BLS48581_isreal(FP16_BLS48581 *x);
/**	@brief Initialise FP16 from two FP8s
 *
	@param x FP16 instance to be initialised
	@param a FP8 to form real part of FP16
	@param b FP8 to form imaginary part of FP16
 */
extern void FP16_BLS48581_from_FP8s(FP16_BLS48581 *x, FP8_BLS48581 *a, FP8_BLS48581 *b);
/**	@brief Initialise FP16 from single FP8
 *
	Imaginary part is set to zero
	@param x FP16 instance to be initialised
	@param a FP8 to form real part of FP16
 */
extern void FP16_BLS48581_from_FP8(FP16_BLS48581 *x, FP8_BLS48581 *a);

/**	@brief Initialise FP16 from single FP8
 *
	real part is set to zero
	@param x FP16 instance to be initialised
	@param a FP8 to form imaginary part of FP16
 */
extern void FP16_BLS48581_from_FP8H(FP16_BLS48581 *x, FP8_BLS48581 *a);


/**	@brief Copy FP16 to another FP16
 *
	@param x FP16 instance, on exit = y
	@param y FP16 instance to be copied
 */
extern void FP16_BLS48581_copy(FP16_BLS48581 *x, FP16_BLS48581 *y);
/**	@brief Set FP16 to zero
 *
	@param x FP16 instance to be set to zero
 */
extern void FP16_BLS48581_zero(FP16_BLS48581 *x);
/**	@brief Set FP16 to unity
 *
	@param x FP16 instance to be set to one
 */
extern void FP16_BLS48581_one(FP16_BLS48581 *x);
/**	@brief Negation of FP16
 *
	@param x FP16 instance, on exit = -y
	@param y FP16 instance
 */
extern void FP16_BLS48581_neg(FP16_BLS48581 *x, FP16_BLS48581 *y);
/**	@brief Conjugation of FP16
 *
	If y=(a,b) on exit x=(a,-b)
	@param x FP16 instance, on exit = conj(y)
	@param y FP16 instance
 */
extern void FP16_BLS48581_conj(FP16_BLS48581 *x, FP16_BLS48581 *y);
/**	@brief Negative conjugation of FP16
 *
	If y=(a,b) on exit x=(-a,b)
	@param x FP16 instance, on exit = -conj(y)
	@param y FP16 instance
 */
extern void FP16_BLS48581_nconj(FP16_BLS48581 *x, FP16_BLS48581 *y);
/**	@brief addition of two FP16s
 *
	@param x FP16 instance, on exit = y+z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_BLS48581_add(FP16_BLS48581 *x, FP16_BLS48581 *y, FP16_BLS48581 *z);
/**	@brief subtraction of two FP16s
 *
	@param x FP16 instance, on exit = y-z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_BLS48581_sub(FP16_BLS48581 *x, FP16_BLS48581 *y, FP16_BLS48581 *z);
/**	@brief Multiplication of an FP16 by an FP8
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP8 multiplier
 */
extern void FP16_BLS48581_pmul(FP16_BLS48581 *x, FP16_BLS48581 *y, FP8_BLS48581 *a);

/**	@brief Multiplication of an FP16 by an FP2
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP2 multiplier
 */
extern void FP16_BLS48581_qmul(FP16_BLS48581 *x, FP16_BLS48581 *y, FP2_BLS48581 *a);

/**	@brief Multiplication of an FP16 by an FP
 *
	@param x FP16 instance, on exit = y*a
	@param y FP16 instance
	@param a FP multiplier
 */
extern void FP16_BLS48581_tmul(FP16_BLS48581 *x, FP16_BLS48581 *y, FP_BLS48581 *a);

/**	@brief Multiplication of an FP16 by a small integer
 *
	@param x FP16 instance, on exit = y*i
	@param y FP16 instance
	@param i an integer
 */
extern void FP16_BLS48581_imul(FP16_BLS48581 *x, FP16_BLS48581 *y, int i);
/**	@brief Squaring an FP16
 *
	@param x FP16 instance, on exit = y^2
	@param y FP16 instance
 */
extern void FP16_BLS48581_sqr(FP16_BLS48581 *x, FP16_BLS48581 *y);
/**	@brief Multiplication of two FP16s
 *
	@param x FP16 instance, on exit = y*z
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_BLS48581_mul(FP16_BLS48581 *x, FP16_BLS48581 *y, FP16_BLS48581 *z);
/**	@brief Inverting an FP16
 *
	@param x FP16 instance, on exit = 1/y
	@param y FP16 instance
 */
extern void FP16_BLS48581_inv(FP16_BLS48581 *x, FP16_BLS48581 *y);
/**	@brief Formats and outputs an FP16 to the console
 *
	@param x FP16 instance to be printed
 */
extern void FP16_BLS48581_output(FP16_BLS48581 *x);
/**	@brief Formats and outputs an FP16 to the console in raw form (for debugging)
 *
	@param x FP16 instance to be printed
 */
extern void FP16_BLS48581_rawoutput(FP16_BLS48581 *x);
/**	@brief multiplies an FP16 instance by irreducible polynomial sqrt(1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1)*x
 */
extern void FP16_BLS48581_times_i(FP16_BLS48581 *x);
/**	@brief multiplies an FP16 instance by irreducible polynomial (1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1))^2*x
 */
extern void FP16_BLS48581_times_i2(FP16_BLS48581 *x);

/**	@brief multiplies an FP16 instance by irreducible polynomial (1+sqrt(-1))
 *
	@param x FP16 instance, on exit = sqrt(1+sqrt(-1))^4*x
 */
extern void FP16_BLS48581_times_i4(FP16_BLS48581 *x);


/**	@brief Normalises the components of an FP16
 *
	@param x FP16 instance to be normalised
 */
extern void FP16_BLS48581_norm(FP16_BLS48581 *x);
/**	@brief Reduces all components of possibly unreduced FP16 mod Modulus
 *
	@param x FP16 instance, on exit reduced mod Modulus
 */
extern void FP16_BLS48581_reduce(FP16_BLS48581 *x);
/**	@brief Raises an FP16 to the power of a BIG
 *
	@param x FP16 instance, on exit = y^b
	@param y FP16 instance
	@param b BIG number
 */
extern void FP16_BLS48581_pow(FP16_BLS48581 *x, FP16_BLS48581 *y, BIG_584_29 b);
/**	@brief Raises an FP16 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP16 instance, on exit = x^p
	@param f FP2 precalculated Frobenius constant
 */
extern void FP16_BLS48581_frob(FP16_BLS48581 *x, FP2_BLS48581 *f);
/**	@brief Calculates the XTR addition function r=w*x-conj(x)*y+z
 *
	@param r FP16 instance, on exit = w*x-conj(x)*y+z
	@param w FP16 instance
	@param x FP16 instance
	@param y FP16 instance
	@param z FP16 instance
 */
extern void FP16_BLS48581_xtr_A(FP16_BLS48581 *r, FP16_BLS48581 *w, FP16_BLS48581 *x, FP16_BLS48581 *y, FP16_BLS48581 *z);
/**	@brief Calculates the XTR doubling function r=x^2-2*conj(x)
 *
	@param r FP16 instance, on exit = x^2-2*conj(x)
	@param x FP16 instance
 */
extern void FP16_BLS48581_xtr_D(FP16_BLS48581 *r, FP16_BLS48581 *x);
/**	@brief Calculates FP16 trace of an FP12 raised to the power of a BIG number
 *
	XTR single exponentiation
	@param r FP16 instance, on exit = trace(w^b)
	@param x FP16 instance, trace of an FP12 w
	@param b BIG number
 */
extern void FP16_BLS48581_xtr_pow(FP16_BLS48581 *r, FP16_BLS48581 *x, BIG_584_29 b);
/**	@brief Calculates FP16 trace of c^a.d^b, where c and d are derived from FP16 traces of FP12s
 *
	XTR double exponentiation
	Assumes c=tr(x^m), d=tr(x^n), e=tr(x^(m-n)), f=tr(x^(m-2n))
	@param r FP16 instance, on exit = trace(c^a.d^b)
	@param c FP16 instance, trace of an FP12
	@param d FP16 instance, trace of an FP12
	@param e FP16 instance, trace of an FP12
	@param f FP16 instance, trace of an FP12
	@param a BIG number
	@param b BIG number
 */
extern void FP16_BLS48581_xtr_pow2(FP16_BLS48581 *r, FP16_BLS48581 *c, FP16_BLS48581 *d, FP16_BLS48581 *e, FP16_BLS48581 *f, BIG_584_29 a, BIG_584_29 b);

/**	@brief Conditional copy of FP16 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP16 instance, set to y if s!=0
	@param y another FP16 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP16_BLS48581_cmove(FP16_BLS48581 *x, FP16_BLS48581 *y, int s);


#endif


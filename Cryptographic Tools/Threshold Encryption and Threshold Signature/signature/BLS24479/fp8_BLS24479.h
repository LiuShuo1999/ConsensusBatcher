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
 * @file fp8.h
 * @author Mike Scott
 * @brief FP8 Header File
 *
 */

#ifndef FP8_BLS24479_H
#define FP8_BLS24479_H

#include "fp4_BLS24479.h"
#include "config_curve_BLS24479.h"


/**
	@brief FP8 Structure - towered over two FP4
*/

typedef struct
{
    FP4_BLS24479 a; /**< real part of FP8 */
    FP4_BLS24479 b; /**< imaginary part of FP8 */
} FP8_BLS24479;


/* FP8 prototypes */
/**	@brief Tests for FP8 equal to zero
 *
	@param x FP8 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP8_BLS24479_iszilch(FP8_BLS24479 *x);


/**	@brief Tests for lexically larger 
 *
	@param x FP8 number to be tested if larger than -x
	@return 1 if larger, else returns 0
 */
extern int FP8_BLS24479_islarger(FP8_BLS24479 *x);

/**	@brief Serialize in FP8  
 *
    @param b buffer for output
	@param x FP8 number to be serialized
 */
extern void FP8_BLS24479_toBytes(char *b,FP8_BLS24479 *x);
/**	@brief Serialize out FP8  
 *
	@param x FP8 number to be serialized
    @param b buffer for input
 */
extern void FP8_BLS24479_fromBytes(FP8_BLS24479 *x,char *b);

/**	@brief Tests for FP8 equal to unity
 *
	@param x FP8 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP8_BLS24479_isunity(FP8_BLS24479 *x);
/**	@brief Tests for equality of two FP8s
 *
	@param x FP8 instance to be compared
	@param y FP8 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP8_BLS24479_equals(FP8_BLS24479 *x, FP8_BLS24479 *y);
/**	@brief Tests for FP8 having only a real part and no imaginary part
 *
	@param x FP8 number to be tested
	@return 1 if real, else returns 0
 */
extern int FP8_BLS24479_isreal(FP8_BLS24479 *x);
/**	@brief Initialise FP8 from two FP4s
 *
	@param x FP8 instance to be initialised
	@param a FP4 to form real part of FP8
	@param b FP4 to form imaginary part of FP8
 */
extern void FP8_BLS24479_from_FP4s(FP8_BLS24479 *x, FP4_BLS24479 *a, FP4_BLS24479 *b);
/**	@brief Initialise FP8 from single FP4
 *
	Imaginary part is set to zero
	@param x FP8 instance to be initialised
	@param a FP4 to form real part of FP8
 */
extern void FP8_BLS24479_from_FP4(FP8_BLS24479 *x, FP4_BLS24479 *a);

/**	@brief Initialise FP8 from single FP4
 *
	real part is set to zero
	@param x FP8 instance to be initialised
	@param a FP4 to form imaginary part of FP8
 */
extern void FP8_BLS24479_from_FP4H(FP8_BLS24479 *x, FP4_BLS24479 *a);

/**	@brief Initialise FP8 from single FP
 *
	@param x FP8 instance to be initialised
	@param a FP to form real part of FP8
 */
extern void FP8_BLS24479_from_FP(FP8_BLS24479 *x, FP_BLS24479 *a);

/**	@brief Copy FP8 to another FP8
 *
	@param x FP8 instance, on exit = y
	@param y FP8 instance to be copied
 */
extern void FP8_BLS24479_copy(FP8_BLS24479 *x, FP8_BLS24479 *y);
/**	@brief Set FP8 to zero
 *
	@param x FP8 instance to be set to zero
 */
extern void FP8_BLS24479_zero(FP8_BLS24479 *x);
/**	@brief Set FP8 to unity
 *
	@param x FP8 instance to be set to one
 */
extern void FP8_BLS24479_one(FP8_BLS24479 *x);

/**	@brief Sign of FP8
 *
	@param x FP8 instance
	@return "sign" of FP8
 */
extern int FP8_BLS24479_sign(FP8_BLS24479 *x);

/**	@brief Negation of FP8
 *
	@param x FP8 instance, on exit = -y
	@param y FP8 instance
 */
extern void FP8_BLS24479_neg(FP8_BLS24479 *x, FP8_BLS24479 *y);
/**	@brief Conjugation of FP8
 *
	If y=(a,b) on exit x=(a,-b)
	@param x FP8 instance, on exit = conj(y)
	@param y FP8 instance
 */
extern void FP8_BLS24479_conj(FP8_BLS24479 *x, FP8_BLS24479 *y);
/**	@brief Negative conjugation of FP8
 *
	If y=(a,b) on exit x=(-a,b)
	@param x FP8 instance, on exit = -conj(y)
	@param y FP8 instance
 */
extern void FP8_BLS24479_nconj(FP8_BLS24479 *x, FP8_BLS24479 *y);
/**	@brief addition of two FP8s
 *
	@param x FP8 instance, on exit = y+z
	@param y FP8 instance
	@param z FP8 instance
 */
extern void FP8_BLS24479_add(FP8_BLS24479 *x, FP8_BLS24479 *y, FP8_BLS24479 *z);
/**	@brief subtraction of two FP8s
 *
	@param x FP8 instance, on exit = y-z
	@param y FP8 instance
	@param z FP8 instance
 */
extern void FP8_BLS24479_sub(FP8_BLS24479 *x, FP8_BLS24479 *y, FP8_BLS24479 *z);
/**	@brief Multiplication of an FP8 by an FP4
 *
	@param x FP8 instance, on exit = y*a
	@param y FP8 instance
	@param a FP4 multiplier
 */
extern void FP8_BLS24479_pmul(FP8_BLS24479 *x, FP8_BLS24479 *y, FP4_BLS24479 *a);

/**	@brief Multiplication of an FP8 by an FP2
 *
	@param x FP8 instance, on exit = y*a
	@param y FP8 instance
	@param a FP2 multiplier
 */
extern void FP8_BLS24479_qmul(FP8_BLS24479 *x, FP8_BLS24479 *y, FP2_BLS24479 *a);

/**	@brief Multiplication of an FP8 by an FP
 *
	@param x FP8 instance, on exit = y*a
	@param y FP8 instance
	@param a FP multiplier
 */
extern void FP8_BLS24479_tmul(FP8_BLS24479 *x, FP8_BLS24479 *y, FP_BLS24479 *a);

/**	@brief Multiplication of an FP8 by a small integer
 *
	@param x FP8 instance, on exit = y*i
	@param y FP8 instance
	@param i an integer
 */
extern void FP8_BLS24479_imul(FP8_BLS24479 *x, FP8_BLS24479 *y, int i);
/**	@brief Squaring an FP8
 *
	@param x FP8 instance, on exit = y^2
	@param y FP8 instance
 */
extern void FP8_BLS24479_sqr(FP8_BLS24479 *x, FP8_BLS24479 *y);
/**	@brief Multiplication of two FP8s
 *
	@param x FP8 instance, on exit = y*z
	@param y FP8 instance
	@param z FP8 instance
 */
extern void FP8_BLS24479_mul(FP8_BLS24479 *x, FP8_BLS24479 *y, FP8_BLS24479 *z);
/**	@brief Inverting an FP8
 *
	@param x FP8 instance, on exit = 1/y
	@param y FP8 instance
    @param h optional input hint
 */
extern void FP8_BLS24479_inv(FP8_BLS24479 *x, FP8_BLS24479 *y, FP_BLS24479 *h);
/**	@brief Formats and outputs an FP8 to the console
 *
	@param x FP8 instance to be printed
 */
extern void FP8_BLS24479_output(FP8_BLS24479 *x);

/**	@brief Divide an FP8 by 2
 *
	@param x FP8 instance, on exit = y/2
	@param y FP8 instance
 */
extern void FP8_BLS24479_div2(FP8_BLS24479 *x, FP8_BLS24479 *y);

/**	@brief Formats and outputs an FP8 to the console in raw form (for debugging)
 *
	@param x FP8 instance to be printed
 */
extern void FP8_BLS24479_rawoutput(FP8_BLS24479 *x);
/**	@brief multiplies an FP8 instance by irreducible polynomial sqrt(1+sqrt(-1))
 *
	@param x FP8 instance, on exit = sqrt(1+sqrt(-1)*x
 */
extern void FP8_BLS24479_times_i(FP8_BLS24479 *x);
/**	@brief multiplies an FP8 instance by irreducible polynomial (1+sqrt(-1))
 *
	@param x FP8 instance, on exit = (1+sqrt(-1)*x
 */
extern void FP8_BLS24479_times_i2(FP8_BLS24479 *x);

/**	@brief Normalises the components of an FP8
 *
	@param x FP8 instance to be normalised
 */
extern void FP8_BLS24479_norm(FP8_BLS24479 *x);
/**	@brief Reduces all components of possibly unreduced FP8 mod Modulus
 *
	@param x FP8 instance, on exit reduced mod Modulus
 */
extern void FP8_BLS24479_reduce(FP8_BLS24479 *x);
/**	@brief Raises an FP8 to the power of a BIG
 *
	@param x FP8 instance, on exit = y^b
	@param y FP8 instance
	@param b BIG number
 */
extern void FP8_BLS24479_pow(FP8_BLS24479 *x, FP8_BLS24479 *y, BIG_480_29 b);
/**	@brief Raises an FP8 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP8 instance, on exit = x^p
	@param f FP2 precalculated Frobenius constant
 */
extern void FP8_BLS24479_frob(FP8_BLS24479 *x, FP2_BLS24479 *f);
/**	@brief Calculates the XTR addition function r=w*x-conj(x)*y+z
 *
	@param r FP8 instance, on exit = w*x-conj(x)*y+z
	@param w FP8 instance
	@param x FP8 instance
	@param y FP8 instance
	@param z FP8 instance
 */
extern void FP8_BLS24479_xtr_A(FP8_BLS24479 *r, FP8_BLS24479 *w, FP8_BLS24479 *x, FP8_BLS24479 *y, FP8_BLS24479 *z);
/**	@brief Calculates the XTR doubling function r=x^2-2*conj(x)
 *
	@param r FP8 instance, on exit = x^2-2*conj(x)
	@param x FP8 instance
 */
extern void FP8_BLS24479_xtr_D(FP8_BLS24479 *r, FP8_BLS24479 *x);
/**	@brief Calculates FP8 trace of an FP12 raised to the power of a BIG number
 *
	XTR single exponentiation
	@param r FP8 instance, on exit = trace(w^b)
	@param x FP8 instance, trace of an FP12 w
	@param b BIG number
 */
extern void FP8_BLS24479_xtr_pow(FP8_BLS24479 *r, FP8_BLS24479 *x, BIG_480_29 b);
/**	@brief Calculates FP8 trace of c^a.d^b, where c and d are derived from FP8 traces of FP12s
 *
	XTR double exponentiation
	Assumes c=tr(x^m), d=tr(x^n), e=tr(x^(m-n)), f=tr(x^(m-2n))
	@param r FP8 instance, on exit = trace(c^a.d^b)
	@param c FP8 instance, trace of an FP12
	@param d FP8 instance, trace of an FP12
	@param e FP8 instance, trace of an FP12
	@param f FP8 instance, trace of an FP12
	@param a BIG number
	@param b BIG number
 */
extern void FP8_BLS24479_xtr_pow2(FP8_BLS24479 *r, FP8_BLS24479 *c, FP8_BLS24479 *d, FP8_BLS24479 *e, FP8_BLS24479 *f, BIG_480_29 a, BIG_480_29 b);

/**	@brief Test FP8 for QR
 *
	@param r FP8 instance
    @param h optional generated hint
	@return 1 r is a QR, otherwise 0
 */
extern int  FP8_BLS24479_qr(FP8_BLS24479 *r, FP_BLS24479 *h);

/**	@brief Calculate square root of an FP8
 *
	Square root
	@param r FP8 instance, on exit = sqrt(x)
	@param x FP8 instance
	@param h optional input hint
 */
extern void  FP8_BLS24479_sqrt(FP8_BLS24479 *r, FP8_BLS24479 *x, FP_BLS24479 *h);


/**	@brief Conditional copy of FP8 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP8 instance, set to y if s!=0
	@param y another FP8 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP8_BLS24479_cmove(FP8_BLS24479 *x, FP8_BLS24479 *y, int s);


/**	@brief Divide FP8 number by QNR
 *
	Divide FP8 by the QNR
	@param x FP8 instance
 */
extern void FP8_BLS24479_div_i(FP8_BLS24479 *x);

/**	@brief Divide FP8 number by QNR twice
 *
	Divide FP8 by the QNR twice
	@param x FP8 instance
 */
extern void FP8_BLS24479_div_i2(FP8_BLS24479 *x);

/**	@brief Divide FP8 number by QNR/2
 *
	Divide FP8 by the QNR/2
	@param x FP8 instance
 */
extern void FP8_BLS24479_div_2i(FP8_BLS24479 *x);

/**	@brief Generate random FP8
 *
	@param x random FP8 number
	@param rng random number generator
 */
extern void FP8_BLS24479_rand(FP8_BLS24479 *x, csprng *rng);
#endif


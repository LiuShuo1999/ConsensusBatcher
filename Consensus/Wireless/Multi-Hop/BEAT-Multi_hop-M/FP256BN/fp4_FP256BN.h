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
 * @file fp4.h
 * @author Mike Scott
 * @brief FP4 Header File
 *
 */

#ifndef FP4_FP256BN_H
#define FP4_FP256BN_H

#include "fp2_FP256BN.h"
#include "config_curve_FP256BN.h"

/**
	@brief FP4 Structure - towered over two FP2
*/

typedef struct
{
    FP2_FP256BN a; /**< real part of FP4 */
    FP2_FP256BN b; /**< imaginary part of FP4 */
} FP4_FP256BN;


/* FP4 prototypes */
/**	@brief Tests for FP4 equal to zero
 *
	@param x FP4 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP4_FP256BN_iszilch(FP4_FP256BN *x);

/**	@brief Tests for lexically larger 
 *
	@param x FP4 number to be tested if larger than -x
	@return 1 if larger, else returns 0
 */
extern int FP4_FP256BN_islarger(FP4_FP256BN *x);

/**	@brief Serialize out FP4  
 *
    @param b buffer for output
	@param x FP4 number to be serialized
 */
extern void FP4_FP256BN_toBytes(char *b,FP4_FP256BN *x);

/**	@brief Serialize in FP4  
 *
	@param x FP4 number to be serialized
    @param b buffer for input
 */
extern void FP4_FP256BN_fromBytes(FP4_FP256BN *x,char *b);


/**	@brief Tests for FP4 equal to unity
 *
	@param x FP4 number to be tested
	@return 1 if unity, else returns 0
 */
extern int FP4_FP256BN_isunity(FP4_FP256BN *x);
/**	@brief Tests for equality of two FP4s
 *
	@param x FP4 instance to be compared
	@param y FP4 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP4_FP256BN_equals(FP4_FP256BN *x, FP4_FP256BN *y);
/**	@brief Tests for FP4 having only a real part and no imaginary part
 *
	@param x FP4 number to be tested
	@return 1 if real, else returns 0
 */
extern int FP4_FP256BN_isreal(FP4_FP256BN *x);
/**	@brief Initialise FP4 from two FP2s
 *
	@param x FP4 instance to be initialised
	@param a FP2 to form real part of FP4
	@param b FP2 to form imaginary part of FP4
 */
extern void FP4_FP256BN_from_FP2s(FP4_FP256BN *x, FP2_FP256BN *a, FP2_FP256BN *b);
/**	@brief Initialise FP4 from single FP2
 *
	Imaginary part is set to zero
	@param x FP4 instance to be initialised
	@param a FP2 to form real part of FP4
 */
extern void FP4_FP256BN_from_FP2(FP4_FP256BN *x, FP2_FP256BN *a);

/**	@brief Initialise FP4 from single FP2
 *
	real part is set to zero
	@param x FP4 instance to be initialised
	@param a FP2 to form imaginary part of FP4
 */
extern void FP4_FP256BN_from_FP2H(FP4_FP256BN *x, FP2_FP256BN *a);

/**	@brief Initialise FP4 from single FP
 *
	@param x FP4 instance to be initialised
	@param a FP to form real part of FP4
 */
extern void FP4_FP256BN_from_FP(FP4_FP256BN *x, FP_FP256BN *a);

/**	@brief Copy FP4 to another FP4
 *
	@param x FP4 instance, on exit = y
	@param y FP4 instance to be copied
 */
extern void FP4_FP256BN_copy(FP4_FP256BN *x, FP4_FP256BN *y);
/**	@brief Set FP4 to zero
 *
	@param x FP4 instance to be set to zero
 */
extern void FP4_FP256BN_zero(FP4_FP256BN *x);
/**	@brief Set FP4 to unity
 *
	@param x FP4 instance to be set to one
 */
extern void FP4_FP256BN_one(FP4_FP256BN *x);

/**	@brief Sign of FP4
 *
	@param x FP4 instance
	@return "sign" of FP4
 */
extern int FP4_FP256BN_sign(FP4_FP256BN *x);

/**	@brief Negation of FP4
 *
	@param x FP4 instance, on exit = -y
	@param y FP4 instance
 */
extern void FP4_FP256BN_neg(FP4_FP256BN *x, FP4_FP256BN *y);
/**	@brief Conjugation of FP4
 *
	If y=(a,b) on exit x=(a,-b)
	@param x FP4 instance, on exit = conj(y)
	@param y FP4 instance
 */
extern void FP4_FP256BN_conj(FP4_FP256BN *x, FP4_FP256BN *y);
/**	@brief Negative conjugation of FP4
 *
	If y=(a,b) on exit x=(-a,b)
	@param x FP4 instance, on exit = -conj(y)
	@param y FP4 instance
 */
extern void FP4_FP256BN_nconj(FP4_FP256BN *x, FP4_FP256BN *y);
/**	@brief addition of two FP4s
 *
	@param x FP4 instance, on exit = y+z
	@param y FP4 instance
	@param z FP4 instance
 */
extern void FP4_FP256BN_add(FP4_FP256BN *x, FP4_FP256BN *y, FP4_FP256BN *z);
/**	@brief subtraction of two FP4s
 *
	@param x FP4 instance, on exit = y-z
	@param y FP4 instance
	@param z FP4 instance
 */
extern void FP4_FP256BN_sub(FP4_FP256BN *x, FP4_FP256BN *y, FP4_FP256BN *z);
/**	@brief Multiplication of an FP4 by an FP2
 *
	@param x FP4 instance, on exit = y*a
	@param y FP4 instance
	@param a FP2 multiplier
 */
extern void FP4_FP256BN_pmul(FP4_FP256BN *x, FP4_FP256BN *y, FP2_FP256BN *a);

/**	@brief Multiplication of an FP4 by an FP
 *
	@param x FP4 instance, on exit = y*a
	@param y FP4 instance
	@param a FP multiplier
 */
extern void FP4_FP256BN_qmul(FP4_FP256BN *x, FP4_FP256BN *y, FP_FP256BN *a);

/**	@brief Multiplication of an FP4 by a small integer
 *
	@param x FP4 instance, on exit = y*i
	@param y FP4 instance
	@param i an integer
 */
extern void FP4_FP256BN_imul(FP4_FP256BN *x, FP4_FP256BN *y, int i);
/**	@brief Squaring an FP4
 *
	@param x FP4 instance, on exit = y^2
	@param y FP4 instance
 */
extern void FP4_FP256BN_sqr(FP4_FP256BN *x, FP4_FP256BN *y);
/**	@brief Multiplication of two FP4s
 *
	@param x FP4 instance, on exit = y*z
	@param y FP4 instance
	@param z FP4 instance
 */
extern void FP4_FP256BN_mul(FP4_FP256BN *x, FP4_FP256BN *y, FP4_FP256BN *z);
/**	@brief Inverting an FP4
 *
	@param x FP4 instance, on exit = 1/y
	@param y FP4 instance
    @param h optional input hint
 */
extern void FP4_FP256BN_inv(FP4_FP256BN *x, FP4_FP256BN *y, FP_FP256BN *h);
/**	@brief Formats and outputs an FP4 to the console
 *
	@param x FP4 instance to be printed
 */
extern void FP4_FP256BN_output(FP4_FP256BN *x);
/**	@brief Formats and outputs an FP4 to the console in raw form (for debugging)
 *
	@param x FP4 instance to be printed
 */
extern void FP4_FP256BN_rawoutput(FP4_FP256BN *x);
/**	@brief multiplies an FP4 instance by irreducible polynomial sqrt(1+sqrt(-1))
 *
	@param x FP4 instance, on exit = sqrt(1+sqrt(-1)*x
 */
extern void FP4_FP256BN_times_i(FP4_FP256BN *x);
/**	@brief Normalises the components of an FP4
 *
	@param x FP4 instance to be normalised
 */
extern void FP4_FP256BN_norm(FP4_FP256BN *x);
/**	@brief Reduces all components of possibly unreduced FP4 mod Modulus
 *
	@param x FP4 instance, on exit reduced mod Modulus
 */
extern void FP4_FP256BN_reduce(FP4_FP256BN *x);
/**	@brief Raises an FP4 to the power of a BIG
 *
	@param x FP4 instance, on exit = y^b
	@param y FP4 instance
	@param b BIG number
 */
extern void FP4_FP256BN_pow(FP4_FP256BN *x, FP4_FP256BN *y, BIG_256_28 b);
/**	@brief Raises an FP4 to the power of the internal modulus p, using the Frobenius
 *
	@param x FP4 instance, on exit = x^p
	@param f FP2 precalculated Frobenius constant
 */
extern void FP4_FP256BN_frob(FP4_FP256BN *x, FP2_FP256BN *f);
/**	@brief Calculates the XTR addition function r=w*x-conj(x)*y+z
 *
	@param r FP4 instance, on exit = w*x-conj(x)*y+z
	@param w FP4 instance
	@param x FP4 instance
	@param y FP4 instance
	@param z FP4 instance
 */
extern void FP4_FP256BN_xtr_A(FP4_FP256BN *r, FP4_FP256BN *w, FP4_FP256BN *x, FP4_FP256BN *y, FP4_FP256BN *z);
/**	@brief Calculates the XTR doubling function r=x^2-2*conj(x)
 *
	@param r FP4 instance, on exit = x^2-2*conj(x)
	@param x FP4 instance
 */
extern void FP4_FP256BN_xtr_D(FP4_FP256BN *r, FP4_FP256BN *x);
/**	@brief Calculates FP4 trace of an FP12 raised to the power of a BIG number
 *
	XTR single exponentiation
	@param r FP4 instance, on exit = trace(w^b)
	@param x FP4 instance, trace of an FP12 w
	@param b BIG number
 */
extern void FP4_FP256BN_xtr_pow(FP4_FP256BN *r, FP4_FP256BN *x, BIG_256_28 b);
/**	@brief Calculates FP4 trace of c^a.d^b, where c and d are derived from FP4 traces of FP12s
 *
	XTR double exponentiation
	Assumes c=tr(x^m), d=tr(x^n), e=tr(x^(m-n)), f=tr(x^(m-2n))
	@param r FP4 instance, on exit = trace(c^a.d^b)
	@param c FP4 instance, trace of an FP12
	@param d FP4 instance, trace of an FP12
	@param e FP4 instance, trace of an FP12
	@param f FP4 instance, trace of an FP12
	@param a BIG number
	@param b BIG number
 */
extern void FP4_FP256BN_xtr_pow2(FP4_FP256BN *r, FP4_FP256BN *c, FP4_FP256BN *d, FP4_FP256BN *e, FP4_FP256BN *f, BIG_256_28 a, BIG_256_28 b);

/**	@brief Conditional copy of FP4 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP4 instance, set to y if s!=0
	@param y another FP4 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP4_FP256BN_cmove(FP4_FP256BN *x, FP4_FP256BN *y, int s);

/**	@brief Test FP4 for QR
 * 
	@param r FP4 instance
    @param h optional generated hint
	@return 1 x is a QR, otherwise 0
 */
extern int  FP4_FP256BN_qr(FP4_FP256BN *r, FP_FP256BN *h);

/**	@brief Calculate square root of an FP4
 *
	Square root
	@param r FP4 instance, on exit = sqrt(x)
	@param x FP4 instance
	@param h optional input hint
 */
extern void  FP4_FP256BN_sqrt(FP4_FP256BN *r, FP4_FP256BN *x, FP_FP256BN *h);


/**	@brief Divide FP4 number by QNR
 *
	Divide FP4 by the QNR
	@param x FP4 instance
 */
extern void FP4_FP256BN_div_i(FP4_FP256BN *x);

/**	@brief Divide an FP4 by QNR/2
 *
	Divide FP4 by the QNR/2
	@param x FP4 instance
 */
extern void FP4_FP256BN_div_2i(FP4_FP256BN *x);



/**	@brief Divide an FP4 by 2
 *
	@param x FP4 instance, on exit = y/2
	@param y FP4 instance
 */
extern void FP4_FP256BN_div2(FP4_FP256BN *x, FP4_FP256BN *y);

/**	@brief Generate random FP4
 *
	@param x random FP4 number
	@param rng random number generator
 */
extern void FP4_FP256BN_rand(FP4_FP256BN *x, csprng *rng);

#endif


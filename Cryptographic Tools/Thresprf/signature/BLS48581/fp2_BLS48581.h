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
 * @file fp2.h
 * @author Mike Scott
 * @brief FP2 Header File
 *
 */

#ifndef FP2_BLS48581_H
#define FP2_BLS48581_H

#include "fp_BLS48581.h"

/**
	@brief FP2 Structure - quadratic extension field
*/

typedef struct
{
    FP_BLS48581 a; /**< real part of FP2 */
    FP_BLS48581 b; /**< imaginary part of FP2 */
} FP2_BLS48581;

/* FP2 prototypes */

/**	@brief Tests for FP2 equal to zero
 *
	@param x FP2 number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP2_BLS48581_iszilch(FP2_BLS48581 *x);

/**	@brief Tests for lexically larger 
 *
	@param x FP2 number to be tested if larger than -x
	@return 1 if larger, else returns 0
 */
extern int FP2_BLS48581_islarger(FP2_BLS48581 *x);


/**	@brief Serialize out FP2  
 *
    @param b buffer for output
	@param x FP2 number to be serialized
 */
extern void FP2_BLS48581_toBytes(char *b,FP2_BLS48581 *x);

/**	@brief Serialize in FP2  
 *
	@param x FP2 number to be serialized
    @param b buffer for input
 */
extern void FP2_BLS48581_fromBytes(FP2_BLS48581 *x,char *b);


/**	@brief Conditional copy of FP2 number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x FP2 instance, set to y if s!=0
	@param y another FP2 instance
	@param s copy only takes place if not equal to 0
 */
extern void FP2_BLS48581_cmove(FP2_BLS48581 *x, FP2_BLS48581 *y, int s);
/**	@brief Tests for FP2 equal to one
 *
	@param x FP2 instance to be tested
	@return 1 if x=1, else returns 0
 */
extern int FP2_BLS48581_isunity(FP2_BLS48581 *x);
/**	@brief Tests for equality of two FP2s
 *
	@param x FP2 instance to be compared
	@param y FP2 instance to be compared
	@return 1 if x=y, else returns 0
 */
extern int FP2_BLS48581_equals(FP2_BLS48581 *x, FP2_BLS48581 *y);
/**	@brief Initialise FP2 from two FP numbers
 *
	@param x FP2 instance to be initialised
	@param a FP to form real part of FP2
	@param b FP to form imaginary part of FP2
 */
extern void FP2_BLS48581_from_FPs(FP2_BLS48581 *x, FP_BLS48581 *a, FP_BLS48581 *b);
/**	@brief Initialise FP2 from two BIG integers
 *
	@param x FP2 instance to be initialised
	@param a BIG to form real part of FP2
	@param b BIG to form imaginary part of FP2
 */
extern void FP2_BLS48581_from_BIGs(FP2_BLS48581 *x, BIG_584_29 a, BIG_584_29 b);


/**	@brief Initialise FP2 from two integers
 *
	@param x FP2 instance to be initialised
	@param a int to form real part of FP2
	@param b int to form imaginary part of FP2
 */
extern void FP2_BLS48581_from_ints(FP2_BLS48581 *x, int a, int b);



/**	@brief Initialise FP2 from single FP
 *
	Imaginary part is set to zero
	@param x FP2 instance to be initialised
	@param a FP to form real part of FP2
 */
extern void FP2_BLS48581_from_FP(FP2_BLS48581 *x, FP_BLS48581 *a);
/**	@brief Initialise FP2 from single BIG
 *
	Imaginary part is set to zero
	@param x FP2 instance to be initialised
	@param a BIG to form real part of FP2
 */
extern void FP2_BLS48581_from_BIG(FP2_BLS48581 *x, BIG_584_29 a);
/**	@brief Copy FP2 to another FP2
 *
	@param x FP2 instance, on exit = y
	@param y FP2 instance to be copied
 */
extern void FP2_BLS48581_copy(FP2_BLS48581 *x, FP2_BLS48581 *y);
/**	@brief Set FP2 to zero
 *
	@param x FP2 instance to be set to zero
 */
extern void FP2_BLS48581_zero(FP2_BLS48581 *x);
/**	@brief Set FP2 to unity
 *
	@param x FP2 instance to be set to one
 */
extern void FP2_BLS48581_one(FP2_BLS48581 *x);

/**	@brief Copy from ROM to an FP2
 *
	@param w FP2 number to be copied to
	@param a BIG real part to be copied from ROM
	@param b BIG imag part to be copied from ROM
 */
extern void FP2_BLS48581_rcopy(FP2_BLS48581 *w,const BIG_584_29 a,const BIG_584_29 b);

/**	@brief Sign of FP2
 *
	@param x FP2 instance
	@return "sign" of FP2
 */
extern int FP2_BLS48581_sign(FP2_BLS48581 *x);

/**	@brief Negation of FP2
 *
	@param x FP2 instance, on exit = -y
	@param y FP2 instance
 */
extern void FP2_BLS48581_neg(FP2_BLS48581 *x, FP2_BLS48581 *y);
/**	@brief Conjugation of FP2
 *
	If y=(a,b) on exit x=(a,-b)
	@param x FP2 instance, on exit = conj(y)
	@param y FP2 instance
 */
extern void FP2_BLS48581_conj(FP2_BLS48581 *x, FP2_BLS48581 *y);
/**	@brief addition of two FP2s
 *
	@param x FP2 instance, on exit = y+z
	@param y FP2 instance
	@param z FP2 instance
 */
extern void FP2_BLS48581_add(FP2_BLS48581 *x, FP2_BLS48581 *y, FP2_BLS48581 *z);
/**	@brief subtraction of two FP2s
 *
	@param x FP2 instance, on exit = y-z
	@param y FP2 instance
	@param z FP2 instance
 */
extern void FP2_BLS48581_sub(FP2_BLS48581 *x, FP2_BLS48581 *y, FP2_BLS48581 *z);
/**	@brief Multiplication of an FP2 by an FP
 *
	@param x FP2 instance, on exit = y*b
	@param y FP2 instance
	@param b FP residue
 */
extern void FP2_BLS48581_pmul(FP2_BLS48581 *x, FP2_BLS48581 *y, FP_BLS48581 *b);
/**	@brief Multiplication of an FP2 by a small integer
 *
	@param x FP2 instance, on exit = y*i
	@param y FP2 instance
	@param i an integer
 */
extern void FP2_BLS48581_imul(FP2_BLS48581 *x, FP2_BLS48581 *y, int i);
/**	@brief Squaring an FP2
 *
	@param x FP2 instance, on exit = y^2
	@param y FP2 instance
 */
extern void FP2_BLS48581_sqr(FP2_BLS48581 *x, FP2_BLS48581 *y);
/**	@brief Multiplication of two FP2s
 *
	@param x FP2 instance, on exit = y*z
	@param y FP2 instance
	@param z FP2 instance
 */
extern void FP2_BLS48581_mul(FP2_BLS48581 *x, FP2_BLS48581 *y, FP2_BLS48581 *z);
/**	@brief Formats and outputs an FP2 to the console
 *
	@param x FP2 instance
 */
extern void FP2_BLS48581_output(FP2_BLS48581 *x);
/**	@brief Formats and outputs an FP2 to the console in raw form (for debugging)
 *
	@param x FP2 instance
 */
extern void FP2_BLS48581_rawoutput(FP2_BLS48581 *x);
/**	@brief Inverting an FP2
 *
	@param x FP2 instance, on exit = 1/y
	@param y FP2 instance
    @param h optional input hint
 */
extern void FP2_BLS48581_inv(FP2_BLS48581 *x, FP2_BLS48581 *y, FP_BLS48581 *h);
/**	@brief Divide an FP2 by 2
 *
	@param x FP2 instance, on exit = y/2
	@param y FP2 instance
 */
extern void FP2_BLS48581_div2(FP2_BLS48581 *x, FP2_BLS48581 *y);
/**	@brief Multiply an FP2 by (1+sqrt(-1))
 *
	Note that (1+sqrt(-1)) is irreducible for FP4
	@param x FP2 instance, on exit = x*(1+sqrt(-1))
 */
extern void FP2_BLS48581_mul_ip(FP2_BLS48581 *x);
/**	@brief Divide an FP2 by (1+sqrt(-1))/2 -
 *
	Note that (1+sqrt(-1)) is irreducible for FP4
	@param x FP2 instance, on exit = 2x/(1+sqrt(-1))
 */
extern void FP2_BLS48581_div_ip2(FP2_BLS48581 *x);
/**	@brief Divide an FP2 by (1+sqrt(-1))
 *
	Note that (1+sqrt(-1)) is irreducible for FP4
	@param x FP2 instance, on exit = x/(1+sqrt(-1))
 */
extern void FP2_BLS48581_div_ip(FP2_BLS48581 *x);
/**	@brief Normalises the components of an FP2
 *
	@param x FP2 instance to be normalised
 */
extern void FP2_BLS48581_norm(FP2_BLS48581 *x);
/**	@brief Reduces all components of possibly unreduced FP2 mod Modulus
 *
	@param x FP2 instance, on exit reduced mod Modulus
 */
extern void FP2_BLS48581_reduce(FP2_BLS48581 *x);
/**	@brief Raises an FP2 to the power of a BIG
 *
	@param x FP2 instance, on exit = y^b
	@param y FP2 instance
	@param b BIG number
 */
extern void FP2_BLS48581_pow(FP2_BLS48581 *x, FP2_BLS48581 *y, BIG_584_29 b);

/**	@brief Test FP2 for QR
 *
	@param x FP2 instance
    @param h optional generated hint
	@return true or false
 */
extern int FP2_BLS48581_qr(FP2_BLS48581 *x, FP_BLS48581 *h);

/**	@brief Square root of an FP2
 *
	@param x FP2 instance, on exit = sqrt(y)
	@param y FP2 instance
    @param h optional input hint
 */
extern void FP2_BLS48581_sqrt(FP2_BLS48581 *x, FP2_BLS48581 *y, FP_BLS48581 *h);

/**	@brief Multiply an FP2 by sqrt(-1)
 *
	Note that -1 is QNR
	@param x FP2 instance, on exit = x*sqrt(-1)
 */
extern void FP2_BLS48581_times_i(FP2_BLS48581 *x);
/**	@brief Generate random FP2
 *
	@param x random FP2 number
	@param rng random number generator
 */
extern void FP2_BLS48581_rand(FP2_BLS48581 *x, csprng *rng);
#endif

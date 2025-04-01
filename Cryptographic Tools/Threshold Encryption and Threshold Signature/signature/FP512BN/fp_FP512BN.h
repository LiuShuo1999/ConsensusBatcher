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
 * @file fp.h
 * @author Mike Scott
 * @brief FP Header File
 *
 */

#ifndef FP_FP512BN_H
#define FP_FP512BN_H

#include "big_512_29.h"
#include "config_field_FP512BN.h"


/**
	@brief FP Structure - quadratic extension field
*/

typedef struct
{
    BIG_512_29 g;	/**< Big representation of field element */
    sign32 XES;	/**< Excess */
} FP_FP512BN;


/* Field Params - see rom.c */
extern const BIG_512_29 Modulus_FP512BN;	/**< Actual Modulus set in rom_field_yyy.c */
extern const BIG_512_29 ROI_FP512BN;	    /**< Root of unity set in rom_field_yyy.c */
extern const BIG_512_29 R2modp_FP512BN;	/**< Montgomery constant */
extern const BIG_512_29 CRu_FP512BN;       /**< Cube Root of Unity */
extern const BIG_512_29 SQRTm3_FP512BN; /**< Square root of -3 */
extern const BIG_512_29 TWK_FP512BN; /**< Tweak for square roots, pre-calculated from field norm */
extern const chunk MConst_FP512BN;		/**< Constant associated with Modulus - for Montgomery = 1/p mod 2^BASEBITS */


#define MODBITS_FP512BN MBITS_FP512BN                        /**< Number of bits in Modulus for selected curve */
#define TBITS_FP512BN (MBITS_FP512BN%BASEBITS_512_29)           /**< Number of active bits in top word */
#define TMASK_FP512BN (((chunk)1<<TBITS_FP512BN)-1)          /**< Mask for active bits in top word */
#define FEXCESS_FP512BN (((sign32)1<<MAXXES_FP512BN)-1)	     /**< 2^(BASEBITS*NLEN-MODBITS)-1 - normalised BIG can be multiplied by less than this before reduction */
#define OMASK_FP512BN (-((chunk)(1)<<TBITS_FP512BN))         /**<  for masking out overflow bits */

//#define BIG_ENDIAN_SIGN_FP512BN 

//#define FUSED_MODMUL
//#define DEBUG_REDUCE

/* FP prototypes */

/**	@brief Create FP from integer
 *
	@param x FP to be initialised
	@param a integer
 */
extern void FP_FP512BN_from_int(FP_FP512BN *x,int a);

/**	@brief Tests for FP equal to zero mod Modulus
 *
	@param x FP number to be tested
	@return 1 if zero, else returns 0
 */
extern int FP_FP512BN_iszilch(FP_FP512BN *x);


/**	@brief Tests for lexically largest 
 *
	@param x FP number to be tested if larger than -x
	@return 1 if larger, else returns 0
 */
extern int FP_FP512BN_islarger(FP_FP512BN *x);

/**	@brief Serialize out FP  
 *
    @param b buffer for output
	@param x FP number to be serialized
 */
extern void FP_FP512BN_toBytes(char *b,FP_FP512BN *x);

/**	@brief Serialize in FP  
 *
	@param x FP number to be serialized
    @param b buffer for input
 */
extern void FP_FP512BN_fromBytes(FP_FP512BN *x,char *b);



/**	@brief Tests for FP equal to one mod Modulus
 *
	@param x FP number to be tested
	@return 1 if one, else returns 0
 */
extern int FP_FP512BN_isunity(FP_FP512BN *x);


/**	@brief Set FP to zero
 *
	@param x FP number to be set to 0
 */
extern void FP_FP512BN_zero(FP_FP512BN *x);

/**	@brief Copy an FP
 *
	@param y FP number to be copied to
	@param x FP to be copied from
 */
extern void FP_FP512BN_copy(FP_FP512BN *y, FP_FP512BN *x);

/**	@brief Copy from ROM to an FP
 *
	@param y FP number to be copied to
	@param x BIG to be copied from ROM
 */
extern void FP_FP512BN_rcopy(FP_FP512BN *y, const BIG_512_29 x);


/**	@brief Compares two FPs
 *
	@param x FP number
	@param y FP number
	@return 1 if equal, else returns 0
 */
extern int FP_FP512BN_equals(FP_FP512BN *x, FP_FP512BN *y);


/**	@brief Conditional constant time swap of two FP numbers
 *
	Conditionally swaps parameters in constant time (without branching)
	@param x an FP number
	@param y another FP number
	@param s swap takes place if not equal to 0
 */
extern void FP_FP512BN_cswap(FP_FP512BN *x, FP_FP512BN *y, int s);
/**	@brief Conditional copy of FP number
 *
	Conditionally copies second parameter to the first (without branching)
	@param x an FP number
	@param y another FP number
	@param s copy takes place if not equal to 0
 */
extern void FP_FP512BN_cmove(FP_FP512BN *x, FP_FP512BN *y, int s);
/**	@brief Converts from BIG integer to residue form mod Modulus
 *
	@param x BIG number to be converted
	@param y FP result
 */
extern void FP_FP512BN_nres(FP_FP512BN *y, BIG_512_29 x);
/**	@brief Converts from residue form back to BIG integer form
 *
	@param y FP number to be converted to BIG
	@param x BIG result
 */
extern void FP_FP512BN_redc(BIG_512_29 x, FP_FP512BN *y);
/**	@brief Sets FP to representation of unity in residue form
 *
	@param x FP number to be set equal to unity.
 */
extern void FP_FP512BN_one(FP_FP512BN *x);


/**	@brief returns "sign" of an FP
 *
	@param x FP number
    @return 0 for positive, 1 for negative
 */
extern int FP_FP512BN_sign(FP_FP512BN *x);


/**	@brief Reduces DBIG to BIG exploiting special form of the modulus
 *
	This function comes in different flavours depending on the form of Modulus that is currently in use.
	@param r BIG number, on exit = d mod Modulus
	@param d DBIG number to be reduced
 */
extern void FP_FP512BN_mod(BIG_512_29 r, DBIG_512_29 d);

#ifdef FUSED_MODMUL
extern void FP_FP512BN_modmul(BIG_512_29, BIG_512_29, BIG_512_29);
#endif

/**	@brief Fast Modular multiplication of two FPs, mod Modulus
 *
	Uses appropriate fast modular reduction method
	@param x FP number, on exit the modular product = y*z mod Modulus
	@param y FP number, the multiplicand
	@param z FP number, the multiplier
 */
extern void FP_FP512BN_mul(FP_FP512BN *x, FP_FP512BN *y, FP_FP512BN *z);
/**	@brief Fast Modular multiplication of an FP, by a small integer, mod Modulus
 *
	@param x FP number, on exit the modular product = y*i mod Modulus
	@param y FP number, the multiplicand
	@param i a small number, the multiplier
 */
extern void FP_FP512BN_imul(FP_FP512BN *x, FP_FP512BN *y, int i);
/**	@brief Fast Modular squaring of an FP, mod Modulus
 *
	Uses appropriate fast modular reduction method
	@param x FP number, on exit the modular product = y^2 mod Modulus
	@param y FP number, the number to be squared

 */
extern void FP_FP512BN_sqr(FP_FP512BN *x, FP_FP512BN *y);
/**	@brief Modular addition of two FPs, mod Modulus
 *
	@param x FP number, on exit the modular sum = y+z mod Modulus
	@param y FP number
	@param z FP number
 */
extern void FP_FP512BN_add(FP_FP512BN *x, FP_FP512BN *y, FP_FP512BN *z);
/**	@brief Modular subtraction of two FPs, mod Modulus
 *
	@param x FP number, on exit the modular difference = y-z mod Modulus
	@param y FP number
	@param z FP number
 */
extern void FP_FP512BN_sub(FP_FP512BN *x, FP_FP512BN *y, FP_FP512BN *z);
/**	@brief Modular division by 2 of an FP, mod Modulus
 *
	@param x FP number, on exit =y/2 mod Modulus
	@param y FP number
 */
extern void FP_FP512BN_div2(FP_FP512BN *x, FP_FP512BN *y);
/**	@brief Fast Modular exponentiation of an FP, to the power of a BIG, mod Modulus
 *
	@param x FP number, on exit  = y^z mod Modulus
	@param y FP number
	@param z BIG number exponent
 */
extern void FP_FP512BN_pow(FP_FP512BN *x, FP_FP512BN *y, BIG_512_29 z);


/**	@brief Inverse square root precalculation
 *
	@param r FP number, on exit  = x^(p-2*e-1)/2^(e+1) mod Modulus
	@param x FP number
 */
extern void FP_FP512BN_progen(FP_FP512BN *r,FP_FP512BN *x);

/**	@brief Fast Modular square root of a an FP, mod Modulus
 *
	@param x FP number, on exit  = sqrt(y) mod Modulus
	@param y FP number, the number whose square root is calculated
    @param h an optional precalculation
 */
extern void FP_FP512BN_sqrt(FP_FP512BN *x, FP_FP512BN *y, FP_FP512BN *h);

/**	@brief Modular negation of a an FP, mod Modulus
 *
	@param x FP number, on exit = -y mod Modulus
	@param y FP number
 */
extern void FP_FP512BN_neg(FP_FP512BN *x, FP_FP512BN *y);
/**	@brief Outputs an FP number to the console
 *
	Converts from residue form before output
	@param x an FP number
 */
extern void FP_FP512BN_output(FP_FP512BN *x);
/**	@brief Outputs an FP number to the console, in raw form
 *
	@param x a BIG number
 */
extern void FP_FP512BN_rawoutput(FP_FP512BN *x);
/**	@brief Reduces possibly unreduced FP mod Modulus
 *
	@param x FP number, on exit reduced mod Modulus
 */
extern void FP_FP512BN_reduce(FP_FP512BN *x);
/**	@brief normalizes FP
 *
	@param x FP number, on exit normalized
 */
extern void FP_FP512BN_norm(FP_FP512BN *x);
/**	@brief Tests for FP a quadratic residue mod Modulus
 *
	@param x FP number to be tested
    @param h an optional precalculation
	@return 1 if quadratic residue, else returns 0 if quadratic non-residue
 */
extern int FP_FP512BN_qr(FP_FP512BN *x,FP_FP512BN *h);

/**	@brief Simultaneous Inverse and Square root
 *
	@param i FP number, on exit = 1/x mod Modulus
	@param s FP number, on exit = sqrt(x) mod Modulus
	@param x FP number
	@return 1 if quadratic residue, else returns 0 if quadratic non-residue
 */
extern int FP_FP512BN_invsqrt(FP_FP512BN *i,FP_FP512BN *s,FP_FP512BN *x);

/**	@brief Simultaneous Inverse and Square root of different numbers
 *
	@param i FP number, on exit = 1/i mod Modulus
	@param s FP number, on exit = sqrt(s) mod Modulus
	@return 1 if quadratic residue, else returns 0 if quadratic non-residue
 */
extern int FP_FP512BN_tpo(FP_FP512BN *i, FP_FP512BN *s);



/**	@brief Modular inverse of a an FP, mod Modulus
 *
	@param x FP number, on exit = 1/y mod Modulus
	@param y FP number
    @param h an optional input precalculation
 */
extern void FP_FP512BN_inv(FP_FP512BN *x, FP_FP512BN *y, FP_FP512BN *h);

/**	@brief Generate random FP
 *
	@param x random FP number
	@param rng random number generator
 */
extern void FP_FP512BN_rand(FP_FP512BN *x, csprng *rng);



#endif

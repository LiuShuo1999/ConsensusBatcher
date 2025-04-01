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
 * @file config_curve.h
 * @author Mike Scott
 * @brief Config Curve  Header File
 *
 */

#ifndef CONFIG_CURVE_FP256BN_H
#define CONFIG_CURVE_FP256BN_H

#include"core.h"
#include"config_field_FP256BN.h"

// ECP stuff

#define CURVETYPE_FP256BN WEIERSTRASS          /**< Define Curve Type */
#define CURVE_A_FP256BN 0            /**< Curve A parameter */
#define PAIRING_FRIENDLY_FP256BN BN_CURVE   /**< Is curve pairing-friendly */
#define CURVE_SECURITY_FP256BN 128     /**< Curve security level in AES bits */
#define HTC_ISO_FP256BN 0            /**< Use Isogenies for Hash to Curve */

// Permit alternate compression method if 3 spare top bits in field representation
// Must be set manually
//#define ALLOW_ALT_COMPRESS_FP256BN

#if PAIRING_FRIENDLY_FP256BN != NOT_PF

#define HTC_ISO_G2_FP256BN 0        /**< Use Isogenies for G2 Hash to Curve */
#define USE_GLV_FP256BN     /**< Note this method is patented (GLV), so maybe you want to comment this out */
#define USE_GS_G2_FP256BN /**< Well we didn't patent it :) But may be covered by GLV patent :( */
#define USE_GS_GT_FP256BN /**< Not patented, so probably OK to always use this */

#define POSITIVEX 0
#define NEGATIVEX 1

#define SEXTIC_TWIST_FP256BN M_TYPE       /**< Sextic Twist M or D type */
#define SIGN_OF_X_FP256BN NEGATIVEX          /**< Sign of curve parameter */ 

#define ATE_BITS_FP256BN 66           /**< Number of Bits in curve parameter */
#define G2_TABLE_FP256BN 83           /**< Size of table for pairing precomputation for fixed G2 */

#endif

#if CURVE_SECURITY_FP256BN == 128
#define AESKEY_FP256BN 16 /**< Symmetric Key size - 128 bits */
#define HASH_TYPE_FP256BN SHA256  /**< Hash type */
#endif

#if CURVE_SECURITY_FP256BN == 192
#define AESKEY_FP256BN 24 /**< Symmetric Key size - 192 bits */
#define HASH_TYPE_FP256BN SHA384  /**< Hash type */
#endif

#if CURVE_SECURITY_FP256BN == 256
#define AESKEY_FP256BN 32 /**< Symmetric Key size - 256 bits */
#define HASH_TYPE_FP256BN SHA512  /**< Hash type */
#endif



#endif

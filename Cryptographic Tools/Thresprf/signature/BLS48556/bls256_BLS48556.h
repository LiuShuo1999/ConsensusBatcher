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
 * @file bls256.h
 * @author Mike Scott
 * @date 28th Novemebr 2018
 * @brief BLS Header file
 *
 * Allows some user configuration
 * defines structures
 * declares functions
 *
 */

#ifndef BLS256_BLS48556_H
#define BLS256_BLS48556_H

#include "pair8_BLS48556.h"

/* Field size is assumed to be greater than or equal to group size */

#define BGS_BLS48556 MODBYTES_560_29  /**< BLS Group Size */
#define BFS_BLS48556 MODBYTES_560_29  /**< BLS Field Size */

#define BLS_OK           0  /**< Function completed without error */
#define BLS_FAIL        -1  /**< Point is NOT on the curve */

/* BLS API functions */

/** @brief Initialise BLS
 *
    @return BLS_OK if worked, otherwise BLS_FAIL
 */
int BLS_BLS48556_INIT();

/** @brief Generate Key Pair
 *
    @param IKM is an octet containing random Initial Keying Material
    @param S on output a private key
    @param W on output a public key = S*G, where G is fixed generator
    @return BLS_OK
 */
int BLS_BLS48556_KEY_PAIR_GENERATE(octet *IKM, octet* S, octet *W);

/** @brief Calculate a signature
 *
    @param SIG the ouput signature
    @param M is the message to be signed
    @param S an input private key
    @return BLS_OK
 */
int BLS_BLS48556_CORE_SIGN(octet *SIG, octet *M, octet *S);

/** @brief Verify a signature
 *
    @param SIG an input signature
    @param M is the message whose signature is to be verified.
    @param W an public key
    @return BLS_OK if verified, otherwise BLS_FAIL
 */
int BLS_BLS48556_CORE_VERIFY(octet *SIG, octet *M, octet *W);

#endif


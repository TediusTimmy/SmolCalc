/*
BSD 3-Clause License

Copyright (c) 2022, Thomas DiModica
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

* Redistributions of source code must retain the above copyright notice, this
  list of conditions and the following disclaimer.

* Redistributions in binary form must reproduce the above copyright notice,
  this list of conditions and the following disclaimer in the documentation
  and/or other materials provided with the distribution.

* Neither the name of the copyright holder nor the names of its
  contributors may be used to endorse or promote products derived from
  this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#include "gtest/gtest.h"

#include <cmath>

#ifndef MISRAbleC

#include "dm_double.h"
#define DM_DOUBLE_PACK(sign, exponent, significand) ((((uint64_t)(exponent)) << 54) | ((significand) - MIN_SIGNIFICAND) | ((sign) ? SIGN_BIT : 0))
#define DM_DOUBLE_PACK_ALT(sign, exponent, significand) ((((uint64_t)(exponent)) << 54) | (significand) | ((sign) ? SIGN_BIT : 0))

const int16_t SPECIAL_EXPONENT = -512;
const uint64_t DM_INFINITY = 0x1FFFFFFFFFFFFFULL;
const uint64_t SIGN_BIT = 0x20000000000000ULL;
const uint64_t MIN_SIGNIFICAND = 1000000000000000ULL;

#else /* The MISRAbleC version */

#include "dm_double_m.h"
#define DM_DOUBLE_PACK(sign, exponent, significand) ((((uint64_t)(exponent - SPECIAL_EXPONENT)) << 53) | ((significand) - MIN_SIGNIFICAND) | ((sign) ? SIGN_BIT : 0))
#define DM_DOUBLE_PACK_ALT(sign, exponent, significand) ((((uint64_t)(exponent - SPECIAL_EXPONENT)) << 53) | (significand) | ((sign) ? SIGN_BIT : 0))

const int16_t SPECIAL_EXPONENT = -512;
const uint64_t DM_INFINITY = 0x1FFFFFFFFFFFFFULL;
const uint64_t SIGN_BIT =  0x8000000000000000ULL;
const uint64_t MIN_SIGNIFICAND = 1000000000000000ULL;

#endif /* MISRAbleC */

#include "dm_double_pretty.h"

TEST(DMDoubleTest, testComparisons) // It was super easy to copy and modify this code from the SlowFloat tests
 {
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U); // Special exponent, zero significand : zero
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);
   dm_double positiveTwo  = DM_DOUBLE_PACK(0, 0, 2000000000000000ULL);
   dm_double negativeTwo  = DM_DOUBLE_PACK(1, 0, 2000000000000000ULL);
   dm_double positiveTen  = DM_DOUBLE_PACK(0, 1, 1000000000000000ULL);
   dm_double negativeTen  = DM_DOUBLE_PACK(1, 1, 1000000000000000ULL);
   dm_double positiveInf  = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY); // Special exponent, ones significand : infinity
   dm_double negativeInf  = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double nan          = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U); // Special exponent, any other significand : NaN

   EXPECT_EQ(1, dm_double_isequal(positiveZero, positiveZero));
   EXPECT_EQ(1, dm_double_isequal(positiveZero, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(positiveZero, nan));

   EXPECT_EQ(1, dm_double_isequal(negativeZero, positiveZero));
   EXPECT_EQ(1, dm_double_isequal(negativeZero, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(negativeZero, nan));

   EXPECT_EQ(0, dm_double_isequal(positiveOne, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, negativeZero));
   EXPECT_EQ(1, dm_double_isequal(positiveOne, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(positiveOne, nan));

   EXPECT_EQ(0, dm_double_isequal(negativeOne, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, positiveOne));
   EXPECT_EQ(1, dm_double_isequal(negativeOne, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(negativeOne, nan));

   EXPECT_EQ(0, dm_double_isequal(positiveTwo, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, negativeOne));
   EXPECT_EQ(1, dm_double_isequal(positiveTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(positiveTwo, nan));

   EXPECT_EQ(0, dm_double_isequal(negativeTwo, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, positiveTwo));
   EXPECT_EQ(1, dm_double_isequal(negativeTwo, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(negativeTwo, nan));

   EXPECT_EQ(0, dm_double_isequal(positiveTen, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, negativeTwo));
   EXPECT_EQ(1, dm_double_isequal(positiveTen, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(positiveTen, nan));

   EXPECT_EQ(0, dm_double_isequal(negativeTen, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, positiveTen));
   EXPECT_EQ(1, dm_double_isequal(negativeTen, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(negativeTen, nan));

   EXPECT_EQ(0, dm_double_isequal(positiveInf, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, negativeTen));
   EXPECT_EQ(1, dm_double_isequal(positiveInf, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(positiveInf, nan));

   EXPECT_EQ(0, dm_double_isequal(negativeInf, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, positiveInf));
   EXPECT_EQ(1, dm_double_isequal(negativeInf, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(negativeInf, nan));

   EXPECT_EQ(0, dm_double_isequal(nan, positiveZero));
   EXPECT_EQ(0, dm_double_isequal(nan, negativeZero));
   EXPECT_EQ(0, dm_double_isequal(nan, positiveOne));
   EXPECT_EQ(0, dm_double_isequal(nan, negativeOne));
   EXPECT_EQ(0, dm_double_isequal(nan, positiveTwo));
   EXPECT_EQ(0, dm_double_isequal(nan, negativeTwo));
   EXPECT_EQ(0, dm_double_isequal(nan, positiveTen));
   EXPECT_EQ(0, dm_double_isequal(nan, negativeTen));
   EXPECT_EQ(0, dm_double_isequal(nan, positiveInf));
   EXPECT_EQ(0, dm_double_isequal(nan, negativeInf));
   EXPECT_EQ(0, dm_double_isequal(nan, nan));


   EXPECT_EQ(0, dm_double_isunequal(positiveZero, positiveZero));
   EXPECT_EQ(0, dm_double_isunequal(positiveZero, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveZero, nan));

   EXPECT_EQ(0, dm_double_isunequal(negativeZero, positiveZero));
   EXPECT_EQ(0, dm_double_isunequal(negativeZero, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeZero, nan));

   EXPECT_EQ(1, dm_double_isunequal(positiveOne, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, negativeZero));
   EXPECT_EQ(0, dm_double_isunequal(positiveOne, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveOne, nan));

   EXPECT_EQ(1, dm_double_isunequal(negativeOne, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, positiveOne));
   EXPECT_EQ(0, dm_double_isunequal(negativeOne, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeOne, nan));

   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, negativeOne));
   EXPECT_EQ(0, dm_double_isunequal(positiveTwo, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveTwo, nan));

   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_isunequal(negativeTwo, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeTwo, nan));

   EXPECT_EQ(1, dm_double_isunequal(positiveTen, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, negativeTwo));
   EXPECT_EQ(0, dm_double_isunequal(positiveTen, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveTen, nan));

   EXPECT_EQ(1, dm_double_isunequal(negativeTen, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, positiveTen));
   EXPECT_EQ(0, dm_double_isunequal(negativeTen, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeTen, nan));

   EXPECT_EQ(1, dm_double_isunequal(positiveInf, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, negativeTen));
   EXPECT_EQ(0, dm_double_isunequal(positiveInf, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(positiveInf, nan));

   EXPECT_EQ(1, dm_double_isunequal(negativeInf, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, positiveInf));
   EXPECT_EQ(0, dm_double_isunequal(negativeInf, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(negativeInf, nan));

   EXPECT_EQ(1, dm_double_isunequal(nan, positiveZero));
   EXPECT_EQ(1, dm_double_isunequal(nan, negativeZero));
   EXPECT_EQ(1, dm_double_isunequal(nan, positiveOne));
   EXPECT_EQ(1, dm_double_isunequal(nan, negativeOne));
   EXPECT_EQ(1, dm_double_isunequal(nan, positiveTwo));
   EXPECT_EQ(1, dm_double_isunequal(nan, negativeTwo));
   EXPECT_EQ(1, dm_double_isunequal(nan, positiveTen));
   EXPECT_EQ(1, dm_double_isunequal(nan, negativeTen));
   EXPECT_EQ(1, dm_double_isunequal(nan, positiveInf));
   EXPECT_EQ(1, dm_double_isunequal(nan, negativeInf));
   EXPECT_EQ(1, dm_double_isunequal(nan, nan));


   EXPECT_EQ(0, dm_double_islessgreater(positiveZero, positiveZero));
   EXPECT_EQ(0, dm_double_islessgreater(positiveZero, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(positiveZero, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveZero, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveZero, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveZero, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveZero, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveZero, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveZero, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(positiveZero, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(positiveZero, nan));

   EXPECT_EQ(0, dm_double_islessgreater(negativeZero, positiveZero));
   EXPECT_EQ(0, dm_double_islessgreater(negativeZero, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeZero, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeZero, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeZero, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeZero, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeZero, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeZero, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeZero, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(negativeZero, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(negativeZero, nan));

   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, positiveZero));
   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, negativeZero));
   EXPECT_EQ(0, dm_double_islessgreater(positiveOne, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(positiveOne, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(positiveOne, nan));

   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, positiveZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, positiveOne));
   EXPECT_EQ(0, dm_double_islessgreater(negativeOne, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(negativeOne, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(negativeOne, nan));

   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, positiveZero));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, negativeOne));
   EXPECT_EQ(0, dm_double_islessgreater(positiveTwo, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTwo, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(positiveTwo, nan));

   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, positiveZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_islessgreater(negativeTwo, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTwo, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(negativeTwo, nan));

   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, positiveZero));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, negativeTwo));
   EXPECT_EQ(0, dm_double_islessgreater(positiveTen, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(positiveTen, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(positiveTen, nan));

   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, positiveZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, positiveTen));
   EXPECT_EQ(0, dm_double_islessgreater(negativeTen, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(negativeTen, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(negativeTen, nan));

   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, positiveZero));
   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, negativeTen));
   EXPECT_EQ(0, dm_double_islessgreater(positiveInf, positiveInf));
   EXPECT_EQ(1, dm_double_islessgreater(positiveInf, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(positiveInf, nan));

   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, positiveZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, negativeZero));
   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, positiveOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, negativeOne));
   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, positiveTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, negativeTwo));
   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, positiveTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, negativeTen));
   EXPECT_EQ(1, dm_double_islessgreater(negativeInf, positiveInf));
   EXPECT_EQ(0, dm_double_islessgreater(negativeInf, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(negativeInf, nan));

   EXPECT_EQ(0, dm_double_islessgreater(nan, positiveZero));
   EXPECT_EQ(0, dm_double_islessgreater(nan, negativeZero));
   EXPECT_EQ(0, dm_double_islessgreater(nan, positiveOne));
   EXPECT_EQ(0, dm_double_islessgreater(nan, negativeOne));
   EXPECT_EQ(0, dm_double_islessgreater(nan, positiveTwo));
   EXPECT_EQ(0, dm_double_islessgreater(nan, negativeTwo));
   EXPECT_EQ(0, dm_double_islessgreater(nan, positiveTen));
   EXPECT_EQ(0, dm_double_islessgreater(nan, negativeTen));
   EXPECT_EQ(0, dm_double_islessgreater(nan, positiveInf));
   EXPECT_EQ(0, dm_double_islessgreater(nan, negativeInf));
   EXPECT_EQ(0, dm_double_islessgreater(nan, nan));


   EXPECT_EQ(0, dm_double_isunordered(positiveZero, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(positiveZero, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(positiveZero, nan));

   EXPECT_EQ(0, dm_double_isunordered(negativeZero, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(negativeZero, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(negativeZero, nan));

   EXPECT_EQ(0, dm_double_isunordered(positiveOne, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(positiveOne, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(positiveOne, nan));

   EXPECT_EQ(0, dm_double_isunordered(negativeOne, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(negativeOne, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(negativeOne, nan));

   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(positiveTwo, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(positiveTwo, nan));

   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(negativeTwo, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(negativeTwo, nan));

   EXPECT_EQ(0, dm_double_isunordered(positiveTen, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(positiveTen, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(positiveTen, nan));

   EXPECT_EQ(0, dm_double_isunordered(negativeTen, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(negativeTen, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(negativeTen, nan));

   EXPECT_EQ(0, dm_double_isunordered(positiveInf, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(positiveInf, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(positiveInf, nan));

   EXPECT_EQ(0, dm_double_isunordered(negativeInf, positiveZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, negativeZero));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, positiveOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, negativeOne));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, positiveTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, negativeTwo));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, positiveTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, negativeTen));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, positiveInf));
   EXPECT_EQ(0, dm_double_isunordered(negativeInf, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(negativeInf, nan));

   EXPECT_EQ(1, dm_double_isunordered(nan, positiveZero));
   EXPECT_EQ(1, dm_double_isunordered(nan, negativeZero));
   EXPECT_EQ(1, dm_double_isunordered(nan, positiveOne));
   EXPECT_EQ(1, dm_double_isunordered(nan, negativeOne));
   EXPECT_EQ(1, dm_double_isunordered(nan, positiveTwo));
   EXPECT_EQ(1, dm_double_isunordered(nan, negativeTwo));
   EXPECT_EQ(1, dm_double_isunordered(nan, positiveTen));
   EXPECT_EQ(1, dm_double_isunordered(nan, negativeTen));
   EXPECT_EQ(1, dm_double_isunordered(nan, positiveInf));
   EXPECT_EQ(1, dm_double_isunordered(nan, negativeInf));
   EXPECT_EQ(1, dm_double_isunordered(nan, nan));


   EXPECT_EQ(0, dm_double_isless(positiveZero, positiveZero));
   EXPECT_EQ(0, dm_double_isless(positiveZero, negativeZero));
   EXPECT_EQ(1, dm_double_isless(positiveZero, positiveOne));
   EXPECT_EQ(0, dm_double_isless(positiveZero, negativeOne));
   EXPECT_EQ(1, dm_double_isless(positiveZero, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(positiveZero, negativeTwo));
   EXPECT_EQ(1, dm_double_isless(positiveZero, positiveTen));
   EXPECT_EQ(0, dm_double_isless(positiveZero, negativeTen));
   EXPECT_EQ(1, dm_double_isless(positiveZero, positiveInf));
   EXPECT_EQ(0, dm_double_isless(positiveZero, negativeInf));
   EXPECT_EQ(0, dm_double_isless(positiveZero, nan));

   EXPECT_EQ(0, dm_double_isless(negativeZero, positiveZero));
   EXPECT_EQ(0, dm_double_isless(negativeZero, negativeZero));
   EXPECT_EQ(1, dm_double_isless(negativeZero, positiveOne));
   EXPECT_EQ(0, dm_double_isless(negativeZero, negativeOne));
   EXPECT_EQ(1, dm_double_isless(negativeZero, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(negativeZero, negativeTwo));
   EXPECT_EQ(1, dm_double_isless(negativeZero, positiveTen));
   EXPECT_EQ(0, dm_double_isless(negativeZero, negativeTen));
   EXPECT_EQ(1, dm_double_isless(negativeZero, positiveInf));
   EXPECT_EQ(0, dm_double_isless(negativeZero, negativeInf));
   EXPECT_EQ(0, dm_double_isless(negativeZero, nan));

   EXPECT_EQ(0, dm_double_isless(positiveOne, positiveZero));
   EXPECT_EQ(0, dm_double_isless(positiveOne, negativeZero));
   EXPECT_EQ(0, dm_double_isless(positiveOne, positiveOne));
   EXPECT_EQ(0, dm_double_isless(positiveOne, negativeOne));
   EXPECT_EQ(1, dm_double_isless(positiveOne, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(positiveOne, negativeTwo));
   EXPECT_EQ(1, dm_double_isless(positiveOne, positiveTen));
   EXPECT_EQ(0, dm_double_isless(positiveOne, negativeTen));
   EXPECT_EQ(1, dm_double_isless(positiveOne, positiveInf));
   EXPECT_EQ(0, dm_double_isless(positiveOne, negativeInf));
   EXPECT_EQ(0, dm_double_isless(positiveOne, nan));

   EXPECT_EQ(1, dm_double_isless(negativeOne, positiveZero));
   EXPECT_EQ(1, dm_double_isless(negativeOne, negativeZero));
   EXPECT_EQ(1, dm_double_isless(negativeOne, positiveOne));
   EXPECT_EQ(0, dm_double_isless(negativeOne, negativeOne));
   EXPECT_EQ(1, dm_double_isless(negativeOne, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(negativeOne, negativeTwo));
   EXPECT_EQ(1, dm_double_isless(negativeOne, positiveTen));
   EXPECT_EQ(0, dm_double_isless(negativeOne, negativeTen));
   EXPECT_EQ(1, dm_double_isless(negativeOne, positiveInf));
   EXPECT_EQ(0, dm_double_isless(negativeOne, negativeInf));
   EXPECT_EQ(0, dm_double_isless(negativeOne, nan));

   EXPECT_EQ(0, dm_double_isless(positiveTwo, positiveZero));
   EXPECT_EQ(0, dm_double_isless(positiveTwo, negativeZero));
   EXPECT_EQ(0, dm_double_isless(positiveTwo, positiveOne));
   EXPECT_EQ(0, dm_double_isless(positiveTwo, negativeOne));
   EXPECT_EQ(0, dm_double_isless(positiveTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(positiveTwo, negativeTwo));
   EXPECT_EQ(1, dm_double_isless(positiveTwo, positiveTen));
   EXPECT_EQ(0, dm_double_isless(positiveTwo, negativeTen));
   EXPECT_EQ(1, dm_double_isless(positiveTwo, positiveInf));
   EXPECT_EQ(0, dm_double_isless(positiveTwo, negativeInf));
   EXPECT_EQ(0, dm_double_isless(positiveTwo, nan));

   EXPECT_EQ(1, dm_double_isless(negativeTwo, positiveZero));
   EXPECT_EQ(1, dm_double_isless(negativeTwo, negativeZero));
   EXPECT_EQ(1, dm_double_isless(negativeTwo, positiveOne));
   EXPECT_EQ(1, dm_double_isless(negativeTwo, negativeOne));
   EXPECT_EQ(1, dm_double_isless(negativeTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(negativeTwo, negativeTwo));
   EXPECT_EQ(1, dm_double_isless(negativeTwo, positiveTen));
   EXPECT_EQ(0, dm_double_isless(negativeTwo, negativeTen));
   EXPECT_EQ(1, dm_double_isless(negativeTwo, positiveInf));
   EXPECT_EQ(0, dm_double_isless(negativeTwo, negativeInf));
   EXPECT_EQ(0, dm_double_isless(negativeTwo, nan));

   EXPECT_EQ(0, dm_double_isless(positiveTen, positiveZero));
   EXPECT_EQ(0, dm_double_isless(positiveTen, negativeZero));
   EXPECT_EQ(0, dm_double_isless(positiveTen, positiveOne));
   EXPECT_EQ(0, dm_double_isless(positiveTen, negativeOne));
   EXPECT_EQ(0, dm_double_isless(positiveTen, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(positiveTen, negativeTwo));
   EXPECT_EQ(0, dm_double_isless(positiveTen, positiveTen));
   EXPECT_EQ(0, dm_double_isless(positiveTen, negativeTen));
   EXPECT_EQ(1, dm_double_isless(positiveTen, positiveInf));
   EXPECT_EQ(0, dm_double_isless(positiveTen, negativeInf));
   EXPECT_EQ(0, dm_double_isless(positiveTen, nan));

   EXPECT_EQ(1, dm_double_isless(negativeTen, positiveZero));
   EXPECT_EQ(1, dm_double_isless(negativeTen, negativeZero));
   EXPECT_EQ(1, dm_double_isless(negativeTen, positiveOne));
   EXPECT_EQ(1, dm_double_isless(negativeTen, negativeOne));
   EXPECT_EQ(1, dm_double_isless(negativeTen, positiveTwo));
   EXPECT_EQ(1, dm_double_isless(negativeTen, negativeTwo));
   EXPECT_EQ(1, dm_double_isless(negativeTen, positiveTen));
   EXPECT_EQ(0, dm_double_isless(negativeTen, negativeTen));
   EXPECT_EQ(1, dm_double_isless(negativeTen, positiveInf));
   EXPECT_EQ(0, dm_double_isless(negativeTen, negativeInf));
   EXPECT_EQ(0, dm_double_isless(negativeTen, nan));

   EXPECT_EQ(0, dm_double_isless(positiveInf, positiveZero));
   EXPECT_EQ(0, dm_double_isless(positiveInf, negativeZero));
   EXPECT_EQ(0, dm_double_isless(positiveInf, positiveOne));
   EXPECT_EQ(0, dm_double_isless(positiveInf, negativeOne));
   EXPECT_EQ(0, dm_double_isless(positiveInf, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(positiveInf, negativeTwo));
   EXPECT_EQ(0, dm_double_isless(positiveInf, positiveTen));
   EXPECT_EQ(0, dm_double_isless(positiveInf, negativeTen));
   EXPECT_EQ(0, dm_double_isless(positiveInf, positiveInf));
   EXPECT_EQ(0, dm_double_isless(positiveInf, negativeInf));
   EXPECT_EQ(0, dm_double_isless(positiveInf, nan));

   EXPECT_EQ(1, dm_double_isless(negativeInf, positiveZero));
   EXPECT_EQ(1, dm_double_isless(negativeInf, negativeZero));
   EXPECT_EQ(1, dm_double_isless(negativeInf, positiveOne));
   EXPECT_EQ(1, dm_double_isless(negativeInf, negativeOne));
   EXPECT_EQ(1, dm_double_isless(negativeInf, positiveTwo));
   EXPECT_EQ(1, dm_double_isless(negativeInf, negativeTwo));
   EXPECT_EQ(1, dm_double_isless(negativeInf, positiveTen));
   EXPECT_EQ(1, dm_double_isless(negativeInf, negativeTen));
   EXPECT_EQ(1, dm_double_isless(negativeInf, positiveInf));
   EXPECT_EQ(0, dm_double_isless(negativeInf, negativeInf));
   EXPECT_EQ(0, dm_double_isless(negativeInf, nan));

   EXPECT_EQ(0, dm_double_isless(nan, positiveZero));
   EXPECT_EQ(0, dm_double_isless(nan, negativeZero));
   EXPECT_EQ(0, dm_double_isless(nan, positiveOne));
   EXPECT_EQ(0, dm_double_isless(nan, negativeOne));
   EXPECT_EQ(0, dm_double_isless(nan, positiveTwo));
   EXPECT_EQ(0, dm_double_isless(nan, negativeTwo));
   EXPECT_EQ(0, dm_double_isless(nan, positiveTen));
   EXPECT_EQ(0, dm_double_isless(nan, negativeTen));
   EXPECT_EQ(0, dm_double_isless(nan, positiveInf));
   EXPECT_EQ(0, dm_double_isless(nan, negativeInf));
   EXPECT_EQ(0, dm_double_isless(nan, nan));


   EXPECT_EQ(0, dm_double_isgreater(positiveZero, positiveZero));
   EXPECT_EQ(0, dm_double_isgreater(positiveZero, negativeZero));
   EXPECT_EQ(0, dm_double_isgreater(positiveZero, positiveOne));
   EXPECT_EQ(1, dm_double_isgreater(positiveZero, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(positiveZero, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreater(positiveZero, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(positiveZero, positiveTen));
   EXPECT_EQ(1, dm_double_isgreater(positiveZero, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(positiveZero, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(positiveZero, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(positiveZero, nan));

   EXPECT_EQ(0, dm_double_isgreater(negativeZero, positiveZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeZero, negativeZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeZero, positiveOne));
   EXPECT_EQ(1, dm_double_isgreater(negativeZero, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeZero, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreater(negativeZero, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(negativeZero, positiveTen));
   EXPECT_EQ(1, dm_double_isgreater(negativeZero, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(negativeZero, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(negativeZero, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(negativeZero, nan));

   EXPECT_EQ(1, dm_double_isgreater(positiveOne, positiveZero));
   EXPECT_EQ(1, dm_double_isgreater(positiveOne, negativeZero));
   EXPECT_EQ(0, dm_double_isgreater(positiveOne, positiveOne));
   EXPECT_EQ(1, dm_double_isgreater(positiveOne, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(positiveOne, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreater(positiveOne, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(positiveOne, positiveTen));
   EXPECT_EQ(1, dm_double_isgreater(positiveOne, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(positiveOne, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(positiveOne, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(positiveOne, nan));

   EXPECT_EQ(0, dm_double_isgreater(negativeOne, positiveZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeOne, negativeZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeOne, positiveOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeOne, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeOne, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreater(negativeOne, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(negativeOne, positiveTen));
   EXPECT_EQ(1, dm_double_isgreater(negativeOne, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(negativeOne, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(negativeOne, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(negativeOne, nan));

   EXPECT_EQ(1, dm_double_isgreater(positiveTwo, positiveZero));
   EXPECT_EQ(1, dm_double_isgreater(positiveTwo, negativeZero));
   EXPECT_EQ(1, dm_double_isgreater(positiveTwo, positiveOne));
   EXPECT_EQ(1, dm_double_isgreater(positiveTwo, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(positiveTwo, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreater(positiveTwo, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(positiveTwo, positiveTen));
   EXPECT_EQ(1, dm_double_isgreater(positiveTwo, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(positiveTwo, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(positiveTwo, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(positiveTwo, nan));

   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, positiveZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, negativeZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, positiveOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, positiveTen));
   EXPECT_EQ(1, dm_double_isgreater(negativeTwo, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(negativeTwo, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(negativeTwo, nan));

   EXPECT_EQ(1, dm_double_isgreater(positiveTen, positiveZero));
   EXPECT_EQ(1, dm_double_isgreater(positiveTen, negativeZero));
   EXPECT_EQ(1, dm_double_isgreater(positiveTen, positiveOne));
   EXPECT_EQ(1, dm_double_isgreater(positiveTen, negativeOne));
   EXPECT_EQ(1, dm_double_isgreater(positiveTen, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreater(positiveTen, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(positiveTen, positiveTen));
   EXPECT_EQ(1, dm_double_isgreater(positiveTen, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(positiveTen, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(positiveTen, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(positiveTen, nan));

   EXPECT_EQ(0, dm_double_isgreater(negativeTen, positiveZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, negativeZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, positiveOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, positiveTwo));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, positiveTen));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(negativeTen, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(negativeTen, nan));

   EXPECT_EQ(1, dm_double_isgreater(positiveInf, positiveZero));
   EXPECT_EQ(1, dm_double_isgreater(positiveInf, negativeZero));
   EXPECT_EQ(1, dm_double_isgreater(positiveInf, positiveOne));
   EXPECT_EQ(1, dm_double_isgreater(positiveInf, negativeOne));
   EXPECT_EQ(1, dm_double_isgreater(positiveInf, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreater(positiveInf, negativeTwo));
   EXPECT_EQ(1, dm_double_isgreater(positiveInf, positiveTen));
   EXPECT_EQ(1, dm_double_isgreater(positiveInf, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(positiveInf, positiveInf));
   EXPECT_EQ(1, dm_double_isgreater(positiveInf, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(positiveInf, nan));

   EXPECT_EQ(0, dm_double_isgreater(negativeInf, positiveZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, negativeZero));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, positiveOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, positiveTwo));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, positiveTen));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, positiveInf));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(negativeInf, nan));

   EXPECT_EQ(0, dm_double_isgreater(nan, positiveZero));
   EXPECT_EQ(0, dm_double_isgreater(nan, negativeZero));
   EXPECT_EQ(0, dm_double_isgreater(nan, positiveOne));
   EXPECT_EQ(0, dm_double_isgreater(nan, negativeOne));
   EXPECT_EQ(0, dm_double_isgreater(nan, positiveTwo));
   EXPECT_EQ(0, dm_double_isgreater(nan, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreater(nan, positiveTen));
   EXPECT_EQ(0, dm_double_isgreater(nan, negativeTen));
   EXPECT_EQ(0, dm_double_isgreater(nan, positiveInf));
   EXPECT_EQ(0, dm_double_isgreater(nan, negativeInf));
   EXPECT_EQ(0, dm_double_isgreater(nan, nan));


   EXPECT_EQ(1, dm_double_islessequal(positiveZero, positiveZero));
   EXPECT_EQ(1, dm_double_islessequal(positiveZero, negativeZero));
   EXPECT_EQ(1, dm_double_islessequal(positiveZero, positiveOne));
   EXPECT_EQ(0, dm_double_islessequal(positiveZero, negativeOne));
   EXPECT_EQ(1, dm_double_islessequal(positiveZero, positiveTwo));
   EXPECT_EQ(0, dm_double_islessequal(positiveZero, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(positiveZero, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(positiveZero, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(positiveZero, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveZero, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveZero, nan));

   EXPECT_EQ(1, dm_double_islessequal(negativeZero, positiveZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeZero, negativeZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeZero, positiveOne));
   EXPECT_EQ(0, dm_double_islessequal(negativeZero, negativeOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeZero, positiveTwo));
   EXPECT_EQ(0, dm_double_islessequal(negativeZero, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(negativeZero, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(negativeZero, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(negativeZero, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeZero, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeZero, nan));

   EXPECT_EQ(0, dm_double_islessequal(positiveOne, positiveZero));
   EXPECT_EQ(0, dm_double_islessequal(positiveOne, negativeZero));
   EXPECT_EQ(1, dm_double_islessequal(positiveOne, positiveOne));
   EXPECT_EQ(0, dm_double_islessequal(positiveOne, negativeOne));
   EXPECT_EQ(1, dm_double_islessequal(positiveOne, positiveTwo));
   EXPECT_EQ(0, dm_double_islessequal(positiveOne, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(positiveOne, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(positiveOne, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(positiveOne, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveOne, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveOne, nan));

   EXPECT_EQ(1, dm_double_islessequal(negativeOne, positiveZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeOne, negativeZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeOne, positiveOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeOne, negativeOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeOne, positiveTwo));
   EXPECT_EQ(0, dm_double_islessequal(negativeOne, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(negativeOne, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(negativeOne, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(negativeOne, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeOne, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeOne, nan));

   EXPECT_EQ(0, dm_double_islessequal(positiveTwo, positiveZero));
   EXPECT_EQ(0, dm_double_islessequal(positiveTwo, negativeZero));
   EXPECT_EQ(0, dm_double_islessequal(positiveTwo, positiveOne));
   EXPECT_EQ(0, dm_double_islessequal(positiveTwo, negativeOne));
   EXPECT_EQ(1, dm_double_islessequal(positiveTwo, positiveTwo));
   EXPECT_EQ(0, dm_double_islessequal(positiveTwo, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(positiveTwo, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(positiveTwo, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(positiveTwo, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveTwo, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveTwo, nan));

   EXPECT_EQ(1, dm_double_islessequal(negativeTwo, positiveZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeTwo, negativeZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeTwo, positiveOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeTwo, negativeOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeTwo, positiveTwo));
   EXPECT_EQ(1, dm_double_islessequal(negativeTwo, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(negativeTwo, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(negativeTwo, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(negativeTwo, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeTwo, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeTwo, nan));

   EXPECT_EQ(0, dm_double_islessequal(positiveTen, positiveZero));
   EXPECT_EQ(0, dm_double_islessequal(positiveTen, negativeZero));
   EXPECT_EQ(0, dm_double_islessequal(positiveTen, positiveOne));
   EXPECT_EQ(0, dm_double_islessequal(positiveTen, negativeOne));
   EXPECT_EQ(0, dm_double_islessequal(positiveTen, positiveTwo));
   EXPECT_EQ(0, dm_double_islessequal(positiveTen, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(positiveTen, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(positiveTen, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(positiveTen, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveTen, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveTen, nan));

   EXPECT_EQ(1, dm_double_islessequal(negativeTen, positiveZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeTen, negativeZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeTen, positiveOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeTen, negativeOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeTen, positiveTwo));
   EXPECT_EQ(1, dm_double_islessequal(negativeTen, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(negativeTen, positiveTen));
   EXPECT_EQ(1, dm_double_islessequal(negativeTen, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(negativeTen, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeTen, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeTen, nan));

   EXPECT_EQ(0, dm_double_islessequal(positiveInf, positiveZero));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, negativeZero));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, positiveOne));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, negativeOne));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, positiveTwo));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, negativeTwo));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(positiveInf, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(positiveInf, nan));

   EXPECT_EQ(1, dm_double_islessequal(negativeInf, positiveZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, negativeZero));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, positiveOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, negativeOne));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, positiveTwo));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, negativeTwo));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, positiveTen));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, negativeTen));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, positiveInf));
   EXPECT_EQ(1, dm_double_islessequal(negativeInf, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(negativeInf, nan));

   EXPECT_EQ(0, dm_double_islessequal(nan, positiveZero));
   EXPECT_EQ(0, dm_double_islessequal(nan, negativeZero));
   EXPECT_EQ(0, dm_double_islessequal(nan, positiveOne));
   EXPECT_EQ(0, dm_double_islessequal(nan, negativeOne));
   EXPECT_EQ(0, dm_double_islessequal(nan, positiveTwo));
   EXPECT_EQ(0, dm_double_islessequal(nan, negativeTwo));
   EXPECT_EQ(0, dm_double_islessequal(nan, positiveTen));
   EXPECT_EQ(0, dm_double_islessequal(nan, negativeTen));
   EXPECT_EQ(0, dm_double_islessequal(nan, positiveInf));
   EXPECT_EQ(0, dm_double_islessequal(nan, negativeInf));
   EXPECT_EQ(0, dm_double_islessequal(nan, nan));


   EXPECT_EQ(1, dm_double_isgreaterequal(positiveZero, positiveZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveZero, negativeZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveZero, positiveOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveZero, negativeOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveZero, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveZero, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveZero, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveZero, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveZero, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveZero, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveZero, nan));

   EXPECT_EQ(1, dm_double_isgreaterequal(negativeZero, positiveZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeZero, negativeZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeZero, positiveOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeZero, negativeOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeZero, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeZero, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeZero, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeZero, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeZero, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeZero, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeZero, nan));

   EXPECT_EQ(1, dm_double_isgreaterequal(positiveOne, positiveZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveOne, negativeZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveOne, positiveOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveOne, negativeOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveOne, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveOne, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveOne, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveOne, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveOne, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveOne, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveOne, nan));

   EXPECT_EQ(0, dm_double_isgreaterequal(negativeOne, positiveZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeOne, negativeZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeOne, positiveOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeOne, negativeOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeOne, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeOne, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeOne, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeOne, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeOne, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeOne, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeOne, nan));

   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTwo, positiveZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTwo, negativeZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTwo, positiveOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTwo, negativeOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTwo, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTwo, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveTwo, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTwo, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveTwo, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTwo, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveTwo, nan));

   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTwo, positiveZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTwo, negativeZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTwo, positiveOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTwo, negativeOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTwo, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeTwo, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTwo, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeTwo, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTwo, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeTwo, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTwo, nan));

   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, positiveZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, negativeZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, positiveOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, negativeOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, negativeTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveTen, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveTen, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveTen, nan));

   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, positiveZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, negativeZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, positiveOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, negativeOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, positiveTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeTen, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeTen, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeTen, nan));

   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, positiveZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, negativeZero));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, positiveOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, negativeOne));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, positiveTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, negativeTwo));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, positiveTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, negativeTen));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(positiveInf, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(positiveInf, nan));

   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, positiveZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, negativeZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, positiveOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, negativeOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, positiveTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, positiveTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, positiveInf));
   EXPECT_EQ(1, dm_double_isgreaterequal(negativeInf, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(negativeInf, nan));

   EXPECT_EQ(0, dm_double_isgreaterequal(nan, positiveZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, negativeZero));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, positiveOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, negativeOne));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, positiveTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, negativeTwo));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, positiveTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, negativeTen));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, positiveInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, negativeInf));
   EXPECT_EQ(0, dm_double_isgreaterequal(nan, nan));
 }

TEST(DMDoubleTest, testRounding) // Is this OVERKILL? Maybe.
 {
   // Odd breaks the mold because I flipped the sense between SlowFloat and here.
#ifndef MISRAbleC
   const int T = 1;
   const int F = 0;
#else /* MISRAbleC version */
   const unsigned int T = 1U;
   const unsigned int F = 0U;
#endif /* MISRAbleC */

   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 0, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 0, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 0, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 0, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 1, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 0, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(1, 1, 0, 1, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(1, 1, 0, 0, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 1, DM_FE_TONEAREST));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 0, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(0, 1, 0, 1, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(0, 1, 0, 0, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 1, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 0, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 1, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 0, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 1, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 0, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 1, DM_FE_TONEAREST));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 0, DM_FE_TONEAREST));


   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, 0, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, 0, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, 0, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, 0, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, 0, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, 0, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, 0, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, 0, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 0, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 1, DM_FE_TONEARESTFROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 0, DM_FE_TONEARESTFROMZERO));


   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, DM_FE_UPWARD));
   EXPECT_EQ(T, dm_decideRound(0, 0, 1, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, DM_FE_UPWARD));
   EXPECT_EQ(T, dm_decideRound(0, 1, 1, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 1, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 1, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 1, DM_FE_UPWARD));
   EXPECT_EQ(T, dm_decideRound(0, 0, 0, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 1, DM_FE_UPWARD));
   EXPECT_EQ(T, dm_decideRound(0, 1, 0, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 0, -1, 1, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 0, -1, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, -1, 1, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, -1, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, -1, 1, DM_FE_UPWARD));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 0, DM_FE_UPWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, -1, 1, DM_FE_UPWARD));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 0, DM_FE_UPWARD));


   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(T, dm_decideRound(1, 0, 1, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(T, dm_decideRound(1, 1, 1, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(T, dm_decideRound(1, 0, 0, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(T, dm_decideRound(1, 1, 0, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(1, 0, -1, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(1, 1, -1, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, -1, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 0, -1, 0, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, -1, 1, DM_FE_DOWNWARD));
   EXPECT_EQ(F, dm_decideRound(0, 1, -1, 0, DM_FE_DOWNWARD));


   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, -1, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, -1, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, -1, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, -1, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, -1, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, -1, 0, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, -1, 1, DM_FE_TOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, -1, 0, DM_FE_TOWARDZERO));


   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(1, 0, 0, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(1, 0, 0, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(0, 0, 0, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(0, 0, 0, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 0, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 1, DM_FE_TONEARESTODD));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 0, DM_FE_TONEARESTODD));


   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 0, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 1, DM_FE_TONEARESTTOWARDZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 0, DM_FE_TONEARESTTOWARDZERO));


   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, 1, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, 1, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, 1, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, 1, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, 0, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, 0, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, 0, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, 0, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(1, 0, -1, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 0, -1, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(1, 1, -1, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(1, 1, -1, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 0, -1, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 0, -1, 0, DM_FE_FROMZERO));
   EXPECT_EQ(F, dm_decideRound(0, 1, -1, 1, DM_FE_FROMZERO));
   EXPECT_EQ(T, dm_decideRound(0, 1, -1, 0, DM_FE_FROMZERO));


      // Invalid mode should behave like DM_FE_TOWARDZERO
   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 1, 42));
   EXPECT_EQ(F, dm_decideRound(1, 0, 1, 0, 42));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 1, 42));
   EXPECT_EQ(F, dm_decideRound(1, 1, 1, 0, 42));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 1, 42));
   EXPECT_EQ(F, dm_decideRound(0, 0, 1, 0, 42));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 1, 42));
   EXPECT_EQ(F, dm_decideRound(0, 1, 1, 0, 42));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 1, 42));
   EXPECT_EQ(F, dm_decideRound(1, 0, 0, 0, 42));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 1, 42));
   EXPECT_EQ(F, dm_decideRound(1, 1, 0, 0, 42));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 1, 42));
   EXPECT_EQ(F, dm_decideRound(0, 0, 0, 0, 42));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 1, 42));
   EXPECT_EQ(F, dm_decideRound(0, 1, 0, 0, 42));
   EXPECT_EQ(F, dm_decideRound(1, 0, -1, 1, 42));
   EXPECT_EQ(F, dm_decideRound(1, 0, -1, 0, 42));
   EXPECT_EQ(F, dm_decideRound(1, 1, -1, 1, 42));
   EXPECT_EQ(F, dm_decideRound(1, 1, -1, 0, 42));
   EXPECT_EQ(F, dm_decideRound(0, 0, -1, 1, 42));
   EXPECT_EQ(F, dm_decideRound(0, 0, -1, 0, 42));
   EXPECT_EQ(F, dm_decideRound(0, 1, -1, 1, 42));
   EXPECT_EQ(F, dm_decideRound(0, 1, -1, 0, 42));
 }

TEST(DMDoubleTest, testClassifiers)
 {
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U); // Special exponent, zero significand : zero
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);
   dm_double positiveInf  = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY); // Special exponent, ones significand : infinity
   dm_double negativeInf  = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double nan          = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U); // Special exponent, any other significand : NaN

   dm_double trap         = DM_DOUBLE_PACK_ALT(0, 0, DM_INFINITY - 2);
   dm_double negativenan  = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 1U);

   EXPECT_EQ(0, dm_double_iscanonical(trap)); // This is the only time we test this. All other functions will ignore this case.
   EXPECT_EQ(1, dm_double_iscanonical(positiveZero));
   EXPECT_EQ(1, dm_double_iscanonical(negativeZero));
   EXPECT_EQ(1, dm_double_iscanonical(positiveOne));
   EXPECT_EQ(1, dm_double_iscanonical(negativeOne));
   EXPECT_EQ(1, dm_double_iscanonical(positiveInf));
   EXPECT_EQ(1, dm_double_iscanonical(negativeInf));
   EXPECT_EQ(1, dm_double_iscanonical(nan));

   EXPECT_EQ(1, dm_double_isfinite(positiveZero));
   EXPECT_EQ(1, dm_double_isfinite(negativeZero));
   EXPECT_EQ(1, dm_double_isfinite(positiveOne));
   EXPECT_EQ(1, dm_double_isfinite(negativeOne));
   EXPECT_EQ(0, dm_double_isfinite(positiveInf));
   EXPECT_EQ(0, dm_double_isfinite(negativeInf));
   EXPECT_EQ(0, dm_double_isfinite(nan));

   EXPECT_EQ(0, dm_double_isinf(positiveZero));
   EXPECT_EQ(0, dm_double_isinf(negativeZero));
   EXPECT_EQ(0, dm_double_isinf(positiveOne));
   EXPECT_EQ(0, dm_double_isinf(negativeOne));
   EXPECT_EQ(1, dm_double_isinf(positiveInf));
   EXPECT_EQ(1, dm_double_isinf(negativeInf));
   EXPECT_EQ(0, dm_double_isinf(nan));

   EXPECT_EQ(0, dm_double_isnan(positiveZero));
   EXPECT_EQ(0, dm_double_isnan(negativeZero));
   EXPECT_EQ(0, dm_double_isnan(positiveOne));
   EXPECT_EQ(0, dm_double_isnan(negativeOne));
   EXPECT_EQ(0, dm_double_isnan(positiveInf));
   EXPECT_EQ(0, dm_double_isnan(negativeInf));
   EXPECT_EQ(1, dm_double_isnan(nan));

   EXPECT_EQ(0, dm_double_isnormal(positiveZero));
   EXPECT_EQ(0, dm_double_isnormal(negativeZero));
   EXPECT_EQ(1, dm_double_isnormal(positiveOne));
   EXPECT_EQ(1, dm_double_isnormal(negativeOne));
   EXPECT_EQ(0, dm_double_isnormal(positiveInf));
   EXPECT_EQ(0, dm_double_isnormal(negativeInf));
   EXPECT_EQ(0, dm_double_isnormal(nan));

   EXPECT_EQ(0, dm_double_signbit(positiveZero));
   EXPECT_EQ(1, dm_double_signbit(negativeZero));
   EXPECT_EQ(0, dm_double_signbit(positiveOne));
   EXPECT_EQ(1, dm_double_signbit(negativeOne));
   EXPECT_EQ(0, dm_double_signbit(positiveInf));
   EXPECT_EQ(1, dm_double_signbit(negativeInf));
   EXPECT_EQ(0, dm_double_signbit(nan));
   EXPECT_EQ(1, dm_double_signbit(negativenan)); // This is the only time we test this. All other functions will ignore this case.

   EXPECT_EQ(0, dm_double_issubnormal(positiveZero));
   EXPECT_EQ(0, dm_double_issubnormal(negativeZero));
   EXPECT_EQ(0, dm_double_issubnormal(positiveOne));
   EXPECT_EQ(0, dm_double_issubnormal(negativeOne));
   EXPECT_EQ(0, dm_double_issubnormal(positiveInf));
   EXPECT_EQ(0, dm_double_issubnormal(negativeInf));
   EXPECT_EQ(0, dm_double_issubnormal(nan));

   EXPECT_EQ(0, dm_double_issignaling(positiveZero));
   EXPECT_EQ(0, dm_double_issignaling(negativeZero));
   EXPECT_EQ(0, dm_double_issignaling(positiveOne));
   EXPECT_EQ(0, dm_double_issignaling(negativeOne));
   EXPECT_EQ(0, dm_double_issignaling(positiveInf));
   EXPECT_EQ(0, dm_double_issignaling(negativeInf));
   EXPECT_EQ(0, dm_double_issignaling(nan));

   EXPECT_EQ(1, dm_double_iszero(positiveZero));
   EXPECT_EQ(1, dm_double_iszero(negativeZero));
   EXPECT_EQ(0, dm_double_iszero(positiveOne));
   EXPECT_EQ(0, dm_double_iszero(negativeOne));
   EXPECT_EQ(0, dm_double_iszero(positiveInf));
   EXPECT_EQ(0, dm_double_iszero(negativeInf));
   EXPECT_EQ(0, dm_double_iszero(nan));

   EXPECT_EQ(FP_ZERO, dm_double_fpclassify(positiveZero));
   EXPECT_EQ(FP_ZERO, dm_double_fpclassify(negativeZero));
   EXPECT_EQ(FP_NORMAL, dm_double_fpclassify(positiveOne));
   EXPECT_EQ(FP_NORMAL, dm_double_fpclassify(negativeOne));
   EXPECT_EQ(FP_INFINITE, dm_double_fpclassify(positiveInf));
   EXPECT_EQ(FP_INFINITE, dm_double_fpclassify(negativeInf));
   EXPECT_EQ(FP_NAN, dm_double_fpclassify(nan));
   EXPECT_EQ(FP_NAN, dm_double_fpclassify(negativenan));
 }

TEST(DMDoubleTest, testVarious)
 {
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);
   dm_double positiveTwo  = DM_DOUBLE_PACK(0, 0, 2000000000000000ULL);
   dm_double negativeTwo  = DM_DOUBLE_PACK(1, 0, 2000000000000000ULL);
   dm_double nan          = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U); // Special exponent, any other significand : NaN
   dm_double positiveInf  = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double negativeInf  = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U);
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);

   EXPECT_EQ(negativeOne, dm_double_neg(positiveOne));
   EXPECT_EQ(positiveOne, dm_double_fabs(negativeOne));

   EXPECT_EQ(positiveOne, dm_double_fmin(positiveOne, nan));
   EXPECT_EQ(positiveOne, dm_double_fmin(nan, positiveOne));
   EXPECT_EQ(positiveOne, dm_double_fmin(positiveTwo, positiveOne));
   EXPECT_EQ(positiveOne, dm_double_fmin(positiveOne, positiveTwo));

   EXPECT_EQ(positiveOne, dm_double_fmax(positiveOne, nan));
   EXPECT_EQ(positiveOne, dm_double_fmax(nan, positiveOne));
   EXPECT_EQ(positiveTwo, dm_double_fmax(positiveTwo, positiveOne));
   EXPECT_EQ(positiveTwo, dm_double_fmax(positiveOne, positiveTwo));

   EXPECT_EQ(positiveTwo, dm_double_copysign(positiveTwo, positiveOne));
   EXPECT_EQ(positiveTwo, dm_double_copysign(negativeTwo, positiveOne));
   EXPECT_EQ(negativeTwo, dm_double_copysign(positiveTwo, negativeOne));
   EXPECT_EQ(negativeTwo, dm_double_copysign(negativeTwo, negativeOne));

   EXPECT_EQ(DM_FE_TONEAREST, dm_fegetround());
   EXPECT_EQ(DM_FE_TONEAREST, dm_fesetround(-1));
   EXPECT_EQ(DM_FE_TONEAREST, dm_fesetround(42));
   EXPECT_EQ(DM_FE_TONEARESTODD, dm_fesetround(DM_FE_TONEARESTODD));
   EXPECT_EQ(DM_FE_TONEAREST, dm_fesetround(DM_FE_TONEAREST));

   int exp;
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dm_double_frexp(DM_DOUBLE_PACK(0, 12, 1234567890123456ULL), &exp));
   EXPECT_EQ(12, exp);
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 1234567890123456ULL), dm_double_frexp(DM_DOUBLE_PACK(1, 12, 1234567890123456ULL), &exp));
   EXPECT_EQ(12, exp);

   EXPECT_EQ(positiveInf, dm_double_ldexp(DM_DOUBLE_PACK(0, 12, 1234567890123456ULL), 1200));
   EXPECT_EQ(negativeInf, dm_double_ldexp(DM_DOUBLE_PACK(1, 12, 1234567890123456ULL), 1200));
   EXPECT_EQ(positiveInf, dm_double_ldexp(DM_DOUBLE_PACK(0, 12, 1234567890123456ULL), 500));
   EXPECT_EQ(negativeInf, dm_double_ldexp(DM_DOUBLE_PACK(1, 12, 1234567890123456ULL), 500));

   EXPECT_EQ(positiveZero, dm_double_ldexp(DM_DOUBLE_PACK(0, 12, 1234567890123456ULL), -1200));
   EXPECT_EQ(negativeZero, dm_double_ldexp(DM_DOUBLE_PACK(1, 12, 1234567890123456ULL), -1200));
   EXPECT_EQ(positiveZero, dm_double_ldexp(DM_DOUBLE_PACK(0, -12, 1234567890123456ULL), -500));
   EXPECT_EQ(negativeZero, dm_double_ldexp(DM_DOUBLE_PACK(1, -12, 1234567890123456ULL), -500));

   EXPECT_EQ(DM_DOUBLE_PACK(0, 500, 1234567890123456ULL), dm_double_ldexp(DM_DOUBLE_PACK(0, -12, 1234567890123456ULL), 512));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 500, 1234567890123456ULL), dm_double_ldexp(DM_DOUBLE_PACK(1, -12, 1234567890123456ULL), 512));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -500, 1234567890123456ULL), dm_double_ldexp(DM_DOUBLE_PACK(0, 12, 1234567890123456ULL), -512));
   EXPECT_EQ(DM_DOUBLE_PACK(1, -500, 1234567890123456ULL), dm_double_ldexp(DM_DOUBLE_PACK(1, 12, 1234567890123456ULL), -512));

   dm_double temp;
   EXPECT_EQ(positiveZero, dm_double_modf(positiveInf, &temp));
   EXPECT_EQ(positiveInf, temp);
   EXPECT_EQ(negativeZero, dm_double_modf(negativeInf, &temp));
   EXPECT_EQ(negativeInf, temp);
   EXPECT_EQ(positiveZero, dm_double_modf(positiveZero, &temp));
   EXPECT_EQ(positiveZero, temp);
   EXPECT_EQ(negativeZero, dm_double_modf(negativeZero, &temp));
   EXPECT_EQ(negativeZero, temp);

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL), dm_double_modf(DM_DOUBLE_PACK(0, 0, 1500000000000000ULL), &temp));
   EXPECT_EQ(positiveOne, temp);
   EXPECT_EQ(DM_DOUBLE_PACK(1, -1, 5000000000000000ULL), dm_double_modf(DM_DOUBLE_PACK(1, 0, 1500000000000000ULL), &temp));
   EXPECT_EQ(negativeOne, temp);
 }

TEST(DMDoubleTest, testRounds)
 {
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U);
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);
   dm_double positiveTwo  = DM_DOUBLE_PACK(0, 0, 2000000000000000ULL);
   dm_double negativeTwo  = DM_DOUBLE_PACK(1, 0, 2000000000000000ULL);
   dm_double positiveNine = DM_DOUBLE_PACK(0, 0, 9000000000000000ULL);
   dm_double negativeNine = DM_DOUBLE_PACK(1, 0, 9000000000000000ULL);
   dm_double positiveTen  = DM_DOUBLE_PACK(0, 1, 1000000000000000ULL);
   dm_double negativeTen  = DM_DOUBLE_PACK(1, 1, 1000000000000000ULL);


   EXPECT_EQ(positiveZero, dm_double_ceil(positiveZero));
   EXPECT_EQ(positiveOne, dm_double_ceil(positiveOne));
   EXPECT_EQ(positiveOne, dm_double_ceil(DM_DOUBLE_PACK(0, -1, 1000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_ceil(DM_DOUBLE_PACK(1, -1, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_ceil(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL), dm_double_ceil(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_ceil(DM_DOUBLE_PACK(1, 0, 1100000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_ceil(DM_DOUBLE_PACK(1, 0, 1500000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_ceil(DM_DOUBLE_PACK(1, 0, 1700000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_ceil(DM_DOUBLE_PACK(1, 0, 9100000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_ceil(DM_DOUBLE_PACK(1, 0, 9500000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_ceil(DM_DOUBLE_PACK(1, 0, 9700000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_ceil(DM_DOUBLE_PACK(0, 0, 1100000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_ceil(DM_DOUBLE_PACK(0, 0, 1500000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_ceil(DM_DOUBLE_PACK(0, 0, 1700000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_ceil(DM_DOUBLE_PACK(0, 0, 9100000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_ceil(DM_DOUBLE_PACK(0, 0, 9500000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_ceil(DM_DOUBLE_PACK(0, 0, 9700000000000000ULL)));


   EXPECT_EQ(positiveZero, dm_double_floor(positiveZero));
   EXPECT_EQ(negativeOne, dm_double_floor(negativeOne));
   EXPECT_EQ(positiveZero, dm_double_floor(DM_DOUBLE_PACK(0, -1, 1000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_floor(DM_DOUBLE_PACK(1, -1, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_floor(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL), dm_double_floor(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_floor(DM_DOUBLE_PACK(1, 0, 1100000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_floor(DM_DOUBLE_PACK(1, 0, 1500000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_floor(DM_DOUBLE_PACK(1, 0, 1700000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_floor(DM_DOUBLE_PACK(1, 0, 9100000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_floor(DM_DOUBLE_PACK(1, 0, 9500000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_floor(DM_DOUBLE_PACK(1, 0, 9700000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_floor(DM_DOUBLE_PACK(0, 0, 1100000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_floor(DM_DOUBLE_PACK(0, 0, 1500000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_floor(DM_DOUBLE_PACK(0, 0, 1700000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_floor(DM_DOUBLE_PACK(0, 0, 9100000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_floor(DM_DOUBLE_PACK(0, 0, 9500000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_floor(DM_DOUBLE_PACK(0, 0, 9700000000000000ULL)));


   EXPECT_EQ(positiveZero, dm_double_trunc(positiveZero));
   EXPECT_EQ(positiveOne, dm_double_trunc(positiveOne));
   EXPECT_EQ(positiveZero, dm_double_trunc(DM_DOUBLE_PACK(0, -1, 1000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_trunc(DM_DOUBLE_PACK(1, -1, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_trunc(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL), dm_double_trunc(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_trunc(DM_DOUBLE_PACK(1, 0, 1100000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_trunc(DM_DOUBLE_PACK(1, 0, 1500000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_trunc(DM_DOUBLE_PACK(1, 0, 1700000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_trunc(DM_DOUBLE_PACK(1, 0, 9100000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_trunc(DM_DOUBLE_PACK(1, 0, 9500000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_trunc(DM_DOUBLE_PACK(1, 0, 9700000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_trunc(DM_DOUBLE_PACK(0, 0, 1100000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_trunc(DM_DOUBLE_PACK(0, 0, 1500000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_trunc(DM_DOUBLE_PACK(0, 0, 1700000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_trunc(DM_DOUBLE_PACK(0, 0, 9100000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_trunc(DM_DOUBLE_PACK(0, 0, 9500000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_trunc(DM_DOUBLE_PACK(0, 0, 9700000000000000ULL)));


   EXPECT_EQ(positiveZero, dm_double_round(positiveZero));
   EXPECT_EQ(positiveOne, dm_double_round(positiveOne));
   EXPECT_EQ(positiveZero, dm_double_round(DM_DOUBLE_PACK(0, -2, 1000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_round(DM_DOUBLE_PACK(1, -2, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_round(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL), dm_double_round(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_round(DM_DOUBLE_PACK(1, 0, 1100000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_round(DM_DOUBLE_PACK(1, 0, 1500000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_round(DM_DOUBLE_PACK(1, 0, 1700000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_round(DM_DOUBLE_PACK(1, 0, 9100000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_round(DM_DOUBLE_PACK(1, 0, 9500000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_round(DM_DOUBLE_PACK(1, 0, 9700000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_round(DM_DOUBLE_PACK(0, 0, 1100000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_round(DM_DOUBLE_PACK(0, 0, 1500000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_round(DM_DOUBLE_PACK(0, 0, 1700000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_round(DM_DOUBLE_PACK(0, 0, 9100000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_round(DM_DOUBLE_PACK(0, 0, 9500000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_round(DM_DOUBLE_PACK(0, 0, 9700000000000000ULL)));

   EXPECT_EQ(negativeZero, dm_double_round(DM_DOUBLE_PACK(1, -1, 2000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_round(DM_DOUBLE_PACK(1, -1, 5000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_round(DM_DOUBLE_PACK(1, -1, 7000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_round(DM_DOUBLE_PACK(0, -1, 2000000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_round(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_round(DM_DOUBLE_PACK(0, -1, 7000000000000000ULL)));

      // BUG CASE
   EXPECT_EQ(DM_DOUBLE_PACK(0, 15, 1000000000000000ULL), dm_double_round(DM_DOUBLE_PACK(0, 15, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 15, 1000000000000001ULL), dm_double_roundeven(DM_DOUBLE_PACK(0, 15, 1000000000000001ULL)));

   EXPECT_EQ(positiveZero, dm_double_roundeven(positiveZero));
   EXPECT_EQ(positiveOne, dm_double_roundeven(positiveOne));
   EXPECT_EQ(positiveZero, dm_double_roundeven(DM_DOUBLE_PACK(0, -2, 1000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_roundeven(DM_DOUBLE_PACK(1, -2, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_roundeven(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL), dm_double_roundeven(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_roundeven(DM_DOUBLE_PACK(1, 0, 1100000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_roundeven(DM_DOUBLE_PACK(1, 0, 1500000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_roundeven(DM_DOUBLE_PACK(1, 0, 1700000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_roundeven(DM_DOUBLE_PACK(1, 0, 9100000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_roundeven(DM_DOUBLE_PACK(1, 0, 9500000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_roundeven(DM_DOUBLE_PACK(1, 0, 9700000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_roundeven(DM_DOUBLE_PACK(0, 0, 1100000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_roundeven(DM_DOUBLE_PACK(0, 0, 1500000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_roundeven(DM_DOUBLE_PACK(0, 0, 1700000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_roundeven(DM_DOUBLE_PACK(0, 0, 9100000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_roundeven(DM_DOUBLE_PACK(0, 0, 9500000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_roundeven(DM_DOUBLE_PACK(0, 0, 9700000000000000ULL)));

   EXPECT_EQ(negativeZero, dm_double_roundeven(DM_DOUBLE_PACK(1, -1, 2000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_roundeven(DM_DOUBLE_PACK(1, -1, 5000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_roundeven(DM_DOUBLE_PACK(1, -1, 7000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_roundeven(DM_DOUBLE_PACK(0, -1, 2000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_roundeven(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_roundeven(DM_DOUBLE_PACK(0, -1, 7000000000000000ULL)));

   EXPECT_EQ(positiveTwo, dm_double_roundeven(DM_DOUBLE_PACK(0, 0, 2500000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_roundeven(DM_DOUBLE_PACK(1, 0, 2500000000000000ULL)));


   dm_fesetround(DM_FE_TONEAREST);

   EXPECT_EQ(positiveZero, dm_double_nearbyint(positiveZero));
   EXPECT_EQ(positiveOne, dm_double_nearbyint(positiveOne));
   EXPECT_EQ(positiveZero, dm_double_nearbyint(DM_DOUBLE_PACK(0, -2, 1000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_nearbyint(DM_DOUBLE_PACK(1, -2, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_nearbyint(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL), dm_double_nearbyint(DM_DOUBLE_PACK(1, 16, 1000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_nearbyint(DM_DOUBLE_PACK(1, 0, 1100000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_nearbyint(DM_DOUBLE_PACK(1, 0, 1500000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_nearbyint(DM_DOUBLE_PACK(1, 0, 1700000000000000ULL)));
   EXPECT_EQ(negativeNine, dm_double_nearbyint(DM_DOUBLE_PACK(1, 0, 9100000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_nearbyint(DM_DOUBLE_PACK(1, 0, 9500000000000000ULL)));
   EXPECT_EQ(negativeTen, dm_double_nearbyint(DM_DOUBLE_PACK(1, 0, 9700000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_nearbyint(DM_DOUBLE_PACK(0, 0, 1100000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_nearbyint(DM_DOUBLE_PACK(0, 0, 1500000000000000ULL)));
   EXPECT_EQ(positiveTwo, dm_double_nearbyint(DM_DOUBLE_PACK(0, 0, 1700000000000000ULL)));
   EXPECT_EQ(positiveNine, dm_double_nearbyint(DM_DOUBLE_PACK(0, 0, 9100000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_nearbyint(DM_DOUBLE_PACK(0, 0, 9500000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_nearbyint(DM_DOUBLE_PACK(0, 0, 9700000000000000ULL)));

   EXPECT_EQ(negativeZero, dm_double_nearbyint(DM_DOUBLE_PACK(1, -1, 2000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_nearbyint(DM_DOUBLE_PACK(1, -1, 5000000000000000ULL)));
   EXPECT_EQ(negativeOne, dm_double_nearbyint(DM_DOUBLE_PACK(1, -1, 7000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_nearbyint(DM_DOUBLE_PACK(0, -1, 2000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_nearbyint(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_nearbyint(DM_DOUBLE_PACK(0, -1, 7000000000000000ULL)));

   EXPECT_EQ(positiveTwo, dm_double_nearbyint(DM_DOUBLE_PACK(0, 0, 2500000000000000ULL)));
   EXPECT_EQ(negativeTwo, dm_double_nearbyint(DM_DOUBLE_PACK(1, 0, 2500000000000000ULL)));
 }

TEST(DMDoubleTest, testAdds)
 {
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U);
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);
   dm_double positiveInf  = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double negativeInf  = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double nan1         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U);
   dm_double nan2         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 2U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);
   dm_double positiveTwo  = DM_DOUBLE_PACK(0, 0, 2000000000000000ULL);
   dm_double negativeTwo  = DM_DOUBLE_PACK(1, 0, 2000000000000000ULL);
   dm_double positiveTen  = DM_DOUBLE_PACK(0, 1, 1000000000000000ULL);
   dm_double negativeTen  = DM_DOUBLE_PACK(1, 1, 1000000000000000ULL);

   EXPECT_EQ(nan1, dm_double_add(nan1, positiveOne));
   EXPECT_EQ(nan1, dm_double_add(positiveOne, nan1));
   EXPECT_EQ(nan2, dm_double_add(nan2, nan1));

   EXPECT_EQ(positiveInf, dm_double_add(positiveInf, positiveInf));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 255), dm_double_add(positiveInf, negativeInf));
   EXPECT_EQ(positiveInf, dm_double_add(positiveInf, positiveOne));
   EXPECT_EQ(positiveInf, dm_double_add(positiveOne, positiveInf));

   EXPECT_EQ(positiveZero, dm_double_add(positiveZero, positiveZero));
   EXPECT_EQ(positiveZero, dm_double_add(positiveZero, negativeZero));
   EXPECT_EQ(positiveZero, dm_double_add(negativeZero, positiveZero));
   EXPECT_EQ(negativeZero, dm_double_add_r(positiveZero, negativeZero, DM_FE_DOWNWARD));
   EXPECT_EQ(positiveOne, dm_double_add(positiveZero, positiveOne));
   EXPECT_EQ(positiveOne, dm_double_add(positiveOne, positiveZero));
   EXPECT_EQ(negativeZero, dm_double_add(negativeZero, negativeZero));

   dm_fesetround(DM_FE_TONEAREST);

   EXPECT_EQ(positiveTwo, dm_double_add(positiveOne, positiveOne)); // Basic cases
   EXPECT_EQ(negativeTwo, dm_double_add(negativeOne, negativeOne));
   EXPECT_EQ(negativeOne, dm_double_add(positiveOne, negativeTwo));
   EXPECT_EQ(positiveOne, dm_double_add(negativeOne, positiveTwo));
   EXPECT_EQ(positiveZero, dm_double_add(positiveOne, negativeOne)); // Cancel to zero
   EXPECT_EQ(negativeZero, dm_double_add_r(positiveOne, negativeOne, DM_FE_DOWNWARD)); // Cancel to zero w/ rounding
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1100000000000000ULL), dm_double_add(positiveOne, positiveTen)); // Handles offset of exponents correctly
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1100000000000000ULL), dm_double_add(positiveTen, positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 1, 1100000000000000ULL), dm_double_add(negativeOne, negativeTen));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 1, 1100000000000000ULL), dm_double_add(negativeTen, negativeOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 9000000000000000ULL), dm_double_add(negativeOne, positiveTen)); // Result needs exponent - 1 : result in bounds path
      // Result needs exponent - 1 : result > MAX_SIGNIFICAND path
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 9900000000000000ULL), dm_double_add(negativeOne, DM_DOUBLE_PACK(0, 2, 1000000000000000ULL))); // 100 - 1
      // Result needs exponent + 1
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1800000000000000ULL), dm_double_add(DM_DOUBLE_PACK(0, 0, 9000000000000000ULL), DM_DOUBLE_PACK(0, 0, 9000000000000000ULL))); // 9 + 9
   EXPECT_EQ(DM_DOUBLE_PACK(0, 2, 1000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(0, 1, 9900000000000000ULL), positiveOne)); // 99 + 1
   EXPECT_EQ(DM_DOUBLE_PACK(0, 2, 1000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(0, 1, 9990000000000000ULL), DM_DOUBLE_PACK(0, -1, 1000000000000000ULL))); // 99.9 + .1
      // Catastrophic cancellation
   EXPECT_EQ(positiveOne, dm_double_add(DM_DOUBLE_PACK(0, 1, 2000000000000000ULL), DM_DOUBLE_PACK(1, 1, 1900000000000000ULL))); // 20 - 19
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 1000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(0, 1, 2000000000000000ULL), DM_DOUBLE_PACK(1, 1, 1990000000000000ULL))); // 20.0 - 19.9
      // Result flushes to zero
   EXPECT_EQ(positiveZero, dm_double_add(DM_DOUBLE_PACK(0, -511, 2000000000000000ULL), DM_DOUBLE_PACK(1, -511, 1900000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_add(DM_DOUBLE_PACK(1, -511, 2000000000000000ULL), DM_DOUBLE_PACK(0, -511, 1900000000000000ULL)));
      // Result flushes to infinity
   EXPECT_EQ(positiveInf, dm_double_add(DM_DOUBLE_PACK(0, 511, 9000000000000000ULL), DM_DOUBLE_PACK(0, 511, 9000000000000000ULL)));
   EXPECT_EQ(negativeInf, dm_double_add(DM_DOUBLE_PACK(1, 511, 9000000000000000ULL), DM_DOUBLE_PACK(1, 511, 9000000000000000ULL)));
      // Rebound with rounding : pre-check
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999990ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1, -15, 1000000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(0, -16, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1, -16, 1000000000000000ULL)));
      // Rebound with rounding
   EXPECT_EQ(positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1, -17, 1000000000000000ULL)));
   EXPECT_EQ(positiveOne, dm_double_add(DM_DOUBLE_PACK(1, -17, 1000000000000000ULL), positiveOne));
      // No rebound
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add_r(positiveOne, DM_DOUBLE_PACK(1, -17, 1000000000000000ULL), DM_FE_TOWARDZERO));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add_r(DM_DOUBLE_PACK(1, -17, 1000000000000000ULL), positiveOne, DM_FE_TOWARDZERO));
      // Final Case : Add w/ round w/ no overflow
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000000001ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(0, -16, 9000000000000000ULL)));
      // Pathological case
   EXPECT_EQ(DM_DOUBLE_PACK(0, 17, 1000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(0, 16, 9999999999999999ULL), DM_DOUBLE_PACK(0, 0, 9000000000000000ULL)));
      // Actually, testing the 128 bit math.
   EXPECT_EQ(DM_DOUBLE_PACK(0,  2, 1010000000000000ULL), dm_double_add(DM_DOUBLE_PACK(0,  2, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  3, 1001000000000000ULL), dm_double_add(DM_DOUBLE_PACK(0,  3, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  4, 1000100000000000ULL), dm_double_add(DM_DOUBLE_PACK(0,  4, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  5, 1000010000000000ULL), dm_double_add(DM_DOUBLE_PACK(0,  5, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  6, 1000001000000000ULL), dm_double_add(DM_DOUBLE_PACK(0,  6, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  7, 1000000100000000ULL), dm_double_add(DM_DOUBLE_PACK(0,  7, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  8, 1000000010000000ULL), dm_double_add(DM_DOUBLE_PACK(0,  8, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  9, 1000000001000000ULL), dm_double_add(DM_DOUBLE_PACK(0,  9, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 10, 1000000000100000ULL), dm_double_add(DM_DOUBLE_PACK(0, 10, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 11, 1000000000010000ULL), dm_double_add(DM_DOUBLE_PACK(0, 11, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 12, 1000000000001000ULL), dm_double_add(DM_DOUBLE_PACK(0, 12, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 13, 1000000000000100ULL), dm_double_add(DM_DOUBLE_PACK(0, 13, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 14, 1000000000000010ULL), dm_double_add(DM_DOUBLE_PACK(0, 14, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 15, 1000000000000001ULL), dm_double_add(DM_DOUBLE_PACK(0, 15, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), positiveOne));


      // Pathological Tests for correct rounding
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9900000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9990000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999900000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999990000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999900000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999990000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999900000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999990000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999900ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999990ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -17, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 1000000000000000ULL), positiveOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000001ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -17, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 4999999999999999ULL), positiveOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -17, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 5000000000000000ULL), positiveOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 4999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,  -17, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 5000000000000001ULL), positiveOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 1000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9100000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9910000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9991000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999100000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999910000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999991000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999100000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999910000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999991000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999100000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999910000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999991000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999100ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999910ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999991ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,  -17, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 9000000000000000ULL), positiveOne));

   dm_fesetround(DM_FE_TONEARESTODD);

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9900000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9990000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999900000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999990000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999900000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999990000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999900000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999990000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999900ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999990ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -17, 1000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 1000000000000000ULL), positiveOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000001ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -17, 4999999999999999ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 4999999999999999ULL), positiveOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,  -17, 5000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 5000000000000000ULL), positiveOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 4999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,  -17, 5000000000000001ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 5000000000000001ULL), positiveOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 1000000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -1, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9100000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -2, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9910000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -3, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9991000000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -4, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999100000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -5, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999910000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -6, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999991000000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -7, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999100000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -8, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999910000000ULL), dm_double_add(DM_DOUBLE_PACK(1,   -9, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999991000000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -10, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999100000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -11, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999910000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -12, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999991000ULL), dm_double_add(DM_DOUBLE_PACK(1,  -13, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999100ULL), dm_double_add(DM_DOUBLE_PACK(1,  -14, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999910ULL), dm_double_add(DM_DOUBLE_PACK(1,  -15, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999991ULL), dm_double_add(DM_DOUBLE_PACK(1,  -16, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(DM_DOUBLE_PACK(1,  -17, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(                               positiveOne, dm_double_add(DM_DOUBLE_PACK(1,  -18, 9000000000000000ULL), positiveOne));

   dm_fesetround(DM_FE_TONEAREST);


   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9000000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -1, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9900000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -2, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9990000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -3, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -4, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999900000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -5, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999990000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -6, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -7, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999900000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -8, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999990000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -9, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -10, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999900000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -11, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999990000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -12, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -13, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999900ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -14, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999990ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -15, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -16, 1000000000000000ULL)));
   EXPECT_EQ(                               positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -17, 1000000000000000ULL)));
   EXPECT_EQ(                               positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -18, 1000000000000000ULL)));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000001ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -1, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -2, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -3, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -4, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -5, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -6, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -7, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -8, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -9, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -10, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -11, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -12, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -13, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -14, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -15, 4999999999999999ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -16, 4999999999999999ULL)));
   EXPECT_EQ(                               positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -17, 4999999999999999ULL)));
   EXPECT_EQ(                               positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -18, 4999999999999999ULL)));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -1, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -2, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -3, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -4, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -5, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -6, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -7, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -8, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -9, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -10, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -11, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -12, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -13, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -14, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -15, 5000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -16, 5000000000000000ULL)));
   EXPECT_EQ(                               positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -17, 5000000000000000ULL)));
   EXPECT_EQ(                               positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -18, 5000000000000000ULL)));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 4999999999999999ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -1, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9500000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -2, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9950000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -3, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9995000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -4, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999500000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -5, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999950000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -6, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999995000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -7, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999500000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -8, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999950000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -9, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999995000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -10, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999500000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -11, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999950000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -12, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999995000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -13, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999500ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -14, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999950ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -15, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999995ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -16, 5000000000000001ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -17, 5000000000000001ULL)));
   EXPECT_EQ(                               positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -18, 5000000000000001ULL)));

   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 1000000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -1, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9100000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -2, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9910000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -3, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9991000000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -4, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999100000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -5, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999910000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -6, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999991000000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -7, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999100000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -8, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999910000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,   -9, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999991000000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -10, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999100000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -11, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999910000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -12, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999991000ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -13, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999100ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -14, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999910ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -15, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999991ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -16, 9000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 9999999999999999ULL), dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -17, 9000000000000000ULL)));
   EXPECT_EQ(                               positiveOne, dm_double_add(positiveOne, DM_DOUBLE_PACK(1,  -18, 9000000000000000ULL)));
 }

TEST(DMDoubleTest, testSubs)
 {
   dm_double nan1         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U);
   dm_double nan2         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 2U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);

      // Every sub routine (He he he...) handles NaNs individually so that the resulting NaN doesn't have its sign mutated.
   EXPECT_EQ(nan1, dm_double_sub(nan1, positiveOne));
   EXPECT_EQ(nan1, dm_double_sub(positiveOne, nan1));
   EXPECT_EQ(nan2, dm_double_sub(nan2, nan1));

   EXPECT_EQ(nan1, dm_double_sub_r(nan1, positiveOne, DM_FE_DOWNWARD));
   EXPECT_EQ(nan1, dm_double_sub_r(positiveOne, nan1, DM_FE_DOWNWARD));
   EXPECT_EQ(nan2, dm_double_sub_r(nan2, nan1, DM_FE_DOWNWARD));

      // With the code in mind, any subtraction here will do.
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 4000000000000000ULL), dm_double_sub(DM_DOUBLE_PACK(0, 1, 1000000000000000ULL), DM_DOUBLE_PACK(0, 0, 6000000000000000ULL))); // 10 - 6
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 5000000000000000ULL), dm_double_sub_r(DM_DOUBLE_PACK(0, 1, 1100000000000000ULL), DM_DOUBLE_PACK(0, 0, 6000000000000000ULL), DM_FE_DOWNWARD)); // 11 - 6
 }

TEST(DMDoubleTest, testMuls)
 {
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U);
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);
   dm_double positiveInf  = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double negativeInf  = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double nan1         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U);
   dm_double nan2         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 2U);
   dm_double nan255       = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 255U);
   dm_double nnan255      = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 255U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);

      // Nans
   EXPECT_EQ(nan1, dm_double_mul(nan1, positiveOne));
   EXPECT_EQ(nan1, dm_double_mul(positiveOne, nan1));
   EXPECT_EQ(nan2, dm_double_mul(nan2, nan1));

      // Inf * 0 = Nan
   EXPECT_EQ(nan255, dm_double_mul(positiveInf, positiveZero));
   EXPECT_EQ(nan255, dm_double_mul(positiveZero, positiveInf));
   EXPECT_EQ(nnan255, dm_double_mul(negativeInf, positiveZero));
   EXPECT_EQ(nnan255, dm_double_mul(positiveZero, negativeInf));
   EXPECT_EQ(nnan255, dm_double_mul(positiveInf, negativeZero));
   EXPECT_EQ(nnan255, dm_double_mul(negativeZero, positiveInf));
   EXPECT_EQ(nan255, dm_double_mul(negativeInf, negativeZero));
   EXPECT_EQ(nan255, dm_double_mul(negativeZero, negativeInf));

      // Infinities
   EXPECT_EQ(positiveInf, dm_double_mul(positiveInf, positiveOne));
   EXPECT_EQ(positiveInf, dm_double_mul(positiveOne, positiveInf));
   EXPECT_EQ(negativeInf, dm_double_mul(negativeInf, positiveOne));
   EXPECT_EQ(negativeInf, dm_double_mul(positiveOne, negativeInf));

      // Zeros
   EXPECT_EQ(positiveZero, dm_double_mul(positiveZero, positiveOne));
   EXPECT_EQ(negativeZero, dm_double_mul(negativeOne, positiveZero));
   EXPECT_EQ(negativeZero, dm_double_mul(negativeZero, positiveOne));
   EXPECT_EQ(positiveZero, dm_double_mul(negativeOne, negativeZero));

      // Overflows
   EXPECT_EQ(positiveInf, dm_double_mul(DM_DOUBLE_PACK(0, 511, 9000000000000000ULL), DM_DOUBLE_PACK(0, 511, 9000000000000000ULL)));
   EXPECT_EQ(positiveInf, dm_double_mul(DM_DOUBLE_PACK(1, 511, 9000000000000000ULL), DM_DOUBLE_PACK(1, 511, 9000000000000000ULL)));
   EXPECT_EQ(negativeInf, dm_double_mul(DM_DOUBLE_PACK(1, 511, 9000000000000000ULL), DM_DOUBLE_PACK(0, 511, 9000000000000000ULL)));
   EXPECT_EQ(negativeInf, dm_double_mul(DM_DOUBLE_PACK(0, 511, 9000000000000000ULL), DM_DOUBLE_PACK(1, 511, 9000000000000000ULL)));

      // Underflows
   EXPECT_EQ(positiveZero, dm_double_mul(DM_DOUBLE_PACK(0, -511, 9000000000000000ULL), DM_DOUBLE_PACK(0, -10, 9000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_mul(DM_DOUBLE_PACK(1, -511, 9000000000000000ULL), DM_DOUBLE_PACK(1, -10, 9000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_mul(DM_DOUBLE_PACK(1, -511, 9000000000000000ULL), DM_DOUBLE_PACK(0, -10, 9000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_mul(DM_DOUBLE_PACK(0, -511, 9000000000000000ULL), DM_DOUBLE_PACK(1, -10, 9000000000000000ULL)));

      // Standard cases, no rounding
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 6000000000000000ULL), dm_double_mul(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), DM_DOUBLE_PACK(0, 0, 3000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1800000000000000ULL), dm_double_mul(DM_DOUBLE_PACK(0, 0, 6000000000000000ULL), DM_DOUBLE_PACK(0, 0, 3000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 6000000000000000ULL), dm_double_mul(DM_DOUBLE_PACK(1, 0, 2000000000000000ULL), DM_DOUBLE_PACK(1, 0, 3000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 6000000000000000ULL), dm_double_mul(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), DM_DOUBLE_PACK(1, 0, 3000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 9999999999999998ULL), dm_double_mul(DM_DOUBLE_PACK(0, 0, 9999999999999999ULL), DM_DOUBLE_PACK(0, 0, 9999999999999999ULL)));

      // Flushes to infinity
   EXPECT_EQ(positiveInf, dm_double_mul(DM_DOUBLE_PACK(0, 255, 2000000000000000ULL), DM_DOUBLE_PACK(0, 256, 5000000000000000ULL)));
   EXPECT_EQ(negativeInf, dm_double_mul(DM_DOUBLE_PACK(1, 255, 2000000000000000ULL), DM_DOUBLE_PACK(0, 256, 5000000000000000ULL)));

      // Flushes to zero
   EXPECT_EQ(positiveZero, dm_double_mul(DM_DOUBLE_PACK(0, -256, 2000000000000000ULL), DM_DOUBLE_PACK(0, -256, 3000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_mul(DM_DOUBLE_PACK(1, -256, 2000000000000000ULL), DM_DOUBLE_PACK(0, -256, 3000000000000000ULL)));

      // Saved from zero
   EXPECT_EQ(DM_DOUBLE_PACK(0, -511, 1200000000000000ULL), dm_double_mul(DM_DOUBLE_PACK(0, -256, 4000000000000000ULL), DM_DOUBLE_PACK(0, -256, 3000000000000000ULL)));

      // Standard cases, rounding
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 2000000000000000ULL), dm_double_mul(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), DM_DOUBLE_PACK(0, 0, 9999999999999999ULL)));
 }

TEST(DMDoubleTest, testDivs)
 {
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U);
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);
   dm_double positiveInf  = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double negativeInf  = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double nan1         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U);
   dm_double nan2         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 2U);
   dm_double nan255       = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 255U);
   dm_double nnan255      = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 255U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);

      // Nans
   EXPECT_EQ(nan1, dm_double_div(nan1, positiveOne));
   EXPECT_EQ(nan1, dm_double_div(positiveOne, nan1));
   EXPECT_EQ(nan2, dm_double_div(nan2, nan1));

      // Inf / Inf = Nan, 0 / 0 = Nan
   EXPECT_EQ(nan255, dm_double_div(positiveInf, positiveInf));
   EXPECT_EQ(nan255, dm_double_div(negativeInf, negativeInf));
   EXPECT_EQ(nnan255, dm_double_div(negativeZero, positiveZero));
   EXPECT_EQ(nnan255, dm_double_div(positiveZero, negativeZero));

      // Infinities
   EXPECT_EQ(positiveInf, dm_double_div(positiveInf, positiveOne));
   EXPECT_EQ(positiveInf, dm_double_div(positiveOne, positiveZero));
   EXPECT_EQ(negativeInf, dm_double_div(negativeInf, positiveOne));
   EXPECT_EQ(negativeInf, dm_double_div(positiveOne, negativeZero));

      // Zeros
   EXPECT_EQ(positiveZero, dm_double_div(positiveOne, positiveInf));
   EXPECT_EQ(positiveZero, dm_double_div(positiveZero, positiveOne));
   EXPECT_EQ(negativeZero, dm_double_div(negativeOne, positiveInf));
   EXPECT_EQ(negativeZero, dm_double_div(negativeZero, positiveOne));

      // Overflows
   EXPECT_EQ(positiveInf, dm_double_div(DM_DOUBLE_PACK(0, 511, 9000000000000000ULL), DM_DOUBLE_PACK(0, -511, 9000000000000000ULL)));
   EXPECT_EQ(positiveInf, dm_double_div(DM_DOUBLE_PACK(1, 511, 9000000000000000ULL), DM_DOUBLE_PACK(1, -511, 9000000000000000ULL)));
   EXPECT_EQ(negativeInf, dm_double_div(DM_DOUBLE_PACK(1, 511, 9000000000000000ULL), DM_DOUBLE_PACK(0, -511, 9000000000000000ULL)));
   EXPECT_EQ(negativeInf, dm_double_div(DM_DOUBLE_PACK(0, 511, 9000000000000000ULL), DM_DOUBLE_PACK(1, -511, 9000000000000000ULL)));

      // Underflows
   EXPECT_EQ(positiveZero, dm_double_div(DM_DOUBLE_PACK(0, -511, 9000000000000000ULL), DM_DOUBLE_PACK(0, 10, 9000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_div(DM_DOUBLE_PACK(1, -511, 9000000000000000ULL), DM_DOUBLE_PACK(1, 10, 9000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_div(DM_DOUBLE_PACK(1, -511, 9000000000000000ULL), DM_DOUBLE_PACK(0, 10, 9000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_div(DM_DOUBLE_PACK(0, -511, 9000000000000000ULL), DM_DOUBLE_PACK(1, 10, 9000000000000000ULL)));

      // Standard cases, no rounding
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), dm_double_div(DM_DOUBLE_PACK(0, 0, 6000000000000000ULL), DM_DOUBLE_PACK(0, 0, 3000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 4000000000000000ULL), dm_double_div(DM_DOUBLE_PACK(0, 0, 1200000000000000ULL), DM_DOUBLE_PACK(0, 0, 3000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), dm_double_div(DM_DOUBLE_PACK(1, 0, 6000000000000000ULL), DM_DOUBLE_PACK(1, 0, 3000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 2000000000000000ULL), dm_double_div(DM_DOUBLE_PACK(0, 0, 6000000000000000ULL), DM_DOUBLE_PACK(1, 0, 3000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 9999999999999999ULL), dm_double_div(DM_DOUBLE_PACK(0, 0, 9999999999999999ULL), DM_DOUBLE_PACK(0, 0, 1000000000000000ULL)));

      // Flushes to infinity
   EXPECT_EQ(positiveInf, dm_double_div(DM_DOUBLE_PACK(0, 256, 9000000000000000ULL), DM_DOUBLE_PACK(0, -256, 3000000000000000ULL)));
   EXPECT_EQ(negativeInf, dm_double_div(DM_DOUBLE_PACK(1, 256, 9000000000000000ULL), DM_DOUBLE_PACK(0, -256, 3000000000000000ULL)));

      // Flushes to zero
   EXPECT_EQ(positiveZero, dm_double_div(DM_DOUBLE_PACK(0, -255, 1200000000000000ULL), DM_DOUBLE_PACK(0, 256, 3000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_div(DM_DOUBLE_PACK(1, -255, 1200000000000000ULL), DM_DOUBLE_PACK(0, 256, 3000000000000000ULL)));

      // Saved from infinity
   EXPECT_EQ(DM_DOUBLE_PACK(0, 511, 4000000000000000ULL), dm_double_div(DM_DOUBLE_PACK(0, 256, 1200000000000000ULL), DM_DOUBLE_PACK(0, -256, 3000000000000000ULL)));

      // Standard cases, rounding
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 6666666666666667ULL), dm_double_div(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), DM_DOUBLE_PACK(0, 0, 3000000000000000ULL)));
 }

TEST(DMDoubleTest, testStringConversions)
 {
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 5000000000000000ULL), dm_double_fromstring("5.0"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1234567890123456ULL), dm_double_fromstring("12345678901234561"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1234567890123457ULL), dm_double_fromstring("12345678901234569"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1234567890123456ULL), dm_double_fromstring("12345678901234565"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1234567890123458ULL), dm_double_fromstring("12345678901234575"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 18, 1234567890123457ULL), dm_double_fromstring("1234567890123456501"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1234567890123456ULL), dm_double_fromstring("12345678901234560"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 17, 1234567890123456ULL), dm_double_fromstring("123456789012345600"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 17, 1234567890123456ULL), dm_double_fromstring("123456789012345601"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 17, 1234567890123456ULL), dm_double_fromstring("123456789012345611"));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 5000000000000000ULL), dm_double_fromstring("-5.0"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dm_double_fromstring("1.2345678901234561"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123457ULL), dm_double_fromstring("1.2345678901234569"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dm_double_fromstring("1.2345678901234565"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dm_double_fromstring("1.23456789012345650"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123457ULL), dm_double_fromstring("1.234567890123456501"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dm_double_fromstring("1.2345678901234560"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dm_double_fromstring("1.23456789012345600"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dm_double_fromstring("1.23456789012345601"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dm_double_fromstring("1.23456789012345611"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1000000000000000ULL), dm_double_fromstring("9.9999999999999999"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 10, 5000000000000000ULL), dm_double_fromstring("5e10"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 10, 5000000000000000ULL), dm_double_fromstring("5e+10"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -10, 5000000000000000ULL), dm_double_fromstring("5e-10"));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromstring("5e1000"));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U), dm_double_fromstring("5e-1000"));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromstring("-5e1000"));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U), dm_double_fromstring("-5e-1000"));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromstring("5e2000"));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U), dm_double_fromstring("5e-2000"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -2, 1000000000000000ULL), dm_double_fromstring("0.01"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -3, 1000000000000000ULL), dm_double_fromstring("0.001"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -2, 1000000000000000ULL), dm_double_fromstring("0,01"));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -3, 1000000000000000ULL), dm_double_fromstring("0,001"));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U), dm_double_fromstring("0e1000000"));

   char dest [25];

   dm_double_tostring(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U), dest);
   EXPECT_STREQ("NaN", dest);
   dm_double_tostring(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 1U), dest);
   EXPECT_STREQ("-NaN", dest);
   dm_double_tostring(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY), dest);
   EXPECT_STREQ("Inf", dest);
   dm_double_tostring(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY), dest);
   EXPECT_STREQ("-Inf", dest);
   dm_double_tostring(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U), dest);
   EXPECT_STREQ("0.000000000000000e+0", dest);
   dm_double_tostring(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U), dest);
   EXPECT_STREQ("-0.000000000000000e+0", dest);
   dm_double_tostring(DM_DOUBLE_PACK(0, -511, 1234567890123456ULL), dest);
   EXPECT_STREQ("1.234567890123456e-511", dest);
   dm_double_tostring(DM_DOUBLE_PACK(1, 511, 1234567890123456ULL), dest);
   EXPECT_STREQ("-1.234567890123456e+511", dest);

   dm_double_tostring(DM_DOUBLE_PACK(0, -82, 1234567890123456ULL), dest);
   EXPECT_STREQ("1.234567890123456e-82", dest);
   dm_double_tostring(DM_DOUBLE_PACK(1, 82, 1234567890123456ULL), dest);
   EXPECT_STREQ("-1.234567890123456e+82", dest);

   dm_double_tostring(DM_DOUBLE_PACK(0, 90, 10000000123456789ULL), dest);
#ifndef MISRAbleC
   EXPECT_STREQ("10.000000123456789e+90", dest);
#else /* MISRAbleC version */
   EXPECT_STREQ( "0.000000123456789e+90", dest);
#endif /* MISRAbleC */
 }

TEST(DMDoubleTest, testDoubleConversions)
 {
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U), dm_double_fromdouble(0.0));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U), dm_double_fromdouble(-0.0));
#ifndef MISRAbleC
   EXPECT_EQ(0U, dm_double_fromdouble(1.0)); // The Boulder is conflicted....
#else /* MISRAbleC version */
   EXPECT_EQ(0U, dm_double_fromdouble(0.0)); // The Boulder is unconflicted.
#endif /* MISRAbleC */
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000000000ULL), dm_double_fromdouble(1.0));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 1000000000000000ULL), dm_double_fromdouble(-1.0));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromdouble(std::pow(10.0, 10000.0)));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromdouble(-std::pow(10.0, 10000.0)));
   if (!std::signbit(std::asin(2.0)))
    {
      EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 127U), dm_double_fromdouble(std::asin(2.0)));
      EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 127U), dm_double_fromdouble(-std::asin(2.0)));
    }
   else
    {
      EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 127U), dm_double_fromdouble(-std::asin(2.0)));
      EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 127U), dm_double_fromdouble(std::asin(2.0)));
    }
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1000000000000000ULL), dm_double_fromdouble(9.9999999999999999));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 1, 1000000000000000ULL), dm_double_fromdouble(-9.9999999999999999));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 9999999999999998ULL), dm_double_fromdouble(9.9999999999999991)); // This is a limitation of double
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 9999999999999998ULL), dm_double_fromdouble(-9.9999999999999991)); // This is a limitation of double
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 9999999999999998ULL), dm_double_fromdouble(9.9999999999999989)); // This is a limitation of double
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 9999999999999998ULL), dm_double_fromdouble(-9.9999999999999989)); // This is a limitation of double
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL), dm_double_fromdouble(0.5));
   EXPECT_EQ(DM_DOUBLE_PACK(1, -1, 5000000000000000ULL), dm_double_fromdouble(-0.5));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_fromdouble(10000000000000005));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000001ULL), dm_double_fromdouble(10000000000000013));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000002ULL), dm_double_fromdouble(10000000000000015));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000002ULL), dm_double_fromdouble(10000000000000018));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000001ULL), dm_double_fromdouble(10000000000000010));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000002ULL), dm_double_fromdouble(10000000000000025));


   EXPECT_EQ(0.0, dm_double_todouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U)));
   EXPECT_EQ(-0.0, dm_double_todouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U)));
   EXPECT_EQ(0, std::signbit(dm_double_todouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U))));
   EXPECT_NE(0, std::signbit(dm_double_todouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U))));
   EXPECT_EQ(1.0, dm_double_todouble(DM_DOUBLE_PACK(0, 0, 1000000000000000ULL)));
   EXPECT_EQ(-1.0, dm_double_todouble(DM_DOUBLE_PACK(1, 0, 1000000000000000ULL)));
   EXPECT_EQ(10000000000.0, dm_double_todouble(DM_DOUBLE_PACK(0, 10, 1000000000000000ULL)));
   EXPECT_EQ(-10000000000.0, dm_double_todouble(DM_DOUBLE_PACK(1, 10, 1000000000000000ULL)));
   volatile double base = 10.0;        // This variable is volatile so that the compiler
   if (1e100 == std::pow(base, 100.0)) // can't pre-compute this function call.
    {
      EXPECT_EQ(1e100, dm_double_todouble(DM_DOUBLE_PACK(0, 100, 1000000000000000ULL)));
      EXPECT_EQ(-1e100, dm_double_todouble(DM_DOUBLE_PACK(1, 100, 1000000000000000ULL)));
    }
   else
    {
      std::cout << "Windows sucks." << std::endl;
    }
   EXPECT_EQ(1e100, static_cast<double>(dm_double_tolongdouble(DM_DOUBLE_PACK(0, 100, 1000000000000000ULL))));
   EXPECT_EQ(-1e100, static_cast<double>(dm_double_tolongdouble(DM_DOUBLE_PACK(1, 100, 1000000000000000ULL))));
   EXPECT_EQ(0.5, dm_double_todouble(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL)));
   EXPECT_EQ(-0.5, dm_double_todouble(DM_DOUBLE_PACK(1, -1, 5000000000000000ULL)));

   EXPECT_NE(0, std::isinf(dm_double_todouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY))));
   EXPECT_EQ(0, std::signbit(dm_double_todouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY))));
   EXPECT_NE(0, std::isinf(dm_double_todouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY))));
   EXPECT_NE(0, std::signbit(dm_double_todouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY))));
   EXPECT_NE(0, std::isnan(dm_double_todouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U))));
   EXPECT_EQ(0, std::signbit(dm_double_todouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U))));
   EXPECT_NE(0, std::isnan(dm_double_todouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 1U))));
   EXPECT_NE(0, std::signbit(dm_double_todouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 1U))));


   dm_fesetround(DM_FE_FROMZERO);
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), dm_double_fromdouble(2.0));
   dm_fesetround(DM_FE_UPWARD);
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), dm_double_fromdouble(2.0));
   dm_fesetround(DM_FE_TONEAREST);

   EXPECT_EQ(DM_DOUBLE_PACK(0, -20, 1000000000000000ULL), dm_double_fromdouble(1e-20));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -19, 1000000000000000ULL), dm_double_fromdouble(1e-19));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -18, 1000000000000000ULL), dm_double_fromdouble(1e-18));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -17, 1000000000000000ULL), dm_double_fromdouble(1e-17));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -16, 1000000000000000ULL), dm_double_fromdouble(1e-16));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -15, 1000000000000000ULL), dm_double_fromdouble(1e-15));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -14, 1000000000000000ULL), dm_double_fromdouble(1e-14));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -13, 1000000000000000ULL), dm_double_fromdouble(1e-13));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -12, 1000000000000000ULL), dm_double_fromdouble(1e-12));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -11, 1000000000000000ULL), dm_double_fromdouble(1e-11));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -10, 1000000000000000ULL), dm_double_fromdouble(1e-10));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -9, 1000000000000000ULL), dm_double_fromdouble( 1e-9));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -8, 1000000000000000ULL), dm_double_fromdouble( 1e-8));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -7, 1000000000000000ULL), dm_double_fromdouble( 1e-7));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -6, 1000000000000000ULL), dm_double_fromdouble( 1e-6));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -5, 1000000000000000ULL), dm_double_fromdouble( 1e-5));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -4, 1000000000000000ULL), dm_double_fromdouble( 1e-4));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -3, 1000000000000000ULL), dm_double_fromdouble( 1e-3));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -2, 1000000000000000ULL), dm_double_fromdouble( 1e-2));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -1, 1000000000000000ULL), dm_double_fromdouble( 1e-1));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   0, 1000000000000000ULL), dm_double_fromdouble(  1e0));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   1, 1000000000000000ULL), dm_double_fromdouble(  1e1));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   2, 1000000000000000ULL), dm_double_fromdouble(  1e2));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   3, 1000000000000000ULL), dm_double_fromdouble(  1e3));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   4, 1000000000000000ULL), dm_double_fromdouble(  1e4));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   5, 1000000000000000ULL), dm_double_fromdouble(  1e5));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   6, 1000000000000000ULL), dm_double_fromdouble(  1e6));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   7, 1000000000000000ULL), dm_double_fromdouble(  1e7));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   8, 1000000000000000ULL), dm_double_fromdouble(  1e8));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   9, 1000000000000000ULL), dm_double_fromdouble(  1e9));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  10, 1000000000000000ULL), dm_double_fromdouble( 1e10));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  11, 1000000000000000ULL), dm_double_fromdouble( 1e11));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  12, 1000000000000000ULL), dm_double_fromdouble( 1e12));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  13, 1000000000000000ULL), dm_double_fromdouble( 1e13));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  14, 1000000000000000ULL), dm_double_fromdouble( 1e14));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  15, 1000000000000000ULL), dm_double_fromdouble( 1e15));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  16, 1000000000000000ULL), dm_double_fromdouble( 1e16));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  17, 1000000000000000ULL), dm_double_fromdouble( 1e17));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  18, 1000000000000000ULL), dm_double_fromdouble( 1e18));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  19, 1000000000000000ULL), dm_double_fromdouble( 1e19));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  20, 1000000000000000ULL), dm_double_fromdouble( 1e20));
 }

TEST(DMDoubleTest, testLongDoubleConversions)
 {
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U), dm_double_fromlongdouble(0.0L));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U), dm_double_fromlongdouble(-0.0L));
#ifndef MISRAbleC
   EXPECT_EQ(0U, dm_double_fromlongdouble(1.0L)); // The Boulder is conflicted....
#else /* MISRAbleC version */
   EXPECT_EQ(0U, dm_double_fromlongdouble(0.0L)); // The Boulder is unconflicted.
#endif /* MISRAbleC */
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000000000ULL), dm_double_fromlongdouble(1.0L));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 1000000000000000ULL), dm_double_fromlongdouble(-1.0L));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromlongdouble(std::pow(10.0L, 10000.0L)));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromlongdouble(-std::pow(10.0L, 10000.0L)));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 127U), dm_double_fromlongdouble(std::nan(""))); // std::asin(2.0L) is negative?
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 127U), dm_double_fromlongdouble(-std::nan("")));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1000000000000000ULL), dm_double_fromlongdouble(9.9999999999999999L));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 1, 1000000000000000ULL), dm_double_fromlongdouble(-9.9999999999999999L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 9999999999999999ULL), dm_double_fromlongdouble(9.9999999999999991L));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 9999999999999999ULL), dm_double_fromlongdouble(-9.9999999999999991L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 9999999999999999ULL), dm_double_fromlongdouble(9.9999999999999989L));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 9999999999999999ULL), dm_double_fromlongdouble(-9.9999999999999989L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL), dm_double_fromlongdouble(0.5L));
   EXPECT_EQ(DM_DOUBLE_PACK(1, -1, 5000000000000000ULL), dm_double_fromlongdouble(-0.5L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000000ULL), dm_double_fromlongdouble(10000000000000005L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000001ULL), dm_double_fromlongdouble(10000000000000013L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000002ULL), dm_double_fromlongdouble(10000000000000015l));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000002ULL), dm_double_fromlongdouble(10000000000000018L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000001ULL), dm_double_fromlongdouble(10000000000000010L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 16, 1000000000000002ULL), dm_double_fromlongdouble(10000000000000025L));

   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromlongdouble(1e1000L));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY), dm_double_fromlongdouble(-1e1000L));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U), dm_double_fromlongdouble(1e-1000L));
   EXPECT_EQ(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U), dm_double_fromlongdouble(-1e-1000L));


   EXPECT_EQ(0.0L, dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U)));
   EXPECT_EQ(-0.0L, dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U)));
   EXPECT_EQ(0, std::signbit(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U))));
   EXPECT_NE(0, std::signbit(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U))));
   EXPECT_EQ(1.0L, dm_double_tolongdouble(DM_DOUBLE_PACK(0, 0, 1000000000000000ULL)));
   EXPECT_EQ(-1.0L, dm_double_tolongdouble(DM_DOUBLE_PACK(1, 0, 1000000000000000ULL)));
   EXPECT_EQ(10000000000.0L, dm_double_tolongdouble(DM_DOUBLE_PACK(0, 10, 1000000000000000ULL)));
   EXPECT_EQ(-10000000000.0L, dm_double_tolongdouble(DM_DOUBLE_PACK(1, 10, 1000000000000000ULL)));
   EXPECT_EQ(1e100L, dm_double_tolongdouble(DM_DOUBLE_PACK(0, 100, 1000000000000000ULL)));
   EXPECT_EQ(-1e100L, dm_double_tolongdouble(DM_DOUBLE_PACK(1, 100, 1000000000000000ULL)));
   EXPECT_EQ(0.5L, dm_double_tolongdouble(DM_DOUBLE_PACK(0, -1, 5000000000000000ULL)));
   EXPECT_EQ(-0.5L, dm_double_tolongdouble(DM_DOUBLE_PACK(1, -1, 5000000000000000ULL)));

   EXPECT_NE(0, std::isinf(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY))));
   EXPECT_EQ(0, std::signbit(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY))));
   EXPECT_NE(0, std::isinf(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY))));
   EXPECT_NE(0, std::signbit(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY))));
   EXPECT_NE(0, std::isnan(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U))));
   EXPECT_EQ(0, std::signbit(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U))));
   EXPECT_NE(0, std::isnan(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 1U))));
   EXPECT_NE(0, std::signbit(dm_double_tolongdouble(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 1U))));


   dm_fesetround(DM_FE_FROMZERO);
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), dm_double_fromlongdouble(2.0L));
   dm_fesetround(DM_FE_UPWARD);
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), dm_double_fromlongdouble(2.0L));
   dm_fesetround(DM_FE_TONEAREST);

   EXPECT_EQ(DM_DOUBLE_PACK(0, -20, 1000000000000000ULL), dm_double_fromlongdouble(1e-20L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -19, 1000000000000000ULL), dm_double_fromlongdouble(1e-19L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -18, 1000000000000000ULL), dm_double_fromlongdouble(1e-18L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -17, 1000000000000000ULL), dm_double_fromlongdouble(1e-17L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -16, 1000000000000000ULL), dm_double_fromlongdouble(1e-16L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -15, 1000000000000000ULL), dm_double_fromlongdouble(1e-15L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -14, 1000000000000000ULL), dm_double_fromlongdouble(1e-14L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -13, 1000000000000000ULL), dm_double_fromlongdouble(1e-13L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -12, 1000000000000000ULL), dm_double_fromlongdouble(1e-12L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -11, 1000000000000000ULL), dm_double_fromlongdouble(1e-11L));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -10, 1000000000000000ULL), dm_double_fromlongdouble(1e-10L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -9, 1000000000000000ULL), dm_double_fromlongdouble( 1e-9L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -8, 1000000000000000ULL), dm_double_fromlongdouble( 1e-8L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -7, 1000000000000000ULL), dm_double_fromlongdouble( 1e-7L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -6, 1000000000000000ULL), dm_double_fromlongdouble( 1e-6L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -5, 1000000000000000ULL), dm_double_fromlongdouble( 1e-5L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -4, 1000000000000000ULL), dm_double_fromlongdouble( 1e-4L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -3, 1000000000000000ULL), dm_double_fromlongdouble( 1e-3L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -2, 1000000000000000ULL), dm_double_fromlongdouble( 1e-2L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  -1, 1000000000000000ULL), dm_double_fromlongdouble( 1e-1L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   0, 1000000000000000ULL), dm_double_fromlongdouble(  1e0L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   1, 1000000000000000ULL), dm_double_fromlongdouble(  1e1L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   2, 1000000000000000ULL), dm_double_fromlongdouble(  1e2L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   3, 1000000000000000ULL), dm_double_fromlongdouble(  1e3L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   4, 1000000000000000ULL), dm_double_fromlongdouble(  1e4L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   5, 1000000000000000ULL), dm_double_fromlongdouble(  1e5L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   6, 1000000000000000ULL), dm_double_fromlongdouble(  1e6L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   7, 1000000000000000ULL), dm_double_fromlongdouble(  1e7L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   8, 1000000000000000ULL), dm_double_fromlongdouble(  1e8L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,   9, 1000000000000000ULL), dm_double_fromlongdouble(  1e9L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  10, 1000000000000000ULL), dm_double_fromlongdouble( 1e10L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  11, 1000000000000000ULL), dm_double_fromlongdouble( 1e11L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  12, 1000000000000000ULL), dm_double_fromlongdouble( 1e12L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  13, 1000000000000000ULL), dm_double_fromlongdouble( 1e13L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  14, 1000000000000000ULL), dm_double_fromlongdouble( 1e14L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  15, 1000000000000000ULL), dm_double_fromlongdouble( 1e15L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  16, 1000000000000000ULL), dm_double_fromlongdouble( 1e16L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  17, 1000000000000000ULL), dm_double_fromlongdouble( 1e17L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  18, 1000000000000000ULL), dm_double_fromlongdouble( 1e18L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  19, 1000000000000000ULL), dm_double_fromlongdouble( 1e19L));
   EXPECT_EQ(DM_DOUBLE_PACK(0,  20, 1000000000000000ULL), dm_double_fromlongdouble( 1e20L));
 }

TEST(DMDoubleTest, testFmod)
 {
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U); // Special exponent, zero significand : zero
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);
   dm_double positiveInf  = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY); // Special exponent, ones significand : infinity
   dm_double negativeInf  = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double nan1         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U); // Special exponent, any other significand : NaN
   dm_double nan2         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 2U);
   dm_double nan255       = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 255U);
   dm_double nnan255      = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 255U);

      // Nans
   EXPECT_EQ(nan1, dm_double_fmod(nan1, positiveOne));
   EXPECT_EQ(nan1, dm_double_fmod(positiveOne, nan1));
   EXPECT_EQ(nan2, dm_double_fmod(nan2, nan1));

      // Inf % x = Nan, x % 0 = Nan
   EXPECT_EQ(nan255, dm_double_fmod(positiveInf, positiveOne));
   EXPECT_EQ(nnan255, dm_double_fmod(negativeInf, positiveOne));
   EXPECT_EQ(nan255, dm_double_fmod(positiveOne, positiveZero)); // Sign is sign of lhs.
   EXPECT_EQ(nnan255, dm_double_fmod(negativeOne, positiveZero));
   EXPECT_EQ(nan255, dm_double_fmod(positiveOne, negativeZero));
   EXPECT_EQ(nnan255, dm_double_fmod(negativeOne, negativeZero));

      // Return Argument
   EXPECT_EQ(positiveOne, dm_double_fmod(positiveOne, positiveInf));
   EXPECT_EQ(positiveOne, dm_double_fmod(positiveOne, negativeInf));
   EXPECT_EQ(negativeOne, dm_double_fmod(negativeOne, positiveInf));
   EXPECT_EQ(negativeOne, dm_double_fmod(negativeOne, negativeInf));
   EXPECT_EQ(positiveZero, dm_double_fmod(positiveZero, positiveOne));
   EXPECT_EQ(positiveZero, dm_double_fmod(positiveZero, negativeOne));
   EXPECT_EQ(negativeZero, dm_double_fmod(negativeZero, positiveOne));
   EXPECT_EQ(negativeZero, dm_double_fmod(negativeZero, negativeOne));

   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 3000000000000000ULL), dm_double_fmod(DM_DOUBLE_PACK(0, 0, 7000000000000000ULL), DM_DOUBLE_PACK(0, 0, 4000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 3000000000000000ULL), dm_double_fmod(DM_DOUBLE_PACK(1, 0, 7000000000000000ULL), DM_DOUBLE_PACK(0, 0, 4000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2000000000000000ULL), dm_double_fmod(DM_DOUBLE_PACK(0, 1, 7000000000000000ULL), DM_DOUBLE_PACK(0, 0, 4000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_fmod(DM_DOUBLE_PACK(0, 1, 6800000000000000ULL), DM_DOUBLE_PACK(0, 0, 4000000000000000ULL)));
   EXPECT_EQ(negativeZero, dm_double_fmod(DM_DOUBLE_PACK(1, 1, 6800000000000000ULL), DM_DOUBLE_PACK(0, 0, 4000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_fmod(DM_DOUBLE_PACK(0, 2, 6800000000000000ULL), DM_DOUBLE_PACK(0, 0, 4000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, -15, 1000000000000000ULL), dm_double_fmod(DM_DOUBLE_PACK(0, 0, 8000000000000001ULL), DM_DOUBLE_PACK(0, 0, 4000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_fmod(DM_DOUBLE_PACK(0, -500, 8000000000000001ULL), DM_DOUBLE_PACK(0, -500, 4000000000000000ULL)));
 }

TEST(DMDoubleTest, testFMA)
 {
   dm_double positiveZero = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U);
   dm_double negativeZero = DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U);
   dm_double positiveOne  = DM_DOUBLE_PACK(0, 0, 1000000000000000ULL);
   dm_double negativeOne  = DM_DOUBLE_PACK(1, 0, 1000000000000000ULL);
   dm_double positiveTwo  = DM_DOUBLE_PACK(0, 0, 2000000000000000ULL);
   dm_double positiveThr  = DM_DOUBLE_PACK(0, 0, 3000000000000000ULL);
   dm_double positiveSix  = DM_DOUBLE_PACK(0, 0, 6000000000000000ULL);
   dm_double positiveTen  = DM_DOUBLE_PACK(0, 1, 1000000000000000ULL);
   dm_double negativeTen  = DM_DOUBLE_PACK(1, 1, 1000000000000000ULL);
   dm_double positiveInf  = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY);
   dm_double nan1         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U);
   dm_double nan2         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 2U);
   dm_double nan3         = DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 3U);

   EXPECT_EQ(nan1, dm_double_fma(nan1, nan2, nan3));
   EXPECT_EQ(nan1, dm_double_fma(nan1, positiveOne, positiveOne));
   EXPECT_EQ(nan2, dm_double_fma(positiveOne, nan2, nan3));
   EXPECT_EQ(nan2, dm_double_fma(positiveOne, nan2, positiveOne));
   EXPECT_EQ(nan3, dm_double_fma(positiveOne, positiveOne, nan3));

   EXPECT_EQ(positiveOne, dm_double_fma(positiveZero, positiveTwo, positiveOne));
   EXPECT_EQ(positiveThr, dm_double_fma(positiveTwo, positiveZero, positiveThr));
   EXPECT_EQ(positiveSix, dm_double_fma(positiveTwo, positiveThr, positiveZero));

   EXPECT_EQ(positiveInf, dm_double_fma(positiveInf, positiveOne, positiveOne));
   EXPECT_EQ(positiveInf, dm_double_fma(positiveOne, positiveInf, positiveOne));
   EXPECT_EQ(positiveInf, dm_double_fma(positiveOne, positiveOne, positiveInf));


   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1200000000000000ULL), dm_double_fma(positiveTwo, positiveThr, positiveSix));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 8000000000000000ULL), dm_double_fma(positiveTwo, positiveOne, positiveSix));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 2, 1010000000000000ULL), dm_double_fma(positiveTen, positiveTen, positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 9900000000000000ULL), dm_double_fma(positiveTen, positiveTen, negativeOne));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 2, 1010000000000000ULL), dm_double_fma(positiveTen, negativeTen, negativeOne));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 1, 9900000000000000ULL), dm_double_fma(positiveTen, negativeTen, positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 2, 1010000000000000ULL), dm_double_fma(positiveOne, positiveOne, DM_DOUBLE_PACK(0, 2, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 9900000000000000ULL), dm_double_fma(positiveOne, negativeOne, DM_DOUBLE_PACK(0, 2, 1000000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_fma(DM_DOUBLE_PACK(0, 0, 9999999999999999ULL), positiveOne, DM_DOUBLE_PACK(0, -16, 9000000000000000ULL)));
   EXPECT_EQ(positiveInf, dm_double_fma(DM_DOUBLE_PACK(0, 256, 1000000000000000ULL), DM_DOUBLE_PACK(0, 256, 1000000000000000ULL), DM_DOUBLE_PACK(0, 511, 1000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_fma(DM_DOUBLE_PACK(0, -256, 1000000000000000ULL), DM_DOUBLE_PACK(1, -266, 9000000000000000ULL), DM_DOUBLE_PACK(0, -511, 1000000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_fma(positiveTen, positiveOne, DM_DOUBLE_PACK(0, -19, 1000000000000000ULL)));
   EXPECT_EQ(positiveTen, dm_double_fma(DM_DOUBLE_PACK(0, -19, 1000000000000000ULL), positiveOne, positiveTen));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000100000ULL), dm_double_fma(positiveOne, positiveOne, DM_DOUBLE_PACK(0, -10, 1000000001000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000100000ULL), dm_double_fma(positiveOne, positiveOne, DM_DOUBLE_PACK(0, -10, 1000000401000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000100000ULL), dm_double_fma(positiveOne, positiveOne, DM_DOUBLE_PACK(0, -10, 1000000501000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000100000ULL), dm_double_fma(positiveOne, positiveOne, DM_DOUBLE_PACK(0, -10, 1000000601000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000100000ULL), dm_double_fma(positiveOne, DM_DOUBLE_PACK(0, -10, 1000000001000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000100000ULL), dm_double_fma(positiveOne, DM_DOUBLE_PACK(0, -10, 1000000401000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000100000ULL), dm_double_fma(positiveOne, DM_DOUBLE_PACK(0, -10, 1000000501000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 1000000000100000ULL), dm_double_fma(positiveOne, DM_DOUBLE_PACK(0, -10, 1000000601000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 8200000000000000ULL), dm_double_fma(DM_DOUBLE_PACK(0, 0, 9000000000000000ULL), DM_DOUBLE_PACK(0, 0, 9000000000000000ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2524157875323882ULL), dm_double_fma(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2524157875570795ULL), dm_double_fma(DM_DOUBLE_PACK(0, 0, 1234567890323456ULL), DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 0, 2524157875323876ULL), dm_double_fma(DM_DOUBLE_PACK(0, 0, 1234567890123452ULL), DM_DOUBLE_PACK(0, 0, 1234567890123455ULL), positiveOne));


   EXPECT_EQ(positiveZero, dm_double_fma(positiveTen, negativeTen, DM_DOUBLE_PACK(0, 2, 1000000000000000ULL)));
   dm_fesetround(DM_FE_DOWNWARD);
   EXPECT_EQ(negativeZero, dm_double_fma(positiveTen, negativeTen, DM_DOUBLE_PACK(0, 2, 1000000000000000ULL)));
   dm_fesetround(DM_FE_TONEAREST);
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 9000000000000000ULL), dm_double_fma(positiveOne, negativeTen, positiveOne));
   EXPECT_EQ(DM_DOUBLE_PACK(1, 0, 9000000000000000ULL), dm_double_fma(positiveOne, positiveOne, negativeTen));
   EXPECT_EQ(DM_DOUBLE_PACK(0, 1, 1900000000000000ULL), dm_double_fma(positiveOne, negativeOne, DM_DOUBLE_PACK(0, 1, 2000000000000000ULL)));
   EXPECT_EQ(positiveThr, dm_double_fma(positiveTwo, positiveThr, dm_double_neg(positiveThr)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, -10, 3333499999997531ULL), dm_double_fma(DM_DOUBLE_PACK(0, 0, 9999999999988888ULL), DM_DOUBLE_PACK(0, 0, 9999999999977777ULL), DM_DOUBLE_PACK(1, 2, 1000000000000000ULL)));
   EXPECT_EQ(DM_DOUBLE_PACK(1, -10, 3333500000000000ULL), dm_double_add(dm_double_mul(DM_DOUBLE_PACK(0, 0, 9999999999988888ULL), DM_DOUBLE_PACK(0, 0, 9999999999977777ULL)), DM_DOUBLE_PACK(1, 2, 1000000000000000ULL)));

   EXPECT_EQ(positiveInf, dm_double_fma(DM_DOUBLE_PACK(0, 256, 1000000000000000ULL), DM_DOUBLE_PACK(0, 256, 2000000000000000ULL), DM_DOUBLE_PACK(1, 511, 1000000000000000ULL)));
   EXPECT_EQ(positiveZero, dm_double_fma(DM_DOUBLE_PACK(0, -256, 1000000000000000ULL), DM_DOUBLE_PACK(1, -256, 9000000000000000ULL), DM_DOUBLE_PACK(0, -511, 1000000000000000ULL)));
 }

TEST(DMDoubleTest, testPrettyPrint)
 {
   char dest [24];

   dm_double_toprettystring(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 1U), dest);
   EXPECT_STREQ("NaN", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 1U), dest);
   EXPECT_STREQ("-NaN", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, DM_INFINITY), dest);
   EXPECT_STREQ("Inf", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, DM_INFINITY), dest);
   EXPECT_STREQ("-Inf", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK_ALT(0, SPECIAL_EXPONENT, 0U), dest);
   EXPECT_STREQ("0", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK_ALT(1, SPECIAL_EXPONENT, 0U), dest);
   EXPECT_STREQ("-0", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(0, -511, 1234567890123456ULL), dest);
   EXPECT_STREQ("1.234567890123456e-511", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 511, 1234567890123456ULL), dest);
   EXPECT_STREQ("-1.234567890123456e+511", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(1, -82, 1234567890123456ULL), dest);
   EXPECT_STREQ("-1.234567890123456e-82", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(0, 82, 1234567890123456ULL), dest);
   EXPECT_STREQ("1.234567890123456e+82", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, 90, 10000000123456789ULL), dest);
   EXPECT_STREQ("Err", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 90, 10000000123456789ULL), dest);
   EXPECT_STREQ("Err", dest);


   dm_double_toprettystring(DM_DOUBLE_PACK(0, 0, 1234567890123456ULL), dest);
   EXPECT_STREQ("1.234567890123456", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 0, 1234567890123456ULL), dest);
   EXPECT_STREQ("-1.234567890123456", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, 1, 1234567890123456ULL), dest);
   EXPECT_STREQ("12.34567890123456", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 1, 1234567890123456ULL), dest);
   EXPECT_STREQ("-12.34567890123456", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, 14, 1234567890123456ULL), dest);
   EXPECT_STREQ("123456789012345.6", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 14, 1234567890123456ULL), dest);
   EXPECT_STREQ("-123456789012345.6", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, 15, 1234567890123456ULL), dest);
   EXPECT_STREQ("1234567890123456", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 15, 1234567890123456ULL), dest);
   EXPECT_STREQ("-1234567890123456", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, 12, 1234567890123000ULL), dest);
   EXPECT_STREQ("1234567890123", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 12, 1234567890123000ULL), dest);
   EXPECT_STREQ("-1234567890123", dest);


   dm_double_toprettystring(DM_DOUBLE_PACK(0, -7, 1234567890123456ULL), dest);
   EXPECT_STREQ("1.234567890123456e-7", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, -7, 1234567890123456ULL), dest);
   EXPECT_STREQ("-1.234567890123456e-7", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, -7, 1234567890123450ULL), dest);
   EXPECT_STREQ("1.23456789012345e-7", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, -7, 1234567890123450ULL), dest);
   EXPECT_STREQ("-1.23456789012345e-7", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, -7, 1234567890123400ULL), dest);
   EXPECT_STREQ("0.00000012345678901234", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, -7, 1234567890123400ULL), dest);
   EXPECT_STREQ("-0.00000012345678901234", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, -5, 1234567890123456ULL), dest);
   EXPECT_STREQ("0.00001234567890123456", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, -5, 1234567890123456ULL), dest);
   EXPECT_STREQ("-0.00001234567890123456", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, 0, 1000000000000000ULL), dest);
   EXPECT_STREQ("1", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 0, 1000000000000000ULL), dest);
   EXPECT_STREQ("-1", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, -10, 3000000000000000ULL), dest);
   EXPECT_STREQ("0.0000000003", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, -10, 3000000000000000ULL), dest);
   EXPECT_STREQ("-0.0000000003", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, -16, 1000000000000000ULL), dest);
   EXPECT_STREQ("1e-16", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, -16, 1000000000000000ULL), dest);
   EXPECT_STREQ("-1e-16", dest);

   dm_double_toprettystring(DM_DOUBLE_PACK(0, 20, 1000000000000000ULL), dest);
   EXPECT_STREQ("1e+20", dest);
   dm_double_toprettystring(DM_DOUBLE_PACK(1, 20, 1000000000000000ULL), dest);
   EXPECT_STREQ("-1e+20", dest);
 }

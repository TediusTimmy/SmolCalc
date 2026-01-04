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

#include "SlowFloat.h"

#include <cmath>

TEST(SlowFloatTests, testDefaultConstructor)
 {
   EXPECT_EQ(0U, SlowFloat::sfZero.significand);
   EXPECT_EQ(0, SlowFloat::sfZero.exponent);

   SlowFloat::SlowFloat* bob = new SlowFloat::SlowFloat(999999999U, 16383);
   delete bob;
   bob = new SlowFloat::SlowFloat(SlowFloat::sfZero);
   EXPECT_EQ(0U, bob->significand);
   EXPECT_EQ(0, bob->exponent);
   delete bob;
 }

namespace SlowFloat
 {
   bool isInf (const SlowFloat& arg);
   bool isZero (const SlowFloat& arg);
   bool decideRound (bool sign, bool even, int64_t comp, bool zero);
 }

TEST(SlowFloatTests, testRounding) // Is this OVERKILL? Maybe.
 {
   SlowFloat::mode = SlowFloat::ROUND_TIES_EVEN;

   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, false));


   SlowFloat::mode = SlowFloat::ROUND_TIES_AWAY;

   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, false));


   SlowFloat::mode = SlowFloat::ROUND_POSITIVE_INFINITY;

   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, -1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, -1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, false));


   SlowFloat::mode = SlowFloat::ROUND_NEGATIVE_INFINITY;

   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, -1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, -1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, -1, false));


   SlowFloat::mode = SlowFloat::ROUND_ZERO;

   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, -1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, -1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, -1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, -1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, -1, false));


   SlowFloat::mode = SlowFloat::ROUND_TIES_ODD;

   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, false));


   SlowFloat::mode = SlowFloat::ROUND_TIES_ZERO;

   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, true));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, false));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, false));


   SlowFloat::mode = SlowFloat::ROUND_AWAY;

   EXPECT_FALSE(SlowFloat::decideRound(true, true, 1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, 1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, 0, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, 0, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, true, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(true, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(true, false, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, true, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, true, -1, false));
   EXPECT_FALSE(SlowFloat::decideRound(false, false, -1, true));
   EXPECT_TRUE(SlowFloat::decideRound(false, false, -1, false));


   SlowFloat::mode = SlowFloat::ROUND_TIES_EVEN;
 }

TEST(SlowFloatTests, testConversions)
 {
   EXPECT_EQ(0U, SlowFloat::SlowFloat(0.0).significand);
   EXPECT_EQ(0,  SlowFloat::SlowFloat(0.0).exponent);

   EXPECT_EQ(0xFFFFFFFFU, SlowFloat::SlowFloat(-0.0).significand);
   EXPECT_EQ(0,           SlowFloat::SlowFloat(-0.0).exponent);

   EXPECT_EQ(100000000U, SlowFloat::SlowFloat(1.0).significand);
   EXPECT_EQ(0,          SlowFloat::SlowFloat(1.0).exponent);

   EXPECT_EQ(~100000000U, SlowFloat::SlowFloat(-1.0).significand);
   EXPECT_EQ(0,           SlowFloat::SlowFloat(-1.0).exponent);

   EXPECT_EQ(0U,     SlowFloat::SlowFloat(std::pow(10.0, 10000.0)).significand);
   EXPECT_EQ(-32768, SlowFloat::SlowFloat(std::pow(10.0, 10000.0)).exponent);

   EXPECT_EQ(0xFFFFFFFFU, SlowFloat::SlowFloat(-std::pow(10.0, 10000.0)).significand);
   EXPECT_EQ(-32768,      SlowFloat::SlowFloat(-std::pow(10.0, 10000.0)).exponent);

   if (std::signbit(std::asin(2.0)))
    {
      EXPECT_EQ(~255U,   SlowFloat::SlowFloat(std::asin(2.0)).significand);
      EXPECT_EQ(-32768, SlowFloat::SlowFloat(std::asin(2.0)).exponent);

      EXPECT_EQ(255U,  SlowFloat::SlowFloat(-std::asin(2.0)).significand);
      EXPECT_EQ(-32768, SlowFloat::SlowFloat(-std::asin(2.0)).exponent);
    }
   else
    {
      EXPECT_EQ(255U,   SlowFloat::SlowFloat(std::asin(2.0)).significand);
      EXPECT_EQ(-32768, SlowFloat::SlowFloat(std::asin(2.0)).exponent);

      EXPECT_EQ(~255U,  SlowFloat::SlowFloat(-std::asin(2.0)).significand);
      EXPECT_EQ(-32768, SlowFloat::SlowFloat(-std::asin(2.0)).exponent);
    }

   EXPECT_EQ(100000000U, SlowFloat::SlowFloat(9.999999999).significand);
   EXPECT_EQ(1,          SlowFloat::SlowFloat(9.999999999).exponent);

   EXPECT_EQ(~100000000U, SlowFloat::SlowFloat(-9.999999999).significand);
   EXPECT_EQ(1,           SlowFloat::SlowFloat(-9.999999999).exponent);

   EXPECT_EQ(999999999U, SlowFloat::SlowFloat(9.999999991).significand);
   EXPECT_EQ(0,          SlowFloat::SlowFloat(9.999999991).exponent);

   EXPECT_EQ(~999999999U, SlowFloat::SlowFloat(-9.999999991).significand);
   EXPECT_EQ(0,           SlowFloat::SlowFloat(-9.999999991).exponent);

   EXPECT_EQ(999999999U, SlowFloat::SlowFloat(9.999999989).significand);
   EXPECT_EQ(0,          SlowFloat::SlowFloat(9.999999989).exponent);

   EXPECT_EQ(~999999999U, SlowFloat::SlowFloat(-9.999999989).significand);
   EXPECT_EQ(0,           SlowFloat::SlowFloat(-9.999999989).exponent);

   EXPECT_EQ(500000000U, SlowFloat::SlowFloat(0.5).significand);
   EXPECT_EQ(-1,         SlowFloat::SlowFloat(0.5).exponent);

   EXPECT_EQ(~500000000U, SlowFloat::SlowFloat(-0.5).significand);
   EXPECT_EQ(-1,          SlowFloat::SlowFloat(-0.5).exponent);


   EXPECT_EQ(100000000U, SlowFloat::SlowFloat(1.000000005).significand);
   EXPECT_EQ(100000001U, SlowFloat::SlowFloat(1.000000015).significand); // Doesn't round to 2 because actually 49999...
   EXPECT_EQ(100000002U, SlowFloat::SlowFloat(1.000000018).significand);
   EXPECT_EQ(100000001U, SlowFloat::SlowFloat(1.000000010).significand);
   EXPECT_EQ(100000002U, SlowFloat::SlowFloat(100000002.5).significand);


   EXPECT_EQ(0.0, static_cast<double>(SlowFloat::SlowFloat(0U, 0)));
   EXPECT_EQ(-0.0, static_cast<double>(SlowFloat::SlowFloat(~0U, 0)));

   EXPECT_EQ(1.0, static_cast<double>(SlowFloat::SlowFloat(100000000U, 0)));
   EXPECT_EQ(-1.0, static_cast<double>(SlowFloat::SlowFloat(~100000000U, 0)));

   EXPECT_EQ(10000000000.0, static_cast<double>(SlowFloat::SlowFloat(100000000U, 10)));
   EXPECT_EQ(-10000000000.0, static_cast<double>(SlowFloat::SlowFloat(~100000000U, 10)));

   EXPECT_EQ(0.5, static_cast<double>(SlowFloat::SlowFloat(500000000U, -1))); // DERP!
   EXPECT_EQ(-0.5, static_cast<double>(SlowFloat::SlowFloat(~500000000U, -1)));

   EXPECT_TRUE(std::isinf(static_cast<double>(SlowFloat::SlowFloat(0U, -32768))));
   EXPECT_FALSE(std::signbit(static_cast<double>(SlowFloat::SlowFloat(0U, -32768))));

   EXPECT_TRUE(std::isinf(static_cast<double>(SlowFloat::SlowFloat(0xFFFFFFFFU, -32768))));
   EXPECT_TRUE(std::signbit(static_cast<double>(SlowFloat::SlowFloat(0xFFFFFFFFU, -32768))));

   EXPECT_TRUE(std::isnan(static_cast<double>(SlowFloat::SlowFloat(1U, -32768))));
   EXPECT_FALSE(std::signbit(static_cast<double>(SlowFloat::SlowFloat(1U, -32768))));

   EXPECT_TRUE(std::isnan(static_cast<double>(SlowFloat::SlowFloat(~1U, -32768))));
   EXPECT_TRUE(std::signbit(static_cast<double>(SlowFloat::SlowFloat(~1U, -32768))));

   EXPECT_FALSE(SlowFloat::isInf(SlowFloat::SlowFloat(1U, -32768)));
   EXPECT_FALSE(SlowFloat::isZero(SlowFloat::SlowFloat(1U, -32768)));

   SlowFloat::mode = SlowFloat::ROUND_AWAY;
   EXPECT_EQ(200000000U, SlowFloat::SlowFloat(2.0).significand);
   SlowFloat::mode = SlowFloat::ROUND_POSITIVE_INFINITY;
   EXPECT_EQ(200000000U, SlowFloat::SlowFloat(2.0).significand);
   SlowFloat::mode = SlowFloat::ROUND_TIES_EVEN;

   EXPECT_EQ(-20, SlowFloat::SlowFloat(1e-20).exponent);
   EXPECT_EQ(-19, SlowFloat::SlowFloat(1e-19).exponent);
   EXPECT_EQ(-18, SlowFloat::SlowFloat(1e-18).exponent);
   EXPECT_EQ(-17, SlowFloat::SlowFloat(1e-17).exponent);
   EXPECT_EQ(-16, SlowFloat::SlowFloat(1e-16).exponent);
   EXPECT_EQ(-15, SlowFloat::SlowFloat(1e-15).exponent);
   EXPECT_EQ(-14, SlowFloat::SlowFloat(1e-14).exponent);
   EXPECT_EQ(-13, SlowFloat::SlowFloat(1e-13).exponent);
   EXPECT_EQ(-12, SlowFloat::SlowFloat(1e-12).exponent);
   EXPECT_EQ(-11, SlowFloat::SlowFloat(1e-11).exponent);
   EXPECT_EQ(-10, SlowFloat::SlowFloat(1e-10).exponent);
   EXPECT_EQ( -9, SlowFloat::SlowFloat( 1e-9).exponent);
   EXPECT_EQ( -8, SlowFloat::SlowFloat( 1e-8).exponent);
   EXPECT_EQ( -7, SlowFloat::SlowFloat( 1e-7).exponent);
   EXPECT_EQ( -6, SlowFloat::SlowFloat( 1e-6).exponent);
   EXPECT_EQ( -5, SlowFloat::SlowFloat( 1e-5).exponent);
   EXPECT_EQ( -4, SlowFloat::SlowFloat( 1e-4).exponent);
   EXPECT_EQ( -3, SlowFloat::SlowFloat( 1e-3).exponent);
   EXPECT_EQ( -2, SlowFloat::SlowFloat( 1e-2).exponent);
   EXPECT_EQ( -1, SlowFloat::SlowFloat( 1e-1).exponent);
   EXPECT_EQ(  0, SlowFloat::SlowFloat(  1e0).exponent);
   EXPECT_EQ(  1, SlowFloat::SlowFloat(  1e1).exponent);
   EXPECT_EQ(  2, SlowFloat::SlowFloat(  1e2).exponent);
   EXPECT_EQ(  3, SlowFloat::SlowFloat(  1e3).exponent);
   EXPECT_EQ(  4, SlowFloat::SlowFloat(  1e4).exponent);
   EXPECT_EQ(  5, SlowFloat::SlowFloat(  1e5).exponent);
   EXPECT_EQ(  6, SlowFloat::SlowFloat(  1e6).exponent);
   EXPECT_EQ(  7, SlowFloat::SlowFloat(  1e7).exponent);
   EXPECT_EQ(  8, SlowFloat::SlowFloat(  1e8).exponent);
   EXPECT_EQ(  9, SlowFloat::SlowFloat(  1e9).exponent);
   EXPECT_EQ( 10, SlowFloat::SlowFloat( 1e10).exponent);
   EXPECT_EQ( 11, SlowFloat::SlowFloat( 1e11).exponent);
   EXPECT_EQ( 12, SlowFloat::SlowFloat( 1e12).exponent);
   EXPECT_EQ( 13, SlowFloat::SlowFloat( 1e13).exponent);
   EXPECT_EQ( 14, SlowFloat::SlowFloat( 1e14).exponent);
   EXPECT_EQ( 15, SlowFloat::SlowFloat( 1e15).exponent);
   EXPECT_EQ( 16, SlowFloat::SlowFloat( 1e16).exponent);
   EXPECT_EQ( 17, SlowFloat::SlowFloat( 1e17).exponent);
   EXPECT_EQ( 18, SlowFloat::SlowFloat( 1e18).exponent);
   EXPECT_EQ( 19, SlowFloat::SlowFloat( 1e19).exponent);
   EXPECT_EQ( 20, SlowFloat::SlowFloat( 1e20).exponent);
 }

TEST(SlowFloatTests, testComparisons)
 {
   SlowFloat::SlowFloat positiveZero (0U, 0);
   SlowFloat::SlowFloat negativeZero (~0U, 0);
   SlowFloat::SlowFloat positiveOne  (100000000U, 0);
   SlowFloat::SlowFloat negativeOne  (~100000000U, 0);
   SlowFloat::SlowFloat positiveTwo  (200000000U, 0);
   SlowFloat::SlowFloat negativeTwo  (~200000000U, 0);
   SlowFloat::SlowFloat positiveTen  (100000000U, 1);
   SlowFloat::SlowFloat negativeTen  (~100000000U, 1);
   SlowFloat::SlowFloat positiveInf  (0U, -32768);
   SlowFloat::SlowFloat negativeInf  (~0U, -32768);
   SlowFloat::SlowFloat nan          (1U, -32768);

   EXPECT_TRUE(positiveZero == positiveZero);
   EXPECT_TRUE(positiveZero == negativeZero);
   EXPECT_FALSE(positiveZero == positiveOne);
   EXPECT_FALSE(positiveZero == negativeOne);
   EXPECT_FALSE(positiveZero == positiveTwo);
   EXPECT_FALSE(positiveZero == negativeTwo);
   EXPECT_FALSE(positiveZero == positiveTen);
   EXPECT_FALSE(positiveZero == negativeTen);
   EXPECT_FALSE(positiveZero == positiveInf);
   EXPECT_FALSE(positiveZero == negativeInf);
   EXPECT_FALSE(positiveZero == nan);

   EXPECT_TRUE(negativeZero == positiveZero);
   EXPECT_TRUE(negativeZero == negativeZero);
   EXPECT_FALSE(negativeZero == positiveOne);
   EXPECT_FALSE(negativeZero == negativeOne);
   EXPECT_FALSE(negativeZero == positiveTwo);
   EXPECT_FALSE(negativeZero == negativeTwo);
   EXPECT_FALSE(negativeZero == positiveTen);
   EXPECT_FALSE(negativeZero == negativeTen);
   EXPECT_FALSE(negativeZero == positiveInf);
   EXPECT_FALSE(negativeZero == negativeInf);
   EXPECT_FALSE(negativeZero == nan);

   EXPECT_FALSE(positiveOne == positiveZero);
   EXPECT_FALSE(positiveOne == negativeZero);
   EXPECT_TRUE(positiveOne == positiveOne);
   EXPECT_FALSE(positiveOne == negativeOne);
   EXPECT_FALSE(positiveOne == positiveTwo);
   EXPECT_FALSE(positiveOne == negativeTwo);
   EXPECT_FALSE(positiveOne == positiveTen);
   EXPECT_FALSE(positiveOne == negativeTen);
   EXPECT_FALSE(positiveOne == positiveInf);
   EXPECT_FALSE(positiveOne == negativeInf);
   EXPECT_FALSE(positiveOne == nan);

   EXPECT_FALSE(negativeOne == positiveZero);
   EXPECT_FALSE(negativeOne == negativeZero);
   EXPECT_FALSE(negativeOne == positiveOne);
   EXPECT_TRUE(negativeOne == negativeOne);
   EXPECT_FALSE(negativeOne == positiveTwo);
   EXPECT_FALSE(negativeOne == negativeTwo);
   EXPECT_FALSE(negativeOne == positiveTen);
   EXPECT_FALSE(negativeOne == negativeTen);
   EXPECT_FALSE(negativeOne == positiveInf);
   EXPECT_FALSE(negativeOne == negativeInf);
   EXPECT_FALSE(negativeOne == nan);

   EXPECT_FALSE(positiveTwo == positiveZero);
   EXPECT_FALSE(positiveTwo == negativeZero);
   EXPECT_FALSE(positiveTwo == positiveOne);
   EXPECT_FALSE(positiveTwo == negativeOne);
   EXPECT_TRUE(positiveTwo == positiveTwo);
   EXPECT_FALSE(positiveTwo == negativeTwo);
   EXPECT_FALSE(positiveTwo == positiveTen);
   EXPECT_FALSE(positiveTwo == negativeTen);
   EXPECT_FALSE(positiveTwo == positiveInf);
   EXPECT_FALSE(positiveTwo == negativeInf);
   EXPECT_FALSE(positiveTwo == nan);

   EXPECT_FALSE(negativeTwo == positiveZero);
   EXPECT_FALSE(negativeTwo == negativeZero);
   EXPECT_FALSE(negativeTwo == positiveOne);
   EXPECT_FALSE(negativeTwo == negativeOne);
   EXPECT_FALSE(negativeTwo == positiveTwo);
   EXPECT_TRUE(negativeTwo == negativeTwo);
   EXPECT_FALSE(negativeTwo == positiveTen);
   EXPECT_FALSE(negativeTwo == negativeTen);
   EXPECT_FALSE(negativeTwo == positiveInf);
   EXPECT_FALSE(negativeTwo == negativeInf);
   EXPECT_FALSE(negativeTwo == nan);

   EXPECT_FALSE(positiveTen == positiveZero);
   EXPECT_FALSE(positiveTen == negativeZero);
   EXPECT_FALSE(positiveTen == positiveOne);
   EXPECT_FALSE(positiveTen == negativeOne);
   EXPECT_FALSE(positiveTen == positiveTwo);
   EXPECT_FALSE(positiveTen == negativeTwo);
   EXPECT_TRUE(positiveTen == positiveTen);
   EXPECT_FALSE(positiveTen == negativeTen);
   EXPECT_FALSE(positiveTen == positiveInf);
   EXPECT_FALSE(positiveTen == negativeInf);
   EXPECT_FALSE(positiveTen == nan);

   EXPECT_FALSE(negativeTen == positiveZero);
   EXPECT_FALSE(negativeTen == negativeZero);
   EXPECT_FALSE(negativeTen == positiveOne);
   EXPECT_FALSE(negativeTen == negativeOne);
   EXPECT_FALSE(negativeTen == positiveTwo);
   EXPECT_FALSE(negativeTen == negativeTwo);
   EXPECT_FALSE(negativeTen == positiveTen);
   EXPECT_TRUE(negativeTen == negativeTen);
   EXPECT_FALSE(negativeTen == positiveInf);
   EXPECT_FALSE(negativeTen == negativeInf);
   EXPECT_FALSE(negativeTen == nan);

   EXPECT_FALSE(positiveInf == positiveZero);
   EXPECT_FALSE(positiveInf == negativeZero);
   EXPECT_FALSE(positiveInf == positiveOne);
   EXPECT_FALSE(positiveInf == negativeOne);
   EXPECT_FALSE(positiveInf == positiveTwo);
   EXPECT_FALSE(positiveInf == negativeTwo);
   EXPECT_FALSE(positiveInf == positiveTen);
   EXPECT_FALSE(positiveInf == negativeTen);
   EXPECT_TRUE(positiveInf == positiveInf);
   EXPECT_FALSE(positiveInf == negativeInf);
   EXPECT_FALSE(positiveInf == nan);

   EXPECT_FALSE(negativeInf == positiveZero);
   EXPECT_FALSE(negativeInf == negativeZero);
   EXPECT_FALSE(negativeInf == positiveOne);
   EXPECT_FALSE(negativeInf == negativeOne);
   EXPECT_FALSE(negativeInf == positiveTwo);
   EXPECT_FALSE(negativeInf == negativeTwo);
   EXPECT_FALSE(negativeInf == positiveTen);
   EXPECT_FALSE(negativeInf == negativeTen);
   EXPECT_FALSE(negativeInf == positiveInf);
   EXPECT_TRUE(negativeInf == negativeInf);
   EXPECT_FALSE(negativeInf == nan);

   EXPECT_FALSE(nan == positiveZero);
   EXPECT_FALSE(nan == negativeZero);
   EXPECT_FALSE(nan == positiveOne);
   EXPECT_FALSE(nan == negativeOne);
   EXPECT_FALSE(nan == positiveTwo);
   EXPECT_FALSE(nan == negativeTwo);
   EXPECT_FALSE(nan == positiveTen);
   EXPECT_FALSE(nan == negativeTen);
   EXPECT_FALSE(nan == positiveInf);
   EXPECT_FALSE(nan == negativeInf);
   EXPECT_FALSE(nan == nan);


   EXPECT_FALSE(positiveZero != positiveZero);
   EXPECT_FALSE(positiveZero != negativeZero);
   EXPECT_TRUE(positiveZero != positiveOne);
   EXPECT_TRUE(positiveZero != negativeOne);
   EXPECT_TRUE(positiveZero != positiveTwo);
   EXPECT_TRUE(positiveZero != negativeTwo);
   EXPECT_TRUE(positiveZero != positiveTen);
   EXPECT_TRUE(positiveZero != negativeTen);
   EXPECT_TRUE(positiveZero != positiveInf);
   EXPECT_TRUE(positiveZero != negativeInf);
   EXPECT_TRUE(positiveZero != nan);

   EXPECT_FALSE(negativeZero != positiveZero);
   EXPECT_FALSE(negativeZero != negativeZero);
   EXPECT_TRUE(negativeZero != positiveOne);
   EXPECT_TRUE(negativeZero != negativeOne);
   EXPECT_TRUE(negativeZero != positiveTwo);
   EXPECT_TRUE(negativeZero != negativeTwo);
   EXPECT_TRUE(negativeZero != positiveTen);
   EXPECT_TRUE(negativeZero != negativeTen);
   EXPECT_TRUE(negativeZero != positiveInf);
   EXPECT_TRUE(negativeZero != negativeInf);
   EXPECT_TRUE(negativeZero != nan);

   EXPECT_TRUE(positiveOne != positiveZero);
   EXPECT_TRUE(positiveOne != negativeZero);
   EXPECT_FALSE(positiveOne != positiveOne);
   EXPECT_TRUE(positiveOne != negativeOne);
   EXPECT_TRUE(positiveOne != positiveTwo);
   EXPECT_TRUE(positiveOne != negativeTwo);
   EXPECT_TRUE(positiveOne != positiveTen);
   EXPECT_TRUE(positiveOne != negativeTen);
   EXPECT_TRUE(positiveOne != positiveInf);
   EXPECT_TRUE(positiveOne != negativeInf);
   EXPECT_TRUE(positiveOne != nan);

   EXPECT_TRUE(negativeOne != positiveZero);
   EXPECT_TRUE(negativeOne != negativeZero);
   EXPECT_TRUE(negativeOne != positiveOne);
   EXPECT_FALSE(negativeOne != negativeOne);
   EXPECT_TRUE(negativeOne != positiveTwo);
   EXPECT_TRUE(negativeOne != negativeTwo);
   EXPECT_TRUE(negativeOne != positiveTen);
   EXPECT_TRUE(negativeOne != negativeTen);
   EXPECT_TRUE(negativeOne != positiveInf);
   EXPECT_TRUE(negativeOne != negativeInf);
   EXPECT_TRUE(negativeOne != nan);

   EXPECT_TRUE(positiveTwo != positiveZero);
   EXPECT_TRUE(positiveTwo != negativeZero);
   EXPECT_TRUE(positiveTwo != positiveOne);
   EXPECT_TRUE(positiveTwo != negativeOne);
   EXPECT_FALSE(positiveTwo != positiveTwo);
   EXPECT_TRUE(positiveTwo != negativeTwo);
   EXPECT_TRUE(positiveTwo != positiveTen);
   EXPECT_TRUE(positiveTwo != negativeTen);
   EXPECT_TRUE(positiveTwo != positiveInf);
   EXPECT_TRUE(positiveTwo != negativeInf);
   EXPECT_TRUE(positiveTwo != nan);

   EXPECT_TRUE(negativeTwo != positiveZero);
   EXPECT_TRUE(negativeTwo != negativeZero);
   EXPECT_TRUE(negativeTwo != positiveOne);
   EXPECT_TRUE(negativeTwo != negativeOne);
   EXPECT_TRUE(negativeTwo != positiveTwo);
   EXPECT_FALSE(negativeTwo != negativeTwo);
   EXPECT_TRUE(negativeTwo != positiveTen);
   EXPECT_TRUE(negativeTwo != negativeTen);
   EXPECT_TRUE(negativeTwo != positiveInf);
   EXPECT_TRUE(negativeTwo != negativeInf);
   EXPECT_TRUE(negativeTwo != nan);

   EXPECT_TRUE(positiveTen != positiveZero);
   EXPECT_TRUE(positiveTen != negativeZero);
   EXPECT_TRUE(positiveTen != positiveOne);
   EXPECT_TRUE(positiveTen != negativeOne);
   EXPECT_TRUE(positiveTen != positiveTwo);
   EXPECT_TRUE(positiveTen != negativeTwo);
   EXPECT_FALSE(positiveTen != positiveTen);
   EXPECT_TRUE(positiveTen != negativeTen);
   EXPECT_TRUE(positiveTen != positiveInf);
   EXPECT_TRUE(positiveTen != negativeInf);
   EXPECT_TRUE(positiveTen != nan);

   EXPECT_TRUE(negativeTen != positiveZero);
   EXPECT_TRUE(negativeTen != negativeZero);
   EXPECT_TRUE(negativeTen != positiveOne);
   EXPECT_TRUE(negativeTen != negativeOne);
   EXPECT_TRUE(negativeTen != positiveTwo);
   EXPECT_TRUE(negativeTen != negativeTwo);
   EXPECT_TRUE(negativeTen != positiveTen);
   EXPECT_FALSE(negativeTen != negativeTen);
   EXPECT_TRUE(negativeTen != positiveInf);
   EXPECT_TRUE(negativeTen != negativeInf);
   EXPECT_TRUE(negativeTen != nan);

   EXPECT_TRUE(positiveInf != positiveZero);
   EXPECT_TRUE(positiveInf != negativeZero);
   EXPECT_TRUE(positiveInf != positiveOne);
   EXPECT_TRUE(positiveInf != negativeOne);
   EXPECT_TRUE(positiveInf != positiveTwo);
   EXPECT_TRUE(positiveInf != negativeTwo);
   EXPECT_TRUE(positiveInf != positiveTen);
   EXPECT_TRUE(positiveInf != negativeTen);
   EXPECT_FALSE(positiveInf != positiveInf);
   EXPECT_TRUE(positiveInf != negativeInf);
   EXPECT_TRUE(positiveInf != nan);

   EXPECT_TRUE(negativeInf != positiveZero);
   EXPECT_TRUE(negativeInf != negativeZero);
   EXPECT_TRUE(negativeInf != positiveOne);
   EXPECT_TRUE(negativeInf != negativeOne);
   EXPECT_TRUE(negativeInf != positiveTwo);
   EXPECT_TRUE(negativeInf != negativeTwo);
   EXPECT_TRUE(negativeInf != positiveTen);
   EXPECT_TRUE(negativeInf != negativeTen);
   EXPECT_TRUE(negativeInf != positiveInf);
   EXPECT_FALSE(negativeInf != negativeInf);
   EXPECT_TRUE(negativeInf != nan);

   EXPECT_TRUE(nan != positiveZero);
   EXPECT_TRUE(nan != negativeZero);
   EXPECT_TRUE(nan != positiveOne);
   EXPECT_TRUE(nan != negativeOne);
   EXPECT_TRUE(nan != positiveTwo);
   EXPECT_TRUE(nan != negativeTwo);
   EXPECT_TRUE(nan != positiveTen);
   EXPECT_TRUE(nan != negativeTen);
   EXPECT_TRUE(nan != positiveInf);
   EXPECT_TRUE(nan != negativeInf);
   EXPECT_TRUE(nan != nan);


   EXPECT_FALSE(positiveZero < positiveZero);
   EXPECT_FALSE(positiveZero < negativeZero);
   EXPECT_TRUE(positiveZero < positiveOne);
   EXPECT_FALSE(positiveZero < negativeOne);
   EXPECT_TRUE(positiveZero < positiveTwo);
   EXPECT_FALSE(positiveZero < negativeTwo);
   EXPECT_TRUE(positiveZero < positiveTen);
   EXPECT_FALSE(positiveZero < negativeTen);
   EXPECT_TRUE(positiveZero < positiveInf);
   EXPECT_FALSE(positiveZero < negativeInf);
   EXPECT_FALSE(positiveZero < nan);

   EXPECT_FALSE(negativeZero < positiveZero);
   EXPECT_FALSE(negativeZero < negativeZero);
   EXPECT_TRUE(negativeZero < positiveOne);
   EXPECT_FALSE(negativeZero < negativeOne);
   EXPECT_TRUE(negativeZero < positiveTwo);
   EXPECT_FALSE(negativeZero < negativeTwo);
   EXPECT_TRUE(negativeZero < positiveTen);
   EXPECT_FALSE(negativeZero < negativeTen);
   EXPECT_TRUE(negativeZero < positiveInf);
   EXPECT_FALSE(negativeZero < negativeInf);
   EXPECT_FALSE(negativeZero < nan);

   EXPECT_FALSE(positiveOne < positiveZero);
   EXPECT_FALSE(positiveOne < negativeZero);
   EXPECT_FALSE(positiveOne < positiveOne);
   EXPECT_FALSE(positiveOne < negativeOne);
   EXPECT_TRUE(positiveOne < positiveTwo);
   EXPECT_FALSE(positiveOne < negativeTwo);
   EXPECT_TRUE(positiveOne < positiveTen);
   EXPECT_FALSE(positiveOne < negativeTen);
   EXPECT_TRUE(positiveOne < positiveInf);
   EXPECT_FALSE(positiveOne < negativeInf);
   EXPECT_FALSE(positiveOne < nan);

   EXPECT_TRUE(negativeOne < positiveZero);
   EXPECT_TRUE(negativeOne < negativeZero);
   EXPECT_TRUE(negativeOne < positiveOne);
   EXPECT_FALSE(negativeOne < negativeOne);
   EXPECT_TRUE(negativeOne < positiveTwo);
   EXPECT_FALSE(negativeOne < negativeTwo);
   EXPECT_TRUE(negativeOne < positiveTen);
   EXPECT_FALSE(negativeOne < negativeTen);
   EXPECT_TRUE(negativeOne < positiveInf);
   EXPECT_FALSE(negativeOne < negativeInf);
   EXPECT_FALSE(negativeOne < nan);

   EXPECT_FALSE(positiveTwo < positiveZero);
   EXPECT_FALSE(positiveTwo < negativeZero);
   EXPECT_FALSE(positiveTwo < positiveOne);
   EXPECT_FALSE(positiveTwo < negativeOne);
   EXPECT_FALSE(positiveTwo < positiveTwo);
   EXPECT_FALSE(positiveTwo < negativeTwo);
   EXPECT_TRUE(positiveTwo < positiveTen);
   EXPECT_FALSE(positiveTwo < negativeTen);
   EXPECT_TRUE(positiveTwo < positiveInf);
   EXPECT_FALSE(positiveTwo < negativeInf);
   EXPECT_FALSE(positiveTwo < nan);

   EXPECT_TRUE(negativeTwo < positiveZero);
   EXPECT_TRUE(negativeTwo < negativeZero);
   EXPECT_TRUE(negativeTwo < positiveOne);
   EXPECT_TRUE(negativeTwo < negativeOne);
   EXPECT_TRUE(negativeTwo < positiveTwo);
   EXPECT_FALSE(negativeTwo < negativeTwo);
   EXPECT_TRUE(negativeTwo < positiveTen);
   EXPECT_FALSE(negativeTwo < negativeTen);
   EXPECT_TRUE(negativeTwo < positiveInf);
   EXPECT_FALSE(negativeTwo < negativeInf);
   EXPECT_FALSE(negativeTwo < nan);

   EXPECT_FALSE(positiveTen < positiveZero);
   EXPECT_FALSE(positiveTen < negativeZero);
   EXPECT_FALSE(positiveTen < positiveOne);
   EXPECT_FALSE(positiveTen < negativeOne);
   EXPECT_FALSE(positiveTen < positiveTwo);
   EXPECT_FALSE(positiveTen < negativeTwo);
   EXPECT_FALSE(positiveTen < positiveTen);
   EXPECT_FALSE(positiveTen < negativeTen);
   EXPECT_TRUE(positiveTen < positiveInf);
   EXPECT_FALSE(positiveTen < negativeInf);
   EXPECT_FALSE(positiveTen < nan);

   EXPECT_TRUE(negativeTen < positiveZero);
   EXPECT_TRUE(negativeTen < negativeZero);
   EXPECT_TRUE(negativeTen < positiveOne);
   EXPECT_TRUE(negativeTen < negativeOne);
   EXPECT_TRUE(negativeTen < positiveTwo);
   EXPECT_TRUE(negativeTen < negativeTwo);
   EXPECT_TRUE(negativeTen < positiveTen);
   EXPECT_FALSE(negativeTen < negativeTen);
   EXPECT_TRUE(negativeTen < positiveInf);
   EXPECT_FALSE(negativeTen < negativeInf);
   EXPECT_FALSE(negativeTen < nan);

   EXPECT_FALSE(positiveInf < positiveZero);
   EXPECT_FALSE(positiveInf < negativeZero);
   EXPECT_FALSE(positiveInf < positiveOne);
   EXPECT_FALSE(positiveInf < negativeOne);
   EXPECT_FALSE(positiveInf < positiveTwo);
   EXPECT_FALSE(positiveInf < negativeTwo);
   EXPECT_FALSE(positiveInf < positiveTen);
   EXPECT_FALSE(positiveInf < negativeTen);
   EXPECT_FALSE(positiveInf < positiveInf);
   EXPECT_FALSE(positiveInf < negativeInf);
   EXPECT_FALSE(positiveInf < nan);

   EXPECT_TRUE(negativeInf < positiveZero);
   EXPECT_TRUE(negativeInf < negativeZero);
   EXPECT_TRUE(negativeInf < positiveOne);
   EXPECT_TRUE(negativeInf < negativeOne);
   EXPECT_TRUE(negativeInf < positiveTwo);
   EXPECT_TRUE(negativeInf < negativeTwo);
   EXPECT_TRUE(negativeInf < positiveTen);
   EXPECT_TRUE(negativeInf < negativeTen);
   EXPECT_TRUE(negativeInf < positiveInf);
   EXPECT_FALSE(negativeInf < negativeInf);
   EXPECT_FALSE(negativeInf < nan);

   EXPECT_FALSE(nan < positiveZero);
   EXPECT_FALSE(nan < negativeZero);
   EXPECT_FALSE(nan < positiveOne);
   EXPECT_FALSE(nan < negativeOne);
   EXPECT_FALSE(nan < positiveTwo);
   EXPECT_FALSE(nan < negativeTwo);
   EXPECT_FALSE(nan < positiveTen);
   EXPECT_FALSE(nan < negativeTen);
   EXPECT_FALSE(nan < positiveInf);
   EXPECT_FALSE(nan < negativeInf);
   EXPECT_FALSE(nan < nan);


   EXPECT_FALSE(positiveZero > positiveZero);
   EXPECT_FALSE(positiveZero > negativeZero);
   EXPECT_FALSE(positiveZero > positiveOne);
   EXPECT_TRUE(positiveZero > negativeOne);
   EXPECT_FALSE(positiveZero > positiveTwo);
   EXPECT_TRUE(positiveZero > negativeTwo);
   EXPECT_FALSE(positiveZero > positiveTen);
   EXPECT_TRUE(positiveZero > negativeTen);
   EXPECT_FALSE(positiveZero > positiveInf);
   EXPECT_TRUE(positiveZero > negativeInf);
   EXPECT_FALSE(positiveZero > nan);

   EXPECT_FALSE(negativeZero > positiveZero);
   EXPECT_FALSE(negativeZero > negativeZero);
   EXPECT_FALSE(negativeZero > positiveOne);
   EXPECT_TRUE(negativeZero > negativeOne);
   EXPECT_FALSE(negativeZero > positiveTwo);
   EXPECT_TRUE(negativeZero > negativeTwo);
   EXPECT_FALSE(negativeZero > positiveTen);
   EXPECT_TRUE(negativeZero > negativeTen);
   EXPECT_FALSE(negativeZero > positiveInf);
   EXPECT_TRUE(negativeZero > negativeInf);
   EXPECT_FALSE(negativeZero > nan);

   EXPECT_TRUE(positiveOne > positiveZero);
   EXPECT_TRUE(positiveOne > negativeZero);
   EXPECT_FALSE(positiveOne > positiveOne);
   EXPECT_TRUE(positiveOne > negativeOne);
   EXPECT_FALSE(positiveOne > positiveTwo);
   EXPECT_TRUE(positiveOne > negativeTwo);
   EXPECT_FALSE(positiveOne > positiveTen);
   EXPECT_TRUE(positiveOne > negativeTen);
   EXPECT_FALSE(positiveOne > positiveInf);
   EXPECT_TRUE(positiveOne > negativeInf);
   EXPECT_FALSE(positiveOne > nan);

   EXPECT_FALSE(negativeOne > positiveZero);
   EXPECT_FALSE(negativeOne > negativeZero);
   EXPECT_FALSE(negativeOne > positiveOne);
   EXPECT_FALSE(negativeOne > negativeOne);
   EXPECT_FALSE(negativeOne > positiveTwo);
   EXPECT_TRUE(negativeOne > negativeTwo);
   EXPECT_FALSE(negativeOne > positiveTen);
   EXPECT_TRUE(negativeOne > negativeTen);
   EXPECT_FALSE(negativeOne > positiveInf);
   EXPECT_TRUE(negativeOne > negativeInf);
   EXPECT_FALSE(negativeOne > nan);

   EXPECT_TRUE(positiveTwo > positiveZero);
   EXPECT_TRUE(positiveTwo > negativeZero);
   EXPECT_TRUE(positiveTwo > positiveOne);
   EXPECT_TRUE(positiveTwo > negativeOne);
   EXPECT_FALSE(positiveTwo > positiveTwo);
   EXPECT_TRUE(positiveTwo > negativeTwo);
   EXPECT_FALSE(positiveTwo > positiveTen);
   EXPECT_TRUE(positiveTwo > negativeTen);
   EXPECT_FALSE(positiveTwo > positiveInf);
   EXPECT_TRUE(positiveTwo > negativeInf);
   EXPECT_FALSE(positiveTwo > nan);

   EXPECT_FALSE(negativeTwo > positiveZero);
   EXPECT_FALSE(negativeTwo > negativeZero);
   EXPECT_FALSE(negativeTwo > positiveOne);
   EXPECT_FALSE(negativeTwo > negativeOne);
   EXPECT_FALSE(negativeTwo > positiveTwo);
   EXPECT_FALSE(negativeTwo > negativeTwo);
   EXPECT_FALSE(negativeTwo > positiveTen);
   EXPECT_TRUE(negativeTwo > negativeTen);
   EXPECT_FALSE(negativeTwo > positiveInf);
   EXPECT_TRUE(negativeTwo > negativeInf);
   EXPECT_FALSE(negativeTwo > nan);

   EXPECT_TRUE(positiveTen > positiveZero);
   EXPECT_TRUE(positiveTen > negativeZero);
   EXPECT_TRUE(positiveTen > positiveOne);
   EXPECT_TRUE(positiveTen > negativeOne);
   EXPECT_TRUE(positiveTen > positiveTwo);
   EXPECT_TRUE(positiveTen > negativeTwo);
   EXPECT_FALSE(positiveTen > positiveTen);
   EXPECT_TRUE(positiveTen > negativeTen);
   EXPECT_FALSE(positiveTen > positiveInf);
   EXPECT_TRUE(positiveTen > negativeInf);
   EXPECT_FALSE(positiveTen > nan);

   EXPECT_FALSE(negativeTen > positiveZero);
   EXPECT_FALSE(negativeTen > negativeZero);
   EXPECT_FALSE(negativeTen > positiveOne);
   EXPECT_FALSE(negativeTen > negativeOne);
   EXPECT_FALSE(negativeTen > positiveTwo);
   EXPECT_FALSE(negativeTen > negativeTwo);
   EXPECT_FALSE(negativeTen > positiveTen);
   EXPECT_FALSE(negativeTen > negativeTen);
   EXPECT_FALSE(negativeTen > positiveInf);
   EXPECT_TRUE(negativeTen > negativeInf);
   EXPECT_FALSE(negativeTen > nan);

   EXPECT_TRUE(positiveInf > positiveZero);
   EXPECT_TRUE(positiveInf > negativeZero);
   EXPECT_TRUE(positiveInf > positiveOne);
   EXPECT_TRUE(positiveInf > negativeOne);
   EXPECT_TRUE(positiveInf > positiveTwo);
   EXPECT_TRUE(positiveInf > negativeTwo);
   EXPECT_TRUE(positiveInf > positiveTen);
   EXPECT_TRUE(positiveInf > negativeTen);
   EXPECT_FALSE(positiveInf > positiveInf);
   EXPECT_TRUE(positiveInf > negativeInf);
   EXPECT_FALSE(positiveInf > nan);

   EXPECT_FALSE(negativeInf > positiveZero);
   EXPECT_FALSE(negativeInf > negativeZero);
   EXPECT_FALSE(negativeInf > positiveOne);
   EXPECT_FALSE(negativeInf > negativeOne);
   EXPECT_FALSE(negativeInf > positiveTwo);
   EXPECT_FALSE(negativeInf > negativeTwo);
   EXPECT_FALSE(negativeInf > positiveTen);
   EXPECT_FALSE(negativeInf > negativeTen);
   EXPECT_FALSE(negativeInf > positiveInf);
   EXPECT_FALSE(negativeInf > negativeInf);
   EXPECT_FALSE(negativeInf > nan);

   EXPECT_FALSE(nan > positiveZero);
   EXPECT_FALSE(nan > negativeZero);
   EXPECT_FALSE(nan > positiveOne);
   EXPECT_FALSE(nan > negativeOne);
   EXPECT_FALSE(nan > positiveTwo);
   EXPECT_FALSE(nan > negativeTwo);
   EXPECT_FALSE(nan > positiveTen);
   EXPECT_FALSE(nan > negativeTen);
   EXPECT_FALSE(nan > positiveInf);
   EXPECT_FALSE(nan > negativeInf);
   EXPECT_FALSE(nan > nan);


   EXPECT_TRUE(positiveZero <= positiveZero);
   EXPECT_TRUE(positiveZero <= negativeZero);
   EXPECT_TRUE(positiveZero <= positiveOne);
   EXPECT_FALSE(positiveZero <= negativeOne);
   EXPECT_TRUE(positiveZero <= positiveTwo);
   EXPECT_FALSE(positiveZero <= negativeTwo);
   EXPECT_TRUE(positiveZero <= positiveTen);
   EXPECT_FALSE(positiveZero <= negativeTen);
   EXPECT_TRUE(positiveZero <= positiveInf);
   EXPECT_FALSE(positiveZero <= negativeInf);
   EXPECT_FALSE(positiveZero <= nan);

   EXPECT_TRUE(negativeZero <= positiveZero);
   EXPECT_TRUE(negativeZero <= negativeZero);
   EXPECT_TRUE(negativeZero <= positiveOne);
   EXPECT_FALSE(negativeZero <= negativeOne);
   EXPECT_TRUE(negativeZero <= positiveTwo);
   EXPECT_FALSE(negativeZero <= negativeTwo);
   EXPECT_TRUE(negativeZero <= positiveTen);
   EXPECT_FALSE(negativeZero <= negativeTen);
   EXPECT_TRUE(negativeZero <= positiveInf);
   EXPECT_FALSE(negativeZero <= negativeInf);
   EXPECT_FALSE(negativeZero <= nan);

   EXPECT_FALSE(positiveOne <= positiveZero);
   EXPECT_FALSE(positiveOne <= negativeZero);
   EXPECT_TRUE(positiveOne <= positiveOne);
   EXPECT_FALSE(positiveOne <= negativeOne);
   EXPECT_TRUE(positiveOne <= positiveTwo);
   EXPECT_FALSE(positiveOne <= negativeTwo);
   EXPECT_TRUE(positiveOne <= positiveTen);
   EXPECT_FALSE(positiveOne <= negativeTen);
   EXPECT_TRUE(positiveOne <= positiveInf);
   EXPECT_FALSE(positiveOne <= negativeInf);
   EXPECT_FALSE(positiveOne <= nan);

   EXPECT_TRUE(negativeOne <= positiveZero);
   EXPECT_TRUE(negativeOne <= negativeZero);
   EXPECT_TRUE(negativeOne <= positiveOne);
   EXPECT_TRUE(negativeOne <= negativeOne);
   EXPECT_TRUE(negativeOne <= positiveTwo);
   EXPECT_FALSE(negativeOne <= negativeTwo);
   EXPECT_TRUE(negativeOne <= positiveTen);
   EXPECT_FALSE(negativeOne <= negativeTen);
   EXPECT_TRUE(negativeOne <= positiveInf);
   EXPECT_FALSE(negativeOne <= negativeInf);
   EXPECT_FALSE(negativeOne <= nan);

   EXPECT_FALSE(positiveTwo <= positiveZero);
   EXPECT_FALSE(positiveTwo <= negativeZero);
   EXPECT_FALSE(positiveTwo <= positiveOne);
   EXPECT_FALSE(positiveTwo <= negativeOne);
   EXPECT_TRUE(positiveTwo <= positiveTwo);
   EXPECT_FALSE(positiveTwo <= negativeTwo);
   EXPECT_TRUE(positiveTwo <= positiveTen);
   EXPECT_FALSE(positiveTwo <= negativeTen);
   EXPECT_TRUE(positiveTwo <= positiveInf);
   EXPECT_FALSE(positiveTwo <= negativeInf);
   EXPECT_FALSE(positiveTwo <= nan);

   EXPECT_TRUE(negativeTwo <= positiveZero);
   EXPECT_TRUE(negativeTwo <= negativeZero);
   EXPECT_TRUE(negativeTwo <= positiveOne);
   EXPECT_TRUE(negativeTwo <= negativeOne);
   EXPECT_TRUE(negativeTwo <= positiveTwo);
   EXPECT_TRUE(negativeTwo <= negativeTwo);
   EXPECT_TRUE(negativeTwo <= positiveTen);
   EXPECT_FALSE(negativeTwo <= negativeTen);
   EXPECT_TRUE(negativeTwo <= positiveInf);
   EXPECT_FALSE(negativeTwo <= negativeInf);
   EXPECT_FALSE(negativeTwo <= nan);

   EXPECT_FALSE(positiveTen <= positiveZero);
   EXPECT_FALSE(positiveTen <= negativeZero);
   EXPECT_FALSE(positiveTen <= positiveOne);
   EXPECT_FALSE(positiveTen <= negativeOne);
   EXPECT_FALSE(positiveTen <= positiveTwo);
   EXPECT_FALSE(positiveTen <= negativeTwo);
   EXPECT_TRUE(positiveTen <= positiveTen);
   EXPECT_FALSE(positiveTen <= negativeTen);
   EXPECT_TRUE(positiveTen <= positiveInf);
   EXPECT_FALSE(positiveTen <= negativeInf);
   EXPECT_FALSE(positiveTen <= nan);

   EXPECT_TRUE(negativeTen <= positiveZero);
   EXPECT_TRUE(negativeTen <= negativeZero);
   EXPECT_TRUE(negativeTen <= positiveOne);
   EXPECT_TRUE(negativeTen <= negativeOne);
   EXPECT_TRUE(negativeTen <= positiveTwo);
   EXPECT_TRUE(negativeTen <= negativeTwo);
   EXPECT_TRUE(negativeTen <= positiveTen);
   EXPECT_TRUE(negativeTen <= negativeTen);
   EXPECT_TRUE(negativeTen <= positiveInf);
   EXPECT_FALSE(negativeTen <= negativeInf);
   EXPECT_FALSE(negativeTen <= nan);

   EXPECT_FALSE(positiveInf <= positiveZero);
   EXPECT_FALSE(positiveInf <= negativeZero);
   EXPECT_FALSE(positiveInf <= positiveOne);
   EXPECT_FALSE(positiveInf <= negativeOne);
   EXPECT_FALSE(positiveInf <= positiveTwo);
   EXPECT_FALSE(positiveInf <= negativeTwo);
   EXPECT_FALSE(positiveInf <= positiveTen);
   EXPECT_FALSE(positiveInf <= negativeTen);
   EXPECT_TRUE(positiveInf <= positiveInf);
   EXPECT_FALSE(positiveInf <= negativeInf);
   EXPECT_FALSE(positiveInf <= nan);

   EXPECT_TRUE(negativeInf <= positiveZero);
   EXPECT_TRUE(negativeInf <= negativeZero);
   EXPECT_TRUE(negativeInf <= positiveOne);
   EXPECT_TRUE(negativeInf <= negativeOne);
   EXPECT_TRUE(negativeInf <= positiveTwo);
   EXPECT_TRUE(negativeInf <= negativeTwo);
   EXPECT_TRUE(negativeInf <= positiveTen);
   EXPECT_TRUE(negativeInf <= negativeTen);
   EXPECT_TRUE(negativeInf <= positiveInf);
   EXPECT_TRUE(negativeInf <= negativeInf);
   EXPECT_FALSE(negativeInf <= nan);

   EXPECT_FALSE(nan <= positiveZero);
   EXPECT_FALSE(nan <= negativeZero);
   EXPECT_FALSE(nan <= positiveOne);
   EXPECT_FALSE(nan <= negativeOne);
   EXPECT_FALSE(nan <= positiveTwo);
   EXPECT_FALSE(nan <= negativeTwo);
   EXPECT_FALSE(nan <= positiveTen);
   EXPECT_FALSE(nan <= negativeTen);
   EXPECT_FALSE(nan <= positiveInf);
   EXPECT_FALSE(nan <= negativeInf);
   EXPECT_FALSE(nan <= nan);


   EXPECT_TRUE(positiveZero >= positiveZero);
   EXPECT_TRUE(positiveZero >= negativeZero);
   EXPECT_FALSE(positiveZero >= positiveOne);
   EXPECT_TRUE(positiveZero >= negativeOne);
   EXPECT_FALSE(positiveZero >= positiveTwo);
   EXPECT_TRUE(positiveZero >= negativeTwo);
   EXPECT_FALSE(positiveZero >= positiveTen);
   EXPECT_TRUE(positiveZero >= negativeTen);
   EXPECT_FALSE(positiveZero >= positiveInf);
   EXPECT_TRUE(positiveZero >= negativeInf);
   EXPECT_FALSE(positiveZero >= nan);

   EXPECT_TRUE(negativeZero >= positiveZero);
   EXPECT_TRUE(negativeZero >= negativeZero);
   EXPECT_FALSE(negativeZero >= positiveOne);
   EXPECT_TRUE(negativeZero >= negativeOne);
   EXPECT_FALSE(negativeZero >= positiveTwo);
   EXPECT_TRUE(negativeZero >= negativeTwo);
   EXPECT_FALSE(negativeZero >= positiveTen);
   EXPECT_TRUE(negativeZero >= negativeTen);
   EXPECT_FALSE(negativeZero >= positiveInf);
   EXPECT_TRUE(negativeZero >= negativeInf);
   EXPECT_FALSE(negativeZero >= nan);

   EXPECT_TRUE(positiveOne >= positiveZero);
   EXPECT_TRUE(positiveOne >= negativeZero);
   EXPECT_TRUE(positiveOne >= positiveOne);
   EXPECT_TRUE(positiveOne >= negativeOne);
   EXPECT_FALSE(positiveOne >= positiveTwo);
   EXPECT_TRUE(positiveOne >= negativeTwo);
   EXPECT_FALSE(positiveOne >= positiveTen);
   EXPECT_TRUE(positiveOne >= negativeTen);
   EXPECT_FALSE(positiveOne >= positiveInf);
   EXPECT_TRUE(positiveOne >= negativeInf);
   EXPECT_FALSE(positiveOne >= nan);

   EXPECT_FALSE(negativeOne >= positiveZero);
   EXPECT_FALSE(negativeOne >= negativeZero);
   EXPECT_FALSE(negativeOne >= positiveOne);
   EXPECT_TRUE(negativeOne >= negativeOne);
   EXPECT_FALSE(negativeOne >= positiveTwo);
   EXPECT_TRUE(negativeOne >= negativeTwo);
   EXPECT_FALSE(negativeOne >= positiveTen);
   EXPECT_TRUE(negativeOne >= negativeTen);
   EXPECT_FALSE(negativeOne >= positiveInf);
   EXPECT_TRUE(negativeOne >= negativeInf);
   EXPECT_FALSE(negativeOne >= nan);

   EXPECT_TRUE(positiveTwo >= positiveZero);
   EXPECT_TRUE(positiveTwo >= negativeZero);
   EXPECT_TRUE(positiveTwo >= positiveOne);
   EXPECT_TRUE(positiveTwo >= negativeOne);
   EXPECT_TRUE(positiveTwo >= positiveTwo);
   EXPECT_TRUE(positiveTwo >= negativeTwo);
   EXPECT_FALSE(positiveTwo >= positiveTen);
   EXPECT_TRUE(positiveTwo >= negativeTen);
   EXPECT_FALSE(positiveTwo >= positiveInf);
   EXPECT_TRUE(positiveTwo >= negativeInf);
   EXPECT_FALSE(positiveTwo >= nan);

   EXPECT_FALSE(negativeTwo >= positiveZero);
   EXPECT_FALSE(negativeTwo >= negativeZero);
   EXPECT_FALSE(negativeTwo >= positiveOne);
   EXPECT_FALSE(negativeTwo >= negativeOne);
   EXPECT_FALSE(negativeTwo >= positiveTwo);
   EXPECT_TRUE(negativeTwo >= negativeTwo);
   EXPECT_FALSE(negativeTwo >= positiveTen);
   EXPECT_TRUE(negativeTwo >= negativeTen);
   EXPECT_FALSE(negativeTwo >= positiveInf);
   EXPECT_TRUE(negativeTwo >= negativeInf);
   EXPECT_FALSE(negativeTwo >= nan);

   EXPECT_TRUE(positiveTen >= positiveZero);
   EXPECT_TRUE(positiveTen >= negativeZero);
   EXPECT_TRUE(positiveTen >= positiveOne);
   EXPECT_TRUE(positiveTen >= negativeOne);
   EXPECT_TRUE(positiveTen >= positiveTwo);
   EXPECT_TRUE(positiveTen >= negativeTwo);
   EXPECT_TRUE(positiveTen >= positiveTen);
   EXPECT_TRUE(positiveTen >= negativeTen);
   EXPECT_FALSE(positiveTen >= positiveInf);
   EXPECT_TRUE(positiveTen >= negativeInf);
   EXPECT_FALSE(positiveTen >= nan);

   EXPECT_FALSE(negativeTen >= positiveZero);
   EXPECT_FALSE(negativeTen >= negativeZero);
   EXPECT_FALSE(negativeTen >= positiveOne);
   EXPECT_FALSE(negativeTen >= negativeOne);
   EXPECT_FALSE(negativeTen >= positiveTwo);
   EXPECT_FALSE(negativeTen >= negativeTwo);
   EXPECT_FALSE(negativeTen >= positiveTen);
   EXPECT_TRUE(negativeTen >= negativeTen);
   EXPECT_FALSE(negativeTen >= positiveInf);
   EXPECT_TRUE(negativeTen >= negativeInf);
   EXPECT_FALSE(negativeTen >= nan);

   EXPECT_TRUE(positiveInf >= positiveZero);
   EXPECT_TRUE(positiveInf >= negativeZero);
   EXPECT_TRUE(positiveInf >= positiveOne);
   EXPECT_TRUE(positiveInf >= negativeOne);
   EXPECT_TRUE(positiveInf >= positiveTwo);
   EXPECT_TRUE(positiveInf >= negativeTwo);
   EXPECT_TRUE(positiveInf >= positiveTen);
   EXPECT_TRUE(positiveInf >= negativeTen);
   EXPECT_TRUE(positiveInf >= positiveInf);
   EXPECT_TRUE(positiveInf >= negativeInf);
   EXPECT_FALSE(positiveInf >= nan);

   EXPECT_FALSE(negativeInf >= positiveZero);
   EXPECT_FALSE(negativeInf >= negativeZero);
   EXPECT_FALSE(negativeInf >= positiveOne);
   EXPECT_FALSE(negativeInf >= negativeOne);
   EXPECT_FALSE(negativeInf >= positiveTwo);
   EXPECT_FALSE(negativeInf >= negativeTwo);
   EXPECT_FALSE(negativeInf >= positiveTen);
   EXPECT_FALSE(negativeInf >= negativeTen);
   EXPECT_FALSE(negativeInf >= positiveInf);
   EXPECT_TRUE(negativeInf >= negativeInf);
   EXPECT_FALSE(negativeInf >= nan);

   EXPECT_FALSE(nan >= positiveZero);
   EXPECT_FALSE(nan >= negativeZero);
   EXPECT_FALSE(nan >= positiveOne);
   EXPECT_FALSE(nan >= negativeOne);
   EXPECT_FALSE(nan >= positiveTwo);
   EXPECT_FALSE(nan >= negativeTwo);
   EXPECT_FALSE(nan >= positiveTen);
   EXPECT_FALSE(nan >= negativeTen);
   EXPECT_FALSE(nan >= positiveInf);
   EXPECT_FALSE(nan >= negativeInf);
   EXPECT_FALSE(nan >= nan);
 }

TEST(SlowFloatTests, testAdds)
 {
   SlowFloat::SlowFloat positiveZero (0U, 0);
   SlowFloat::SlowFloat negativeZero (~0U, 0);
   SlowFloat::SlowFloat positiveInf  (0U, -32768);
   SlowFloat::SlowFloat negativeInf  (~0U, -32768);
   SlowFloat::SlowFloat nan1         (1U, -32768);
   SlowFloat::SlowFloat nan2         (2U, -32768);
   SlowFloat::SlowFloat positiveOne  (100000000U, 0);
   SlowFloat::SlowFloat negativeOne  (~100000000U, 0);
   SlowFloat::SlowFloat positiveTwo  (200000000U, 0);
   SlowFloat::SlowFloat negativeTwo  (~200000000U, 0);
   SlowFloat::SlowFloat positiveTen  (100000000U, 1);
   SlowFloat::SlowFloat negativeTen  (~100000000U, 1);
   SlowFloat::SlowFloat res          (SlowFloat::sfZero);

   res = nan1 + positiveOne;
   EXPECT_EQ(1U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne + nan1;
   EXPECT_EQ(1U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = nan2 + nan1;
   EXPECT_EQ(2U, res.significand);
   EXPECT_EQ(-32768, res.exponent);


   res = positiveInf + positiveInf;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveInf + negativeInf;
   EXPECT_EQ(255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveInf + positiveOne;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne + positiveInf;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);


   res = positiveZero + positiveZero;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = positiveZero + negativeZero;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeZero + positiveZero;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   SlowFloat::mode = SlowFloat::ROUND_NEGATIVE_INFINITY;
   res = positiveZero + negativeZero;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);
   SlowFloat::mode = SlowFloat::ROUND_TIES_EVEN;

   res = positiveZero + positiveOne;
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = positiveOne + positiveZero;
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeZero + negativeZero;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);



      // Basic cases
   res = positiveOne + positiveOne;
   EXPECT_EQ(200000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeOne + negativeOne;
   EXPECT_EQ(~200000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = positiveOne + negativeTwo;
   EXPECT_EQ(~100000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeOne + positiveTwo;
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

      // Cancel to zero
   res = positiveOne + negativeOne;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

      // Cancel to zero w/ rounding
   SlowFloat::mode = SlowFloat::ROUND_NEGATIVE_INFINITY;
   res = positiveOne + negativeOne;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);
   SlowFloat::mode = SlowFloat::ROUND_TIES_EVEN;

      // Handles offset of exponents correctly
   res = positiveOne + positiveTen;
   EXPECT_EQ(110000000U, res.significand);
   EXPECT_EQ(1, res.exponent);

   res = positiveTen + positiveOne;
   EXPECT_EQ(110000000U, res.significand);
   EXPECT_EQ(1, res.exponent);

   res = negativeOne + negativeTen;
   EXPECT_EQ(~110000000U, res.significand);
   EXPECT_EQ(1, res.exponent);

   res = negativeTen + negativeOne;
   EXPECT_EQ(~110000000U, res.significand);
   EXPECT_EQ(1, res.exponent);

      // Result needs exponent - 1 : result in bounds path
   res = negativeOne + positiveTen;
   EXPECT_EQ(900000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

      // Result needs exponent - 1 : result > MAX_SIGNIFICAND path
   res = negativeOne + SlowFloat::SlowFloat(100000000U, 2); // 100 - 1
   EXPECT_EQ(990000000U, res.significand);
   EXPECT_EQ(1, res.exponent);

      // Result needs exponent + 1
   res = SlowFloat::SlowFloat(900000000U, 0) + SlowFloat::SlowFloat(900000000U, 0); // 9 + 9
   EXPECT_EQ(180000000U, res.significand);
   EXPECT_EQ(1, res.exponent);

      // Result needs exponent + 1
   res = SlowFloat::SlowFloat(990000000U, 1) + SlowFloat::SlowFloat(100000000U, 0); // 99 + 1
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(2, res.exponent);

      // Result needs exponent + 1
   res = SlowFloat::SlowFloat(999000000U, 1) + SlowFloat::SlowFloat(100000000U, -1); // 99.9 + .1
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(2, res.exponent);

      // Catastrophic cancellation
   res = SlowFloat::SlowFloat(200000000U, 1) + SlowFloat::SlowFloat(~190000000U, 1); // 20 - 19
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(200000000U, 1) + SlowFloat::SlowFloat(~199000000U, 1); // 20.0 - 19.9
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(-1, res.exponent);

      // Result flushes to zero
   res = SlowFloat::SlowFloat(200000000U, -32767) + SlowFloat::SlowFloat(~190000000U, -32767);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(~200000000U, -32767) + SlowFloat::SlowFloat(190000000U, -32767);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

      // Result flushes to infinity
   res = SlowFloat::SlowFloat(900000000U, 32767) + SlowFloat::SlowFloat(900000000U, 32767);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, 32767) + SlowFloat::SlowFloat(~900000000U, 32767);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

      // Rebound with rounding : pre-check
   res = SlowFloat::SlowFloat(100000000U, 0) + SlowFloat::SlowFloat(~100000000U, -8);
   EXPECT_EQ(999999990U, res.significand);
   EXPECT_EQ(-1, res.exponent);

   res = SlowFloat::SlowFloat(100000000U, 0) + SlowFloat::SlowFloat(100000000U, -9);
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(100000000U, 0) + SlowFloat::SlowFloat(~100000000U, -9);
   EXPECT_EQ(999999999U, res.significand);
   EXPECT_EQ(-1, res.exponent);

      // Rebound with rounding
   res = SlowFloat::SlowFloat(100000000U, 0) + SlowFloat::SlowFloat(~100000000U, -10);
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(~100000000U, -10) + SlowFloat::SlowFloat(100000000U, 0);
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

      // No rebound
   SlowFloat::mode = SlowFloat::ROUND_ZERO;
   res = SlowFloat::SlowFloat(100000000U, 0) + SlowFloat::SlowFloat(~100000000U, -10);
   EXPECT_EQ(999999999U, res.significand);
   EXPECT_EQ(-1, res.exponent);

   res = SlowFloat::SlowFloat(~100000000U, -10) + SlowFloat::SlowFloat(100000000U, 0);
   EXPECT_EQ(999999999U, res.significand);
   EXPECT_EQ(-1, res.exponent);
   SlowFloat::mode = SlowFloat::ROUND_TIES_EVEN;

      // Final Case : Add w/ round w/ no overflow
   res = SlowFloat::SlowFloat(100000000U, 0) + SlowFloat::SlowFloat(900000000U, -9);
   EXPECT_EQ(100000001U, res.significand);
   EXPECT_EQ(0, res.exponent);

      // Pathological case
   res = SlowFloat::SlowFloat(999999999U, 9) + SlowFloat::SlowFloat(900000000U, 0);
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(10, res.exponent);

      // Failure case : difference of one and cancellation
   res = SlowFloat::SlowFloat(100000000U, 1) + SlowFloat::SlowFloat(~990000000U, 0);
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(-1, res.exponent);
 }

TEST(SlowFloatTests, testSubs)
 {
   SlowFloat::SlowFloat nan1        (1U, -32768);
   SlowFloat::SlowFloat nan2        (2U, -32768);
   SlowFloat::SlowFloat positiveOne (100000000U, 0);
   SlowFloat::SlowFloat res         (SlowFloat::sfZero);

   res = nan1 - positiveOne;
   EXPECT_EQ(1U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne - nan1;
   EXPECT_EQ(1U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = nan2 - nan1;
   EXPECT_EQ(2U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

      // With the code in mind, any subtraction here will do.
   res = SlowFloat::SlowFloat(100000000U, 1) - SlowFloat::SlowFloat(600000000U, 0); // 10 - 6
   EXPECT_EQ(400000000U, res.significand);
   EXPECT_EQ(0, res.exponent);
 }

TEST(SlowFloatTests, testRounds)
 {
   SlowFloat::SlowFloat positiveOne  (100000000U, 0);

   EXPECT_EQ(900000000U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -1)).significand);
   EXPECT_EQ(990000000U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -2)).significand);
   EXPECT_EQ(999000000U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -3)).significand);
   EXPECT_EQ(999900000U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -4)).significand);
   EXPECT_EQ(999990000U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -5)).significand);
   EXPECT_EQ(999999000U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -6)).significand);
   EXPECT_EQ(999999900U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -7)).significand);
   EXPECT_EQ(999999990U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -8)).significand);
   EXPECT_EQ(999999999U, (positiveOne - SlowFloat::SlowFloat(100000000U,  -9)).significand);
   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(100000000U, -10)).significand);
   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(100000000U, -11)).significand);

   EXPECT_EQ(500000001U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -1)).significand);
   EXPECT_EQ(950000000U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -2)).significand);
   EXPECT_EQ(995000000U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -3)).significand);
   EXPECT_EQ(999500000U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -4)).significand);
   EXPECT_EQ(999950000U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -5)).significand);
   EXPECT_EQ(999995000U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -6)).significand);
   EXPECT_EQ(999999500U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -7)).significand);
   EXPECT_EQ(999999950U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -8)).significand);
   EXPECT_EQ(999999995U, (positiveOne - SlowFloat::SlowFloat(499999999U,  -9)).significand);
   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(499999999U, -10)).significand);
   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(499999999U, -11)).significand);

   EXPECT_EQ(500000000U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -1)).significand);
   EXPECT_EQ(950000000U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -2)).significand);
   EXPECT_EQ(995000000U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -3)).significand);
   EXPECT_EQ(999500000U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -4)).significand);
   EXPECT_EQ(999950000U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -5)).significand);
   EXPECT_EQ(999995000U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -6)).significand);
   EXPECT_EQ(999999500U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -7)).significand);
   EXPECT_EQ(999999950U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -8)).significand);
   EXPECT_EQ(999999995U, (positiveOne - SlowFloat::SlowFloat(500000000U,  -9)).significand);
   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(500000000U, -10)).significand);
   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(500000000U, -11)).significand);

   EXPECT_EQ(499999999U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -1)).significand);
   EXPECT_EQ(950000000U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -2)).significand);
   EXPECT_EQ(995000000U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -3)).significand);
   EXPECT_EQ(999500000U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -4)).significand);
   EXPECT_EQ(999950000U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -5)).significand);
   EXPECT_EQ(999995000U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -6)).significand);
   EXPECT_EQ(999999500U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -7)).significand);
   EXPECT_EQ(999999950U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -8)).significand);
   EXPECT_EQ(999999995U, (positiveOne - SlowFloat::SlowFloat(500000001U,  -9)).significand);
   EXPECT_EQ(999999999U, (positiveOne - SlowFloat::SlowFloat(500000001U, -10)).significand);
   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(500000001U, -11)).significand);

   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -1)).significand);
   EXPECT_EQ(910000000U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -2)).significand);
   EXPECT_EQ(991000000U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -3)).significand);
   EXPECT_EQ(999100000U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -4)).significand);
   EXPECT_EQ(999910000U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -5)).significand);
   EXPECT_EQ(999991000U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -6)).significand);
   EXPECT_EQ(999999100U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -7)).significand);
   EXPECT_EQ(999999910U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -8)).significand);
   EXPECT_EQ(999999991U, (positiveOne - SlowFloat::SlowFloat(900000000U,  -9)).significand);
   EXPECT_EQ(999999999U, (positiveOne - SlowFloat::SlowFloat(900000000U, -10)).significand);
   EXPECT_EQ(100000000U, (positiveOne - SlowFloat::SlowFloat(900000000U, -11)).significand);


   EXPECT_EQ(900000000U, (-SlowFloat::SlowFloat(100000000U,  -1) + positiveOne).significand);
   EXPECT_EQ(990000000U, (-SlowFloat::SlowFloat(100000000U,  -2) + positiveOne).significand);
   EXPECT_EQ(999000000U, (-SlowFloat::SlowFloat(100000000U,  -3) + positiveOne).significand);
   EXPECT_EQ(999900000U, (-SlowFloat::SlowFloat(100000000U,  -4) + positiveOne).significand);
   EXPECT_EQ(999990000U, (-SlowFloat::SlowFloat(100000000U,  -5) + positiveOne).significand);
   EXPECT_EQ(999999000U, (-SlowFloat::SlowFloat(100000000U,  -6) + positiveOne).significand);
   EXPECT_EQ(999999900U, (-SlowFloat::SlowFloat(100000000U,  -7) + positiveOne).significand);
   EXPECT_EQ(999999990U, (-SlowFloat::SlowFloat(100000000U,  -8) + positiveOne).significand);
   EXPECT_EQ(999999999U, (-SlowFloat::SlowFloat(100000000U,  -9) + positiveOne).significand);
   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(100000000U, -10) + positiveOne).significand);
   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(100000000U, -11) + positiveOne).significand);

   EXPECT_EQ(500000001U, (-SlowFloat::SlowFloat(499999999U,  -1) + positiveOne).significand);
   EXPECT_EQ(950000000U, (-SlowFloat::SlowFloat(499999999U,  -2) + positiveOne).significand);
   EXPECT_EQ(995000000U, (-SlowFloat::SlowFloat(499999999U,  -3) + positiveOne).significand);
   EXPECT_EQ(999500000U, (-SlowFloat::SlowFloat(499999999U,  -4) + positiveOne).significand);
   EXPECT_EQ(999950000U, (-SlowFloat::SlowFloat(499999999U,  -5) + positiveOne).significand);
   EXPECT_EQ(999995000U, (-SlowFloat::SlowFloat(499999999U,  -6) + positiveOne).significand);
   EXPECT_EQ(999999500U, (-SlowFloat::SlowFloat(499999999U,  -7) + positiveOne).significand);
   EXPECT_EQ(999999950U, (-SlowFloat::SlowFloat(499999999U,  -8) + positiveOne).significand);
   EXPECT_EQ(999999995U, (-SlowFloat::SlowFloat(499999999U,  -9) + positiveOne).significand);
   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(499999999U, -10) + positiveOne).significand);
   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(499999999U, -11) + positiveOne).significand);

   EXPECT_EQ(500000000U, (-SlowFloat::SlowFloat(500000000U,  -1) + positiveOne).significand);
   EXPECT_EQ(950000000U, (-SlowFloat::SlowFloat(500000000U,  -2) + positiveOne).significand);
   EXPECT_EQ(995000000U, (-SlowFloat::SlowFloat(500000000U,  -3) + positiveOne).significand);
   EXPECT_EQ(999500000U, (-SlowFloat::SlowFloat(500000000U,  -4) + positiveOne).significand);
   EXPECT_EQ(999950000U, (-SlowFloat::SlowFloat(500000000U,  -5) + positiveOne).significand);
   EXPECT_EQ(999995000U, (-SlowFloat::SlowFloat(500000000U,  -6) + positiveOne).significand);
   EXPECT_EQ(999999500U, (-SlowFloat::SlowFloat(500000000U,  -7) + positiveOne).significand);
   EXPECT_EQ(999999950U, (-SlowFloat::SlowFloat(500000000U,  -8) + positiveOne).significand);
   EXPECT_EQ(999999995U, (-SlowFloat::SlowFloat(500000000U,  -9) + positiveOne).significand);
   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(500000000U, -10) + positiveOne).significand);
   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(500000000U, -11) + positiveOne).significand);

   EXPECT_EQ(499999999U, (-SlowFloat::SlowFloat(500000001U,  -1) + positiveOne).significand);
   EXPECT_EQ(950000000U, (-SlowFloat::SlowFloat(500000001U,  -2) + positiveOne).significand);
   EXPECT_EQ(995000000U, (-SlowFloat::SlowFloat(500000001U,  -3) + positiveOne).significand);
   EXPECT_EQ(999500000U, (-SlowFloat::SlowFloat(500000001U,  -4) + positiveOne).significand);
   EXPECT_EQ(999950000U, (-SlowFloat::SlowFloat(500000001U,  -5) + positiveOne).significand);
   EXPECT_EQ(999995000U, (-SlowFloat::SlowFloat(500000001U,  -6) + positiveOne).significand);
   EXPECT_EQ(999999500U, (-SlowFloat::SlowFloat(500000001U,  -7) + positiveOne).significand);
   EXPECT_EQ(999999950U, (-SlowFloat::SlowFloat(500000001U,  -8) + positiveOne).significand);
   EXPECT_EQ(999999995U, (-SlowFloat::SlowFloat(500000001U,  -9) + positiveOne).significand);
   EXPECT_EQ(999999999U, (-SlowFloat::SlowFloat(500000001U, -10) + positiveOne).significand);
   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(500000001U, -11) + positiveOne).significand);

   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(900000000U,  -1) + positiveOne).significand);
   EXPECT_EQ(910000000U, (-SlowFloat::SlowFloat(900000000U,  -2) + positiveOne).significand);
   EXPECT_EQ(991000000U, (-SlowFloat::SlowFloat(900000000U,  -3) + positiveOne).significand);
   EXPECT_EQ(999100000U, (-SlowFloat::SlowFloat(900000000U,  -4) + positiveOne).significand);
   EXPECT_EQ(999910000U, (-SlowFloat::SlowFloat(900000000U,  -5) + positiveOne).significand);
   EXPECT_EQ(999991000U, (-SlowFloat::SlowFloat(900000000U,  -6) + positiveOne).significand);
   EXPECT_EQ(999999100U, (-SlowFloat::SlowFloat(900000000U,  -7) + positiveOne).significand);
   EXPECT_EQ(999999910U, (-SlowFloat::SlowFloat(900000000U,  -8) + positiveOne).significand);
   EXPECT_EQ(999999991U, (-SlowFloat::SlowFloat(900000000U,  -9) + positiveOne).significand);
   EXPECT_EQ(999999999U, (-SlowFloat::SlowFloat(900000000U, -10) + positiveOne).significand);
   EXPECT_EQ(100000000U, (-SlowFloat::SlowFloat(900000000U, -11) + positiveOne).significand);
 }

TEST(SlowFloatTests, testMuls)
 {
   SlowFloat::SlowFloat positiveZero (0U, 0);
   SlowFloat::SlowFloat negativeZero (~0U, 0);
   SlowFloat::SlowFloat positiveInf  (0U, -32768);
   SlowFloat::SlowFloat negativeInf  (~0U, -32768);
   SlowFloat::SlowFloat nan1         (1U, -32768);
   SlowFloat::SlowFloat nan2         (2U, -32768);
   SlowFloat::SlowFloat positiveOne  (100000000U, 0);
   SlowFloat::SlowFloat negativeOne  (~100000000U, 0);
   SlowFloat::SlowFloat res          (SlowFloat::sfZero);

      // Nans
   res = nan1 * positiveOne;
   EXPECT_EQ(1U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne * nan1;
   EXPECT_EQ(1U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = nan2 * nan1;
   EXPECT_EQ(2U, res.significand);
   EXPECT_EQ(-32768, res.exponent);


      // Inf * 0 = Nan
   res = positiveInf * positiveZero;
   EXPECT_EQ(255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveZero * positiveInf;
   EXPECT_EQ(255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = negativeInf * positiveZero;
   EXPECT_EQ(~255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveZero * negativeInf;
   EXPECT_EQ(~255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveInf * negativeZero;
   EXPECT_EQ(~255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = negativeZero * positiveInf;
   EXPECT_EQ(~255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = negativeInf * negativeZero;
   EXPECT_EQ(255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = negativeZero * negativeInf;
   EXPECT_EQ(255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);


      // Infinities
   res = positiveInf * positiveOne;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne * positiveInf;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = negativeInf * positiveOne;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne * negativeInf;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);


      // Zeros
   res = positiveZero * positiveOne;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeOne * positiveZero;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeZero * positiveOne;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeOne * negativeZero;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);


      // Overflows
   res = SlowFloat::SlowFloat(900000000U, 32767) * SlowFloat::SlowFloat(900000000U, 32767);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, 32767) * SlowFloat::SlowFloat(~900000000U, 32767);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, 32767) * SlowFloat::SlowFloat(900000000U, 32767);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(900000000U, 32767) * SlowFloat::SlowFloat(~900000000U, 32767);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

      // Underflows
   res = SlowFloat::SlowFloat(900000000U, -32767) * SlowFloat::SlowFloat(900000000U, -10);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, -32767) * SlowFloat::SlowFloat(~900000000U, -10);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, -32767) * SlowFloat::SlowFloat(900000000U, -10);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(900000000U, -32767) * SlowFloat::SlowFloat(~900000000U, -10);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);


      // Standard cases, no rounding
   res = SlowFloat::SlowFloat(200000000U, 0) * SlowFloat::SlowFloat(300000000U, 0);
   EXPECT_EQ(600000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(600000000U, 0) * SlowFloat::SlowFloat(300000000U, 0);
   EXPECT_EQ(180000000U, res.significand);
   EXPECT_EQ(1, res.exponent);

   res = SlowFloat::SlowFloat(~200000000U, 0) * SlowFloat::SlowFloat(~300000000U, 0);
   EXPECT_EQ(600000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(200000000U, 0) * SlowFloat::SlowFloat(~300000000U, 0);
   EXPECT_EQ(~600000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(999999999U, 0) * SlowFloat::SlowFloat(999999999U, 0);
   EXPECT_EQ(999999998U, res.significand);
   EXPECT_EQ(1, res.exponent);


      // Flushes to infinity
   res = SlowFloat::SlowFloat(200000000U, 16383) * SlowFloat::SlowFloat(500000000U, 16384);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(~200000000U, 16383) * SlowFloat::SlowFloat(500000000U, 16384);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

      // Flushes to zero
   res = SlowFloat::SlowFloat(200000000U, -16384) * SlowFloat::SlowFloat(300000000U, -16384);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(~200000000U, -16384) * SlowFloat::SlowFloat(300000000U, -16384);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

      // Saved from zero
   res = SlowFloat::SlowFloat(400000000U, -16384) * SlowFloat::SlowFloat(300000000U, -16384);
   EXPECT_EQ(120000000U, res.significand);
   EXPECT_EQ(-32767, res.exponent);


      // Standard cases, rounding
   res = SlowFloat::SlowFloat(200000000U, 0) * SlowFloat::SlowFloat(999999999U, 0);
   EXPECT_EQ(200000000U, res.significand);
   EXPECT_EQ(1, res.exponent);
 }

TEST(SlowFloatTests, testDivs)
 {
   SlowFloat::SlowFloat positiveZero (0U, 0);
   SlowFloat::SlowFloat negativeZero (~0U, 0);
   SlowFloat::SlowFloat positiveInf  (0U, -32768);
   SlowFloat::SlowFloat negativeInf  (~0U, -32768);
   SlowFloat::SlowFloat nan1         (1U, -32768);
   SlowFloat::SlowFloat nan2         (2U, -32768);
   SlowFloat::SlowFloat positiveOne  (100000000U, 0);
   SlowFloat::SlowFloat negativeOne  (~100000000U, 0);
   SlowFloat::SlowFloat res          (SlowFloat::sfZero);

      // Nans
   res = nan1 / positiveOne;
   EXPECT_EQ(1U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne / nan1;
   EXPECT_EQ(1U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = nan2 / nan1;
   EXPECT_EQ(2U, res.significand);
   EXPECT_EQ(-32768, res.exponent);


      // Inf / Inf = Nan, 0 / 0 = Nan
   res = positiveInf / positiveInf;
   EXPECT_EQ(255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = negativeInf / negativeInf;
   EXPECT_EQ(255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = negativeZero / positiveZero;
   EXPECT_EQ(~255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveZero / negativeZero;
   EXPECT_EQ(~255U, res.significand);
   EXPECT_EQ(-32768, res.exponent);


      // Infinities
   res = positiveInf / positiveOne;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne / positiveZero;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = negativeInf / positiveOne;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = positiveOne / negativeZero;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);


      // Zeros
   res = positiveOne / positiveInf;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = positiveZero / positiveOne;
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeOne / positiveInf;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = negativeZero / positiveOne;
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);


      // Overflows
   res = SlowFloat::SlowFloat(900000000U, 32767) / SlowFloat::SlowFloat(900000000U, -32767);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, 32767) / SlowFloat::SlowFloat(~900000000U, -32767);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, 32767) / SlowFloat::SlowFloat(900000000U, -32767);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(900000000U, 32767) / SlowFloat::SlowFloat(~900000000U, -32767);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

      // Underflows
   res = SlowFloat::SlowFloat(900000000U, -32767) / SlowFloat::SlowFloat(900000000U, 10);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, -32767) / SlowFloat::SlowFloat(~900000000U, 10);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, -32767) / SlowFloat::SlowFloat(900000000U, 10);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(900000000U, -32767) / SlowFloat::SlowFloat(~900000000U, 10);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);


      // Standard cases, no rounding
   res = SlowFloat::SlowFloat(600000000U, 0) / SlowFloat::SlowFloat(300000000U, 0);
   EXPECT_EQ(200000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(120000000U, 0) / SlowFloat::SlowFloat(300000000U, 0);
   EXPECT_EQ(400000000U, res.significand);
   EXPECT_EQ(-1, res.exponent);

   res = SlowFloat::SlowFloat(~600000000U, 0) / SlowFloat::SlowFloat(~300000000U, 0);
   EXPECT_EQ(200000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(600000000U, 0) / SlowFloat::SlowFloat(~300000000U, 0);
   EXPECT_EQ(~200000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(999999999U, 0) / SlowFloat::SlowFloat(100000000U, 0);
   EXPECT_EQ(999999999U, res.significand);
   EXPECT_EQ(0, res.exponent);


      // Flushes to infinity
   res = SlowFloat::SlowFloat(900000000U, 16384) / SlowFloat::SlowFloat(300000000U, -16384);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::SlowFloat(~900000000U, 16384) / SlowFloat::SlowFloat(300000000U, -16384);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

      // Flushes to zero
   res = SlowFloat::SlowFloat(120000000U, -16383) / SlowFloat::SlowFloat(300000000U, 16384);
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::SlowFloat(~120000000U, -16383) / SlowFloat::SlowFloat(300000000U, 16384);
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

      // Saved from infinity
   res = SlowFloat::SlowFloat(120000000U, 16384) / SlowFloat::SlowFloat(300000000U, -16384);
   EXPECT_EQ(400000000U, res.significand);
   EXPECT_EQ(32767, res.exponent);


      // Standard cases, rounding
   res = SlowFloat::SlowFloat(200000000U, 0) / SlowFloat::SlowFloat(300000000U, 0);
   EXPECT_EQ(666666667U, res.significand);
   EXPECT_EQ(-1, res.exponent);
 }

TEST(SlowFloatTests, testToStrings)
 {
   EXPECT_EQ("NaN", SlowFloat::toString(SlowFloat::SlowFloat(255U, -32768)));
   EXPECT_EQ("-NaN", SlowFloat::toString(SlowFloat::SlowFloat(~255U, -32768)));
   EXPECT_EQ("Inf", SlowFloat::toString(SlowFloat::SlowFloat(0U, -32768)));
   EXPECT_EQ("-Inf", SlowFloat::toString(SlowFloat::SlowFloat(~0U, -32768)));
   EXPECT_EQ("0", SlowFloat::toString(SlowFloat::SlowFloat(0U, 0)));
   EXPECT_EQ("-0", SlowFloat::toString(SlowFloat::SlowFloat(~0U, 0)));
   EXPECT_EQ("0", SlowFloat::toString(SlowFloat::SlowFloat(0U, 10)));
   EXPECT_EQ("-0", SlowFloat::toString(SlowFloat::SlowFloat(~0U, -12)));
   EXPECT_EQ("12.3456789", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, 1)));
   EXPECT_EQ("1.23456789", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, 0)));
   EXPECT_EQ("0.123456789", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, -1)));
   EXPECT_EQ("123456789", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, 8)));
   EXPECT_EQ("1.23456789e+9", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, 9)));
   EXPECT_EQ("1.23456789e+12", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, 12)));
   EXPECT_EQ("1.23456789e-8", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, -8)));
   EXPECT_EQ("1.23456789e-12", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, -12)));
   EXPECT_EQ("0.000000123456789", SlowFloat::toString(SlowFloat::SlowFloat(123456789U, -7)));
   EXPECT_EQ("0.0000001234", SlowFloat::toString(SlowFloat::SlowFloat(123400000U, -7)));
   EXPECT_EQ("0.0012345", SlowFloat::toString(SlowFloat::SlowFloat(123450000U, -3)));
   EXPECT_EQ("200", SlowFloat::toString(SlowFloat::SlowFloat(200000000U, 2)));
   EXPECT_EQ("1.23456", SlowFloat::toString(SlowFloat::SlowFloat(123456000U, 0)));
 }

TEST(SlowFloatTests, testFromStrings)
 {
   SlowFloat::SlowFloat res (SlowFloat::sfZero);

   res = SlowFloat::fromString("5.0");
   EXPECT_EQ(500000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1234567891");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(9, res.exponent);

   res = SlowFloat::fromString("1234567899");
   EXPECT_EQ(123456790U, res.significand);
   EXPECT_EQ(9, res.exponent);

   res = SlowFloat::fromString("1234567895");
   EXPECT_EQ(123456790U, res.significand);
   EXPECT_EQ(9, res.exponent);

   res = SlowFloat::fromString("12345678850");
   EXPECT_EQ(123456788U, res.significand);
   EXPECT_EQ(10, res.exponent);

   res = SlowFloat::fromString("123456788501");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(11, res.exponent);

   res = SlowFloat::fromString("1234567890");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(9, res.exponent);

   res = SlowFloat::fromString("12345678900");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(10, res.exponent);

   res = SlowFloat::fromString("12345678901");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(10, res.exponent);

   res = SlowFloat::fromString("12345678911");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(10, res.exponent);

   res = SlowFloat::fromString("-5.0");
   EXPECT_EQ(~500000000U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.234567891");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.234567899");
   EXPECT_EQ(123456790U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.234567895");
   EXPECT_EQ(123456790U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.2345678850");
   EXPECT_EQ(123456788U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.23456788501");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.234567890");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.2345678900");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.2345678901");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("1.2345678911");
   EXPECT_EQ(123456789U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("9.999999999");
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(1, res.exponent);

   res = SlowFloat::fromString("5e10");
   EXPECT_EQ(500000000U, res.significand);
   EXPECT_EQ(10, res.exponent);

   res = SlowFloat::fromString("5e+10");
   EXPECT_EQ(500000000U, res.significand);
   EXPECT_EQ(10, res.exponent);

   res = SlowFloat::fromString("5e-10");
   EXPECT_EQ(500000000U, res.significand);
   EXPECT_EQ(-10, res.exponent);

   res = SlowFloat::fromString("5e40000");
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::fromString("5e-40000");
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("-5e40000");
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::fromString("-5e-40000");
   EXPECT_EQ(~0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("5e900000");
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(-32768, res.exponent);

   res = SlowFloat::fromString("5e-900000");
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);

   res = SlowFloat::fromString("0.01");
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(-2, res.exponent);

   res = SlowFloat::fromString("0.001");
   EXPECT_EQ(100000000U, res.significand);
   EXPECT_EQ(-3, res.exponent);

   res = SlowFloat::fromString("0e1000000");
   EXPECT_EQ(0U, res.significand);
   EXPECT_EQ(0, res.exponent);
 }

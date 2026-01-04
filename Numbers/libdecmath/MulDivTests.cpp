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

#include "dm_muldiv.h"

TEST(MulDivLibTest, testLoadFrom)
 {
      // I generally despise tests like this as stupid: this function is OBVIOUSLY correct.
   dm_muldiv_t res;
   dm_muldiv_loadFrom(res, 10000000000000000ULL);
   EXPECT_EQ(0U, res[3]);
   EXPECT_EQ(0U, res[2]);
   EXPECT_EQ(0x2386F2U, res[1]);
   EXPECT_EQ(0x6FC10000U, res[0]);
 }

TEST(MulDivLibTest, testMulBy)
 {
   dm_muldiv_t res;
   dm_muldiv_loadFrom(res, 10000000000000000ULL);
   dm_muldiv_mulBy(res, 10000000000000000ULL);
   EXPECT_EQ(0x4EEU, res[3]);
   EXPECT_EQ(0x2D6D415BU, res[2]);
   EXPECT_EQ(0x85ACEF81U, res[1]);
   EXPECT_EQ(0U, res[0]);

   dm_muldiv_loadFrom(res, 9999999999999999ULL);
   dm_muldiv_mulBy(res, 9999999999999999ULL);
   EXPECT_EQ(0x4EEU, res[3]);
   EXPECT_EQ(0x2D6D415BU, res[2]);
   EXPECT_EQ(0x8565E19CU, res[1]);
   EXPECT_EQ(0x207E0001U, res[0]);
 }

TEST(MulDivLibTest, testDivBy)
 {
   dm_muldiv_t res;
   res[3] = 0x4EEU;
   res[2] = 0x2D6D415BU;
   res[1] = 0x8565E19CU;
   res[0] = 0x207E0001U;

   uint64_t quot, rem;
   dm_muldiv_divBy(res, 10000000000000000ULL, &quot, &rem);
   // 9999999999999998 0000000000000001
   EXPECT_EQ(9999999999999998U, quot);
   EXPECT_EQ(1U, rem);


   dm_muldiv_loadFrom(res, 10000000000000000ULL);
   dm_muldiv_mulBy(res, 1000000000000000ULL);

   dm_muldiv_divBy(res, 5555555555555555ULL, &quot, &rem);
   EXPECT_EQ(1800000000000000U, quot);
   EXPECT_EQ(1000000000000000U, rem);


   dm_muldiv_loadFrom(res, 9999999999999999ULL);
   dm_muldiv_mulBy(res, 8888888888888888ULL);

   dm_muldiv_divBy(res, 5555555555555555ULL, &quot, &rem);
   EXPECT_EQ(15999999999999998U, quot);
   EXPECT_EQ(2222222222222222U, rem);


   dm_muldiv_loadFrom(res, 10000000000000000ULL);
   dm_muldiv_mulBy(res, 0x0080000000000000ULL);

   dm_muldiv_divBy(res, 10000000000000000ULL, &quot, &rem);
   EXPECT_EQ(0x0080000000000000ULL, quot);
   EXPECT_EQ(0U, rem);
 }

TEST(MulDivLibTest, testZero)
 {
   // Obviously correct.
   dm_muldiv_t res;
   res[3] = 0U;
   res[2] = 0U;
   res[1] = 0U;
   res[0] = 0U;

   EXPECT_EQ(1, dm_muldiv_zero(res));

   res[3] = 1U;
   EXPECT_EQ(0, dm_muldiv_zero(res));

   res[3] = 0U;
   res[2] = 1U;
   EXPECT_EQ(0, dm_muldiv_zero(res));

   res[2] = 0U;
   res[1] = 1U;
   EXPECT_EQ(0, dm_muldiv_zero(res));

   res[1] = 0U;
   res[0] = 1U;
   EXPECT_EQ(0, dm_muldiv_zero(res));

   dm_muldiv_t a;
   a[3] = 0x00FF0000U;
   a[2] = 0xFFFF0000U;
   a[1] = 0xFFFF0000U;
   a[0] = 0xFFFF0000U;
   dm_muldiv_t b;
   b[3] = 0x01FF0000U;
   b[2] = 0xFFFF0000U;
   b[1] = 0xFFFF0000U;
   b[0] = 0xFFFF0000U;
   EXPECT_EQ(1, dm_muldiv_less(a, b));
 }

TEST(MulDivLibTest, testSubCarries)
 {
   dm_muldiv_t a;
   a[3] = 0x01FF0001U;
   a[2] = 0x01FF0001U;
   a[1] = 0x01FF0001U;
   a[0] = 0x01FF0000U;
   dm_muldiv_t b;
   b[3] = 0x00FF0000U;
   b[2] = 0x80FF0000U;
   b[1] = 0x80FF0000U;
   b[0] = 0x80FF0000U;

   dm_muldiv_sub(a, a, b);

   EXPECT_EQ(0x01000000U, a[3]);
   EXPECT_EQ(0x81000000U, a[2]);
   EXPECT_EQ(0x81000000U, a[1]);
   EXPECT_EQ(0x81000000U, a[0]);
 }

TEST(MulDivLibTest, testEverHappens)
 {
   uint64_t n, d, q, r;
   dm_muldiv_t a;

   for (n = 1000000000000000ULL; n < 10000000000000000ULL; n += 1000000000000000ULL)
    {
      std::cout << n << std::endl;
      for (d = 1000000000000000ULL; d < 10000000000000000ULL; d += 1000000000000ULL)
       {
         dm_muldiv_loadFrom(a, n);
         dm_muldiv_mulBy(a, 10000000000000000ULL);

         dm_muldiv_divBy(a, d, &q, &r);
         __uint128_t x, y;
         x = n;
         x *= 10000000000000000ULL;
         y = d;
         EXPECT_EQ(x / y, q);
         EXPECT_EQ(x % y, r);
       }
    }
 }

TEST(MulDivLibTest, testRange)
 {
   uint64_t n, q, r;
   dm_muldiv_t a;
   for (n = 1; n <= 10000000000000000000ULL; n *= 10)
    {
      dm_muldiv_loadFrom(a, 1000000000000000ULL);
      dm_muldiv_mulBy(a, n);
      dm_muldiv_divBy(a, n, &q, &r);
      EXPECT_EQ(1000000000000000ULL, q);
      EXPECT_EQ(0U, r);
    }
 }

TEST(MulDivLibTest, testMakeHappen)
 {
   uint64_t n, q, r;
   dm_muldiv_t a;

   n = 1193000000000000ULL;
   // 0x3f11e9bdb5b268000000000000000
   // 20467376370321685741568000000000000
   a[0] = 0U;
   a[1] = 0x80000000U;
   a[2] = 0x9BDB5B26U;  
   a[3] = 0x3F11EU;

   dm_muldiv_divBy(a, n, &q, &r);
   EXPECT_EQ(17156224954167381174ULL, q);
   EXPECT_EQ(     986000000000000ULL, r);
 }

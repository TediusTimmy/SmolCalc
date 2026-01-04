/*
BSD 3-Clause License

Copyright (c) 2023, Thomas DiModica
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

#include "Float.hpp"

   // Basically, making this legacy code no longer legacy by creating tests.
   // Run IO through its paces, so that we can use it as a root of trust for further tests.
TEST(FixedTests, testNoBadBoom)
 {
   BigInt::Float test ("1.2");
   EXPECT_EQ("1.2000000e0", test.toString());

   test.fromString("1,2");
   EXPECT_EQ("1.2000000e0", test.toString());

   test.fromString("9000");
   EXPECT_EQ("9.0000000e3", test.toString());

   test.fromString("+9000");
   EXPECT_EQ("9.0000000e3", test.toString());

   test.fromString("-9000");
   EXPECT_EQ("-9.0000000e3", test.toString());

   test.fromString(".002");
   EXPECT_EQ("2.0000000e-3", test.toString());

   test.fromString(",003");
   EXPECT_EQ("3.0000000e-3", test.toString());

   test.fromString("9000.002");
   EXPECT_EQ("9.0000020e3", test.toString());

   test.fromString("8000,003");
   EXPECT_EQ("8.0000030e3", test.toString());

      // Gross. What was decade-ago me thinking?
   test.fromString("9000A");
   EXPECT_EQ("9.0000000e4", test.toString());

   test.fromString("9000.A");
   EXPECT_EQ("9.0000000e3", test.toString());

   test.fromString("9000.");
   EXPECT_EQ("9.0000000e3", test.toString());

   test.fromString("9000.02A");
   EXPECT_EQ("9.0000200e3", test.toString());

   test.fromString("-9000.002");
   EXPECT_EQ("-9.0000020e3", test.toString());

   test.fromString("-8000,003");
   EXPECT_EQ("-8.0000030e3", test.toString());

   test.fromString("-.002");
   EXPECT_EQ("-2.0000000e-3", test.toString());

   test.fromString("-,003");
   EXPECT_EQ("-3.0000000e-3", test.toString());


   test.fromString("0");
   EXPECT_EQ("0.0000000e0", test.toString());
   EXPECT_EQ(test.toString().length(), test.getLength());

   test.fromString("0.00");
   EXPECT_EQ("0.0000000e0", test.toString());

   test.fromString("0.");
   EXPECT_EQ("0.0000000e0", test.toString());

   test.fromString("");
   EXPECT_EQ("0.0000000e0", test.toString());

   test.fromString("-0");
   EXPECT_EQ("-0.0000000e0", test.toString());
   EXPECT_EQ(test.toString().length(), test.getLength());
   EXPECT_TRUE(test.isSigned());


   test.fromString("12.123");
   EXPECT_EQ("1.2123000e1", test.toString());

   BigInt::Float test2 (test);
   EXPECT_EQ(test.toString(), test2.toString());

   BigInt::Float test3 (test.toString());
   EXPECT_EQ(test.toString(), test3.toString());

   test2 = -test;
   EXPECT_EQ("1.2123000e1", test.toString());
   EXPECT_EQ("-1.2123000e1", test2.toString());

   EXPECT_EQ(test.toString().length(), test.getLength());
   EXPECT_EQ(test2.toString().length(), test2.getLength());


   test.fromString("12.123e0");
   EXPECT_EQ("1.2123000e1", test.toString());
   EXPECT_EQ(test.toString().length(), test.getLength());

   test.fromString("12.123e3");
   EXPECT_EQ("1.2123000e4", test.toString());

   test.fromString("12,123e3");
   EXPECT_EQ("1.2123000e4", test.toString());

   test.fromString("12.123e+3");
   EXPECT_EQ("1.2123000e4", test.toString());

   test.fromString("12.123e-3");
   EXPECT_EQ("1.2123000e-2", test.toString());
   EXPECT_EQ(test.toString().length(), test.getLength());

   test.fromString("12.123E0");
   EXPECT_EQ("1.2123000e1", test.toString());
   EXPECT_EQ(test.toString().length(), test.getLength());

   test.fromString("12.123E3");
   EXPECT_EQ("1.2123000e4", test.toString());

   test.fromString("12.123E+3");
   EXPECT_EQ("1.2123000e4", test.toString());
   EXPECT_EQ(test.toString().length(), test.getLength());

   test.fromString("12.123E-3");
   EXPECT_EQ("1.2123000e-2", test.toString());
   EXPECT_EQ(test.toString().length(), test.getLength());

   BigInt::Float i ("INF");
   BigInt::Float n ("NAN");

   EXPECT_EQ("Inf", i.toString());
   EXPECT_EQ(i.toString().length(), i.getLength());
   i.fromString("-Inf");
   EXPECT_EQ("-Inf", i.toString());
   EXPECT_EQ(i.toString().length(), i.getLength());
   i.fromString("Inf");
   EXPECT_EQ("Inf", i.toString());
   i.fromString("-INF");
   EXPECT_EQ("-Inf", i.toString());
   i.fromString("+INF");
   EXPECT_EQ("Inf", i.toString());
   i.fromString("+Inf");
   EXPECT_EQ("Inf", i.toString());
   EXPECT_EQ("NaN", n.toString());
   n.fromString("NaN");
   EXPECT_EQ("NaN", n.toString());
   EXPECT_EQ(n.toString().length(), n.getLength());


   test.fromString("12e0");
   EXPECT_EQ("1.2000000e1", test.toString());

   test.fromString("12e1");
   EXPECT_EQ("1.2000000e2", test.toString());

   test.fromString("12e+1");
   EXPECT_EQ("1.2000000e2", test.toString());

   test.fromString("12e-1");
   EXPECT_EQ("1.2000000e0", test.toString());

   test.fromString("12e"); // None of the lexers should ever give me this,
   EXPECT_EQ("1.2000000e1", test.toString()); // but make sure it doesn't crash.

   test.fromString("12E0");
   EXPECT_EQ("1.2000000e1", test.toString());

   test.fromString("12E1");
   EXPECT_EQ("1.2000000e2", test.toString());

   test.fromString("12E+1");
   EXPECT_EQ("1.2000000e2", test.toString());

   test.fromString("12E-1");
   EXPECT_EQ("1.2000000e0", test.toString());

   test.fromString("12E"); // None of the lexers should ever give me this,
   EXPECT_EQ("1.2000000e1", test.toString()); // but make sure it doesn't crash.
 }

TEST(FixedTests, testAdds)
 {
   BigInt::Float a ("1.35");
   BigInt::Float b ("1.125");
   BigInt::Float c;

   c = a + b;
   EXPECT_EQ("2.4750000e0", c.toString());

   c = b + a;
   EXPECT_EQ("2.4750000e0", c.toString());

   c = a + a;
   EXPECT_EQ("2.7000000e0", c.toString());

      // Those three covered the Fixed adder. Now, let's get the Integer portion: negative numbers, zeros, result zero.

   b.fromString("0.000");

   c = a + b;
   EXPECT_EQ("1.3500000e0", c.toString());

   c = b + a;
   EXPECT_EQ("1.3500000e0", c.toString());

   b.fromString("-0.000");

   c = a + b;
   EXPECT_EQ("1.3500000e0", c.toString());

   c = b + a;
   EXPECT_EQ("1.3500000e0", c.toString());

   a.fromString("-0");

   c = a + b;
   EXPECT_EQ("-0.0000000e0", c.toString());

   c = b + a;
   EXPECT_EQ("-0.0000000e0", c.toString());

   a.fromString("0.0");

   c = a + b;
   EXPECT_EQ("0.0000000e0", c.toString());

   c = b + a;
   EXPECT_EQ("0.0000000e0", c.toString());

   b.fromString("0.000");

   c = a + b;
   EXPECT_EQ("0.0000000e0", c.toString());

   c = b + a;
   EXPECT_EQ("0.0000000e0", c.toString());


   a.fromString("1.35");
   b.fromString("-1.35");

   c = a + b;
   EXPECT_EQ("0.0000000e0", c.toString());

   a.fromString("-1.35");
   b.fromString("-1.125");

   c = a + b;
   EXPECT_EQ("-2.4750000e0", c.toString());

   c = b + a;
   EXPECT_EQ("-2.4750000e0", c.toString());

   c = a + a;
   EXPECT_EQ("-2.7000000e0", c.toString());

   a.fromString("-1.35");
   b.fromString("1.125");

   c = a + b;
   EXPECT_EQ("-2.2500000e-1", c.toString());

   c = b + a;
   EXPECT_EQ("-2.2500000e-1", c.toString());

   a.fromString("1.35");
   b.fromString("-1.125");

   c = a + b;
   EXPECT_EQ("2.2500000e-1", c.toString());

   c = b + a;
   EXPECT_EQ("2.2500000e-1", c.toString());

   a.fromString("2.359999999999");
   b.fromString("-1.125");

   c = a + b;
   EXPECT_EQ("1.234999999999e0", c.toString());

   c = b + a;
   EXPECT_EQ("1.234999999999e0", c.toString());

   a.fromString("1.0");
   b.fromString("NaN");

   EXPECT_EQ(32U, b.isNaN());

   c = a + b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(32U, c.isNaN());

   c = b + a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(32U, c.isNaN());

   a.fromString("Inf");
   b.fromString("-Inf");

   c = a + b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(1U, c.isNaN());

   c = b + a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(1U, c.isNaN());

   a = c;
   b.fromString("NaN");

   c = a + b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(33U, c.isNaN());

   c = b + a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(33U, c.isNaN());

   a.fromString("Inf");
   b.fromString("Inf");

   c = a + b;
   EXPECT_EQ("Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   c = b + a;
   EXPECT_EQ("Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   a.fromString("-Inf");
   b.fromString("-Inf");

   c = a + b;
   EXPECT_EQ("-Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   c = b + a;
   EXPECT_EQ("-Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   a.fromString("Inf");
   b.fromString("1");

   c = a + b;
   EXPECT_EQ("Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   c = b + a;
   EXPECT_EQ("Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   a.fromString("1.1111111e0");
   b.fromString("1.1111111e-3");

   c = a + b;
   EXPECT_EQ("1.1122222e0", c.toString());

   c = b + a;
   EXPECT_EQ("1.1122222e0", c.toString());

   a.fromString("1.1111111e0");
   b.fromString("1.1111111e-90");

   c = a + b;
   EXPECT_EQ("1.1111111e0", c.toString());

   c = b + a;
   EXPECT_EQ("1.1111111e0", c.toString());

   a.fromString("0.000");
   b.fromString("0.000");

   c = a - b;
   EXPECT_EQ("0.0000000e0", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_NEGATIVE_INFINITY);

   a.fromString("1.1111111e0");
   b.fromString("-1.1111111e-90");

   c = a + b;
   EXPECT_EQ("1.1111110e0", c.toString());

   c = b + a;
   EXPECT_EQ("1.1111110e0", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);
   BigInt::Fixed::setRoundMode(BigInt::ROUND_ZERO);

   a.fromString("1.1111111e0");
   b.fromString("-1.1111111e-90");

   c = a + b;
   EXPECT_EQ("1.1111110e0", c.toString());

   c = b + a;
   EXPECT_EQ("1.1111110e0", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);
   BigInt::Fixed::setRoundMode(BigInt::ROUND_NEGATIVE_INFINITY);

   a.fromString("-1.1111111e0");
   b.fromString("1.1111111e0");

   c = a + b;
   EXPECT_EQ("-0.0000000e0", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);

   a.fromString("99");
   b.fromString("1");

   c = a + b;
   EXPECT_EQ("1.0000000e2", c.toString());

   a.fromString("9999");
   b.fromString("-9990");

   c = a + b;
   EXPECT_EQ("9.0000000e0", c.toString());

   a.fromString("1.0000000e0");
   b.fromString("3.0000000e-8");

   c = a - b;
   EXPECT_EQ("9.9999997e-1", c.toString());
 }

TEST(FixedTests, testSubs)
 {
   BigInt::Float a ("1.35");
   BigInt::Float b ("1.125");
   BigInt::Float c;

   c = a - b;
   EXPECT_EQ("2.2500000e-1", c.toString());

   c = b - a;
   EXPECT_EQ("-2.2500000e-1", c.toString());

   c = a - a;
   EXPECT_EQ("0.0000000e0", c.toString());


   b.fromString("0.0");

   c = a - b;
   EXPECT_EQ("1.3500000e0", c.toString());

   c = b - a;
   EXPECT_EQ("-1.3500000e0", c.toString());


   b.fromString("-1.35");

   c = a - b;
   EXPECT_EQ("2.7000000e0", c.toString());

   a.fromString("-1.35");
   b.fromString("-1.125");

   c = a - b;
   EXPECT_EQ("-2.2500000e-1", c.toString());

   c = b - a;
   EXPECT_EQ("2.2500000e-1", c.toString());

   c = a - a;
   EXPECT_EQ("0.0000000e0", c.toString());

   a.fromString("-1.35");
   b.fromString("1.125");

   c = a - b;
   EXPECT_EQ("-2.4750000e0", c.toString());

   c = b - a;
   EXPECT_EQ("2.4750000e0", c.toString());

   a.fromString("1.35");
   b.fromString("-1.125");

   c = a - b;
   EXPECT_EQ("2.4750000e0", c.toString());

   c = b - a;
   EXPECT_EQ("-2.4750000e0", c.toString());

   a.fromString("1.0");
   b.fromString("NaN");

   c = a - b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(32U, c.isNaN());

   c = b - a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(32U, c.isNaN());

   a.fromString("Inf");
   b.fromString("Inf");

   c = a - b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(1U, c.isNaN());

   c = b - a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(1U, c.isNaN());

   a = c;
   b.fromString("NaN");

   c = a - b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(33U, c.isNaN());

   c = b - a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(33U, c.isNaN());

   a.fromString("Inf");
   b.fromString("-Inf");

   c = a - b;
   EXPECT_EQ("Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   c = b - a;
   EXPECT_EQ("-Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   a.fromString("-Inf");
   b.fromString("-Inf");

   c = a - b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(1U, c.isNaN());

   c = b - a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(1U, c.isNaN());

   a.fromString("Inf");
   b.fromString("1");

   c = a - b;
   EXPECT_EQ("Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   c = b - a;
   EXPECT_EQ("-Inf", c.toString());
   EXPECT_TRUE(c.isInfinity());

   a.fromString("1.1111111e0");
   b.fromString("1.1111111e-3");

   c = a - b;
   EXPECT_EQ("1.1100000e0", c.toString());

   c = b - a;
   EXPECT_EQ("-1.1100000e0", c.toString());

   a.fromString("1.1111111e0");
   b.fromString("1.1111111e-90");

   c = a - b;
   EXPECT_EQ("1.1111111e0", c.toString());

   c = b - a;
   EXPECT_EQ("-1.1111111e0", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_NEGATIVE_INFINITY);

   a.fromString("1.1111111e0");
   b.fromString("1.1111111e-90");

   c = a - b;
   EXPECT_EQ("1.1111110e0", c.toString());

   c = b - a;
   EXPECT_EQ("-1.1111111e0", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);
   BigInt::Fixed::setRoundMode(BigInt::ROUND_POSITIVE_INFINITY);

   a.fromString("1.1111111e0");
   b.fromString("1.1111111e-90");

   c = a - b;
   EXPECT_EQ("1.1111111e0", c.toString());

   c = b - a;
   EXPECT_EQ("-1.1111110e0", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);
   BigInt::Fixed::setRoundMode(BigInt::ROUND_NEGATIVE_INFINITY);

   a.fromString("1.1111111e0");
   b.fromString("1.1111111e0");

   c = a - b;
   EXPECT_EQ("-0.0000000e0", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);
   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_ODD);

   a.fromString("1.0000000e9");
   b.fromString("5.0000000e0");

   // 1000000000 - 5 = 999999995
   c = a - b;
   EXPECT_EQ("9.9999999e8", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);
 }

TEST(FixedTests, testComparisons)
 {
   BigInt::Float a ("1.25");
   BigInt::Float b ("2.25");

   EXPECT_FALSE(a >  b);
   EXPECT_FALSE(a >= b);
   EXPECT_TRUE (a <  b);
   EXPECT_TRUE (a <= b);
   EXPECT_FALSE(a == b);
   EXPECT_TRUE (a != b);

   EXPECT_TRUE (b >  a);
   EXPECT_TRUE (b >= a);
   EXPECT_FALSE(b <  a);
   EXPECT_FALSE(b <= a);
   EXPECT_FALSE(b == a);
   EXPECT_TRUE (b != a);

   BigInt::Float c ("1.25");
   BigInt::Float d ("1.250");

   EXPECT_FALSE(c >  d);
   EXPECT_TRUE (c >= d);
   EXPECT_FALSE(c <  d);
   EXPECT_TRUE (c <= d);
   EXPECT_TRUE (c == d);
   EXPECT_FALSE(c != d);

   EXPECT_FALSE(d >  c);
   EXPECT_TRUE (d >= c);
   EXPECT_FALSE(d <  c);
   EXPECT_TRUE (d <= c);
   EXPECT_TRUE (d == c);
   EXPECT_FALSE(d != c);

   BigInt::Float e ("-1.25");
   BigInt::Float f ("1.25");

   EXPECT_FALSE(e >  f);
   EXPECT_FALSE(e >= f);
   EXPECT_TRUE (e <  f);
   EXPECT_TRUE (e <= f);
   EXPECT_FALSE(e == f);
   EXPECT_TRUE (e != f);

   EXPECT_TRUE (f >  e);
   EXPECT_TRUE (f >= e);
   EXPECT_FALSE(f <  e);
   EXPECT_FALSE(f <= e);
   EXPECT_FALSE(f == e);
   EXPECT_TRUE (f != e);

   BigInt::Float g ("0");
   BigInt::Float h ("1");

   EXPECT_FALSE(g >  h);
   EXPECT_FALSE(g >= h);
   EXPECT_TRUE (g <  h);
   EXPECT_TRUE (g <= h);
   EXPECT_FALSE(g == h);
   EXPECT_TRUE (g != h);

   EXPECT_TRUE (h >  g);
   EXPECT_TRUE (h >= g);
   EXPECT_FALSE(h <  g);
   EXPECT_FALSE(h <= g);
   EXPECT_FALSE(h == g);
   EXPECT_TRUE (h != g);

   BigInt::Float i ("0");
   EXPECT_TRUE(g == i);

   BigInt::Float j ("-1.25");
   BigInt::Float k ("-2.25");

   EXPECT_TRUE (j >  k);
   EXPECT_TRUE (j >= k);
   EXPECT_FALSE(j <  k);
   EXPECT_FALSE(j <= k);
   EXPECT_FALSE(j == k);
   EXPECT_TRUE (j != k);

   EXPECT_FALSE(k >  j);
   EXPECT_FALSE(k >= j);
   EXPECT_TRUE (k <  j);
   EXPECT_TRUE (k <= j);
   EXPECT_FALSE(k == j);
   EXPECT_TRUE (k != j);

   a.fromString("2.25");
   b.fromString("Inf");

   EXPECT_FALSE(a >  b);
   EXPECT_FALSE(a >= b);
   EXPECT_TRUE (a <  b);
   EXPECT_TRUE (a <= b);
   EXPECT_FALSE(a == b);
   EXPECT_TRUE (a != b);

   EXPECT_TRUE (b >  a);
   EXPECT_TRUE (b >= a);
   EXPECT_FALSE(b <  a);
   EXPECT_FALSE(b <= a);
   EXPECT_FALSE(b == a);
   EXPECT_TRUE (b != a);

   a.fromString("-Inf");
   b.fromString("-2.25");

   EXPECT_FALSE(a >  b);
   EXPECT_FALSE(a >= b);
   EXPECT_TRUE (a <  b);
   EXPECT_TRUE (a <= b);
   EXPECT_FALSE(a == b);
   EXPECT_TRUE (a != b);

   EXPECT_TRUE (b >  a);
   EXPECT_TRUE (b >= a);
   EXPECT_FALSE(b <  a);
   EXPECT_FALSE(b <= a);
   EXPECT_FALSE(b == a);
   EXPECT_TRUE (b != a);

   a.fromString("Inf");
   b.fromString("Inf");

   EXPECT_FALSE(a >  b);
   EXPECT_TRUE (a >= b);
   EXPECT_FALSE(a <  b);
   EXPECT_TRUE (a <= b);
   EXPECT_TRUE (a == b);
   EXPECT_FALSE(a != b);

   EXPECT_FALSE(b >  a);
   EXPECT_TRUE (b >= a);
   EXPECT_FALSE(b <  a);
   EXPECT_TRUE (b <= a);
   EXPECT_TRUE (b == a);
   EXPECT_FALSE(b != a);

   a.fromString("-Inf");
   b.fromString("Inf");

   EXPECT_FALSE(a >  b);
   EXPECT_FALSE(a >= b);
   EXPECT_TRUE (a <  b);
   EXPECT_TRUE (a <= b);
   EXPECT_FALSE(a == b);
   EXPECT_TRUE (a != b);

   EXPECT_TRUE (b >  a);
   EXPECT_TRUE (b >= a);
   EXPECT_FALSE(b <  a);
   EXPECT_FALSE(b <= a);
   EXPECT_FALSE(b == a);
   EXPECT_TRUE (b != a);

   a.fromString("7");
   b.fromString("NaN");

   EXPECT_FALSE(a >  b);
   EXPECT_FALSE(a >= b);
   EXPECT_FALSE(a <  b);
   EXPECT_FALSE(a <= b);
   EXPECT_FALSE(a == b);
   EXPECT_TRUE (a != b);

   EXPECT_FALSE(b >  a);
   EXPECT_FALSE(b >= a);
   EXPECT_FALSE(b <  a);
   EXPECT_FALSE(b <= a);
   EXPECT_FALSE(b == a);
   EXPECT_TRUE (b != a);

   a.fromString("0");
   b.fromString("-0");

   EXPECT_FALSE(a >  b);
   EXPECT_TRUE (a >= b);
   EXPECT_FALSE(a <  b);
   EXPECT_TRUE (a <= b);
   EXPECT_TRUE (a == b);
   EXPECT_FALSE(a != b);

   EXPECT_FALSE(b >  a);
   EXPECT_TRUE (b >= a);
   EXPECT_FALSE(b <  a);
   EXPECT_TRUE (b <= a);
   EXPECT_TRUE (b == a);
   EXPECT_FALSE(b != a);

   a.fromString("1.25");
   b.fromString("22.5");

   EXPECT_FALSE(a >  b);
   EXPECT_FALSE(a >= b);
   EXPECT_TRUE (a <  b);
   EXPECT_TRUE (a <= b);
   EXPECT_FALSE(a == b);
   EXPECT_TRUE (a != b);

   EXPECT_TRUE (b >  a);
   EXPECT_TRUE (b >= a);
   EXPECT_FALSE(b <  a);
   EXPECT_FALSE(b <= a);
   EXPECT_FALSE(b == a);
   EXPECT_TRUE (b != a);

   a.fromString("-1.25");
   b.fromString("-22.5");

   EXPECT_TRUE (a >  b);
   EXPECT_TRUE (a >= b);
   EXPECT_FALSE(a <  b);
   EXPECT_FALSE(a <= b);
   EXPECT_FALSE(a == b);
   EXPECT_TRUE (a != b);

   EXPECT_FALSE(b >  a);
   EXPECT_FALSE(b >= a);
   EXPECT_TRUE (b <  a);
   EXPECT_TRUE (b <= a);
   EXPECT_FALSE(b == a);
   EXPECT_TRUE (b != a);


   EXPECT_TRUE (a == a);
 }

TEST(FixedTests, testMulterply)
 {
   BigInt::Float a ("2");
   BigInt::Float b ("3");
   BigInt::Float c;

   c = a * b;
   EXPECT_EQ("6.0000000e0", c.toString());

   c = b * a;
   EXPECT_EQ("6.0000000e0", c.toString());

   BigInt::Float d ("0");

   c = a * d;
   EXPECT_EQ("0.0000000e0", c.toString());

   c = d * a;
   EXPECT_EQ("0.0000000e0", c.toString());

   a.fromString("3");
   b.fromString("4");

   c = a * b;
   EXPECT_EQ("1.2000000e1", c.toString());

   c = b * a;
   EXPECT_EQ("1.2000000e1", c.toString());

   a.fromString("Inf");
   b.fromString("0");

   c = a * b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(2U, c.isNaN());

   c = b * a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(2U, c.isNaN());

   a = c;
   b.fromString("NaN");

   c = a * b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(34U, c.isNaN());

   c = b * a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(34U, c.isNaN());

   a.fromString("NaN");
   b.fromString("0");

   c = a * b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(32U, c.isNaN());

   c = b * a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(32U, c.isNaN());

   a.fromString("Inf");
   b.fromString("7");

   c = a * b;
   EXPECT_EQ("Inf", c.toString());

   c = b * a;
   EXPECT_EQ("Inf", c.toString());

   b = -b;

   c = a * b;
   EXPECT_EQ("-Inf", c.toString());

   c = b * a;
   EXPECT_EQ("-Inf", c.toString());


   a.fromString("9.99999999");
   EXPECT_EQ(8U, a.getPrecision());
   a.changePrecision(7);
   EXPECT_EQ(7U, a.getPrecision());
   EXPECT_EQ("1.0000000e1", a.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_POSITIVE_INFINITY);
   a.fromString("3.1622777");
   b.fromString("3.1622776");
   c = a * b;
   EXPECT_EQ("1.0000000e1", c.toString());
   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);

   a.fromString("2.00000000");
   b.fromString("3.0000000");
   c = a * b;
   EXPECT_EQ("6.00000000e0", c.toString());
 }

TEST(FixedTests, testDerverd)
 {
   BigInt::Float a ("0");
   BigInt::Float b ("3");
   BigInt::Float c;

   c = a / b;
   EXPECT_EQ("0.0000000e0", c.toString());

   c = b / a;
   EXPECT_EQ("Inf", c.toString());

   a.fromString("1");
   c = a / b;
   EXPECT_EQ("3.3333333e-1", c.toString());

   a.fromString("6");
   c = a / b;
   EXPECT_EQ("2.0000000e0", c.toString());

   a.fromString("2");
   b.fromString("-3");
   c = a / b;
   EXPECT_EQ("-6.6666667e-1", c.toString());

   a.fromString("100.000");
   b.fromString("50.00");
   c = a / b;
   EXPECT_EQ("2.0000000e0", c.toString());

   a.fromString("125.000");
   b.fromString("50.00");
   c = a / b;
   EXPECT_EQ("2.5000000e0", c.toString());

   a.fromString("175.000");
   b.fromString("50.00");
   c = a / b;
   EXPECT_EQ("3.5000000e0", c.toString());

   a.fromString("170.000");
   b.fromString("50.00");
   c = a / b;
   EXPECT_EQ("3.4000000e0", c.toString());

   a.fromString("2.000");
   b.fromString("3.00");
   c = a / b;
   EXPECT_EQ("6.6666667e-1", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_ZERO);
   c = a / b;
   EXPECT_EQ("6.6666666e-1", c.toString());

   BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);

   a.fromString("0");
   b.fromString("0");

   c = a / b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(4U, c.isNaN());

   c = b / a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(4U, c.isNaN());

   a.fromString("Inf");
   b.fromString("Inf");

   c = a / b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(8U, c.isNaN());

   c = b / a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(8U, c.isNaN());

   a.fromString("0");
   b.fromString("0");

   c = a / b;

   a.fromString("Inf");
   b.fromString("Inf");

   b = a / b;
   a = c;

   c = a / b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(12U, c.isNaN());

   c = b / a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(12U, c.isNaN());

   a.fromString("NaN");
   b.fromString("7");

   c = a / b;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(32U, c.isNaN());

   c = b / a;
   EXPECT_EQ("NaN", c.toString());
   EXPECT_EQ(32U, c.isNaN());

   a.fromString("Inf");
   b.fromString("3");
   c = a / b;
   EXPECT_EQ("Inf", c.toString());
   c = b / a;
   EXPECT_EQ("0.0000000e0", c.toString());

   a.fromString("0");
   b.fromString("3");
   c = a / b;
   EXPECT_EQ("0.0000000e0", c.toString());
   c = b / a;
   EXPECT_EQ("Inf", c.toString());

   a.fromString("-0");
   b.fromString("3");
   c = a / b;
   EXPECT_EQ("-0.0000000e0", c.toString());
   c = b / a;
   EXPECT_EQ("-Inf", c.toString());

   a.fromString("2.00000000");
   b.fromString("3.00");
   c = a / b;
   EXPECT_EQ("6.66666667e-1", c.toString());

   a.fromString("2.00000000");
   b.fromString("-3.00");
   c = a / b;
   EXPECT_EQ("-6.66666667e-1", c.toString());
 }

   // This function is the foundation of using Fixeds in code.
   // It is the only time that client code calls an Integer function directly.
TEST(FixedTests, testRoundInt)
 {
   BigInt::Float test ("2.5");
   EXPECT_EQ(2, test.roundToInteger().toInt()); // Assume round to even

   test.fromString("3.5");
   EXPECT_EQ(4, test.roundToInteger().toInt());

   test.fromString("-2.5");
   EXPECT_EQ(-2, test.roundToInteger().toInt());

   test.fromString("-3.5");
   EXPECT_EQ(-4, test.roundToInteger().toInt());

   test.fromString("3.2");
   EXPECT_EQ(3, test.roundToInteger().toInt());

   test.fromString("-3.2");
   EXPECT_EQ(-3, test.roundToInteger().toInt());

   test.fromString("5");
   EXPECT_EQ(5, test.roundToInteger().toInt());

   test.fromString("0");
   EXPECT_EQ(0, test.roundToInteger().toInt());

   test.fromString(".001");
   EXPECT_EQ(0, test.roundToInteger().toInt());


   test.fromString("2.5");
   EXPECT_EQ("2.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString("3.5");
   EXPECT_EQ("4.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString("-2.5");
   EXPECT_EQ("-2.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString("-3.5");
   EXPECT_EQ("-4.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString("3.2");
   EXPECT_EQ("3.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString("-3.2");
   EXPECT_EQ("-3.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString("5");
   EXPECT_EQ("5.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString("0");
   EXPECT_EQ("0.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString(".001");
   EXPECT_EQ("0.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());

   test.fromString("1.1234567890123456789e-9");
   EXPECT_EQ("0.0000000e0", test.roundToInteger(BigInt::ROUND_TIES_EVEN).toString());
 }

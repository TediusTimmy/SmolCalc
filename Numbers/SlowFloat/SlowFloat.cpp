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
#include "SlowFloat.h"

#include <cmath>

#include <sstream>
#include <iomanip>
#include <cctype>

namespace SlowFloat
 {

const int16_t SPECIAL_EXPONENT = -32768;
const int16_t MAX_EXPONENT = 32767;
const int16_t MIN_EXPONENT = -32767;
const uint32_t POSITIVE_INFINITY = 0;
const uint32_t NEGATIVE_INFINITY = ~0;
const uint32_t NOT_A_NUMBER = 255;

const uint32_t BIAS =           1000000000;
const uint32_t MAX_SIGNIFICAND = 999999999;
const uint32_t MIN_SIGNIFICAND = 100000000;
const uint32_t SIGN_BIT = 0x80000000;
const int16_t CUTOFF = 9; // Difference in exponents where we consider the operation degenerate.

const SlowFloat sfZero = SlowFloat(0, 0);
const SlowFloat sfNaN  = SlowFloat(NOT_A_NUMBER, SPECIAL_EXPONENT);
const SlowFloat sfInf  = SlowFloat(POSITIVE_INFINITY, SPECIAL_EXPONENT);
const SlowFloat sfNInf = SlowFloat(NEGATIVE_INFINITY, SPECIAL_EXPONENT);

bool isInf (const SlowFloat& arg)
 {
   if (arg.exponent != SPECIAL_EXPONENT)
      return false;
   if ((arg.significand != POSITIVE_INFINITY) && (arg.significand != NEGATIVE_INFINITY))
      return false;
   return true;
 }

bool isNaN (const SlowFloat& arg)
 {
   if (arg.exponent != SPECIAL_EXPONENT)
      return false;
   if ((arg.significand == POSITIVE_INFINITY) || (arg.significand == NEGATIVE_INFINITY))
      return false;
   return true;
 }

bool isZero (const SlowFloat& arg)
 {
   if (arg.exponent == SPECIAL_EXPONENT)
      return false;
   if ((arg.significand != POSITIVE_INFINITY) && (arg.significand != NEGATIVE_INFINITY))
      return false;
   return true;
 }

bool getSign (const SlowFloat& arg)
 {
   if (0 == (arg.significand & SIGN_BIT))
      return false;
   return true;
 }

   // Table of powers of ten up to CUTOFF. (Yes, there is an extra entry, for the -1 case that needs to return 1.)
const uint64_t makeShift [] = { 1U, 1U, 10U, 100U, 1000U, 10000U, 100000U, 1000000U, 10000000U, 100000000U, 1000000000U };

SlowFloat_Round_Mode mode = ROUND_TIES_EVEN;

   // This code is suspiciously familiar....
// sign - sign of result (true is negative)
// even - whether the current significand is even
// comp - divisor - 2 * remainder : this will be positive if we should round down, negative if we should round up, and zero if we are exactly in the middle
// zero - whether the remainder is zero : true if the division was exact (if true, comp must be positive)
// returns whether the unsigned significand should be incremented away from zero
bool decideRound (bool sign, bool even, int64_t comp, bool zero)
 {
   switch (mode)
    {
      case ROUND_TIES_EVEN:
         if ((comp < 0) || ((comp == 0) && !even)) return true;
         break;
      case ROUND_TIES_AWAY:
         if (comp <= 0) return true;
         break;
      case ROUND_POSITIVE_INFINITY:
         if (!sign && !zero) return true;
         break;
      case ROUND_NEGATIVE_INFINITY:
         if (sign && !zero) return true;
         break;
      case ROUND_ZERO:
         break;
      case ROUND_TIES_ODD:
         if ((comp < 0) || ((comp == 0) && even)) return true;
         break;
      case ROUND_TIES_ZERO:
         if (comp < 0) return true;
         break;
      case ROUND_AWAY:
         if (!zero) return true;
         break;
    }
   return false;
 }

SlowFloat::SlowFloat(uint32_t significand, int16_t exponent) : significand(significand), exponent(exponent)
 {
 }

SlowFloat::SlowFloat (double arg)
 {
   if (0.0 == arg)
    {
      significand = 0;
      exponent = 0;
    }
   else if (std::isinf(arg))
    {
      significand = POSITIVE_INFINITY;
      exponent = SPECIAL_EXPONENT;
    }
   else if (std::isnan(arg))
    {
      significand = NOT_A_NUMBER;
      exponent = SPECIAL_EXPONENT;
    }
   else
    {
      exponent = static_cast<int16_t>(std::floor(std::log10(std::fabs(arg)))); // Should never overflow
         // I haven't run into a case where Windows' bad pow bites me ... yet. But preempt.
      if (exponent >= 0)
         significand = static_cast<uint32_t>(std::fabs(arg) / std::pow(10.0, exponent) * MIN_SIGNIFICAND);
      else
         significand = static_cast<uint32_t>(std::fabs(arg) * std::pow(10.0, -exponent) * MIN_SIGNIFICAND);
      double temp = 5.0;
         // One Windows, 10^-8 is poorly enough calculated that converting 2.0 results in 2.00000001
         //    when rounding to positive infinity or away from zero.
      int bias = exponent - (CUTOFF - 1);
      if (bias >= 0) temp = temp - (std::fabs(arg) / std::pow(10.0, bias) - significand) * 10;
      else temp = temp - (std::fabs(arg) * std::pow(10.0, -bias) - significand) * 10;
      int comp = 0;
      if (temp > 0.0) comp = 1;
      else if (temp < 0.0) comp = -1;
      if (decideRound(arg < 0.0, 0 == (significand & 1), comp, 5.0 == temp))
       {
         ++significand;
         if (significand == BIAS)
          {
            significand = MIN_SIGNIFICAND;
            ++exponent;
          }
       }
    }
   if (std::signbit(arg))
    {
      significand = ~significand;
    }
 }

SlowFloat::operator double () const
 {
   double sign = 1.0;
   uint32_t sig = significand;
   if (getSign(*this))
    {
      sign = -1.0;
      sig = ~sig;
    }
   if (isNaN(*this))
      return std::copysign(std::asin(2.0), sign);
   if (isInf(*this))
      return std::copysign(std::exp(1000000.0), sign);
      // One Windows, 7.00000000 kept being ever so slightly above 7.0
   int bias = exponent - (CUTOFF - 1);
   if (bias >= 0)
      return sign * static_cast<double>(sig) * std::pow(10.0, bias);
   else
      return sign * static_cast<double>(sig) / std::pow(10.0, -bias);
 }


std::string toString (const SlowFloat& arg)
 {
   std::ostringstream temp;
   uint32_t sig = arg.significand;

   if (getSign(arg))
    {
      temp << '-';
      sig = ~sig;
    }

   if (isNaN(arg))
      temp << "NaN";
   else if (isInf(arg))
      temp << "Inf";
   else if (isZero(arg))
      temp << "0";
   else if ((arg.exponent > 8) || (arg.exponent < -7))
    {
      uint32_t first = sig / MIN_SIGNIFICAND;
      uint32_t rest = sig % MIN_SIGNIFICAND;
      temp << first << '.' << std::setw(8) << std::setfill('0') << rest << 'e';
      if (arg.exponent > -1)
         temp << '+';
      temp << arg.exponent;
    }
   else if (arg.exponent > -1)
    {
      std::string temp2 = std::to_string(sig);
      temp2 = temp2.substr(0, arg.exponent + 1) + '.' + temp2.substr(arg.exponent + 1);
      while ('0' == temp2[temp2.size() - 1])
         temp2.resize(temp2.size() - 1);
      if ('.' == temp2[temp2.size() - 1])
         temp2.resize(temp2.size() - 1);
      temp << temp2;
    }
   else
    {
      std::ostringstream temp2;
      temp2 << "0.";
      if (arg.exponent < -1)
         temp2 << std::setw(-arg.exponent - 1)<< std::setfill('0') << '0';
      temp2 << sig;
      std::string temp3 = temp2.str();
      while ('0' == temp3[temp3.size() - 1])
         temp3.resize(temp3.size() - 1);
      temp << temp3;
    }

   return temp.str();
 }

SlowFloat fromString (const std::string& arg)
 {
   const char* iter = arg.c_str();
   bool resultSign = false;
   int32_t resultExponent = -1;
   uint32_t resultSignificand = 0;
   int digits = 0;
   bool hasResidue = false;
   bool allZero = true;
   bool realDigit = false;
   int residue = 0;

   if ('-' == *iter)
    {
      resultSign = true;
      ++iter;
    }

   while (std::isdigit(*iter))
    {
      if (digits < CUTOFF)
       {
         if (realDigit || ('0' != *iter))
          {
            resultSignificand = resultSignificand * 10 + *iter - '0';
            ++digits;
            realDigit = true;
          }
       }
      else
       {
         if (!hasResidue)
          {
            hasResidue = true;
            if ('0' == *iter)
             {
               // Do Nothing
             }
            else
             {
               allZero = false;
               if (*iter > '5')
                {
                  residue = -1;
                }
               else if (*iter < '5')
                {
                  residue = 1;
                }
             }
          }
         else
          {
            if (0 == residue)
             {
               if ('0' == *iter)
                {
                  // Do Nothing
                }
               else
                {
                  if (allZero)
                     residue = 1;
                  else
                     residue = -1;
                  allZero = false;
                }
             }
          }
       }
      if (realDigit)
         ++resultExponent;
      ++iter;
    }
   if ('.' == *iter)
    {
      ++iter;
    }
   while (std::isdigit(*iter))
    {
      if (digits < CUTOFF)
       {
         if (realDigit || ('0' != *iter))
          {
            resultSignificand = resultSignificand * 10 + *iter - '0';
            ++digits;
            realDigit = true;
          }
       }
      else
       {
         if (!hasResidue)
          {
            hasResidue = true;
            if ('0' == *iter)
             {
               // Do Nothing
             }
            else
             {
               allZero = false;
               if (*iter > '5')
                {
                  residue = -1;
                }
               else if (*iter < '5')
                {
                  residue = 1;
                }
             }
          }
         else
          {
            if (0 == residue)
             {
               if ('0' == *iter)
                {
                  // Do Nothing
                }
               else
                {
                  if (allZero)
                     residue = 1;
                  else
                     residue = -1;
                  allZero = false;
                }
             }
          }
       }
      if (!realDigit)
         --resultExponent;
      ++iter;
    }
   while (digits < CUTOFF)
    {
      resultSignificand *= 10;
      ++digits;
    }
   if (allZero) // By the definition of the comp argument.
      residue = 1;
   if (decideRound(resultSign, 0 == (1 & resultSignificand), residue, allZero))
    {
      ++resultSignificand;
      if (resultSignificand == BIAS)
       {
         resultSignificand = MIN_SIGNIFICAND;
         ++resultExponent;
       }
    }
   if ('e' == (*iter | ' '))
    {
      ++iter;
      int32_t exponentSign = 1;
      int32_t exponentValue = 0;
      if ('-' == *iter)
       {
         exponentSign = -1;
         ++iter;
       }
      if ('+' == *iter)
       {
         ++iter;
       }
      while (std::isdigit(*iter))
       {
         exponentValue = exponentValue * 10 + *iter - '0';
         ++iter;
         if (((exponentValue * exponentSign) > (2 * MAX_EXPONENT)) || ((exponentValue * exponentSign) < (2 * MIN_EXPONENT)))
            break;
       }
      resultExponent += exponentValue * exponentSign;
    }

   if (0 == resultSignificand)
    {
      resultExponent = 0;
    }

   if (resultExponent > MAX_EXPONENT) // Flush to infinity?
    {
      if (resultSign) return -sfInf;
      return sfInf;
    }
   if (resultExponent < MIN_EXPONENT) // Flush to zero?
    {
      if (resultSign) return -sfZero;
      return sfZero;
    }

   return SlowFloat(resultSignificand ^ (resultSign ? 0xFFFFFFFFU : 0U), resultExponent);
 }


SlowFloat operator - (const SlowFloat& src)
 {
   return SlowFloat(~src.significand, src.exponent);
 }


SlowFloat operator + (const SlowFloat& lhs, const SlowFloat& rhs)
 {
      // First, handle NaNs, as they have the highest precedence.
   if (isNaN(lhs)) return lhs; // Prefer lhs NaN's payload.
   if (isNaN(rhs)) return rhs;

      // Next, if both are infinities.
   if (isInf(lhs) && isInf(rhs))
    {
      if (getSign(lhs) != getSign(rhs)) return sfNaN; // Inf - Inf = NaN
      return lhs; // Both have the same sign, so just return lhs.
    }
      // Anything else with infinity is infinity
   if (isInf(lhs)) return lhs;
   if (isInf(rhs)) return rhs;

      // Finally, zeros.
   if (isZero(lhs) && isZero(rhs))
    {
      if ((getSign(lhs) && getSign(rhs)) || ((getSign(lhs) != getSign(rhs)) && (mode == ROUND_NEGATIVE_INFINITY)))
         return -sfZero;
      return sfZero; // All other cases return positive zero.
    }
   if (isZero(lhs)) return rhs;
   if (isZero(rhs)) return lhs;

      // Now, we can start the add proper!
   uint64_t lhd = lhs.significand ^ (getSign(lhs) ? 0xFFFFFFFFU : 0U);
   uint64_t rhd = rhs.significand ^ (getSign(rhs) ? 0xFFFFFFFFU : 0U);

      // Normalize exponents
   int32_t resultExponent = lhs.exponent;
   int16_t expDiff = 0;
   if (lhs.exponent > rhs.exponent)
    {
      if ((lhs.exponent - rhs.exponent) <= (CUTOFF + 1))
       {
         expDiff = lhs.exponent - rhs.exponent;
         if (expDiff > 2) // 2 : guard digit, rounding digit / sticky digit
          {
            uint64_t removed;
            removed = rhd % makeShift[(expDiff - 2) + 1];
            rhd /= makeShift[(expDiff - 2) + 1];
            if ((0U != removed) && (0U == (rhd % 5U))) ++rhd;
            expDiff = 2;
          }
         lhd *= makeShift[expDiff + 1];
       }
      else
       {
         expDiff = 2;
         lhd *= 100;
         rhd = 1;
       }
    }
   else if (lhs.exponent < rhs.exponent)
    {
      resultExponent = rhs.exponent;
      if ((rhs.exponent - lhs.exponent) <= (CUTOFF + 1))
       {
         expDiff = rhs.exponent - lhs.exponent;
         if (expDiff > 2)
          {
            uint64_t removed;
            removed = lhd % makeShift[(expDiff - 2) + 1];
            lhd /= makeShift[(expDiff - 2) + 1];
            if ((0U != removed) && (0U == (lhd % 5U))) ++lhd;
            expDiff = 2;
          }
         rhd *= makeShift[expDiff + 1];
       }
      else
       {
         expDiff = 2;
         rhd *= 100;
         lhd = 1;
       }
    }

      // Compute the digits and sign.
   bool resultSign = getSign(lhs);
   if (getSign(lhs) == getSign(rhs)) // Both same sign
    {
      lhd += rhd;
    }
   else // Different Signs
    {
      if (lhd >= rhd) // lhs larger : result sign of lhs
         lhd -= rhd;
      else // rhs larger : result sign of rhs (opposite sign of lhs)
       {
         lhd = rhd - lhd;
         resultSign = !resultSign;
       }
    }

      // Normalize the result
   if (0 == lhd)
    {
      if (mode != ROUND_NEGATIVE_INFINITY) resultSign = false;
      else resultSign = true;
      resultExponent = 0;
    }
   else if (lhd > MAX_SIGNIFICAND)
    {
      if (0 == expDiff) // Did overflow occur? ie 9 + 9
       {
         ++resultExponent;
       }
      uint64_t temp = makeShift[expDiff]; // This is why the table has one extra entry: we want the previous entry here.
      uint64_t test = BIAS * temp;
      if (lhd < test) // Did destructive cancellation occur? ie 100 - 1.
       {
         --resultExponent;
       }
      else
       {
         temp *= 10;
         test *= 10;
         if (lhd >= test) // Did overflow occur? ie 99 + 1
          {
            ++resultExponent;
            temp *= 10;
          }
       }
      uint64_t rem = lhd % temp;
      lhd /= temp;
      if (decideRound(resultSign, 0 == (lhd & 1), static_cast<int64_t>(temp) - static_cast<int64_t>(rem << 1), rem == 0))
       {
         ++lhd;
         if (lhd == BIAS)
          {
            lhd = MIN_SIGNIFICAND;
            ++resultExponent;
          }
       }
      if (resultExponent > MAX_EXPONENT) // Flush to infinity?
       {
         lhd = 0;
         resultExponent = SPECIAL_EXPONENT;
       }
    }
   else
    {
      if (expDiff == 1) // If we have gotten here, and an operation like 10 - 1 occurred, then fix the exponent.
       {
         --resultExponent;
       }
      while (lhd < MIN_SIGNIFICAND)
       {
         lhd *= 10;
         --resultExponent;
       }
      if (resultExponent < MIN_EXPONENT) // Flush to zero?
       {
         lhd = 0;
         resultExponent = 0;
       }
    }

   return SlowFloat(lhd ^ (resultSign ? 0xFFFFFFFFU : 0U), resultExponent);
 }

SlowFloat operator - (const SlowFloat& lhs, const SlowFloat& rhs)
 {
      // Only handle NaNs, as we don't want to mutate the payload of an rhs NaN.
   if (isNaN(lhs)) return lhs; // Prefer lhs NaN's payload.
   if (isNaN(rhs)) return rhs;

   return lhs + -rhs; // Given the "stupid" implementation of unary operator -, I believe this will be correct.
 }

SlowFloat operator * (const SlowFloat& lhs, const SlowFloat& rhs)
 {
   bool resultSign = getSign(lhs) != getSign(rhs);

      // First, handle NaNs, as they have the highest precedence.
   if (isNaN(lhs)) return lhs; // Prefer lhs NaN's payload.
   if (isNaN(rhs)) return rhs;

      // Next, infinity times zero is NaN.
   if ((isInf(lhs) && isZero(rhs)) || (isZero(lhs) && isInf(rhs)))
    {
      if (resultSign) return -sfNaN; // Preserve the sign of NaN ...
      return sfNaN;
    }

      // Next, if either are infinities.
   if (isInf(lhs) || isInf(rhs))
    {
      if (resultSign) return -sfInf; // Preserve the sign of infinity.
      return sfInf;
    }

      // Finally, zeros.
   if (isZero(lhs) || isZero(rhs))
    {
      if (resultSign) return -sfZero;
      return sfZero;
    }

      // Compute the exponent of the result, and use it to decide if we are even multiplying.
   int32_t resultExponent = static_cast<int32_t>(lhs.exponent) + rhs.exponent;
   if (resultExponent > MAX_EXPONENT)
    {
      if (resultSign) return -sfInf;
      return sfInf;
    }
   else if (resultExponent < (MIN_EXPONENT - 1))
    {
      if (resultSign) return -sfZero;
      return sfZero;
    }

      // Now, we can start the multiply proper!
   uint64_t lhd = lhs.significand ^ (getSign(lhs) ? 0xFFFFFFFFU : 0U);
   uint64_t rhd = rhs.significand ^ (getSign(rhs) ? 0xFFFFFFFFU : 0U);

   lhd *= rhd;

   uint64_t temp = MIN_SIGNIFICAND;
   if (lhd >= (static_cast<uint64_t>(BIAS) * MIN_SIGNIFICAND))
    {
      temp *= 10;
      ++resultExponent;
    }

   uint64_t rem = lhd % temp;
   lhd /= temp;
   if (decideRound(resultSign, 0 == (lhd & 1), static_cast<int64_t>(temp) - static_cast<int64_t>(rem << 1), rem == 0))
    {
      ++lhd; // This can't cause an overflow.
    }

   if (resultExponent > MAX_EXPONENT) // Flush to infinity?
    {
      if (resultSign) return -sfInf;
      return sfInf;
    }
   if (resultExponent < MIN_EXPONENT) // Were we unsuccessful in saving a result from flushing to zero?
    {
      if (resultSign) return -sfZero;
      return sfZero;
    }

   return SlowFloat(lhd ^ (resultSign ? 0xFFFFFFFFU : 0U), resultExponent);
 }

SlowFloat operator / (const SlowFloat& lhs, const SlowFloat& rhs)
 {
   bool resultSign = getSign(lhs) != getSign(rhs);

      // First, handle NaNs, as they have the highest precedence.
   if (isNaN(lhs)) return lhs; // Prefer lhs NaN's payload.
   if (isNaN(rhs)) return rhs;

      // Next, zero divided by zero or infinity divided by infinity is NaN.
   if ((isZero(lhs) && isZero(rhs)) || (isInf(lhs) && isInf(rhs)))
    {
      if (resultSign) return -sfNaN; // Preserve the sign of NaN ...
      return sfNaN;
    }

      // Next, if the result is infinity.
   if (isInf(lhs) || isZero(rhs))
    {
      if (resultSign) return -sfInf;
      return sfInf;
    }

      // Finally, if the result is zero.
   if (isZero(lhs) || isInf(rhs))
    {
      if (resultSign) return -sfZero;
      return sfZero;
    }

      // Compute the exponent of the result, and use it to decide if we are even dividing.
   int32_t resultExponent = static_cast<int32_t>(lhs.exponent) - rhs.exponent;
   if (resultExponent > (MAX_EXPONENT + 1))
    {
      if (resultSign) return -sfInf;
      return sfInf;
    }
   else if (resultExponent < MIN_EXPONENT)
    {
      if (resultSign) return -sfZero;
      return sfZero;
    }

      // Now, we can start the divide proper!
   uint64_t lhd = lhs.significand ^ (getSign(lhs) ? 0xFFFFFFFFU : 0U);
   uint64_t rhd = rhs.significand ^ (getSign(rhs) ? 0xFFFFFFFFU : 0U);

   if (lhd >= rhd)
    {
      rhd *= 10;
    }
   else
    {
      --resultExponent;
    }
   lhd *= BIAS;
   uint64_t rem = lhd % rhd;
   lhd = lhd / rhd;

   if (decideRound(resultSign, 0 == (lhd & 1), static_cast<int64_t>(rhd) - static_cast<int64_t>(rem << 1), rem == 0))
    {
      ++lhd; // This can never cause an overflow.
    }

   if (resultExponent > MAX_EXPONENT) // Were we unsuccessful in saving a result from flushing to infinity?
    {
      if (resultSign) return -sfInf;
      return sfInf;
    }
   if (resultExponent < MIN_EXPONENT) // Flush to zero?
    {
      if (resultSign) return -sfZero;
      return sfZero;
    }

   return SlowFloat(lhd ^ (resultSign ? 0xFFFFFFFFU : 0U), resultExponent);
 }


bool operator > (const SlowFloat& lhs, const SlowFloat& rhs)
 {
   if (isNaN(lhs) || isNaN(rhs)) return false;
   if (isInf(lhs) || isInf(rhs))
    {
      if (isInf(lhs) && isInf(rhs))
       {
         if (getSign(lhs) == getSign(rhs)) return false;
         else if (getSign(lhs)) return false;
         else return true;
       }
      else if (isInf(lhs)) return !getSign(lhs);
      else return getSign(rhs);
    }
   if (isZero(lhs) || isZero(rhs))
    {
      if (isZero(lhs) && isZero(rhs)) return false;
      else if (isZero(lhs)) return getSign(rhs);
      else return !getSign(lhs);
    }
   if (getSign(lhs) != getSign(rhs)) return getSign(rhs);
   if (lhs.exponent != rhs.exponent) return getSign(lhs) ^ (lhs.exponent > rhs.exponent);
   return lhs.significand > rhs.significand;
 }

bool operator < (const SlowFloat& lhs, const SlowFloat& rhs)
 {
   if (isNaN(lhs) || isNaN(rhs)) return false;
   if (isInf(lhs) || isInf(rhs))
    {
      if (isInf(lhs) && isInf(rhs))
       {
         if (getSign(lhs) == getSign(rhs)) return false;
         else if (getSign(lhs)) return true;
         else return false;
       }
      else if (isInf(lhs)) return getSign(lhs);
      else return !getSign(rhs);
    }
   if (isZero(lhs) || isZero(rhs))
    {
      if (isZero(lhs) && isZero(rhs)) return false;
      else if (isZero(lhs)) return !getSign(rhs);
      else return getSign(lhs);
    }
   if (getSign(lhs) != getSign(rhs)) return getSign(lhs);
   if (lhs.exponent != rhs.exponent) return getSign(lhs) ^ (lhs.exponent < rhs.exponent);
   return lhs.significand < rhs.significand;
 }

bool operator >= (const SlowFloat& lhs, const SlowFloat& rhs)
 {
   if (isNaN(lhs) || isNaN(rhs)) return false;
   if (isInf(lhs) || isInf(rhs))
    {
      if (isInf(lhs) && isInf(rhs))
       {
         if (getSign(lhs) == getSign(rhs)) return true;
         else if (getSign(lhs)) return false;
         else return true;
       }
      else if (isInf(lhs)) return !getSign(lhs);
      else return getSign(rhs);
    }
   if (isZero(lhs) || isZero(rhs))
    {
      if (isZero(lhs) && isZero(rhs)) return true;
      else if (isZero(lhs)) return getSign(rhs);
      else return !getSign(lhs);
    }
   if (getSign(lhs) != getSign(rhs)) return getSign(rhs);
   if (lhs.exponent != rhs.exponent) return getSign(lhs) ^ (lhs.exponent > rhs.exponent);
   return lhs.significand >= rhs.significand;
 }

bool operator <= (const SlowFloat& lhs, const SlowFloat& rhs)
 {
   if (isNaN(lhs) || isNaN(rhs)) return false;
   if (isInf(lhs) || isInf(rhs))
    {
      if (isInf(lhs) && isInf(rhs))
       {
         if (getSign(lhs) == getSign(rhs)) return true;
         else if (getSign(lhs)) return true;
         else return false;
       }
      else if (isInf(lhs)) return getSign(lhs);
      else return !getSign(rhs);
    }
   if (isZero(lhs) || isZero(rhs))
    {
      if (isZero(lhs) && isZero(rhs)) return true;
      else if (isZero(lhs)) return !getSign(rhs);
      else return getSign(lhs);
    }
   if (getSign(lhs) != getSign(rhs)) return getSign(lhs);
   if (lhs.exponent != rhs.exponent) return getSign(lhs) ^ (lhs.exponent < rhs.exponent);
   return lhs.significand <= rhs.significand;
 }

bool operator == (const SlowFloat& lhs, const SlowFloat& rhs)
 {
   if (isNaN(lhs) || isNaN(rhs)) return false;
   if (isZero(lhs) || isZero(rhs))
    {
      if (isZero(lhs) && isZero(rhs)) return true;
      else return false;
    }
   return (lhs.significand == rhs.significand) && (lhs.exponent == rhs.exponent);
 }

bool operator != (const SlowFloat& lhs, const SlowFloat& rhs)
 {
   if (isNaN(lhs) || isNaN(rhs)) return true;
   if (isZero(lhs) || isZero(rhs))
    {
      if (isZero(lhs) && isZero(rhs)) return false;
      else return true;
    }
   return (lhs.significand != rhs.significand) || (lhs.exponent != rhs.exponent);
 }

 }

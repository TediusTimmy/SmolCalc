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

#include <ctype.h>
#include <inttypes.h>
#include <math.h> // Needed for constants such as FP_NORMAL

#include "dm_double_m.h"
#include "dm_muldiv.h"

const int16_t SPECIAL_EXPONENT = -512;
const int16_t MAX_EXPONENT = 511;
const int16_t MIN_EXPONENT = -MAX_EXPONENT;
const uint64_t DM_INFINITY = 0x1FFFFFFFFFFFFFULL;
const uint64_t SIGN_BIT =  0x8000000000000000ULL;
const uint64_t EXPONENT_FIELD = 1023ULL; // MAX_EXPONENT - SPECIAL_EXPONENT

const uint64_t BIAS =           10000000000000000ULL;
const uint64_t MAX_SIGNIFICAND = 9999999999999999ULL;
const uint64_t MIN_SIGNIFICAND = 1000000000000000ULL;
const int16_t CUTOFF = 16; // Difference in exponents where we consider the operation degenerate.

dm_double DM_DOUBLE_PACK(uint64_t sign, int16_t exponent, uint64_t significand)
 {
   int16_t real_exponent = exponent - SPECIAL_EXPONENT;
   uint64_t use_exponent = (uint64_t)real_exponent;
   return (use_exponent << 53) | (significand - MIN_SIGNIFICAND) | (!!sign ? SIGN_BIT : 0U);
 }
dm_double DM_DOUBLE_PACK_ALT(uint64_t sign, int16_t exponent, uint64_t significand)
 {
   int16_t real_exponent = exponent - SPECIAL_EXPONENT;
   uint64_t use_exponent = (uint64_t)real_exponent;
   return (use_exponent << 53) | significand | (!!sign ? SIGN_BIT : 0U);
 }

int16_t DM_DOUBLE_UNPACK_EXPONENT(dm_double value)
 {
   uint64_t use_exponent = (value >> 53) & EXPONENT_FIELD;
   int16_t real_exponent = (int16_t)use_exponent;
   return real_exponent + SPECIAL_EXPONENT;
 }
uint64_t DM_DOUBLE_UNPACK_SIGNIFICAND(dm_double value)
 {
   return (value & DM_INFINITY) + MIN_SIGNIFICAND;
 }
uint64_t DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(dm_double value)
 {
   return value & DM_INFINITY;
 }

const dm_double dm_double_Zero = 0U;
const dm_double dm_double_NaN  = 255U;
const dm_double dm_double_Inf  = DM_INFINITY;

int dm_double_fpclassify(dm_double arg)
 {
   int result;
   int16_t exponent = DM_DOUBLE_UNPACK_EXPONENT(arg);
   if (SPECIAL_EXPONENT != exponent)
    {
      result = FP_NORMAL;
    }
   else
    {
      uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(arg);
      if (DM_INFINITY == significand)
       {
         result = FP_INFINITE;
       }
      else if (0U == significand)
       {
         result = FP_ZERO;
       }
      else
       {
         result = FP_NAN;
       }
    }
   return result;
 }

int dm_double_iscanonical(dm_double arg)
 {
   int result = 1;
   if (FP_NORMAL == dm_double_fpclassify(arg))
    {
      uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
      if (MAX_SIGNIFICAND < significand)
       {
         result = 0;
       }
    }
   return result;
 }

int dm_double_isfinite(dm_double arg)
 {
   int clasS = dm_double_fpclassify(arg);
   return ((FP_INFINITE != clasS) && (FP_NAN != clasS));
 }

int dm_double_isinf(dm_double arg)
 {
   return DM_INFINITY == (arg & ~SIGN_BIT);
 }

int dm_double_isnan(dm_double arg)
 {
   return FP_NAN == dm_double_fpclassify(arg);
 }

int dm_double_isnormal(dm_double arg)
 {
   return FP_NORMAL == dm_double_fpclassify(arg);
 }

int dm_double_signbit(dm_double arg)
 {
   return (arg & SIGN_BIT) ? 1 : 0;
 }

int dm_double_issubnormal(dm_double arg)
 {
   (void) arg; // Suppress unused.
   return 0;
 }

int dm_double_iszero(dm_double arg)
 {
   return 0U == (arg & ~SIGN_BIT);
 }

 /* It is tempting to use the sign bit as the "signaling" bit,
    but we'll not raise any exceptions. */
int dm_double_issignaling(dm_double arg)
 {
   (void) arg; // Suppress unused.
   return 0;
 }

int dm_double_isgreater(dm_double lhs, dm_double rhs)
 {
   int result;
   if (dm_double_isnan(lhs) || dm_double_isnan(rhs))
    {
      result = 0;
    }
   else if (dm_double_isinf(lhs) || dm_double_isinf(rhs))
    {
      if (!!dm_double_isinf(lhs))
       {
         if (dm_double_isinf(rhs) && (dm_double_signbit(lhs) == dm_double_signbit(rhs)))
          {
            result = 0;
          }
         else
          {
            result = !dm_double_signbit(lhs);
          }
       }
      else
       {
         result = dm_double_signbit(rhs);
       }
    }
      // Assume exponent of zero is most negative number.
   else if (dm_double_iszero(lhs) && dm_double_iszero(rhs))
    {
      result = 0;
    }
   else if (dm_double_signbit(lhs) != dm_double_signbit(rhs))
    {
      result = dm_double_signbit(rhs);
    }
   else if (DM_DOUBLE_UNPACK_EXPONENT(lhs) != DM_DOUBLE_UNPACK_EXPONENT(rhs))
    {
      result = dm_double_signbit(lhs) ^ (DM_DOUBLE_UNPACK_EXPONENT(lhs) > DM_DOUBLE_UNPACK_EXPONENT(rhs));
    }
   else if (!dm_double_signbit(lhs))
    {
      result = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(lhs) > DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(rhs);
    }
   else
    {
      result = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(lhs) < DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(rhs);
    }
   return result;
 }

int dm_double_isgreaterequal(dm_double lhs, dm_double rhs)
 {
   int result;
   if (dm_double_isnan(lhs) || dm_double_isnan(rhs))
    {
      result = 0;
    }
   else if (dm_double_isinf(lhs) || dm_double_isinf(rhs))
    {
      if (!!dm_double_isinf(lhs))
       {
         if (dm_double_isinf(rhs) && (dm_double_signbit(lhs) == dm_double_signbit(rhs)))
          {
            result = 1;
          }
         else
          {
            result = !dm_double_signbit(lhs);
          }
       }
      else
       {
         result = dm_double_signbit(rhs);
       }
    }
      // Assume exponent of zero is most negative number.
   else if (dm_double_iszero(lhs) && dm_double_iszero(rhs))
    {
      result = 1;
    }
   else if (dm_double_signbit(lhs) != dm_double_signbit(rhs))
    {
      result = dm_double_signbit(rhs);
    }
   else if (DM_DOUBLE_UNPACK_EXPONENT(lhs) != DM_DOUBLE_UNPACK_EXPONENT(rhs))
    {
      result = dm_double_signbit(lhs) ^ (DM_DOUBLE_UNPACK_EXPONENT(lhs) > DM_DOUBLE_UNPACK_EXPONENT(rhs));
    }
   else if (!dm_double_signbit(lhs))
    {
      result = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(lhs) >= DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(rhs);
    }
   else
    {
      result = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(lhs) <= DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(rhs);
    }
   return result;
 }

int dm_double_isless(dm_double lhs, dm_double rhs)
 {
   int result;
   if (dm_double_isnan(lhs) || dm_double_isnan(rhs))
    {
      result = 0;
    }
   else if (dm_double_isinf(lhs) || dm_double_isinf(rhs))
    {
      if (!!dm_double_isinf(lhs))
       {
         if (dm_double_isinf(rhs) && (dm_double_signbit(lhs) == dm_double_signbit(rhs)))
          {
            result = 0;
          }
         else
          {
            result = dm_double_signbit(lhs);
          }
       }
      else
       {
         result = !dm_double_signbit(rhs);
       }
    }
      // Assume exponent of zero is most negative number.
   else if (dm_double_iszero(lhs) && dm_double_iszero(rhs))
    {
      result = 0;
    }
   else if (dm_double_signbit(lhs) != dm_double_signbit(rhs))
    {
      result = dm_double_signbit(lhs);
    }
   else if (DM_DOUBLE_UNPACK_EXPONENT(lhs) != DM_DOUBLE_UNPACK_EXPONENT(rhs))
    {
      result = dm_double_signbit(lhs) ^ (DM_DOUBLE_UNPACK_EXPONENT(lhs) < DM_DOUBLE_UNPACK_EXPONENT(rhs));
    }
   else if (!dm_double_signbit(lhs))
    {
      result = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(lhs) < DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(rhs);
    }
   else
    {
      result = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(lhs) > DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(rhs);
    }
   return result;
 }

int dm_double_islessequal(dm_double lhs, dm_double rhs)
 {
   int result;
   if (dm_double_isnan(lhs) || dm_double_isnan(rhs))
    {
      result = 0;
    }
   else if (dm_double_isinf(lhs) || dm_double_isinf(rhs))
    {
      if (!!dm_double_isinf(lhs))
       {
         if (dm_double_isinf(rhs) && (dm_double_signbit(lhs) == dm_double_signbit(rhs)))
          {
            result = 1;
          }
         else
          {
            result = dm_double_signbit(lhs);
          }
       }
      else
       {
         result = !dm_double_signbit(rhs);
       }
    }
      // Assume exponent of zero is most negative number.
   else if (dm_double_iszero(lhs) && dm_double_iszero(rhs))
    {
      result = 1;
    }
   else if (dm_double_signbit(lhs) != dm_double_signbit(rhs))
    {
      result = dm_double_signbit(lhs);
    }
   else if (DM_DOUBLE_UNPACK_EXPONENT(lhs) != DM_DOUBLE_UNPACK_EXPONENT(rhs))
    {
      result = dm_double_signbit(lhs) ^ (DM_DOUBLE_UNPACK_EXPONENT(lhs) < DM_DOUBLE_UNPACK_EXPONENT(rhs));
    }
   else if (!dm_double_signbit(lhs))
    {
      result = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(lhs) <= DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(rhs);
    }
   else
    {
      result = DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(lhs) >= DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(rhs);
    }
   return result;
 }

int dm_double_islessgreater(dm_double lhs, dm_double rhs)
 {
   return dm_double_isgreater(lhs, rhs) || dm_double_isless(lhs, rhs);
 }

int dm_double_isunordered(dm_double lhs, dm_double rhs)
 {
   int result = 0;
   if (dm_double_isnan(lhs) || dm_double_isnan(rhs))
    {
      result = 1;
    }
   return result;
 }

int dm_double_isequal(dm_double lhs, dm_double rhs)
 {
   int result;
   if (dm_double_isnan(lhs) || dm_double_isnan(rhs))
    {
      result = 0;
    }
   else if (dm_double_iszero(lhs) && dm_double_iszero(rhs))
    {
      result = 1;
    }
   else
    {
      result = lhs == rhs;
    }
   return result;
 }

int dm_double_isunequal(dm_double lhs, dm_double rhs)
 {
   int result;
   if (dm_double_isnan(lhs) || dm_double_isnan(rhs))
    {
      result = 1;
    }
   else if (dm_double_iszero(lhs) && dm_double_iszero(rhs))
    {
      result = 0;
    }
   else
    {
      result = lhs != rhs;
    }
   return result;
 }

dm_double dm_double_fabs(dm_double arg)
 {
   return arg & ~SIGN_BIT;
 }

dm_double dm_double_fmin(dm_double lhs, dm_double rhs)
 {
   dm_double result;
   if (!!dm_double_isnan(rhs))
    {
      result = lhs;
    }
   else if (!!dm_double_isnan(lhs))
    {
      result = rhs;
    }
   else
    {
      result = dm_double_islessequal(lhs, rhs) ? lhs : rhs;
    }
   return result;
 }

dm_double dm_double_fmax(dm_double lhs, dm_double rhs)
 {
   dm_double result;
   if (!!dm_double_isnan(rhs))
    {
      result = lhs;
    }
   else if (!!dm_double_isnan(lhs))
    {
      result = rhs;
    }
   else
    {
      result = dm_double_isgreaterequal(lhs, rhs) ? lhs : rhs;
    }
   return result;
 }

dm_double dm_double_copysign(dm_double mag, dm_double sgn)
 {
   return (mag & ~SIGN_BIT) | (sgn & SIGN_BIT);
 }

   // Table of powers of ten up to CUTOFF.
   // (Yes, there is an extra entry, for the -1 case that needs to return 1.)
   // There's also an extra entry for 17, for the rounding case when all digits are rounded.
const uint64_t makeShift [] =
 {
   1ULL, // -1
   1ULL, // 0
                  10ULL,               100ULL,           1000ULL,           10000ULL,           100000ULL, // 5
             1000000ULL,          10000000ULL,      100000000ULL,      1000000000ULL,      10000000000ULL, // 10
        100000000000ULL,     1000000000000ULL, 10000000000000ULL, 100000000000000ULL, 1000000000000000ULL, // 15
   10000000000000000ULL, 10000000000000000ULL // 16
 };

int dm_global_round_mode = DM_FE_TONEAREST;

int dm_fesetround(int round_mode)
 {
   if ((DM_FE_TONEAREST <= round_mode) && (DM_FE_FROMZERO >= round_mode))
    {
      dm_global_round_mode = round_mode;
    }
   return dm_global_round_mode;
 }

int dm_fegetround()
 {
   return dm_global_round_mode;
 }

   // This code is suspiciously familiar....
// sign - sign of result (non-zero is negative)
// sodd - whether the current significand is even : significand & 1
// comp - divisor - 2 * remainder : this will be positive if we should round down, negative if we should round up, and zero if we are exactly in the middle
// zero - whether the remainder is zero : one if the division was exact (if one, comp must be positive)
// returns whether the unsigned significand should be incremented away from zero
uint64_t dm_decideRound(int sign, int sodd, int64_t comp, int zero, int round_mode)
 {
   uint64_t result = 0U;
   switch (round_mode)
    {
      case DM_FE_TONEAREST:
         if ((comp < 0) || ((comp == 0) && !!sodd))
          {
            result = 1U;
          }
         break;
      case DM_FE_TONEARESTFROMZERO:
         if (comp <= 0)
          {
            result = 1U;
          }
         break;
      case DM_FE_UPWARD:
         if (!sign && !zero)
          {
            result = 1U;
          }
         break;
      case DM_FE_DOWNWARD:
         if (!!sign && !zero)
          {
            result = 1U;
          }
         break;
      case DM_FE_TOWARDZERO:
         break;
      case DM_FE_TONEARESTODD:
         if ((comp < 0) || ((comp == 0) && !sodd))
          {
            result = 1U;
          }
         break;
      case DM_FE_TONEARESTTOWARDZERO:
         if (comp < 0)
          {
            result = 1U;
          }
         break;
      case DM_FE_FROMZERO:
         if (!zero)
          {
            result = 1U;
          }
         break;
      default:
         // This case should never happen and indicates a programming error.
         break;
    }
   return result;
 }

dm_double dm_double_ceil(dm_double arg)
 {
   dm_double result;
   int16_t exponent = DM_DOUBLE_UNPACK_EXPONENT(arg);
   if (SPECIAL_EXPONENT == exponent)
    {
      result = arg;
    }
   else if (0 > exponent)
    {
      if (0U != (arg & SIGN_BIT))
       {
         result = dm_double_Zero | SIGN_BIT;
       }
      else
       {
         result = DM_DOUBLE_PACK(0, 0, MIN_SIGNIFICAND);
       }
    }
   else if (exponent >= CUTOFF)
    {
      result = arg;
    }
   else
    {
      uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
      uint64_t shift = makeShift[CUTOFF - exponent];
      if (0U != (arg & SIGN_BIT))
       {
         significand = significand - (significand % shift);
       }
      else
       {
         uint64_t residue = significand % shift;
         significand /= shift;
         if (0U != residue)
          {
            ++significand;
          }
         significand *= shift;
         if (BIAS == significand)
          {
            significand = MIN_SIGNIFICAND;
            ++exponent; // Cannot round to infinity.
          }
       }
      result = DM_DOUBLE_PACK(arg & SIGN_BIT, exponent, significand);
    }
   return result;
 }

dm_double dm_double_floor(dm_double arg)
 {
   dm_double result;
   int16_t exponent = DM_DOUBLE_UNPACK_EXPONENT(arg);
   if (SPECIAL_EXPONENT == exponent)
   {
      result = arg;
   }
   else if (0 > exponent)
    {
      if (0U != (arg & SIGN_BIT))
       {
         result = DM_DOUBLE_PACK(1, 0, MIN_SIGNIFICAND);
       }
      else
       {
         result = dm_double_Zero;
       }
    }
   else if (exponent >= CUTOFF)
    {
      result = arg;
    }
   else
    {
      uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
      uint64_t shift = makeShift[CUTOFF - exponent];
      if (!(arg & SIGN_BIT))
       {
         significand = significand - (significand % shift);
       }
      else
       {
         uint64_t residue = significand % shift;
         significand /= shift;
         if (0U != residue)
          {
            ++significand;
          }
         significand *= shift;
         if (BIAS == significand)
          {
            significand = MIN_SIGNIFICAND;
            ++exponent; // Cannot round to infinity.
          }
       }
      result = DM_DOUBLE_PACK(arg & SIGN_BIT, exponent, significand);
    }
   return result;
 }

dm_double dm_double_trunc(dm_double arg)
 {
   dm_double result;
   int16_t exponent = DM_DOUBLE_UNPACK_EXPONENT(arg);
   if (SPECIAL_EXPONENT == exponent)
    {
      result = arg;
    }
   else if (0 > exponent)
    {
      result = dm_double_Zero | (arg & SIGN_BIT);
    }
   else if (exponent >= CUTOFF)
    {
      result = arg;
    }
   else
    {
      uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
      uint64_t shift = makeShift[CUTOFF - exponent];
      significand = significand - (significand % shift);
      result = DM_DOUBLE_PACK(arg & SIGN_BIT, exponent, significand);
    }
   return result;
 }

dm_double dm_double_nearbyint(dm_double arg)
 {
   dm_double result;
   int16_t exponent = DM_DOUBLE_UNPACK_EXPONENT(arg);
   if (SPECIAL_EXPONENT == exponent)
    {
      result = arg;
    }
   else if (-1 > exponent)
    {
      result = dm_double_Zero | (arg & SIGN_BIT);
    }
   else if (exponent >= CUTOFF)
    {
      result = arg;
    }
   else
    {
      uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
      uint64_t shift = makeShift[CUTOFF - exponent];
      uint64_t residue = significand % shift;
      significand /= shift;
      significand += dm_decideRound(arg & SIGN_BIT, significand & 1U, shift - (residue << 1), 0U == residue, dm_global_round_mode);
      significand *= shift;
      if (BIAS == significand)
       {
         significand = MIN_SIGNIFICAND;
         ++exponent; // Cannot round to infinity.
       }
      if (0U != significand)
       {
         result = DM_DOUBLE_PACK(arg & SIGN_BIT, exponent, significand);
       }
      else
       {
         result = dm_double_Zero | (arg & SIGN_BIT);
       }
    }
   return result;
 }

dm_double dm_double_round(dm_double arg)
 {
   dm_double result;
   int16_t exponent = DM_DOUBLE_UNPACK_EXPONENT(arg);
   if (SPECIAL_EXPONENT == exponent)
    {
      result = arg;
    }
   else if (-1 > exponent)
    {
      result = dm_double_Zero | (arg & SIGN_BIT);
    }
   else if (exponent >= CUTOFF)
    {
      result = arg;
    }
   else
    {
      uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
      uint64_t shift = makeShift[CUTOFF - exponent];
      uint64_t residue = significand % shift;
      significand /= shift;
      if (shift <= (residue << 1))
       {
         ++significand;
       }
      significand *= shift;
      if (BIAS == significand)
       {
         significand = MIN_SIGNIFICAND;
         ++exponent; // Cannot round to infinity.
       }
      if (0U != significand)
       {
         result = DM_DOUBLE_PACK(arg & SIGN_BIT, exponent, significand);
       }
      else
       {
         result = dm_double_Zero | (arg & SIGN_BIT);
       }
    }
   return result;
 }

dm_double dm_double_roundeven(dm_double arg)
 {
   dm_double result;
   int16_t exponent = DM_DOUBLE_UNPACK_EXPONENT(arg);
   if (SPECIAL_EXPONENT == exponent)
    {
      result = arg;
    }
   else if (-1 > exponent)
    {
      result = dm_double_Zero | (arg & SIGN_BIT);
    }
   else if (exponent >= CUTOFF)
    {
      result = arg;
    }
   else
    {
      uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
      uint64_t shift = makeShift[CUTOFF - exponent];
      uint64_t residue = significand % shift;
      significand /= shift;
      if (shift < (residue << 1))
       {
         ++significand;
       }
      else if ((shift == (residue << 1)) && (significand & 1U))
       {
         ++significand; // Round halfway cases to even
       }
      else
       {
         // Not a rounding case
       }
      significand *= shift;
      if (BIAS == significand)
       {
         significand = MIN_SIGNIFICAND;
         ++exponent; // Cannot round to infinity.
       }
      if (0U != significand)
       {
         result = DM_DOUBLE_PACK(arg & SIGN_BIT, exponent, significand);
       }
      else
       {
         result = dm_double_Zero | (arg & SIGN_BIT);
       }
    }
   return result;
 }

dm_double dm_double_modf(dm_double value, dm_double* intpart)
 {
   dm_double result;
   if (SPECIAL_EXPONENT == DM_DOUBLE_UNPACK_EXPONENT(value))
    {
      *intpart = value;
      if (DM_INFINITY != DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(value))
       {
         result = value;
       }
      else
       {
         result = dm_double_Zero | (value & SIGN_BIT);
       }
    }
   else
    {
      *intpart = dm_double_trunc(value);
      result = dm_double_sub(value, *intpart);
    }
   return result;
 }

dm_double dm_double_frexp(dm_double value, int* exponent)
 {
   *exponent = DM_DOUBLE_UNPACK_EXPONENT(value);
   return DM_DOUBLE_PACK_ALT(value & SIGN_BIT, 0, DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(value));
 }

dm_double dm_double_ldexp(dm_double value, int exponent)
 {
   dm_double result;
   int16_t old_exponent = DM_DOUBLE_UNPACK_EXPONENT(value);
   if (((MAX_EXPONENT * 2) < exponent) || (MAX_EXPONENT < (exponent + old_exponent)))
    {
      result = dm_double_Inf | (value & SIGN_BIT);
    }
   else if (((MIN_EXPONENT * 2) > exponent) || (MIN_EXPONENT > (exponent + old_exponent)))
    {
      result = dm_double_Zero | (value & SIGN_BIT);
    }
   else
    {
      result = DM_DOUBLE_PACK_ALT(value & SIGN_BIT, exponent + old_exponent, DM_DOUBLE_UNPACK_SIGNIFICAND_ALT(value));
    }
   return result;
 }

dm_double dm_double_neg(dm_double arg)
 {
   return arg ^ SIGN_BIT;
 }

dm_double dm_double_add(dm_double lhs, dm_double rhs)
 {
   return dm_double_add_r(lhs, rhs, dm_global_round_mode);
 }

dm_double dm_double_sub(dm_double lhs, dm_double rhs)
 {
   dm_double result;
      // Only handle NaNs, as we don't want to mutate the payload of an rhs NaN.
   if (!!dm_double_isnan(lhs))
    {
      result = lhs; // Prefer lhs NaN's payload.
    }
   else if (!!dm_double_isnan(rhs))
    {
      result = rhs;
    }
   else
    {
      result = dm_double_add_r(lhs, dm_double_neg(rhs), dm_global_round_mode);
    }
   return result;
 }

dm_double dm_double_mul(dm_double lhs, dm_double rhs)
 {
   return dm_double_mul_r(lhs, rhs, dm_global_round_mode);
 }

dm_double dm_double_div(dm_double lhs, dm_double rhs)
 {
   return dm_double_div_r(lhs, rhs, dm_global_round_mode);
 }

uint64_t dm_internal_round(uint64_t quot, uint64_t rem)
 {
   uint64_t result = 0U;
   if ((0U != rem) && (0U == (quot % 5U))) // Remainder mod 10 is 0 or 5
    {
      result = 1U;
    }
   return result;
 }

dm_double dm_double_add_r(dm_double lhs, dm_double rhs, int round_mode)
 {
   dm_double result;
      // First, handle NaNs, as they have the highest precedence.
   if (!!dm_double_isnan(lhs))
    {
      result = lhs; // Prefer lhs NaN's payload.
    }
   else if (!!dm_double_isnan(rhs))
    {
      result = rhs;
    }

      // Next, if both are infinities.
   else if (dm_double_isinf(lhs) && dm_double_isinf(rhs))
    {
      if (dm_double_signbit(lhs) != dm_double_signbit(rhs))
       {
         result = dm_double_NaN; // Inf - Inf = NaN
       }
      else
       {
         result = lhs; // Both have the same sign, so just return lhs.
       }
    }
      // Anything else with infinity is infinity
   else if (!!dm_double_isinf(lhs))
    {
      result = lhs;
    }
   else if (!!dm_double_isinf(rhs))
    {
      result = rhs;
    }

      // Finally, zeros.
   else if (dm_double_iszero(lhs) && dm_double_iszero(rhs))
    {
      if ((dm_double_signbit(lhs) && dm_double_signbit(rhs)) || ((dm_double_signbit(lhs) != dm_double_signbit(rhs)) && (DM_FE_DOWNWARD == round_mode)))
       {
         result = dm_double_neg(dm_double_Zero);
       }
      else
       {
         result = dm_double_Zero; // All other cases return positive zero.
       }
    }
   else if (!!dm_double_iszero(lhs))
    {
      result = rhs;
    }
   else if (!!dm_double_iszero(rhs))
    {
      result = lhs;
    }

   else
    {
         // Now, we can start the add proper!
      uint64_t lhd = DM_DOUBLE_UNPACK_SIGNIFICAND(lhs);
      uint64_t rhd = DM_DOUBLE_UNPACK_SIGNIFICAND(rhs);
      uint64_t resultSignificand;

         // Normalize exponents
      int16_t lhse = DM_DOUBLE_UNPACK_EXPONENT(lhs);
      int16_t rhse = DM_DOUBLE_UNPACK_EXPONENT(rhs);
      int32_t resultExponent = lhse;
      int16_t expDiff = 0;
      if (lhse > rhse)
       {
         if ((lhse - rhse) <= (CUTOFF + 1))
          {
            expDiff = lhse - rhse;
            if (expDiff > 2) // 2 : guard digit, rounding digit / sticky digit
             {
               uint64_t removed;
               removed = rhd % makeShift[(expDiff - 2) + 1];
               rhd /= makeShift[(expDiff - 2) + 1];
               rhd += dm_internal_round(rhd, removed);
               expDiff = 2;
             }
            lhd *= makeShift[expDiff + 1];
          }
         else
          {
            expDiff = 2;
            lhd *= 100U;
            rhd = 1U;
          }
       }
      else if (lhse < rhse)
       {
         resultExponent = rhse;
         if ((rhse - lhse) <= (CUTOFF + 1))
          {
            expDiff = rhse - lhse;
            if (expDiff > 2)
             {
               uint64_t removed;
               removed = lhd % makeShift[(expDiff - 2) + 1];
               lhd /= makeShift[(expDiff - 2) + 1];
               lhd += dm_internal_round(lhd, removed);
               expDiff = 2;
             }
            rhd *= makeShift[expDiff + 1];
          }
         else
          {
            expDiff = 2;
            rhd *= 100U;
            lhd = 1U;
          }
       }
      else
       {
         // Exponents are the same
       }

         // Compute the digits and sign.
      int resultSign = dm_double_signbit(lhs);
      if (dm_double_signbit(lhs) == dm_double_signbit(rhs)) // Both same sign
       {
         lhd += rhd;
       }
      else // Different Signs
       {
         if (lhd >= rhd) // lhd >= rhd, lhs larger : result sign of lhs
          {
            lhd -= rhd;
          }
         else // rhs larger : result sign of rhs (opposite sign of lhs)
          {
            lhd = rhd - lhd;
            resultSign = !resultSign;
          }
       }
      resultSignificand = lhd;

         // Normalize the result
      if (0U == lhd)
       {
         if (DM_FE_DOWNWARD != round_mode)
          {
            resultSign = 0;
          }
         else
          {
            resultSign = 1;
          }
         resultExponent = SPECIAL_EXPONENT;
         resultSignificand = MIN_SIGNIFICAND;
       }
      else if (lhd > MAX_SIGNIFICAND)
       {
         if (0 == expDiff) // Did overflow occur? ie 9 + 9
          {
            ++resultExponent;
          }
         uint64_t temp = makeShift[expDiff]; // This is why the table has one extra entry: we want the previous entry here.
         uint64_t test = BIAS * makeShift[expDiff];
         if (lhd < test) // Did destructive cancellation occur? ie 100 - 1.
          {
            --resultExponent;
          }
         else
          {
            temp *= 10U;
            test *= 10U;
            if (lhd >= test) // Did overflow occur? ie 99 + 1
             {
               ++resultExponent;
               temp *= 10U;
             }
          }
         uint64_t rem;
         resultSignificand = lhd / temp;
         rem = lhd % temp;
         resultSignificand += dm_decideRound(resultSign, resultSignificand & 1U, (int64_t)temp - (int64_t)(rem << 1), 0U == rem, round_mode);
         if (resultSignificand == BIAS)
          {
            resultSignificand = MIN_SIGNIFICAND;
            ++resultExponent;
          }
         if (resultExponent > MAX_EXPONENT) // Flush to infinity?
          {
            resultSignificand = DM_INFINITY + MIN_SIGNIFICAND;
            resultExponent = SPECIAL_EXPONENT;
          }
       }
      else
       {
         if (expDiff == 1) // If we have gotten here, and an operation like 10 - 1 occurred, then fix the exponent.
          {
            --resultExponent;
          }
         while (resultSignificand < MIN_SIGNIFICAND)
          {
            resultSignificand *= 10U;
            --resultExponent;
          }
         if (resultExponent < MIN_EXPONENT) // Flush to zero?
          {
            resultSignificand = MIN_SIGNIFICAND;
            resultExponent = SPECIAL_EXPONENT;
          }
       }

      result = DM_DOUBLE_PACK(resultSign, resultExponent, resultSignificand);
    }
   return result;
 }

dm_double dm_double_sub_r(dm_double lhs, dm_double rhs, int round_mode)
 {
   dm_double result;
      // Only handle NaNs, as we don't want to mutate the payload of an rhs NaN.
   if (!!dm_double_isnan(lhs))
   {
      result = lhs; // Prefer lhs NaN's payload.
   }
   else if (!!dm_double_isnan(rhs))
   {
      result = rhs;
   }
   else
   {
      result = dm_double_add_r(lhs, dm_double_neg(rhs), round_mode);
   }
   return result;
 }

dm_double dm_double_mul_r(dm_double lhs, dm_double rhs, int round_mode)
 {
   dm_double result;
   int resultSign = dm_double_signbit(lhs) != dm_double_signbit(rhs);

      // First, handle NaNs, as they have the highest precedence.
   if (!!dm_double_isnan(lhs))
    {
      result = lhs; // Prefer lhs NaN's payload.
    }
   else if (!!dm_double_isnan(rhs))
    {
      result = rhs;
    }

      // Next, infinity times zero is NaN.
   else if ((dm_double_isinf(lhs) && dm_double_iszero(rhs)) || (dm_double_iszero(lhs) && dm_double_isinf(rhs)))
    {
      if (!!resultSign)
       {
         result = dm_double_neg(dm_double_NaN); // Preserve the sign of NaN ...
       }
      else
       {
         result = dm_double_NaN;
       }
    }

      // Next, if either are infinities.
   else if (dm_double_isinf(lhs) || dm_double_isinf(rhs))
    {
      if (!!resultSign)
       {
         result = dm_double_neg(dm_double_Inf); // Preserve the sign of infinity.
       }
      else
       {
         result = dm_double_Inf;
       }
    }

      // Finally, zeros.
   else if (dm_double_iszero(lhs) || dm_double_iszero(rhs))
    {
      if (!!resultSign)
       {
         result = dm_double_neg(dm_double_Zero);
       }
      else
       {
         result = dm_double_Zero;
       }
    }

   else
    {
         // Compute the exponent of the result, and use it to decide if we are even multiplying.
      int16_t resultExponent = DM_DOUBLE_UNPACK_EXPONENT(lhs) + DM_DOUBLE_UNPACK_EXPONENT(rhs);
      if (resultExponent > MAX_EXPONENT)
       {
         if (!!resultSign)
          {
            result = dm_double_neg(dm_double_Inf);
          }
         else
          {
            result = dm_double_Inf;
          }
       }
      else if (resultExponent < (MIN_EXPONENT - 1)) // Minus one in case we can save the result from flushing to zero.
       {
         if (!!resultSign)
          {
            result = dm_double_neg(dm_double_Zero);
          }
         else
          {
            result = dm_double_Zero;
          }
       }
      else
       {
         // Result is in range
            // Now, we can start the multiply proper!
         dm_muldiv_t lhd;
         dm_muldiv_loadFrom(lhd, DM_DOUBLE_UNPACK_SIGNIFICAND(lhs));

         dm_muldiv_mulBy(lhd, DM_DOUBLE_UNPACK_SIGNIFICAND(rhs));

         uint64_t temp = MIN_SIGNIFICAND;
         dm_muldiv_t test;
         dm_muldiv_loadFrom(test, BIAS);
         dm_muldiv_mulBy(test, MIN_SIGNIFICAND);
         if (!dm_muldiv_less(lhd, test)) // lhd >= BIAS * MIN_SIGNIFICAND
          {
            temp *= 10;
            ++resultExponent;
          }

         uint64_t resultSignificand;
         uint64_t rem;
         dm_muldiv_divBy(lhd, temp, &resultSignificand, &rem);
            // This can't cause an overflow.
         resultSignificand += dm_decideRound(resultSign, resultSignificand & 1U, (int64_t)temp - (int64_t)(rem << 1), 0U == rem, round_mode);

         if (resultExponent > MAX_EXPONENT) // Flush to infinity?
          {
            if (!!resultSign)
             {
               result = dm_double_neg(dm_double_Inf);
             }
            else
             {
               result = dm_double_Inf;
             }
          }
         else if (resultExponent < MIN_EXPONENT) // Were we unsuccessful in saving a result from flushing to zero?
          {
            if (!!resultSign)
             {
               result = dm_double_neg(dm_double_Zero);
             }
            else
             {
               result = dm_double_Zero;
             }
          }
         else
          {
            result = DM_DOUBLE_PACK(resultSign, resultExponent, resultSignificand);
          }
       }
    }
   return result;
 }

dm_double dm_double_div_r(dm_double lhs, dm_double rhs, int round_mode)
 {
   dm_double result;
   int resultSign = dm_double_signbit(lhs) != dm_double_signbit(rhs);

      // First, handle NaNs, as they have the highest precedence.
   if (!!dm_double_isnan(lhs))
    {
      result = lhs; // Prefer lhs NaN's payload.
    }
   else if (!!dm_double_isnan(rhs))
    {
      result = rhs;
    }

      // Next, zero divided by zero or infinity divided by infinity is NaN.
   else if ((dm_double_iszero(lhs) && dm_double_iszero(rhs)) || (dm_double_isinf(lhs) && dm_double_isinf(rhs)))
    {
      if (!!resultSign)
       {
         result = dm_double_neg(dm_double_NaN); // Preserve the sign of NaN ...
       }
      else
       {
         result = dm_double_NaN;
       }
    }

      // Next, if the result is infinity.
   else if (dm_double_isinf(lhs) || dm_double_iszero(rhs))
    {
      if (!!resultSign)
       {
         result = dm_double_neg(dm_double_Inf);
       }
      else
       {
         result = dm_double_Inf;
       }
    }

      // Finally, if the result is zero.
   else if (dm_double_iszero(lhs) || dm_double_isinf(rhs))
    {
      if (!!resultSign)
       {
         result = dm_double_neg(dm_double_Zero);
       }
      else
       {
         result = dm_double_Zero;
       }
    }

   else
    {
         // Compute the exponent of the result, and use it to decide if we are even dividing.
      int16_t resultExponent = DM_DOUBLE_UNPACK_EXPONENT(lhs) - DM_DOUBLE_UNPACK_EXPONENT(rhs);
      if (resultExponent > (MAX_EXPONENT + 1)) // Plus one in case we can save the result from flushing to infinity.
       {
         if (!!resultSign)
          {
            result = dm_double_neg(dm_double_Inf);
          }
         else
          {
            result = dm_double_Inf;
          }
       }
      else if (resultExponent < MIN_EXPONENT)
       {
         if (!!resultSign)
          {
            result = dm_double_neg(dm_double_Zero);
          }
         else
          {
            result = dm_double_Zero;
          }
       }
      else
       {
         // Result is in range
            // Now, we can start the divide proper!
         dm_muldiv_t lhd;
         dm_muldiv_loadFrom(lhd, DM_DOUBLE_UNPACK_SIGNIFICAND(lhs));
         uint64_t rhd = DM_DOUBLE_UNPACK_SIGNIFICAND(rhs);

         if (DM_DOUBLE_UNPACK_SIGNIFICAND(lhs) < DM_DOUBLE_UNPACK_SIGNIFICAND(rhs))
          {
            --resultExponent;
          }
         else
          {
            rhd *= 10;
          }

         dm_muldiv_mulBy(lhd, BIAS);
         uint64_t resultSignificand;
         uint64_t rem;
         dm_muldiv_divBy(lhd, rhd, &resultSignificand, &rem);
            // This can't cause an overflow.
         resultSignificand += dm_decideRound(resultSign, resultSignificand & 1U, (int64_t)rhd - (int64_t)(rem << 1), 0U == rem, round_mode);

         if (resultExponent > MAX_EXPONENT) // Were we unsuccessful in saving a result from flushing to infinity?
          {
            if (!!resultSign)
             {
               result = dm_double_neg(dm_double_Inf);
             }
            else
             {
               result = dm_double_Inf;
             }
          }
         else if (resultExponent < MIN_EXPONENT) // Flush to zero?
          {
            if (!!resultSign)
             {
               result = dm_double_neg(dm_double_Zero);
             }
            else
             {
               result = dm_double_Zero;
             }
          }
         else
          {
            result = DM_DOUBLE_PACK(resultSign, resultExponent, resultSignificand);
          }
       }
    }
   return result;
 }

dm_double dm_double_fmod(dm_double lhs, dm_double rhs)
 {
   dm_double result;
   int resultSign = dm_double_signbit(lhs);

      // First, handle NaNs, as they have the highest precedence.
   if (!!dm_double_isnan(lhs))
    {
      result = lhs; // Prefer lhs NaN's payload.
    }
   else if (!!dm_double_isnan(rhs))
    {
      result = rhs;
    }

      // Then, check if the result is NaN.
   else if (dm_double_isinf(lhs) || dm_double_iszero(rhs))
    {
      if (!!resultSign)
       {
         result = dm_double_neg(dm_double_NaN);
       }
      else
       {
         result = dm_double_NaN;
       }
    }

      // Finally, if the result is the lhs argument.
   else if (dm_double_iszero(lhs) || dm_double_isinf(rhs))
    {
      result = lhs;
    }

   else
    {
         // Working variables
      uint64_t resultSignificand = DM_DOUBLE_UNPACK_SIGNIFICAND(lhs);
      uint64_t rhd = DM_DOUBLE_UNPACK_SIGNIFICAND(rhs);
      int32_t resultExponent = DM_DOUBLE_UNPACK_EXPONENT(rhs);
      int32_t expDiff = DM_DOUBLE_UNPACK_EXPONENT(lhs) - resultExponent;

         // Shift-subtract loop to compute the remainder.
         // This algorithm subtracts rhs shifted by 10 ^ x such that the result from subtraction is always exact.
         // So, the remainder is EXACT, just as IEEE-754 says that it ought to be.
         // Remember: we assume that the arguments are exact.
      while ((expDiff >= 0) && (0U != resultSignificand))
       {
         while (resultSignificand >= rhd)
          {
            resultSignificand -= rhd;
          }
         --expDiff;
         if (expDiff >= 0)
          {
            resultSignificand *= 10U;
          }
       }

         // Normalize the result
      if (0U == resultSignificand)
       {
         resultExponent = SPECIAL_EXPONENT;
         resultSignificand = MIN_SIGNIFICAND;
       }
      else if (resultSignificand < MIN_SIGNIFICAND)
       {
         while (resultSignificand < MIN_SIGNIFICAND)
          {
            resultSignificand *= 10U;
            --resultExponent;
          }
         if (resultExponent < MIN_EXPONENT) // Flush to zero?
          {
            resultSignificand = MIN_SIGNIFICAND;
            resultExponent = SPECIAL_EXPONENT;
          }
       }
      else
       {
         // Result is normal.
       }

      result = DM_DOUBLE_PACK(resultSign, resultExponent, resultSignificand);
    }

   return result;
 }

dm_double dm_double_fma(dm_double lhs, dm_double rhs, dm_double ths)
 {
   dm_double result;
   int prodSign = dm_double_signbit(lhs) != dm_double_signbit(rhs);

      // If any argument is Nan, Zero, or Infinity: then the result is the same as the result of the base operations.
      // And that means that if any aren't normal.
   if (!(dm_double_isnormal(lhs) && dm_double_isnormal(rhs) && dm_double_isnormal(ths)))
    {
      result = dm_double_add(dm_double_mul(lhs, rhs), ths);
    }
   else
    {

         // Compute the exponent of the product: we defer flush to infinity to the end.
      int16_t phse = DM_DOUBLE_UNPACK_EXPONENT(lhs) + DM_DOUBLE_UNPACK_EXPONENT(rhs);

         // Now, we can start the multiply proper!
      dm_muldiv_t lhd;
      dm_muldiv_loadFrom(lhd, DM_DOUBLE_UNPACK_SIGNIFICAND(lhs));

      dm_muldiv_mulBy(lhd, DM_DOUBLE_UNPACK_SIGNIFICAND(rhs));

      uint64_t temp = MIN_SIGNIFICAND * 10U;
      dm_muldiv_t test;
      dm_muldiv_loadFrom(test, BIAS);
      dm_muldiv_mulBy(test, MIN_SIGNIFICAND);
      if (!dm_muldiv_less(lhd, test)) // lhd >= BIAS * MIN_SIGNIFICAND
       {
         ++phse;
       }
      else // Normalize the location of the product.
       {
         dm_muldiv_mulBy(lhd, 10U);
       }

      int32_t resultExponent = phse;
      uint64_t resultSignificand;
      int resultSign = prodSign;

      int16_t thse = DM_DOUBLE_UNPACK_EXPONENT(ths);
      int16_t expDiff = phse - thse;
      if (expDiff < 0)
       {
         expDiff = -expDiff;
       }
         // If we are not the special case (the case where we have to do a full subtraction)
         // If the exponents are equal and the signs different, we may need all of the digits in order to handle cancellation.
      if (!((expDiff < 2) && (prodSign != dm_double_signbit(ths))))
       {
         uint64_t thd = DM_DOUBLE_UNPACK_SIGNIFICAND(ths) * 100U;

         uint64_t phd;
         uint64_t rem;
         dm_muldiv_divBy(lhd, temp / 100U, &phd, &rem);
         phd += dm_internal_round(phd, rem);

         if (phse > thse)
          {
            if (expDiff <= (CUTOFF + 1))
             {
               uint64_t removed;
               removed = thd % makeShift[expDiff + 1];
               thd /= makeShift[expDiff + 1];
               thd += dm_internal_round(thd, removed);
             }
            else
             {
               thd = 1U;
             }
          }
         else if (phse < thse)
          {
            resultExponent = thse;
            if (expDiff <= (CUTOFF + 1))
             {
               uint64_t removed;
               removed = phd % makeShift[expDiff + 1];
               phd /= makeShift[expDiff + 1];
               phd += dm_internal_round(phd, removed);
             }
            else
             {
               phd = 1U;
             }
          }
         else
          {
            // No normalization needed
          }

            // Compute the digits and sign.
         if (prodSign == dm_double_signbit(ths)) // Both same sign
          {
            phd += thd;
          }
         else // Different Signs
          {
            if (phd >= thd) // lhs larger : result sign of lhs
             {
               phd -= thd;
             }
            else // rhs larger : result sign of rhs (opposite sign of lhs)
             {
               phd = thd - phd;
               resultSign = !resultSign;
             }
          }
         resultSignificand = phd; // This cannot possibly be zero.

            // Normalize the result
         uint64_t scal = 10U;
         uint64_t test = BIAS * 10U;
         if (phd < test) // Did destructive cancellation occur? ie 100 - 1.
          {
            --resultExponent;
          }
         else
          {
            scal *= 10U;
            test *= 10U;
            if (phd >= test) // Did overflow occur? ie 9 + 1
             {
               ++resultExponent;
               scal *= 10U;
             }
          }
         resultSignificand = phd / scal;
         rem = phd % scal;
         resultSignificand += dm_decideRound(resultSign, resultSignificand & 1U, (int64_t)scal - (int64_t)(rem << 1), rem == 0U, dm_global_round_mode);
         if (resultSignificand == BIAS)
          {
            resultSignificand = MIN_SIGNIFICAND;
            ++resultExponent;
          }
       }
      else // The pathological case
       {
         dm_muldiv_t thd;
         dm_muldiv_loadFrom(thd, DM_DOUBLE_UNPACK_SIGNIFICAND(ths));
         dm_muldiv_mulBy(thd, BIAS);

         if (phse > thse)
          {
            dm_muldiv_mulBy(lhd, 10U);
          }
         else if (phse < thse)
          {
            resultExponent = thse;
            dm_muldiv_mulBy(thd, 10U);
          }
         else
          {
            // No normalization needed
          }

         if (!dm_muldiv_less(lhd, thd)) // lhs larger : result sign of lhs
          {
            dm_muldiv_sub(lhd, lhd, thd);
          }
         else // rhs larger : result sign of rhs (opposite sign of lhs)
          {
            dm_muldiv_sub(lhd, thd, lhd);
            resultSign = !resultSign;
          }

         dm_muldiv_t test2;
         dm_muldiv_loadFrom(test2, BIAS);
         dm_muldiv_mulBy(test2, BIAS);
         uint64_t rem;
         if (!!dm_muldiv_zero(lhd))
          {
            if (DM_FE_DOWNWARD != dm_global_round_mode)
             {
               resultSign = 0;
             }
            else
             {
               resultSign = 1;
             }
            resultExponent = SPECIAL_EXPONENT;
            resultSignificand = MIN_SIGNIFICAND;
          }
         else if (!dm_muldiv_less(lhd, test2))
          {
            uint64_t shift = BIAS * 10U;
            dm_muldiv_divBy(lhd, shift, &resultSignificand, &rem);
            resultSignificand += dm_decideRound(resultSign, resultSignificand & 1U, (int64_t)(shift) - (int64_t)(rem << 1), rem == 0U, dm_global_round_mode);
          }
         else
          {
            if (expDiff == 1) // If we have gotten here, and an operation like 10 - 1 occurred, then fix the exponent.
             {
               --resultExponent;
             }
            while (!!dm_muldiv_less(lhd, test))
             {
               dm_muldiv_mulBy(lhd, 10U);
               --resultExponent;
             }
            dm_muldiv_divBy(lhd, BIAS, &resultSignificand, &rem);
            resultSignificand += dm_decideRound(resultSign, resultSignificand & 1U, (int64_t)BIAS - (int64_t)(rem << 1), rem == 0U, dm_global_round_mode);
          }
       }

      if (resultExponent > MAX_EXPONENT) // Flush to infinity?
       {
         resultSignificand = DM_INFINITY + MIN_SIGNIFICAND;
         resultExponent = SPECIAL_EXPONENT;
       }
      else if (resultExponent < MIN_EXPONENT) // Flush to zero?
       {
         resultSignificand = MIN_SIGNIFICAND;
         resultExponent = SPECIAL_EXPONENT;
       }
      else
       {
         // Normal number.
       }

      result = DM_DOUBLE_PACK(resultSign, resultExponent, resultSignificand);
    }

   return result;
 }

void dm_double_tostring(dm_double arg, char dest [24])
 {
   int sign = dm_double_signbit(arg);
   uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
   int exponent = DM_DOUBLE_UNPACK_EXPONENT(arg);

   if (!!dm_double_isnan(arg))
    {
      if (!!sign)
       {
         dest[0] = '-';
       }
      dest[sign + 0] = 'N';
      dest[sign + 1] = 'a';
      dest[sign + 2] = 'N';
      dest[sign + 3] = '\0';
    }
   else if (!!dm_double_isinf(arg))
    {
      if (!!sign)
       {
         dest[0] = '-';
       }
      dest[sign + 0] = 'I';
      dest[sign + 1] = 'n';
      dest[sign + 2] = 'f';
      dest[sign + 3] = '\0';
    }
   else
    {
      if (!!dm_double_iszero(arg))
       {
         exponent = 0;
         significand = 0U;
       }
      uint64_t digit = significand / MIN_SIGNIFICAND;
      if (10U == digit)
       {
         digit = 0U;
       }
      uint64_t rest = significand % MIN_SIGNIFICAND;

      if (!!sign)
       {
         dest[0] = '-';
       }
      int first = (int)digit;
      dest[sign + 0] = '0' + first;
      dest[sign + 1] = '.';
      for (int i = 14; i >= 0; --i)
       {
         uint64_t next = rest % 10U;
         int dig = (int)next;
         rest = rest / 10U;
         dest[sign + 2 + i] = '0' + dig;
       }
      dest[sign + 17] = 'e';
      if (exponent >= 0)
       {
         dest[sign + 18] = '+';
       }
      else
       {
         dest[sign + 18] = '-';
         exponent = -exponent;
       }
      if (exponent > 99)
       {
         int next = exponent % 10;
         exponent = exponent / 10;
         dest[sign + 21] = '0' + next;
         next = exponent % 10;
         exponent = exponent / 10;
         dest[sign + 20] = '0' + next;
         dest[sign + 19] = '0' + exponent;
         dest[sign + 22] = '\0';
       }
      else if (exponent > 9)
       {
         int next = exponent % 10;
         exponent = exponent / 10;
         dest[sign + 20] = '0' + next;
         dest[sign + 19] = '0' + exponent;
         dest[sign + 21] = '\0';
       }
      else
       {
         dest[sign + 19] = '0' + exponent;
         dest[sign + 20] = '\0';
       }
    }
 }

dm_double dm_double_fromstring(const char * arg)
 {
   int resultSign = 0;
   int32_t resultExponent = -1;
   uint64_t resultSignificand = 0;
   int digits = 0;
   int hasResidue = 0;
   int allZero = 1;
   int realDigit = 0;
   int residue = 0;
   const char * iter = arg;

   if ('-' == *iter)
    {
      resultSign = 1;
      ++iter;
    }

   while (!!isdigit((unsigned char)*iter))
    {
      if (digits < CUTOFF)
       {
         if (!!realDigit || ('0' != *iter))
          {
            char temp = *iter - '0';
            resultSignificand *= 10U;
            resultSignificand += (unsigned char)temp;
            ++digits;
            realDigit = 1;
          }
       }
      else
       {
         if (!hasResidue)
          {
            hasResidue = 1;
            if ('0' == *iter)
             {
               // Do Nothing
             }
            else
             {
               allZero = 0;
               if (*iter > '5')
                {
                  residue = -1;
                }
               else if (*iter < '5')
                {
                  residue = 1;
                }
               else
                {
                  residue = 0;
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
                  if (!!allZero)
                   {
                     residue = 1;
                   }
                  else
                   {
                     residue = -1;
                   }
                  allZero = 0;
                }
             }
          }
       }
      if (!!realDigit)
       {
         ++resultExponent;
       }
      ++iter;
    }
   if (('.' == *iter) || (',' == *iter))
    {
      ++iter;
    }
   while (!!isdigit((unsigned char)*iter))
    {
      if (digits < CUTOFF)
       {
         if (!!realDigit || ('0' != *iter))
          {
            char temp = *iter - '0';
            resultSignificand *= 10U;
            resultSignificand += (unsigned char)temp;
            ++digits;
            realDigit = 1;
          }
       }
      else
       {
         if (!hasResidue)
          {
            hasResidue = 1;
            if ('0' == *iter)
             {
               // Do Nothing
             }
            else
             {
               allZero = 0;
               if (*iter > '5')
                {
                  residue = -1;
                }
               else if (*iter < '5')
                {
                  residue = 1;
                }
               else
                {
                  residue = 0;
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
                  if (!!allZero)
                   {
                     residue = 1;
                   }
                  else
                   {
                     residue = -1;
                   }
                  allZero = 0;
                }
             }
          }
       }
      if (!realDigit)
       {
         --resultExponent;
       }
      ++iter;
    }
   while (digits < CUTOFF)
    {
      resultSignificand *= 10;
      ++digits;
    }
   if (!!allZero) // By the definition of the comp argument.
    {
      residue = 1;
    }
   resultSignificand += dm_decideRound(resultSign, resultSignificand & 1U, residue, allZero, dm_global_round_mode);
   if (resultSignificand == BIAS)
    {
      resultSignificand = MIN_SIGNIFICAND;
      ++resultExponent;
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
      while (!!isdigit((unsigned char)*iter))
       {
         exponentValue = (exponentValue * 10) + (*iter - '0');
         ++iter;
         if (((exponentValue * exponentSign) > (2 * MAX_EXPONENT)) || ((exponentValue * exponentSign) < (2 * MIN_EXPONENT)))
          {
            break;
          }
       }
      resultExponent += exponentValue * exponentSign;
    }

   if (0U == resultSignificand)
    {
      resultExponent = SPECIAL_EXPONENT;
      resultSignificand = MIN_SIGNIFICAND;
    }

   dm_double finalResult;
   if (resultExponent > MAX_EXPONENT) // Flush to infinity?
    {
      if (!!resultSign)
       {
         finalResult = dm_double_neg(dm_double_Inf);
       }
      else
       {
         finalResult = dm_double_Inf;
       }
    }
   else if (resultExponent < MIN_EXPONENT) // Flush to zero?
    {
      if (!!resultSign)
      {
         finalResult = dm_double_neg(dm_double_Zero);
      }
      else
      {
         finalResult = dm_double_Zero;
      }
    }
   else
    {
      finalResult = DM_DOUBLE_PACK(resultSign, resultExponent, resultSignificand);
    }

   return finalResult;
 }

#ifndef DM_NO_DOUBLE_MATH

double dm_double_todouble(dm_double arg)
 {
   double result;
   double sign = 1.0;
   uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
   if (!!dm_double_signbit(arg))
    {
      sign = -1.0;
    }
   if (!!dm_double_isnan(arg))
    {
      result = copysign(NAN, sign);
    }
   else if (!!dm_double_isinf(arg))
    {
      result = copysign(INFINITY, sign);
    }
   else if (!!dm_double_iszero(arg))
    {
      result = copysign(0.0, sign);
    }
   else
    {
      int bias = DM_DOUBLE_UNPACK_EXPONENT(arg) - (CUTOFF - 1);
      double d_significand = (double)significand;
      if (bias >= 0)
       {
         result = sign * d_significand * pow(10.0, bias);
       }
      else
       {
         result = sign * d_significand / pow(10.0, -bias);
       }
    }
   return result;
 }

dm_double dm_double_fromdouble(double arg)
 {
   int sign;
   int16_t exponent;
   uint64_t significand;
   sign = 0;
   if (0.0 == arg)
    {
      significand = MIN_SIGNIFICAND;
      exponent = SPECIAL_EXPONENT;
    }
   else if (!!isinf(arg))
    {
      significand = DM_INFINITY + MIN_SIGNIFICAND;
      exponent = SPECIAL_EXPONENT;
    }
   else if (!!isnan(arg))
    {
      significand = 127U + MIN_SIGNIFICAND;
      exponent = SPECIAL_EXPONENT;
    }
   else
    {
      exponent = (int16_t)(floor(log10(fabs(arg)))); // Should never overflow
      if (exponent >= 0)
       {
         significand = (uint64_t)(fabs(arg) / pow(10.0, exponent) * MIN_SIGNIFICAND);
       }
      else
       {
         significand = (uint64_t)(fabs(arg) * pow(10.0, -exponent) * MIN_SIGNIFICAND);
       }
      double d_significand = (double)significand;
      double temp = 5.0;
      int bias = exponent - (CUTOFF - 1);
      if (bias >= 0)
       {
         temp = temp - (fabs(arg) / pow(10.0, bias) - d_significand) * 10.0;
       }
      else
       {
         temp = temp - (fabs(arg) * pow(10.0, -bias) - d_significand) * 10.0;
       }
      int comp;
      if (temp > 0.0)
       {
         comp = 1;
       }
      else if (temp < 0.0)
       {
         comp = -1;
       }
      else
       {
         comp = 0;
       }
      significand += dm_decideRound(arg < 0.0, significand & 1U, comp, 5.0 == temp, dm_global_round_mode);
      if (significand == BIAS)
       {
         significand = MIN_SIGNIFICAND;
         ++exponent;
       }
    }
   if (!!signbit(arg))
    {
      sign = 1;
    }
   return DM_DOUBLE_PACK(sign, exponent, significand);
 }

#ifndef DM_NO_LONG_DOUBLE_MATH

long double dm_double_tolongdouble(dm_double arg)
 {
   long double result;
   long double sign = 1.0;
   uint64_t significand = DM_DOUBLE_UNPACK_SIGNIFICAND(arg);
   if (!!dm_double_signbit(arg))
    {
      sign = -1.0;
    }
   if (!!dm_double_isnan(arg))
    {
      result = copysign(NAN, sign);
    }
   else if (!!dm_double_isinf(arg))
    {
      result = copysign(INFINITY, sign);
    }
   else if (!!dm_double_iszero(arg))
    {
      result = copysign(0.0L, sign);
    }
   else
    {
      long double d_significand = (long double)significand;
      int bias = DM_DOUBLE_UNPACK_EXPONENT(arg) - (CUTOFF - 1);
      if (bias >= 0)
       {
         result = sign * d_significand * powl(10.0L, bias);
       }
      else
       {
         result = sign * d_significand / powl(10.0L, -bias);
       }
    }
   return result;
 }

dm_double dm_double_fromlongdouble(long double arg)
 {
   int sign;
   int16_t exponent;
   uint64_t significand;
   sign = 0;
   if (0.0 == arg)
    {
      significand = MIN_SIGNIFICAND;
      exponent = SPECIAL_EXPONENT;
    }
   else if (!!isinf(arg))
    {
      significand = DM_INFINITY + MIN_SIGNIFICAND;
      exponent = SPECIAL_EXPONENT;
    }
   else if (!!isnan(arg))
    {
      significand = 127U + MIN_SIGNIFICAND;
      exponent = SPECIAL_EXPONENT;
    }
   else
    {
      exponent = (int16_t)(floorl(log10l(fabsl(arg)))); // Should never overflow this type
      if (exponent >= 0)
       {
         significand = (uint64_t)(fabsl(arg) / powl(10.0, exponent) * MIN_SIGNIFICAND);
       }
      else
       {
         significand = (uint64_t)(fabsl(arg) * powl(10.0, -exponent) * MIN_SIGNIFICAND);
       }
      long double d_significand = (long double)significand;
      long double temp = 5.0;
      int bias = exponent - (CUTOFF - 1);
      if (bias >= 0)
       {
         temp = temp - (fabsl(arg) / powl(10.0, bias) - d_significand) * 10.0;
       }
      else
       {
         temp = temp - (fabsl(arg) * powl(10.0, -bias) - d_significand) * 10.0;
       }
      int comp;
      if (temp > 0.0)
       {
         comp = 1;
       }
      else if (temp < 0.0)
       {
         comp = -1;
       }
      else
       {
         comp = 0;
       }
      significand += dm_decideRound(arg < 0.0, significand & 1U, comp, 5.0 == temp, dm_global_round_mode);
      if (significand == BIAS)
       {
         significand = MIN_SIGNIFICAND;
         ++exponent;
       }

      if (exponent > MAX_EXPONENT) // Can overflow in the dest
       {
         significand = DM_INFINITY + MIN_SIGNIFICAND;
         exponent = SPECIAL_EXPONENT;
       }
      else if (exponent < MIN_EXPONENT)
       {
         significand = MIN_SIGNIFICAND;
         exponent = SPECIAL_EXPONENT;
       }
      else
       {
         // Exponent is in range
       }
    }
   if (!!signbit(arg))
    {
      sign = 1;
    }
   return DM_DOUBLE_PACK(sign, exponent, significand);
 }

#endif /* ! DM_NO_LONG_DOUBLE_MATH */
#endif /* ! DM_NO_DOUBLE_MATH */

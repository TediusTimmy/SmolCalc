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
#include "dm_muldiv.h"

void dm_muldiv_loadFrom(dm_muldiv_t dest, uint64_t source)
 {
#ifndef DM_NO_128_BIT_TYPE
   *dest = source;
#else // DM_NO_128_BIT_TYPE
   dest[3] = 0;
   dest[2] = 0;
   dest[1] = source >> 32U;
   dest[0] = source;
#endif
 }

void dm_muldiv_mulBy(dm_muldiv_t dest, uint64_t rhs)
 {
#ifndef DM_NO_128_BIT_TYPE
   *dest *= rhs;
#else // DM_NO_128_BIT_TYPE
/*
   A  B  C  D
         X  Y
   ----------
  AY BY CY DY
  BX CX DX

   While this looks suspicious, it can't overflow:
      9 + 9 + 9 * 9 = 99
      (BASE - 1) + (BASE - 1) + (BASE - 1) * (BASE - 1)
      (2 * BASE - 2) + (BASE ^ 2 - 2 * BASE + 1)
      BASE ^ 2 - 1
   We take the high half of the previous low product,
      plus the low half of the current high product,
      plus the current low product.
*/
   uint64_t low = rhs & 0xFFFFFFFFU;
   uint64_t high = rhs >> 32U;
   uint64_t temp1 = low * dest[0];
   uint64_t temp2 = high * dest[0];
   dest[0] = temp1;
   temp1 >>= 32U;
   temp1 = temp1 + (temp2 & 0xFFFFFFFFU) + (low * dest[1]);
   temp2 >>= 32U;
   temp2 = temp2 + (high * dest[1]);
   dest[1] = temp1;
   temp1 >>= 32U;
   temp1 = temp1 + (temp2 & 0xFFFFFFFFU) + (low * dest[2]);
   temp2 >>= 32U;
   temp2 = temp2 + (high * dest[2]);
   dest[2] = temp1;
   temp1 >>= 32U;
   temp1 = temp1 + (temp2 & 0xFFFFFFFFU) + (low * dest[3]);
   dest[3] = temp1;
#endif
 }

#ifdef DM_NO_128_BIT_TYPE
void dm_muldiv_copy(dm_muldiv_t dest, dm_muldiv_t src)
 {
   dest[0] = src[0];
   dest[1] = src[1];
   dest[2] = src[2];
   dest[3] = src[3];
 }

void dm_muldiv_extract(dm_muldiv_t source, uint64_t* dest)
 {
   *dest = (((uint64_t)source[1]) << 32U) | source[0];
 }
#ifdef DM_USE_SLOW_BINARY
void dm_muldiv_shl1(dm_muldiv_t arg)
 {
   uint64_t temp = ((uint64_t)arg[0]) << 1U;
   arg[0] = temp;
   temp >>= 32U;
   temp = temp | ((uint64_t)arg[1]) << 1U;
   arg[1] = temp;
   temp >>= 32U;
   temp = temp | ((uint64_t)arg[2]) << 1U;
   arg[2] = temp;
   temp >>= 32U;
   temp = temp | ((uint64_t)arg[3]) << 1U;
   arg[3] = temp;
 }

void dm_muldiv_shr1(dm_muldiv_t arg)
 {
   uint64_t temp = ((uint64_t)arg[3]);
   arg[3] = temp >> 1U;
   temp <<= 32U;
   temp = temp | ((uint64_t)arg[2]);
   arg[2] = temp >> 1U;
   temp <<= 32U;
   temp = temp | ((uint64_t)arg[1]);
   arg[1] = temp >> 1U;
   temp <<= 32U;
   temp = temp | ((uint64_t)arg[0]);
   arg[0] = temp >> 1U;
 }
#else /* Use faster word division */
void dm_muldiv_shl(dm_muldiv_t arg, unsigned int count)
 {
   unsigned int real_count = count;
   if (32U < real_count)
    {
      arg[3] = arg[2];
      arg[2] = arg[1];
      arg[1] = arg[0];
      arg[0] = 0U;
      real_count -= 32U;
    }
   uint64_t temp = ((uint64_t)arg[0]) << real_count;
   arg[0] = temp;
   temp >>= 32U;
   temp = temp | ((uint64_t)arg[1]) << real_count;
   arg[1] = temp;
   temp >>= 32U;
   temp = temp | ((uint64_t)arg[2]) << real_count;
   arg[2] = temp;
   temp >>= 32U;
   temp = temp | ((uint64_t)arg[3]) << real_count;
   arg[3] = temp;
 }

void dm_muldiv_shr(dm_muldiv_t arg, unsigned int count)
 {
   unsigned int real_count = count;
   if (32U < real_count)
    {
      arg[0] = arg[1];
      arg[1] = arg[2];
      arg[2] = arg[3];
      arg[3] = 0U;
      real_count -= 32U;
    }
   uint64_t temp = ((uint64_t)arg[3]);
   arg[3] = temp >> real_count;
   temp <<= 32U;
   temp = temp | ((uint64_t)arg[2]);
   arg[2] = temp >> real_count;
   temp <<= 32U;
   temp = temp | ((uint64_t)arg[1]);
   arg[1] = temp >> real_count;
   temp <<= 32U;
   temp = temp | ((uint64_t)arg[0]);
   arg[0] = temp >> real_count;
 }
#endif /* DM_USE_SLOW_BINARY */
#endif /* DM_NO_128_BIT_TYPE */

void dm_muldiv_divBy(dm_muldiv_t lhs, uint64_t rhs, uint64_t* quo, uint64_t* rem)
 {
#ifndef DM_NO_128_BIT_TYPE
   *quo = *lhs / rhs;
   *rem = *lhs % rhs;
#else // DM_NO_128_BIT_TYPE
#ifdef DM_USE_SLOW_BINARY
   dm_muldiv_t d;
   dm_muldiv_t n;
   uint64_t m;
   uint64_t q;
   dm_muldiv_loadFrom(d, rhs);
   dm_muldiv_copy(n, lhs);
   m = 1U;
   q = 0U;
   while (0 != dm_muldiv_less(d, n))
    {
      dm_muldiv_shl1(d);
      m <<= 1;
      if (0x8000000000000000ULL == m)
       {
         break;
       }
    }
   if (0 != dm_muldiv_less(n, d))
    {
      dm_muldiv_shr1(d);
      m >>= 1;
    }

   while (0U != m)
    {
      dm_muldiv_sub(n, n, d);
      q += m;
      while (0 != dm_muldiv_less(n, d))
       {
         dm_muldiv_shr1(d);
         m >>= 1;
       }
    }
   *quo = q;
   dm_muldiv_extract(n, rem);
#else /* Use faster word division */
   if (0U == (lhs[3] | lhs[2])) // Is this a degenerate case?
    {
      uint64_t n = (((uint64_t)lhs[1]) << 32U) | lhs[0];
      *quo = n / rhs;
      *rem = n % rhs;
    }
   else
    {
      dm_muldiv_t n;
      dm_muldiv_t d;
      dm_muldiv_t s;
      dm_muldiv_t t;
      dm_muldiv_copy(n, lhs);
      dm_muldiv_loadFrom(d, rhs);
      uint64_t dig1;
      uint64_t dig2;

      unsigned int count = 0U;
      uint64_t byt = 0xFF00000000000000ULL;
      uint64_t bit = 0x8000000000000000ULL;
      while (0U == (rhs & byt))
       {
         byt >>= 8U;
         bit >>= 8U;
         count += 8U;
       }
      while (0U == (rhs & bit))
       {
         bit >>= 1U;
         ++count;
       }

      dm_muldiv_shl(n, count);
      dm_muldiv_shl(d, count);

      s[3] = 0U;
      s[2] = d[1];
      s[1] = d[0];
      s[0] = 0U;

      dm_muldiv_copy(t, s);

      dig1 = ((((uint64_t)n[3]) << 32U) | n[2]) / d[1];

      dm_muldiv_mulBy(t, dig1);

      if (0 != dm_muldiv_less(n, t)) // if t > n
       {
         --dig1;
         dm_muldiv_sub(t, t, s);
         if (0 != dm_muldiv_less(n, t)) // According to Knuth, only need to do this twice.
          {
            --dig1;
            dm_muldiv_sub(t, t, s);
          }
       }
      dm_muldiv_sub(n, n, t);

      dm_muldiv_copy(s, d);
      dm_muldiv_copy(t, d);

      dig2 = ((((uint64_t)n[2]) << 32U) | n[1]) / d[1];

      dm_muldiv_mulBy(t, dig2);

      if (0 != dm_muldiv_less(n, t))
       {
         --dig2;
         dm_muldiv_sub(t, t, s);
         if (0 != dm_muldiv_less(n, t))
          {
            --dig2;
            dm_muldiv_sub(t, t, s);
          }
       }
      dm_muldiv_sub(n, n, t);

      *quo = (dig1 << 32U) | dig2;
      dm_muldiv_shr(n, count);
      dm_muldiv_extract(n, rem);
    }
#endif /* DM_USE_SLOW_BINARY */
#endif /* DM_NO_128_BIT_TYPE */
 }


void dm_muldiv_sub(dm_muldiv_t dest, dm_muldiv_t lhs, dm_muldiv_t rhs) // dest may alias lhs or rhs.
 {
#ifndef DM_NO_128_BIT_TYPE
   *dest = *lhs - *rhs;
#else // DM_NO_128_BIT_TYPE
   uint64_t temp = ((uint64_t)lhs[0]) - rhs[0];
   dest[0] = temp;
   temp >>= 32U;
   temp = ((uint64_t)lhs[1]) - rhs[1] - (temp & 1U);
   dest[1] = temp;
   temp >>= 32U;
   temp = ((uint64_t)lhs[2]) - rhs[2] - (temp & 1U);
   dest[2] = temp;
   temp >>= 32U;
   temp = ((uint64_t)lhs[3]) - rhs[3] - (temp & 1U);
   dest[3] = temp;
#endif
 }

int dm_muldiv_zero(dm_muldiv_t arg)
 {
#ifndef DM_NO_128_BIT_TYPE
   return 0U == *arg;
#else // DM_NO_128_BIT_TYPE
   return 0U == (arg[0] | arg[1] | arg[2] | arg[3]);
#endif
 }

int dm_muldiv_less(dm_muldiv_t lhs, dm_muldiv_t rhs)
 {
#ifndef DM_NO_128_BIT_TYPE
   return *lhs < *rhs;
#else // DM_NO_128_BIT_TYPE
   int result;
   if (lhs[3] != rhs[3])
    {
      result = lhs[3] < rhs[3];
    }
   else if (lhs[2] != rhs[2])
    {
      result = lhs[2] < rhs[2];
    }
   else if (lhs[1] != rhs[1])
    {
      result = lhs[1] < rhs[1];
    }
   else
    {
      result = lhs[0] < rhs[0];
    }
   return result;
#endif
 }

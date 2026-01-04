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

#include <stdint.h>
#include <string.h>

   /* ABI compatible defines. */
typedef uint64_t dm_double;

int dm_double_iscanonical (dm_double);
int dm_double_isinf       (dm_double);
int dm_double_isnan       (dm_double);
int dm_double_signbit     (dm_double);
int dm_double_iszero      (dm_double);
dm_double dm_double_fabs  (dm_double);
dm_double dm_double_frexp (dm_double, int*);
void dm_double_tostring   (dm_double, char [25]);

static void conditionResult(char result [24])
 {
   int base = 16;
   if ('-' == result[0])
    {
      base = base + 1;
    }
   int index = base;
   while ('0' == result[index])
    {
      --index;
    }
   if ('.' == result[index])
    {
      --index;
    }
   if (index != base)
    {
      memmove(&result[index + 1], &result[base + 1], 6U);
    }
 }

   /*
      What is a pretty string?
         If the number is Inf or NaN, print as normal.
         If the number isn't canonical, print "Err" (no sign).

         If the exponent is greater than fifteen, print like normal.
         If the exponent is less than negative ten, print like normal.
         If the exponent is between zero and fifteen inclusive, put the decimal point where it should be and remove trailing zeros after the decimal point.
            If the number is an integer, remove the decimal point.
         If the exponent is between negative one and negative five inclusive, put the decimal at the front, place abs(exponent) - 1 leading zeros, and remove trailing zeros.
            -9.999999999999999e-511
            -0.00009999999999999999
         If the exponent is between negative six and negative ten inclusive:
            If the exponent plus the number of trailing zeros is greater than negative six, print as -1 <= exp <= -5, else print like normal.
            Don't go all the way to 21, it would be too much.
            -9.999999999999999e-511
            -0.0000000009
   */
void dm_double_toprettystring (dm_double arg, char result [24])
 {
   char temp [25];
      // Just use the output of normal tostring.
   if (dm_double_isnan(arg) || dm_double_isinf(arg))
    {
      (void) memset(temp, '\0', 25U);
      dm_double_tostring(arg, temp);
      memmove(result, temp, 24U);
    }
      // Special error string. If the software emits this, we will assume that it is a bug in the software.
   else if (!dm_double_iscanonical(arg))
    {
      result[0] = 'E';
      result[1] = 'r';
      result[2] = 'r';
      result[3] = '\0';
    }
   else if (!!dm_double_iszero(arg)) // Zeros need special handling.
    {
      if (!!dm_double_signbit(arg))
       {
         result[0] = '-';
         result[1] = '0';
         result[2] = '\0';
       }
      else
       {
         result[0] = '0';
         result[1] = '\0';
       }
    }
   else // This is a number to print.
    {
      int sign = dm_double_signbit(arg);
      int exponent;
         // This function is ABI compatible, as it never involves itself with the underlying representation.
      dm_double significand = dm_double_fabs(dm_double_frexp(arg, &exponent));

      if ((exponent > 15) || (exponent < -10)) // Firstly: all other cases.
       {
         (void) memset(temp, '\0', 25U);
         dm_double_tostring(arg, temp);
         memmove(result, temp, 24U);
         conditionResult(result);
       }
      else if (exponent >= 0) // Numbers that we need to move the radix point to the right for.
       {
         dm_double_tostring(significand, temp); // Just the significand, always positive.

         int index = 1;
         while (exponent > 0)
          {
            char swap = temp[index];
            temp[index] = temp[index + 1];
            temp[index + 1] = swap;
            ++index;
            --exponent;
          }

         temp[17] = '\0'; // Don't need that 'e' anymore.
         index = 16;
         while ('0' == temp[index]) // Remove trailing '0's.
          {
            temp[index] = '\0';
            --index;
          }
         if ('.' == temp[index]) // If the number is an integer, then remove the decimal point.
          {
            temp[index] = '\0';
          }

         // Now, recompose the result.
         index = 0;
         if (0 != sign)
          {
            result[index] = '-';
            ++index;
          }
         int c = 0; // strcpy, but not because its "bad"
         while ((c < 17) && ('\0' != temp[c]))
          {
            result[index] = temp[c];
            ++index;
            ++c;
          }
         result[index] = '\0'; // I'm sure some static analysis tool is now happy.
       }
      else // Exponent < 0 but > -10
       {
            // First step : remove trailing zeros and count zeros removed.
         dm_double_tostring(significand, temp); // Just the significand, always positive.
         temp[17] = '\0'; // Don't need that 'e' anymore.
         int index = 16;
         int removed = 0;
         while ('0' == temp[index]) // Remove trailing '0's.
          {
            temp[index] = '\0';
            --index;
            ++removed;
          }

         if ((exponent + removed) >= -5) // Do we just want to print leading zeros?
          {
            index = 0;
            if (0 != sign)
             {
               result[index] = '-';
               ++index;
             }
            result[index] = '0';
            ++index;
            result[index] = '.';
            ++index;
            while (exponent < -1)
             {
               result[index] = '0';
               ++index;
               ++exponent;
             }
            result[index] = temp[0];
            ++index;
            int c = 2; // strcpy, but not because its "bad"
            while ((c < 17) && ('\0' != temp[c]))
             {
               result[index] = temp[c];
               ++index;
               ++c;
             }
            result[index] = '\0'; // I'm sure some static analysis tool is now happy.
          }
         else // This is actually a normal case. This is inefficient, but we're, like, doing things in decimal, man.
          {
            (void) memset(temp, '\0', 25U);
            dm_double_tostring(arg, temp);
            memmove(result, temp, 24U);
            conditionResult(result);
          }
       }
    }
 }

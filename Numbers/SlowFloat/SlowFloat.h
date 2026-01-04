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
#ifndef SLOWFLOAT_H
#define SLOWFLOAT_H

/*
   SlowFloat is a decimal floating point class with nine digits of significand
   and an exponent range of -32767 to 32767, inclusive.

   It is six bytes (48 bits) per number. The exponent range is silly because
   it was wanted for the type to be an even number of bytes, the range covers
   up the lack of subnormals, and the size really isn't important anyway.

   The significand is a nine-digit, normalized number: 100000000 - 999999999.
   SlowFloat makes no attempt to track significance as the General Decimal
   Arithmetic Specification tries to. It also tries to be standards-conforming
   C++, unlike the assembly of DEC64.

   To store the sign, we take advantage of the fact that 99999999 does not
   have the most-significant bit set in binary. If the most-significant bit is
   set, then the number is negative and the significand has been ones'
   complemented.

   The exponent field is just a two's complement number, with -32768 being
   reserved for special cases, indicated in the significand:
      All zeros : positive infinity
      All ones : negative infinity
      Anything else : NaN

   The canonical representation of zero is with the exponent zero also.
   SlowFloat has no concept of signaling NaNs.
   The IEEE-754 exceptions perform their default behavior, as most users of
      the C++ types float or double would expect.


   References:
      General Decimal Arithmetic Specification
         https://speleotrove.com/decimal/decarith.html
      DEC64
         https://www.crockford.com/dec64.html
*/

#include <string>
#include <cstdint>

namespace SlowFloat
 {

enum SlowFloat_Round_Mode
 {
   ROUND_TIES_EVEN,
   ROUND_TIES_AWAY,
   ROUND_POSITIVE_INFINITY,
   ROUND_NEGATIVE_INFINITY,
   ROUND_ZERO,
   ROUND_TIES_ODD,
   ROUND_TIES_ZERO,
   ROUND_AWAY
 };

extern SlowFloat_Round_Mode mode;

class SlowFloat final
 {
public:
   uint32_t significand; // If msb is set, then the number is negative and ones' complemented.
   int16_t  exponent;    // Do to the stupid range, no subnormals. -32768 is reserved for special values.

   SlowFloat() = delete;
   SlowFloat(const SlowFloat&) = default;
   SlowFloat& operator = (const SlowFloat&) = default;

   explicit SlowFloat(uint32_t, int16_t);
   explicit SlowFloat (double);
   explicit operator double () const;
 };

extern const SlowFloat sfZero;

   std::string toString (const SlowFloat&);
   SlowFloat fromString (const std::string&);

   SlowFloat operator - (const SlowFloat&);

   SlowFloat operator + (const SlowFloat&, const SlowFloat&);
   SlowFloat operator - (const SlowFloat&, const SlowFloat&);
   SlowFloat operator * (const SlowFloat&, const SlowFloat&);
   SlowFloat operator / (const SlowFloat&, const SlowFloat&);

   bool operator > (const SlowFloat&, const SlowFloat&);
   bool operator < (const SlowFloat&, const SlowFloat&);
   bool operator >= (const SlowFloat&, const SlowFloat&);
   bool operator <= (const SlowFloat&, const SlowFloat&);
   bool operator == (const SlowFloat&, const SlowFloat&);
   bool operator != (const SlowFloat&, const SlowFloat&);

 }

#endif /* SLOWFLOAT_H */

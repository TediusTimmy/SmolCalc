/*
Copyright (c) 2013 Thomas DiModica.
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. Neither the name of Thomas DiModica nor the names of other contributors
   may be used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THOMAS DIMODICA AND CONTRIBUTORS ``AS IS'' AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED.  IN NO EVENT SHALL THOMAS DIMODICA OR CONTRIBUTORS BE LIABLE
FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.
*/

#include <cstdlib>
#include <string>
#include "FloatFixed.hpp"

#ifndef SEPERATOR
 #define SEPERATOR '.'
#endif

namespace BigInt
 {


   unsigned long FloatFixed::defPrec = 7;


   bool operator > (const FloatFixed & lhs, const FloatFixed & rhs)
    {
      return lhs.compare(rhs) > 0;
    }

   bool operator >= (const FloatFixed & lhs, const FloatFixed & rhs)
    {
      return lhs.compare(rhs) >= 0;
    }


   FloatFixed FloatFixed::operator - (void) const
    {
      FloatFixed returnedFixed(*this);
      returnedFixed.Data.negate();
      return returnedFixed;
    }


   int FloatFixed::compare (const FloatFixed & to) const
    {
      FloatFixed temp;

      if (Digits > to.Digits)
       {
         temp = to;
         temp.changePrecision(Digits);
         return Data.compare(temp.Data);
       }
      else if (Digits < to.Digits)
       {
         temp = *this;
         temp.changePrecision(to.Digits);
         return temp.Data.compare(to.Data);
       }

      return Data.compare(to.Data);
    }


   FloatFixed operator + (const FloatFixed & lhs, const FloatFixed & rhs)
    {
      FloatFixed temp;

      if (lhs.Digits > rhs.Digits)
       {
         temp = rhs;
         temp.changePrecision(lhs.Digits);
         temp.Data = lhs.Data + temp.Data;
       }
      else if (lhs.Digits < rhs.Digits)
       {
         temp = lhs;
         temp.changePrecision(rhs.Digits);
         temp.Data = temp.Data + rhs.Data;
       }
      else
       {
         temp.Data = lhs.Data + rhs.Data;
         temp.Digits = lhs.Digits;
       }

      return temp;
    }

   FloatFixed operator - (const FloatFixed & lhs, const FloatFixed & rhs)
    {
      FloatFixed temp;

      if (lhs.Digits > rhs.Digits)
       {
         temp = rhs;
         temp.changePrecision(lhs.Digits);
         temp.Data = lhs.Data - temp.Data;
       }
      else if (lhs.Digits < rhs.Digits)
       {
         temp = lhs;
         temp.changePrecision(rhs.Digits);
         temp.Data = temp.Data - rhs.Data;
       }
      else
       {
         temp.Data = lhs.Data - rhs.Data;
         temp.Digits = lhs.Digits;
       }

      return temp;
    }

   FloatFixed operator * (const FloatFixed & lhs, const FloatFixed & rhs)
    {
      FloatFixed temp (lhs);

      unsigned long resDigits;
      if (lhs.Digits > rhs.Digits)
         resDigits = lhs.Digits;
      else
         resDigits = rhs.Digits;

      temp.Data = temp.Data * rhs.Data;
      temp.Digits += rhs.Digits;

      FloatFixed top (10, 0);
      if (temp >= top)
       {
         temp.changePrecision(resDigits - 1);
         temp.sticky = true;
       }
      else
       {
         temp.changePrecision(resDigits);
            //Did we round up to 10?
         if (temp >= top)
          {
               //The removed digit will always be zero.
            temp.changePrecision(resDigits - 1);
            temp.sticky = true;
          }
       }

      return temp;
    }

   FloatFixed operator / (const FloatFixed & lhs, const FloatFixed & rhs)
    {
      FloatFixed q = lhs;
      FloatFixed r = rhs;
      Integer d;
      bool s = lhs.Data.isSigned() ^ rhs.Data.isSigned();
      int oneExtra = (rhs > lhs) ? 1 : 0;

      if (lhs.Digits > rhs.Digits)
       {
         q.Data = q.Data * pow10(r.Digits + oneExtra);
         q.Digits = lhs.Digits + oneExtra;
       }
      else
       {
         q.Data = q.Data * pow10(2 * r.Digits - q.Digits + oneExtra);
         q.Digits = rhs.Digits + oneExtra;
       }

      d = r.Data;
      d.abs();

      quotrem(q.Data, r.Data, q.Data, r.Data);

      r.Data = r.Data * Integer(2);

      if (Fixed::decideRound(s, q.Data.isEven(), d.compare(r.Data.abs()),
                             r.Data.isZero(), q.Data.is0mod5()))
       {
         // This will only ever be called with q positive.
         q.Data = q.Data + Integer(1);
       }

      q.sticky = (oneExtra == 1);

      return q;
    }


   void FloatFixed::changePrecision (unsigned long newPrec)
    {
      changePrecision(newPrec, Fixed::getRoundMode());
    }

   void FloatFixed::changePrecision (unsigned long newPrec, Fixed_Round_Mode mode)
    {
      Integer scale, rem;
      bool s = Data.isSigned();

      if (newPrec == Digits) return;
      if (newPrec > Digits)
       {
         scale = pow10(newPrec - Digits);
         Data = Data * scale;
       }
      else
       {
         scale = pow10(Digits - newPrec);
         quotrem(Data, scale, Data, rem);

         rem = rem * Integer(2);

         if (Fixed::decideRound(s, Data.isEven(), scale.compare(rem.abs()),
                                rem.isZero(), Data.is0mod5(), mode))
          {
            if (s) Data = Data - Integer(1);
            else Data = Data + Integer(1);
          }
       }
      Digits = newPrec;
    }


   std::string FloatFixed::toString (void) const
    {
      std::string result;
      result = Data.toString();
      if (Digits == 0) return result;
      if (result.length() <= Digits)
       {
         while (result.length() < Digits) result = '0' + result;
         result = '0' + (SEPERATOR + result);
       }
      else
       {
         result = result.substr(0, result.length() - Digits) + SEPERATOR +
            result.substr(result.length() - Digits, Digits);
       }
      return result;
    }

   size_t FloatFixed::getLength (void) const
    {
      size_t sep = (0U == Digits) ? 0U : 1U;
      size_t digits = Data.getLength();
      if (digits < Digits)
       {
         digits = Digits + 1U; // Leading zero
       }
      return digits + sep;
    }


   void FloatFixed::fromString (const char * src)
    {
      const char * iter = src;
      std::string conv;

      conv.reserve(2048);

      sticky = false;

      if (*iter != SEPERATOR)
       {
         while ((*iter >= '0') && (*iter <= '9')) iter++;
       }

      Digits = 0;
      iter++;
      while ((*iter >= '0') && (*iter <= '9')) { iter++; Digits++; }

      iter = src;

      if (*iter != SEPERATOR) conv += *(iter++);
      iter++;
      while ((*iter >= '0') && (*iter <= '9')) conv += *(iter++);

      Data.fromString(conv);
    }


 } /* namespace BigInt */

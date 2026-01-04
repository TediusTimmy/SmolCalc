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

#ifndef FLOAT_HPP
#define FLOAT_HPP

#include <string>
#include "FloatFixed.hpp"

namespace BigInt
 {

   class Float
    {

      private:
         static unsigned long minPrecision;
         static unsigned long maxPrecision;

         static bool change (const Float &, const Float &, unsigned long&);

      public:
         static unsigned long getMinPrecision (void) { return minPrecision; }
         static unsigned long setMinPrecision (unsigned long newPrecision)
          { return (minPrecision = newPrecision); }

         static unsigned long getMaxPrecision (void) { return maxPrecision; }
         static unsigned long setMaxPrecision (unsigned long newPrecision)
          { return (maxPrecision = newPrecision); }

      private:
         FloatFixed Data;
         bool Sign;
         Integer Exponent;

         bool Infinity;
         long NaN;

         void precisionChanger (unsigned long newPrecision);

      public:

         Float (const Float & from) :
            Data (from.Data), Sign (from.Sign), Exponent (from.Exponent),
            Infinity (from.Infinity), NaN (from.NaN) { }
         Float () :
            Data (0, minPrecision), Sign (false), Exponent(0L),
            Infinity (false), NaN (0)
            { }
         Float (const std::string &);
         Float (const char *);
         ~Float () { /* This has nothing to do. */ }

         unsigned long getPrecision (void) const
          { return Data.getPrecision(); }
         unsigned long setPrecision (unsigned long newPrecision)
          {
            precisionChanger(newPrecision);
            return Data.getPrecision();
          }

         void changePrecision (unsigned long newPrecision)
          {
            if (newPrecision < minPrecision) newPrecision = minPrecision;
            if (newPrecision > maxPrecision) newPrecision = maxPrecision;
            precisionChanger(newPrecision);
          }

         const Integer & exponent (void) const { return Exponent; }

         bool isSigned (void) const { return Sign; }
         bool isNegative (void) const
            { if (Data.isZero()) return false; return Sign; }
         bool isInfinity (void) const { return Infinity; }
         long isNaN (void) const { return NaN; }
         bool isZero (void) const
            { if (Infinity || NaN) return false; return Data.isZero(); }

         std::string toString (void) const;
         size_t getLength (void) const;

         void fromString (const std::string & src)
            { fromString(src.c_str()); }
         void fromString (const char *);

         friend Float operator + (const Float &, const Float &);
         friend Float operator - (const Float &, const Float &);
         friend Float operator * (const Float &, const Float &);
         friend Float operator / (const Float &, const Float &);

         Float operator - (void) const
          { Float returnedFloat(*this); returnedFloat.Sign = !returnedFloat.Sign; return returnedFloat; }
         bool operator ! (void) const { return isZero(); }

         Float & operator = (const Float &) = default;

            //Only works for finite, nonzero _numbers_
         int compare (const Float &) const;

            //Cowlishaw doesn't do normalization, but I need it to make
            //toString work correctly.
         Float & normalize (void);

         Integer roundToInteger (void) const;
         Float roundToInteger (Fixed_Round_Mode) const;

    }; /* class Float */

   bool operator > (const Float &, const Float &);
   bool operator < (const Float &, const Float &);
   bool operator >= (const Float &, const Float &);
   bool operator <= (const Float &, const Float &);
   bool operator == (const Float &, const Float &);
   bool operator != (const Float &, const Float &);

 } /* namespace BigInt */

#endif /* FLOAT_HPP */

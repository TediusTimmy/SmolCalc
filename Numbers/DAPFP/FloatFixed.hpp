/*
Copyright (c) 2010 Thomas DiModica.
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

#ifndef FLOATFIXED_HPP
#define FLOATFIXED_HPP

#include "../BCNum/Integer.hpp"
#include "../BCNum/Fixed.hpp"

namespace BigInt
 {

   class FloatFixed final
    {
   private:
      static unsigned long defPrec;

   public:
      static unsigned long getDefaultPrecision (void) { return defPrec; }
      static unsigned long setDefaultPrecision (unsigned long newPrecision)
       { return (defPrec = newPrecision); }

   private:
      Integer Data;
      unsigned long Digits;
      bool sticky;

   public:

      FloatFixed (const FloatFixed & from) :
         Data (from.Data), Digits (from.Digits), sticky(from.sticky) { }
      explicit FloatFixed (unsigned long precision = defPrec) :
         Data (), Digits (precision), sticky(false) { }
      explicit FloatFixed (long i, unsigned long p = defPrec) :
         Data (i), Digits (p), sticky(false) { }
      ~FloatFixed () { /* This has nothing to do. */ }

      bool getSticky() const { return sticky; }
      void clearSticky() { sticky = false; }

      unsigned long getPrecision (void) const { return Digits; }
      unsigned long setPrecision (unsigned long newPrecision)
         { return (Digits = newPrecision); }

      void changePrecision (unsigned long); //changes Data to match
      void changePrecision (unsigned long, Fixed_Round_Mode);

      bool isSigned (void) const { return Data.isSigned(); }
      bool isZero (void) const { return Data.isZero(); }

      std::string toString (void) const;
      size_t getLength (void) const;

      void fromString (const std::string & src)
         { fromString(src.c_str()); }
      void fromString (const char *);

      friend FloatFixed operator + (const FloatFixed &, const FloatFixed &);
      friend FloatFixed operator - (const FloatFixed &, const FloatFixed &);
      friend FloatFixed operator * (const FloatFixed &, const FloatFixed &);
      friend FloatFixed operator / (const FloatFixed &, const FloatFixed &);

      FloatFixed & operator = (const FloatFixed &) = default;

      FloatFixed operator - (void) const;
      bool operator ! (void) const { return isZero(); }

      int compare (const FloatFixed &) const;

      const Integer& getRaw() const { return Data; }

    }; /* class FloatFixed */

   FloatFixed operator + (const FloatFixed &, const FloatFixed &);
   FloatFixed operator - (const FloatFixed &, const FloatFixed &);
   FloatFixed operator * (const FloatFixed &, const FloatFixed &);
   FloatFixed operator / (const FloatFixed &, const FloatFixed &);

   bool operator > (const FloatFixed &, const FloatFixed &);
   bool operator >= (const FloatFixed &, const FloatFixed &);

 } /* namespace BigInt */

#endif /* FLOATFIXED_HPP */

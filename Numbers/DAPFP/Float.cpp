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
#include <sstream>
#include <cstring>
#include "Float.hpp"

namespace BigInt
 {

    /*
      Remember: you will get digits + 1 of actual precision.
    */
   unsigned long Float::minPrecision = 7;
   unsigned long Float::maxPrecision = 134217727; // 2^27


   Float::Float (const std::string & from)
    {
      fromString(from);
    }

   Float::Float (const char * from)
    {
      fromString(from);
    }


   void Float::precisionChanger(unsigned long newPrecision)
    {
      bool checkOverflow = newPrecision < Data.getPrecision();

      if (Sign) Data = -Data;
      Data.changePrecision(newPrecision);
      if (Sign) Data = -Data;

      if (checkOverflow)
       {
         FloatFixed top (10, 0);
         if (Data >= top)
          {
               //Generally implies that the removed digit is '0'.
            Data.changePrecision(Data.getPrecision() - 1);
            Data.setPrecision(Data.getPrecision() + 1);
            Exponent = Exponent + Integer(1U);
          }
       }
    }


   bool operator > (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return false;
            else return true;
          }
         else if (lhs.isInfinity()) return !lhs.isSigned();
         else return rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) > 0;
    }

   bool operator < (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return false;
            else if (lhs.isSigned()) return true;
            else return false;
          }
         else if (lhs.isInfinity()) return lhs.isSigned();
         else return !rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) < 0;
    }

   bool operator >= (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return true;
            else if (lhs.isSigned()) return false;
            else return true;
          }
         else if (lhs.isInfinity()) return !lhs.isSigned();
         else return rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return true;
      return lhs.compare(rhs) >= 0;
    }

   bool operator <= (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         if (lhs.isInfinity() && rhs.isInfinity())
          {
            if (lhs.isSigned() == rhs.isSigned()) return true;
            else if (lhs.isSigned()) return true;
            else return false;
          }
         else if (lhs.isInfinity()) return lhs.isSigned();
         else return !rhs.isSigned();
       }
      if (lhs.isZero() && rhs.isZero()) return true;
      return lhs.compare(rhs) <= 0;
    }

   bool operator == (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return false;
      if (lhs.isInfinity() && rhs.isInfinity()) return lhs.isSigned() == rhs.isSigned();
      if (lhs.isInfinity() || rhs.isInfinity()) return false;
      if (lhs.isZero() && rhs.isZero()) return true;
      return lhs.compare(rhs) == 0;
    }

   bool operator != (const Float & lhs, const Float & rhs)
    {
      if (lhs.isNaN() || rhs.isNaN()) return true;
      if (lhs.isInfinity() && rhs.isInfinity()) return lhs.isSigned() != rhs.isSigned();
      if (lhs.isInfinity() || rhs.isInfinity()) return true;
      if (lhs.isZero() && rhs.isZero()) return false;
      return lhs.compare(rhs) != 0;
    }


   int Float::compare (const Float & to) const
    {
      if (&to == this) return 0;

      if (!Sign && to.Sign) return 1;
      else if (Sign && !to.Sign) return -1;

      if (0 == Exponent.compare(to.Exponent))
       {
         if (Sign)
            return -Data.compare(to.Data);
         else
            return Data.compare(to.Data);
       }
      else
       {
         if (Sign)
            return -Exponent.compare(to.Exponent);
         else
            return Exponent.compare(to.Exponent);
       }
    }


   bool Float::change (const Float & arg1, const Float & arg2, unsigned long& difference)
    {
      unsigned long prec;
      Integer diff;

      prec = arg1.getPrecision() >= arg2.getPrecision() ?
         arg1.getPrecision() :
         arg2.getPrecision();

      diff = 0 < arg1.exponent().compare(arg2.exponent()) ?
         arg1.exponent() - arg2.exponent() :
         arg2.exponent() - arg1.exponent();

      difference = static_cast<unsigned long>(diff.toInt());

         // Account for a  guard digit (in case of cancellation) and a rounding digit.
      if (difference > (prec + 2))
       {
         difference = prec + 3;

            // For these cases, always do the math.
         if ((ROUND_POSITIVE_INFINITY == Fixed::getRoundMode()) ||
             (ROUND_NEGATIVE_INFINITY == Fixed::getRoundMode()) ||
             (ROUND_ZERO == Fixed::getRoundMode()))
          {
            return true;
          }

         return false;
       }
      return true;
    }


   Float operator + (const Float & lhs, const Float & rhs)
    {
      Float temp;
       /*
         We don't bump temp's precision here because it wouldn't be
         meaningful. It isn't meaningful to have an Infinity with 500
         digits of precision.
       */

         //First, handle NaNs, as they have the highest precedence.
       /*
         As I write this, I still haven't convinced myself of the usefulness
         of diagnostic NaNs. I have never had language support for them.
         Undocumented: 1: Inf - Inf, 2: 0 * Inf, 4: Inf / Inf, 8: 0 / 0
         16: Inv Op (sqrt/ln(-)), 32: User Input / Initialized Value
       */
      if (lhs.isNaN() || rhs.isNaN())
       {
         if (lhs.isNaN() && rhs.isNaN())
            temp.NaN = lhs.isNaN() | rhs.isNaN();
         else if (lhs.isNaN())
            temp.NaN = lhs.isNaN();
         else
            temp.NaN = rhs.isNaN();
         return temp;
       }

         //Next, if both are infinities.
      if (lhs.isInfinity() && rhs.isInfinity())
       {
            //Inf - Inf = NaN
         if (lhs.isSigned() != rhs.isSigned())
          {
            temp.NaN = 1;
            return temp;
          }

         temp.Sign = lhs.Sign;
         temp.Infinity = true;
         return temp;
       }

         //Anything else with infinity is infinity
      if (lhs.isInfinity())
       {
         temp.Sign = lhs.Sign;
         temp.Infinity = true;
         return temp;
       }
      if (rhs.isInfinity())
       {
         temp.Sign = rhs.Sign;
         temp.Infinity = true;
         return temp;
       }

         //Finally, zeros.
      if (lhs.isZero() && rhs.isZero() && (lhs.Sign != rhs.Sign))
         return temp;
      if (rhs.isZero()) return lhs;
      if (lhs.isZero()) return rhs;

      unsigned long diff;
         //Do we need to compute anything?
      if (Float::change(lhs, rhs, diff))
       {
         Float temp1 (lhs), temp2 (rhs);
            //Denormalize the Precisions to take account of the Exponents
         if (0 < temp1.Exponent.compare(temp2.Exponent))
          {
            temp.Exponent = temp1.Exponent;
            temp2.Data.setPrecision(temp2.Data.getPrecision() + diff);
          }
         else if (0 > temp1.Exponent.compare(temp2.Exponent))
          {
            temp.Exponent = temp2.Exponent;
            temp1.Data.setPrecision(temp1.Data.getPrecision() + diff);
          }
         else temp.Exponent = temp1.Exponent;

            //Assign the correct signs before operating
         if (temp1.Sign) temp1.Data = -temp1.Data;
         if (temp2.Sign) temp2.Data = -temp2.Data;

         temp.Data = temp1.Data + temp2.Data;

            //Clear the sign
         temp.Sign = temp.Data.isSigned();
         if (temp.Sign) temp.Data = -temp.Data;

            //Normalize result
         temp.normalize();
         temp.setPrecision(lhs.Data.getPrecision() > rhs.Data.getPrecision() ?
            lhs.Data.getPrecision() : rhs.Data.getPrecision());

         if (temp.Data.isZero() && (ROUND_NEGATIVE_INFINITY == Fixed::getRoundMode()))
            temp.Sign = true;
       }
      else
       {
            //Just return either the lhs or the rhs
         temp = 0 < lhs.Exponent.compare(rhs.Exponent) ? lhs : rhs;
            //We don't change the precision of the result,
            //because we haven't added any meaningful information
            //For example: 2.0 + 1.000E-16 should not be 2.000
       }

      return temp;
    }

   Float operator - (const Float & lhs, const Float & rhs)
    {
      Float temp;

      if (lhs.isNaN() || rhs.isNaN())
       {
         if (lhs.isNaN() && rhs.isNaN())
            temp.NaN = lhs.isNaN() | rhs.isNaN();
         else if (lhs.isNaN())
            temp.NaN = lhs.isNaN();
         else
            temp.NaN = rhs.isNaN();
         return temp;
       }

      if (lhs.isInfinity() && rhs.isInfinity())
       {
         if (lhs.isSigned() == rhs.isSigned())
          {
            temp.NaN = 1;
            return temp;
          }

         temp.Sign = lhs.Sign;
         temp.Infinity = true;
         return temp;
       }

      if (lhs.isInfinity())
       {
         temp.Sign = lhs.Sign;
         temp.Infinity = true;
         return temp;
       }
      if (rhs.isInfinity())
       {
         temp.Sign = !rhs.Sign;
         temp.Infinity = true;
         return temp;
       }

      if (lhs.isZero() && rhs.isZero() && (lhs.Sign == rhs.Sign))
         return temp;
      if (rhs.isZero()) return lhs;
      if (lhs.isZero()) return -rhs;

      unsigned long diff;
      if (Float::change(lhs, rhs, diff))
       {
         Float temp1 (lhs), temp2 (rhs);
         if (0 < temp1.Exponent.compare(temp2.Exponent))
          {
            temp.Exponent = temp1.Exponent;
            temp2.Data.setPrecision(temp2.Data.getPrecision() + diff);
          }
         else if (0 > temp1.Exponent.compare(temp2.Exponent))
          {
            temp.Exponent = temp2.Exponent;
            temp1.Data.setPrecision(temp1.Data.getPrecision() + diff);
          }
         else temp.Exponent = temp1.Exponent;

         if (temp1.Sign) temp1.Data = -temp1.Data;
         if (temp2.Sign) temp2.Data = -temp2.Data;

         temp.Data = temp1.Data - temp2.Data;

         temp.Sign = temp.Data.isSigned();
         if (temp.Sign) temp.Data = -temp.Data;

         temp.normalize();
         temp.setPrecision(lhs.Data.getPrecision() > rhs.Data.getPrecision() ?
            lhs.Data.getPrecision() : rhs.Data.getPrecision());

         if (temp.Data.isZero() && (ROUND_NEGATIVE_INFINITY == Fixed::getRoundMode()))
            temp.Sign = true;
       }
      else
       {
         temp = 0 < lhs.Exponent.compare(rhs.Exponent) ? lhs : -rhs;
       }

      return temp;
    }

   Float operator * (const Float & lhs, const Float & rhs)
    {
      Float temp;

         //Again, NaN has precedence
      if (lhs.isNaN() || rhs.isNaN())
       {
         if (lhs.isNaN() && rhs.isNaN())
            temp.NaN = lhs.isNaN() | rhs.isNaN();
         else if (lhs.isNaN())
            temp.NaN = lhs.isNaN();
         else
            temp.NaN = rhs.isNaN();
         return temp;
       }

         //Inf*0 = NaN
      if ((lhs.isInfinity() && rhs.isZero()) ||
          (lhs.isZero() && rhs.isInfinity()))
       {
         temp.NaN = 2;
         return temp;
       }

         //Put this here, as Zero and Infinity need the correct sign.
      temp.Sign = lhs.isSigned() ^ rhs.isSigned();

         //Infinity handling
      if (lhs.isInfinity() || rhs.isInfinity())
       {
         temp.Infinity = true;
         return temp;
       }

         //Zero handling: return a zero with the correct sign
      if (lhs.isZero() || rhs.isZero()) return temp;

       {
            //Do the multiply
         temp.Data = lhs.Data * rhs.Data;
         temp.Exponent = lhs.Exponent + rhs.Exponent;

            //Normalize
         if (temp.Data.getSticky())
          {
            temp.Data.clearSticky();
            temp.Data.setPrecision(temp.Data.getPrecision() + 1);
            temp.Exponent = temp.Exponent + Integer(1U);
          }
       }

      return temp;
    }

   Float operator / (const Float & lhs, const Float & rhs)
    {
      Float temp;

         //Finally, NaN has precedence
      if (lhs.isNaN() || rhs.isNaN())
       {
         if (lhs.isNaN() && rhs.isNaN())
            temp.NaN = lhs.isNaN() | rhs.isNaN();
         else if (lhs.isNaN())
            temp.NaN = lhs.isNaN();
         else
            temp.NaN = rhs.isNaN();
         return temp;
       }

         //Handle the two Inf/Inf and 0/0 NaN cases
      if (lhs.isZero() && rhs.isZero())
       {
         temp.NaN = 4;
         return temp;
       }
      if (lhs.isInfinity() && rhs.isInfinity())
       {
         temp.NaN = 8;
         return temp;
       }

         //Put this here, as Zero and Infinity need the correct sign.
      temp.Sign = lhs.isSigned() ^ rhs.isSigned();

         //Return an infinity
      if (lhs.isInfinity() || rhs.isZero())
       {
         temp.Infinity = true;
         return temp;
       }

         //Return a zero
      if (lhs.isZero() || rhs.isInfinity()) return temp;

       {
            //Do the divide
         temp.Data = lhs.Data / rhs.Data;
         temp.Exponent = lhs.Exponent - rhs.Exponent;

            //Normalize
         if (temp.Data.getSticky())
          {
            temp.Data.clearSticky();
            temp.Data.setPrecision(temp.Data.getPrecision() - 1);
            temp.Exponent = temp.Exponent - Integer(1U);
          }
       }

      return temp;
    }


   std::string Float::toString (void) const
    {
      if (NaN) return std::string ("NaN");
      if (Infinity)
       {
         if (Sign) return std::string ("-Inf");
         else return std::string ("Inf");
       }
      std::string result ("");
      if (Sign) result += "-";
      result += Data.toString();
      result += "e";
      std::ostringstream temp;
      temp << Exponent.toString();
      result += temp.str();
      return result;
    }

   size_t Float::getLength (void) const
    {
      if (NaN)
         return 3;
      if (Infinity)
         return 3 + (Sign ? 1 : 0);
      return Data.getLength() + 1 + Exponent.getLength() + (Sign ? 1 : 0);
    }


   void Float::fromString (const char * src)
    {
      if (!std::strcmp(src, "Inf") || !std::strcmp(src, "+Inf") ||
          !std::strcmp(src, "INF") || !std::strcmp(src, "+INF"))
       {
         Data = FloatFixed(0, minPrecision);
         Sign = false;
         Exponent = Integer(0U);
         Infinity = true;
         NaN = 0;
         return;
       }

      if (!std::strcmp(src, "-Inf") || !std::strcmp(src, "-INF"))
       {
         Data = FloatFixed(0, minPrecision);
         Sign = true;
         Exponent = Integer(0U);
         Infinity = true;
         NaN = 0;
         return;
       }

      if (!std::strcmp(src, "NaN") || !std::strcmp(src, "NAN"))
       {
         Data = FloatFixed(0, minPrecision);
         Sign = false;
         Exponent = Integer(0U);
         Infinity = false;
         NaN = 32;
         return;
       }

      if (*src == '\0')
       {
         Data = FloatFixed(0, minPrecision);
         Sign = false;
         Exponent = Integer(0U);
         Infinity = false;
         NaN = 0;
         return;
       }

      Sign = false;
      Infinity = false;
      NaN = 0;

      const char * iter = src;
      std::string conv;
      bool computeExponent = true, insertSep = true;
      long newExponent = -1;

      conv.reserve(2048);

         // The Fixed is "signless".
      if (*iter == '-')
       {
         Sign = true;
         iter++;
       }
      else if (*iter == '+') iter++;

       /*
         We strip the separator from the string and insert our own to create
         a normalized Fixed portion. Floats created from strings have string
         defined precision.
       */
      if ((*iter == '.') || (*iter == ',')) newExponent++;
      for (;(*iter != '\0') && (*iter != 'e') && (*iter != 'E'); iter++)
       {
         if ((*iter != '.') && (*iter != ',')) conv += *iter;
         else computeExponent = false;
         if (computeExponent) newExponent++;
         if (insertSep)
          {
            conv += '.';
            insertSep = false;
          }
       }
      Data.fromString(conv);

      if ((*iter == 'e') || (*iter == 'E'))
       {
         ++iter;
         if (*iter == '+') ++iter;
         Exponent.fromString(iter);
         Exponent = Exponent + Integer(newExponent);
       }
      else Exponent = Integer(newExponent);

      if (Data.isZero()) Exponent = Integer(0U);

      normalize();
    }


    /*
      This function seems to be orphaned now, as I inlined all
      of the normalization in +, -, *, and / to take advantage
      of a priori knowledge of the results.

      Not really, as + and - really need all this work.
      So does input.
    */
   Float & Float::normalize (void)
    {
      FloatFixed upper ((long long)10, 0), lower ((long long)1, 0);

      if (Data.isZero())
       {
         Exponent = Integer(0U);
         Data.setPrecision(minPrecision);
       }

      else if (Data >= upper)
       {
         Data.setPrecision(Data.getPrecision() + 1);
         Exponent = Exponent + Integer(1U);
       }
      else
       {
         while (lower > Data)
          {
            Data.setPrecision(Data.getPrecision() - 1);
            Exponent = Exponent - Integer(1U);
          }
       }

      if (getPrecision() < getMinPrecision())
         setPrecision(getMinPrecision());
      else if (getPrecision() > getMaxPrecision())
         setPrecision(getMaxPrecision());

      return *this;
    }


   Integer Float::roundToInteger (void) const
    {
      if (Exponent.isSigned() || Data.isZero()) return Integer();

      FloatFixed temp (Data);
      if (Sign) temp = -temp;
      temp.changePrecision(Exponent.toInt(), Fixed::getRoundMode());
      return temp.getRaw();
    }


   Float Float::roundToInteger (Fixed_Round_Mode mode) const
    {
      if (Exponent.isSigned() || Data.isZero()) return Float();

      Float temp (*this);
      if (Sign) temp.Data = -temp.Data;
      temp.Data.changePrecision(Exponent.toInt(), mode);
      temp.Data.changePrecision(Data.getPrecision(), mode);
      if (Sign) temp.Data = -temp.Data;
      return temp;
    }


 } /* namespace BigInt */

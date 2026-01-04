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
#include "libmpdec_NumberSystem.h"
#include "NumberHolder.h"

#include "../external/libmpdec/mpdecimal.h"
#include <cmath>
#include <cctype>

static int ROUND_MODE = MPD_ROUND_HALF_EVEN;
static size_t PRECISION = 34U; // IEEE 754 Quad
static mpd_context_t CONTEXT;

class libdec_NumberHolder final : public NumberHolder
 {
private:
   mpd_t* value;
   size_t precision;

public:
   libdec_NumberHolder() = delete;
   libdec_NumberHolder(const mpd_t* src, size_t precision) : precision(precision)
    {
      value = mpd_qncopy(src);
    }
   explicit libdec_NumberHolder(size_t prec) : precision(prec)
    {
      value = mpd_qnew();
    }
   explicit libdec_NumberHolder(const char* src)
    {
      const char* temp = src;
      size_t prec = 0;
      while ((*temp != 'e') && (*temp != 'E') && (*temp != '\0'))
       {
         if (std::isdigit(*temp))
          {
            ++prec;
          }
         ++temp;
       }
      precision = prec;
      value = mpd_qnew();
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, precision);
      mpd_qsetround(&CONTEXT, ROUND_MODE);
      mpd_qset_string(value, src, &CONTEXT, &trash);
    }
   ~libdec_NumberHolder()
    {
      mpd_del(value);
    }


   virtual std::shared_ptr<NumberHolder> duplicate() const override
    {
      return std::make_shared<libdec_NumberHolder>(value, precision);
    }


   virtual double asDouble() const override
    {
      mpd_t* temp = mpd_qncopy(value);
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, precision);
      mpd_qsetround(&CONTEXT, MPD_ROUND_FLOOR);
      mpd_qround_to_int(temp, temp, &CONTEXT, &trash);
      return static_cast<double>(mpd_qget_ssize(value, &trash));
    }
   virtual std::string toString() const override
    {
      return mpd_to_sci(value, 0);
    }
   virtual std::string toExprString() const override
    {
      if (0 != mpd_isinfinite(value))
       {
         if (0 != mpd_isnegative(value))
          {
            return "-1/0";
          }
         else
          {
            return "1/0";
          }
       }
      else if (0 != mpd_isnan(value))
       {
         if (0 != mpd_isnegative(value))
          {
            return "-0/0";
          }
         else
          {
            return "0/0";
          }
       }
      return toString();
    }
   virtual size_t getLength (void) const override
    {
      return precision; // Good enough for the use of the function.
    }


   virtual bool isSigned() const override { return 0 != mpd_isnegative(value); }
   virtual bool isZero() const override { return 0 != mpd_iszero(value); }
   virtual bool isNaN() const override { return 0 != mpd_isnan(value); }
   virtual bool isInf() const override { return 0 != mpd_isinfinite(value); }
   virtual bool shortMinMax() const override { return 0 != mpd_isnan(value); }


   virtual void round() override
    {
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, precision);
      mpd_qsetround(&CONTEXT, MPD_ROUND_HALF_UP);
      mpd_qround_to_int(value, value, &CONTEXT, &trash);
    }

   virtual void floor() override
    {
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, precision);
      mpd_qsetround(&CONTEXT, MPD_ROUND_FLOOR);
      mpd_qround_to_int(value, value, &CONTEXT, &trash);
    }

   virtual void ceil() override
    {
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, precision);
      mpd_qsetround(&CONTEXT, MPD_ROUND_CEILING);
      mpd_qround_to_int(value, value, &CONTEXT, &trash);
    }



   virtual std::shared_ptr<NumberHolder> operator - () const override
    {
      std::shared_ptr<libdec_NumberHolder> temp = std::make_shared<libdec_NumberHolder>(value, precision);
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, precision);
      mpd_qminus(temp->value, temp->value, &CONTEXT, &trash);
      return temp;
    }


   virtual size_t getPrecision() const override
    {
      return precision;
    }

   virtual void changePrecision(size_t newPrec) override
    {
      if (0U == newPrec)
       {
         newPrec = 1U;
       }
      precision = newPrec;
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, precision);
      mpd_qsetround(&CONTEXT, ROUND_MODE);
      mpd_qplus(value, value, &CONTEXT, &trash);
    }


   virtual bool less (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      if ((0 != mpd_isnan(value)) || (0 != mpd_isnan(RHS.value))) return false;
      uint32_t trash;
      return mpd_qcmp(value, RHS.value, &trash) < 0;
    }

   virtual bool less_equal (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      if ((0 != mpd_isnan(value)) || (0 != mpd_isnan(RHS.value))) return false;
      uint32_t trash;
      return mpd_qcmp(value, RHS.value, &trash) <= 0;
    }

   virtual bool greater (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      if ((0 != mpd_isnan(value)) || (0 != mpd_isnan(RHS.value))) return false;
      uint32_t trash;
      return mpd_qcmp(value, RHS.value, &trash) > 0;
    }

   virtual bool greater_equal (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      if ((0 != mpd_isnan(value)) || (0 != mpd_isnan(RHS.value))) return false;
      uint32_t trash;
      return mpd_qcmp(value, RHS.value, &trash) >= 0;
    }

   virtual bool equal (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      if ((0 != mpd_isnan(value)) || (0 != mpd_isnan(RHS.value))) return false;
      uint32_t trash;
      return mpd_qcmp(value, RHS.value, &trash) == 0;
    }

   virtual bool not_equal_to (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      if ((0 != mpd_isnan(value)) || (0 != mpd_isnan(RHS.value))) return false;
      uint32_t trash;
      return mpd_qcmp(value, RHS.value, &trash) != 0;
    }



   virtual std::shared_ptr<NumberHolder> add (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      size_t prec = std::max(precision, RHS.precision);
      std::shared_ptr<libdec_NumberHolder> temp = std::make_shared<libdec_NumberHolder>(prec);
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, prec);
      mpd_qsetround(&CONTEXT, ROUND_MODE);
      mpd_qadd(temp->value, value, RHS.value, &CONTEXT, &trash);
      return temp;
    }

   virtual std::shared_ptr<NumberHolder> subtract (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      size_t prec = std::max(precision, RHS.precision);
      std::shared_ptr<libdec_NumberHolder> temp = std::make_shared<libdec_NumberHolder>(prec);
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, prec);
      mpd_qsetround(&CONTEXT, ROUND_MODE);
      mpd_qsub(temp->value, value, RHS.value, &CONTEXT, &trash);
      return temp;
    }

   virtual std::shared_ptr<NumberHolder> multiply (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      size_t prec = std::min(precision + RHS.precision, std::max(std::max(precision, RHS.precision), PRECISION));
      std::shared_ptr<libdec_NumberHolder> temp = std::make_shared<libdec_NumberHolder>(prec);
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, prec);
      mpd_qsetround(&CONTEXT, ROUND_MODE);
      mpd_qmul(temp->value, value, RHS.value, &CONTEXT, &trash);
      return temp;
    }

   virtual std::shared_ptr<NumberHolder> divide (const NumberHolder& rhs) const override
    {
      const libdec_NumberHolder& RHS = dynamic_cast<const libdec_NumberHolder&>(rhs);
      std::shared_ptr<libdec_NumberHolder> temp = std::make_shared<libdec_NumberHolder>(PRECISION);
      uint32_t trash;
      mpd_qsetprec(&CONTEXT, PRECISION);
      mpd_qsetround(&CONTEXT, ROUND_MODE);
      mpd_qdiv(temp->value, value, RHS.value, &CONTEXT, &trash);
      return temp;
    }

 };


libmpdec_NumberSystem::libmpdec_NumberSystem() : NumberSystem(
   std::shared_ptr<libdec_NumberHolder>(),
   std::shared_ptr<libdec_NumberHolder>(),
   std::shared_ptr<libdec_NumberHolder>(),
   std::shared_ptr<libdec_NumberHolder>())
 {
   mpd_init(&CONTEXT, PRECISION);
   mpd_qsetround(&CONTEXT, ROUND_MODE);
   FLOAT_ZERO = std::make_shared<libdec_NumberHolder>("0.0");
   FLOAT_ONE = std::make_shared<libdec_NumberHolder>("1.0");
   FLOAT_NAN = std::make_shared<libdec_NumberHolder>("NAN");
   FLOAT_INF = std::make_shared<libdec_NumberHolder>("INF");
 }

libmpdec_NumberSystem::~libmpdec_NumberSystem()
 {
 }


std::shared_ptr<NumberHolder> libmpdec_NumberSystem::fromString(const std::string& src) const
 {
   return fromString(src.c_str());
 }
std::shared_ptr<NumberHolder> libmpdec_NumberSystem::fromString(const char* src) const
 {
   return std::make_shared<libdec_NumberHolder>(src);
 }

std::shared_ptr<NumberHolder> libmpdec_NumberSystem::fromInt(size_t src) const
 {
   return std::make_shared<libdec_NumberHolder>(std::to_string(src).c_str());
 }


void libmpdec_NumberSystem::setRoundMode(NumberSystem_Round_Mode mode)
 {
   switch (mode)
    {
   case ROUND_TIES_EVEN:
      ROUND_MODE = MPD_ROUND_HALF_EVEN;
      currentRoundMode = mode;
      break;
   case ROUND_TIES_AWAY:
      ROUND_MODE = MPD_ROUND_HALF_UP;
      currentRoundMode = mode;
      break;
   case ROUND_POSITIVE_INFINITY:
      ROUND_MODE = MPD_ROUND_CEILING;
      currentRoundMode = mode;
      break;
   case ROUND_NEGATIVE_INFINITY:
      ROUND_MODE = MPD_ROUND_FLOOR;
      currentRoundMode = mode;
      break;
   case ROUND_ZERO:
      ROUND_MODE = MPD_ROUND_DOWN;
      currentRoundMode = mode;
      break;
   case ROUND_TIES_ODD:
      break;
   case ROUND_TIES_ZERO:
      ROUND_MODE = MPD_ROUND_HALF_DOWN;
      currentRoundMode = mode;
      break;
   case ROUND_AWAY:
      ROUND_MODE = MPD_ROUND_UP;
      currentRoundMode = mode;
      break;
   case ROUND_DOUBLE:
      ROUND_MODE = MPD_ROUND_05UP;
      currentRoundMode = mode;
      break;
    }
 }


size_t libmpdec_NumberSystem::getDefaultPrecision() const
 {
   return PRECISION;
 }

void libmpdec_NumberSystem::setDefaultPrecision(size_t newPrec)
 {
   if (0 == newPrec)
    {
      newPrec = 1;
    }
   PRECISION = newPrec;
 }

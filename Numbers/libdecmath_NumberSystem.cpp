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
#include "libdecmath_NumberSystem.h"
#include "NumberHolder.h"

#include "libdecmath/dm_double.h"
#include "libdecmath/dm_double_pretty.h"

#include <cmath>

class libdecmath_NumberHolder final : public NumberHolder
 {
private:
   dm_double value;

public:
   libdecmath_NumberHolder()= delete;
   explicit libdecmath_NumberHolder(dm_double src) : value(src) { }
   ~libdecmath_NumberHolder() { }


   virtual std::shared_ptr<NumberHolder> duplicate() const override
    {
      return std::make_shared<libdecmath_NumberHolder>(value);
    }


   virtual double asDouble() const override
    {
      return dm_double_todouble(value);
    }
   virtual std::string toString() const override { char temp [24]; dm_double_toprettystring(value, temp); return temp; }
   virtual std::string toExprString() const override
    {
      if (1 == dm_double_isinf(value))
       {
         if (1 == dm_double_signbit(value))
          {
            return "-1/0";
          }
         else
          {
            return "1/0";
          }
       }
      else if (1 == dm_double_isnan(value))
       {
         if (1 == dm_double_signbit(value))
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
      return 23U; // -9.999999999999999e-999 ?
    }


   virtual bool isSigned() const override { return 1 == dm_double_signbit(value); }
   virtual bool isZero() const override { return 1 == dm_double_iszero(value); }
   virtual bool isNaN() const override { return 1 == dm_double_isnan(value); }
   virtual bool isInf() const override { return 1 == dm_double_isinf(value); }
   virtual bool shortMinMax() const override { return 1 == dm_double_isnan(value); }


   virtual void round() override
    {
      value = dm_double_round(value);
    }

   virtual void floor() override
    {
      value = dm_double_floor(value);
    }

   virtual void ceil() override
    {
      value = dm_double_ceil(value);
    }



   virtual std::shared_ptr<NumberHolder> operator - () const override
    {
      return std::make_shared<libdecmath_NumberHolder>(dm_double_neg(value));
    }


   virtual size_t getPrecision() const override
    {
      return 16U;
    }

   virtual void changePrecision(size_t) override
    {
    }


   virtual bool less (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return 1 == dm_double_isless(value, RHS.value);
    }

   virtual bool less_equal (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return 1 == dm_double_islessequal(value, RHS.value);
    }

   virtual bool greater (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return 1 == dm_double_isgreater(value, RHS.value);
    }

   virtual bool greater_equal (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return 1 == dm_double_isgreaterequal(value, RHS.value);
    }

   virtual bool equal (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return 1 == dm_double_isequal(value, RHS.value);
    }

   virtual bool not_equal_to (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return 1 == dm_double_isunequal(value, RHS.value);
    }



   virtual std::shared_ptr<NumberHolder> add (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return std::make_shared<libdecmath_NumberHolder>(dm_double_add(value, RHS.value));
    }

   virtual std::shared_ptr<NumberHolder> subtract (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return std::make_shared<libdecmath_NumberHolder>(dm_double_sub(value, RHS.value));
    }

   virtual std::shared_ptr<NumberHolder> multiply (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return std::make_shared<libdecmath_NumberHolder>(dm_double_mul(value, RHS.value));
    }

   virtual std::shared_ptr<NumberHolder> divide (const NumberHolder& rhs) const override
    {
      const libdecmath_NumberHolder& RHS = dynamic_cast<const libdecmath_NumberHolder&>(rhs);
      return std::make_shared<libdecmath_NumberHolder>(dm_double_div(value, RHS.value));
    }

 };


libdecmath_NumberSystem::libdecmath_NumberSystem() : NumberSystem(
   std::make_shared<libdecmath_NumberHolder>(dm_double_fromdouble(0.0)),
   std::make_shared<libdecmath_NumberHolder>(dm_double_fromdouble(1.0)),
   std::make_shared<libdecmath_NumberHolder>(dm_double_fromdouble(std::nan(""))),
   std::make_shared<libdecmath_NumberHolder>(dm_double_fromdouble(INFINITY)))
 {
 }

libdecmath_NumberSystem::~libdecmath_NumberSystem()
 {
 }


std::shared_ptr<NumberHolder> libdecmath_NumberSystem::fromString(const std::string& src) const
 {
   return fromString(src.c_str());
 }
std::shared_ptr<NumberHolder> libdecmath_NumberSystem::fromString(const char* src) const
 {
   return std::make_shared<libdecmath_NumberHolder>(dm_double_fromstring(src));
 }

std::shared_ptr<NumberHolder> libdecmath_NumberSystem::fromInt(size_t src) const
 {
   return std::make_shared<libdecmath_NumberHolder>(dm_double_fromdouble(static_cast<double>(src)));
 }


void libdecmath_NumberSystem::setRoundMode(NumberSystem_Round_Mode mode)
 {
   switch (mode)
    {
   case ROUND_TIES_EVEN:
      dm_fesetround(DM_FE_TONEAREST);
      currentRoundMode = mode;
      break;
   case ROUND_TIES_AWAY:
      dm_fesetround(DM_FE_TONEARESTFROMZERO);
      currentRoundMode = mode;
      break;
   case ROUND_POSITIVE_INFINITY:
      dm_fesetround(DM_FE_UPWARD);
      currentRoundMode = mode;
      break;
   case ROUND_NEGATIVE_INFINITY:
      dm_fesetround(DM_FE_DOWNWARD);
      currentRoundMode = mode;
      break;
   case ROUND_ZERO:
      dm_fesetround(DM_FE_TOWARDZERO);
      currentRoundMode = mode;
      break;
   case ROUND_TIES_ODD:
      dm_fesetround(DM_FE_TONEARESTODD);
      currentRoundMode = mode;
      break;
   case ROUND_TIES_ZERO:
      dm_fesetround(DM_FE_TONEARESTTOWARDZERO);
      currentRoundMode = mode;
      break;
   case ROUND_AWAY:
      dm_fesetround(DM_FE_FROMZERO);
      currentRoundMode = mode;
      break;
   case ROUND_DOUBLE:
      break;
    }
 }


size_t libdecmath_NumberSystem::getDefaultPrecision() const
 {
   return 16U;
 }

void libdecmath_NumberSystem::setDefaultPrecision(size_t)
 {
 }

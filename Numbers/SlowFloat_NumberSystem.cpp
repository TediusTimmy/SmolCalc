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
#include "SlowFloat_NumberSystem.h"
#include "NumberHolder.h"

#include "SlowFloat/SlowFloat.h"

#include <cmath>

class SlowFloat_NumberHolder final : public NumberHolder
 {
private:
   SlowFloat::SlowFloat value;

public:
   SlowFloat_NumberHolder() = delete;
   explicit SlowFloat_NumberHolder(const SlowFloat::SlowFloat& src) : value(src) { }
   ~SlowFloat_NumberHolder() { }


   virtual std::shared_ptr<NumberHolder> duplicate() const override
    {
      return std::make_shared<SlowFloat_NumberHolder>(value);
    }


   virtual double asDouble() const override
    {
      return static_cast<double>(value);
    }
   virtual std::string toString() const override { return SlowFloat::toString(value); }
   virtual std::string toExprString() const override
    {
      if (-32768 == value.exponent)
       {
         if (0U == value.significand)
          {
            return "1/0";
          }
         else if (~0U == value.significand)
          {
            return "-1/0";
          }
         else if ((1U << 31) & value.significand)
          {
            return "-0/0";
          }
         else
          {
            return "0/0";
          }
       }
      return SlowFloat::toString(value);
    }
   virtual size_t getLength (void) const override
    {
      return 18U; // -9.99999999e-99999 ?
    }


   virtual bool isSigned() const override { return 0 != ((1U << 31) & value.significand); }
   virtual bool isZero() const override { return (-32768 != value.exponent) && ((0U == value.significand) || (~0U == value.significand)); }
   virtual bool isNaN() const override { return (-32768 == value.exponent) && ((0U != value.significand) && (~0U != value.significand)); }
   virtual bool isInf() const override { return (-32768 == value.exponent) && ((0U == value.significand) || (~0U == value.significand)); }
   virtual bool shortMinMax() const override { return (-32768 == value.exponent) && ((0U != value.significand) && (~0U != value.significand)); }


   virtual void round() override
    {
      value = SlowFloat::SlowFloat(std::round(static_cast<double>(value)));
    }

   virtual void floor() override
    {
      value = SlowFloat::SlowFloat(std::floor(static_cast<double>(value)));
    }

   virtual void ceil() override
    {
      value = SlowFloat::SlowFloat(std::ceil(static_cast<double>(value)));
    }



   virtual std::shared_ptr<NumberHolder> operator - () const override
    {
      return std::make_shared<SlowFloat_NumberHolder>(-value);
    }


   virtual size_t getPrecision() const override
    {
      return 9U;
    }

   virtual void changePrecision(size_t) override
    {
    }


   virtual bool less (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return value < RHS.value;
    }

   virtual bool less_equal (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return value <= RHS.value;
    }

   virtual bool greater (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return value > RHS.value;
    }

   virtual bool greater_equal (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return value >= RHS.value;
    }

   virtual bool equal (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return value == RHS.value;
    }

   virtual bool not_equal_to (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return value != RHS.value;
    }



   virtual std::shared_ptr<NumberHolder> add (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return std::make_shared<SlowFloat_NumberHolder>(value + RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> subtract (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return std::make_shared<SlowFloat_NumberHolder>(value - RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> multiply (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return std::make_shared<SlowFloat_NumberHolder>(value * RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> divide (const NumberHolder& rhs) const override
    {
      const SlowFloat_NumberHolder& RHS = dynamic_cast<const SlowFloat_NumberHolder&>(rhs);
      return std::make_shared<SlowFloat_NumberHolder>(value / RHS.value);
    }

 };


SlowFloat_NumberSystem::SlowFloat_NumberSystem() : NumberSystem(
   std::make_shared<SlowFloat_NumberHolder>(SlowFloat::SlowFloat(0.0)),
   std::make_shared<SlowFloat_NumberHolder>(SlowFloat::SlowFloat(1.0)),
   std::make_shared<SlowFloat_NumberHolder>(SlowFloat::SlowFloat(std::nan(""))),
   std::make_shared<SlowFloat_NumberHolder>(SlowFloat::SlowFloat(INFINITY)))
 {
 }

SlowFloat_NumberSystem::~SlowFloat_NumberSystem()
 {
 }


std::shared_ptr<NumberHolder> SlowFloat_NumberSystem::fromString(const std::string& src) const
 {
   return std::make_shared<SlowFloat_NumberHolder>(SlowFloat::fromString(src));
 }
std::shared_ptr<NumberHolder> SlowFloat_NumberSystem::fromString(const char* src) const
 {
   return fromString(std::string(src));
 }

std::shared_ptr<NumberHolder> SlowFloat_NumberSystem::fromInt(size_t src) const
 {
   return std::make_shared<SlowFloat_NumberHolder>(SlowFloat::SlowFloat(static_cast<double>(src)));
 }


void SlowFloat_NumberSystem::setRoundMode(NumberSystem_Round_Mode mode)
 {
   switch (mode)
    {
   case ROUND_TIES_EVEN:
      SlowFloat::mode = SlowFloat::ROUND_TIES_EVEN;
      currentRoundMode = mode;
      break;
   case ROUND_TIES_AWAY:
      SlowFloat::mode = SlowFloat::ROUND_TIES_AWAY;
      currentRoundMode = mode;
      break;
   case ROUND_POSITIVE_INFINITY:
      SlowFloat::mode = SlowFloat::ROUND_POSITIVE_INFINITY;
      currentRoundMode = mode;
      break;
   case ROUND_NEGATIVE_INFINITY:
      SlowFloat::mode = SlowFloat::ROUND_NEGATIVE_INFINITY;
      currentRoundMode = mode;
      break;
   case ROUND_ZERO:
      SlowFloat::mode = SlowFloat::ROUND_ZERO;
      currentRoundMode = mode;
      break;
   case ROUND_TIES_ODD:
      SlowFloat::mode = SlowFloat::ROUND_TIES_ODD;
      currentRoundMode = mode;
      break;
   case ROUND_TIES_ZERO:
      SlowFloat::mode = SlowFloat::ROUND_TIES_ZERO;
      currentRoundMode = mode;
      break;
   case ROUND_AWAY:
      SlowFloat::mode = SlowFloat::ROUND_AWAY;
      currentRoundMode = mode;
      break;
   case ROUND_DOUBLE:
      break;
    }
 }


size_t SlowFloat_NumberSystem::getDefaultPrecision() const
 {
   return 9U;
 }

void SlowFloat_NumberSystem::setDefaultPrecision(size_t)
 {
 }

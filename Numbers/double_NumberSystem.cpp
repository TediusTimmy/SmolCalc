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
#include "double_NumberSystem.h"
#include "NumberHolder.h"

#include <cmath>
#include <cfenv>
#include <cstdlib>
#include <sstream>
#include <iomanip>

class double_NumberHolder final : public NumberHolder
 {
private:
   double value;

public:
   double_NumberHolder() = delete;
   explicit double_NumberHolder(double src) : value(src) { }
   ~double_NumberHolder() { }


   virtual std::shared_ptr<NumberHolder> duplicate() const override
    {
      return std::make_shared<double_NumberHolder>(value);
    }


   virtual double asDouble() const override { return value; }
   virtual std::string toString() const override
    {
      std::ostringstream temp;
      temp << std::setprecision(15U) << value;
      return temp.str();
    }
   virtual std::string toExprString() const override
    {
      if (std::isinf(value))
       {
         if (std::signbit(value))
          {
            return "-1/0";
          }
         else
          {
            return "1/0";
          }
       }
      else if (std::isnan(value))
       {
         if (std::signbit(value))
          {
            return "-0/0";
          }
         else
          {
            return "0/0";
          }
       }
      std::ostringstream temp;
      temp << std::setprecision(17U) << value;
      return temp.str();
    }
   virtual size_t getLength (void) const override
    {
      return 23U; // -9.999999999999999e-999 ?
    }


   virtual bool isSigned() const override { return std::signbit(value); }
   virtual bool isZero() const override { return FP_ZERO == std::fpclassify(value); }
   virtual bool isNaN() const override { return std::isnan(value); }
   virtual bool isInf() const override { return std::isinf(value); }
   virtual bool shortMinMax() const override { return std::isnan(value); }


   virtual void round() override { value = std::round(value); }
   virtual void floor() override { value = std::floor(value); }
   virtual void ceil() override { value = std::ceil(value); }



   virtual std::shared_ptr<NumberHolder> operator - () const override
    {
      return std::make_shared<double_NumberHolder>(-value);
    }


   virtual size_t getPrecision() const override
    {
      return 15U;
    }

   virtual void changePrecision(size_t) override
    {
    }


   virtual bool less (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return value < RHS.value;
    }

   virtual bool less_equal (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return value <= RHS.value;
    }

   virtual bool greater (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return value > RHS.value;
    }

   virtual bool greater_equal (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return value >= RHS.value;
    }

   virtual bool equal (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return value == RHS.value;
    }

   virtual bool not_equal_to (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return value != RHS.value;
    }



   virtual std::shared_ptr<NumberHolder> add (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return std::make_shared<double_NumberHolder>(value + RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> subtract (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return std::make_shared<double_NumberHolder>(value - RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> multiply (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return std::make_shared<double_NumberHolder>(value * RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> divide (const NumberHolder& rhs) const override
    {
      const double_NumberHolder& RHS = dynamic_cast<const double_NumberHolder&>(rhs);
      return std::make_shared<double_NumberHolder>(value / RHS.value);
    }

 };


double_NumberSystem::double_NumberSystem() : NumberSystem(
   std::make_shared<double_NumberHolder>(0.0),
   std::make_shared<double_NumberHolder>(1.0),
   std::make_shared<double_NumberHolder>(std::nan("")),
   std::make_shared<double_NumberHolder>(INFINITY))
 {
 }

double_NumberSystem::~double_NumberSystem()
 {
 }


std::shared_ptr<NumberHolder> double_NumberSystem::fromString(const std::string& src) const
 {
   try
    {
      return std::make_shared<double_NumberHolder>(std::stod(src));
    }
   catch (const std::out_of_range&)
    {
      return std::make_shared<double_NumberHolder>(std::nan(""));
    }
 }
std::shared_ptr<NumberHolder> double_NumberSystem::fromString(const char* src) const
 {
   return std::make_shared<double_NumberHolder>(std::strtod(src, nullptr));
 }

std::shared_ptr<NumberHolder> double_NumberSystem::fromInt(size_t src) const
 {
   return std::make_shared<double_NumberHolder>(static_cast<double>(src));
 }


void double_NumberSystem::setRoundMode(NumberSystem_Round_Mode mode)
 {
   switch (mode)
    {
   case ROUND_TIES_EVEN:
      std::fesetround(FE_TONEAREST);
      currentRoundMode = mode;
      break;
   case ROUND_TIES_AWAY:
      break;
   case ROUND_POSITIVE_INFINITY:
      std::fesetround(FE_UPWARD);
      currentRoundMode = mode;
      break;
   case ROUND_NEGATIVE_INFINITY:
      std::fesetround(FE_DOWNWARD);
      currentRoundMode = mode;
      break;
   case ROUND_ZERO:
      std::fesetround(FE_TOWARDZERO);
      currentRoundMode = mode;
      break;
   case ROUND_TIES_ODD:
      break;
   case ROUND_TIES_ZERO:
      break;
   case ROUND_AWAY:
      break;
   case ROUND_DOUBLE:
      break;
    }
 }


size_t double_NumberSystem::getDefaultPrecision() const
 {
   return 15U;
 }

void double_NumberSystem::setDefaultPrecision(size_t)
 {
 }

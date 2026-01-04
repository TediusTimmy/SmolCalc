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
#include "DAPFP_NumberSystem.h"
#include "NumberHolder.h"

#include "DAPFP/Float.hpp"

#include <cmath>

class DAPFP_NumberHolder final : public NumberHolder
 {
private:
   BigInt::Float value;

public:
   DAPFP_NumberHolder() = delete;
   explicit DAPFP_NumberHolder(const BigInt::Float& src) : value(src) { }
   ~DAPFP_NumberHolder() { }


   virtual std::shared_ptr<NumberHolder> duplicate() const override
    {
      return std::make_shared<DAPFP_NumberHolder>(value);
    }


   virtual double asDouble() const override
    {
      if (value.isInfinity())
       {
         if (value.isSigned())
          {
            return -INFINITY;
          }
         else
          {
            return INFINITY;
          }
       }
      else if (value.isNaN())
       {
         return std::nan("");
       }
      return static_cast<double>(value.roundToInteger().toInt());
    }
   virtual std::string toString() const override { return value.toString(); }
   virtual std::string toExprString() const override
    {
      if (value.isInfinity())
       {
         if (value.isSigned())
          {
            return "-1/0";
          }
         else
          {
            return "1/0";
          }
       }
      else if (value.isNaN())
       {
         return "0/0";
       }
      return value.toString();
    }
   virtual size_t getLength (void) const override
    {
      return value.getLength();
    }


   virtual bool isSigned() const override { return value.isSigned(); }
   virtual bool isZero() const override { return value.isZero(); }
   virtual bool isNaN() const override { return value.isNaN(); }
   virtual bool isInf() const override { return value.isInfinity(); }
   virtual bool shortMinMax() const override { return value.isNaN(); }


   virtual void round() override
    {
      value = value.roundToInteger(BigInt::ROUND_TIES_AWAY);
    }

   virtual void floor() override
    {
      value = value.roundToInteger(BigInt::ROUND_NEGATIVE_INFINITY);
    }

   virtual void ceil() override
    {
      value = value.roundToInteger(BigInt::ROUND_POSITIVE_INFINITY);
    }



   virtual std::shared_ptr<NumberHolder> operator - () const override
    {
      return std::make_shared<DAPFP_NumberHolder>(-value);
    }


   virtual size_t getPrecision() const override
    {
      return value.getPrecision();
    }

   virtual void changePrecision(size_t newPrec) override
    {
      value.changePrecision(newPrec);
    }


   virtual bool less (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return value < RHS.value;
    }

   virtual bool less_equal (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return value <= RHS.value;
    }

   virtual bool greater (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return value > RHS.value;
    }

   virtual bool greater_equal (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return value >= RHS.value;
    }

   virtual bool equal (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return value == RHS.value;
    }

   virtual bool not_equal_to (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return value != RHS.value;
    }



   virtual std::shared_ptr<NumberHolder> add (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return std::make_shared<DAPFP_NumberHolder>(value + RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> subtract (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return std::make_shared<DAPFP_NumberHolder>(value - RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> multiply (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return std::make_shared<DAPFP_NumberHolder>(value * RHS.value);
    }

   virtual std::shared_ptr<NumberHolder> divide (const NumberHolder& rhs) const override
    {
      const DAPFP_NumberHolder& RHS = dynamic_cast<const DAPFP_NumberHolder&>(rhs);
      return std::make_shared<DAPFP_NumberHolder>(value / RHS.value);
    }

 };


DAPFP_NumberSystem::DAPFP_NumberSystem() : NumberSystem(
   std::make_shared<DAPFP_NumberHolder>(BigInt::Float("0")),
   std::make_shared<DAPFP_NumberHolder>(BigInt::Float("1")),
   std::make_shared<DAPFP_NumberHolder>(BigInt::Float("NaN")),
   std::make_shared<DAPFP_NumberHolder>(BigInt::Float("Inf")))
 {
 }

DAPFP_NumberSystem::~DAPFP_NumberSystem()
 {
 }


std::shared_ptr<NumberHolder> DAPFP_NumberSystem::fromString(const std::string& src) const
 {
   return fromString(src.c_str());
 }
std::shared_ptr<NumberHolder> DAPFP_NumberSystem::fromString(const char* src) const
 {
   return std::make_shared<DAPFP_NumberHolder>(BigInt::Float(src));
 }

std::shared_ptr<NumberHolder> DAPFP_NumberSystem::fromInt(size_t src) const
 {
   return fromString(std::to_string(src).c_str());
 }


void DAPFP_NumberSystem::setRoundMode(NumberSystem_Round_Mode mode)
 {
   switch (mode)
    {
   case ROUND_TIES_EVEN:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_EVEN);
      break;
   case ROUND_TIES_AWAY:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_AWAY);
      break;
   case ROUND_POSITIVE_INFINITY:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_POSITIVE_INFINITY);
      break;
   case ROUND_NEGATIVE_INFINITY:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_NEGATIVE_INFINITY);
      break;
   case ROUND_ZERO:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_ZERO);
      break;
   case ROUND_TIES_ODD:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_ODD);
      break;
   case ROUND_TIES_ZERO:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_TIES_ZERO);
      break;
   case ROUND_AWAY:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_AWAY);
      break;
   case ROUND_DOUBLE:
      BigInt::Fixed::setRoundMode(BigInt::ROUND_DOUBLE);
      break;
    }
   currentRoundMode = mode;
 }


size_t DAPFP_NumberSystem::getDefaultPrecision() const
 {
   return BigInt::FloatFixed::getDefaultPrecision();
 }

void DAPFP_NumberSystem::setDefaultPrecision(size_t newPrec)
 {
   BigInt::FloatFixed::setDefaultPrecision(static_cast<unsigned long>(newPrec));
 }

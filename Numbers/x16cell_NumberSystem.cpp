/*
BSD 3-Clause License

Copyright (c) 2026, Thomas DiModica
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
#include "x16cell_NumberSystem.h"
#include "NumberHolder.h"

#ifdef __cplusplus
extern "C" {
#endif
#include "x16cell/floats.h"
#ifdef __cplusplus
}
#endif

class x16cell_NumberHolder final : public NumberHolder
 {
private:
   x_float value;

public:
   x16cell_NumberHolder()= delete;
   explicit x16cell_NumberHolder(const x_float src) { float_cpy(value, src); }
   ~x16cell_NumberHolder() { }


   virtual std::shared_ptr<NumberHolder> duplicate() const override
    {
      return std::make_shared<x16cell_NumberHolder>(value);
    }


   virtual double asDouble() const override
    {
      return std::stod(toString());
    }
   virtual std::string toString() const override { char temp [STRING_BUF]; float_to_str(temp, value); return temp; }
   virtual std::string toExprString() const override
    {
      if (isNaN())
       {
         return "1/0";
       }
      return toString();
    }
   virtual size_t getLength (void) const override
    {
      return STRING_BUF - 2U;
    }


   virtual bool isSigned() const override { return (value[0] != 0x80) && (value[1] == 0x80); }
   virtual bool isZero() const override { return (value[0] == 0x80) && (value[1] == 0); }
   virtual bool isNaN() const override { return (value[0] == 0x80) && (value[1] == 0x80); }
   virtual bool isInf() const override { return (value[0] == 0x80) && (value[1] == 0x80); }
   virtual bool shortMinMax() const override { return (value[0] == 0x80) && (value[1] == 0x80); }


   virtual void round() override
    {
      float_round(value);
    }

   virtual void floor() override
    {
      float_trunc(value);
      if (isSigned())
       {
         float_add(value, value, reinterpret_cast<const byte*>("\0\0x80\0x10\0\0"));
       }
    }

   virtual void ceil() override
    {
      float_trunc(value);
      if (!isSigned())
       {
         float_add(value, value, reinterpret_cast<const byte*>("\0\0\0x10\0\0"));
       }
    }



   virtual std::shared_ptr<NumberHolder> operator - () const override
    {
      x_float temp;
      float_cpy(temp, value);
      float_neg(temp);
      return std::make_shared<x16cell_NumberHolder>(temp);
    }


   virtual size_t getPrecision() const override
    {
      return 12U;
    }

   virtual void changePrecision(size_t) override
    {
    }


   static int compare(const x_float lhs, const x_float rhs)
    {
      x_float res, temp;
      float_cpy(temp, rhs);
      float_neg(temp);
      float_add(res, lhs, temp);
      if (res[0] == 0x80)
       {
         if (res[1] == 0x80) // Err -> Uncomparable
          {
            return 42;
          }
         else // 0 -> Equal
          {
            return 0;
          }
       }
      return (res[1] == 0x80) ? -1 : 1;
    }

   virtual bool less (const NumberHolder& rhs) const override
    {
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      int temp = compare(value, RHS.value);
      return (temp == 42) ? false : temp < 0;
    }

   virtual bool less_equal (const NumberHolder& rhs) const override
    {
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      int temp = compare(value, RHS.value);
      return (temp == 42) ? false : temp <= 0;
    }

   virtual bool greater (const NumberHolder& rhs) const override
    {
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      int temp = compare(value, RHS.value);
      return (temp == 42) ? false : temp > 0;
    }

   virtual bool greater_equal (const NumberHolder& rhs) const override
    {
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      int temp = compare(value, RHS.value);
      return (temp == 42) ? false : temp >= 0;
    }

   virtual bool equal (const NumberHolder& rhs) const override
    {
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      int temp = compare(value, RHS.value);
      return (temp == 42) ? false : temp == 0;
    }

   virtual bool not_equal_to (const NumberHolder& rhs) const override
    {
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      int temp = compare(value, RHS.value);
      return (temp == 42) ? false : temp != 0;
    }



   virtual std::shared_ptr<NumberHolder> add (const NumberHolder& rhs) const override
    {
      x_float res;
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      float_add(res, value, RHS.value);
      return std::make_shared<x16cell_NumberHolder>(res);
    }

   virtual std::shared_ptr<NumberHolder> subtract (const NumberHolder& rhs) const override
    {
      x_float res, temp;
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      float_cpy(temp, RHS.value);
      float_neg(temp);
      float_add(res, value, temp);
      return std::make_shared<x16cell_NumberHolder>(res);
    }

   virtual std::shared_ptr<NumberHolder> multiply (const NumberHolder& rhs) const override
    {
      x_float res;
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      float_mul(res, value, RHS.value);
      return std::make_shared<x16cell_NumberHolder>(res);
    }

   virtual std::shared_ptr<NumberHolder> divide (const NumberHolder& rhs) const override
    {
      x_float res;
      const x16cell_NumberHolder& RHS = dynamic_cast<const x16cell_NumberHolder&>(rhs);
      float_div(res, value, RHS.value);
      return std::make_shared<x16cell_NumberHolder>(res);
    }

 };

static std::shared_ptr<NumberHolder> fromString(const char* src)
 {
   x_float res;
   float_from_str(res, src);
   return std::make_shared<x16cell_NumberHolder>(res);
 }


x16cell_NumberSystem::x16cell_NumberSystem() : NumberSystem(
   ::fromString("0"),
   ::fromString("1"),
   ::fromString(""),
   ::fromString(""))
 {
 }

x16cell_NumberSystem::~x16cell_NumberSystem()
 {
 }


std::shared_ptr<NumberHolder> x16cell_NumberSystem::fromString(const std::string& src) const
 {
   return ::fromString(src.c_str());
 }
std::shared_ptr<NumberHolder> x16cell_NumberSystem::fromString(const char* src) const
 {
   return ::fromString(src);
 }

std::shared_ptr<NumberHolder> x16cell_NumberSystem::fromInt(size_t src) const
 {
   return ::fromString(std::to_string(src).c_str());
 }


void x16cell_NumberSystem::setRoundMode(NumberSystem_Round_Mode)
 {
 }


size_t x16cell_NumberSystem::getDefaultPrecision() const
 {
   return 12U;
 }

void x16cell_NumberSystem::setDefaultPrecision(size_t)
 {
 }

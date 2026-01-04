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
#include "mpfr_NumberSystem.h"
#include "NumberHolder.h"

#include <mpfr.h>
#include <cmath>
#include <cctype>

static mpfr_rnd_t ROUND_MODE = MPFR_RNDN;
static size_t PRECISION = 34U; // IEEE 754 Quad
static const double digits2bits = 3.321928094887362347870319429489;

static size_t bitComp(size_t digits)
 {
   return static_cast<size_t>(1U + std::ceil(digits * digits2bits));
 }

class mpfr_NumberHolder final : public NumberHolder
 {
private:
   mpfr_t value;
   size_t precision;

public:
   mpfr_NumberHolder() = delete;
   mpfr_NumberHolder(mpfr_srcptr src, size_t precision) : precision(precision)
    {
      mpfr_init2(value, mpfr_get_prec(src));
      mpfr_set(value, src, ROUND_MODE);
    }
   explicit mpfr_NumberHolder(double src) : precision(PRECISION)
    {
      mpfr_init2(value, PRECISION);
      mpfr_set_d(value, src, ROUND_MODE);
    }
   explicit mpfr_NumberHolder(size_t prec) : precision(prec)
    {
      mpfr_init2(value, bitComp(prec));
    }
   explicit mpfr_NumberHolder(const char* src)
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
      mpfr_init2(value, bitComp(prec));
      mpfr_set_str(value, src, 10, ROUND_MODE);
    }
   ~mpfr_NumberHolder()
    {
      mpfr_clear(value);
    }


   virtual std::shared_ptr<NumberHolder> duplicate() const override
    {
      return std::make_shared<mpfr_NumberHolder>(value, precision);
    }


   virtual double asDouble() const override
    {
      return mpfr_get_d(value, ROUND_MODE);
    }
   virtual std::string toString() const override
    {
      std::string res;
      mp_exp_t exp;

      char * loc = mpfr_get_str(NULL, &exp, 10, precision, value, ROUND_MODE);
      res = loc;
      mpfr_free_str(loc);
      if (0 != mpfr_number_p(value))
       {
         // Special non-zero handling....
         if (0 == mpfr_zero_p(value))
          {
            // Exponent is .XYZeW and we want X.YZeW
            --exp; // So decrement it to normalize it.
          }

         // Add in the decimal place
         if (((res.length() > 1U) && (res[0] != '-')) || ((res.length() > 2U) && (res[0] == '-')))
          {
            if (res[0] == '-')
             {
               res.insert(2U, 1U, '.');
             }
            else
             {
               res.insert(1U, 1U, '.');
             }
          }

         // Add in the exponent
         if (0U != exp)
          {
            res = res + 'E' + std::to_string(exp);
          }
       }
      return res;
    }
   virtual std::string toExprString() const override
    {
      if (0 != mpfr_inf_p(value))
       {
         if (0 != mpfr_signbit(value))
          {
            return "-1/0";
          }
         else
          {
            return "1/0";
          }
       }
      else if (0 != mpfr_nan_p(value))
       {
         if (0 != mpfr_signbit(value))
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


   virtual bool isSigned() const override { return 0 != mpfr_signbit(value); }
   virtual bool isZero() const override { return 0 != mpfr_zero_p(value); }
   virtual bool isNaN() const override { return 0 != mpfr_nan_p(value); }
   virtual bool isInf() const override { return 0 != mpfr_inf_p(value); }
   virtual bool shortMinMax() const override { return 0 != mpfr_nan_p(value); }


   virtual void round() override
    {
      mpfr_round(value, value);
    }

   virtual void floor() override
    {
      mpfr_floor(value, value);
    }

   virtual void ceil() override
    {
      mpfr_ceil(value, value);
    }



   virtual std::shared_ptr<NumberHolder> operator - () const override
    {
      std::shared_ptr<mpfr_NumberHolder> temp = std::make_shared<mpfr_NumberHolder>(value, precision);
      mpfr_neg(temp->value, temp->value, ROUND_MODE);
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
      mpfr_prec_round(value, bitComp(newPrec), ROUND_MODE);
    }


   virtual bool less (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      return 0 != mpfr_less_p(value, RHS.value);
    }

   virtual bool less_equal (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      return 0 != mpfr_lessequal_p(value, RHS.value);
    }

   virtual bool greater (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      return 0 != mpfr_greater_p(value, RHS.value);
    }

   virtual bool greater_equal (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      return 0 != mpfr_greaterequal_p(value, RHS.value);
    }

   virtual bool equal (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      return 0 != mpfr_equal_p(value, RHS.value);
    }

   virtual bool not_equal_to (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      return 0 == mpfr_equal_p(value, RHS.value);
    }



   virtual std::shared_ptr<NumberHolder> add (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      size_t prec = std::max(precision, RHS.precision);
      std::shared_ptr<mpfr_NumberHolder> temp = std::make_shared<mpfr_NumberHolder>(prec);
      mpfr_add(temp->value, value, RHS.value, ROUND_MODE);
      return temp;
    }

   virtual std::shared_ptr<NumberHolder> subtract (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      size_t prec = std::max(precision, RHS.precision);
      std::shared_ptr<mpfr_NumberHolder> temp = std::make_shared<mpfr_NumberHolder>(prec);
      mpfr_sub(temp->value, value, RHS.value, ROUND_MODE);
      return temp;
    }

   virtual std::shared_ptr<NumberHolder> multiply (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      size_t prec = std::min(precision + RHS.precision, std::max(std::max(precision, RHS.precision), PRECISION));
      std::shared_ptr<mpfr_NumberHolder> temp = std::make_shared<mpfr_NumberHolder>(prec);
      mpfr_mul(temp->value, value, RHS.value, ROUND_MODE);
      return temp;
    }

   virtual std::shared_ptr<NumberHolder> divide (const NumberHolder& rhs) const override
    {
      const mpfr_NumberHolder& RHS = dynamic_cast<const mpfr_NumberHolder&>(rhs);
      std::shared_ptr<mpfr_NumberHolder> temp = std::make_shared<mpfr_NumberHolder>(PRECISION);
      mpfr_div(temp->value, value, RHS.value, ROUND_MODE);
      return temp;
    }

 };


mpfr_NumberSystem::mpfr_NumberSystem() : NumberSystem(
   std::make_shared<mpfr_NumberHolder>(0.0),
   std::make_shared<mpfr_NumberHolder>(1.0),
   std::make_shared<mpfr_NumberHolder>(std::nan("")),
   std::make_shared<mpfr_NumberHolder>(INFINITY))
 {
 }

mpfr_NumberSystem::~mpfr_NumberSystem()
 {
 }


std::shared_ptr<NumberHolder> mpfr_NumberSystem::fromString(const std::string& src) const
 {
   return fromString(src.c_str());
 }
std::shared_ptr<NumberHolder> mpfr_NumberSystem::fromString(const char* src) const
 {
   return std::make_shared<mpfr_NumberHolder>(src);
 }

std::shared_ptr<NumberHolder> mpfr_NumberSystem::fromInt(size_t src) const
 {
   return std::make_shared<mpfr_NumberHolder>(static_cast<double>(src));
 }


void mpfr_NumberSystem::setRoundMode(NumberSystem_Round_Mode mode)
 {
   switch (mode)
    {
   case ROUND_TIES_EVEN:
      ROUND_MODE = MPFR_RNDN;
      currentRoundMode = mode;
      break;
   case ROUND_TIES_AWAY:
      break;
   case ROUND_POSITIVE_INFINITY:
      ROUND_MODE = MPFR_RNDU;
      currentRoundMode = mode;
      break;
   case ROUND_NEGATIVE_INFINITY:
      ROUND_MODE = MPFR_RNDD;
      currentRoundMode = mode;
      break;
   case ROUND_ZERO:
      ROUND_MODE = MPFR_RNDZ;
      currentRoundMode = mode;
      break;
   case ROUND_TIES_ODD:
      break;
   case ROUND_TIES_ZERO:
      break;
   case ROUND_AWAY:
      ROUND_MODE = MPFR_RNDA;
      currentRoundMode = mode;
      break;
   case ROUND_DOUBLE:
      break;
    }
 }


size_t mpfr_NumberSystem::getDefaultPrecision() const
 {
   return PRECISION;
 }

void mpfr_NumberSystem::setDefaultPrecision(size_t newPrec)
 {
   if (0U == newPrec)
    {
      newPrec = 1U;
    }
   PRECISION = newPrec;
 }

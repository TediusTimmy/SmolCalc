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
#ifndef NUMBERSYSTEM_H
#define NUMBERSYSTEM_H

#include "NumberHolder.h"

enum NumberSystem_Round_Mode
 {
   ROUND_TIES_EVEN,
   ROUND_TIES_AWAY,
   ROUND_POSITIVE_INFINITY,
   ROUND_NEGATIVE_INFINITY,
   ROUND_ZERO,
   ROUND_TIES_ODD,
   ROUND_TIES_ZERO,
   ROUND_AWAY,
   ROUND_DOUBLE
 };

enum NumberSystem_System
 {
   BCNUM_NUMBER_SYSTEM,
   LIBDECMATH_NUMBER_SYSTEM,
   SLOWFLOAT_NUMBER_SYSTEM,
   DOUBLE_NUMBER_SYSTEM,
   LIBMPDEC_NUMBER_SYSTEM,
   MPFR_NUMBER_SYSTEM,
   DAPFP_NUMBER_SYSTEM
 };

class NumberSystem
 {
private:
   static NumberSystem* currentNumberSystem;
protected:
   static NumberSystem_Round_Mode currentRoundMode;
public:
   static NumberSystem& getCurrentNumberSystem();
   static void setCurrentNumberSystem(NumberSystem_System);

   NumberSystem(const std::shared_ptr<NumberHolder>&, const std::shared_ptr<NumberHolder>&,
      const std::shared_ptr<NumberHolder>&, const std::shared_ptr<NumberHolder>&);
   virtual ~NumberSystem();

   std::shared_ptr<NumberHolder> FLOAT_ZERO;
   std::shared_ptr<NumberHolder> FLOAT_ONE;
   std::shared_ptr<NumberHolder> FLOAT_NAN;
   std::shared_ptr<NumberHolder> FLOAT_INF;

   virtual std::shared_ptr<NumberHolder> fromString(const std::string&) const = 0;
   virtual std::shared_ptr<NumberHolder> fromString(const char*) const = 0;

   virtual std::shared_ptr<NumberHolder> fromInt(size_t) const = 0;

   static NumberSystem_Round_Mode getRoundMode();
   virtual void setRoundMode(NumberSystem_Round_Mode) = 0;

   virtual size_t getDefaultPrecision() const = 0;
   virtual void setDefaultPrecision(size_t) = 0;
 };

#endif /* NUMBERSYSTEM_H */

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
#include "NumberSystem.h"
#include "BCNum_NumberSystem.h"
#include "libdecmath_NumberSystem.h"
#include "SlowFloat_NumberSystem.h"
#include "double_NumberSystem.h"
#include "libmpdec_NumberSystem.h"
#include "mpfr_NumberSystem.h"
#include "DAPFP_NumberSystem.h"
#include "x16cell_NumberSystem.h"

BCNum_NumberSystem system0;
libdecmath_NumberSystem system1;
SlowFloat_NumberSystem system2;
double_NumberSystem system3;
libmpdec_NumberSystem system4;
mpfr_NumberSystem system5;
DAPFP_NumberSystem system6;
x16cell_NumberSystem system7;

NumberSystem* NumberSystem::currentNumberSystem = nullptr;
NumberSystem_Round_Mode NumberSystem::currentRoundMode = ROUND_TIES_EVEN;

NumberSystem& NumberSystem::getCurrentNumberSystem()
 {
   return *currentNumberSystem;
 }

void NumberSystem::setCurrentNumberSystem(NumberSystem_System system)
 {
   switch (system)
    {
   case BCNUM_NUMBER_SYSTEM:
      currentNumberSystem = &system0;
      system0.setRoundMode(currentRoundMode);
      break;
   case LIBDECMATH_NUMBER_SYSTEM:
      currentNumberSystem = &system1;
      system1.setRoundMode(currentRoundMode);
      break;
   case SLOWFLOAT_NUMBER_SYSTEM:
      currentNumberSystem = &system2;
      system2.setRoundMode(currentRoundMode);
      break;
   case DOUBLE_NUMBER_SYSTEM:
      currentNumberSystem = &system3;
      system3.setRoundMode(currentRoundMode);
      break;
   case LIBMPDEC_NUMBER_SYSTEM:
      currentNumberSystem = &system4;
      system4.setRoundMode(currentRoundMode);
      break;
   case MPFR_NUMBER_SYSTEM:
      currentNumberSystem = &system5;
      system5.setRoundMode(currentRoundMode);
      break;
   case DAPFP_NUMBER_SYSTEM:
      currentNumberSystem = &system6;
      system6.setRoundMode(currentRoundMode);
      break;
   case X16CELL_NUMBER_SYSTEM:
      currentNumberSystem = &system7;
      currentRoundMode = ROUND_ZERO;
      break;
    }
 }

NumberSystem::NumberSystem(const std::shared_ptr<NumberHolder>& ZERO, const std::shared_ptr<NumberHolder>& ONE,
   const std::shared_ptr<NumberHolder>& NAN, const std::shared_ptr<NumberHolder>& INF) :
   FLOAT_ZERO(ZERO), FLOAT_ONE(ONE), FLOAT_NAN(NAN), FLOAT_INF(INF)
 {
 }

NumberSystem::~NumberSystem()
 {
 }

NumberSystem_Round_Mode NumberSystem::getRoundMode()
 {
   return currentRoundMode;
 }

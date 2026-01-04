/*
BSD 3-Clause License

Copyright (c) 2022, Thomas DiModica
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
#include <iostream>
#include <iomanip>
#include <cmath>
#include "SlowFloat.h"

/*
const int16_t CUTOFF = 9;

void showProgress(double arg)
 {
   int16_t exponent = static_cast<int16_t>(std::floor(std::log(std::fabs(arg)) / std::log(10.0)));
   std::cout << exponent <<std::endl;
   uint32_t significand = static_cast<uint32_t>(std::fabs(arg) / std::pow(10.0, exponent) * 100000000);
   std::cout << significand <<std::endl;
   double temp = 5.0;
   if ((exponent - CUTOFF + 1) >= 0)
      temp -= (std::fabs(arg) / std::pow(10.0, exponent - CUTOFF + 1) - significand) * 10;
   else
      temp -= (std::fabs(arg) * std::pow(10.0, -(exponent - CUTOFF + 1)) - significand) * 10;

      std::cout << std::scientific << std::setprecision(19);
      std::cout << temp << std::endl;
      std::cout << std::fabs(arg) << std::endl;
      std::cout << std::pow(10.0, exponent - CUTOFF + 1) << std::endl;
      std::cout << (std::fabs(arg) / std::pow(10.0, exponent - CUTOFF + 1)) << std::endl;
      std::cout << (std::fabs(arg) / std::pow(10.0, exponent - CUTOFF + 1) - significand) << std::endl;
      std::cout << std::exp((exponent - CUTOFF + 1) * std::log(10.0)) << std::endl;
      std::cout << (std::fabs(arg) / std::exp((exponent - CUTOFF + 1) * std::log(10.0))) << std::endl;
      std::cout << (std::fabs(arg) / std::exp((exponent - CUTOFF + 1) * std::log(10.0)) - significand) << std::endl;
 }

int main (void)
 {
    {
      showProgress(2.0);
    }
   SlowFloat::mode = SlowFloat::ROUND_POSITIVE_INFINITY;
    {
      SlowFloat::SlowFloat bob (2.0);
      std::cout << bob.significand << " " << bob.exponent << std::endl;
    }
   return 0;
 }
*/
/*
int main (void)
 {
   SlowFloat::SlowFloat bob (7.0);
   std::cout << bob.significand << " " << bob.exponent << std::endl;
   std::cout << std::scientific << std::setprecision(19) << static_cast<double>(bob) << std::endl;
   return 0;
 }
*/

int main (void)
 {
   if (-20 != SlowFloat::SlowFloat(1e-20).exponent) std::cout << "Bad.";
   if (-19 != SlowFloat::SlowFloat(1e-19).exponent) std::cout << "Bad.";
   if (-18 != SlowFloat::SlowFloat(1e-18).exponent) std::cout << "Bad.";
   if (-17 != SlowFloat::SlowFloat(1e-17).exponent) std::cout << "Bad.";
   if (-16 != SlowFloat::SlowFloat(1e-16).exponent) std::cout << "Bad.";
   if (-15 != SlowFloat::SlowFloat(1e-15).exponent) std::cout << "Bad.";
   if (-14 != SlowFloat::SlowFloat(1e-14).exponent) std::cout << "Bad.";
   if (-13 != SlowFloat::SlowFloat(1e-13).exponent) std::cout << "Bad.";
   if (-12 != SlowFloat::SlowFloat(1e-12).exponent) std::cout << "Bad.";
   if (-11 != SlowFloat::SlowFloat(1e-11).exponent) std::cout << "Bad.";
   if (-10 != SlowFloat::SlowFloat(1e-10).exponent) std::cout << "Bad.";
   if ( -9 != SlowFloat::SlowFloat( 1e-9).exponent) std::cout << "Bad.";
   if ( -8 != SlowFloat::SlowFloat( 1e-8).exponent) std::cout << "Bad.";
   if ( -7 != SlowFloat::SlowFloat( 1e-7).exponent) std::cout << "Bad.";
   if ( -6 != SlowFloat::SlowFloat( 1e-6).exponent) std::cout << "Bad.";
   if ( -5 != SlowFloat::SlowFloat( 1e-5).exponent) std::cout << "Bad.";
   if ( -4 != SlowFloat::SlowFloat( 1e-4).exponent) std::cout << "Bad.";
   if ( -3 != SlowFloat::SlowFloat( 1e-3).exponent) std::cout << "Bad.";
   if ( -2 != SlowFloat::SlowFloat( 1e-2).exponent) std::cout << "Bad.";
   if ( -1 != SlowFloat::SlowFloat( 1e-1).exponent) std::cout << "Bad.";
   if (  0 != SlowFloat::SlowFloat(  1e0).exponent) std::cout << "Bad.";
   if (  1 != SlowFloat::SlowFloat(  1e1).exponent) std::cout << "Bad.";
   if (  2 != SlowFloat::SlowFloat(  1e2).exponent) std::cout << "Bad.";
   if (  3 != SlowFloat::SlowFloat(  1e3).exponent) std::cout << "Bad.";
   if (  4 != SlowFloat::SlowFloat(  1e4).exponent) std::cout << "Bad.";
   if (  5 != SlowFloat::SlowFloat(  1e5).exponent) std::cout << "Bad.";
   if (  6 != SlowFloat::SlowFloat(  1e6).exponent) std::cout << "Bad.";
   if (  7 != SlowFloat::SlowFloat(  1e7).exponent) std::cout << "Bad.";
   if (  8 != SlowFloat::SlowFloat(  1e8).exponent) std::cout << "Bad.";
   if (  9 != SlowFloat::SlowFloat(  1e9).exponent) std::cout << "Bad.";
   if ( 10 != SlowFloat::SlowFloat( 1e10).exponent) std::cout << "Bad.";
   if ( 11 != SlowFloat::SlowFloat( 1e11).exponent) std::cout << "Bad.";
   if ( 12 != SlowFloat::SlowFloat( 1e12).exponent) std::cout << "Bad.";
   if ( 13 != SlowFloat::SlowFloat( 1e13).exponent) std::cout << "Bad.";
   if ( 14 != SlowFloat::SlowFloat( 1e14).exponent) std::cout << "Bad.";
   if ( 15 != SlowFloat::SlowFloat( 1e15).exponent) std::cout << "Bad.";
   if ( 16 != SlowFloat::SlowFloat( 1e16).exponent) std::cout << "Bad.";
   if ( 17 != SlowFloat::SlowFloat( 1e17).exponent) std::cout << "Bad.";
   if ( 18 != SlowFloat::SlowFloat( 1e18).exponent) std::cout << "Bad.";
   if ( 19 != SlowFloat::SlowFloat( 1e19).exponent) std::cout << "Bad.";
   if ( 20 != SlowFloat::SlowFloat( 1e20).exponent) std::cout << "Bad.";
   return 0;
 }

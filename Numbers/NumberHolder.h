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
#ifndef NUMBERHOLDER_H
#define NUMBERHOLDER_H

#include <string>
#include <memory>

class NumberHolder
 {
public:
   virtual ~NumberHolder();

   virtual std::shared_ptr<NumberHolder> duplicate() const = 0;

   virtual double asDouble() const = 0; // This is for indexes, so it can just be an integer.
   virtual std::string toString() const = 0;
   virtual std::string toExprString() const = 0; // In case they are different.
   virtual size_t getLength (void) const = 0;

   virtual bool isSigned() const = 0;
   virtual bool isZero() const = 0;
   virtual bool isNaN() const = 0;
   virtual bool isInf() const = 0;
      // This function determines if min/max should return this value always.
      // For most number systems, this is if the number is NaN. For BCNum, Inf is included.
   virtual bool shortMinMax() const = 0;

   virtual void round() = 0;
   virtual void floor() = 0;
   virtual void ceil() = 0;

   virtual std::shared_ptr<NumberHolder> operator - () const = 0;

   virtual size_t getPrecision() const = 0;
   virtual void changePrecision(size_t) = 0;

   virtual bool less (const NumberHolder&) const = 0;
   virtual bool less_equal (const NumberHolder&) const = 0;
   virtual bool greater (const NumberHolder&) const = 0;
   virtual bool greater_equal (const NumberHolder&) const = 0;
   virtual bool equal (const NumberHolder&) const = 0;
   virtual bool not_equal_to (const NumberHolder&) const = 0;

   virtual std::shared_ptr<NumberHolder> add (const NumberHolder&) const = 0;
   virtual std::shared_ptr<NumberHolder> subtract (const NumberHolder&) const = 0;
   virtual std::shared_ptr<NumberHolder> multiply (const NumberHolder&) const = 0;
   virtual std::shared_ptr<NumberHolder> divide (const NumberHolder&) const = 0;
 };

bool operator <  (const NumberHolder&, const NumberHolder&);
bool operator <= (const NumberHolder&, const NumberHolder&);
bool operator >  (const NumberHolder&, const NumberHolder&);
bool operator >= (const NumberHolder&, const NumberHolder&);
bool operator == (const NumberHolder&, const NumberHolder&);
bool operator != (const NumberHolder&, const NumberHolder&);

std::shared_ptr<NumberHolder> operator + (const NumberHolder&, const NumberHolder&);
std::shared_ptr<NumberHolder> operator - (const NumberHolder&, const NumberHolder&);
std::shared_ptr<NumberHolder> operator * (const NumberHolder&, const NumberHolder&);
std::shared_ptr<NumberHolder> operator / (const NumberHolder&, const NumberHolder&);

#endif /* NUMBERHOLDER_H */

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
#ifndef FORWARDS_ENGINE_SHUNTING_H
#define FORWARDS_ENGINE_SHUNTING_H

#include <string>
#include <memory>
#include <vector>

namespace Forwards
 {

namespace Input
 {
   class Token;
   class Lexer;
 }

namespace Types
 {
   class FloatValue;
   class ValueType;
 }

namespace Engine
 {

   class CallingContext;

   class ShuntingYard
    {
   public:

      static std::shared_ptr<Types::ValueType> evaluate (Input::Lexer& src, CallingContext& context);

      static std::shared_ptr<Types::ValueType> Constant (CallingContext&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Plus (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Minus (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Multiply (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Divide (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Equals (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> NotEqual (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Greater (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Less (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> GEQ (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> LEQ (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Cat (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> MakeRange (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> Negate (const Input::Token&, CallingContext&, const std::shared_ptr<Types::ValueType>&);
      static std::shared_ptr<Types::ValueType> FunctionCall (const Input::Token&, CallingContext&, const std::vector<std::shared_ptr<Types::ValueType> >&);
      static std::shared_ptr<Types::ValueType> Name (const Input::Token&, CallingContext&);

      static std::string constructMessage(const std::string&, const Input::Token&);

      static std::shared_ptr<Types::FloatValue> FLOAT_ONE();
      static std::shared_ptr<Types::FloatValue> FLOAT_ZERO();

      static std::shared_ptr<Types::ValueType> cellref (const Input::Token&, size_t, size_t);
    };

 } // namespace Engine

 } // namespace Forwards

#endif /* FORWARDS_ENGINE_STDLIB_H */

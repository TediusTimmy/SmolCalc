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
#include "gtest/gtest.h"

#include "Forwards/Engine/ShuntingYard.h"
#include "Forwards/Engine/CallingContext.h"
#include "Forwards/Engine/SpreadSheet.h"
#include "Forwards/Engine/Cell.h"
#include "Forwards/Engine/StdLib.h"

#include "Forwards/Engine/CellRangeExpand.h"
#include "Forwards/Engine/CellRefEval.h"

#include "Forwards/Parser/ContextBuilder.h"
#include "Forwards/Input/Token.h"

#include "Forwards/Types/FloatValue.h"
#include "Forwards/Types/StringValue.h"
#include "Forwards/Types/NilValue.h"
#include "Forwards/Types/CellRefValue.h"
#include "Forwards/Types/CellRangeValue.h"

#include "Backwards/Engine/FatalException.h"
#include "Backwards/Engine/Logger.h"
#include "Backwards/Engine/ProgrammingException.h"
#include "Backwards/Engine/Statement.h"

#include "Backwards/Input/Lexer.h"
#include "Backwards/Input/LineBufferedStreamInput.h"
#include "Backwards/Input/StringInput.h"

#include "Backwards/Parser/SymbolTable.h"
#include "Backwards/Parser/Parser.h"

#include "Backwards/Types/FloatValue.h"
#include "Backwards/Types/StringValue.h"
#include "Backwards/Types/CellRangeValue.h"
#include "Backwards/Types/CellRefValue.h"

#include "NumberSystem.h"

class StringLogger final : public Backwards::Engine::Logger
 {
public:
   std::vector<std::string> logs;
   void log (const std::string& message) { logs.emplace_back(message); }
   std::string get () { return ""; }
 };

static std::shared_ptr<Forwards::Types::FloatValue> makeFloatValue (const char * str)
 {
   return std::make_shared<Forwards::Types::FloatValue>(NumberSystem::getCurrentNumberSystem().fromString(str));
 }

TEST(EngineTests, testFloats)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> one = makeFloatValue("6");
   std::shared_ptr<Forwards::Types::ValueType> two = makeFloatValue("9");
   std::shared_ptr<Forwards::Types::ValueType> six = makeFloatValue("0");
   std::shared_ptr<Forwards::Types::ValueType> sev = makeFloatValue("-2");
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   StringLogger logger;
   context.logger = &logger;

   res = Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("15"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("-3"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("54"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   NumberSystem::getCurrentNumberSystem().setDefaultPrecision(1U);
   res = Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, two, one); // Flipped args
   NumberSystem::getCurrentNumberSystem().setDefaultPrecision(0U);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1.5"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Negate(Forwards::Input::Token(), context, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("-6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("69", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value); // Nice


   std::shared_ptr<Forwards::Types::ValueType> three = std::make_shared<Forwards::Types::NilValue>();

   res = Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_TRUE(std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value->isInf());

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, two, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, sev, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, two, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, sev, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, sev, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, two, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("6", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);



   res = Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("-6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, three, sev);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, three, sev);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, three, sev);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("6", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);


      // fc?? Constant refuses to return a CellRefValue. If any these functions get a CellRefValue, that is probably a programming error.
   std::shared_ptr<Forwards::Types::ValueType> fa = std::make_shared<Forwards::Types::StringValue>("Hi");
   std::shared_ptr<Forwards::Types::ValueType> fc = std::make_shared<Forwards::Types::CellRangeValue>(1, 1, 1, 1);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

      // Cat with string is valid, to be handled by string. Why? Because this function is YUGE as it is.
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, one, fa), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, fa, one), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);
 }

TEST(EngineTests, testStrings)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> one = std::make_shared<Forwards::Types::StringValue>("F");
   std::shared_ptr<Forwards::Types::ValueType> two = std::make_shared<Forwards::Types::StringValue>("U");
   std::shared_ptr<Forwards::Types::ValueType> six = std::make_shared<Forwards::Types::StringValue>("");
   std::shared_ptr<Forwards::Types::ValueType> sev = std::make_shared<Forwards::Types::StringValue>("A");
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   StringLogger logger;
   context.logger = &logger;

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("FU", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value); // Nice


   std::shared_ptr<Forwards::Types::ValueType> three = std::make_shared<Forwards::Types::NilValue>();

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, two, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

//   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, sev, three);
//   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
//   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, two, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

//   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, sev, three);
//   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
//   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, six, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, two, three);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, three);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("F", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);



   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

//   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, three, sev);
//   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
//   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, three, six);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

//   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, three, sev);
//   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
//   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, three, one);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("F", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);


   std::shared_ptr<Forwards::Types::ValueType> floatCat = makeFloatValue("6");

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, floatCat);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("F6", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, floatCat, one);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("6F", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);



   std::shared_ptr<Forwards::Types::ValueType> fc = std::make_shared<Forwards::Types::CellRangeValue>(1, 1, 1, 1);

      // Cat with string is valid, to be handled by string. Why? Because this function is YUGE as it is.
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Negate(Forwards::Input::Token(), context, one), Backwards::Types::TypedOperationException);
 }

TEST(EngineTests, testOtherNils)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> one = std::make_shared<Forwards::Types::NilValue>();
   std::shared_ptr<Forwards::Types::ValueType> two = std::make_shared<Forwards::Types::NilValue>();
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   StringLogger logger;
   context.logger = &logger;

   res = Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, two, one);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Negate(Forwards::Input::Token(), context, one);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, two);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));


   std::shared_ptr<Forwards::Types::ValueType> fc = std::make_shared<Forwards::Types::CellRangeValue>(1, 1, 1, 1);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Plus(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Minus(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Multiply(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Divide(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Cat(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Equals(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::NotEqual(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Greater(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::Less(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);
   
   EXPECT_THROW(Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::GEQ(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, one, fc), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::LEQ(Forwards::Input::Token(), context, fc, one), Backwards::Types::TypedOperationException);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::Negate(Forwards::Input::Token(), context, fc), Backwards::Types::TypedOperationException);
 }

TEST(EngineTests, testVariousCellRanges)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> A1 = std::make_shared<Forwards::Types::CellRefValue>(false, 0, false, 0);
   std::shared_ptr<Forwards::Types::ValueType> A2 = std::make_shared<Forwards::Types::CellRefValue>(false, 0, true, 0);
   std::shared_ptr<Forwards::Types::ValueType> A3 = std::make_shared<Forwards::Types::CellRefValue>(true, 0, false, 0);
   std::shared_ptr<Forwards::Types::ValueType> A4 = std::make_shared<Forwards::Types::CellRefValue>(true, 0, true, 0);

   std::shared_ptr<Forwards::Types::ValueType> B1 = std::make_shared<Forwards::Types::CellRefValue>(false, 1, false, 1);
   std::shared_ptr<Forwards::Types::ValueType> B2 = std::make_shared<Forwards::Types::CellRefValue>(false, 1, true, 1);
   std::shared_ptr<Forwards::Types::ValueType> B3 = std::make_shared<Forwards::Types::CellRefValue>(true, 1, false, 1);
   std::shared_ptr<Forwards::Types::ValueType> B4 = std::make_shared<Forwards::Types::CellRefValue>(true, 1, true, 1);

   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   StringLogger logger;
   context.logger = &logger;

   Forwards::Engine::CellFrame frame (nullptr, 0U, 0U);
   context.pushCell(&frame);

   res = Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, A1, B1);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));
   EXPECT_EQ(0U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->col1);
   EXPECT_EQ(0U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->row1);
   EXPECT_EQ(1U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->col2);
   EXPECT_EQ(1U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->row2);

      // Yes, because we no longer have toString, we don't test the output
   res = Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, A2, B1);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, A3, B1);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, A4, B1);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, A1, B2);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, A1, B3);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, A1, B4);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, B1, A1);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));
   EXPECT_EQ(0U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->col1);
   EXPECT_EQ(0U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->row1);
   EXPECT_EQ(1U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->col2);
   EXPECT_EQ(1U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->row2);

   std::shared_ptr<Forwards::Types::ValueType> F1 = std::make_shared<Forwards::Types::CellRefValue>(false, -1, true, 1);
   std::shared_ptr<Forwards::Types::ValueType> F2 = std::make_shared<Forwards::Types::CellRefValue>(true, 1, false, -1);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, F1, B1), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, F2, B1), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, B1, F1), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, B1, F2), Backwards::Types::TypedOperationException);

   std::shared_ptr<Forwards::Types::ValueType> one = makeFloatValue("6");

   EXPECT_THROW(Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, one, B1), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::ShuntingYard::MakeRange(Forwards::Input::Token(), context, B1, one), Backwards::Types::TypedOperationException);
 }

TEST(EngineTests, testFinalConst)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   StringLogger logger;
   context.logger = &logger;

   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.sheet.resize(3U);
   shet.sheet[0].resize(3);
   shet.sheet[1].resize(3);
   shet.sheet[2].resize(3);

   shet.sheet[0][0] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[1][1] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[1][1]->previousValue = makeFloatValue("6");

   Forwards::Engine::CellFrame frame (shet.sheet[0][0].get(), 0U, 0U);
   context.pushCell(&frame);

   std::shared_ptr<Forwards::Types::ValueType> A1 = std::make_shared<Forwards::Types::CellRefValue>(false, 1, false, 1);
   std::shared_ptr<Forwards::Types::ValueType> A2 = std::make_shared<Forwards::Types::CellRefValue>(false, 1, true, 1);
   std::shared_ptr<Forwards::Types::ValueType> A3 = std::make_shared<Forwards::Types::CellRefValue>(true, 1, false, 1);
   std::shared_ptr<Forwards::Types::ValueType> A4 = std::make_shared<Forwards::Types::CellRefValue>(true, 1, true, 1);

   res = Forwards::Engine::ShuntingYard::Constant(context, A1);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Constant(context, A2);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Constant(context, A3);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   res = Forwards::Engine::ShuntingYard::Constant(context, A4);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   std::shared_ptr<Forwards::Types::ValueType> F1 = std::make_shared<Forwards::Types::CellRefValue>(false, -1, false, 0);
   EXPECT_NO_THROW(Forwards::Engine::ShuntingYard::Constant(context, F1));

   std::shared_ptr<Forwards::Types::ValueType> F2 = std::make_shared<Forwards::Types::CellRefValue>(false, 0, false, -1);
   EXPECT_NO_THROW(Forwards::Engine::ShuntingYard::Constant(context, F2));

   std::shared_ptr<Forwards::Types::ValueType> B1 = std::make_shared<Forwards::Types::CellRefValue>(false, 4, false, 1);
   std::shared_ptr<Forwards::Types::ValueType> B2 = std::make_shared<Forwards::Types::CellRefValue>(false, 1, false, 4);
   std::shared_ptr<Forwards::Types::ValueType> B3 = std::make_shared<Forwards::Types::CellRefValue>(false, 2, false, 2);
   std::shared_ptr<Forwards::Types::ValueType> B4 = std::make_shared<Forwards::Types::CellRefValue>(false, 0, false, 0);

   res = Forwards::Engine::ShuntingYard::Constant(context, B1);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::Constant(context, B2);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::Constant(context, B3);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   res = Forwards::Engine::ShuntingYard::Constant(context, B4);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   shet.sheet[1][1]->previousValue.reset();
   res = Forwards::Engine::ShuntingYard::Constant(context, A1);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   shet.sheet[1][1]->previousValue = makeFloatValue("9");
   res = Forwards::Engine::ShuntingYard::Constant(context, A1);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("9"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

      // Ensure that a cell that references a cell doesn't return a cell reference.
      // previousValue should NEVER be a cell reference, but let's make sure we get a cell reference
   shet.sheet[0][2] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[0][2]->previousValue = std::make_shared<Forwards::Types::CellRefValue>(true, 1, true, 1);
   std::shared_ptr<Forwards::Types::ValueType> A9 = std::make_shared<Forwards::Types::CellRefValue>(true, 0, true, 2);
   res = Forwards::Engine::ShuntingYard::Constant(context, A9);
   ASSERT_FALSE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   ASSERT_TRUE(typeid(Forwards::Types::CellRefValue) == typeid(*res.get()));
   //EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("9"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
 }

TEST(EngineTests, testFunctionsAndRanges)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   StringLogger logger;
   context.logger = &logger;

   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.sheet.resize(3U);
   shet.sheet[0].resize(3);
   shet.sheet[1].resize(3);
   shet.sheet[2].resize(3);

   shet.sheet[0][0] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[0][1] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[0][2] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[1][0] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[1][1] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[1][2] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[2][0] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[2][1] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[2][2] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[0][0]->previousValue = makeFloatValue("1");
   shet.sheet[0][1]->previousValue = makeFloatValue("2");
   shet.sheet[0][2]->previousValue = makeFloatValue("3");
   shet.sheet[1][0]->previousValue = makeFloatValue("4");
   shet.sheet[1][1]->previousValue = makeFloatValue("5");
   shet.sheet[1][2]->previousValue = makeFloatValue("6");
   shet.sheet[2][0]->previousValue = makeFloatValue("7");
   shet.sheet[2][1]->previousValue = makeFloatValue("8");
   shet.sheet[2][2]->previousValue = makeFloatValue("9");

   Forwards::Engine::CellFrame frame (shet.sheet[0][0].get(), 0U, 0U);
   EXPECT_EQ(nullptr, context.topCell());
   context.pushCell(&frame);


   Forwards::Engine::GetterMap map;
   context.map = &map; // This is now required.

   Backwards::Engine::Scope global;
   context.globalScope = &global;
   Forwards::Parser::ContextBuilder::createGlobalScope(global); // Create the global scope before the table.
   Backwards::Parser::GetterSetter gs;
   Backwards::Parser::SymbolTable table (gs, global);
   Backwards::Input::FileInput console ("../Tests/StdLib.txt");
   Backwards::Input::Lexer lexer (console, "StdLib.txt");

   std::shared_ptr<Backwards::Engine::Statement> stdLib = Backwards::Parser::Parser::ParseFunctions(lexer, table, logger);
   stdLib->execute(context);

   Backwards::Input::StringInput willynilly ("set ARG to function (x) is return x[0] end set BARG to function (x) is return x end");
   Backwards::Input::Lexer lexer2 (willynilly, "NewProgram");

   std::shared_ptr<Backwards::Engine::Statement> stdLib2 = Backwards::Parser::Parser::ParseFunctions(lexer2, table, logger);
   stdLib2->execute(context);

   for (const std::string& name : global.names)
    {
      std::string temp = name;
      std::transform(temp.begin(), temp.end(), temp.begin(), [](unsigned char c){ return std::toupper(c); });
      if (name == temp)
       {
         map.insert(std::make_pair(name, table.getVariableGetter(name)));
       }
    }

   ASSERT_TRUE(map.end() != map.find("SUM"));
   ASSERT_TRUE(map.end() != map.find("ARG"));
   ASSERT_TRUE(map.end() != map.find("BARG"));

   std::vector<std::shared_ptr<Forwards::Types::ValueType> > args;
   args.emplace_back(std::make_shared<Forwards::Types::CellRangeValue>(0, 0, 1, 1));

   res = Forwards::Engine::ShuntingYard::FunctionCall(Forwards::Input::Token(Forwards::Input::IDENTIFIER, "SUM", 1U), context, args);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("12"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   args.emplace_back(std::make_shared<Forwards::Types::CellRangeValue>(0, 0, 1, 0));

   res = Forwards::Engine::ShuntingYard::FunctionCall(Forwards::Input::Token(Forwards::Input::IDENTIFIER, "SUM", 1U), context, args);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("17"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);

   args.clear();
   args.emplace_back(std::make_shared<Forwards::Types::CellRangeValue>(0, 0, 0, 0));

   res = Forwards::Engine::ShuntingYard::FunctionCall(Forwards::Input::Token(Forwards::Input::IDENTIFIER, "SUM", 1U), context, args);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);


   args.clear();
   args.emplace_back(std::make_shared<Forwards::Types::StringValue>("Hi"));

   res = Forwards::Engine::ShuntingYard::FunctionCall(Forwards::Input::Token(Forwards::Input::IDENTIFIER, "ARG", 1U), context, args);
   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
   EXPECT_EQ("Hi", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);

   args.clear();
   args.emplace_back(std::make_shared<Forwards::Types::NilValue>());

   res = Forwards::Engine::ShuntingYard::FunctionCall(Forwards::Input::Token(Forwards::Input::IDENTIFIER, "ARG", 1U), context, args);
   ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));

   args.clear();
   args.emplace_back(std::make_shared<Forwards::Types::CellRangeValue>(0, 1, 1, 2));

   res = Forwards::Engine::ShuntingYard::FunctionCall(Forwards::Input::Token(Forwards::Input::IDENTIFIER, "ARG", 1U), context, args);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*res.get()));
   EXPECT_EQ(0U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->col1);
   EXPECT_EQ(1U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->row1);
   EXPECT_EQ(1U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->col2);
   EXPECT_EQ(2U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(res)->row2);

   args.clear();
   args.emplace_back(std::make_shared<Forwards::Types::NilValue>());

   EXPECT_THROW(Forwards::Engine::ShuntingYard::FunctionCall(Forwards::Input::Token(Forwards::Input::IDENTIFIER, "BARG", 1U), context, args), Backwards::Engine::ProgrammingException);

   std::shared_ptr<Backwards::Engine::CallingContext> copied = context.duplicate();
   std::shared_ptr<Forwards::Engine::CallingContext> casted = std::dynamic_pointer_cast<Forwards::Engine::CallingContext>(copied);
   ASSERT_TRUE(nullptr != casted.get());
   EXPECT_EQ(context.topCell(), casted->topCell());
   EXPECT_EQ(context.theSheet, casted->theSheet);
   EXPECT_EQ(context.logger, casted->logger);

   EXPECT_THROW(Forwards::Engine::ShuntingYard::FunctionCall(Forwards::Input::Token(Forwards::Input::IDENTIFIER, "LARRY", 1U), context, args), Backwards::Types::TypedOperationException);
 }

TEST(EngineTests, testCellRangeExpand)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Backwards::Types::CellRangeValue defaulted (std::make_shared<Forwards::Engine::CellRangeExpand>());
   Backwards::Types::CellRangeValue low (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(0U, 0U, 1U, 1U)));
   Backwards::Types::CellRangeValue high (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(2U, 3U, 4U, 5U)));

   EXPECT_FALSE(low.equal(high));
   EXPECT_TRUE(low.notEqual(high));
   EXPECT_FALSE(low.sort(high));
   EXPECT_TRUE(high.sort(low));
   EXPECT_FALSE(low.sort(low));

   EXPECT_NE(0U, low.hash());
   EXPECT_NE(0U, high.hash());

      // All operations on defaulted will core.
   //EXPECT_THROW(low.equal(defaulted), Backwards::Engine::ProgrammingException);
   //EXPECT_THROW(low.sort(defaulted), Backwards::Engine::ProgrammingException);

   Backwards::Types::CellRangeValue smallest (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(0U, 0U, 0U, 0U)));
   Backwards::Types::CellRangeValue med (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(3U, 5U, 7U, 9U)));
   Backwards::Types::CellRangeValue also1 (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(3U, 4U, 3U, 9U)));
   Backwards::Types::CellRangeValue also2 (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(3U, 4U, 7U, 4U)));

   EXPECT_EQ(1U, smallest.getSize());
   EXPECT_EQ(2U, low.getSize());
   EXPECT_EQ(6U, also1.getSize());
   EXPECT_EQ(5U, also2.getSize());

   std::shared_ptr<Backwards::Types::ValueType> res;
   std::shared_ptr<Forwards::Engine::CellRefEval> temp1;
   std::shared_ptr<Forwards::Engine::CellRangeExpand> temp2;
   std::shared_ptr<Forwards::Types::ValueType> ras;

   res = smallest.getIndex(0U);
   ASSERT_TRUE(typeid(Backwards::Types::CellRefValue) == typeid(*res.get()));
   ASSERT_TRUE(typeid(Forwards::Engine::CellRefEval) == typeid(*std::dynamic_pointer_cast<Backwards::Types::CellRefValue>(res)->value.get()));
   temp1 = std::dynamic_pointer_cast<Forwards::Engine::CellRefEval>(std::dynamic_pointer_cast<Backwards::Types::CellRefValue>(res)->value);
   ASSERT_TRUE(typeid(Forwards::Types::CellRefValue) == typeid(*temp1->value.get()));
   ras = temp1->value;
   EXPECT_EQ(true, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->colAbsolute);
   EXPECT_EQ(0U, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->colRef);
   EXPECT_EQ(true, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->rowAbsolute);
   EXPECT_EQ(0U, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->rowRef);

   res = med.getIndex(1U);
   ASSERT_TRUE(typeid(Backwards::Types::CellRangeValue) == typeid(*res.get()));
   ASSERT_TRUE(typeid(Forwards::Engine::CellRangeExpand) == typeid(*std::dynamic_pointer_cast<Backwards::Types::CellRangeValue>(res)->value.get()));
   temp2 = std::dynamic_pointer_cast<Forwards::Engine::CellRangeExpand>(std::dynamic_pointer_cast<Backwards::Types::CellRangeValue>(res)->value);
   ASSERT_TRUE(typeid(Forwards::Types::CellRangeValue) == typeid(*temp2->value.get()));
   ras = temp2->value;
   EXPECT_EQ(4U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(ras)->col1);
   EXPECT_EQ(5U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(ras)->row1);
   EXPECT_EQ(4U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(ras)->col2);
   EXPECT_EQ(9U, std::dynamic_pointer_cast<Forwards::Types::CellRangeValue>(ras)->row2);

   res = also1.getIndex(2U);
   ASSERT_TRUE(typeid(Backwards::Types::CellRefValue) == typeid(*res.get()));
   ASSERT_TRUE(typeid(Forwards::Engine::CellRefEval) == typeid(*std::dynamic_pointer_cast<Backwards::Types::CellRefValue>(res)->value.get()));
   temp1 = std::dynamic_pointer_cast<Forwards::Engine::CellRefEval>(std::dynamic_pointer_cast<Backwards::Types::CellRefValue>(res)->value);
   ASSERT_TRUE(typeid(Forwards::Types::CellRefValue) == typeid(*temp1->value.get()));
   ras = temp1->value;
   EXPECT_EQ(true, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->colAbsolute);
   EXPECT_EQ(3U, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->colRef);
   EXPECT_EQ(true, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->rowAbsolute);
   EXPECT_EQ(6U, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->rowRef);

   res = also2.getIndex(2U);
   ASSERT_TRUE(typeid(Backwards::Types::CellRefValue) == typeid(*res.get()));
   ASSERT_TRUE(typeid(Forwards::Engine::CellRefEval) == typeid(*std::dynamic_pointer_cast<Backwards::Types::CellRefValue>(res)->value.get()));
   temp1 = std::dynamic_pointer_cast<Forwards::Engine::CellRefEval>(std::dynamic_pointer_cast<Backwards::Types::CellRefValue>(res)->value);
   ASSERT_TRUE(typeid(Forwards::Types::CellRefValue) == typeid(*temp1->value.get()));
   ras = temp1->value;
   EXPECT_EQ(true, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->colAbsolute);
   EXPECT_EQ(5U, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->colRef);
   EXPECT_EQ(true, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->rowAbsolute);
   EXPECT_EQ(4U, std::dynamic_pointer_cast<Forwards::Types::CellRefValue>(ras)->rowRef);

   EXPECT_THROW(smallest.getIndex(2U), Backwards::Engine::ProgrammingException);
   EXPECT_THROW(med.getIndex(9U), Backwards::Engine::ProgrammingException);
   EXPECT_THROW(also1.getIndex(8U), Backwards::Engine::ProgrammingException);
   EXPECT_THROW(also2.getIndex(7U), Backwards::Engine::ProgrammingException);
 }

TEST(EngineTests, testCellRefEval)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Backwards::Types::CellRefValue defaulted (std::make_shared<Forwards::Engine::CellRefEval>());
   Backwards::Types::CellRefValue low (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::CellRefValue>(false, 0, false, 1)));
   Backwards::Types::CellRefValue high (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::CellRefValue>(true, 1, true, 2)));
   Backwards::Types::CellRefValue med (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::StringValue>("Hi")));

   EXPECT_FALSE(low.equal(high));
   EXPECT_TRUE(low.notEqual(high));
   EXPECT_FALSE(low.equal(med));
   EXPECT_TRUE(low.notEqual(med));
   EXPECT_TRUE(low.sort(high) | high.sort(low));
   EXPECT_TRUE(low.sort(med) | med.sort(low));
   EXPECT_FALSE(low.sort(low));

   EXPECT_NE(0U, low.hash());
   EXPECT_NE(0U, high.hash());
   EXPECT_NE(0U, med.hash());

      // All operations on defaulted will core.
      // Now a typeid throws an exception. Different exception, but I won't depend on it.
   //EXPECT_THROW(low.equal(defaulted), Backwards::Engine::ProgrammingException);
   //EXPECT_THROW(low.sort(defaulted), Backwards::Engine::ProgrammingException);
 }

TEST(EngineTests, testCellRefEval_EqualCases)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Backwards::Types::CellRefValue un (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::CellRefValue>(false, 0, false, 0)));
   Backwards::Types::CellRefValue deux (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::CellRefValue>(true, 0, false, 0)));
   Backwards::Types::CellRefValue trois (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::CellRefValue>(false, 0, true, 0)));
   Backwards::Types::CellRefValue quatre (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::CellRefValue>(false, 1, false, 0)));
   Backwards::Types::CellRefValue cinq (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::CellRefValue>(false, 0, false, 1)));
   Backwards::Types::CellRefValue six (std::make_shared<Forwards::Engine::CellRefEval>(
      std::make_shared<Forwards::Types::CellRefValue>(false, 0, false, 0)));

   EXPECT_FALSE(un.equal(deux));
   EXPECT_FALSE(un.equal(trois));
   EXPECT_FALSE(un.equal(quatre));
   EXPECT_FALSE(un.equal(cinq));
   EXPECT_TRUE(un.equal(six));

   EXPECT_TRUE(un.sort(deux) | deux.sort(un));
   EXPECT_TRUE(un.sort(trois) | trois.sort(un));
   EXPECT_TRUE(un.sort(quatre) | quatre.sort(un));
   EXPECT_TRUE(un.sort(cinq) | cinq.sort(un));
   EXPECT_FALSE(un.sort(six) | six.sort(un));
 }

TEST(EngineTests, testCellRangeExpand_EqualCases)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Backwards::Types::CellRangeValue un (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(0U, 0U, 1U, 1U)));
   Backwards::Types::CellRangeValue deux (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(1U, 0U, 1U, 1U)));
   Backwards::Types::CellRangeValue trois (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(0U, 1U, 1U, 1U)));
   Backwards::Types::CellRangeValue quatre (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(0U, 0U, 2U, 1U)));
   Backwards::Types::CellRangeValue cinq (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(0U, 0U, 1U, 2U)));
   Backwards::Types::CellRangeValue six (std::make_shared<Forwards::Engine::CellRangeExpand>(std::make_shared<Forwards::Types::CellRangeValue>(0U, 0U, 1U, 1U)));

   EXPECT_FALSE(un.equal(deux));
   EXPECT_FALSE(un.equal(trois));
   EXPECT_FALSE(un.equal(quatre));
   EXPECT_FALSE(un.equal(cinq));
   EXPECT_TRUE(un.equal(six));

   EXPECT_TRUE(un.sort(deux) | deux.sort(un));
   EXPECT_TRUE(un.sort(trois) | trois.sort(un));
   EXPECT_TRUE(un.sort(quatre) | quatre.sort(un));
   EXPECT_TRUE(un.sort(cinq) | cinq.sort(un));
   EXPECT_FALSE(un.sort(six) | six.sort(un));
 }

TEST(EngineTests, testCellEval)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Backwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   StringLogger logger;
   context.logger = &logger;

   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.sheet.resize(1U);
   shet.sheet[0].resize(1U);

   shet.sheet[0][0] = std::make_unique<Forwards::Engine::Cell>();

   Forwards::Engine::CellFrame frame (shet.sheet[0][0].get(), 0U, 0U);
   EXPECT_EQ(nullptr, context.topCell());
   context.pushCell(&frame);

   Forwards::Engine::GetterMap map;
   context.map = &map;

   res = Forwards::Engine::CellEval(context, std::make_shared<Backwards::Types::StringValue>("2 + 3"));

   ASSERT_TRUE(typeid(Backwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("5"), *std::dynamic_pointer_cast<Backwards::Types::FloatValue>(res)->value);

   EXPECT_THROW(Forwards::Engine::CellEval(context, std::make_shared<Backwards::Types::FloatValue>(NumberSystem::getCurrentNumberSystem().fromString("23"))), Backwards::Types::TypedOperationException);
   EXPECT_THROW(Forwards::Engine::CellEval(context, std::make_shared<Backwards::Types::StringValue>("Hello")), Backwards::Types::TypedOperationException);

   Backwards::Engine::CallingContext badContext;
   EXPECT_THROW(Forwards::Engine::CellEval(badContext, std::make_shared<Backwards::Types::StringValue>("2 + 3")), Backwards::Engine::ProgrammingException);
 }

TEST(EngineTests, testName)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> one = makeFloatValue("6");
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   StringLogger logger;
   context.logger = &logger;
   Forwards::Engine::NameMap names;
   context.names = &names;
   names.insert(std::make_pair("Billy", one));

   res = Forwards::Engine::ShuntingYard::Name(Forwards::Input::Token(Forwards::Input::NAME, "Billy", 0U), context);

   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("6"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);


   res = Forwards::Engine::ShuntingYard::Name(Forwards::Input::Token(Forwards::Input::NAME, "Johnny", 0U), context);

   EXPECT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));
 }

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
#include "gtest/gtest.h"

#include "Forwards/Engine/ShuntingYard.h"
#include "Forwards/Engine/CallingContext.h"
#include "Forwards/Engine/SpreadSheet.h"
#include "Forwards/Engine/Cell.h"

#include "Forwards/Parser/ContextBuilder.h"
#include "Forwards/Parser/StringLogger.h"
#include "Forwards/Input/Lexer.h"

#include "Forwards/Types/FloatValue.h"
#include "Forwards/Types/StringValue.h"
#include "Forwards/Types/NilValue.h"

#include "Backwards/Engine/Statement.h"

#include "Backwards/Input/Lexer.h"
#include "Backwards/Input/LineBufferedStreamInput.h"
#include "Backwards/Input/StringInput.h"

#include "Backwards/Parser/SymbolTable.h"
#include "Backwards/Parser/Parser.h"

#include "NumberSystem.h"

static std::shared_ptr<Forwards::Types::FloatValue> makeFloatValue (const char * str)
 {
   return std::make_shared<Forwards::Types::FloatValue>(NumberSystem::getCurrentNumberSystem().fromString(str));
 }

TEST(EngineTests, testShuntingYardFunctions)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   Forwards::Parser::StringLogger logger;
   context.logger = &logger;

   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.sheet.resize(2U);
   shet.sheet[0].resize(2U);
   shet.sheet[1].resize(2U);

   shet.sheet[0][0] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[0][1] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[1][0] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[1][1] = std::make_unique<Forwards::Engine::Cell>();
   shet.sheet[0][0]->previousValue = makeFloatValue("1");
   shet.sheet[0][1]->previousValue = makeFloatValue("2");
   shet.sheet[1][0]->previousValue = makeFloatValue("4");
   shet.sheet[1][1]->previousValue = makeFloatValue("5");

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

   for (const std::string& name : global.names)
    {
      std::string temp = name;
      std::transform(temp.begin(), temp.end(), temp.begin(), [](unsigned char c){ return std::toupper(c); });
      if (name == temp)
       {
         map.insert(std::make_pair(name, table.getVariableGetter(name)));
       }
    }

    {
      Backwards::Input::StringInput input ("A0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("A$1");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("2"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("$B0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("4"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("$B$1");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("5"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2 A1");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("AA0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::NilValue) == typeid(*res.get()));
    }

    {
      Backwards::Input::StringInput input ("@NAN");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_TRUE(std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value->isNaN());
    }

    {
      Backwards::Input::StringInput input ("@NAN + 2");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_TRUE(std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value->isNaN());
    }

    {
      Backwards::Input::StringInput input ("@NAN()");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_TRUE(std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value->isNaN());
    }

    {
      Backwards::Input::StringInput input ("@MAX(1)");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("@MAX(1;3;2)");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("3"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("@MIN(2-;3)");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("@MIN(1;3;-2)");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("-2"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("@SUM(A0:B1)");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("12"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("12;13;14)");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("-;14)");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("6+(13;14)");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }
 }

TEST(EngineTests, testShuntingYardPrec)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   Forwards::Engine::NameMap names;
   context.names = &names;

    {
      Backwards::Input::StringInput input ("2 - 3 - -4");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("3"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2 + 3 + -4");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2+3*4");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("14"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2*(3+4)");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("14"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2-12/4");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("-1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2-12/-4");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("5"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("\"Hi\"&\"There\"&\"You\"");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get()));
      EXPECT_EQ("HiThereYou", std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2 2");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("2 _Hello");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("2 \"Hi\"");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("2=_Hi=0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2<>_Hi<>0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2<=_Hi<=0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2<_Hi<0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("0"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2>=_Hi>=0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2>_Hi>0");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get()));
      EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("1"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
    }

    {
      Backwards::Input::StringInput input ("2++3");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("(3");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("3)");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("3(");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("3 @NAN");
      Forwards::Input::Lexer lexer (input);
      EXPECT_THROW(Forwards::Engine::ShuntingYard::evaluate(lexer, context), Backwards::Types::TypedOperationException);
    }

    {
      Backwards::Input::StringInput input ("");
      Forwards::Input::Lexer lexer (input);
      res = Forwards::Engine::ShuntingYard::evaluate(lexer, context);
      ASSERT_TRUE(nullptr == res.get());
    }
 }

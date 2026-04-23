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

#include "Forwards/Engine/CallingContext.h"
#include "Forwards/Engine/SpreadSheet.h"
#include "Forwards/Engine/Cell.h"

#include "Forwards/Parser/StringLogger.h"
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
#include "Backwards/Parser/ContextBuilder.h"

#include "NumberSystem.h"

TEST(EngineTests, testSpreadSheet_EasyCases)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   Forwards::Parser::StringLogger logger;
   context.logger = &logger;
   ASSERT_EQ("", logger.get());

   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   EXPECT_EQ(nullptr, shet.getCellAt(0U, 0U));

   shet.initCellAt(3U, 3U);
   EXPECT_NE(nullptr, shet.getCellAt(3U, 3U));
   EXPECT_EQ(4U, shet.max_row);

   shet.initCellAt(2U, 2U);
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));

   shet.initCellAt(3U, 2U);
   ASSERT_NE(nullptr, shet.getCellAt(3U, 2U));

   ASSERT_EQ(nullptr, shet.getCellAt(5U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 5U));

   shet.clearCellAt(3U, 2U);
   EXPECT_EQ(nullptr, shet.getCellAt(3U, 2U));

      // Calling these should do no harm.
   shet.clearCellAt(1U, 5U);
   shet.clearCellAt(5U, 1U);

   EXPECT_EQ("", shet.computeCell(context, res, 5U, 1U));

   std::string hello = "Hello";
   Forwards::Engine::Cell* cell = shet.getCellAt(2U, 2U);
   cell->type = Forwards::Engine::LABEL;
   cell->value = hello;

      // Preconditions
   EXPECT_EQ(nullptr, cell->previousValue.get());

   EXPECT_EQ("", shet.computeCell(context, res, 2U, 2U));

   EXPECT_EQ(hello, cell->value);
   EXPECT_EQ(nullptr, cell->previousValue.get()); // New conditions : not updated for return string.

   ASSERT_TRUE(typeid(Forwards::Types::StringValue) == typeid(*res.get())); // Returned hello
   EXPECT_EQ(hello, std::dynamic_pointer_cast<Forwards::Types::StringValue>(res)->value);


   EXPECT_EQ("", shet.computeCell(context, res, 2U, 2U));

   EXPECT_NE("", cell->value); // Post conditions: cell not updated
   EXPECT_NE(res.get(), cell->previousValue.get());
   EXPECT_NE(nullptr, res.get());
   EXPECT_EQ(nullptr, cell->previousValue.get());

   cell->previousValue.reset();
   EXPECT_EQ("", shet.computeCell(context, res, 2U, 2U));
   EXPECT_NE(nullptr, res.get());


   cell->previousValue.reset();
   EXPECT_NE("", cell->value); // Pre conditions
   EXPECT_EQ(nullptr, cell->previousValue.get());

   EXPECT_EQ("", shet.computeCell(context, res, 2U, 2U));

   EXPECT_NE("", cell->value); // Post conditions: cell not updated
   EXPECT_EQ(nullptr, cell->previousValue.get());

   ASSERT_NE(nullptr, res.get());
 }

TEST(EngineTests, testSpreadSheet_ParseCases)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   Forwards::Parser::StringLogger logger;
   context.logger = &logger;
   ASSERT_EQ("", logger.get());

   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.initCellAt(0U, 0U);

   Forwards::Engine::Cell* cell = shet.getCellAt(0U, 0U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "12 * * 3";

   EXPECT_EQ("Data stack underflow at 4", shet.computeCell(context, res, 0U, 0U));
   EXPECT_EQ(nullptr, res.get());

   cell->value = "12 * 3";

   EXPECT_EQ(nullptr, cell->previousValue.get());

   EXPECT_EQ("", shet.computeCell(context, res, 0U, 0U));

   EXPECT_EQ("12 * 3", cell->value); // Post conditions: no change
   EXPECT_EQ(nullptr, cell->previousValue.get());

   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*res.get())); // Returned 36.0
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("36"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(res)->value);
 }

TEST(EngineTests, testSpreadSheet_ExceptionCases)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::shared_ptr<Forwards::Types::ValueType> res;
   Forwards::Engine::CallingContext context;
   Forwards::Parser::StringLogger logger;
   context.logger = &logger;
   ASSERT_EQ("", logger.get());

   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.initCellAt(0U, 0U);

   Forwards::Engine::Cell* cell = shet.getCellAt(0U, 0U);
   cell->type = Forwards::Engine::VALUE;
   cell->previousValue = std::make_shared<Forwards::Types::FloatValue>(NumberSystem::getCurrentNumberSystem().fromString("12"));

   shet.initCellAt(1U, 0U);

   cell = shet.getCellAt(1U, 0U);
   cell->type = Forwards::Engine::LABEL;
   cell->previousValue = std::make_shared<Forwards::Types::StringValue>("12");

   shet.initCellAt(1U, 1U);

   cell = shet.getCellAt(1U, 1U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "A0+B0";

   EXPECT_EQ("Error adding Float to String at 3", shet.computeCell(context, res, 1U, 1U));
   EXPECT_EQ(nullptr, res.get());

   EXPECT_NO_THROW(shet.computeCell(context, 1U, 1U));

    {
      Backwards::Engine::Scope global;
      context.globalScope = &global;
      Backwards::Parser::ContextBuilder::createGlobalScope(global); // Create the global scope before the table.
      Backwards::Parser::GetterSetter gs;
      Backwards::Parser::SymbolTable table (gs, global);

      Forwards::Engine::GetterMap map;
      context.map = &map;

      Backwards::Input::StringInput bada ("set BAD to function (x) is return 12 + 'Hello' end");
      Backwards::Input::Lexer lexerbad (bada, "BAD");

      std::shared_ptr<Backwards::Engine::Statement> stdLib = Backwards::Parser::Parser::ParseFunctions(lexerbad, table, logger);
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

      ASSERT_TRUE(map.end() != map.find("BAD"));

      cell->value = "@BAD";

      EXPECT_EQ("Error adding Float to String", shet.computeCell(context, res, 1U, 1U));
      EXPECT_EQ(nullptr, res.get());
    }
 }

static std::shared_ptr<Forwards::Types::FloatValue> makeFloatValue (const char* str)
 {
   return std::make_shared<Forwards::Types::FloatValue>(NumberSystem::getCurrentNumberSystem().fromString(str));
 }

TEST(EngineTests, testSpreadSheet_Recalc_TBLR) // A1 is evaluated first. It calls B2, which calls A1, which returns 2.
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Forwards::Engine::CallingContext context;
   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;
   Forwards::Engine::NameMap names;
   context.names = &names;

   shet.initCellAt(0U, 0U);
   shet.initCellAt(1U, 1U);

   Forwards::Engine::Cell* cell = shet.getCellAt(0U, 0U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "B1";
   cell->previousValue = makeFloatValue("3");

   cell = shet.getCellAt(1U, 1U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "A0";
   cell->previousValue = makeFloatValue("2");

   shet.recalc(context);

   cell = shet.getCellAt(0U, 0U);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*cell->previousValue.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("2"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(cell->previousValue)->value);
   cell = shet.getCellAt(1U, 1U);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*cell->previousValue.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("2"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(cell->previousValue)->value);
 }

TEST(EngineTests, testSpreadSheet_Recalc_BTRL) // B2 is evaluated first. It calls A1, which calls B2, which returns 3.
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Forwards::Engine::CallingContext context;
   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;
   Forwards::Engine::NameMap names;
   context.names = &names;

   shet.top_down = false;
   shet.left_right = false;

   shet.initCellAt(0U, 0U);
   shet.initCellAt(1U, 1U);

   Forwards::Engine::Cell* cell = shet.getCellAt(0U, 0U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "B1";
   cell->previousValue = makeFloatValue("3");

   cell = shet.getCellAt(1U, 1U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "A0";
   cell->previousValue = makeFloatValue("2");

   shet.recalc(context);

   cell = shet.getCellAt(0U, 0U);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*cell->previousValue.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("3"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(cell->previousValue)->value);
   cell = shet.getCellAt(1U, 1U);
   ASSERT_TRUE(typeid(Forwards::Types::FloatValue) == typeid(*cell->previousValue.get()));
   EXPECT_EQ(*NumberSystem::getCurrentNumberSystem().fromString("3"), *std::dynamic_pointer_cast<Forwards::Types::FloatValue>(cell->previousValue)->value);
 }

TEST(EngineTests, testSpreadSheet_Recalc_NoHang)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   std::cerr << "WARNING: this unit test will hang on failure." << std::endl;

   Forwards::Engine::CallingContext context;
   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;
   Forwards::Engine::NameMap names;
   context.names = &names;

   shet.initCellAt(0U, 5U);
   shet.initCellAt(1U, 2U);
   shet.initCellAt(4U, 0U);

   Forwards::Engine::Cell* cell = shet.getCellAt(0U, 5U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "12";
   cell = shet.getCellAt(1U, 2U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "15";
   cell = shet.getCellAt(4U, 0U);
   cell->type = Forwards::Engine::VALUE;
   cell->value = "A0";

   shet.c_major = true;
   shet.top_down = true;
   shet.left_right = true;
   shet.recalc(context);

   shet.c_major = true;
   shet.top_down = true;
   shet.left_right = false;
   shet.recalc(context);

   shet.c_major = true;
   shet.top_down = false;
   shet.left_right = true;
   shet.recalc(context);

   shet.c_major = true;
   shet.top_down = false;
   shet.left_right = false;
   shet.recalc(context);

   shet.c_major = false;
   shet.top_down = true;
   shet.left_right = true;
   shet.recalc(context);

   shet.c_major = false;
   shet.top_down = true;
   shet.left_right = false;
   shet.recalc(context);

   shet.c_major = false;
   shet.top_down = false;
   shet.left_right = true;
   shet.recalc(context);

   shet.c_major = false;
   shet.top_down = false;
   shet.left_right = false;
   shet.recalc(context);
 }

TEST(EngineTests, testSpreadSheet_ClearRowColumn)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Forwards::Engine::CallingContext context;
   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.initCellAt(0U, 0U);
   shet.initCellAt(1U, 0U);
   shet.initCellAt(2U, 0U);
   shet.initCellAt(0U, 1U);
   shet.initCellAt(1U, 1U);
   shet.initCellAt(2U, 1U);
   shet.initCellAt(0U, 2U);
   shet.initCellAt(1U, 2U);
   shet.initCellAt(2U, 2U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));

   shet.clearColumn(1U);

   EXPECT_NE(nullptr, shet.getCellAt(0U, 0U));
   EXPECT_EQ(nullptr, shet.getCellAt(1U, 0U));
   EXPECT_NE(nullptr, shet.getCellAt(2U, 0U));
   EXPECT_NE(nullptr, shet.getCellAt(0U, 1U));
   EXPECT_EQ(nullptr, shet.getCellAt(1U, 1U));
   EXPECT_NE(nullptr, shet.getCellAt(2U, 1U));
   EXPECT_NE(nullptr, shet.getCellAt(0U, 2U));
   EXPECT_EQ(nullptr, shet.getCellAt(1U, 2U));
   EXPECT_NE(nullptr, shet.getCellAt(2U, 2U));

   shet.clearColumn(7U); // Shouldn't crash.

   shet.initCellAt(1U, 0U);
   shet.initCellAt(1U, 1U);
   shet.initCellAt(1U, 2U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));

   shet.clearRow(1U);

   EXPECT_NE(nullptr, shet.getCellAt(0U, 0U));
   EXPECT_NE(nullptr, shet.getCellAt(1U, 0U));
   EXPECT_NE(nullptr, shet.getCellAt(2U, 0U));
   EXPECT_EQ(nullptr, shet.getCellAt(0U, 1U));
   EXPECT_EQ(nullptr, shet.getCellAt(1U, 1U));
   EXPECT_EQ(nullptr, shet.getCellAt(2U, 1U));
   EXPECT_NE(nullptr, shet.getCellAt(0U, 2U));
   EXPECT_NE(nullptr, shet.getCellAt(1U, 2U));
   EXPECT_NE(nullptr, shet.getCellAt(2U, 2U));

   shet.clearRow(5U); // Shouldn't crash.
 }

TEST(EngineTests, testSpreadSheet_TestInsertCells)
 {
   Forwards::Engine::CallingContext context;
   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.initCellAt(0U, 0U);
   shet.initCellAt(1U, 0U);
   shet.initCellAt(2U, 0U);
   shet.initCellAt(0U, 1U);
   shet.initCellAt(1U, 1U);
   shet.initCellAt(2U, 1U);
   shet.initCellAt(0U, 2U);
   shet.initCellAt(1U, 2U);
   shet.initCellAt(2U, 2U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(3U, shet.sheet[0].size());
   ASSERT_EQ(3U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(3U, shet.max_row);

   shet.insertCellBeforeShiftDown(1U, 1U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U)); //
   ASSERT_NE(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 3U)); //
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(3U, shet.sheet[0].size());
   ASSERT_EQ(4U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(4U, shet.max_row);

   shet.insertCellBeforeShiftDown(5U, 1U);
   shet.insertCellBeforeShiftDown(1U, 5U);

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(3U, shet.sheet[0].size());
   ASSERT_EQ(4U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(4U, shet.max_row);

   shet.insertCellBeforeShiftRight(1U, 1U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U)); // //
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));    //
   ASSERT_NE(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(3U, 1U));    //
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 3U)); //
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(4U, shet.sheet.size());
   ASSERT_EQ(3U, shet.sheet[0].size());
   ASSERT_EQ(4U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(2U, shet.sheet[3].size());
   ASSERT_EQ(4U, shet.max_row);

   shet.insertCellBeforeShiftRight(5U, 1U);
   shet.insertCellBeforeShiftRight(1U, 5U);

   ASSERT_EQ(4U, shet.sheet.size());
   ASSERT_EQ(3U, shet.sheet[0].size());
   ASSERT_EQ(4U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(2U, shet.sheet[3].size());
   ASSERT_EQ(4U, shet.max_row);

   shet.insertCellBeforeShiftDown(0U, 0U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U)); // //
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));    //
   ASSERT_NE(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(3U, 1U));    //
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 3U)); //
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(4U, shet.sheet.size());
   ASSERT_EQ(4U, shet.sheet[0].size());
   ASSERT_EQ(4U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(2U, shet.sheet[3].size());
   ASSERT_EQ(4U, shet.max_row);


   shet.sheet.clear();
   shet.max_row = 0U;

   shet.initCellAt(1U, 1U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(2U, shet.max_row);

   shet.insertCellBeforeShiftRight(0U, 1U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(2U, shet.sheet[2].size());
   ASSERT_EQ(2U, shet.max_row);
 }

TEST(EngineTests, testSpreadSheet_TestRemoveCells)
 {
   Forwards::Engine::CallingContext context;
   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.initCellAt(0U, 0U);
   shet.initCellAt(1U, 0U);
   shet.initCellAt(2U, 0U);
   shet.initCellAt(0U, 1U);
   shet.initCellAt(1U, 1U);
   shet.initCellAt(2U, 1U);
   shet.initCellAt(0U, 2U);
   shet.initCellAt(1U, 2U);
   shet.initCellAt(2U, 2U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(3U, shet.sheet[0].size());
   ASSERT_EQ(3U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(3U, shet.max_row);

   shet.removeCellShiftLeft(0U, 1U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(3U, shet.sheet[0].size());
   ASSERT_EQ(3U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(3U, shet.max_row);

   shet.removeCellShiftUp(0U, 1U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(2U, shet.sheet[0].size());
   ASSERT_EQ(3U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(3U, shet.max_row);

   shet.removeCellShiftUp(0U, 5U);
   shet.removeCellShiftUp(5U, 0U);

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(2U, shet.sheet[0].size());
   ASSERT_EQ(3U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(3U, shet.max_row);


   shet.sheet.clear();
   shet.max_row = 0U;

   shet.initCellAt(1U, 1U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(2U, shet.max_row);

   shet.removeCellShiftLeft(0U, 1U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(3U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 3U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 3U));

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(2U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(2U, shet.max_row);
 }

TEST(EngineTests, testSpreadSheet_TestRemoveRowCol)
 {
   Forwards::Engine::CallingContext context;
   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.initCellAt(2U, 2U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(2U, 2U));

   ASSERT_EQ(3U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(0U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.sheet[2].size());
   ASSERT_EQ(3U, shet.max_row);

   shet.removeColumn(1U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(3U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.max_row);

   shet.removeRow(1U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.max_row);

   shet.removeColumn(4U);

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(3U, shet.max_row);
 }

TEST(EngineTests, testSpreadSheet_TestInsertRowCol)
 {
   Forwards::Engine::CallingContext context;
   Forwards::Engine::SpreadSheet shet;
   context.theSheet = &shet;

   shet.initCellAt(0U, 0U);

   ASSERT_NE(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));

   ASSERT_EQ(1U, shet.sheet.size());
   ASSERT_EQ(1U, shet.sheet[0].size());
   ASSERT_EQ(1U, shet.max_row);

   shet.insertColumnBefore(0U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(1U, shet.sheet[1].size());
   ASSERT_EQ(1U, shet.max_row);

   shet.insertRowBefore(0U);

   ASSERT_EQ(nullptr, shet.getCellAt(0U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 0U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 1U));
   ASSERT_NE(nullptr, shet.getCellAt(1U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 1U));
   ASSERT_EQ(nullptr, shet.getCellAt(0U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(1U, 2U));
   ASSERT_EQ(nullptr, shet.getCellAt(2U, 2U));

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(2U, shet.max_row);

   shet.insertColumnBefore(4U);

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(2U, shet.max_row);

   shet.insertRowBefore(4U);

   ASSERT_EQ(2U, shet.sheet.size());
   ASSERT_EQ(0U, shet.sheet[0].size());
   ASSERT_EQ(2U, shet.sheet[1].size());
   ASSERT_EQ(2U, shet.max_row);
 }

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

#include "Forwards/Types/ValueType.h"

#include "Forwards/Types/FloatValue.h"
#include "Forwards/Types/StringValue.h"
#include "Forwards/Types/NilValue.h"
#include "Forwards/Types/CellRefValue.h"
#include "Forwards/Types/CellRangeValue.h"

#include "NumberSystem.h"

TEST(TypesTests, testFloats)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Forwards::Types::FloatValue defaulted;
   Forwards::Types::FloatValue low (NumberSystem::getCurrentNumberSystem().fromString("1"));
   Forwards::Types::FloatValue med (NumberSystem::getCurrentNumberSystem().fromString("5"));
   Forwards::Types::FloatValue high (NumberSystem::getCurrentNumberSystem().fromString("10"));

   EXPECT_EQ("Float", defaulted.getTypeName());

   EXPECT_EQ("0", defaulted.toString(0U, 0U, false));
   EXPECT_EQ("1", low.toString(0U, 0U, false));
   EXPECT_EQ("5", med.toString(0U, 0U, false));
   EXPECT_EQ("10", high.toString(0U, 0U, false));
   EXPECT_EQ("10", high.toString(0U, 0U, true));

   EXPECT_EQ(Forwards::Types::FLOAT, defaulted.getType());
   EXPECT_EQ(Forwards::Types::FLOAT, low.getType());
   EXPECT_EQ(Forwards::Types::FLOAT, med.getType());
 }

TEST(TypesTests, testStrings)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Forwards::Types::StringValue defaulted;
   Forwards::Types::StringValue low ("A");
   Forwards::Types::StringValue med ("M");
   Forwards::Types::StringValue high ("Z");

   EXPECT_EQ("String", defaulted.getTypeName());

   EXPECT_EQ("", defaulted.toString(0U, 0U, false));
   EXPECT_EQ("A", low.toString(0U, 0U, false));
   EXPECT_EQ("M", med.toString(0U, 0U, false));
   EXPECT_EQ("Z", high.toString(0U, 0U, false));

   EXPECT_EQ(Forwards::Types::STRING, defaulted.getType());
   EXPECT_EQ(Forwards::Types::STRING, low.getType());
   EXPECT_EQ(Forwards::Types::STRING, med.getType());

   Forwards::Types::StringValue toquote1 ("hello");
   Forwards::Types::StringValue toquote2 ("hello \" there");
   Forwards::Types::StringValue toquote3 ("hello \"there\" you");
   Forwards::Types::StringValue toquote4 ("\"hello there");
   Forwards::Types::StringValue toquote5 ("hello there\"");
   Forwards::Types::StringValue toquote6 ("\"hello there\"");
   Forwards::Types::StringValue toquote7 ("\"");
   Forwards::Types::StringValue toquote8 ("\"\"");

   EXPECT_EQ("\"hello\"", toquote1.toString(0U, 0U, true));
   EXPECT_EQ("\"hello \"\" there\"", toquote2.toString(0U, 0U, true));
   EXPECT_EQ("\"hello \"\"there\"\" you\"", toquote3.toString(0U, 0U, true));
   EXPECT_EQ("\"\"\"hello there\"", toquote4.toString(0U, 0U, true));
   EXPECT_EQ("\"hello there\"\"\"", toquote5.toString(0U, 0U, true));
   EXPECT_EQ("\"\"\"hello there\"\"\"", toquote6.toString(0U, 0U, true));
   EXPECT_EQ("\"\"\"\"", toquote7.toString(0U, 0U, true));
   EXPECT_EQ("\"\"\"\"\"\"", toquote8.toString(0U, 0U, true));
 }

TEST(TypesTests, testNil)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Forwards::Types::NilValue defaulted;
   Forwards::Types::NilValue low;
   Forwards::Types::NilValue med;
   Forwards::Types::NilValue high;

   EXPECT_EQ("Nil", defaulted.getTypeName());

   EXPECT_EQ("Nil", defaulted.toString(0U, 0U, false));
   EXPECT_EQ("Nil", low.toString(0U, 0U, false));
   EXPECT_EQ("Nil", med.toString(0U, 0U, false));
   EXPECT_EQ("Nil", high.toString(0U, 0U, false));

   EXPECT_EQ(Forwards::Types::NIL, defaulted.getType());
   EXPECT_EQ(Forwards::Types::NIL, low.getType());
   EXPECT_EQ(Forwards::Types::NIL, med.getType());
 }

TEST(TypesTests, testCellRef)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Forwards::Types::CellRefValue defaulted;
   Forwards::Types::CellRefValue low (true, 5, false, 5);
   Forwards::Types::CellRefValue med (false, 6, true, 6);
   Forwards::Types::CellRefValue high (true, 3, true, 4);
   Forwards::Types::CellRefValue _11 (false, -5, false, -4);

   EXPECT_EQ("CellRef", defaulted.getTypeName());

   EXPECT_EQ("B1", defaulted.toString(1U, 1U, false));
   EXPECT_EQ("F5", defaulted.toString(5U, 5U, false));
   EXPECT_EQ("$F6", low.toString(1U, 1U, false));
   EXPECT_EQ("$F10", low.toString(5U, 5U, false));
   EXPECT_EQ("H$6", med.toString(1U, 1U, false));
   EXPECT_EQ("M$6", med.toString(6U, 6U, false));
   EXPECT_EQ("$D$4", high.toString(1U, 1U, false));
   EXPECT_EQ("$D$4", high.toString(8U, 8U, false));
   EXPECT_EQ("B1", _11.toString(6U, 5U, false));
   EXPECT_EQ("G5", _11.toString(11U, 9U, false));

   EXPECT_EQ("Z0", defaulted.toString(25U, 0U, false));
   EXPECT_EQ("AA0", defaulted.toString(26U, 0U, false));
   EXPECT_EQ("ZZ0", defaulted.toString(701U, 0U, false));

   EXPECT_EQ(Forwards::Types::CELL_REF, defaulted.getType());
   EXPECT_EQ(Forwards::Types::CELL_REF, low.getType());
   EXPECT_EQ(Forwards::Types::CELL_REF, med.getType());

   EXPECT_EQ("ZW99997", _11.toString(1U, 1U, false));
   EXPECT_EQ("ZW0", _11.toString(1U, 4U, false));
   EXPECT_EQ("ZZ99999", _11.toString(4U, 3U, false));

   Forwards::Types::CellRefValue _12 (false, 5, false, 4);
   EXPECT_EQ("ZZ99999", _12.toString(702U - 6U, 99999U - 4U, false));
   EXPECT_EQ("A0", _12.toString(702U - 5U, 99999U - 3U, false));
 }

TEST(TypesTests, testCellRange)
 {
   NumberSystem::setCurrentNumberSystem(BCNUM_NUMBER_SYSTEM);
   Forwards::Types::CellRangeValue defaulted;
   Forwards::Types::CellRangeValue low (1, 1, 2, 2);
   Forwards::Types::CellRangeValue med (3, 5, 7, 9);
   Forwards::Types::CellRangeValue high (9, 7, 5, 3); // Not a real case, but no error checking.

   EXPECT_EQ("CellRange", defaulted.getTypeName());

   EXPECT_EQ("B1:B1", defaulted.toString(0U, 0U, false));
   EXPECT_EQ("B1:B1", defaulted.toString(5U, 5U, false));
   EXPECT_EQ("B1:C2", low.toString(0U, 0U, false));
   EXPECT_EQ("B1:C2", low.toString(5U, 5U, false));
   EXPECT_EQ("D5:H9", med.toString(0U, 0U, false));
   EXPECT_EQ("D5:H9", med.toString(6U, 6U, false));
   EXPECT_EQ("J7:F3", high.toString(0U, 0U, false));
   EXPECT_EQ("J7:F3", high.toString(8U, 8U, false));

   EXPECT_EQ(Forwards::Types::CELL_RANGE, defaulted.getType());
   EXPECT_EQ(Forwards::Types::CELL_RANGE, low.getType());
   EXPECT_EQ(Forwards::Types::CELL_RANGE, med.getType());
 }

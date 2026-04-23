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
#include "Forwards/Engine/ShuntingYard.h"
#include "Forwards/Engine/CallingContext.h"
#include "Forwards/Engine/SpreadSheet.h"
#include "Forwards/Engine/Cell.h"
#include "Forwards/Engine/CellRefEval.h"
#include "Forwards/Engine/CellRangeExpand.h"

#include "Forwards/Input/Token.h"

#include "Forwards/Types/FloatValue.h"
#include "Forwards/Types/StringValue.h"
#include "Forwards/Types/NilValue.h"
#include "Forwards/Types/CellRefValue.h"
#include "Forwards/Types/CellRangeValue.h"

#include "Backwards/Types/FloatValue.h"
#include "Backwards/Types/StringValue.h"
#include "Backwards/Types/ArrayValue.h"
#include "Backwards/Types/NilValue.h"
#include "Backwards/Types/CellRefValue.h"
#include "Backwards/Types/CellRangeValue.h"

#include "Backwards/Engine/Expression.h"
#include "Backwards/Engine/ProgrammingException.h"

#include "NumberSystem.h"

#include <sstream>

namespace Forwards
 {

namespace Engine
 {

   std::string ShuntingYard::constructMessage(const std::string& e, const Input::Token& token)
    {
      std::stringstream str;
      str << e << " at " << token.location;
      throw Backwards::Types::TypedOperationException(str.str());
    }

   std::shared_ptr<Types::FloatValue> ShuntingYard::FLOAT_ONE()
    {
      return std::make_shared<Types::FloatValue>(NumberSystem::getCurrentNumberSystem().FLOAT_ONE);
    }

   std::shared_ptr<Types::FloatValue> ShuntingYard::FLOAT_ZERO()
    {
      return std::make_shared<Types::FloatValue>(NumberSystem::getCurrentNumberSystem().FLOAT_ZERO);
    }


   static std::shared_ptr<Types::ValueType> finalConst (std::shared_ptr<Types::CellRefValue> value, CallingContext& context)
    {
         // Determine column and row.
      int64_t col, row;
      if ((true == value->colAbsolute) && (true == value->rowAbsolute))
       {
         col = value->colRef;
         row = value->rowRef;
       }
      else if (true == value->colAbsolute)
       {
         col = value->colRef;
         row = Types::CellRefValue::getRow(context.topCell()->row, value->rowRef);
       }
      else if (true == value->rowAbsolute)
       {
         col = Types::CellRefValue::getColumn(context.topCell()->col, value->colRef);
         row = value->rowRef;
       }
      else
       {
         col = Types::CellRefValue::getColumn(context.topCell()->col, value->colRef);
         row = Types::CellRefValue::getRow(context.topCell()->row, value->rowRef);
       }

      Cell* cell = context.theSheet->getCellAt(col, row);
         // If no cell, Nil.
      if (nullptr == cell)
       {
         return std::make_shared<Types::NilValue>();
       }

      std::shared_ptr<Types::ValueType> result = cell->previousValue;
      if (nullptr == result.get())
       {
         result = std::make_shared<Types::NilValue>();
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Constant (CallingContext& context, const std::shared_ptr<Types::ValueType>& value)
    {
      std::shared_ptr<Types::ValueType> result = value;
      if (Types::CELL_REF == result->getType())
       {
         result = finalConst(std::static_pointer_cast<Types::CellRefValue>(result), context);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Plus (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = std::make_shared<Types::FloatValue>(*static_cast<Types::FloatValue*>(LHS.get())->value + *static_cast<Types::FloatValue*>(RHS.get())->value);
            break;
         case Types::NIL:
            result = LHS; // This is why we do math this way, instead of the better Backwards way.
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error adding " + LHS->getTypeName() + " to " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = RHS;
            break;
         case Types::NIL:
            result = LHS;
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error adding " + LHS->getTypeName() + " to " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error adding " + LHS->getTypeName() + " to " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Minus (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = std::make_shared<Types::FloatValue>(*static_cast<Types::FloatValue*>(LHS.get())->value - *static_cast<Types::FloatValue*>(RHS.get())->value);
            break;
         case Types::NIL:
            result = LHS;
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error subtracting " + RHS->getTypeName() + " from " + LHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = std::make_shared<Types::FloatValue>(-*static_cast<Types::FloatValue*>(RHS.get())->value);
            break;
         case Types::NIL:
            result = LHS;
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error subtracting " + RHS->getTypeName() + " from " + LHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error subtracting " + RHS->getTypeName() + " from " + LHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Multiply (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = std::make_shared<Types::FloatValue>(*static_cast<Types::FloatValue*>(LHS.get())->value * *static_cast<Types::FloatValue*>(RHS.get())->value);
            break;
         case Types::NIL:
            result = FLOAT_ZERO();
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error multiplying " + LHS->getTypeName() + " by " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = FLOAT_ZERO();
            break;
         case Types::NIL:
            result = LHS;
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error multiplying " + LHS->getTypeName() + " by " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error multiplying " + LHS->getTypeName() + " by " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Divide (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = std::make_shared<Types::FloatValue>(*static_cast<Types::FloatValue*>(LHS.get())->value / *static_cast<Types::FloatValue*>(RHS.get())->value);
            break;
         case Types::NIL: // Nil is a positive zero, and preserve the sign of infinity.
            result = std::make_shared<Types::FloatValue>(*static_cast<Types::FloatValue*>(LHS.get())->value / *NumberSystem::getCurrentNumberSystem().FLOAT_ZERO);
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error dividing " + LHS->getTypeName() + " by " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = FLOAT_ZERO();
            break;
         case Types::NIL:
            result = LHS;
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error dividing " + LHS->getTypeName() + " by " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error dividing " + LHS->getTypeName() + " by " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Cat (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            // We will abuse the fact that FLOAT and STRING don't care what cell they are in.
            result = std::make_shared<Types::StringValue>(LHS->toString(0U, 0U, false) + RHS->toString(0U, 0U, false));
            break;
         case Types::NIL:
            result = std::make_shared<Types::StringValue>(LHS->toString(0U, 0U, false));
            break;
         case Types::STRING:
            result = std::make_shared<Types::StringValue>(LHS->toString(0U, 0U, false) + RHS->toString(0U, 0U, false));
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error catenating " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = std::make_shared<Types::StringValue>(LHS->toString(0U, 0U, false) + RHS->toString(0U, 0U, false));
            break;
         case Types::NIL:
            result = LHS;
            break;
         case Types::STRING:
            result = std::make_shared<Types::StringValue>(LHS->toString(0U, 0U, false) + RHS->toString(0U, 0U, false));
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error catenating " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            result = std::make_shared<Types::StringValue>(RHS->toString(0U, 0U, false));
            break;
         case Types::NIL:
            result = LHS;
            break;
         case Types::STRING:
            result = RHS;
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error catenating " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error catenating " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::MakeRange (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::CellRefValue> LHS = std::dynamic_pointer_cast<Types::CellRefValue>(lhs);
      std::shared_ptr<Types::CellRefValue> RHS = std::dynamic_pointer_cast<Types::CellRefValue>(rhs);
      if ((nullptr == LHS.get()) || (nullptr == RHS.get()))
       {
         constructMessage("Error MakeRange with " + LHS->getTypeName() + " and " + RHS->getTypeName(), tok);
       }

         // Determine column and row.
      int64_t col1, row1;
      if ((true == LHS->colAbsolute) && (true == LHS->rowAbsolute))
       {
         col1 = LHS->colRef;
         row1 = LHS->rowRef;
       }
      else if (true == LHS->colAbsolute)
       {
         col1 = LHS->colRef;
         row1 = context.topCell()->row + LHS->rowRef;
       }
      else if (true == LHS->rowAbsolute)
       {
         col1 = context.topCell()->col + LHS->colRef;
         row1 = LHS->rowRef;
       }
      else
       {
         col1 = context.topCell()->col + LHS->colRef;
         row1 = context.topCell()->row + LHS->rowRef;
       }
      int64_t col2, row2;
      if ((true == RHS->colAbsolute) && (true == RHS->rowAbsolute))
       {
         col2 = RHS->colRef;
         row2 = RHS->rowRef;
       }
      else if (true == RHS->colAbsolute)
       {
         col2 = RHS->colRef;
         row2 = context.topCell()->row + RHS->rowRef;
       }
      else if (true == RHS->rowAbsolute)
       {
         col2 = context.topCell()->col + RHS->colRef;
         row2 = RHS->rowRef;
       }
      else
       {
         col2 = context.topCell()->col + RHS->colRef;
         row2 = context.topCell()->row + RHS->rowRef;
       }

         // Validate
      if ((col1 < 0) || (col2 < 0) || (row1 < 0) || (row2 < 0))
       {
         constructMessage("Invalid cell reference", tok);
       }

      if (col1 > col2)
       {
         std::swap(col1, col2);
       }
      if (row1 > row2)
       {
         std::swap(row1, row2);
       }

      return std::make_shared<Types::CellRangeValue>(col1, row1, col2, row2);
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Equals (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value == *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (static_cast<Types::FloatValue*>(LHS.get())->value->isZero())
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
         switch (RHS->getType())
          {
         case Types::STRING:
            if (static_cast<Types::StringValue*>(LHS.get())->value == static_cast<Types::StringValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (static_cast<Types::StringValue*>(LHS.get())->value.empty())
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::FLOAT:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (static_cast<Types::FloatValue*>(RHS.get())->value->isZero())
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            result = FLOAT_ONE();
            break;
         case Types::STRING:
            if (static_cast<Types::StringValue*>(RHS.get())->value.empty())
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::NotEqual (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value != *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (!static_cast<Types::FloatValue*>(LHS.get())->value->isZero())
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
         switch (RHS->getType())
          {
         case Types::STRING:
            if (static_cast<Types::StringValue*>(LHS.get())->value != static_cast<Types::StringValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (!static_cast<Types::StringValue*>(LHS.get())->value.empty())
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::FLOAT:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (!static_cast<Types::FloatValue*>(RHS.get())->value->isZero())
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            result = FLOAT_ZERO();
            break;
         case Types::STRING:
            if (!static_cast<Types::StringValue*>(RHS.get())->value.empty())
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Greater (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value > *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value > *NumberSystem::getCurrentNumberSystem().FLOAT_ZERO)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
         switch (RHS->getType())
          {
         case Types::STRING:
            if (static_cast<Types::StringValue*>(LHS.get())->value > static_cast<Types::StringValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (static_cast<Types::StringValue*>(LHS.get())->value > "")
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::FLOAT:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*NumberSystem::getCurrentNumberSystem().FLOAT_ZERO > *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
         case Types::STRING:
            result = FLOAT_ZERO();
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Less (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value < *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value < *NumberSystem::getCurrentNumberSystem().FLOAT_ZERO)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
         switch (RHS->getType())
          {
         case Types::STRING:
            if (static_cast<Types::StringValue*>(LHS.get())->value < static_cast<Types::StringValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            result = FLOAT_ZERO();
            break;
         case Types::FLOAT:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*NumberSystem::getCurrentNumberSystem().FLOAT_ZERO < *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            result = FLOAT_ZERO();
            break;
         case Types::STRING:
            if ("" < static_cast<Types::StringValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::GEQ (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value >= *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value >= *NumberSystem::getCurrentNumberSystem().FLOAT_ZERO)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
         switch (RHS->getType())
          {
         case Types::STRING:
            if (static_cast<Types::StringValue*>(LHS.get())->value >= static_cast<Types::StringValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            result = FLOAT_ONE();
            break;
         case Types::FLOAT:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*NumberSystem::getCurrentNumberSystem().FLOAT_ZERO >= *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            result = FLOAT_ONE();
            break;
         case Types::STRING:
            if ("" >= static_cast<Types::StringValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::LEQ (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& lhs, const std::shared_ptr<Types::ValueType>& rhs)
    {
      std::shared_ptr<Types::ValueType> LHS = Constant(context, lhs);
      std::shared_ptr<Types::ValueType> RHS = Constant(context, rhs);
      std::shared_ptr<Types::ValueType> result;
      switch (LHS->getType())
       {
      case Types::FLOAT:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value <= *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (*static_cast<Types::FloatValue*>(LHS.get())->value <= *NumberSystem::getCurrentNumberSystem().FLOAT_ZERO)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::STRING:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::STRING:
         switch (RHS->getType())
          {
         case Types::STRING:
            if (static_cast<Types::StringValue*>(LHS.get())->value <= static_cast<Types::StringValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
            if (static_cast<Types::StringValue*>(LHS.get())->value <= "")
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::FLOAT:
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::NIL:
         switch (RHS->getType())
          {
         case Types::FLOAT:
            if (*NumberSystem::getCurrentNumberSystem().FLOAT_ZERO <= *static_cast<Types::FloatValue*>(RHS.get())->value)
               result = FLOAT_ONE();
            else
               result = FLOAT_ZERO();
            break;
         case Types::NIL:
         case Types::STRING:
            result = FLOAT_ONE();
            break;
         case Types::CELL_REF:
         case Types::CELL_RANGE:
            constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
          }
         break;
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error comparing " + LHS->getTypeName() + " with " + RHS->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Negate (const Input::Token& tok, CallingContext& context, const std::shared_ptr<Types::ValueType>& arg)
    {
      std::shared_ptr<Types::ValueType> ARG = Constant(context, arg);
      std::shared_ptr<Types::ValueType> result;
      switch (ARG->getType())
       {
      case Types::FLOAT:
         result = std::make_shared<Types::FloatValue>(-*static_cast<Types::FloatValue*>(ARG.get())->value);
         break;
      case Types::NIL:
         result = ARG;
         break;
      case Types::STRING:
      case Types::CELL_REF:
      case Types::CELL_RANGE:
         constructMessage("Error negating " + ARG->getTypeName(), tok);
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::FunctionCall (const Input::Token& token, CallingContext& context, const std::vector<std::shared_ptr<Types::ValueType> >& args)
    {
      const auto iter = context.map->find(token.text);
      if (context.map->end() == iter)
       {
         std::stringstream str;
         str << "Name >" << token.text << "< is not a function at " << token.location;
         throw Backwards::Types::TypedOperationException(str.str());
       }
      std::shared_ptr<Backwards::Engine::Expression> location = std::make_shared<Backwards::Engine::Variable>(Backwards::Input::Token(), iter->second);

      std::shared_ptr<Backwards::Types::ArrayValue> newArg = std::make_shared<Backwards::Types::ArrayValue>();
      for (std::shared_ptr<Types::ValueType> expr : args)
       {
         newArg->value.emplace_back(
            std::make_shared<Backwards::Types::CellRefValue>(
               std::make_shared<CellRefEval>(expr)));
       }
      std::vector<std::shared_ptr<Backwards::Engine::Expression> > newArgs;
      newArgs.push_back(std::make_shared<Backwards::Engine::Constant>(Backwards::Input::Token(), newArg));

      Backwards::Engine::FunctionCall call (Backwards::Input::Token(), location, newArgs);

      std::shared_ptr<Backwards::Types::ValueType> returned = call.evaluate(context);

      if (typeid(Backwards::Types::CellRefValue) == typeid(*returned.get()))
       {
         std::shared_ptr<CellRefEval> temp = std::dynamic_pointer_cast<CellRefEval>(static_cast<Backwards::Types::CellRefValue*>(returned.get())->value);
         if (nullptr == temp.get())
          {
            throw Backwards::Engine::ProgrammingException("CellRefHolder was not a Forward CellRefEval.");
          }
         returned = temp->evaluate(context);
       }

      std::shared_ptr<Types::ValueType> result;
      if (typeid(Backwards::Types::FloatValue) == typeid(*returned.get()))
       {
         result = std::make_shared<Types::FloatValue>(static_cast<Backwards::Types::FloatValue*>(returned.get())->value);
       }
      else if (typeid(Backwards::Types::StringValue) == typeid(*returned.get()))
       {
         result = std::make_shared<Types::StringValue>(static_cast<Backwards::Types::StringValue*>(returned.get())->value);
       }
      else if (typeid(Backwards::Types::NilValue) == typeid(*returned.get()))
       {
         result = std::make_shared<Types::NilValue>();
       }
      else if (typeid(Backwards::Types::CellRangeValue) == typeid(*returned.get()))
       {
         std::shared_ptr<CellRangeExpand> temp = std::dynamic_pointer_cast<CellRangeExpand>(static_cast<Backwards::Types::CellRangeValue*>(returned.get())->value);
         if (nullptr == temp.get())
          {
            throw Backwards::Engine::ProgrammingException("CellRangeHolder was not a Forward CellRangeExpand.");
          }
         result = temp->value;
       }
      else
       {
         throw Backwards::Engine::ProgrammingException("Call to function " + token.text + " returned invalid type");
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::Name (const Input::Token& token, CallingContext& context)
    {
      const auto iter = context.names->find(token.text);
      if (context.names->end() == iter)
       {
         return std::make_shared<Types::NilValue>();
       }
      else
       {
         return iter->second;
       }
    }

 } // namespace Forwards

 } // namespace Backwards

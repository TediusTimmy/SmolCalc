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
#include "Forwards/Engine/ShuntingYard.h"

#include "Forwards/Types/FloatValue.h"
#include "Forwards/Types/StringValue.h"
#include "Forwards/Types/CellRefValue.h"

#include "Forwards/Engine/CallingContext.h"

#include "Forwards/Input/Lexer.h"
#include "Forwards/Input/Token.h"

#include "NumberSystem.h"

#include <vector>
#include <map>

namespace Forwards
 {

namespace Engine
 {

   enum LIKE
    {
      NUMBER_LIKE,
      OPERATOR_LIKE
    };

   enum OPERATION
    {
      NO_OP,
      EQUALITY,
      INEQUALITY,
      GREATER_THAN,
      LESS_THAN,
      GREATER_THAN_OR_EQUAL_TO,
      LESS_THAN_OR_EQUAL_TO,
      PLUS,
      MINUS,
      CAT,
      MULTIPLY,
      DIVIDE,
      NEGATE, // Synthetic
      RANGE,
      IDENTIFIER, // Function call
      SEMICOLON, // Separates function call arguments
      OPEN_PARENS
    };

   static const size_t MAX_STACK_SIZE = 512U;

   static void pushOp (OPERATION op, const Input::Token& buildToken, std::vector<OPERATION>& operationStack, std::vector<Input::Token>& tokenStack)
    {
      if (MAX_STACK_SIZE == operationStack.size())
       {
         ShuntingYard::constructMessage("Operation stack overflow", buildToken);
       }
      operationStack.push_back(op);
      tokenStack.push_back(buildToken);
    }

   static void popOp (std::vector<OPERATION>& operationStack, std::vector<Input::Token>& tokenStack)
    {
      operationStack.resize(operationStack.size() - 1U);
      tokenStack.resize(tokenStack.size() - 1U);
    }

   static void pushData (const Input::Token& buildToken, std::vector<std::shared_ptr<Types::ValueType> >& dataStack, const std::shared_ptr<Types::ValueType>&& value)
    {
      if (MAX_STACK_SIZE == dataStack.size())
       {
         ShuntingYard::constructMessage("Data stack overflow", buildToken);
       }
      dataStack.resize(dataStack.size() + 1U);
      dataStack[dataStack.size() - 1U] = value;
    }

   static void popData (const Input::Token& buildToken, std::vector<std::shared_ptr<Types::ValueType> >& dataStack, size_t size)
    {
      if (dataStack.size() < size)
       {
         ShuntingYard::constructMessage("Data stack underflow", buildToken);
       }
    }

   static void doAnOp (CallingContext& context, std::vector<std::shared_ptr<Types::ValueType> >& dataStack, std::vector<OPERATION>& operationStack, std::vector<Input::Token>& tokenStack)
    {

#define caseImpl(x,y) \
   case x: \
      popData(tokenStack[tokenStack.size() - 1U], dataStack, 2U); \
      dataStack[dataStack.size() - 2U] = y(tokenStack[tokenStack.size() - 1U], context, dataStack[dataStack.size() - 2U], dataStack[dataStack.size() - 1U]); \
      dataStack.resize(dataStack.size() - 1U); \
      break

      switch(operationStack[operationStack.size() - 1U])
       {
      caseImpl(EQUALITY, ShuntingYard::Equals);
      caseImpl(INEQUALITY, ShuntingYard::NotEqual);
      caseImpl(GREATER_THAN, ShuntingYard::Greater);
      caseImpl(LESS_THAN, ShuntingYard::Less);
      caseImpl(GREATER_THAN_OR_EQUAL_TO, ShuntingYard::GEQ);
      caseImpl(LESS_THAN_OR_EQUAL_TO, ShuntingYard::LEQ);
      caseImpl(PLUS, ShuntingYard::Plus);
      caseImpl(MINUS, ShuntingYard::Minus);
      caseImpl(CAT, ShuntingYard::Cat);
      caseImpl(MULTIPLY, ShuntingYard::Multiply);
      caseImpl(DIVIDE, ShuntingYard::Divide);
      caseImpl(RANGE, ShuntingYard::MakeRange);
      case NEGATE:
         popData(tokenStack[tokenStack.size() - 1U], dataStack, 1U);
         dataStack[dataStack.size() - 1U] = ShuntingYard::Negate(tokenStack[tokenStack.size() - 1U], context, dataStack[dataStack.size() - 1U]);
         break;
      case NO_OP:
      case IDENTIFIER:
      case SEMICOLON:
      case OPEN_PARENS:
         ShuntingYard::constructMessage("Bad operation in doAnOp.", tokenStack[tokenStack.size() - 1U]);
       }
      popOp(operationStack, tokenStack);
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::cellref (const Input::Token& ref, size_t col, size_t row)
    {
      bool colAbsolute = false, rowAbsolute = false;
      int64_t r_col, r_row;
      const char * iter = ref.text.c_str();
      if ('$' == *iter)
       {
         colAbsolute = true;
         ++iter;
       }
      r_col = *iter - 'A';
      ++iter;
      if (std::isalpha(*iter))
       {
         r_col = (r_col * 26) + (*iter - 'A') + 26;
         ++iter;
       }
      if ('$' == *iter)
       {
         rowAbsolute = true;
         ++iter;
       }
      r_row = std::atoll(iter);
      if (colAbsolute && rowAbsolute)
       {
         return std::make_shared<Types::CellRefValue>(colAbsolute, r_col, rowAbsolute, r_row);
       }
      else if (colAbsolute)
       {
         return std::make_shared<Types::CellRefValue>(colAbsolute, r_col, rowAbsolute, r_row - row);
       }
      else if (rowAbsolute)
       {
         return std::make_shared<Types::CellRefValue>(colAbsolute, r_col - col, rowAbsolute, r_row);
       }
      else
       {
         return std::make_shared<Types::CellRefValue>(colAbsolute, r_col - col, rowAbsolute, r_row - row);
       }
    }

   static int PREC (OPERATION op)
    {
      int result = 0;
#define precMap(x,y) \
      case x: \
         result = y; \
         break
      switch(op)
       {
         precMap(IDENTIFIER,               10);
         precMap(SEMICOLON,                10);
         precMap(OPEN_PARENS,              10);
         precMap(EQUALITY,                 20);
         precMap(INEQUALITY,               20);
         precMap(GREATER_THAN,             20);
         precMap(LESS_THAN,                20);
         precMap(GREATER_THAN_OR_EQUAL_TO, 20);
         precMap(LESS_THAN_OR_EQUAL_TO,    20);
         precMap(PLUS,                     21);
         precMap(MINUS,                    21);
         precMap(CAT,                      21);
         precMap(MULTIPLY,                 22);
         precMap(DIVIDE,                   22);
         precMap(NEGATE,                   23);
         precMap(RANGE,                    24);
         case NO_OP:
            break;
       }
      return result;
    }

   std::shared_ptr<Types::ValueType> ShuntingYard::evaluate (Input::Lexer& src, CallingContext& context)
    {
      std::vector<std::shared_ptr<Types::ValueType> > dataStack;
      std::vector<OPERATION> operationStack;
      std::vector<Input::Token> tokenStack; // should be consistent with operator stack
      LIKE last = OPERATOR_LIKE; // beginning of string is OPERATOR_LIKE
      bool done = false;

#define topStack operationStack[operationStack.size() - 1U]

      dataStack.reserve(MAX_STACK_SIZE);
      operationStack.reserve(MAX_STACK_SIZE);
      tokenStack.reserve(MAX_STACK_SIZE);

#define caseMap(x,y) \
   case x: \
      next_op = y; \
      break

      while (false == done)
       {
         Input::Token buildToken = src.getNextToken();
         OPERATION next_op = NO_OP;
         switch (buildToken.lexeme)
          {
         caseMap(Input::EQUALITY, EQUALITY);
         caseMap(Input::INEQUALITY, INEQUALITY);
         caseMap(Input::GREATER_THAN, GREATER_THAN);
         caseMap(Input::LESS_THAN, LESS_THAN);
         caseMap(Input::GREATER_THAN_OR_EQUAL_TO, GREATER_THAN_OR_EQUAL_TO);
         caseMap(Input::LESS_THAN_OR_EQUAL_TO, LESS_THAN_OR_EQUAL_TO);
         caseMap(Input::PLUS, PLUS);
         caseMap(Input::CAT, CAT);
         caseMap(Input::MULTIPLY, MULTIPLY);
         caseMap(Input::DIVIDE, DIVIDE);
         caseMap(Input::RANGE, RANGE);
         caseMap(Input::SEMICOLON, SEMICOLON);
         case Input::MINUS:
            if (OPERATOR_LIKE == last) // if the last thing was operator-like then this is negation
             {
               next_op = NEGATE;
             }
            else // else it was number-like and this is subtraction
             {
               next_op = MINUS;
             }
            break;
         case Input::OPEN_PARENS:
            if (OPERATOR_LIKE == last)
             {
               pushOp(OPEN_PARENS, buildToken, operationStack, tokenStack);
             }
            else // else we have a number after number
             {
               constructMessage("Expected : end of input", buildToken);
             }
            break;
         case Input::CLOSE_PARENS:
            while ((0U != operationStack.size()) && (topStack != OPEN_PARENS) && (topStack != SEMICOLON) && (topStack != IDENTIFIER))
             {
               doAnOp(context, dataStack, operationStack, tokenStack);
             }
             {
            bool fail = true;
            if (0U != operationStack.size())
             {
               if ((topStack == SEMICOLON) || (topStack == IDENTIFIER))
                {
                  // Call a function.
                  size_t args = 1U;
                  while ((args <= operationStack.size()) && (SEMICOLON == operationStack[operationStack.size() - args]))
                   {
                     ++args;
                   }
                  if ((args <= operationStack.size()) && (IDENTIFIER == operationStack[operationStack.size() - args]))
                   {
                     // Handle @FUN() - no argument
                     size_t d_args = args - (last == OPERATOR_LIKE);
                     popData(buildToken, dataStack, d_args);
                     std::vector<std::shared_ptr<Types::ValueType> > argList;
                     for (size_t arg = d_args; arg > 0; --arg)
                      {
                        argList.push_back(dataStack[dataStack.size() - arg]);
                      }
                     std::shared_ptr<Types::ValueType> temp = FunctionCall(tokenStack[tokenStack.size() - args], context, argList);
                     dataStack.resize(dataStack.size() - d_args);
                     // Do the push to do the bounds check.
                     pushData(tokenStack[tokenStack.size() - args], dataStack, std::move(temp));
                     operationStack.resize(operationStack.size() - args);
                     tokenStack.resize(tokenStack.size() - args);
                     fail = false;
                   }
                }
               else if (topStack == OPEN_PARENS)
                {
                  popOp(operationStack, tokenStack);
                  fail = false;
                }
             }
            if (true == fail)
             {
               constructMessage("Unmatched ')'", buildToken);
             }
             }
            last = NUMBER_LIKE;
            break;
         case Input::CELL_REFERENCE:
            if (NUMBER_LIKE == last)
             {
               constructMessage("Expected : end of input", buildToken);
             }
            pushData(buildToken, dataStack, cellref(buildToken, context.topCell()->col, context.topCell()->row));
            last = NUMBER_LIKE;
            break;
         case Input::NUMBER:
            if (NUMBER_LIKE == last)
             {
               constructMessage("Expected : end of input", buildToken);
             }
            pushData(buildToken, dataStack, std::make_shared<Types::FloatValue>(NumberSystem::getCurrentNumberSystem().fromString(buildToken.text)));
            last = NUMBER_LIKE;
            break;
         case Input::STRING:
            if (NUMBER_LIKE == last)
             {
               constructMessage("Expected : end of input", buildToken);
             }
            pushData(buildToken, dataStack, std::make_shared<Types::StringValue>(buildToken.text));
            last = NUMBER_LIKE;
            break;
         case Input::NAME:
            if (NUMBER_LIKE == last)
             {
               constructMessage("Expected : end of input", buildToken);
             }
            pushData(buildToken, dataStack, Name(buildToken, context));
            last = NUMBER_LIKE;
            break;
         case Input::END_OF_FILE:
            done = true;
            break;
         case Input::IDENTIFIER:
            if (OPERATOR_LIKE == last)
             {
               Input::Token nextToken = src.peekNextToken();
               if (Input::OPEN_PARENS != nextToken.lexeme)
                {
                  std::vector<std::shared_ptr<Types::ValueType> > noArgs;
                  pushData(buildToken, dataStack, FunctionCall(buildToken, context, noArgs));
                  last = NUMBER_LIKE;
                }
               else
                {
                  pushOp(IDENTIFIER, buildToken, operationStack, tokenStack);
                  src.getNextToken();
                }
             }
            else // else we have a number after number
             {
               constructMessage("Expected : end of input", buildToken);
             }
            break;
         default:
            constructMessage("Unexpected", buildToken);
          }

         if (NO_OP != next_op)
          {
            last = OPERATOR_LIKE;

            while (0U != operationStack.size())
             {
               if ((NEGATE != next_op) && (SEMICOLON != next_op))
                {
                  if (PREC(next_op) > PREC(topStack))
                   {
                     break;
                   }
                }
               else
                {
                  if (PREC(next_op) >= PREC(topStack))
                   {
                     break;
                   }
                }

               doAnOp(context, dataStack, operationStack, tokenStack);
             }
            pushOp(next_op, buildToken, operationStack, tokenStack);
          }
       }

      while (0U != operationStack.size())
       {
         doAnOp(context, dataStack, operationStack, tokenStack);
       }

      std::shared_ptr<Types::ValueType> result;
      if (1U == dataStack.size())
       {
         result = Constant(context, dataStack[0]);
       }

      return result;
    }

 } // namespace Engine

 } // namespace Forwards

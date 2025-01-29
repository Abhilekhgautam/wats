#include "parser.hpp"
#include "tokens.hpp"
#include "utils.hpp"

#include <cmath>
#include <iostream>
#include <optional>

#define GENERATE_POSITION token_vec[current_parser_position].GetLine(), \
	                  token_vec[current_parser_position].GetColumn() + 1

void Parser::Parse(){
  auto stmts = ParseStatements();

  if (stmts.has_value()){
    auto stmt_vec = stmts.value();
    std::cout << "Total " << stmt_vec.size() << " statements parsed\n";
    for(auto elt: stmt_vec){
          elt->Debug();
    }
  }
}

void Parser::DidYouMean(const std::string to_add, std::size_t line,
		       std::size_t column){

  const std::string invalid_line = source_code_by_line[line - 1];
  const std::string contents_after_error = invalid_line.substr(column - 1);
  const std::string contents_before_error = invalid_line.substr(0, column - 1);

  const std::string expected_correct_line = contents_before_error + " " +
	         to_add + " " + contents_after_error;

  Color("blue", "Did You Mean?", true);

  std::cout << expected_correct_line << '\n';
  if (to_add != "\n")
      Color("green", SetPlus(column + 1, to_add.length()), true);

}

void Parser::Expected(const std::string str,
		      std::size_t line, std::size_t column){

  std::cout << "[ " << line << ":" << column <<  " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << source_code_by_line[line - 1] << '\n';
  Color("green", SetArrow(column), true);
}

bool Parser::Peek(TokenName tok){
   try{
       if (token_vec.at(current_parser_position + 1).GetTokenName() == tok)
           return true;

       else return false;
    }
   catch(std::out_of_range& e){
    // there is no next token to peek at
    return false;
   }
}

Token Parser::GetCurrentToken(){
    return token_vec[current_parser_position];
}

void Parser::StoreParserPosition(){
  temp_parser_position = current_parser_position;
}

void Parser::ConsumeNext(){
  current_parser_position = current_parser_position + 1;
}

void Parser::BackTrack(){
  current_parser_position = temp_parser_position;
}

bool Parser::IsAtEnd(){
  return current_parser_position >= token_vec.size();
}

// Function Parsing Utilities begins here
std::optional<FunctionDefinitionAST*> Parser::ParseFunctionWithRetType(){
  StoreParserPosition();
  // later when we add fn args..
  [[maybe_unused]]int parenthesis_position;

  if (Peek(TokenName::FUNCTION)) ConsumeNext();
  else return {};

  std::string fn_name;

  if (Peek(TokenName::ID)){
      ConsumeNext();
      fn_name = GetCurrentToken().GetValue();
  } else {
      Expected("Expected an Identifier (Function Name) after the 'function' keyword",GENERATE_POSITION);
      return {};
  }

  if (Peek(TokenName::OPEN_PARENTHESIS)) ConsumeNext();
  else {
    Expected("Expected a '('", GENERATE_POSITION);
	DidYouMean("(", GENERATE_POSITION);
	return {};
  }

  if (Peek(TokenName::CLOSE_PARENTHESIS)) ConsumeNext();
  else{
    Expected("Expected a ')'", GENERATE_POSITION);
	DidYouMean(")", GENERATE_POSITION);
	return {};
  }

  // fixme: add other types later.
  if (Peek(TokenName::I32)){
      // todo: store the type
  }

  auto result = ParseCurlyBraceAndBody();
  if(result.has_value()){
      std::vector<std::pair<Type*, std::string>> dummy;
	  return new FunctionDefinitionAST(fn_name, dummy, result.value(), nullptr);
	} else return {};
}

std::optional<FunctionDefinitionAST*> Parser::ParseFunctionWithoutRetType(){
    StoreParserPosition();
    // later when we add fn args..
    [[maybe_unused]]int parenthesis_position;

    if (Peek(TokenName::FUNCTION)) ConsumeNext();
    else return {};

    std::string fn_name;

    if (Peek(TokenName::ID)){
        ConsumeNext();
        fn_name = GetCurrentToken().GetValue();
    } else {
        Expected("Expected an Identifier (Function Name) after the 'function' keyword",GENERATE_POSITION);
        return {};
    }

    if (Peek(TokenName::OPEN_PARENTHESIS)) ConsumeNext();
    else {
      Expected("Expected a '('", GENERATE_POSITION);
	DidYouMean("(", GENERATE_POSITION);
	return {};
    }

    if (Peek(TokenName::CLOSE_PARENTHESIS)) ConsumeNext();
    else{
      Expected("Expected a ')'", GENERATE_POSITION);
	DidYouMean(")", GENERATE_POSITION);
	return {};
    }

    auto result = ParseCurlyBraceAndBody();
    if(result.has_value()){
        std::vector<std::pair<Type*, std::string>> dummy;
	  return new FunctionDefinitionAST(fn_name, dummy, result.value(), nullptr);
	} else return {};
}

std::optional<FunctionDefinitionAST*> Parser::ParseFunction(){
    StoreParserPosition();

    auto resultRetType = ParseFunctionWithRetType();
    if (resultRetType.has_value()){
       auto val = resultRetType.value();
       std::cout << val->GetFunctionName() << '\n';
       return resultRetType;
	}

	BackTrack();

	auto result = ParseFunctionWithoutRetType();
	if ((result.has_value())){
       auto val = result.value();
       std::cout << val->GetFunctionName() << '\n';
       return result;
	}

	return {};
}
// Function Parsing Ends here

// Parsing Variable Declaration Begins here


std::optional<VariableDeclarationAST*> Parser::ParseVariableDeclWithLet(){
    StoreParserPosition();

    if (Peek(TokenName::LET)){
        ConsumeNext();
    } else return {};

    std::string var_name;
    if (Peek(TokenName::ID)){
        ConsumeNext();
        var_name = GetCurrentToken().GetValue();
    } else {
        Expected("A Variable name after a let expression is required", GENERATE_POSITION);
        return {};
    }

    if (Peek(TokenName::SEMI_COLON)) ConsumeNext();
    else{
       Expected("Consider adding a ';' here.", GENERATE_POSITION);
       DidYouMean(";", GENERATE_POSITION);
       return {};
    }

    return new VariableDeclarationAST(nullptr, var_name);
}

std::optional<VariableDeclarationAST*> Parser::ParseVariableDeclWithType(){
   StoreParserPosition();

   if (Peek(TokenName::LET)){
       ConsumeNext();
   } else return {};

   std::string var_name;
   if (Peek(TokenName::ID)){
       ConsumeNext();
       var_name = GetCurrentToken().GetValue();
   } else {
       Expected("A Variable name after a let expression is required", GENERATE_POSITION);
       return {};
   }

   if (Peek(TokenName::COLON)) ConsumeNext();
   else return {};

   // FixME: i32 for now.
   if(Peek(TokenName::I32)) {
       ConsumeNext();
       // Todo: Store the type.
   } else {
       Expected("Consider mentioning the type of the variable", GENERATE_POSITION);
       return {};
   }

   if (Peek(TokenName::SEMI_COLON)) ConsumeNext();
   else{
      Expected("Consider adding a ';' here.", GENERATE_POSITION);
      DidYouMean(";", GENERATE_POSITION);
      return {};
   }

   return new VariableDeclarationAST(nullptr, var_name);
}

std::optional<VariableDeclarationAST*> Parser::ParseVariableDecl(){
    auto result = ParseVariableDeclWithType();

    if(result.has_value()){
        std::cout << result.value()->GetVarName() << '\n';
        return result;
    }

    BackTrack();
    result = ParseVariableDeclWithLet();

    if(result.has_value()){
        std::cout << result.value()->GetVarName() << '\n';
        return result;
    }

    return {};
}

std::optional<VariableAssignmentAST*> Parser::ParseVariableAssignment(){
   StoreParserPosition();

   std::string var_name;
   if (Peek(TokenName::ID)) {
       ConsumeNext();
       var_name = GetCurrentToken().GetValue();
   }
   else return {};

   if(Peek(TokenName::ASSIGN)) ConsumeNext();
   else return {};

   auto expr = ParseExpression();

   if(!expr.has_value()){
       Expected("Expected an value or expression for the assignment", GENERATE_POSITION);
       return {};
   }

   if (Peek(TokenName::SEMI_COLON)) {
       ConsumeNext();
       return new VariableAssignmentAST(var_name, expr.value());
   }
   else{
      Expected("Consider adding a ';' here.", GENERATE_POSITION);
      DidYouMean(";", GENERATE_POSITION);
      return {};
   }
}

std::optional<VariableDeclareAndAssignAST*> Parser::ParseVariableInitWithLet(){
    StoreParserPosition();

    if(Peek(TokenName::LET)) ConsumeNext();
    else return {};

    std::string var_name;

    if(Peek(TokenName::ID)) {
        ConsumeNext();
        var_name = GetCurrentToken().GetValue();
    }

    if(Peek(TokenName::ASSIGN)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(!expr.has_value()){
      Expected("Expected an value or expression for the assignment", GENERATE_POSITION);
      return {};
    }

    if (Peek(TokenName::SEMI_COLON)){
        ConsumeNext();
    } else {
        Expected("Consider adding a ';' here.", GENERATE_POSITION);
        DidYouMean(";", GENERATE_POSITION);
        return {};
    }

	return new VariableDeclareAndAssignAST(var_name, nullptr, expr.value());
}

std::optional<VariableDeclareAndAssignAST*> Parser::ParseVariableInitWithType(){
    StoreParserPosition();

    if(Peek(TokenName::LET)) ConsumeNext();
    else return {};

    std::string var_name;

    if(Peek(TokenName::ID)) {
        ConsumeNext();
        var_name = GetCurrentToken().GetValue();
    }

    if (Peek(TokenName::COLON)) ConsumeNext();
    else return {};

    // FixME: i32 for now.
    if(Peek(TokenName::I32)) {
        ConsumeNext();
        // Todo: Store the type.
    } else {
        Expected("Consider mentioning the type of the variable", GENERATE_POSITION);
        return {};
    }

    if(Peek(TokenName::ASSIGN)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(!expr.has_value()){
      Expected("Expected an value or expression for the assignment", GENERATE_POSITION);
      return {};
    }

    if (Peek(TokenName::SEMI_COLON)){
        ConsumeNext();
    } else {
        Expected("Consider adding a ';' here.", GENERATE_POSITION);
        DidYouMean(";", GENERATE_POSITION);
        return {};
    }

	return new VariableDeclareAndAssignAST(var_name, nullptr, expr.value());
}

// Parsing Variable Declaration Ends here

std::optional<ExpressionAST*> Parser::ParseExpressionBeginningWithID(){
  StoreParserPosition();

  std::string var_name;
  if (Peek(TokenName::ID)){
      ConsumeNext();
      var_name = GetCurrentToken().GetValue();
  } else return {};

  auto identifier_expr = new IdentifierAST(var_name);
  auto expr = ParseSubExpression();

  if(expr.has_value()) {
      auto value = expr.value();
      return new BinaryExpressionAST(identifier_expr, value.first, value.second);
  }
  else return identifier_expr;
}

std::optional<ExpressionAST*> Parser::ParseExpressionBeginningWithNumber(){
  StoreParserPosition();

  std::string num;
  if (Peek(TokenName::NUMBER)){
      ConsumeNext();
      num = GetCurrentToken().GetValue();
  } else return {};

  auto number_expr = new NumberAST(num);

  auto expr = ParseSubExpression();

  if (expr.has_value()){
      auto val = expr.value();
      return new BinaryExpressionAST(number_expr, val.first, val.second);
  }
  else return new NumberAST(num);
}

std::optional<ExpressionAST*> Parser::ParseExpressionBeginningWithBraces(){
   StoreParserPosition();
   [[maybe_unused]] auto parenthesis_position = current_parser_position;

   if(Peek(TokenName::OPEN_PARENTHESIS)){
       ConsumeNext();
       parenthesis_position = current_parser_position;
   } else return {};

   auto expr = ParseExpression();

   if(expr.has_value()){
       if (Peek(TokenName::CLOSE_PARENTHESIS)){
           ConsumeNext();
           return expr;
       } else {
            Expected("Expected a closing pair for '('", GENERATE_POSITION);
            DidYouMean(")", GENERATE_POSITION);
            return {};
       }
   } else return {};
}

std::optional<std::pair<ExpressionAST*, std::string>> Parser::ParseSubExpression(){
	StoreParserPosition();

	std::optional<ExpressionAST*> expr = ParsePlusExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "+"};
	}

	BackTrack();

	expr = ParseMinusExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "-"};
	}

	BackTrack();

	expr = ParseMulExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "*"};
	}

	BackTrack();

	expr = ParseDivExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "/"};
	}

	BackTrack();

	expr = ParseModExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "%"};
	}

	BackTrack();

	expr = ParseGtExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), ">"};
	}

	BackTrack();

	expr = ParseLtExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "<"};
	}


	BackTrack();

	expr = ParseGteExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), ">="};
	}

	BackTrack();

	expr = ParseLteExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "<="};
	}

	BackTrack();

	expr = ParseEqualsExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "=="};
	}

	BackTrack();

	expr = ParseNotEqualsExpression();

	if(expr.has_value()){
	   return std::pair<ExpressionAST*, std::string>{expr.value(), "!="};
	}

    return {};
}

std::optional<ExpressionAST*> Parser::ParsePlusExpression(){
  StoreParserPosition();

  if (Peek(TokenName::PLUS)) ConsumeNext();
  else return {};

  auto expr = ParseExpression();

  if(expr.has_value()){
      return expr;
  } else return {};
}

std::optional<ExpressionAST*> Parser::ParseMinusExpression(){
  StoreParserPosition();

  if (Peek(TokenName::MINUS)) ConsumeNext();
  else return {};

  auto expr = ParseExpression();

  if(expr.has_value()){
      return expr;
  } else return {};
}

std::optional<ExpressionAST*> Parser::ParseMulExpression(){
  StoreParserPosition();

  if (Peek(TokenName::MUL)) ConsumeNext();
  else return {};

  auto expr = ParseExpression();

  if(expr.has_value()){
      return expr;
  } else return {};
}

std::optional<ExpressionAST*> Parser::ParseDivExpression(){
    StoreParserPosition();

    if (Peek(TokenName::DIV)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(expr.has_value()){
        return expr;
    } else return {};
}

std::optional<ExpressionAST*> Parser::ParseModExpression(){
    StoreParserPosition();

    if (Peek(TokenName::MOD)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(expr.has_value()){
        return expr;
    } else return {};
}

std::optional<ExpressionAST*> Parser::ParseGtExpression(){
    StoreParserPosition();

    if (Peek(TokenName::GT)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(expr.has_value()){
        return expr;
    } else return {};
}

std::optional<ExpressionAST*> Parser::ParseLtExpression(){
    StoreParserPosition();

    if (Peek(TokenName::LT)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(expr.has_value()){
        return expr;
    } else return {};
}

std::optional<ExpressionAST*> Parser::ParseGteExpression(){
    StoreParserPosition();

    if (Peek(TokenName::GTE)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(expr.has_value()){
        return expr;
    } else return {};
}

std::optional<ExpressionAST*> Parser::ParseLteExpression(){
  StoreParserPosition();

  if(Peek(TokenName::LTE)) ConsumeNext();
  else return {};

  auto expr = ParseExpression();

  if(expr.has_value()){
      return expr;
  }
  else return {};
}

std::optional<ExpressionAST*> Parser::ParseEqualsExpression(){
    StoreParserPosition();

    if(Peek(TokenName::EQ)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(expr.has_value()){
        return expr;
    }
    else return {};
}

std::optional<ExpressionAST*> Parser::ParseNotEqualsExpression(){
    StoreParserPosition();

    if(Peek(TokenName::NEQ)) ConsumeNext();
    else return {};

    auto expr = ParseExpression();

    if(expr.has_value()){
        return expr;
    }
    else return {};
}

std::optional<ExpressionAST*> Parser::ParseExpression(){
  StoreParserPosition();

  std::optional<ExpressionAST*> expr = ParseExpressionBeginningWithID();

  if(expr.has_value()) return expr;

  BackTrack();

  expr = ParseExpressionBeginningWithNumber();

  if (expr.has_value()) return expr;

  BackTrack();

  expr = ParseExpressionBeginningWithBraces();

  if(expr.has_value()) return expr;

  return {};
}

std::optional<RangeAST*> Parser::ParseRange(){
  StoreParserPosition();
  auto start = ParseExpression();

  if(!start.has_value()){
    Expected("Expected an expression here", GENERATE_POSITION);
    return {};
  }

  if (Peek(TokenName::TO)){
      ConsumeNext();
  } else {
    Expected("Expected the 'to' keyword.", GENERATE_POSITION);
    DidYouMean("to", GENERATE_POSITION);
  }

  auto end = ParseExpression();

  if(!end.has_value()){
    Expected("Expected an expression here", GENERATE_POSITION);
  }

  // todo: return range
  return new RangeAST(start.value(), end.value());
}

std::optional<std::vector<StatementAST*>> Parser::ParseCurlyBraceAndBody(){
    int curly_brace_position;

    if (Peek(TokenName::OPEN_CURLY)){
        curly_brace_position = current_parser_position + 1;
        ConsumeNext();
    }
    else {
        Expected("Expected a '{' here", GENERATE_POSITION);
    }

    auto body = ParseStatements();

    if (!body.has_value()) return {};

    if (Peek(TokenName::CLOSE_CURLY)){
        ConsumeNext();
        return body;
    } else {
        Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(), token_vec[curly_brace_position].GetColumn());
        return {};
    }
}

std::optional<LoopAST*> Parser::ParseLoop(){
  StoreParserPosition();

  if(Peek(TokenName::LOOP)){
		ConsumeNext();
	  	auto body = ParseCurlyBraceAndBody();

		if (body.has_value()){
		   return new LoopAST(body.value());
		}

	 }
	 return {};
}

std::optional<ForLoopAST*> Parser::ParseForLoop(){
  StoreParserPosition();

  if (Peek(TokenName::FOR)) ConsumeNext();
  else return {};

  std::string iteration_variable;

  if (Peek(TokenName::ID)){
      ConsumeNext();
      iteration_variable = GetCurrentToken().GetValue();
  } else {
    Expected("Expected an iteration variable name after the 'for' keyword", GENERATE_POSITION);
    return {};
  }

  if (Peek(TokenName::IN)) ConsumeNext();
  else{
    Expected("You missed the 'in' keyword in the for loop", GENERATE_POSITION);
    DidYouMean("in", GENERATE_POSITION);
  }

  auto range = ParseRange();

  if(!range.has_value()) return {};

  auto body = ParseCurlyBraceAndBody();

  if (!body.has_value()) return {};

  return new ForLoopAST(iteration_variable, range.value(), body.value());
}

std::optional<WhileLoopAST*> Parser::ParseWhileLoop(){
   StoreParserPosition();

   if (Peek(TokenName::WHILE)) ConsumeNext();
   else return {};

   auto loop_condition = ParseExpression();
   if(!loop_condition.has_value()){
      Expected("Expected an expression after the 'while' keyword", GENERATE_POSITION);
   }

   auto body = ParseCurlyBraceAndBody();

   if(body.has_value()){
		return new WhileLoopAST(loop_condition.value(), body.value());
	} else return {};
}

std::optional<std::vector<MatchArmAST*>> Parser::ParseMatchArms(){
  StoreParserPosition();

  std::vector<MatchArmAST*> arm_vec;

  auto arm = ParseMatchArm();

  if(!arm.has_value()) return {};

  arm_vec.emplace_back(arm.value());

  auto other_arms = ParseMatchArms();

  if(other_arms.has_value()){
     auto other_arm = other_arms.value();
     for (auto arm : other_arm){
         arm_vec.push_back(arm);
     }
     return arm_vec;
  } else {
      return arm_vec;
  }
}

std::optional<MatchArmAST*> Parser::ParseMatchArm(){
  StoreParserPosition();

  auto cond = ParseExpression();

  if(!cond.has_value()) return {};

  if (Peek(TokenName::ARROW)){
      ConsumeNext();
  } else {
    Expected("Expected an '=>' after the expression in 'match' body", GENERATE_POSITION);
    DidYouMean("=>", GENERATE_POSITION);
    return {};
  }

  auto arm_body = ParseCurlyBraceAndBody();

  if (!arm_body.has_value()) return {};
  else return new MatchArmAST(cond.value(), arm_body.value());
}

std::optional<StatementAST*> Parser::ParseMatchStatement(){
   StoreParserPosition();
   int curly_brace_position;

   if (Peek(TokenName::MATCH)) ConsumeNext();
   else return {};

   auto expr = ParseExpression();
   if (!expr.has_value()){
       Expected("Expected an expression after the 'match' keyword", GENERATE_POSITION);
       return {};
   }

   if(Peek(TokenName::OPEN_CURLY)){
       curly_brace_position = current_parser_position + 1;
       ConsumeNext();
   } else {
       Expected("Expected an '{' after the expression in match", GENERATE_POSITION);
       return {};
   }

   auto arms = ParseMatchArms();

   if(!arms.has_value()){
      return {};
   }

   if (Peek(TokenName::CLOSE_CURLY)){
       ConsumeNext();
       return new MatchStatementAST(expr.value(), arms.value());
   } else {
     Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(), token_vec[curly_brace_position].GetColumn());
     return {};
   }
}

std::optional<StatementAST*> Parser::ParseIfStatement(){
  StoreParserPosition();

  if (Peek(TokenName::IF)) ConsumeNext();
  else return {};

  auto condition = ParseExpression();

  if(!condition.has_value()){
    Expected("Expected an condition after the 'if' keyword", GENERATE_POSITION);
    DidYouMean("<some_expression>", GENERATE_POSITION);
    return {};
  }

  auto body = ParseCurlyBraceAndBody();

  if(body.has_value()){
      std::cout << "An If Statement was Parsed\n";
      return new IfStatementAST(condition.value(), body.value());
  } else return {};
}

std::optional<StatementAST*> Parser::ParseElseStatement(){
  StoreParserPosition();

  if (Peek(TokenName::ELSE)){
      ConsumeNext();
	  auto body = ParseCurlyBraceAndBody();

	  if (body.has_value()){
	    return new ElseStatementAST(body.value());
	  } else return {};

	} else return {};
}

std::optional<StatementAST*> Parser::ParseElseIfStatement(){
   StoreParserPosition();

   if (Peek(TokenName::ELSE)) ConsumeNext();
   else return {};

   if (Peek(TokenName::IF)) ConsumeNext();
   else return {};

   auto condition = ParseExpression();

   if(!condition.has_value()){
      Expected("Expected an condition after 'else-if'", GENERATE_POSITION);
      DidYouMean("<some_expression>", GENERATE_POSITION);
      return {};
   }

   auto body = ParseCurlyBraceAndBody();

   if(body.has_value()){
       // do sth
       //
       return new ElseIfStatementAST(condition.value(), body.value());
   } else return {};
}

std::optional<StatementAST*> Parser::ParseStatement(){
  StoreParserPosition();

  std::optional<StatementAST*> result = ParseForLoop();
  if(result.has_value()){
      // can do sth here later
      return result;
  }

  BackTrack();

  result = ParseWhileLoop();
  if(result.has_value()){
    return result;
  }

  BackTrack();

  result = ParseLoop();
  if(result.has_value()){
    return result;
  }

  BackTrack();

  result = ParseVariableInitWithType();
  if(result.has_value()){
    return result;
  }

  BackTrack();

  result = ParseVariableInitWithLet();
  if(result.has_value()){
    return result;
  }

  BackTrack();

  result = ParseVariableDecl();
  if(result.has_value()){
    return result;
  }

  BackTrack();

  result = ParseVariableAssignment();
  if(result.has_value()){
      return result;
  }

  BackTrack();

  result = ParseMatchStatement();
  if(result.has_value()){
     return result;
  }

  BackTrack();

  result = ParseIfStatement();
  if(result.has_value()){
      return result;
  }

  BackTrack();

   result = ParseElseIfStatement();
  if(result.has_value()){
    return result;
  }

  BackTrack();

  result = ParseElseStatement();
  if(result.has_value()){
     return result;
  }

  BackTrack();

  result = ParseFunction();
  if(result.has_value()){
     return result;
  }

  return {};
}

std::optional<std::vector<StatementAST*>> Parser::ParseStatements(){
   StoreParserPosition();

   std::vector<StatementAST*> stmts;
   auto result = ParseStatement();

   if(result.has_value()){
       stmts.push_back(result.value());

       auto new_result = ParseStatements();

       if(new_result.has_value()){
           auto stmt = new_result.value();
           for (auto elt : stmt){
               stmts.push_back(elt);
           }
           return stmts;
       } else return stmts;
   }
   else return stmts;
}

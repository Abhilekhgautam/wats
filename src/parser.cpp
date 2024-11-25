#include "parser.hpp"
#include "utils.hpp"

#include <iostream>

void Parser::Parse(){
  ParseStatements();
}

bool Parser::DidYouMean(const std::string to_add, std::size_t line, std::size_t column){
  const std::string invalid_line = source_code_by_line[line - 1];
  const std::string contents_after_error = invalid_line.substr(column - 1);
  const std::string contents_before_error = invalid_line.substr(0, column - 1);
  
  const std::string expected_correct_line = contents_before_error + " " + to_add + " " + contents_after_error;  

  Color("blue", "Did You Mean?", true);
  std::cout << expected_correct_line << '\n';
  Color("green", SetPlus(column + 1, to_add.length()), true);

  return true;
}

bool Parser::Expected(const std::string str, std::size_t line, std::size_t column){
  std::cout << "[ " << line << ":" << column <<  " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << source_code_by_line[line - 1] << '\n';
  Color("green", SetArrow(column), true);

  return true;
}

bool Parser::Peek(TokenType tok){
  try{
    if (token_vec.at(current_parser_position + 1).GetTokenType() == tok) return true;
    else return false;
  } 
   catch(std::out_of_range& e){
    // there is no next token to peek at
    return false;
   }
}

void Parser::StoreParserPosition(){
  temp_parser_position = current_parser_position;
}

bool Parser::ConsumeNext(){
  current_parser_position = current_parser_position + 1;
  return true;
}

bool Parser::BackTrack(){
  current_parser_position = temp_parser_position;
  return true;
}

bool Parser::IsAtEnd(){
  return current_parser_position >= token_vec.size();	
}

// Function Parsing Utilities begins here
bool Parser::ParseFunctionWithRetType(){
  StoreParserPosition();
  // later when we add fn args..
  [[maybe_unused]]int parenthesis_position;
  return(
	 Peek(TokenType::FUNCTION) && ConsumeNext() && 
         (
	  (Peek(TokenType::ID) && ConsumeNext()) || 
	  Expected("Expected an Identifier (Function Name) after the 'function' keyword", 
	            token_vec[current_parser_position].GetLine(),
		    token_vec[current_parser_position].GetColumn() + 1)
	 ) &&

	 (
	  (Peek(TokenType::OPEN_PARENTHESIS) && ConsumeNext()) ||
	  Expected("Expected a '(' after the function name",
		  token_vec[current_parser_position].GetLine(),
		  token_vec[current_parser_position].GetColumn() + 1)
	 ) &&

	 Peek(TokenType::CLOSE_PARENTHESIS) && ConsumeNext() &&
	 Peek(TokenType::I32) && ConsumeNext() && 
	 ParseCurlyBraceAndBody()
	);
}

bool Parser::ParseFunctionWithoutRetType(){
  StoreParserPosition();
  // later when we add fn args..
  [[maybe_unused]]int parenthesis_position;
  return(
	 Peek(TokenType::FUNCTION) && ConsumeNext() && 
         (
	  (Peek(TokenType::ID) && ConsumeNext()) || 
	  (Expected("You forgot to name your function.", 
	            token_vec[current_parser_position].GetLine(),
		    token_vec[current_parser_position].GetColumn() + 1) && 
	   DidYouMean("your_function_name", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1)
	  )
	 ) &&

	 (
	  (Peek(TokenType::OPEN_PARENTHESIS) && ConsumeNext()) ||
	  Expected("Expected a '(' after the function name",
		  token_vec[current_parser_position].GetLine(),
		  token_vec[current_parser_position].GetColumn() + 1)
	 ) &&

	 Peek(TokenType::CLOSE_PARENTHESIS) && ConsumeNext() &&
	 ParseCurlyBraceAndBody()
	);

}

bool Parser::ParseFunction(){
  StoreParserPosition();

  return (
	   ParseFunctionWithRetType() || 
	  (BackTrack() && ParseFunctionWithoutRetType())
	 );
}
// Function Parsing Ends here

// Parsing Variable Declaration Begins here

bool Parser::ParseVariableDeclWithLet(){
  StoreParserPosition();

  return (
           Peek(TokenType::LET) && ConsumeNext() &&
	   ((Peek(TokenType::ID) && ConsumeNext()) || (Expected("Expected a variable name", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1) && DidYouMean("variable_name", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1)))

	 );
}

bool Parser::ParseVariableAssignment(){
   StoreParserPosition();

   return(
	   Peek(TokenType::ID) && ConsumeNext() &&
	   Peek(TokenType::ASSIGN) && ConsumeNext() && 
	   (ParseExpression() || Expected("Expected an value or expression for the assignment", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1))
	 );
}

bool Parser::ParseVariableInitWithLet(){
    StoreParserPosition();

    return(
           Peek(TokenType::LET) && ConsumeNext()    &&
	   Peek(TokenType::ID) && ConsumeNext()     &&
	   Peek(TokenType::ASSIGN) && ConsumeNext() &&
	   (ParseExpression() || Expected("Expected an value or expression for the assignment", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1))
	
	  );
}

bool Parser::ParseVariableDeclWithType(){
   StoreParserPosition();

   return (
	    Peek(TokenType::I32) && ConsumeNext() &&
	    Peek(TokenType::ID) && ConsumeNext() 

	   );
}

bool Parser::ParseVariableDecl(){
   return (
	    ParseVariableDeclWithLet() || 
	    (BackTrack() && ParseVariableDeclWithType())
	  );
}

// Parsing Variable Declaration Ends here

bool Parser::ParseExpressionBeginningWithID(){
  StoreParserPosition();

  return (
	   Peek(TokenType::ID) && ConsumeNext() &&
	   ParseSubExpression()
	 
	 );
}

bool Parser::ParseExpressionBeginningWithNumber(){
  StoreParserPosition();

  return (
	   Peek(TokenType::NUMBER) && ConsumeNext() &&
	   ParseSubExpression()
	 
	 );
}

bool Parser::ParseExpressionBeginningWithBraces(){
   StoreParserPosition();
   auto parenthesis_position = current_parser_position;
   return(
	  Peek(TokenType::OPEN_PARENTHESIS) && ConsumeNext() && [&]()mutable{parenthesis_position = current_parser_position; return true;}() && 
	  ParseExpression() &&
	  ((Peek(TokenType::CLOSE_PARENTHESIS) && ConsumeNext()) || Expected("Expected a closing pair for '('", token_vec[parenthesis_position].GetLine(), token_vec[parenthesis_position].GetColumn()))
	 );
}

bool Parser::ParseSubExpression(){
  StoreParserPosition();
 
  return(
           ParsePlusExpression()                   ||
	   (BackTrack() && ParseMinusExpression()) ||
	   (BackTrack() && ParseMulExpression())   ||
           (BackTrack() && ParseDivExpression())   ||
	   (BackTrack() && ParseModExpression())   ||
	   (BackTrack() && ParseGtExpression())    ||
	   (BackTrack() && ParseLtExpression())    ||
	   (BackTrack() && ParseGteExpression())   ||
	   (BackTrack() && ParseLteExpression())   ||
           (BackTrack() && ParseEmptyExpression())
	);

}

bool Parser::ParsePlusExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::PLUS) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseMinusExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::MINUS) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseMulExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::MUL) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseDivExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::DIV) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseModExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::MOD) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseGtExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::GT) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseLtExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::LT) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseGteExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::GTE) && ConsumeNext() && 
         ParseExpression()
         );
}
bool Parser::ParseLteExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenType::LTE) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseEmptyExpression(){
  return true;
}

bool Parser::ParseExpression(){
  StoreParserPosition();
  
  return (
	   ParseExpressionBeginningWithID() ||
	   (BackTrack() && ParseExpressionBeginningWithNumber()) || 
	   (BackTrack() && ParseExpressionBeginningWithBraces())
	  );
}

void Parser::ParseExpr(){
  if (ParseExpression()) {
    std::cout << "An expression was parsed\n";
  }
}

bool Parser::ParseRange(){
  StoreParserPosition();
  return(
	  ParseExpression() && 
	  ((Peek(TokenType::TO) && ConsumeNext()) || (Expected("Expected the 'to' keyword.", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1) && DidYouMean("to", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1))) &&
          ParseExpression()
	
	);
}
bool Parser::ParseCurlyBraceAndBody(){
    int curly_brace_position;
    return( 
	    (
             (Peek(TokenType::OPEN_CURLY) && ([&]() mutable {curly_brace_position = current_parser_position + 1;  return true;}()) &&  ConsumeNext()) ||
             Expected("Expected a '{' here", token_vec[current_parser_position + 1].GetLine(), (token_vec[current_parser_position + 1].GetColumn()))
            )	   
            &&
           ParseStatements() &&
           ((Peek(TokenType::CLOSE_CURLY) && ConsumeNext()) ||  Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(), token_vec[curly_brace_position].GetColumn()))
          );
}

bool Parser::ParseLoop(){
  StoreParserPosition();

  return(
         Peek(TokenType::LOOP) && ConsumeNext() &&
	 ParseCurlyBraceAndBody() 
	 );
}


bool Parser::ParseForLoop(){
  StoreParserPosition();
  return(
	  (Peek(TokenType::FOR) && ConsumeNext()) && 
	  ((Peek(TokenType::ID) && ConsumeNext()) || (Expected("Expected an iteration variable name after the 'for' keyword", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1))) &&
	  ((Peek(TokenType::IN) && ConsumeNext()) || (Expected("You missed the 'in' keyword in the for loop", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1) && DidYouMean("in", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1))) &&
	  (ParseRange()) && 
           ParseCurlyBraceAndBody()
	);
}

bool Parser::ParseWhileLoop(){
   StoreParserPosition();

   return(
	   Peek(TokenType::WHILE) && ConsumeNext() && 
	   (ParseExpression() || Expected("Expected an expression after the 'while' keyword", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn())) && 
           ParseCurlyBraceAndBody()	
	  );
}

bool Parser::ParseMatchArms(){
  StoreParserPosition();

  return(
           (ParseMatchArm() && ParseMatchArms()) ||
	   
	   (BackTrack() && ParseMatchArm())
	 
	);
}

bool Parser::ParseMatchArm(){
  StoreParserPosition();
  return(
           ParseExpression() &&
	   ((Peek(TokenType::ARROW) && ConsumeNext()) || (Expected("Expected an '=>' after the expression in 'match' body", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn()) && DidYouMean("=>", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn()))) && 

             ParseCurlyBraceAndBody() 
	 
	 );
}

bool Parser::ParseMatchStatement(){
   StoreParserPosition();
   int curly_brace_position;
   return(
           Peek(TokenType::MATCH) && ConsumeNext() &&
	   (ParseExpression() || Expected("Expected an expression after the 'match' keyword", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn())) &&
	   ((Peek(TokenType::OPEN_CURLY) && [&]()mutable{curly_brace_position = current_parser_position + 1; return true;}() && ConsumeNext()) || Expected("Expected an '{' after the expression in match", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn())) && 
	   ParseMatchArms() &&
	   (Peek(TokenType::CLOSE_CURLY) || Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(), token_vec[curly_brace_position].GetColumn()))
	   
	  );
}

bool Parser::ParseIfStatement(){
  StoreParserPosition();

  return(
          Peek(TokenType::IF) && ConsumeNext() &&
	  (ParseExpression() || (Expected("Expected an condition after the 'if' keyword", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn()) && DidYouMean("<some_expression>", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn())))&&
	  ParseCurlyBraceAndBody()
	);
}

bool Parser::ParseElseStatement(){
  StoreParserPosition();

  return(
         Peek(TokenType::ELSE) && ConsumeNext() &&
	 ((ParseExpression() && DidYouMean("if", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn()) && Expected("An Else Statement don't require any condition", token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn()))  || ParseCurlyBraceAndBody())          && 
	 ParseCurlyBraceAndBody()
	
	);
}

bool Parser::ParseElseIfStatement(){
   StoreParserPosition();

   return(
           Peek(TokenType::ELSE) && ConsumeNext() &&
	   Peek(TokenType::IF) && ConsumeNext() &&
	   (ParseExpression() || (Expected("Expected an condition after 'else-if'", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn()) && DidYouMean("<some_expression>", token_vec[current_parser_position + 1].GetLine(), token_vec[current_parser_position + 1].GetColumn()))) &&

	   ParseCurlyBraceAndBody()
         );
}

bool Parser::ParseStatement(){
  StoreParserPosition();

  return(
           ParseForLoop()                              ||
	   (BackTrack() && ParseWhileLoop())           ||
	   (BackTrack() && ParseLoop())                ||
	   (BackTrack() && ParseVariableInitWithLet()) ||
	   (BackTrack() && ParseVariableDecl())        ||
	   (BackTrack() && ParseVariableAssignment())  ||
	   (BackTrack() && ParseMatchStatement())      ||
	   (BackTrack() && ParseIfStatement())         ||
	   (BackTrack() && ParseElseIfStatement())     ||
	   (BackTrack() && ParseElseStatement())       ||
	   (BackTrack() && ParseFunction())
	
	);
}

bool Parser::ParseStatements(){
   StoreParserPosition();

   return(
	   (ParseStatement() && ParseStatements()) ||
           true
	 
	 );
}

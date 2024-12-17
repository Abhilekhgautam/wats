#include "parser.hpp"
#include "utils.hpp"

#include <iostream>

#define GENERATE_POSITION token_vec[current_parser_position].GetLine(), \
	                  token_vec[current_parser_position].GetColumn() + 1

void Parser::Parse(){
  ParseStatements();
}

bool Parser::DidYouMean(const std::string to_add, std::size_t line, 
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

  return true;
}

bool Parser::Expected(const std::string str, 
		      std::size_t line, std::size_t column){

  std::cout << "[ " << line << ":" << column <<  " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << source_code_by_line[line - 1] << '\n';
  Color("green", SetArrow(column), true);

  return true;
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
/*
std::size_t Parser::GetNextTokenLength(){
  auto next_token = token_vec[current_parser_position + 1];

  return next_token.length();

}
*/ 

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
	 Peek(TokenName::FUNCTION) && ConsumeNext() && 
         (
	  (Peek(TokenName::ID) && ConsumeNext()) || 
	  Expected(
	   "Expected an Identifier (Function Name) after the 'function' keyword",
	    GENERATE_POSITION
	  )
	 ) &&

	 (
	  (Peek(TokenName::OPEN_PARENTHESIS) && ConsumeNext()) ||
	  (Expected("You forgot to name your function.", GENERATE_POSITION) && 
	   DidYouMean("your_function_name", GENERATE_POSITION)
	  )
	 ) &&

	 Peek(TokenName::CLOSE_PARENTHESIS) && ConsumeNext() &&
	 Peek(TokenName::I32) && ConsumeNext() && 
	 ParseCurlyBraceAndBody()
	);
}

bool Parser::ParseFunctionWithoutRetType(){
  StoreParserPosition();
  // later when we add fn args..
  [[maybe_unused]]int parenthesis_position;
  return(
	 Peek(TokenName::FUNCTION) && ConsumeNext() && 
         (
	  (Peek(TokenName::ID) && ConsumeNext()) || 
	  (Expected("You forgot to name your function.", 
		    GENERATE_POSITION) && 
	   DidYouMean("your_function_name",
		   GENERATE_POSITION)
	  )
	 ) &&

	 (
	  (Peek(TokenName::OPEN_PARENTHESIS) && ConsumeNext()) ||
	  Expected("Expected a '(' after the function name", GENERATE_POSITION)
	 ) &&

	 (
	  (Peek(TokenName::CLOSE_PARENTHESIS) && ConsumeNext()) || 
	  (Expected("Expected a ')'.", GENERATE_POSITION) && 
	   DidYouMean(")", GENERATE_POSITION))
	  ) &&
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
  bool no_move_ahead = false;
  return (
           Peek(TokenName::LET) && ConsumeNext() &&
	   ((Peek(TokenName::ID) && ConsumeNext()) || (Expected("Expected a variable name", GENERATE_POSITION) && [&]()mutable{no_move_ahead = true; return true;}())) && ConsumeNext() &&
	   ((Peek(TokenName::SEMI_COLON) && ConsumeNext()) || (no_move_ahead) || (Expected("Consider adding a ';' here.", GENERATE_POSITION) && DidYouMean(";", GENERATE_POSITION)))
           
	 );
}

bool Parser::ParseVariableAssignment(){
   StoreParserPosition();

   return(
	   Peek(TokenName::ID) && ConsumeNext() &&
	   Peek(TokenName::ASSIGN) && ConsumeNext() && 
	   (ParseExpression() || Expected("Expected an value or expression for the assignment", GENERATE_POSITION)) &&
           ((Peek(TokenName::SEMI_COLON) && ConsumeNext()) || (Expected("Consider adding a ';' here.", GENERATE_POSITION) && DidYouMean(";", GENERATE_POSITION)))

	 );
}

bool Parser::ParseVariableInitWithLet(){
    StoreParserPosition();
    bool no_move_ahead = false;
    return(
           Peek(TokenName::LET) && ConsumeNext()    &&
	   Peek(TokenName::ID) && ConsumeNext()     &&
	   Peek(TokenName::ASSIGN) && ConsumeNext() &&
	   (ParseExpression() || (Expected("Expected an value or expression for the assignment", GENERATE_POSITION) && [&]()mutable{no_move_ahead = true; return true;}())) &&
	   ((Peek(TokenName::SEMI_COLON) && ConsumeNext()) || no_move_ahead || (Expected("Consider adding a ';' here.", GENERATE_POSITION) && DidYouMean(";", GENERATE_POSITION)))

	  );
}

bool Parser::ParseVariableDeclWithType(){
   StoreParserPosition();

   return (
	    Peek(TokenName::LET) && ConsumeNext() &&
	    Peek(TokenName::ID) && ConsumeNext() &&
	    Peek(TokenName::COLON) && ConsumeNext() &&
	    Peek(TokenName::I32) && ConsumeNext() &&
            ((Peek(TokenName::SEMI_COLON) && ConsumeNext()) || (Expected("Consider adding a ';' here", GENERATE_POSITION) && DidYouMean(";", GENERATE_POSITION)))

	   );
}

bool Parser::ParseVariableDecl(){
   return (
	    ParseVariableDeclWithType() || 
	    (BackTrack() && ParseVariableDeclWithLet())
	  );
}

// Parsing Variable Declaration Ends here

bool Parser::ParseExpressionBeginningWithID(){
  StoreParserPosition();

  return (
	   Peek(TokenName::ID) && ConsumeNext() &&
	   ParseSubExpression()
	 
	 );
}

bool Parser::ParseExpressionBeginningWithNumber(){
  StoreParserPosition();

  return (
	   Peek(TokenName::NUMBER) && ConsumeNext() &&
	   ParseSubExpression()
	 
	 );
}

bool Parser::ParseExpressionBeginningWithBraces(){
   StoreParserPosition();
   auto parenthesis_position = current_parser_position;
   return(
	  Peek(TokenName::OPEN_PARENTHESIS) && ConsumeNext() && [&]()mutable{parenthesis_position = current_parser_position; return true;}() && 
	  ParseExpression() &&
	  ((Peek(TokenName::CLOSE_PARENTHESIS) && ConsumeNext()) || (Expected("Expected a closing pair for '('", GENERATE_POSITION) && DidYouMean(")", GENERATE_POSITION)))
	 ); 
} 

bool Parser::ParseSubExpression(){ 
	StoreParserPosition();
	return( ParsePlusExpression()                   ||
	      (BackTrack() && ParseMinusExpression())   ||
	      (BackTrack() && ParseMulExpression())     ||
	      (BackTrack() && ParseDivExpression())     ||
	      (BackTrack() && ParseModExpression())     ||
	      (BackTrack() && ParseGtExpression())      ||
	      (BackTrack() && ParseLtExpression())      ||
	      (BackTrack() && ParseGteExpression())     ||
	      (BackTrack() && ParseLteExpression())     ||
              (BackTrack() && ParseEmptyExpression())
	);

}

bool Parser::ParsePlusExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::PLUS) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseMinusExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::MINUS) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseMulExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::MUL) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseDivExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::DIV) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseModExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::MOD) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseGtExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::GT) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseLtExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::LT) && ConsumeNext() && 
         ParseExpression()
         );
}

bool Parser::ParseGteExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::GTE) && ConsumeNext() && 
         ParseExpression()
         );
}
bool Parser::ParseLteExpression(){
  StoreParserPosition();

  return(
	 Peek(TokenName::LTE) && ConsumeNext() && 
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
	  (ParseExpression() || Expected("Expected an expression here", GENERATE_POSITION)) && 
	  ((Peek(TokenName::TO) && ConsumeNext()) || (Expected("Expected the 'to' keyword.", GENERATE_POSITION) && DidYouMean("to", GENERATE_POSITION))) &&
          (ParseExpression() || Expected("Expected an expression here", GENERATE_POSITION))
	
	);
}
bool Parser::ParseCurlyBraceAndBody(){
    int curly_brace_position;
    return( 
	    (
             (Peek(TokenName::OPEN_CURLY) && ([&]() mutable {curly_brace_position = current_parser_position + 1;  return true;}()) &&  ConsumeNext()) ||
             Expected("Expected a '{' here", GENERATE_POSITION)
	    )
            &&
           ParseStatements() &&
           ((Peek(TokenName::CLOSE_CURLY) && ConsumeNext()) ||  Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(), token_vec[curly_brace_position].GetColumn()))
          );
}

bool Parser::ParseLoop(){
  StoreParserPosition();

  return(
         Peek(TokenName::LOOP) && ConsumeNext() &&
	 ParseCurlyBraceAndBody() 
	 );
}


bool Parser::ParseForLoop(){
  StoreParserPosition();
  return(
	  (Peek(TokenName::FOR) && ConsumeNext()) && 
	  ((Peek(TokenName::ID) && ConsumeNext()) || (Expected("Expected an iteration variable name after the 'for' keyword", GENERATE_POSITION))) &&
	  ((Peek(TokenName::IN) && ConsumeNext()) || (Expected("You missed the 'in' keyword in the for loop", GENERATE_POSITION) && DidYouMean("in", GENERATE_POSITION))) &&
	  (ParseRange()) && 
           ParseCurlyBraceAndBody()
	);
}

bool Parser::ParseWhileLoop(){
   StoreParserPosition();

   return(
	   Peek(TokenName::WHILE) && ConsumeNext() && 
	   (ParseExpression() || Expected("Expected an expression after the 'while' keyword", GENERATE_POSITION)) && 
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

// Fixme: Has a bug after the call to ParseCurlyBraceAndBody
bool Parser::ParseMatchArm(){
  StoreParserPosition();
  return(
           ParseExpression() &&
	   ((Peek(TokenName::ARROW) && ConsumeNext()) || (Expected("Expected an '=>' after the expression in 'match' body", GENERATE_POSITION) && DidYouMean("=>", GENERATE_POSITION))) && 

             ParseCurlyBraceAndBody() 
	 
	 );
}
// Fixme: Doesn't work correctly
bool Parser::ParseMatchStatement(){
   StoreParserPosition();
   int curly_brace_position;
   return(
           Peek(TokenName::MATCH) && ConsumeNext() &&
	   (ParseExpression() || Expected("Expected an expression after the 'match' keyword", GENERATE_POSITION)) &&
	   ((Peek(TokenName::OPEN_CURLY) && [&]()mutable{curly_brace_position = current_parser_position + 1; return true;}() && ConsumeNext()) || Expected("Expected an '{' after the expression in match", GENERATE_POSITION)) && 
	   ParseMatchArms() &&
	   (Peek(TokenName::CLOSE_CURLY) || Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(), token_vec[curly_brace_position].GetColumn()))
	   
	  );
}

bool Parser::ParseIfStatement(){
  StoreParserPosition();

  return(
          Peek(TokenName::IF) && ConsumeNext() &&
	  (ParseExpression() || (Expected("Expected an condition after the 'if' keyword", GENERATE_POSITION) && DidYouMean("<some_expression>", GENERATE_POSITION)))&&
	  ParseCurlyBraceAndBody()
	);
}

bool Parser::ParseElseStatement(){
  StoreParserPosition();

  return(
         Peek(TokenName::ELSE) && ConsumeNext() &&
	 ((ParseExpression() && DidYouMean("if", GENERATE_POSITION) && Expected("An Else Statement don't require any condition", GENERATE_POSITION))  || ParseCurlyBraceAndBody()) && 
	 ParseCurlyBraceAndBody()
	
	);
}

bool Parser::ParseElseIfStatement(){
   StoreParserPosition();

   return(
           Peek(TokenName::ELSE) && ConsumeNext() &&
	   Peek(TokenName::IF) && ConsumeNext() &&
	   (ParseExpression() || (Expected("Expected an condition after 'else-if'", GENERATE_POSITION) && DidYouMean("<some_expression>", GENERATE_POSITION))) &&

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

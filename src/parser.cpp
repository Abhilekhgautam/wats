#include "parser.hpp"
#include "utils.hpp"

#include <iostream>
#include <bitset>

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

bool Parser::Peek(TokenType tok){
   try{
       if (token_vec.at(current_parser_position + 1).GetTokenType() == tok) 
           return true;

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
	  Expected(
	   "Expected an Identifier (Function Name) after the 'function' keyword",
	    GENERATE_POSITION
	  )
	 ) &&

	 (
	  (Peek(TokenType::OPEN_PARENTHESIS) && ConsumeNext()) ||
	  (Expected("You forgot to name your function.", GENERATE_POSITION) && 
	   DidYouMean("your_function_name", GENERATE_POSITION)
	  )) &&

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
		    GENERATE_POSITION) && 
	   DidYouMean("your_function_name",
		   GENERATE_POSITION)
	  )
	 ) &&

	 (
	  (Peek(TokenType::OPEN_PARENTHESIS) && ConsumeNext()) ||
	  Expected("Expected a '(' after the function name", GENERATE_POSITION)
	 ) &&

	 ((Peek(TokenType::CLOSE_PARENTHESIS) && ConsumeNext()) || (Expected("Expected a ')'.", GENERATE_POSITION) && DidYouMean(")", GENERATE_POSITION))) &&
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
           Peek(TokenType::LET) && ConsumeNext() &&
	   ((Peek(TokenType::ID) && ConsumeNext()) || (Expected("Expected a variable name", GENERATE_POSITION) && [&]()mutable{no_move_ahead = true; return true;}())) &&
	   ((Peek(TokenType::SEMI_COLON) && ConsumeNext()) || (no_move_ahead) || (Expected("Consider adding a ';' here.", GENERATE_POSITION) && DidYouMean(";", GENERATE_POSITION)))
           

	 );
}

bool Parser::ParseVariableAssignment(){
   StoreParserPosition();

   return(
	   Peek(TokenType::ID) && ConsumeNext() &&
	   Peek(TokenType::ASSIGN) && ConsumeNext() && 
	   (ParseExpression() || Expected("Expected an value or expression for the assignment", GENERATE_POSITION)) &&
           ((Peek(TokenType::SEMI_COLON) && ConsumeNext()) || (Expected("Consider adding a ';' here.", GENERATE_POSITION) && DidYouMean(";", GENERATE_POSITION)))

	 );
}

bool Parser::ParseVariableInitWithLet(){
    StoreParserPosition();
    bool no_move_ahead = false;
    return(
           Peek(TokenType::LET) && ConsumeNext()    &&
	   Peek(TokenType::ID) && ConsumeNext()     &&
	   Peek(TokenType::ASSIGN) && ConsumeNext() &&
	   (ParseExpression() || (Expected("Expected an value or expression for the assignment", GENERATE_POSITION) && [&]()mutable{no_move_ahead = true; return true;}())) &&
	   ((Peek(TokenType::SEMI_COLON) && ConsumeNext()) || no_move_ahead || (Expected("Consider adding a ';' here.", GENERATE_POSITION) && DidYouMean(";", GENERATE_POSITION)))

	  );
}

bool Parser::ParseVariableDeclWithType(){
   StoreParserPosition();

   return (
	    Peek(TokenType::LET) && ConsumeNext() &&
	    Peek(TokenType::ID) && ConsumeNext() &&
	    Peek(TokenType::COLON) && ConsumeNext() &&
	    Peek(TokenType::I32) && ConsumeNext() &&
            ((Peek(TokenType::SEMI_COLON) && ConsumeNext()) || (Expected("Consider adding a ';' here", GENERATE_POSITION) && DidYouMean(";", GENERATE_POSITION)))

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
	  ((Peek(TokenType::CLOSE_PARENTHESIS) && ConsumeNext()) || (Expected("Expected a closing pair for '('", GENERATE_POSITION) && DidYouMean(")", GENERATE_POSITION)))
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
	  (ParseExpression() || Expected("Expected an expression here", GENERATE_POSITION)) && 
	  ((Peek(TokenType::TO) && ConsumeNext()) || (Expected("Expected the 'to' keyword.", GENERATE_POSITION) && DidYouMean("to", GENERATE_POSITION))) &&
          (ParseExpression() || Expected("Expected an expression here", GENERATE_POSITION))
	
	);
}
bool Parser::ParseCurlyBraceAndBody(){
    int curly_brace_position;
    return( 
	    (
             (Peek(TokenType::OPEN_CURLY) && ([&]() mutable {curly_brace_position = current_parser_position + 1;  return true;}()) &&  ConsumeNext()) ||
             Expected("Expected a '{' here", GENERATE_POSITION)
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
	  ((Peek(TokenType::ID) && ConsumeNext()) || (Expected("Expected an iteration variable name after the 'for' keyword", GENERATE_POSITION))) &&
	  ((Peek(TokenType::IN) && ConsumeNext()) || (Expected("You missed the 'in' keyword in the for loop", GENERATE_POSITION) && DidYouMean("in", GENERATE_POSITION))) &&
	  (ParseRange()) && 
           ParseCurlyBraceAndBody()
	);
}

bool Parser::ParseWhileLoop(){
   StoreParserPosition();

   return(
	   Peek(TokenType::WHILE) && ConsumeNext() && 
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

bool Parser::ParseMatchArm(){
  StoreParserPosition();
  return(
           ParseExpression() &&
	   ((Peek(TokenType::ARROW) && ConsumeNext()) || (Expected("Expected an '=>' after the expression in 'match' body", GENERATE_POSITION) && DidYouMean("=>", GENERATE_POSITION))) && 

             ParseCurlyBraceAndBody() 
	 
	 );
}

bool Parser::ParseMatchStatement(){
   StoreParserPosition();
   int curly_brace_position;
   return(
           Peek(TokenType::MATCH) && ConsumeNext() &&
	   (ParseExpression() || Expected("Expected an expression after the 'match' keyword", GENERATE_POSITION)) &&
	   ((Peek(TokenType::OPEN_CURLY) && [&]()mutable{curly_brace_position = current_parser_position + 1; return true;}() && ConsumeNext()) || Expected("Expected an '{' after the expression in match", GENERATE_POSITION)) && 
	   ParseMatchArms() &&
	   (Peek(TokenType::CLOSE_CURLY) || Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(), token_vec[curly_brace_position].GetColumn()))
	   
	  );
}

bool Parser::ParseIfStatement(){
  StoreParserPosition();

  return(
          Peek(TokenType::IF) && ConsumeNext() &&
	  (ParseExpression() || (Expected("Expected an condition after the 'if' keyword", GENERATE_POSITION) && DidYouMean("<some_expression>", GENERATE_POSITION)))&&
	  ParseCurlyBraceAndBody()
	);
}

bool Parser::ParseElseStatement(){
  StoreParserPosition();

  return(
         Peek(TokenType::ELSE) && ConsumeNext() &&
	 ((ParseExpression() && DidYouMean("if", GENERATE_POSITION) && Expected("An Else Statement don't require any condition", GENERATE_POSITION))  || ParseCurlyBraceAndBody()) && 
	 ParseCurlyBraceAndBody()
	
	);
}

bool Parser::ParseElseIfStatement(){
   StoreParserPosition();

   return(
           Peek(TokenType::ELSE) && ConsumeNext() &&
	   Peek(TokenType::IF) && ConsumeNext() &&
	   (ParseExpression() || (Expected("Expected an condition after 'else-if'", GENERATE_POSITION) && DidYouMean("<some_expression>", GENERATE_POSITION))) &&

	   ParseCurlyBraceAndBody()
         );
}

bool Parser::ParseErrorenousFunction(){
    StoreParserPosition();

    std::bitset<5> fn_tokens{0b11111};
    std::bitset<4> tokens{0b1111};
    
    if (!Peek(TokenType::FUNCTION)) fn_tokens[4] = 0; 
    ConsumeNext();

    if (!Peek(TokenType::ID)) fn_tokens[3] = 0; 
    ConsumeNext();

    if (!Peek(TokenType::OPEN_PARENTHESIS)) fn_tokens[2] = 0; 
    ConsumeNext();

    if (!Peek(TokenType::CLOSE_PARENTHESIS)) fn_tokens[1] = 0;
    ConsumeNext();

    if (!Peek(TokenType::OPEN_CURLY)) fn_tokens[0] = 0;
    ConsumeNext();

    if(fn_tokens == std::bitset<5>{0b01111}){
        Expected("Expected the 'function' keyword", GENERATE_POSITION);
	return true;
    }

    BackTrack();

    if (Peek(TokenType::ID)) tokens[3] = 0; 
    ConsumeNext();

    if (Peek(TokenType::OPEN_PARENTHESIS)) tokens[2] = 0; 
    ConsumeNext();

    if (Peek(TokenType::CLOSE_PARENTHESIS)) tokens[1] = 0;
    ConsumeNext();

    if (Peek(TokenType::OPEN_CURLY)) tokens[0] = 0;
    ConsumeNext();

    if(tokens == std::bitset<4>{0b0000}){
        Expected("Expected the 'function' keyword", GENERATE_POSITION);
	return true;
    }

    return false;
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
	   (BackTrack() && ParseFunction())            ||
           // Our Compiler is aware of some frequent errors
	   (BackTrack() && ParseErrorenousFunction())
	);
}

bool Parser::ParseStatements(){
   StoreParserPosition();

   return(
	   (ParseStatement() && ParseStatements()) ||
           true
	 
	 );
}

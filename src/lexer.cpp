#include "lexer.hpp"
#include "utils.hpp"

#include <stdexcept>
#include <format>
#include <iostream>

std::map<std::string, TokenType> Lexer::keywords = {
    {"export", TokenType::EXPORT},
    {"function", TokenType::FUNCTION},
    {"where", TokenType::WHERE},
    {"from", TokenType::FROM},
    {"import", TokenType::IMPORT},
    {"i32", TokenType::I32},
    {"i64", TokenType::I64},
    {"f32", TokenType::F32},
    {"f64", TokenType::F64}	
};

Lexer::Lexer(std::string str){
  column = 1;
  line = 1;
  current_scan_position = 0;
  source_code = str;

  source_code_by_line = split_str(str);
}

bool Lexer::IsAtEnd(){
  return current_scan_position >= source_code.length();	
}

void Lexer::Tokenize(){
  while(!IsAtEnd()){
    char current_char;
     try{
       current_char = source_code.at(current_scan_position);
     } catch (std::out_of_range& e){
	 return;    
     }
     ScanToken(current_char);
     current_scan_position = current_scan_position + 1;	  
  }
}

char Lexer::Peek(){
  try{
     return source_code.at(current_scan_position + 1);  
  } catch (std::out_of_range& exp){
    return '\0';  
  }
}

char Lexer::PeekNext(){
  try{
     return source_code.at(current_scan_position + 2);	  
  } catch (std::out_of_range& exp){
     return '\0';	  
  }	
}

void Lexer::ConsumeNext(){
  current_scan_position = current_scan_position + 1;	
}

void Lexer::AddToken(Token tok){
  token_vec.push_back(tok);	
}

void Lexer::ScanToken(const char c){
  switch(c){
     case '\n': {
		  line = line + 1;
		  column = 1;
		  break;
		}
     case ' ': {
		  column = column + 1;     
		  break;
	       }
     case '+': {
		  AddToken(Token{TokenType::PLUS, line, column, c}); 
		  column = column + 1; 
		  break;
	       }
     case '-': {
		  AddToken(Token{TokenType::MINUS, line, column, c});
		  column = column + 1;  
		  break;
	       }
     case '*': {
		 AddToken(Token{TokenType::MUL, line, column,  c});
		 column = column + 1; 
		 break;
	       }
     case '/': {
		 AddToken(Token{TokenType::DIV, line, column, c}); 
		 column = column + 1;
		 break;
	       }
     case '%': {
		 AddToken({TokenType::MOD, line, column, c}); 
		 column = column + 1; 
		 break;
	       }
     case '(': {
		 AddToken(Token{TokenType::OPEN_PARENTHESIS, line, column, c}); 
		 column = column + 1; 
		 break;
	       }
     case ')': {
		 AddToken(Token{TokenType::CLOSE_PARENTHESIS, line, column, c}); 
		 column = column + 1; 
		 break;
	       }
     case '{': {
		 AddToken(Token{TokenType::OPEN_CURLY, line, column, c});
		 column = column + 1;
		 break;
	       }
     case '}': {
		 AddToken(Token{TokenType::CLOSE_CURLY, line, column, c}); 
		 column = column + 1;
		 break;
	       }
     case '[': {
		 AddToken(Token{TokenType::OPEN_SQUARE, line, column, c});
		 column = column + 1; 
		 break;
	       }
     case ']': {
		 AddToken(Token{TokenType::CLOSE_SQUARE, line, column, c});
		 column = column + 1; 
		 break;
	       }
     case '#': {
		 while (!(Peek() == '\n')){
	           ConsumeNext(); // skip comment;		 
		 }       
		 AddToken(Token{TokenType::HASH, line, column, c});
		 column = column + 1;
		 break;
	       }
     case ',': {
		 AddToken(Token{TokenType::COMMA, line, column, c}); 
		 column = column + 1; 
		 break;
	       }
     case '=': {
		 if (Peek() == '=') {
	            AddToken(Token{TokenType::EQ, line, column, "=="}); 
		    ConsumeNext(); 
		    column = column + 2;
		 }
		 else {
	            AddToken(Token{TokenType::ASSIGN, line,column, c}); 
		    column = column + 1;
		 }

		 break;

	       }
     case '<': {
		 if (Peek() == '=') {
		     AddToken(Token{TokenType::LTE, line, column, "<="}); 
		     ConsumeNext() ; 
	             column = column + 2;
		 }
		 else {
		    AddToken(Token{TokenType::LT, line, column, c}); 
                    column = column + 1;
		 }
		 break;
	       }
     case '>': {
		 if (Peek() == '=') {
		    AddToken(Token{TokenType::GTE, line, column, ">="});
		    ConsumeNext();
		    column = column + 2;
		 }
	         else {
		    AddToken(Token{TokenType::GT, line, column, c});
		    ConsumeNext();
		    column = column + 1;
		 }

		 break;
	       }
     case '!': {
		 if (Peek() == '=') {
		    AddToken(Token{TokenType::NEQ, line, column, "!="});
		    ConsumeNext();
		    column = column + 2;
		 }
		 else {
		    AddToken(Token{TokenType::NOT, line, column, c});
		    column = column + 1;
		 }
		 break;
	       }
     default: 
	   // handle numbers
           if (isdigit(c)){
	      std::string number = std::format("{}{}",c,Number());
	      AddToken(Token{TokenType::NUMBER, line, column, number});
	      column = column + 1;
	   } else if (isalnum(c)){ // handle id and keywords
	     std::string id = std::format("{}{}",c, Identifier());
	     
	     auto found = keywords.find(id);

	     if (found != keywords.end()){    
		AddToken(Token{found->second, line, column, found->first});     
	     }
	     else AddToken(Token{TokenType::ID, line, column, id});
	     
	     column = column + 1;
	   
	   }else{
	     std::string err_msg = std::format("Unkown Token: '{}'", c);
	     Error(err_msg);
	   }
  }	
}

std::string Lexer::Number(){
  std::string num_str = "";
  while(isdigit(Peek())){
    num_str.push_back(Peek()); 	  
    ConsumeNext();
    column = column + 1;
  }
  if(Peek() == '.' && isdigit(PeekNext())){
    num_str.push_back(Peek());
    ConsumeNext();
    column = column + 1;
  }
  while(isdigit(Peek())){
    num_str.push_back(Peek());
    ConsumeNext();
    column = column + 1;
  }
  
  return num_str;

}

std::string Lexer::Identifier(){
  std::string id_str = "";
  while(isalnum(Peek()) || Peek() == '_'){
    id_str.push_back(Peek());
    ConsumeNext();
    column = column + 1;
  }
  return id_str;

}

void Lexer::Error(const std::string& err_msg){
  std::cout << source_code_by_line[line - 1] << '\n';
  color("green", setArrow(column), true);
  std::cout << "[ " << line << ":" << column <<  " ] ";
  color("red", "Error: ");
  color("blue", err_msg);

  std::exit(0);
}


void Lexer::Debug(){
  for (const auto& elt: token_vec){
    std::cout << elt << '\n';	  
  }	
}

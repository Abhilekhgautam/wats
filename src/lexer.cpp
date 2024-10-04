#include "lexer.hpp"
#include <stdexcept>
#include <format>
#include <iostream>

Lexer::Lexer(std::string str){
  column = 0;
  line = 1;
  current_scan_position = 0;
  source_code = str;
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
     case '+': {AddToken(Token::PLUS); column = column + 1;  break;}
     case '-': {AddToken(Token::MINUS); column = column + 1;  break;}
     case '*': {AddToken(Token::MUL); column = column + 1;  break;}
     case '/': {AddToken(Token::DIV); column = column + 1;  break;}
     case '%': {AddToken(Token::MOD); column = column + 1;  break;}
     case '(': {AddToken(Token::OPEN_PARENTHESIS); column = column + 1;  break;}
     case ')': {AddToken(Token::CLOSE_PARENTHESIS); column = column + 1;  break;}
     case '{': {AddToken(Token::OPEN_CURLY); column = column + 1;  break;}
     case '}': {AddToken(Token::CLOSE_CURLY); column = column + 1;  break;}
     case '[': {AddToken(Token::OPEN_SQUARE); column = column + 1;  break;}
     case ']': {AddToken(Token::CLOSE_SQUARE); column = column + 1;  break;}
     case '#': {
		 while (!(Peek() == '\n')){
	           ConsumeNext(); // skip comment;		 
		 }       
		 AddToken(Token::HASH);
		 column = column + 1;
		 break;
	       }
     case ',': {AddToken(Token::COMMA); column = column + 1;  break;}
     case '=': {
		 if (Peek() == '=') {AddToken(Token::EQ); ConsumeNext(); column = column + 2;}
		 else {AddToken(Token::ASSIGN); column = column + 1;}

		 break;

	       }
     case '<': {
		 if (Peek() == '=') {
		     AddToken(Token::LTE); 
		     ConsumeNext() ; 
	             column = column + 2;
		 }
		 else {
		    AddToken(Token::LT); 
                    column = column + 1;
		 }
		 break;
	       }
     case '>': {
		 if (Peek() == '=') {
		    AddToken(Token::GTE);
		    ConsumeNext();
		    column = column + 2;
		 }
	         else {
		    AddToken(Token::GT);
		    ConsumeNext();
		    column = column + 1;
		 }

		 break;
	       }
     case '!': {
		 if (Peek() == '=') {
		    AddToken(Token::NEQ);
		    ConsumeNext();
		    column = column + 1;
		 }
		 else {
		    AddToken(Token::NOT);
		    column = column + 1;
		 }
		 break;
	       }
     default: 
           if (isdigit(c)){
	      column = column + 1;
	      [[maybe_unused]]std::string number = std::format("{}{}",c,Number());
	      AddToken(Token::NUMBER);
	   } else if (isalnum(c)){
	     column = column + 1;
	     [[maybe_unused]]std::string id = std::format("{}{}",c, Identifier());
	     AddToken(Token::ID);
	   }else{
	     std::string err_msg = std::format("Unkown Token: {}", c);
	     error(err_msg);
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



void Lexer::error(const std::string& err_msg){
  std::cout << "[ " << line << ":" << column <<  " ] " << err_msg;	
  std::exit(0);
}

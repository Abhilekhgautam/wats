#ifndef TOKEN_HPP
#define TOKEN_HPP

#include <string>
#include <iostream>

enum class TokenType{
  PLUS = '+',
  MINUS = '-',
  MUL = '*',
  DIV = '/',
  MOD = '%',
  EQ,
  NOT,
  NEQ,
  LT,
  GT,
  LTE,
  GTE,
  ID,
  NUM,
  OPEN_PARENTHESIS,
  CLOSE_PARENTHESIS,
  OPEN_SQUARE,
  CLOSE_SQUARE,
  OPEN_CURLY,
  CLOSE_CURLY,
  COMMA,
  HASH,
  ASSIGN,
  NUMBER,

  // Keywords
  EXPORT,
  IMPORT,
  I32,
  I64,
  F32,
  F64,
  LOOP,
  FUNCTION,
  CONST,
  WHERE,
  FROM
};

class Token{
  public:
    Token(TokenType tok_type, std::size_t line, std::size_t pos, std::string tok_value): tok_type(tok_type), line(line), pos(pos), tok_value(tok_value){}
    
    Token(TokenType tok_type, std::size_t line, std::size_t pos, char tok_value): tok_type(tok_type), line(line), pos(pos), tok_value(std::string{tok_value}){}
    
    friend std::ostream& operator << (std::ostream& os, Token tok){
	os << "line: " << tok.line << "col: " << tok.pos << " "<< tok.tok_value;    
	return os;
    }
  private:
    TokenType tok_type;
    std::size_t line;
    std::size_t pos;
    std::string tok_value;
};
#endif

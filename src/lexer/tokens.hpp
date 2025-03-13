#ifndef TOKEN_HPP
#define TOKEN_HPP

#include "../SourceLocation.hpp"

#include <iostream>
#include <string>

enum class TokenName {
  PLUS,
  MINUS,
  MUL,
  DIV,
  MOD,
  EQ,
  NOT,
  NEQ,
  LT,
  GT,
  LTE,
  GTE,
  ID,
  NUM,
  SEMI_COLON,
  OPEN_PARENTHESIS,
  CLOSE_PARENTHESIS,
  OPEN_SQUARE,
  CLOSE_SQUARE,
  OPEN_CURLY,
  CLOSE_CURLY,
  COMMA,
  ARROW,
  ASSIGN,
  COLON,
  NUMBER,
  // Keywords
  EXPORT,
  IMPORT,
  I32,
  I64,
  F32,
  F64,
  LOOP,
  FOR,
  WHILE,
  FUNCTION,
  LET,
  CONST,
  WHERE,
  FROM,
  TO,
  IN,
  MATCH,
  IF,
  ELSE,
  BREAK
};

std::ostream &operator<<(std::ostream &os, TokenName type);

class Token {
public:
  Token(TokenName tok_type, SourceLocation loc,
        std::string tok_value)
      : tok_type(tok_type), location(loc), tok_value(tok_value) {}

  Token(TokenName tok_type, SourceLocation loc, char tok_value)
      : tok_type(tok_type), location(loc),
        tok_value(std::string{tok_value}) {}

  friend std::ostream &operator<<(std::ostream &os, Token tok) {
    os << "line: " << tok.GetLine() << " col: " << tok.GetColumn() << " " << tok.tok_value
       << " " << tok.tok_type;
    return os;
  }
  inline std::string GetValue() const { return tok_value; }
  inline int GetLine() const { return location.GetLine(); }
  inline int GetColumn() const { return location.GetColumn(); }
  inline TokenName GetTokenName() const{ return tok_type; }
  inline bool IsIdentifier() const { return tok_type == TokenName::ID; }

  inline SourceLocation& GetSourceLocation() {return location;}

private:
  TokenName tok_type;
  SourceLocation location;
  std::string tok_value;
};
#endif

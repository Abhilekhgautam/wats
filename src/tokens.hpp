#ifndef TOKEN_HPP
#define TOKEN_HPP

enum class Token{
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
};

#endif

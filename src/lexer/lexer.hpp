#ifndef LEXER_HPP
#define LEXER_HPP

#include <map>
#include <string>
#include <vector>

#include "../CompilerContext.hpp"
#include "tokens.hpp"

class Lexer {
public:
  Lexer(CompilerContext& context);
  void Tokenize();
  // Write the given error msg to stdout
  void Error(const std::string &err_msg);

  const std::vector<Token>& GetTokens();

  static std::map<std::string, TokenName> keywords;

  CompilerContext& context;

  // Print the Token Information
  void Debug();

private:
  // Maintain the current column position
  int column;
  // Maintain the current line number
  int line;
  // Position where the Lexer is currently at
  std::size_t current_scan_position;

  std::vector<Token> token_vec;

  // Check if the Lexer reached the end of string
  bool IsAtEnd();
  void ScanToken(const char c);
  // Push token to the token_vec
  void AddToken(Token tok);
  // Return the element next to current_scan_position
  char Peek();
  // Return the element next to next of current_scan_position
  char PeekNext();
  // Eat the next char
  void ConsumeNext();

  std::string Number();
  std::string Identifier();
  std::string String();
};
#endif

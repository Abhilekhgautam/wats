#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <array>

#include "tokens.hpp"

class Lexer{
  public:
    Lexer(std::string str);
    void Tokenize();
    void error(const std::string& err_msg);
    void debug();
  private:
    std::string source_code;
    std::size_t column;
    std::size_t line;
    std::size_t current_scan_position;

    std::vector<Token> token_vec;

    std::array<Token, 2> lookahead_buffer;

    bool IsAtEnd();
    void ScanToken(const char c);
    void AddToken(Token tok);
    char Peek();
    char PeekNext();
    void ConsumeNext();

    std::string Number();
    std::string Identifier();
};
#endif

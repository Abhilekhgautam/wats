#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <map>

#include "tokens.hpp"

class Lexer{
  public:
    Lexer(std::string str);
    void Tokenize();
    // Write the given error msg to stdout
    void Error(const std::string& err_msg);
    
    const std::vector<Token>& GetTokens();
    const std::vector<std::string>& GetSourceCode();
    // Print the Token Information
    void Debug();
  private:
    std::string source_code;
    std::vector<std::string> source_code_by_line;
    // Maintain the current column positin
    std::size_t column;
    // Maintain the current line number
    std::size_t line;
    // Position where the Lexer is currently at
    std::size_t current_scan_position;

    std::vector<Token> token_vec;
    
    static std::map<std::string, TokenType> keywords;
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
};
#endif

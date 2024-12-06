#ifndef PARSER_HPP
#define PARSER_HPP 

#include <vector>
#include "tokens.hpp"

class Parser{
  public:
    Parser(const std::vector<Token>& token_vec, const std::vector<std::string>& source_code_by_line): token_vec(token_vec), source_code_by_line(source_code_by_line), current_parser_position(-1){}
    
    void Parse();

    std::size_t GetExprLength();

    bool DidYouMean(std::string to_add, std::size_t line, std::size_t column);
    void Error(const std::string& err_msg);
  private:

   std::vector<Token> token_vec;
   std::vector<std::string> source_code_by_line;
   std::size_t current_parser_position;
   std::size_t temp_parser_position;

   std::vector<Token> backup_token;
   bool IsAtEnd();
   bool PeekIgnoringNewLine(TokenType);
   bool Peek(TokenType);
   bool ConsumeNext();
   bool BackTrack();
   void StoreParserPosition();

   // Utility for Parsing Function Definition
   bool ParseFunctionWithRetType();
   bool ParseFunctionWithoutRetType();
   bool ParseFunction();
   bool ParseErrorenousFunction();
   // Utility for Parsing Variable Declaration
   bool ParseVariableDeclWithLet();
   bool ParseVariableDeclWithType();
   bool ParseVariableAssignment();
   bool ParseVariableInitWithLet();
   bool ParseVariableDecl();

   // Utility for Parsing Expression
   bool ParseExpressionBeginningWithID();
   bool ParseExpressionBeginningWithNumber();
   bool ParseExpressionBeginningWithBraces();

   bool ParsePlusExpression();
   bool ParseMinusExpression();
   bool ParseMulExpression();
   bool ParseDivExpression();
   bool ParseModExpression();
   bool ParseGtExpression();
   bool ParseLtExpression();
   bool ParseGteExpression();
   bool ParseLteExpression();
   bool ParseEmptyExpression();

   // Not the subtraction
   bool ParseSubExpression();

   bool ParseExpression();
   
   // Todo: Maybe a better name
   void ParseExpr();

   bool ParseRange();

   bool ParseCurlyBraceAndBody();

   bool ParseMatchArms();
   bool ParseMatchArm();
   bool ParseMatchStatement();

   bool ParseIfStatement();
   bool ParseElseStatement();
   bool ParseElseIfStatement();

   bool ParseLoop();
   bool ParseForLoop();
   bool ParseWhileLoop();

   bool ParseStatement();
   bool ParseStatements();

   bool Expected(const std::string, std::size_t line, std::size_t column);
   
   std::size_t expression_length;
};

#endif

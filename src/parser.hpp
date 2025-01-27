#ifndef PARSER_HPP
#define PARSER_HPP

#include <vector>
#include <optional>

#include "AST/ASTNode.hpp"

#include "tokens.hpp"
class Parser{
  public:
    Parser(const std::vector<Token>& token_vec, const std::vector<std::string>& source_code_by_line): token_vec(token_vec), source_code_by_line(source_code_by_line), current_parser_position(-1){}

    void Parse();

    std::size_t GetExprLength();

    void DidYouMean(std::string to_add, std::size_t line, std::size_t column);
    void Error(const std::string& err_msg);
  private:
   std::vector<Token> token_vec;
   std::vector<std::string> source_code_by_line;
   std::size_t current_parser_position;
   std::size_t temp_parser_position;

   std::vector<Token> backup_token;
   bool IsAtEnd();
   bool PeekIgnoringNewLine(TokenName);
   bool Peek(TokenName);

   Token GetCurrentToken();
   void ConsumeNext();
   void BackTrack();
   void StoreParserPosition();

   std::size_t GetNextTokenLength();

   // Utility for Parsing Function Definition
   std::optional<FunctionDefinitionAST*> ParseFunctionWithRetType();
   std::optional<FunctionDefinitionAST*> ParseFunctionWithoutRetType();
   std::optional<FunctionDefinitionAST*> ParseFunction();
   bool ParseErrorenousFunction();
   // Utility for Parsing Variable Declaration
   std::optional<VariableDeclarationAST*> ParseVariableDeclWithLet();
   std::optional<VariableDeclarationAST*> ParseVariableDeclWithType();
   std::optional<VariableAssignmentAST*> ParseVariableAssignment();
   std::optional<VariableDeclareAndAssignAST*> ParseVariableInitWithLet();
   std::optional<StatementAST*> ParseVariableDecl();

   // Utility for Parsing Expression
   std::optional<ExpressionAST*> ParseExpressionBeginningWithID();
   std::optional<ExpressionAST*> ParseExpressionBeginningWithNumber();
   std::optional<ExpressionAST*> ParseExpressionBeginningWithBraces();

   std::optional<ExpressionAST*> ParsePlusExpression();
   std::optional<ExpressionAST*> ParseMinusExpression();
   std::optional<ExpressionAST*> ParseMulExpression();
   std::optional<ExpressionAST*> ParseDivExpression();
   std::optional<ExpressionAST*> ParseModExpression();
   std::optional<ExpressionAST*> ParseGtExpression();
   std::optional<ExpressionAST*> ParseLtExpression();
   std::optional<ExpressionAST*> ParseGteExpression();
   std::optional<ExpressionAST*> ParseLteExpression();
   std::optional<ExpressionAST*> ParseEqualsExpression();

   // Not the subtraction
   std::optional<std::pair<ExpressionAST*, std::string>> ParseSubExpression();

   std::optional<ExpressionAST*> ParseExpression();

   std::optional<RangeAST*> ParseRange();

   std::optional<StatementAST*> ParseCurlyBraceAndBody();

   std::optional<std::vector<MatchArmAST*>> ParseMatchArms();
   std::optional<MatchArmAST*> ParseMatchArm();
   std::optional<StatementAST*> ParseMatchStatement();

   std::optional<StatementAST*> ParseIfStatement();
   std::optional<StatementAST*> ParseElseStatement();
   std::optional<StatementAST*> ParseElseIfStatement();

   std::optional<StatementAST*> ParseLoop();
   std::optional<StatementAST*> ParseForLoop();
   std::optional<StatementAST*> ParseWhileLoop();

   std::optional<StatementAST*> ParseStatement();
   std::optional<StatementAST*> ParseStatements();


   void Expected(const std::string, std::size_t line, std::size_t column);

};

#endif

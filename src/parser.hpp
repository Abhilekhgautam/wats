#ifndef PARSER_HPP
#define PARSER_HPP

#include <memory>
#include <optional>
#include <vector>

#include "AST/FunctionDefinitionAST.hpp"
#include "AST/VariableDeclarationAST.hpp"
#include "AST/VariableAssignmentAST.hpp"
#include "AST/VariableDeclareAndAssignAST.hpp"
#include "AST/MatchArmAST.hpp"
#include "AST/ForLoopAST.hpp"
#include "AST/WhileLoopAST.hpp"
#include "AST/LoopAST.hpp"
#include "AST/BreakStatementAST.hpp"
#include "AST/FunctionCallStatementAST.hpp"
#include "AST/IfStatementAST.hpp"
#include "AST/ElseIfStatementAST.hpp"
#include "AST/ElseStatementAST.hpp"
#include "AST/MatchStatementAST.hpp"
#include "AST/FunctionArgumentAST.hpp"


#include "tokens.hpp"
class Parser {
public:
  Parser(const std::vector<Token> &token_vec,
         const std::vector<std::string> &source_code_by_line)
      : token_vec(token_vec), source_code_by_line(source_code_by_line),
        current_parser_position(-1) {}

  void Parse();

  std::size_t GetExprLength();

  void DidYouMean(std::string to_add, std::size_t line, std::size_t column);
  void Error(const std::string &err_msg);

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
  std::optional<std::unique_ptr<FunctionDefinitionAST>>
  ParseFunctionWithRetType();
  std::optional<std::unique_ptr<FunctionDefinitionAST>>
  ParseFunctionWithoutRetType();
  std::optional<std::unique_ptr<FunctionDefinitionAST>> ParseFunction();

  std::optional<std::unique_ptr<FunctionArgumentAST>> ParseFunctionArguments();
  std::optional<std::unique_ptr<FunctionArgumentAST>> ParseFunctionArgument();

  // Utility for Parsing Variable Declaration
  std::optional<std::unique_ptr<VariableDeclarationAST>>
  ParseVariableDeclWithLet();
  std::optional<std::unique_ptr<VariableDeclarationAST>>
  ParseVariableDeclWithType();
  std::optional<std::unique_ptr<VariableAssignmentAST>>
  ParseVariableAssignment();
  std::optional<std::unique_ptr<VariableDeclareAndAssignAST>>
  ParseVariableInitWithLet();
  std::optional<std::unique_ptr<VariableDeclareAndAssignAST>>
  ParseVariableInitWithType();
  std::optional<std::unique_ptr<VariableDeclarationAST>> ParseVariableDecl();

  // Utility for Parsing Expression
  std::optional<std::unique_ptr<ExpressionAST>>
  ParseExpressionBeginningWithID();
  std::optional<std::unique_ptr<ExpressionAST>>
  ParseExpressionBeginningWithNumber();
  std::optional<std::unique_ptr<ExpressionAST>>
  ParseExpressionBeginningWithBraces();

  std::optional<std::unique_ptr<ExpressionAST>> ParsePlusExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseMinusExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseMulExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseDivExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseModExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseGtExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseLtExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseGteExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseLteExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseEqualsExpression();
  std::optional<std::unique_ptr<ExpressionAST>> ParseNotEqualsExpression();

  // Not the subtraction
  std::optional<std::pair<std::unique_ptr<ExpressionAST>, std::string>>
  ParseSubExpression();

  std::optional<std::unique_ptr<ExpressionAST>> ParseExpression();

  std::optional<std::unique_ptr<RangeAST>> ParseRange();

  std::optional<std::vector<std::unique_ptr<StatementAST>>>
  ParseCurlyBraceAndBody();

  std::optional<std::vector<std::unique_ptr<MatchArmAST>>> ParseMatchArms();
  std::optional<std::unique_ptr<MatchArmAST>> ParseMatchArm();
  std::optional<std::unique_ptr<MatchStatementAST>> ParseMatchStatement();

  std::optional<std::unique_ptr<IfStatementAST>> ParseIfStatement();
  std::optional<std::unique_ptr<ElseStatementAST>> ParseElseStatement();
  std::optional<std::unique_ptr<ElseIfStatementAST>> ParseElseIfStatement();

  std::optional<std::unique_ptr<BreakStatementAST>> ParseBreakStatement();
  std::optional<std::unique_ptr<FunctionCallAST>> ParseFunctionCallStatement();

  std::optional<std::unique_ptr<LoopAST>> ParseLoop();
  std::optional<std::unique_ptr<ForLoopAST>> ParseForLoop();
  std::optional<std::unique_ptr<WhileLoopAST>> ParseWhileLoop();

  std::optional<std::unique_ptr<StatementAST>> ParseStatement();
  std::optional<std::vector<std::unique_ptr<StatementAST>>> ParseStatements();

  void Expected(const std::string, std::size_t line, std::size_t column);
};

#endif

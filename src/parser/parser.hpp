#ifndef PARSER_HPP
#define PARSER_HPP

#include <list>
#include <memory>
#include <optional>
#include <vector>
#include <string_view>

#include "../AST/BreakStatementAST.hpp"
#include "../AST/ElseIfStatementAST.hpp"
#include "../AST/ElseStatementAST.hpp"
#include "../AST/ForLoopAST.hpp"
#include "../AST/FunctionArgumentAST.hpp"
#include "../AST/FunctionCallStatementAST.hpp"
#include "../AST/FunctionDefinitionAST.hpp"
#include "../AST/FunctionParameterAST.hpp"
#include "../AST/IfStatementAST.hpp"
#include "../AST/LoopAST.hpp"
#include "../AST/MatchArmAST.hpp"
#include "../AST/MatchStatementAST.hpp"
#include "../AST/VariableAssignmentAST.hpp"
#include "../AST/VariableDeclarationAST.hpp"
#include "../AST/VariableDeclareAndAssignAST.hpp"
#include "../AST/WhileLoopAST.hpp"

#include "../AST/OperatorAST.hpp"

#include "../CompilerContext.hpp"
#include "../lexer/tokens.hpp"

class Parser {
public:
  Parser(const CompilerContext &context, const std::vector<Token> &token_vec)
      : context(context), token_vec(token_vec), current_parser_position(0) {
    operator2Precedence["=="] = 10;
    operator2Precedence["<"] = 10;
    operator2Precedence[">"] = 10;
    operator2Precedence["<="] = 10;
    operator2Precedence[">="] = 10;
    operator2Precedence["!="] = 10;

    operator2Precedence["+"] = 20;
    operator2Precedence["-"] = 20;

    operator2Precedence["*"] = 40;
    operator2Precedence["/"] = 40;
    operator2Precedence["%"] = 40;
  }

  std::optional<std::vector<std::unique_ptr<StatementAST>>> Parse();

  std::size_t GetExprLength();

  void DidYouMean(std::string_view to_add, std::size_t line, std::size_t column,
                  bool space_before = true) const;
  void Error(const std::string &err_msg);

private:
  const CompilerContext &context;
  enum class ParserStatus {
    // When the parser encounters the keyword `function`.
    PARSING_FN_DEFINITION,
    PARSING_FN_DEFINITION_FAILED,
    PARSED_FN_DEFINTION,

    // When the parser encounters the `let` keyword
    PARSING_VARIABLE_DECLARATION,
    PARSING_VARIABLE_DECLARATION_FAILED,
    PARSED_VARIABLE_DECLARATION,

    PARSING_VARIABLE_ASSIGNMENT,
    PARSING_VARIABLE_ASSIGNMENT_FAILED,
    PARSED_VARIABLE_ASSIGNMENT,

    PARSING_FN_CALL,
    PARSING_FN_CALL_FAILED,
    PARSED_FN_CALL,

    // When the parser encounters the `if` keyword
    PARSING_IF_STATEMENT,
    PARSING_IF_STATEMENT_FAILED,
    PARSED_IF_STATEMENT,

    // When the parser encounters the `else if` keyword
    PARSING_ELSEIF_STATEMENT,
    PARSING_ELSEIF_STATEMENT_FAILED,
    PARSED_ELSEIF_STATEMENT,

    // When the parser encounters the `if` keyword
    PARSING_ELSE_STATEMENT,
    PARSING_ELSE_STATEMENT_FAILED,
    PARSED_ELSE_STATEMENT,

    // When the parser encounters the `match` keyword
    PARSING_MATCH_STATEMENT,
    PARSING_MATCH_STATEMENT_FAILED,
    PARSED_MATCH_STATEMENT,

    PARSING_MATCH_ARM,
    PARSING_MATCH_ARM_FAILED,
    PARSED_MATCH_ARM,

    PARSING_EXPRESSION,
    PARSING_EXPRESSION_FAILED,
    PARSED_EXPRESSION,

    // When the parser encounters `=` while variable declaration
    PARSING_VARIABLE_DECL_ASSIGN,
    PARSING_VARIABLE_DECL_ASSIGN_FAILED,
    PARSED_VARIABLE_DECL_ASSIGN,

    // When the parser encounters the `for` keyword
    PARSING_FOR_LOOP,
    PARSING_FOR_LOOP_FAILED,
    PARSED_FOR_LOOP,

    // When the parser encounters the `while` keyword
    PARSING_WHILE_LOOP,
    PARSING_WHILE_LOOP_FAILED,
    PARSED_WHILE_LOOP,

    // When the parser encounters the `loop` keyword
    PARSING_LOOP,
    PARSING_LOOP_FAILED,
    PARSED_LOOP,

    // While parsing the function id(s) are encountered after '('
    PARSING_FN_ARGUMENTS,
    PARSING_FN_ARGUMENTS_FAILED,
    PARSED_FN_ARGUMENTS,

    // While parsing function call id(s) are encountered after '('
    PARSING_FN_PARAMETERS,
    PARSING_FN_PARAMETERS_FAILED,
    PARSED_FN_PARAMETERS,

    // Immediately after the `while` keyword is encountered.
    PARSING_WHILE_CONDITION,
    PARSING_WHILE_CONDITION_FAILED,
    PARSED_WHILE_CONDITION,

    // Immediately after the `if` keyword is encountered.
    PARSING_IF_CONDITION,
    PARSING_IF_CONDITION_FAILED,
    PARSED_IF_CONDITION,

    // Immediately after the `else if` keyword is encountered.
    PARSING_ELSEIF_CONDITION,
    PARSING_ELSEIF_CONDITION_FAILED,
    PARSED_ELSEIF_CONDITION,
  };
  const std::vector<Token> &token_vec;
  std::size_t current_parser_position;
  std::size_t temp_parser_position;

  std::vector<Token> backup_token;
  bool IsAtEnd() const;
  bool PeekIgnoringNewLine(TokenName);
  bool Peek(TokenName) const;

  Token GetCurrentToken() const;
  void Consume();
  void BackTrack();
  void StoreParserPosition();

  std::size_t GetNextTokenLength();

  // Utility for Parsing Function Definition
  std::unique_ptr<FunctionDefinitionAST>
  ParseFunctionWithRetType();
  std::unique_ptr<FunctionDefinitionAST>
  ParseFunctionWithoutRetType();
  std::unique_ptr<FunctionDefinitionAST> ParseFunction();

  std::vector<std::unique_ptr<FunctionArgumentAST>> ParseFunctionArguments();
  std::unique_ptr<FunctionArgumentAST> ParseFunctionArgument();

  std::unique_ptr<FunctionParameterAST>
  ParseFunctionParameters();

  std::unique_ptr<ExpressionAST> ParseFunctionParameter();

  // Utility for Parsing Variable Declaration
  std::unique_ptr<VariableDeclarationAST>
  ParseVariableDeclWithLet();
  std::unique_ptr<VariableDeclarationAST>
  ParseVariableDeclWithType();
  std::unique_ptr<VariableAssignmentAST>
  ParseVariableAssignment();
  std::unique_ptr<VariableDeclareAndAssignAST>
  ParseVariableInitWithLet();
  std::unique_ptr<VariableDeclareAndAssignAST>
  ParseVariableInitWithType();
  std::unique_ptr<VariableDeclarationAST> ParseVariableDecl();

  std::unique_ptr<ExpressionAST> ParseIdentifier();
  std::unique_ptr<ExpressionAST> ParseNumber();
  std::unique_ptr<ExpressionAST> ParseParenExpr();
  std::unique_ptr<ExpressionAST> ParsePrimary();
  std::unique_ptr<ExpressionAST> ParseBinaryOpRHS(int exprPrec, std::unique_ptr<ExpressionAST> LHS);

  std::unique_ptr<ExpressionAST> ParseExpression();

  std::unique_ptr<RangeAST> ParseRange();

  std::vector<std::unique_ptr<StatementAST>>
  ParseCurlyBraceAndBody();

  std::vector<std::unique_ptr<MatchArmAST>> ParseMatchArms();
  std::unique_ptr<MatchArmAST> ParseMatchArm();
  std::unique_ptr<MatchStatementAST> ParseMatchStatement();

  std::unique_ptr<IfStatementAST> ParseIfStatement();
  std::unique_ptr<ElseStatementAST> ParseElseStatement();
  std::unique_ptr<ElseIfStatementAST> ParseElseIfStatement();

  std::unique_ptr<BreakStatementAST> ParseBreakStatement();
  std::unique_ptr<FunctionCallAST> ParseFunctionCallStatement();

  std::unique_ptr<LoopAST> ParseLoop();
  std::unique_ptr<ForLoopAST> ParseForLoop();
  std::unique_ptr<WhileLoopAST> ParseWhileLoop();

  std::unique_ptr<ReturnStatementAST> ParseReturnStatement();

  std::unique_ptr<StatementAST> ParseStatement();
  std::vector<std::unique_ptr<StatementAST>> ParseStatements();

  void Expected(const std::string&, std::size_t line, std::size_t column);
  void Unexpected(const std::string&, std::size_t line, std::size_t column,
                  std::size_t times = 0);

  inline bool CheckInsideFunction() const;

  std::list<ParserStatus> status_list;

private:
  int error_count = 0;
  std::unordered_map<std::string, int> operator2Precedence;

public:
  bool HasErrors() { return error_count > 0; }
  void IncrementErrorCount() { error_count++; }

  int GetPrecedence(const std::string&);
};

#endif

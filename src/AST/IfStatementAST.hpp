#ifndef IF_AST
#define IF_AST

#include "ElseStatementAST.hpp"
#include "ExpressionAST.hpp"
#include "StatementAST.hpp"

#include <memory>
#include <vector>

class IfStatementAST : public StatementAST {
public:
  IfStatementAST(std::unique_ptr<ExpressionAST> condition,
                 std::vector<std::unique_ptr<StatementAST>> if_body,
                 std::vector<std::unique_ptr<ElseIfStatementAST>> &else_if_ast,
                 std::unique_ptr<ElseStatementAST> else_ast, SourceLocation loc)
      : condition(std::move(condition)), if_body(std::move(if_body)),
        else_if_ast(std::move(else_if_ast)), loc(loc) {
    if (else_ast) {
      this->else_ast = std::move(else_ast);
    } else {
      else_ast = nullptr;
    }
  }

  virtual ~IfStatementAST() = default;

  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator &generator) override;

private:
  std::unique_ptr<ExpressionAST> condition;
  std::vector<std::unique_ptr<StatementAST>> if_body;
  std::vector<std::unique_ptr<ElseIfStatementAST>> else_if_ast;
  std::unique_ptr<ElseStatementAST> else_ast;
  SourceLocation loc;

public:
  void Debug() override;
  SourceLocation GetSourceLocation() override { return loc; };
  std::vector<std::unique_ptr<StatementAST>> &GetIfBody() { return if_body; }

  ExpressionAST &GetIfCondition() { return *condition; }

  bool hasElse() { return else_ast ? true : false; }
  bool hasElseIf() { return else_if_ast.size(); }

  ElseStatementAST &GetElseStatement() { return *else_ast; }
  std::vector<std::unique_ptr<ElseIfStatementAST>> &GetElseIfStatements() {
    return else_if_ast;
  }
};

#endif

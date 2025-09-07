#ifndef ELSE_IF_AST
#define ELSE_IF_AST

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"

#include <memory>
#include <vector>

class ElseIfStatementAST : public StatementAST {
public:
  ElseIfStatementAST(std::unique_ptr<ExpressionAST> condition,
                     std::vector<std::unique_ptr<StatementAST>> else_if_body,
                     SourceLocation loc)
      : condition(std::move(condition)), else_if_body(std::move(else_if_body)),
        loc(loc) {}

  virtual ~ElseIfStatementAST() = default;

  ExpressionAST &GetCondition() { return *condition; }
  std::vector<std::unique_ptr<StatementAST>> &GetBody() { return else_if_body; }

  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator &generator) override;

private:
  std::unique_ptr<ExpressionAST> condition;
  std::vector<std::unique_ptr<StatementAST>> else_if_body;
  SourceLocation loc;

public:
  void Debug() override;
  SourceLocation GetSourceLocation() override { return loc; }
};

#endif

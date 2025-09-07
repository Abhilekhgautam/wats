#ifndef WHILE_AST
#define WHILE_AST

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"
#include <vector>
/// while i < 5 {
///     # loop body
/// }
class WhileLoopAST : public StatementAST {
public:
  WhileLoopAST(std::unique_ptr<ExpressionAST> condition,
               std::vector<std::unique_ptr<StatementAST>> loop_body)
      : condition(std::move(condition)), loop_body(std::move(loop_body)) {}

  virtual ~WhileLoopAST() = default;

  ExpressionAST &GetCondition() { return *condition; }

  std::vector<std::unique_ptr<StatementAST>> &GetBody() { return loop_body; }

  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator &generator) override;

private:
  std::unique_ptr<ExpressionAST> condition;
  std::vector<std::unique_ptr<StatementAST>> loop_body;

public:
  void Debug() override;
  SourceLocation GetSourceLocation() override { return {}; }
};

#endif

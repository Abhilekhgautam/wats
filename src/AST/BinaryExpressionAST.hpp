#ifndef BINARY_EXPR
#define BINARY_EXPR

#include "ExpressionAST.hpp"
#include "OperatorAST.hpp"

#include "../IRGenerator/IRGenerator.hpp"
#include <nlohmann/json.hpp>

class BinaryExpressionAST : public ExpressionAST {
public:
  BinaryExpressionAST(std::unique_ptr<ExpressionAST> expr_lhs,
                      std::unique_ptr<ExpressionAST> expr_rhs, OperatorNode op,
                      std::vector<SourceLocation> &loc)
      : expr_lhs(std::move(expr_lhs)), expr_rhs(std::move(expr_rhs)), op(op),
        loc(loc) {}

  virtual ~BinaryExpressionAST() = default;

private:
  std::unique_ptr<ExpressionAST> expr_lhs;
  std::unique_ptr<ExpressionAST> expr_rhs;
  OperatorNode op;

  std::vector<SourceLocation> loc;

public:
  void Debug() override;
  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator &generator) override;

  std::string GetOperator() const { return op.GetOperatorSymbol(); }
  ExpressionAST &GetLeftOperand() const{ return *expr_lhs; }
  ExpressionAST &GetRightOperand() const { return *expr_rhs; }

  std::span<const SourceLocation> GetSourceLocation() override { return loc; }

  int GetLength() override;
};

#endif

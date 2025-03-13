#ifndef BINARY_EXPR
#define BINARY_EXPR

#include "ExpressionAST.hpp"
#include "OperatorAST.hpp"
#include <functional>

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
  std::string type;

  std::vector<SourceLocation> loc;

public:
  void Debug() override;
  void Accept(SemanticAnalyzer &analyzer) override;

  std::string GetOperator() const { return op.GetOperatorSymbol(); }
  std::reference_wrapper<ExpressionAST> GetLeftOperand() {
    return std::ref(*expr_lhs);
  }
  std::reference_wrapper<ExpressionAST> GetRightOperand() {
    return std::ref(*expr_rhs);
  }

  std::string GetType() override { return type; }
  void SetType(std::string type) { this->type = type; }

  std::span<const SourceLocation> GetSourceLocation() override { return loc; }
};

#endif

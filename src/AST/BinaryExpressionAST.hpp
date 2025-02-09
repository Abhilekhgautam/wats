#ifndef BINARY_EXPR
#define BINARY_EXPR

#include "ExpressionAST.hpp"

class BinaryExpressionAST : public ExpressionAST{
  public:
    BinaryExpressionAST(std::unique_ptr<ExpressionAST> expr_lhs, std::unique_ptr<ExpressionAST> expr_rhs, std::string operator_symbol)
	    : expr_lhs(std::move(expr_lhs)), expr_rhs(std::move(expr_rhs)), operator_symbol(operator_symbol){}

	virtual ~BinaryExpressionAST() = default;
  private:
    std::unique_ptr<ExpressionAST> expr_lhs;
    std::unique_ptr<ExpressionAST> expr_rhs;
    std::string operator_symbol;

    public:
       void Debug() override;

};

#endif

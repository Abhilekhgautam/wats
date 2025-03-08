#ifndef BINARY_EXPR
#define BINARY_EXPR

#include "ExpressionAST.hpp"
#include <functional>

class BinaryExpressionAST : public ExpressionAST{
  public:
    BinaryExpressionAST(std::unique_ptr<ExpressionAST> expr_lhs, std::unique_ptr<ExpressionAST> expr_rhs, std::string operator_symbol)
	    : expr_lhs(std::move(expr_lhs)), expr_rhs(std::move(expr_rhs)), operator_symbol(operator_symbol){}

	virtual ~BinaryExpressionAST() = default;
  private:
    std::unique_ptr<ExpressionAST> expr_lhs;
    std::unique_ptr<ExpressionAST> expr_rhs;
    std::string operator_symbol;
    std::string type;

    public:
       void Debug() override;
       void Accept(SemanticAnalyzer& analyzer) override;

       std::string GetOperator() const {return operator_symbol;}
       std::reference_wrapper<ExpressionAST> GetLeftOperand() {return std::ref(*expr_lhs);}
       std::reference_wrapper<ExpressionAST> GetRightOperand() {return std::ref(*expr_rhs);}

       std::string GetType() override { return type;}
       void SetType(std::string type) {this->type = type;}


};

#endif

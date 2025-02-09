#ifndef ELSE_IF_AST
#define ELSE_IF_AST

#include "StatementAST.hpp"
#include "ExpressionAST.hpp"

#include <vector>
#include <memory>

class ElseIfStatementAST : public StatementAST{
  public:
    ElseIfStatementAST(std::unique_ptr<ExpressionAST> condition, std::vector<std::unique_ptr<StatementAST>> else_if_body)
	: condition(std::move(condition)), else_if_body(std::move(else_if_body)){}

	virtual ~ElseIfStatementAST() = default;
   private:
    std::unique_ptr<ExpressionAST> condition;
    std::vector<std::unique_ptr<StatementAST>> else_if_body;

    public:
       void Debug() override;
};

#endif

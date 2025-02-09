#ifndef WHILE_AST
#define WHILE_AST

#include "StatementAST.hpp"
#include "ExpressionAST.hpp"
#include <vector>
/// while i < 5 {
///     # loop body
/// }
class WhileLoopAST : public StatementAST{
   public:
    WhileLoopAST(std::unique_ptr<ExpressionAST> condition, std::vector<std::unique_ptr<StatementAST>> loop_body)
	    : condition(std::move(condition)), loop_body(std::move(loop_body)){}

	virtual ~WhileLoopAST() = default;
   private:
    std::unique_ptr<ExpressionAST> condition;
    std::vector<std::unique_ptr<StatementAST>>  loop_body;
   public:
   void Debug() override;
};

#endif

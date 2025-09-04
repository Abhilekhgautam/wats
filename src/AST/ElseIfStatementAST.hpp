#ifndef ELSE_IF_AST
#define ELSE_IF_AST

#include "StatementAST.hpp"
#include "ExpressionAST.hpp"

#include <vector>
#include <memory>

class ElseIfStatementAST : public StatementAST{
  public:
    ElseIfStatementAST(std::unique_ptr<ExpressionAST> condition, std::vector<std::unique_ptr<StatementAST>> else_if_body, SourceLocation loc)
	: condition(std::move(condition)), else_if_body(std::move(else_if_body)), loc(loc){}

	virtual ~ElseIfStatementAST() = default;

	void Accept(SemanticAnalyzer& analyzer) override;
	nlohmann::json Accept(IRGenerator& generator) override;

   private:
    std::unique_ptr<ExpressionAST> condition;
    std::vector<std::unique_ptr<StatementAST>> else_if_body;
    SourceLocation loc;
    public:
       void Debug() override;
       SourceLocation GetSourceLocation() override {return loc;}
};

#endif

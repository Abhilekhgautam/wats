#ifndef IF_AST
#define IF_AST

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"

#include <memory>
#include <vector>

class IfStatementAST : public StatementAST{
 public:
    IfStatementAST(std::unique_ptr<ExpressionAST> condition, std::vector<std::unique_ptr<StatementAST>> if_body)
       : condition(std::move(condition)), if_body(std::move(if_body)){}

    virtual ~IfStatementAST() = default;
 private:
    std::unique_ptr<ExpressionAST> condition;
    std::vector<std::unique_ptr<StatementAST>> if_body;

public:
   void Debug() override;
};

#endif

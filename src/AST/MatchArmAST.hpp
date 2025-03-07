#ifndef MATCH_ARM_AST
#define MATCH_ARM_AST

#include "StatementAST.hpp"
#include "ExpressionAST.hpp"

#include <vector>

class MatchArmAST : public StatementAST{
    public:
       MatchArmAST(std::unique_ptr<ExpressionAST> cond, std::vector<std::unique_ptr<StatementAST>> body)
       : cond(std::move(cond)), body(std::move(body)){}

       virtual ~MatchArmAST() = default;
       void Accept(SemanticAnalyzer& analyzer) override;
    private:
      std::unique_ptr<ExpressionAST> cond;
      std::vector<std::unique_ptr<StatementAST>> body;
      public:
         void Debug() override;
};

#endif

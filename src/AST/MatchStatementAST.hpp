#ifndef MATCH_STMT_AST
#define MATCH_STMT_AST

#include "StatementAST.hpp"
#include "MatchArmAST.hpp"

class MatchStatementAST : public StatementAST{
  public:
     MatchStatementAST(std::unique_ptr<ExpressionAST> cond, std::vector<std::unique_ptr<MatchArmAST>> arms)
     : cond(std::move(cond)), arms(std::move(arms)){}

     virtual ~MatchStatementAST() = default;
  private:
    std::unique_ptr<ExpressionAST> cond;
    std::vector<std::unique_ptr<MatchArmAST>> arms;

    public:
       void Debug() override;
};

#endif

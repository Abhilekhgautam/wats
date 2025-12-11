#ifndef MATCH_STMT_AST
#define MATCH_STMT_AST

#include "MatchArmAST.hpp"
#include "StatementAST.hpp"

class MatchStatementAST : public StatementAST {
public:
  MatchStatementAST(std::unique_ptr<ExpressionAST> cond,
                    std::vector<std::unique_ptr<MatchArmAST>> arms)
      : cond(std::move(cond)), arms(std::move(arms)) {}

  virtual ~MatchStatementAST() = default;
  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator &generator) override;

private:
  std::unique_ptr<ExpressionAST> cond;
  std::vector<std::unique_ptr<MatchArmAST>> arms;

public:
  void Debug() override;
  ExpressionAST &GetCondition() { return *cond; }
  std::vector<std::unique_ptr<MatchArmAST>> &GetArms() { return arms; }
  SourceLocation GetSourceLocation() override { return {}; }
};

#endif

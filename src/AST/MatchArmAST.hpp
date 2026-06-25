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
    nlohmann::json Accept(IRGenerator& generator) override;

    ExpressionAST& getCondition() {return *cond;}
    std::unique_ptr<ExpressionAST> takeCondition()  {return std::move(cond);}

    std::vector<std::unique_ptr<StatementAST>>& getBody(){return body;}
    const std::vector<std::unique_ptr<StatementAST>>& getBody() const {return body;}


private:
      std::unique_ptr<ExpressionAST> cond;
      std::vector<std::unique_ptr<StatementAST>> body;
public:
      void Debug() override;
      SourceLocation GetSourceLocation() override {return {};}
};

#endif

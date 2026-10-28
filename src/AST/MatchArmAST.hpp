#ifndef MATCH_ARM_AST
#define MATCH_ARM_AST

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"

#include <vector>

class MatchArmAST : public StatementAST {
public:
    MatchArmAST(std::unique_ptr<ExpressionAST> cond, std::vector<std::unique_ptr<StatementAST>> body) :
        cond(std::move(cond)), body(std::move(body)) {}

    virtual ~MatchArmAST() = default;
    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::unique_ptr<ExpressionAST> cond;
    std::vector<std::unique_ptr<StatementAST>> body;

    void Debug() override;
    SourceLocation GetSourceLocation() override { return {}; }
};

#endif

#ifndef MATCH_STMT_AST
#define MATCH_STMT_AST

#include "MatchArmAST.hpp"
#include "StatementAST.hpp"

enum struct MatchType { ALL, ONCE };

class MatchStatementAST : public StatementAST {
public:
    MatchStatementAST(const MatchType &matchType, std::vector<std::unique_ptr<MatchArmAST>> arms) :
        type(matchType), arms(std::move(arms)) {}

    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    MatchType getType() { return type; }
    MatchType getType() const { return type; }

    std::vector<std::unique_ptr<MatchArmAST>> &getArms() { return arms; }
    const std::vector<std::unique_ptr<MatchArmAST>> &getArms() const { return arms; }

private:
    MatchType type;
    std::vector<std::unique_ptr<MatchArmAST>> arms;

public:
    void Debug() override;
    SourceLocation GetSourceLocation() override { return {}; }
};

#endif

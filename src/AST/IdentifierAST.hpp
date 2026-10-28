#ifndef ID_EXPR
#define ID_EXPR

#include "ExpressionAST.hpp"

class IdentifierAST : public ExpressionAST {
public:
    IdentifierAST(std::string name, SourceLocation loc) : name(std::move(name)), loc(loc) {}

    virtual ~IdentifierAST() override = default;

    std::string name;
    SourceLocation loc;

    void Debug() override;
    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::span<const SourceLocation> GetSourceLocation() override { return {&loc, 1}; }
    int GetLength() override { return name.length(); }
};

#endif

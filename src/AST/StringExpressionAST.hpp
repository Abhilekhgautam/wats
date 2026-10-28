#ifndef WATS_STRING_AST
#define WATS_STRING_AST

#include "ExpressionAST.hpp"

class StringExpressionAST : public ExpressionAST {
public:
    const std::string value;
    const SourceLocation loc;

    StringExpressionAST(std::string val, SourceLocation loc);

    void Debug() override;
    void Accept(SemanticAnalyzer&) override;
    nlohmann::json Accept(IRGenerator&) override;

    std::span<const SourceLocation> GetSourceLocation() override {return {&loc, 1};}
    int GetLength() override {return value.length() + 1;}

};

#endif

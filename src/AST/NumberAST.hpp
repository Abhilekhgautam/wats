#ifndef NUM_AST
#define NUM_AST

#include "ExpressionAST.hpp"

#include <variant>

class NumberAST : public ExpressionAST {
public:
    NumberAST(const std::string num, bool has_decimal, const SourceLocation loc) :
        num(num), has_decimal(has_decimal), loc(loc) {}

    virtual ~NumberAST() = default;

    std::string num;
    bool has_decimal;

    std::variant<long long, double> value;
    SourceLocation loc;

    void Debug() override;
    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::span<const SourceLocation> GetSourceLocation() override { return {&loc, 1}; }
    std::span<const SourceLocation> GetSourceLocation() const { return {&loc, 1}; }
    int GetLength() override { return num.length(); }
};

#endif

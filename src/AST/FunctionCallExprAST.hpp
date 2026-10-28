#ifndef FN_CALL_EXPR
#define FN_CALL_EXPR

#include "ExpressionAST.hpp"

#include "../SourceLocation.hpp"
#include "FunctionParameterAST.hpp"
#include "IdentifierAST.hpp"

class FunctionCallExprAST : public ExpressionAST {
public:
    FunctionCallExprAST(std::unique_ptr<IdentifierAST> fn_name, std::unique_ptr<FunctionParameterAST> args, SourceLocation &loc) :
        fn_name(std::move(fn_name)), args(std::move(args)), loc(loc) {}

    virtual ~FunctionCallExprAST() override = default;
    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::unique_ptr<IdentifierAST> fn_name;
    std::unique_ptr<FunctionParameterAST> args;
    SourceLocation loc;

    void Debug() override;
    std::span<const SourceLocation> GetSourceLocation() override { return {&loc, 1}; }
    int GetLength() override;
};

#endif

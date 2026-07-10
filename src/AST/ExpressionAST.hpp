#ifndef EXPR_AST
#define EXPR_AST

#include "../SourceLocation.hpp"
#include "../semantics/semanticAnalyzer.hpp"

#include <nlohmann/json.hpp>

#include <span>
class IRGenerator;
class ExpressionAST {
public:
    virtual std::string GetType() const { return type; }
    virtual void SetType(const std::string &t) { type = t; }
    virtual ~ExpressionAST() = default;
    virtual void Accept(SemanticAnalyzer &analyzer) = 0;
    virtual nlohmann::json Accept(IRGenerator &generator) = 0;
    virtual void Debug() = 0;

    virtual std::span<const SourceLocation> GetSourceLocation() = 0;

    virtual int GetLength() = 0;

private:
    std::string type;
};

#endif

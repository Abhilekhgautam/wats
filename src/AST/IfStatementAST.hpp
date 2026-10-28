#ifndef IF_AST
#define IF_AST

#include "ElseStatementAST.hpp"
#include "ExpressionAST.hpp"
#include "StatementAST.hpp"

#include <memory>
#include <vector>

class IfStatementAST : public StatementAST {
public:
    IfStatementAST(std::unique_ptr<ExpressionAST> condition, std::vector<std::unique_ptr<StatementAST>> if_body,
                   std::vector<std::unique_ptr<ElseIfStatementAST>> &else_if_ast,
                   std::unique_ptr<ElseStatementAST> else_ast, SourceLocation loc) :
        condition(std::move(condition)), if_body(std::move(if_body)), else_if_ast(std::move(else_if_ast)), loc(loc) {
        if (else_ast) {
            this->else_ast = std::move(else_ast);
        } else {
            else_ast = nullptr;
        }
    }

    virtual ~IfStatementAST() override = default;

    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::unique_ptr<ExpressionAST> condition;
    std::vector<std::unique_ptr<StatementAST>> if_body;
    std::vector<std::unique_ptr<ElseIfStatementAST>> else_if_ast;
    std::unique_ptr<ElseStatementAST> else_ast;
    SourceLocation loc;

    void Debug() override;
    SourceLocation GetSourceLocation() override { return loc; };

    bool hasElse() const { return else_ast ? true : false; }
    bool hasElseIf() const { return else_if_ast.size(); }

};

#endif

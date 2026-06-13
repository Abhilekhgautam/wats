//
// Created by void on 6/13/26.
//

#ifndef WATS_RETURNSTATEMENTAST_H
#define WATS_RETURNSTATEMENTAST_H

#include "StatementAST.hpp"
#include "ExpressionAST.hpp"

class ReturnStatementAST : public StatementAST {
public:
    ReturnStatementAST(std::unique_ptr<ExpressionAST> expr, const SourceLocation& loc) : expr(std::move(expr)), location(loc){}
    virtual ~ReturnStatementAST() override = default;
    ExpressionAST& GetReturnExpression() const {return *expr;};

    void Accept(SemanticAnalyzer& analyzer) override;
    nlohmann::json Accept(IRGenerator& generator) override;
    SourceLocation GetSourceLocation() override {return location;};

    void Debug() override;
private:
    std::unique_ptr<ExpressionAST> expr;
    SourceLocation location;
};


#endif //WATS_RETURNSTATEMENTAST_H
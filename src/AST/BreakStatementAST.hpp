#ifndef BREAK_AST
#define BREAK_AST

#include "StatementAST.hpp"

class BreakStatementAST : public StatementAST{
    public:
     BreakStatementAST() = default;
     virtual ~BreakStatementAST() = default;

     void Accept(SemanticAnalyzer& analyzer) override;

    void Debug() override;
};

#endif

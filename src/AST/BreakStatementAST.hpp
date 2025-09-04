#ifndef BREAK_AST
#define BREAK_AST

#include "StatementAST.hpp"

class BreakStatementAST : public StatementAST{
    public:
     BreakStatementAST(SourceLocation location): loc(location){}
     virtual ~BreakStatementAST() = default;

     void Accept(SemanticAnalyzer& analyzer) override;
     nlohmann::json Accept(IRGenerator& generator) override;

    void Debug() override;
    SourceLocation GetSourceLocation() override {return loc;};

    private:
        SourceLocation loc;
};

#endif

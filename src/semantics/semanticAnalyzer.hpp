#ifndef SEM_ANALYZER_HPP
#define SEM_ANALYZER_HPP

#include "./scope.hpp"

class StatementAST;
class VariableDeclarationAST;
class VariableAssignmentAST;
class VariableDeclareAndAssignAST;
class WhileLoopAST;
class ForLoopAST;
class LoopAST;
class BreakStatementAST;
class IfStatementAST;
class ElseIfStatementAST;
class ElseStatementAST;
class FunctionArgumentAST;
class FunctionCallAST;
class FunctionDefinitionAST;
class MatchStatementAST;
class MatchArmAST;

class ExpressionAST;
class BinaryExpressionAST;
class IdentifierAST;
class NumberAST;
class RangeAST;
class FunctionCallAST;
class FunctionCallExprAST;


#include <vector>
#include <memory>

class SemanticAnalyzer {
    public:
        SemanticAnalyzer(std::vector<std::unique_ptr<StatementAST>>& stmt_ast): stmt_ast(stmt_ast){}
        void analyze();
    public:
        std::vector<std::unique_ptr<StatementAST>>& stmt_ast;
        void Visit(VariableDeclarationAST& ast);
        void Visit(VariableAssignmentAST& ast);
        void Visit(VariableDeclareAndAssignAST& ast);
        void Visit(WhileLoopAST& ast);
        void Visit(ForLoopAST& ast);
        void Visit(LoopAST& ast);
        void Visit(BreakStatementAST& ast);
        void Visit(IfStatementAST& ast);
        void Visit(ElseIfStatementAST& ast);
        void Visit(ElseStatementAST& ast);
        void Visit(FunctionArgumentAST& ast);
        void Visit(FunctionCallAST& ast);
        void Visit(FunctionDefinitionAST& ast);
        void Visit(MatchStatementAST& ast);
        void Visit(MatchArmAST& ast);

        void Visit(BinaryExpressionAST& ast);
        void Visit(IdentifierAST& ast);
        void Visit(NumberAST& ast);
        void Visit(RangeAST& ast);
        void Visit(FunctionCallExprAST& ast);

    private:
        Scope current_scope;

};

#endif

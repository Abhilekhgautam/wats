#ifndef ID_GEN_H
#define ID_GEN_H

#include <numbers>
#include <nlohmann/json.hpp>
#include "../AST/VariableAssignmentAST.hpp"
#include "../AST/BreakStatementAST.hpp"
#include "../AST/ElseIfStatementAST.hpp"

#include "IRContext.hpp"

class IRGenerator{
    public:
      IRGenerator() = default;

      nlohmann::json Generate(ExpressionAST& ast);
      nlohmann::json Generate(StatementAST& ast);

      nlohmann::json Generate(const VariableAssignmentAST& ast);
      nlohmann::json Generate(const VariableDeclareAndAssignAST& ast);
      nlohmann::json Generate(const BinaryExpressionAST& ast);
      nlohmann::json Generate(const BreakStatementAST& ast);
      nlohmann::json Generate(const ElseIfStatementAST& ast);
      nlohmann::json Generate(const ElseStatementAST& ast);
      nlohmann::json Generate(const ForLoopAST& ast);
      nlohmann::json Generate(const FunctionArgumentAST& ast);
      nlohmann::json Generate(const FunctionCallExprAST& ast);
      nlohmann::json Generate(const FunctionCallAST& ast);
      nlohmann::json Generate(const FunctionDefinitionAST& ast);
      nlohmann::json Generate(const IdentifierAST& ast);
      nlohmann::json Generate(const IfStatementAST& ast);
      nlohmann::json Generate(const LoopAST& ast);
      nlohmann::json Generate(const MatchArmAST& ast);
      nlohmann::json Generate(const MatchStatementAST& ast);
      nlohmann::json Generate(const NumberAST& ast);
      nlohmann::json Generate(const RangeAST& ast);
      nlohmann::json Generate(const VariableDeclarationAST& ast);
      nlohmann::json Generate(const WhileLoopAST& ast);
      nlohmann::json Generate(const ReturnStatementAST& ast);

private:
    IRContext ctx;

public:
    IRContext& GetContext() {return ctx;}

};

#endif

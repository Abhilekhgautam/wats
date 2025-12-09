#ifndef ID_GEN_H
#define ID_GEN_H

#include "../AST/BreakStatementAST.hpp"
#include "../AST/ElseIfStatementAST.hpp"
#include "../AST/VariableAssignmentAST.hpp"
#include <nlohmann/json.hpp>
class IRGenerator {
public:
  IRGenerator() = default;

  nlohmann::json Generate(ExpressionAST &ast);
  nlohmann::json Generate(StatementAST &ast);

  nlohmann::json Generate(VariableAssignmentAST &ast);
  nlohmann::json Generate(VariableDeclareAndAssignAST &ast);
  nlohmann::json Generate(BinaryExpressionAST &ast);
  nlohmann::json Generate(BreakStatementAST &ast);
  nlohmann::json Generate(ElseIfStatementAST &ast);
  nlohmann::json Generate(ElseStatementAST &ast);
  nlohmann::json Generate(ForLoopAST &ast);
  nlohmann::json Generate(FunctionArgumentAST &ast);
  nlohmann::json Generate(FunctionCallExprAST &ast);
  nlohmann::json Generate(FunctionCallAST &ast);
  nlohmann::json Generate(FunctionDefinitionAST &ast);
  nlohmann::json Generate(IdentifierAST &ast);
  nlohmann::json Generate(IfStatementAST &ast);
  nlohmann::json Generate(LoopAST &ast);
  nlohmann::json Generate(MatchArmAST &ast);
  nlohmann::json Generate(MatchStatementAST &ast);
  nlohmann::json Generate(NumberAST &ast);
  nlohmann::json Generate(RangeAST &ast);
  nlohmann::json Generate(VariableDeclarationAST &ast);
  nlohmann::json Generate(WhileLoopAST &ast);

  std::string NewTempVar();
  std::string extract_ir_result(nlohmann::json &result_json,
                                nlohmann::json &instruction_array);
};

#endif

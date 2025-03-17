#ifndef SEM_ANALYZER_HPP
#define SEM_ANALYZER_HPP

#include "./functionSymbolTable.hpp"
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

#include "../CompilerContext.hpp"

#include <memory>
#include <vector>

class SemanticAnalyzer {
public:
  SemanticAnalyzer(CompilerContext &context,
                   std::vector<std::unique_ptr<StatementAST>> &stmt_ast)
      : context(context), stmt_ast(stmt_ast) {}
  void analyze();
  CompilerContext &context;

public:
  std::vector<std::unique_ptr<StatementAST>> &stmt_ast;
  void Visit(VariableDeclarationAST &ast);
  void Visit(VariableAssignmentAST &ast);
  void Visit(VariableDeclareAndAssignAST &ast);
  void Visit(WhileLoopAST &ast);
  void Visit(ForLoopAST &ast);
  void Visit(LoopAST &ast);
  void Visit(BreakStatementAST &ast);
  void Visit(IfStatementAST &ast);
  void Visit(ElseIfStatementAST &ast);
  void Visit(ElseStatementAST &ast);
  void Visit(FunctionArgumentAST &ast);
  void Visit(FunctionCallAST &ast);
  void Visit(FunctionDefinitionAST &ast);
  void Visit(MatchStatementAST &ast);
  void Visit(MatchArmAST &ast);

  void Visit(BinaryExpressionAST &ast);
  void Visit(IdentifierAST &ast);
  void Visit(NumberAST &ast);
  void Visit(RangeAST &ast);
  void Visit(FunctionCallExprAST &ast);

public:
  void Error(const std::string str, std::size_t line, std::size_t column,
             int len = 1);
  void Expected(const std::string, std::size_t line, std::size_t column);
  void Unexpected(const std::string, std::size_t line, std::size_t column,
                  std::size_t times = 0);
  void AddFunctionToSymbolTable(std::string name, FunctionInfo info) {
    table.AddToSymbolTable(name, info);
  }
  std::optional<FunctionInfo> FindSymbolTable(std::string name) {
    return table.FindInSymbolTable(name);
  }

private:
  std::unique_ptr<Scope> current_scope;
  FunctionSymbolTable table;
};

#endif

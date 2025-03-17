#include "functionSymbolTable.hpp"
#include "scopeType.hpp"
#include <memory>
#include <type_traits>
#ifdef __EMSCRIPTEN__
#include <cerrno>
#endif

#include "scope.hpp"
#include "semanticAnalyzer.hpp"

#include "../AST/BinaryExpressionAST.hpp"
#include "../AST/ForLoopAST.hpp"
#include "../AST/FunctionCallExprAST.hpp"
#include "../AST/FunctionDefinitionAST.hpp"
#include "../AST/IdentifierAST.hpp"
#include "../AST/NumberAST.hpp"
#include "../AST/StatementAST.hpp"
#include "../AST/VariableAssignmentAST.hpp"
#include "../AST/VariableDeclarationAST.hpp"
#include "../AST/VariableDeclareAndAssignAST.hpp"

#include <cassert>
#include <charconv>
#include <iostream>

void SemanticAnalyzer::Error(const std::string str, std::size_t line,
                             std::size_t column, int len) {
  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << context.source_code_by_line[line - 1] << '\n';
  Color("green", SetArrowLeft(column, len), true);
}

void SemanticAnalyzer::Expected(const std::string str, std::size_t line,
                                std::size_t column) {

  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << context.source_code_by_line[line - 1] << '\n';
  Color("green", SetArrow(column), true);
}

void SemanticAnalyzer::Unexpected(const std::string str, std::size_t line,
                                  std::size_t column, std::size_t times) {

  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << context.source_code_by_line[line - 1] << '\n';
  if (!times) {
    /*Color("red",
          SetArrow(column - GetCurrentToken().GetValue().length() + 1,
                   GetCurrentToken().GetValue().length()),
          true);*/
  } else {
    Color("red", SetArrow(column - times + 1, times), true);
  }
}

void SemanticAnalyzer::analyze() {
  // Create a Global Scope
  current_scope = std::make_unique<Scope>(nullptr, ScopeType::GLOBAL);

  if (!current_scope) {
    std::cout << "This is nullptr.\n";
  }

  for (const auto &elt : stmt_ast) {
    elt->Accept(*this);
  }
}

void SemanticAnalyzer::Visit(VariableDeclarationAST &ast) {
  auto result = current_scope->FindSymbolInCurrentScope(ast.GetVarName());
  if (result.has_value()) {
    Error("Variable " + ast.GetVarName() +
              " already declared in the current scope",
          ast.GetVarLocation().GetLine(), ast.GetVarLocation().GetColumn());
  } else {
    current_scope->AddSymbol(ast.GetVarName(), ast.GetType());
  }
}

void SemanticAnalyzer::Visit(NumberAST &ast) {
  std::string num = ast.GetNumber();

  if (ast.HasDecimal()) {
    double value;
#ifdef __EMSCRIPTEN__
    char *ptr{};
    auto result = strtod(num.data(), &ptr);
    if (errno == ERANGE) {
      Error(num + "  is not within the f64 range",
            ast.GetSourceLocation().front().GetLine(),
            ast.GetSourceLocation().front().GetColumn(), num.length());

      return;
    }
#endif

#ifndef __EMSCRIPTEN__
    // Convert to double
    auto result = std::from_chars(num.data(), num.data() + num.size(), value);

    if (result.ec == std::errc::result_out_of_range) {
      Error(num + "  is not within the f64 range",
            ast.GetSourceLocation().front().GetLine(),
            ast.GetSourceLocation().front().GetColumn(), num.length());

      return;
    }
#endif
    // Set to f64 by default
    ast.SetType("f64");
    ast.SetValue(value);
  } else {
    long long value;

    auto result = std::from_chars(num.data(), num.data() + num.size(), value);

    if (result.ec == std::errc::result_out_of_range) {
      // Out of range error

      return;
    }
    // Set to i64 by default
    ast.SetType("i64");
    ast.SetValue(value);
  }
}

void SemanticAnalyzer::Visit(IdentifierAST &ast) {
  auto result = current_scope->FindSymbol(ast.GetName());

  if (!result.has_value()) {
    Error("No such variable " + ast.GetName() + " in the current scope",
          ast.GetSourceLocation().front().GetLine(),
          ast.GetSourceLocation().front().GetColumn());
  } else {
    // Annotate the node with type information
    ast.SetType(result->get().GetType());
  }
}

void SemanticAnalyzer::Visit(BinaryExpressionAST &ast) {
  // Check type of the operands
  auto &left_operand = ast.GetLeftOperand().get();
  auto &right_operand = ast.GetRightOperand().get();

  // Visit the left operand
  left_operand.Accept(*this);

  // Visit the right operand
  right_operand.Accept(*this);

  if (left_operand.GetType() != right_operand.GetType()) {
    Error("The operation { " + left_operand.GetType() + " } " +
              ast.GetOperator() + " { " + right_operand.GetType() +
              " } is not allowed",
          // Fix me: this is just a quick fix
          ast.GetSourceLocation()[1].GetLine(),
          ast.GetSourceLocation()[1].GetColumn());
  } else {
    // We are sure that all the operands are of the same type
    ast.SetType(left_operand.GetType());
  }
}

void SemanticAnalyzer::Visit(VariableAssignmentAST &ast) {
  std::string var_name = ast.GetVarName();
  auto result = current_scope->FindSymbolInCurrentScope(var_name);
  if (!result.has_value()) {
    Error("No variable: " + var_name + "  in the current scope",
          ast.GetSourceLocation()[0].GetLine(),
          ast.GetSourceLocation()[0].GetColumn(), var_name.length());
    return;
  } else {
    auto &assigned_expr = ast.GetExpr();
    // Visit the expression
    assigned_expr.Accept(*this);

    auto var_type = result.value().get().GetType();

    if (var_type.empty()) {
      // Update the symbol table
      current_scope->UpdateSymbolTable(var_name, assigned_expr.GetType());
      return;
    }

    if (assigned_expr.GetType() != var_type) {
      // Error: Type mismatch
      std::cout << "Type Mismatch\n";
    }
  }
}

void SemanticAnalyzer::Visit(VariableDeclareAndAssignAST &ast) {
  std::string var_name = ast.GetVarName();
  auto result = current_scope->FindSymbolInCurrentScope(var_name);
  if (result.has_value()) {
    Error("Variable " + var_name + " already declared in the current scope",
          ast.GetSourceLocation()[0].GetLine(),
          ast.GetSourceLocation()[0].GetColumn(), var_name.length());
    return;
  } else {
    auto &expr = ast.GetExpr();
    expr.Accept(*this);

    std::string var_type = ast.GetType();
    std::string expr_type = expr.GetType();

    if (var_type.empty()) {
      ast.SetType(expr_type);
      current_scope->AddSymbol(var_name, expr_type);
    } else {
      if (var_type != expr_type) {
        Error("Type Mimatch: " + expr_type +
                  " cannot be assigned to a variable of " + var_type,
              ast.GetSourceLocation()[0].GetLine(),
              ast.GetSourceLocation()[0].GetColumn(), var_name.length());
      } else {
        current_scope->AddSymbol(var_name, expr_type);
      }
    }
  }
}

void SemanticAnalyzer::Visit(ForLoopAST &ast) {
  if (current_scope->GetType() != ScopeType::FUNCTION) {
    std::cout << "A for loop must be inside a function definition\n";
    return;
  }
  // Create a new scope
  auto for_scope =
      std::make_unique<Scope>(std::move(current_scope), ScopeType::LOOP);

  current_scope = std::move(for_scope);
  // Add the iteration variable to the current scope
  current_scope->AddSymbol(ast.GetIterationVariableName(), "i64");

  auto &range = ast.GetRange();
  // Visit the range node
  range.Accept(*this);

  // Check the type of start and end expr in range
  if (range.GetStart().get().GetType() != range.GetEnd().get().GetType()) {
    // Todo: Report better error
    std::cout << "Types in range expr doesn't match";
    return;
  }

  // Visit the loop body
  for (auto &elt : ast.GetLoopBody()) {
    elt->Accept(*this);
  }

  // After the loop body is completed
  // Switch back to the parent scope
  current_scope = current_scope->GetParent();
}

void SemanticAnalyzer::Visit(LoopAST &ast) {
  /*
    STEP: 1. Check for global scope
          2. Create a new scope
          3. Visit the loop body
  */
}

void SemanticAnalyzer::Visit(WhileLoopAST &ast) {
  /*
    STEP: 1. Check for global scope
          2. Create a new scope
          3. Visit the loop condition
          4. Visit the loop body
  */
}

void SemanticAnalyzer::Visit(IfStatementAST &ast) {}
void SemanticAnalyzer::Visit(ElseIfStatementAST &ast) {}
void SemanticAnalyzer::Visit(ElseStatementAST &ast) {}
void SemanticAnalyzer::Visit(MatchStatementAST &ast) {}
void SemanticAnalyzer::Visit(MatchArmAST &ast) {}
void SemanticAnalyzer::Visit(FunctionCallAST &ast) {}
void SemanticAnalyzer::Visit(FunctionCallExprAST &ast) {}
void SemanticAnalyzer::Visit(FunctionDefinitionAST &ast) {

  if (current_scope->GetType() != ScopeType::GLOBAL) {
    std::cout << "A function definition must be within a global scope.\n";
    return;
  }
  // Create a new scope
  auto fn_scope =
      std::make_unique<Scope>(std::move(current_scope), ScopeType::FUNCTION);

  current_scope = std::move(fn_scope);

  std::cout << "Visiting fn defn node\n";

  auto args = ast.GetFunctionArguments();

  FunctionInfo fn_info;

  if (!args.has_value())
    fn_info = FunctionInfo{{}, ast.GetFunctionReturnType()};
  else
    fn_info = FunctionInfo(ast.GetFunctionArguments().value().GetArgs());

  auto val = FindSymbolTable(ast.GetFunctionName());

  if (val.has_value()) {

    if (val.value() == fn_info) {
      // TODO: better error message
      std::cout << ast.GetFunctionName() + " is already defined\n";
      return;
    }
  }
  // Add the function to the function's symbol table
  AddFunctionToSymbolTable(ast.GetFunctionName(), fn_info);
  // Visit the function body
  for (auto &elt : ast.GetFunctionBody()) {
    elt->Accept(*this);
  }

  // After the loop body is completed
  // Switch back to the parent scope
  if (!current_scope) {
    std::cout << "nullptr\n";
  }
  current_scope = current_scope->GetParent();
}
void SemanticAnalyzer::Visit(FunctionArgumentAST &ast) {}
void SemanticAnalyzer::Visit(BreakStatementAST &ast) {}
void SemanticAnalyzer::Visit(RangeAST &ast) {
  auto start = ast.GetStart();
  auto end = ast.GetEnd();

  start.get().Accept(*this);
  end.get().Accept(*this);
}

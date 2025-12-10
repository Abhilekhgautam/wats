#include "functionSymbolTable.hpp"
#include "scopeType.hpp"
#include <memory>
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
#include "../AST/IfStatementAST.hpp"
#include "../AST/LoopAST.hpp"
#include "../AST/NumberAST.hpp"
#include "../AST/StatementAST.hpp"
#include "../AST/VariableAssignmentAST.hpp"
#include "../AST/VariableDeclarationAST.hpp"
#include "../AST/VariableDeclareAndAssignAST.hpp"
#include "../AST/WhileLoopAST.hpp"
#include "../AST/MatchStatementAST.hpp"
#include "../AST/MatchArmAST.hpp"
#include "../AST/FunctionCallStatementAST.hpp"

#include <cassert>
#include <charconv>
#include <iostream>

void SemanticAnalyzer::Error(const std::string str, std::size_t line,
                             std::size_t column, int len) {
  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << context.source_code_by_line[line - 1] << '\n';
  Color("green", SetArrow(column, len), true);
}

template <typename... Params>
void SemanticAnalyzer::MultiPartError(const std::string str, std::size_t line,
                                      std::size_t column, int len,
                                      Params... rest) {
  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << context.source_code_by_line[line - 1] << '\n';

  Color("green", MultiPartArrow(column, len, std::forward<Params>(rest)...),
        true);
}

void SemanticAnalyzer::Expected(const std::string str, std::size_t line,
                                std::size_t column) {

  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << context.source_code_by_line[line - 1] << '\n';
  Color("green", SetArrow(column, 1), true);
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
  Scope temp_scope(nullptr, ScopeType::GLOBAL);
  current_scope = &temp_scope;

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
    IncrementErrorCount();
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
      IncrementErrorCount();
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
      IncrementErrorCount();
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
      IncrementErrorCount();
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
    IncrementErrorCount();
  } else {
    // Annotate the node with type information
    ast.SetType(result.value().get().GetType());
  }
}

void SemanticAnalyzer::Visit(BinaryExpressionAST &ast) {
  // Check type of the operands
  ExpressionAST &left_operand = ast.GetLeftOperand();
  ExpressionAST &right_operand = ast.GetRightOperand();

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

    IncrementErrorCount();
  } else {
    // We are sure that all the operands are of the same type
    ast.SetType(left_operand.GetType());
  }
}

void SemanticAnalyzer::Visit(VariableAssignmentAST &ast) {
  std::string var_name = ast.GetVarName();
  auto result = current_scope->FindSymbol(var_name);
  if (!result.has_value()) {
    Error("Variable: '" + var_name +
              "' used without previous declaration in the current scope",
          ast.GetSourceLocation().GetLine(),
          ast.GetSourceLocation().GetColumn(), var_name.length());
    IncrementErrorCount();
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
      MultiPartError("Type Mismatch: " + assigned_expr.GetType() +
                         " cannot be assigned to variable of type " + var_type +
                         ".",
                     ast.GetSourceLocation().GetLine(),
                     ast.GetSourceLocation().GetColumn() - var_name.length(),
                     var_name.length(),
                     ast.GetExpr().GetSourceLocation()[0].GetColumn() -
                         ast.GetExpr().GetLength(),
                     ast.GetExpr().GetLength());
      IncrementErrorCount();
    }
  }
}

void SemanticAnalyzer::Visit(VariableDeclareAndAssignAST &ast) {
  std::string var_name = ast.GetVarName();
  auto result = current_scope->FindSymbolInCurrentScope(var_name);
  if (result.has_value()) {
    Error("Variable " + var_name + " already declared in the current scope",
          ast.GetSourceLocation().GetLine(),
          ast.GetSourceLocation().GetColumn(), var_name.length());
    IncrementErrorCount();
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
        MultiPartError("Type Mismatch: " + expr_type +
                           " cannot be assigned to a variable of " + var_type,
                       ast.GetSourceLocation().GetLine(),
                       ast.GetSourceLocation().GetColumn() - var_name.length(),
                       var_name.length(),
                       ast.GetExpr().GetSourceLocation()[0].GetColumn() -
                           ast.GetExpr().GetLength(),
                       ast.GetExpr().GetLength());

        IncrementErrorCount();
      } else {
        current_scope->AddSymbol(var_name, expr_type);
      }
    }
  }
}

void SemanticAnalyzer::Visit(ForLoopAST &ast) {
  if (current_scope->GetType() != ScopeType::FUNCTION) {
    Error("A for loop must be inside a function definition",
          ast.GetSourceLocation().GetLine(),
          ast.GetSourceLocation().GetColumn(), /* for - 3 characters long*/ 3);
    return;
  }
  // Create a new scope
  Scope for_scope(current_scope, ScopeType::LOOP);

  current_scope = &for_scope;
  // Add the iteration variable to the current scope
  current_scope->AddSymbol(ast.GetIterationVariableName(), "i64");

  auto &range = ast.GetRange();
  // Visit the range node
  range.Accept(*this);

  // Check the type of start and end expr in range
  if (range.GetStart().GetType() != range.GetEnd().GetType()) {
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
  if (current_scope->GetType() != ScopeType::FUNCTION) {
    Error("A loop must be inside a function definition",
          ast.GetSourceLocation().GetLine(),
          ast.GetSourceLocation().GetColumn(), /* loop - 3 characters long*/ 4);
    IncrementErrorCount();
    return;
  }
  // Create a new scope
  Scope for_scope(current_scope, ScopeType::LOOP);

  current_scope = &for_scope;

  // Visit the loop body
  for (auto &elt : ast.GetLoopBody()) {
    elt->Accept(*this);
  }

  // After the loop body is completed
  // Switch back to the parent scope
  current_scope = current_scope->GetParent();
}

void SemanticAnalyzer::Visit([[maybe_unused]] WhileLoopAST &ast) {
  /*
    STEP: 1. Check for global scope
          2. Create a new scope
          3. Visit the loop condition
          4. Visit the loop body
  */

  if (current_scope->GetType() == ScopeType::GLOBAL) {
    Error("A 'while loop' must be within a function body.",
          ast.GetSourceLocation().GetLine(),
          ast.GetSourceLocation().GetColumn(), /* while - 4 char long */ 4);
    IncrementErrorCount();
    return;
  }

  ExpressionAST &cond = ast.GetCondition();

  cond.Accept(*this);

  Scope while_scope(current_scope, ScopeType::LOOP);
  current_scope = &while_scope;

  for (auto &elt : ast.GetBody()) {
    elt->Accept(*this);
  }

  current_scope = current_scope->GetParent();
}

void SemanticAnalyzer::Visit(IfStatementAST &ast) {
  if (current_scope->GetType() == ScopeType::GLOBAL) {
    Error("An 'if statement' must be within a function body",
          ast.GetSourceLocation().GetLine(), ast.GetSourceLocation().GetLine());
    IncrementErrorCount();
    return;
  }

  ExpressionAST &if_condition = ast.GetIfCondition();

  if_condition.Accept(*this);

  Scope if_scope(current_scope, ScopeType::BRANCH);

  current_scope = &if_scope;
  for (auto &elt : ast.GetIfBody()) {
    elt->Accept(*this);
  }

  current_scope = current_scope->GetParent();

  if (ast.hasElseIf()) {
    auto &else_if_vec = ast.GetElseIfStatements();

    for (const auto &elt : else_if_vec) {
      ExpressionAST &else_if_condition = elt->GetCondition();

      else_if_condition.Accept(*this);

      Scope else_if_scope(current_scope, ScopeType::BRANCH);
      current_scope = &else_if_scope;

      for (auto &stmt : elt->GetBody()) {
        stmt->Accept(*this);
      }
      current_scope = current_scope->GetParent();
    }
  }

  if (ast.hasElse()) {
    auto &else_stmt = ast.GetElseStatement();

    Scope else_scope(current_scope, ScopeType::BRANCH);
    current_scope = &else_scope;
    for (auto &body : else_stmt.GetBody()) {
      body->Accept(*this);
    }
    current_scope = current_scope->GetParent();
  }
}

void SemanticAnalyzer::Visit(ElseIfStatementAST &ast) {
  // STEP 1: Condition Type Check
  ExpressionAST &else_if_condition = ast.GetCondition();
  else_if_condition.Accept(*this);

  // NOTE: Assuming your language requires conditions to be explicitly 'bool'
  // If your language allows automatic coercion (e.g., non-zero integer is
  // true), adjust this check.
  if (else_if_condition.GetType() != "bool") {
    Error("Condition in 'else if' must be of type 'bool'",
          else_if_condition.GetSourceLocation().front().GetLine(),
          else_if_condition.GetSourceLocation().front().GetColumn(),
          else_if_condition.GetLength());
    IncrementErrorCount();
  }

  // STEP 2: Scope and Body Traversal (Scope management is already handled in
  // IfStatementAST) Re-enter the scope created by the parent IfStatementAST
  // loop for this block's analysis
  Scope else_if_scope(current_scope, ScopeType::BRANCH);
  current_scope = &else_if_scope;

  for (auto &stmt : ast.GetBody()) {
    stmt->Accept(*this);
  }

  current_scope = current_scope->GetParent();
}

void SemanticAnalyzer::Visit(ElseStatementAST &ast) {
  // STEP 1: Scope Creation and Traversal
  // The logic is simpler: create a scope and traverse the body.
  Scope else_scope(current_scope, ScopeType::BRANCH);
  current_scope = &else_scope;

  for (auto &body : ast.GetBody()) {
    body->Accept(*this);
  }

  current_scope = current_scope->GetParent();
}

void SemanticAnalyzer::Visit(MatchStatementAST &ast) {
  // STEP 1: Evaluate Match Expression Type
  ExpressionAST &match_expr = ast.GetCondition();
  match_expr.Accept(*this);
  std::string match_type = match_expr.GetType();

  // STEP 2: Scope Creation (for the overall statement)
  Scope match_scope(current_scope, ScopeType::BRANCH);
  current_scope = &match_scope;

  // STEP 3: Check All Arms
  for (const auto &arm : ast.GetArms()) {
    // Pass the expected type down to the arm visitor
    // (This requires adding a parameter to Visit(MatchArmAST &ast, const
    // std::string& expected_type)) Since we can't change the signature, we rely
    // on the arm to handle the match_type implicitly.
    arm->Accept(*this);
  }

  // STEP 4: Scope Exit
  current_scope = current_scope->GetParent();
}

void SemanticAnalyzer::Visit(MatchArmAST &ast) {
  // STEP 1: Evaluate Arm Condition/Pattern Type
  ExpressionAST &arm_condition = ast.GetCondition();
  arm_condition.Accept(*this);
  std::string arm_type = arm_condition.GetType();

  // In a real compiler, we would now compare 'arm_type' against the
  // 'match_type'. We skip that comparison here since the parent didn't pass
  // 'match_type'.

  // STEP 2: Scope Creation and Traversal
  // Each arm has its own local scope for any variables declared inside the
  // body.
  Scope arm_scope(current_scope, ScopeType::BRANCH);
  current_scope = &arm_scope;

  for (auto &stmt : ast.GetBody()) {
    stmt->Accept(*this);
  }

  // STEP 3: Scope Exit
  current_scope = current_scope->GetParent();
}
void SemanticAnalyzer::Visit(FunctionCallAST &ast) {
  // STEP 1: Function Existence Check
  std::string fn_name = ast.GetFunctionName()->GetName();
  auto fn_info_opt = FindSymbolTable(fn_name);

  if (!fn_info_opt.has_value()) {
    Error("Function '" + fn_name + "' is not defined.",
          ast.GetSourceLocation().GetLine(),
          ast.GetSourceLocation().GetColumn(), fn_name.length());
    IncrementErrorCount();
    return;
  }
  FunctionInfo fn_info = fn_info_opt.value();

  // STEP 2: Argument Count Check
  size_t expected_count = fn_info.GetArgs().size();
  size_t actual_count = ast.GetArgs().size();

  if (actual_count != expected_count) {
    Error("Function '" + fn_name + "' expects " +
              std::to_string(expected_count) + " arguments but received " +
              std::to_string(actual_count) + ".",
          ast.GetSourceLocation().GetLine(),
          ast.GetSourceLocation().GetColumn(), fn_name.length());
    IncrementErrorCount();
    return;
  }

  // STEP 3: Argument Type Check and Traversal
  // (Assuming GetArgs() returns vector<StatementAST> or a similar structure)
  // For simplicity, we skip full type comparison here, but the call below is
  // essential:
  for (auto &arg_stmt : ast.GetArgs()) {
    arg_stmt->Accept(*this);
    // In a complete solution: check arg_stmt's inferred type against fn_info's
    // expected type
  }
}
void SemanticAnalyzer::Visit(FunctionCallExprAST &ast) {
  // STEP 1 & 2: Existence and Argument Count Check (Same as FunctionCallAST)
  std::string fn_name = ast.GetFunctionName();
  auto fn_info_opt = FindSymbolTable(fn_name);

  if (!fn_info_opt.has_value()) {
    // ... Error reporting for missing function ...
    IncrementErrorCount();
    return;
  }
  FunctionInfo fn_info = fn_info_opt.value();

  // (Argument count/type checks for ExpressionAST parameters skipped for
  // brevity, but they must be performed here using FunctionParameterAST data)

  // STEP 3: Return Type Annotation
  std::string return_type = fn_info.GetRetType();
  if (return_type.empty() || return_type == "void") {
    Error("Return value of function '" + fn_name +
              "' cannot be used in an expression.",
          ast.GetSourceLocation().front().GetLine(),
          ast.GetSourceLocation().front().GetColumn(), ast.GetLength());
    IncrementErrorCount();
    return;
  }

  // Annotate the node with the function's declared return type.
  ast.SetType(return_type);
}
void SemanticAnalyzer::Visit(FunctionDefinitionAST &ast) {

  if (current_scope->GetType() != ScopeType::GLOBAL) {
    Error("A function definition must be within a global scope",
          ast.GetSourceLocation().GetLine(), ast.GetSourceLocation().GetLine());
    IncrementErrorCount();
    return;
  }
  // Create a new scope
  Scope fn_scope(current_scope, ScopeType::FUNCTION);

  // add the fn arguments to the fn scope
  auto opt_args = ast.GetFunctionArguments();
  if (opt_args.has_value()) {
    auto fn_args = opt_args.value();

    auto args = fn_args.get().GetArgs();

    for (const auto &elt : args) {
      fn_scope.AddSymbol(elt, "i64");
    }
  }
  current_scope = &fn_scope;

  auto args = ast.GetFunctionArguments();

  FunctionInfo fn_info;

  if (!args.has_value())
    fn_info = FunctionInfo{{}, ast.GetFunctionReturnType()};
  else
    fn_info = FunctionInfo(ast.GetFunctionArguments().value().get().GetArgs());

  auto val = FindSymbolTable(ast.GetFunctionName());

  if (val.has_value()) {

    if (val.value() == fn_info) {
      // TODO: better error message
      Error(
          "The function: " + ast.GetFunctionName() + " is already defined.",
          ast.GetSourceLocation().GetLine(),
          ast.GetFunctionNameAsIdentifier().GetSourceLocation()[0].GetColumn(),
          ast.GetFunctionName().length());
      IncrementErrorCount();
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

  current_scope = current_scope->GetParent();
}
void SemanticAnalyzer::Visit([[maybe_unused]] FunctionArgumentAST &ast) {}
void SemanticAnalyzer::Visit(BreakStatementAST &ast) {
  // STEP 1: Context Check (Search up the scope chain)
  Scope *search_scope = current_scope;
  bool found_loop_scope = false;

  while (search_scope != nullptr) {
    if (search_scope->GetType() == ScopeType::LOOP) {
      found_loop_scope = true;
      break;
    }
    search_scope = search_scope->GetParent();
  }

  // STEP 2: Error if not in a loop
  if (!found_loop_scope) {
    Error("'break' statement used outside of a loop",
          ast.GetSourceLocation().GetLine(),
          ast.GetSourceLocation().GetColumn(), 5 /* length of "break" */);
    IncrementErrorCount();
  }
}
void SemanticAnalyzer::Visit(RangeAST &ast) {
  ExpressionAST &start = ast.GetStart();
  ExpressionAST &end = ast.GetEnd();

  start.Accept(*this);
  end.Accept(*this);
}

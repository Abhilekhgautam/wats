#include <memory>
#include "functionSymbolTable.hpp"
#include "scopeType.hpp"

#include "../AST/MatchStatementAST.hpp"
#include "../AST/ReturnStatementAST.h"
#ifdef __EMSCRIPTEN__
#include <cerrno>
#endif

#include "scope.hpp"
#include "semanticAnalyzer.hpp"

#include "../AST/StringExpressionAST.hpp"
#include "../AST/BinaryExpressionAST.hpp"
#include "../AST/ForLoopAST.hpp"
#include "../AST/FunctionCallExprAST.hpp"
#include "../AST/FunctionDefinitionAST.hpp"
#include "../AST/IdentifierAST.hpp"
#include "../AST/IfStatementAST.hpp"
#include "../AST/LoopAST.hpp"
#include "../AST/MatchArmAST.hpp"
#include "../AST/NumberAST.hpp"
#include "../AST/StatementAST.hpp"
#include "../AST/VariableAssignmentAST.hpp"
#include "../AST/VariableDeclarationAST.hpp"
#include "../AST/VariableDeclareAndAssignAST.hpp"
#include "../AST/WhileLoopAST.hpp"

#include <cassert>
#include <charconv>
#include <format>
#include <iostream>

void SemanticAnalyzer::Error(const std::string &str, const std::size_t line, const std::size_t column, const int len) {
    std::cout << "[ " << line << ":" << column << " ] ";
    Color("red", "Error: ");
    Color("blue", str, true);

    std::cout << context.source_code_by_line[line - 1] << '\n';
    Color("green", SetArrow(column, len), true);
}

template<typename... Params>
void SemanticAnalyzer::MultiPartError(const std::string &str, const std::size_t line, const std::size_t column,
                                      const int len, Params... rest) {
    std::cout << "[ " << line << ":" << column << " ] ";
    Color("red", "Error: ");
    Color("blue", str, true);

    std::cout << context.source_code_by_line[line - 1] << '\n';

    Color("green", MultiPartArrow(column, len, std::forward<Params>(rest)...), true);
}

void SemanticAnalyzer::Expected(const std::string &str, const std::size_t line, const std::size_t column) {

    std::cout << "[ " << line << ":" << column << " ] ";
    Color("red", "Error: ");
    Color("blue", str, true);

    std::cout << context.source_code_by_line[line - 1] << '\n';
    Color("green", SetArrow(column, 1), true);
}

void SemanticAnalyzer::Unexpected(const std::string &str, const std::size_t line, const std::size_t column,
                                  const std::size_t times) {

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

    for (const auto &elt: stmt_ast) {
        elt->Accept(*this);
    }
}

Type SemanticAnalyzer::infer(ExpressionAST& ast){
    ast.Accept(*this);

    // TODO: check for values existence.
    return inferEngine.resolve(ast.type.value());
}

void SemanticAnalyzer::Visit(VariableDeclarationAST &ast) {
    auto result = current_scope->FindSymbolInCurrentScope(ast.variable_name);
    if (result.has_value()) {
        Error("Variable " + ast.variable_name + " already declared in the current scope", ast.GetVarLocation().GetLine(),
              ast.GetVarLocation().GetColumn());
        IncrementErrorCount();
    } else {
        auto inferred_type = inferEngine.freshVar();
        current_scope->AddSymbol(ast.variable_name, inferred_type);
    }
}

void SemanticAnalyzer::Visit(StringExpressionAST& ast){
   const std::string str = ast.value;

   ast.type = TypeKind::String;
}

void SemanticAnalyzer::Visit(NumberAST &ast) {
    const std::string num = ast.num;

    if (ast.has_decimal) {
        double value;
#ifdef __EMSCRIPTEN__
        char *ptr{};
        auto result = strtod(num.data(), &ptr);
        if (errno == ERANGE) {
            Error(num + "  is not within the f64 range", ast.GetSourceLocation().front().GetLine(),
                  ast.GetSourceLocation().front().GetColumn(), num.length());
            IncrementErrorCount();
            return;
        }
#endif

#ifndef __EMSCRIPTEN__
        // Convert to double
        auto result = std::from_chars(num.data(), num.data() + num.size(), value);

        if (result.ec == std::errc::result_out_of_range) {
            Error(num + "  is not within the f64 range", ast.GetSourceLocation().front().GetLine(),
                  ast.GetSourceLocation().front().GetColumn(), num.length());
            IncrementErrorCount();
            return;
        }
#endif
        // later decide the bitwidth based on the usage.
        ast.type = TypeKind::F64;
        ast.value = value;
    } else {
        long long value;

        auto result = std::from_chars(num.data(), num.data() + num.size(), value);

        if (result.ec == std::errc::result_out_of_range) {
            // Out of range error
            IncrementErrorCount();
            return;
        }
        // decide the bit-width later based on the usage.
        ast.type = TypeKind::I64;
        ast.value = value;
    }
}


void SemanticAnalyzer::Visit(IdentifierAST &ast) {
    auto result = current_scope->FindSymbol(ast.name);

    if (!result.has_value()) {
        Error("No such variable " + ast.name + " in the current scope", ast.GetSourceLocation().front().GetLine(),
              ast.GetSourceLocation().front().GetColumn());
        IncrementErrorCount();
    } else {
        // Annotate the node with type information
        ast.type = inferEngine.resolve(result.value());
    }
}

void SemanticAnalyzer::Visit(BinaryExpressionAST &ast) {
    // TODO: Check for null on lhs and rhs expr

    // Check type of the operands
    ExpressionAST &left_operand = *ast.expr_lhs;
    ExpressionAST &right_operand = *ast.expr_rhs;

    // Visit the left operand
    left_operand.Accept(*this);

    // Visit the right operand
    right_operand.Accept(*this);

    auto errorVal = inferEngine.unify(left_operand.type.value(), right_operand.type.value());

    if (errorVal == UnifyError::Mismatch){
        Error("The operation { " + left_operand.type.value().to_string() + " } " + ast.op.GetOperatorSymbol() + " { " +
                      right_operand.type.value().to_string() + " } is not allowed",
              // Fix me: this is just a quick fix
              1, 1);

        IncrementErrorCount();

    }

    ast.type = inferEngine.resolve(left_operand.type.value());

    // Check for invald op in type.
    if (left_operand.type.value() == TypeKind::String && ast.op.GetOperatorSymbol() != "+"){
        auto err_msg = std::format("The operator {} is not supported for strings", ast.op.GetOperatorSymbol());
        Error(err_msg,
              // Fix me: this is just a quick fix
              1, 1);

        IncrementErrorCount();

    }
 }

void SemanticAnalyzer::Visit(VariableAssignmentAST &ast) {
    const std::string var_name = ast.variable_name.name;
    if (auto result = current_scope->FindSymbol(var_name); !result.has_value()) {
        Error("Variable: '" + var_name + "' used without previous declaration in the current scope",
              ast.GetSourceLocation().GetLine(), ast.GetSourceLocation().GetColumn(), var_name.length());
        IncrementErrorCount();
    } else {
        auto &assigned_expr = *ast.expr;
        // Visit the expression
        assigned_expr.Accept(*this);

        Type var_type = result.value();

        if (!result.has_value()) {
            // Update the symbol table
            current_scope->UpdateSymbolTable(var_name, inferEngine.resolve(assigned_expr.type.value()));
            return;
        }
        else {
            auto inferred_type = inferEngine.resolve(assigned_expr.type.value());
            // Type mismatch.
            auto errorVal = inferEngine.unify(result.value(), inferred_type);

            if (errorVal == UnifyError::Mismatch){
                MultiPartError("Type Mismatch: " + assigned_expr.type.value().to_string() + " cannot be assigned to variable of type " +
                                   var_type.to_string() + ".",
                           ast.GetSourceLocation().GetLine(), ast.GetSourceLocation().GetColumn() - var_name.length(),
                           var_name.length(),
                           ast.expr->GetSourceLocation()[0].GetColumn() - ast.expr->GetLength(),
                           ast.expr->GetLength());
            IncrementErrorCount();

                return;
            }
            
            current_scope->UpdateSymbolTable(var_name, inferred_type);
            return;

        }
      }
}

void SemanticAnalyzer::Visit(VariableDeclareAndAssignAST &ast) {
    const std::string var_name = ast.variable_name;
    if (const auto result = current_scope->FindSymbolInCurrentScope(var_name); result.has_value()) {
        Error("Variable " + var_name + " already declared in the current scope", ast.GetSourceLocation().GetLine(),
              ast.GetSourceLocation().GetColumn(), var_name.length());
        IncrementErrorCount();
    } else {
        auto &expr = *ast.expr;
        expr.Accept(*this);

        Type var_type = inferEngine.freshVar(); 
        Type expr_type = infer(expr);

        auto errorVal = inferEngine.unify(var_type, expr_type);

        if (errorVal == UnifyError::Mismatch){
            MultiPartError("Type Mismatch: " + expr_type.to_string() + " cannot be assigned to a variable of " + var_type.to_string(),
                                   ast.GetSourceLocation().GetLine(),
                                   ast.GetSourceLocation().GetColumn() - var_name.length(), var_name.length(),
                                   ast.expr->GetSourceLocation()[0].GetColumn() - ast.expr->GetLength(),
                                   ast.expr->GetLength());

            IncrementErrorCount();
        }

        current_scope->AddSymbol(var_name, inferEngine.resolve(var_type));
     }
}

void SemanticAnalyzer::Visit(ForLoopAST &ast) {
    if (current_scope->GetType() != ScopeType::FUNCTION) {
        Error("A for loop must be inside a function definition", ast.GetSourceLocation().GetLine(),
              ast.GetSourceLocation().GetColumn(), /* for - 3 characters long*/ 3);
        return;
    }
    // Create a new scope
    Scope for_scope(current_scope, ScopeType::LOOP);

    current_scope = &for_scope;
    // Add the iteration variable to the current scope
    current_scope->AddSymbol(ast.GetIterationVariableName(), TypeKind::I64);

    auto &range = *ast.range;
    // Visit the range node
    range.Accept(*this);

    // TODO: Check for the range type...
    //  Check the type of start and end expr in range
    //  if (range.GetStart().GetType() != range.GetEnd().GetType()) {
    //    // Todo: Report better error
    //    std::cout << "Types in range expr doesn't match";
    //    return;
    //  }

    // Visit the loop body
    for (auto &elt: ast.loop_body) {
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
        Error("A loop must be inside a function definition", ast.GetSourceLocation().GetLine(),
              ast.GetSourceLocation().GetColumn(), /* loop - 3 characters long*/ 4);
        IncrementErrorCount();
        return;
    }
    // Create a new scope
    Scope for_scope(current_scope, ScopeType::LOOP);

    current_scope = &for_scope;

    // Visit the loop body
    for (auto &elt: ast.loop_body) {
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
        Error("A 'while loop' must be within a function body.", ast.GetSourceLocation().GetLine(),
              ast.GetSourceLocation().GetColumn(), /* while - 4 char long */ 4);
        IncrementErrorCount();
        return;
    }

    auto &cond = *ast.condition;

    cond.Accept(*this);

    Scope while_scope(current_scope, ScopeType::LOOP);
    current_scope = &while_scope;

    for (auto &elt: ast.GetBody()) {
        elt->Accept(*this);
    }

    current_scope = current_scope->GetParent();
}

void SemanticAnalyzer::Visit(IfStatementAST &ast) {
    if (current_scope->GetType() == ScopeType::GLOBAL) {
        Error("An 'if statement' must be within a function body", ast.GetSourceLocation().GetLine(),
              ast.GetSourceLocation().GetLine());
        IncrementErrorCount();
        return;
    }

    auto &if_condition = *ast.condition;

    if_condition.Accept(*this);

    Scope if_scope(current_scope, ScopeType::BRANCH);

    current_scope = &if_scope;
    for (auto &elt: ast.if_body) {
        elt->Accept(*this);
    }

    current_scope = current_scope->GetParent();

    if (ast.hasElseIf()) {
        auto &else_if_vec = ast.else_if_ast;

        for (const auto &elt: else_if_vec) {
            ExpressionAST &else_if_condition = *elt->condition;

            else_if_condition.Accept(*this);

            Scope else_if_scope(current_scope, ScopeType::BRANCH);
            current_scope = &else_if_scope;

            for (auto &stmt: elt->else_if_body) {
                stmt->Accept(*this);
            }
            current_scope = current_scope->GetParent();
        }
    }

    if (ast.hasElse()) {
        auto &else_stmt = ast.else_ast;

        Scope else_scope(current_scope, ScopeType::BRANCH);
        current_scope = &else_scope;
        for (auto &body: else_stmt->else_body) {
            body->Accept(*this);
        }
        current_scope = current_scope->GetParent();
    }
}
void SemanticAnalyzer::Visit([[maybe_unused]] ElseIfStatementAST &ast) {}
void SemanticAnalyzer::Visit([[maybe_unused]] ElseStatementAST &ast) {}
void SemanticAnalyzer::Visit(MatchStatementAST &ast) {
    for (const auto &elt: ast.arms) {
        elt->Accept(*this);
    }
}
void SemanticAnalyzer::Visit(MatchArmAST &ast) {
    if (current_scope->GetType() == ScopeType::GLOBAL) {
        Error("An 'match statement' must be within a function body", ast.GetSourceLocation().GetLine(),
              ast.GetSourceLocation().GetLine());
        IncrementErrorCount();
        return;
    }

    auto &condition = *ast.cond;

    condition.Accept(*this);

    Scope arm_scope(current_scope, ScopeType::BRANCH);

    current_scope = &arm_scope;
    for (auto &elt: ast.body) {
        elt->Accept(*this);
    }

    current_scope = current_scope->GetParent();
}
void SemanticAnalyzer::Visit([[maybe_unused]] FunctionCallAST &ast) {}
void SemanticAnalyzer::Visit(FunctionCallExprAST &ast) {
    if (current_scope->GetType() != ScopeType::FUNCTION) {
        Error("Cannot call a function outside of a function", ast.GetSourceLocation()[0].GetLine(),
              ast.GetSourceLocation()[0].GetLine());
        IncrementErrorCount();
        return;
    }
    const auto fn_name = ast.fn_name->name;

    if (const auto fn_info = FindSymbolTable(fn_name); fn_info.has_value()) {
        const auto& [params_type, ret_type]  = fn_info.value();

        ast.type =  ret_type;

        auto& args = ast.args;

        const auto param_size = params_type.size();
        const auto args_size = args->GetFunctionArguments().size();

        if (param_size != args_size) {
            const std::string msg = std::format("Function {} expects {} arguments but {} were provided", fn_name, param_size, args_size);
            Error(msg, ast.GetSourceLocation()[0].GetLine(), ast.GetSourceLocation()[0].GetLine());
            IncrementErrorCount();
            return;
        }
        auto count = 0;
        for (const auto& arg: args->GetFunctionArguments()) {
            arg->Accept(*this);
            auto inferred_type = infer(*arg);

            Type arg_type = inferred_type;
            Type param_type = params_type[count++];

            auto unifyError = inferEngine.unify(arg_type, param_type);

            if (unifyError.has_value()){
                if (unifyError == UnifyError::Mismatch){

                    const std::string msg = std::format("Expected argument of type {} but got {} ", param_type.to_string(), arg_type.to_string());
                    Error(msg, arg->GetSourceLocation()[0].GetLine(), arg->GetSourceLocation()[0].GetColumn());
                }
            }
        }
    }
    else {
        const std::string msg = std::format("No function {} defined in current scope.", fn_name);
        Error(msg, ast.GetSourceLocation()[0].GetLine(), ast.GetSourceLocation()[0].GetColumn());
        IncrementErrorCount();
    }
}
void SemanticAnalyzer::Visit(FunctionDefinitionAST &ast) {

    if (current_scope->GetType() != ScopeType::GLOBAL) {
        Error("A function definition must be within a global scope", ast.GetSourceLocation().GetLine(),
              ast.GetSourceLocation().GetLine());
        IncrementErrorCount();
        return;
    }
    // Create a new scope
    Scope fn_scope(current_scope, ScopeType::FUNCTION);

    // add the fn arguments to the fn scope
    auto &opt_args = ast.arguments;
    std::vector<Type> arg_types;

    for (const auto &arg: opt_args) {
        auto annotated_type = typeTable.resolve(arg->annotated_type).value();
        fn_scope.AddSymbol(arg->idName, annotated_type);
        arg_types.push_back(annotated_type);
    }

    current_scope = &fn_scope;

    // auto args = ast.GetFunctionArguments();

    const FunctionInfo fn_info(arg_types, ast.ret_type);


    if (const auto val = FindSymbolTable(ast.GetFunctionName()); val.has_value()) {

        if (val.value() == fn_info) {
            // TODO: better error message
            Error("The function: " + ast.GetFunctionName() + " is already defined.", ast.GetSourceLocation().GetLine(),
                  ast.GetFunctionNameAsIdentifier().GetSourceLocation()[0].GetColumn(),
                  static_cast<int>(ast.GetFunctionName().length()));
            IncrementErrorCount();
            return;
        }
    }
    // Add the function to the function's symbol table
    AddFunctionToSymbolTable(ast.GetFunctionName(), fn_info);
    // Visit the function body
    for (auto &elt: ast.function_body) {
        elt->Accept(*this);
        if (const auto returnAST = dynamic_cast<ReturnStatementAST *>(elt.get())) {
            auto &returnExpr = returnAST->GetReturnExpression();

            auto returnExprType = infer(returnExpr);

            // TODO: handle void 
            auto fnReturnType = typeTable.resolve(ast.annotated_ret_type.value());

            auto unifyError = inferEngine.unify(returnExprType, fnReturnType.value());

            if(unifyError.has_value()){
                if (unifyError == UnifyError::Mismatch){
                    Unexpected(std::format("Cannot return a {} from function whose return type is {}",
                                               returnExprType.to_string(), ast.annotated_ret_type.value()),
                                   returnAST->GetSourceLocation().GetLine(), returnAST->GetSourceLocation().GetColumn(),
                                   1);
                    IncrementErrorCount();
                    return;

                }
            }
        }
    }

    // After the loop body is completed
    // Switch back to the parent scope

    current_scope = current_scope->GetParent();
}
void SemanticAnalyzer::Visit([[maybe_unused]] FunctionArgumentAST &ast) {}
void SemanticAnalyzer::Visit([[maybe_unused]] BreakStatementAST &ast) {}
void SemanticAnalyzer::Visit(RangeAST &ast) {
    ExpressionAST &start = *ast.start;
    ExpressionAST &end = *ast.end;

    start.Accept(*this);
    end.Accept(*this);
}

void SemanticAnalyzer::Visit(ReturnStatementAST &ast) {
    auto &expr = ast.GetReturnExpression();
    expr.Accept(*this);
}

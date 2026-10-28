#include "IRGenerator.hpp"
#include "../AST/StringExpressionAST.hpp"
#include "../AST/BinaryExpressionAST.hpp"
#include "../AST/ExpressionAST.hpp"
#include "../AST/IfStatementAST.hpp"
#include "../AST/LoopAST.hpp"
#include "../AST/NumberAST.hpp"
#include "../AST/ReturnStatementAST.h"
#include "../AST/VariableDeclarationAST.hpp"
#include "../AST/VariableDeclareAndAssignAST.hpp"

#include <format>

#include "../AST/ForLoopAST.hpp"
#include "../AST/FunctionCallExprAST.hpp"
#include "../AST/MatchStatementAST.hpp"

using nlohmann::json;

static std::string GetTemporaryVariableName() {
    static int count = 1;
    return std::format("var_{}", count++);
}

static std::string GetLabelName() {
    static int count = 0;
    return std::format("bb_{}", count++);
}

// Helper for Arithmetic Operations
// ===================================================
//

json GenerateArithmeticOperations(IRGenerator &generator, ExpressionAST &LHS, ExpressionAST &RHS,
                                  const std::string &op) {
    json retInstruction = json::array();

    // auto& lhs_expr = ast.GetLeftOperand();
    const Type lhs_type = LHS.type.value();

    // auto& rhs_expr = ast.GetRightOperand();
    const Type rhs_type = RHS.type.value();

    std::vector<SourceLocation> unknown;

    std::string lhs_name;
    std::string rhs_name;
    if (auto numAST = dynamic_cast<const NumberAST *>(&LHS)) {
        auto tempNumAST = std::make_unique<NumberAST>(numAST->num, numAST->has_decimal,
                                                      numAST->GetSourceLocation().front());
        std::string num = numAST->num;

        tempNumAST->value = numAST->value;
        tempNumAST->type = numAST->type;

        //tempNumAST->SetValue(numAST->GetValue());
        const std::string tempVarName = GetTemporaryVariableName();
        VariableDeclareAndAssignAST tempAST(tempVarName, lhs_type.to_string(), std::move(tempNumAST), unknown);
        tempAST.type_name = lhs_type;

        const json tempASTJSON = generator.Generate(tempAST);
        if (tempASTJSON.is_array()) {
            for (const auto &elt: tempASTJSON) {
                retInstruction.push_back(elt);
            }
        } else {
            retInstruction.push_back(tempASTJSON);
        }
        lhs_name = tempVarName;
    }
    else if (auto callAST = dynamic_cast<const FunctionCallExprAST *>(&LHS)) {
        auto gen_code = generator.Generate(*callAST);

        const std::string tempVarName = GetTemporaryVariableName();
        SourceLocation loc{};

        gen_code[gen_code.size() - 1]["dest"] = tempVarName;
        gen_code[gen_code.size() - 1]["type"] = callAST->type.value().to_string();

        lhs_name = tempVarName;

        for (const auto& elt: gen_code) {
            retInstruction.push_back(elt);
        }
    }
    else {
        json lhs = generator.Generate(LHS);
        if (lhs.is_array()) {
            for (const auto &elt: lhs) {
                lhs_name = elt["dest"];
                retInstruction.push_back(elt);
            }
        } else {
            lhs_name = lhs["args"][0];
        }
    }


    if (auto numAST = dynamic_cast<NumberAST *>(&RHS)) {
        auto tempNumAST = std::make_unique<NumberAST>(numAST->num, numAST->has_decimal,
                                                      numAST->GetSourceLocation().front());
        std::string num = numAST->num;

        tempNumAST->value = numAST->value;
        tempNumAST->type = numAST->type;

        const std::string tempVarName = GetTemporaryVariableName();
        VariableDeclareAndAssignAST tempAST(tempVarName, lhs_type.to_string(), std::move(tempNumAST), unknown);
        tempAST.type_name = lhs_type;
        tempAST.annotated_type = lhs_type.to_string();

        const json tempASTJSON = generator.Generate(tempAST);
        if (tempASTJSON.is_array()) {
            for (const auto &elt: tempASTJSON) {
                retInstruction.push_back(elt);
            }
        } else {
            retInstruction.push_back(tempASTJSON);
        }

        rhs_name = tempVarName;
    }
    else if (auto callAST = dynamic_cast<const FunctionCallExprAST *>(&RHS)) {
        auto gen_code = generator.Generate(*callAST);

        const std::string tempVarName = GetTemporaryVariableName();
        SourceLocation loc{};

        gen_code[gen_code.size() - 1]["dest"] = tempVarName;
        gen_code[gen_code.size() - 1]["type"] = callAST->type.value().to_string();

        rhs_name = tempVarName;

        for (const auto& elt: gen_code) {
            retInstruction.push_back(elt);
        }
    }
    else {
        json rhs = generator.Generate(RHS);
        if (rhs.is_array()) {
            for (const auto &elt: rhs) {
                rhs_name = elt["dest"];
                retInstruction.push_back(elt);
            }
        } else {
            rhs_name = rhs["args"][0];
        }
    }


    json instruction;
    const std::string destination = GetTemporaryVariableName();
    if (op == "+") {
        instruction = {
                {"op", "add"}, {"dest", destination}, {"type", lhs_type.to_string()}, {"args", json::array({lhs_name, rhs_name})}};
    } else if (op == "-") {
        instruction = {
                {"op", "sub"}, {"dest", destination}, {"type", lhs_type.to_string()}, {"args", json::array({lhs_name, rhs_name})}};
    } else if (op == "*") {
        instruction = {
                {"op", "mul"}, {"dest", destination}, {"type", lhs_type.to_string()}, {"args", json::array({lhs_name, rhs_name})}};
    } else if (op == "/") {
        instruction = {
                {"op", "div"}, {"dest", destination}, {"type", lhs_type.to_string()}, {"args", json::array({lhs_name, rhs_name})}};
    } else if (op == ">") {
        instruction = {
                {"op", "gt"}, {"dest", destination}, {"type", lhs_type.to_string()}, {"args", json::array({lhs_name, rhs_name})}};
    } else if (op == "<") {
        instruction = {
                {"op", "lt"}, {"dest", destination}, {"type", "bool"}, {"args", json::array({lhs_name, rhs_name})}};
    } else if (op == "==") {
        instruction = {
                {"op", "eq"}, {"dest", destination}, {"type", "bool"}, {"args", json::array({lhs_name, rhs_name})}};
    } else if (op == ">=") {
        instruction = {
                {"op", "ge"}, {"dest", destination}, {"type", "bool"}, {"args", json::array({lhs_name, rhs_name})}};
    } else if (op == "<=") {
        instruction = {
                {"op", "le"}, {"dest", destination}, {"type", "bool"}, {"args", json::array({lhs_name, rhs_name})}};
    }
    retInstruction.push_back(instruction);

    return retInstruction;
}

// End Helper
// ====================================================
//

json IRGenerator::Generate(ExpressionAST &ast) { return ast.Accept(*this); }

json IRGenerator::Generate(StatementAST &ast) { return ast.Accept(*this); }

json IRGenerator::Generate(const VariableAssignmentAST &ast) {
    ExpressionAST &expr = *ast.expr;
    json gen_code = Generate(expr);
    json retInstruction = json::array();
    if (!gen_code.is_array() && gen_code["op"] == "const" && gen_code.contains("val")) {
            retInstruction.push_back(
                     {{"op", "const"}, {"dest", ast.variable_name.name}, {"type", gen_code["type"]}, {"value", gen_code["val"]}});
    }
    else if (gen_code.is_array() && gen_code[gen_code.size() - 1]["op"] == "call") {
        for (int i = 0 ; i < static_cast<int>(gen_code.size()) - 1 ; ++i) {
            retInstruction.push_back(gen_code[i]);
        }
        gen_code[gen_code.size() - 1]["dest"] = ast.variable_name.name;
        retInstruction.push_back(gen_code[gen_code.size() - 1]);
    }
    else {
        if (gen_code.is_array()) {
            for (const auto &elt: gen_code) {
                retInstruction.push_back(elt);
            }
            retInstruction.push_back({{"op", "id"},
                                      {"dest", ast.variable_name.name},
                                      {"type", expr.type.value().to_string()},
                                      {"args", {retInstruction.back()["dest"]}}});
        } else {
            retInstruction.push_back({
                    {"op", gen_code["op"]},
                    {"dest", ast.variable_name.name},
                    {"type", gen_code["type"]},
                    {"args", gen_code["args"]},
            });
        }
    }
    return retInstruction;
}

json IRGenerator::Generate(const VariableDeclareAndAssignAST &ast) {
    ExpressionAST &expr = *ast.expr;
    std::string type = expr.type->to_string();
    json gen_code = Generate(expr);
    json retInstruction;
    if (!gen_code.is_array() && gen_code["op"] == "const" && gen_code.contains("val")) {
        retInstruction.push_back(
                {{"op", "const"}, {"dest", ast.variable_name}, {"type", type}, {"value", gen_code["val"]}});
    }
    else if (gen_code.is_array() && gen_code[gen_code.size() - 1]["op"] == "call") {
        for (int i = 0 ; i < static_cast<int>(gen_code.size()) - 1 ; ++i) {
            retInstruction.push_back(gen_code[i]);
        }
        gen_code[gen_code.size() - 1]["dest"] = ast.variable_name;
        retInstruction.push_back(gen_code[gen_code.size() - 1]);
    }
    else if (gen_code.is_array()) {
        for (const auto &elt: gen_code) {
            retInstruction.push_back(elt);
        }
        retInstruction.push_back(
                {{"op", "id"}, {"dest", ast.variable_name}, {"type", type}, {"args", {retInstruction.back()["dest"]}}});
    } else {
        retInstruction.push_back({
                {"op", gen_code["op"]},
                {"dest", ast.variable_name},
                {"type", type},
                {"args", gen_code["args"]},
        });
    }
    return retInstruction;
}

json IRGenerator::Generate(const BinaryExpressionAST &ast) {
    auto &LHS = *ast.expr_lhs;
    auto &RHS = *ast.expr_rhs;
    return GenerateArithmeticOperations(*this, LHS, RHS, ast.op.GetOperatorSymbol());
}

json IRGenerator::Generate(const StringExpressionAST& ast){
    json instruction;
    instruction["op"] = "const";
    instruction["type"] = ast.type.value().to_string();
    instruction["val"] = ast.value;

    return instruction;
}

json IRGenerator::Generate(const NumberAST &ast) {
    json instruction;
    instruction["op"] = "const";
    instruction["type"] = ast.type.value().to_string();
    instruction["val"] = std::stol(ast.num);

    return instruction;
}

json IRGenerator::Generate(const IdentifierAST &ast) {
    json instruction;
    instruction["op"] = "id";
    instruction["type"] = ast.type.value().to_string();
    instruction["args"] = json::array({ast.name});

    return instruction;
}

json IRGenerator::Generate(const VariableDeclarationAST &ast) {
    json instruction = {{"op", "const"}, {"dest", ast.variable_name}, {"type", ""}, {"value", "undef"}};

    return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const BreakStatementAST &ast) {

    const std::string jmp_label = GetContext().GetBreakPointName();

    json instruction = {
            {"op", "jmp"},
            {"labels", {jmp_label}},
    };

    return instruction;
}

json IRGenerator::Generate(const ElseIfStatementAST &ast) {
    json retInstruction;

    auto &cond = *ast.condition;

    const json cond_json = Generate(cond);

    for (const auto &elt: cond_json) {
        retInstruction.push_back(elt);
    }

    auto condition_expr = retInstruction.back();
    const std::string cond_val = condition_expr["dest"];

    const std::string cond_var_name = GetTemporaryVariableName();

    json cond_var = {{"op", "id"}, {"dest", cond_var_name}, {"type", "bool"}, {"args", {cond_val}}};

    retInstruction.push_back(cond_var);

    const std::string else_if_body = GetLabelName();
    const std::string else_if_exit = GetLabelName();

    json break_instruction = {
            {"op", "br"},
            {"labels", {else_if_body, else_if_exit}},
            {"args", {cond_var_name}},
    };

    retInstruction.push_back(break_instruction);
    const json body_label = {{"label", else_if_body}};

    retInstruction.push_back(body_label);

    for (const auto &body: ast.else_if_body) {
        const json body_json = Generate(*body);

        if (body_json.is_array()) {
            for (const auto &b: body_json) {
                retInstruction.push_back(b);
            }
        } else {
            retInstruction.push_back(body_json);
        }
    }
    // Jump to exit.
    const json jump_exit = {{"op", "jmp"}, {"labels", {GetContext().GetIfExitLabelName()}}};
    retInstruction.push_back(jump_exit);

    const json exit_label = {{"label", else_if_exit}};
    retInstruction.push_back(exit_label);

    return retInstruction;
}

json IRGenerator::Generate(const ElseStatementAST &ast) {
    json retInstruction = json::array();

    for (const auto &elt: ast.GetBody()) {
        const json gen_json = Generate(*elt);
        if (gen_json.is_array()) {
            for (const auto &val: gen_json) {
                retInstruction.push_back(val);
            }
        } else {
            retInstruction.push_back(Generate(*elt));
        }
    }

    // Jump to exit.
    const json jump_exit = {{"op", "jmp"}, {"labels", {GetContext().GetIfExitLabelName()}}};
    retInstruction.push_back(jump_exit);

    return retInstruction;
}

json IRGenerator::Generate([[maybe_unused]] const ForLoopAST &ast) {
    json retInstruction = json::array();

    // update the current break points and skip points
    const std::string current_break_point = GetContext().GetBreakPointName();
    const std::string current_skip_point = GetContext().GetSkipPointName();

    const std::string loop_header = GetLabelName();

    const std::string iter_var_name = ast.GetIterationVariableName();
    const auto &range_expr = *ast.range;

    const auto &start_expr = range_expr.start;

    std::vector<SourceLocation> unknown;
    SourceLocation unknown_loc;
    std::unique_ptr<ExpressionAST> temp_start_expr;

    if (const auto id = dynamic_cast<const IdentifierAST *>(start_expr.get())) {
        temp_start_expr = std::make_unique<IdentifierAST>(id->name, unknown_loc);
        temp_start_expr->type = start_expr->type;
    } else if (const auto num = dynamic_cast<const NumberAST *>(start_expr.get())) {
        temp_start_expr = std::make_unique<NumberAST>(num->num, num->has_decimal, unknown_loc);
        temp_start_expr->type = start_expr->type; 
    }

    temp_start_expr->type = TypeKind::I64;
    // FIXME: set type to actual type later.
    auto tempAST = VariableDeclareAndAssignAST(iter_var_name, "i64", std::move(temp_start_expr), unknown);
    tempAST.type_name = TypeKind::I64;

    const json iter_arg_json = Generate(tempAST);

    for (const auto &elt: iter_arg_json) {
        retInstruction.push_back(elt);
    }

    // Start for loop header
    retInstruction.push_back({{"label", loop_header}});
    const auto &end_expr = range_expr.end;
    std::unique_ptr<ExpressionAST> temp_end_expr;

    if (const auto id = dynamic_cast<const IdentifierAST *>(end_expr.get())) {
        temp_end_expr = std::make_unique<IdentifierAST>(id->name, unknown_loc);
        temp_end_expr->type = end_expr->type;
    } else if (const auto num = dynamic_cast<const NumberAST *>(end_expr.get())) {
        temp_end_expr = std::make_unique<NumberAST>(num->num, num->has_decimal, unknown_loc);
        temp_end_expr->type = end_expr->type;
    }
    auto temp_iter_var_ast = std::make_unique<IdentifierAST>(iter_var_name, unknown_loc);
    temp_iter_var_ast->type = TypeKind::I64;

    const auto condition_check_ast = std::make_unique<BinaryExpressionAST>(
            std::move(temp_iter_var_ast), std::move(temp_end_expr), OperatorNode("<=", unknown_loc), unknown);

    condition_check_ast->type = end_expr->type;
    const json condition_expr = Generate(*condition_check_ast);

    for (const auto &elt: condition_expr) {
        retInstruction.push_back(elt);
    }

    const std::string cond_val = condition_expr.back()["dest"];

    const std::string cond_var = GetTemporaryVariableName();
    json cond = {{"op", "id"}, {"dest", cond_var}, {"type", "bool"}, {"args", {cond_val}}};

    retInstruction.push_back(cond);

    const std::string loop_body = GetLabelName();
    const std::string loop_exit = GetLabelName();

    // Create conditional jump
    const json break_instruction = {
            {"op", "br"},
            {"labels", {loop_body, loop_exit}},
            {"args", {cond_var}},
    };

    retInstruction.push_back(break_instruction);
    // end for loop header

    // start for loop body

    // Start for loop header
    retInstruction.push_back({{"label", loop_body}});

    // Set the new skip point to loops header
    GetContext().SetSkipPointName(loop_header);

    // Set the new break point to loop exit
    GetContext().SetBreakPointName(loop_exit);

    for (const auto &elt: ast.loop_body) {
        if ([[maybe_unused]] auto breakStmt = dynamic_cast<BreakStatementAST *>(elt.get())) {
            const json break_json = {{"op", "jmp"}, {"labels", {loop_exit}}};
            retInstruction.push_back(break_json);
            continue;
        }
        auto loop_stmt_json = Generate(*elt);
        if (loop_stmt_json.is_array()) {
            for (const auto &stmt: loop_stmt_json) {
                retInstruction.push_back(stmt);
            }
        } else {
            retInstruction.push_back(loop_stmt_json);
        }
    }

    // Reset the skip point
    GetContext().SetSkipPointName(current_skip_point);

    // Reset the break point to loop exit
    GetContext().SetBreakPointName(current_break_point);

    // Increment the loop iteration variable
    // FIXME: use the step instead of 1.

    // Create a constant 1
    std::unique_ptr<ExpressionAST> numAST = std::make_unique<NumberAST>("1", false, unknown_loc);
    numAST->type = TypeKind::I64;
    std::string const_one = GetTemporaryVariableName();

    auto idAST = std::make_unique<IdentifierAST>(iter_var_name, unknown_loc);
    idAST->type = TypeKind::I64;

    // Add 1 to the iteration variable.
    std::unique_ptr<ExpressionAST> addAST = std::make_unique<BinaryExpressionAST>(
            std::move(idAST), std::move(numAST), OperatorNode("+", unknown_loc), unknown);
    addAST->type = TypeKind::I64;

    auto newIdAST = IdentifierAST(iter_var_name, unknown_loc);
    newIdAST.type = TypeKind::I64;

    std::vector<SourceLocation> loc = {unknown_loc};
    const auto assignAddResultAST = std::make_unique<VariableAssignmentAST>(newIdAST, std::move(addAST), loc);

    // Update the iteration variable
    const auto increment_json = Generate(*assignAddResultAST);

    for (const auto &elt: increment_json) {
        retInstruction.push_back(elt);
    }

    // Unconditional jump to the loop header
    const json jmp_loop_header = {{"op", "jmp"}, {"labels", {loop_header}}};

    retInstruction.push_back(jmp_loop_header);

    // End of loop body

    // Start of loop exit
    const json loop_exit_label = {{"label", loop_exit}};

    retInstruction.push_back(loop_exit_label);

    return retInstruction;
}

json IRGenerator::Generate([[maybe_unused]] const FunctionDefinitionAST &ast) {
    json instruction;

    return instruction;
}

json IRGenerator::Generate(const IfStatementAST &ast) {
    json retInstruction = json::array({});

    ExpressionAST &if_condition = *ast.condition;

    const std::string current_exit_label = GetContext().GetIfExitLabelName();

    if (const json &condition_val = Generate(if_condition); condition_val.is_array()) {
        for (const auto &elt: condition_val) {
            retInstruction.push_back(elt);
        }
    }

    auto condition_expr = retInstruction.back();
    const std::string cond_val = condition_expr["dest"];

    json cond = {{"op", "id"}, {"dest", "cond"}, {"type", "bool"}, {"args", {cond_val}}};

    retInstruction.push_back(cond);

    const std::string success_label = GetLabelName();
    const std::string failure_label = GetLabelName();

    // label to jump to, if one of the body gets executed.
    const std::string exit_label = GetLabelName();

    GetContext().SetIfExitLabel(exit_label);

    if (ast.hasElse() || ast.hasElseIf()) {
        json break_instruction = {
                {"op", "br"},
                {"labels", {success_label, failure_label}},
                {"args", {"cond"}},
        };

        retInstruction.push_back(break_instruction);
    } else {
        json break_instruction = {
                {"op", "br"},
                {"labels", {success_label, exit_label}},
                {"args", {"cond"}},
        };

        retInstruction.push_back(break_instruction);
    }


    const json label_instruction = {{"label", success_label}};
    retInstruction.push_back(label_instruction);

    for (const auto &elt: ast.if_body) {

        if (const json &gen_json = Generate(*elt); gen_json.is_array()) {
            for (const auto &val: gen_json) {
                retInstruction.push_back(val);
            }
        } else {
            retInstruction.push_back(gen_json);
        }
    }

    // Jump to exit.
    const json jump_exit = {{"op", "jmp"}, {"labels", {exit_label}}};
    retInstruction.push_back(jump_exit);

    if (ast.hasElse() || ast.hasElseIf()) {
        const json if_fail = {{"label", failure_label}};
        retInstruction.push_back(if_fail);
    }

    // Generate Else If
    for (const auto &elt: ast.else_if_ast) {
        if (const json else_if_json = Generate(*elt); else_if_json.is_array()) {
            for (const auto &else_if: else_if_json) {
                retInstruction.push_back(else_if);
            }
        } else {
            retInstruction.push_back(else_if_json);
        }
    }

    // Generate else
    if (ast.hasElse()) {
        for (auto else_json = Generate(*ast.else_ast); const auto &elt: else_json) {
            retInstruction.push_back(elt);
        }
    }


    const json exit_label_json = {{"label", exit_label}};
    retInstruction.push_back(exit_label_json);

    GetContext().SetIfExitLabel(current_exit_label);

    return retInstruction;
}

json IRGenerator::Generate(const LoopAST &ast) {
    json retInstruction = json::array({});

    const std::string current_break_point = GetContext().GetBreakPointName();
    const std::string current_skip_point = GetContext().GetSkipPointName();

    const std::string body_label = GetLabelName();
    const std::string outside_loop_body_label = GetLabelName();

    const json label_instruction = {{"label", body_label}};
    retInstruction.push_back(label_instruction);

    // Set new break points and skip points
    GetContext().SetBreakPointName(outside_loop_body_label);
    GetContext().SetSkipPointName(body_label);

    for (const auto &elt: ast.loop_body) {
        auto bodyElementJson = Generate(*elt);
        if ([[maybe_unused]] auto breakStatement = dynamic_cast<BreakStatementAST *>(elt.get())) {
            if (!bodyElementJson.is_array()) {
                if (bodyElementJson["op"] == "jmp" && bodyElementJson["labels"].is_null()) {
                    bodyElementJson["labels"].push_back(outside_loop_body_label);
                    retInstruction.push_back(bodyElementJson);
                }
            }
        } else {
            if (bodyElementJson.is_array()) {
                for (const auto &element: bodyElementJson) {
                    retInstruction.push_back(element);
                }
            } else {
                retInstruction.push_back(bodyElementJson);
            }
        }
    }

    GetContext().SetBreakPointName(current_break_point);
    GetContext().SetSkipPointName(current_skip_point);

    const json jump_instruction = {{"op", "jmp"}, {"labels", {body_label}}};
    retInstruction.push_back(jump_instruction);

    const json exit_label_instruction = {{"label", outside_loop_body_label}};
    retInstruction.push_back(exit_label_instruction);

    return retInstruction;
}

json IRGenerator::Generate([[maybe_unused]] const MatchArmAST &ast) {
    json instruction;

    return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const MatchStatementAST &ast) {
    json retInstruction = json::array();

    const auto match_type = ast.type;

    if (!ast.arms.empty()) {

        if (match_type == MatchType::ALL) {
            std::vector<std::unique_ptr<ElseIfStatementAST>> empty_else_if;

            for (const auto &arm: ast.arms) {
                auto if_stmt = std::make_unique<IfStatementAST>(std::move(arm->cond), std::move(arm->body),
                                                                empty_else_if, nullptr, arm->GetSourceLocation());

                for (auto if_stmt_json = Generate(*if_stmt); const auto &if_json: if_stmt_json) {
                    retInstruction.push_back(if_json);
                }
            }

        }

        else {
            auto &first_arm = ast.arms[0];
            std::vector<std::unique_ptr<ElseIfStatementAST>> else_if_stmts;

            for (const auto &arm: ast.arms | std::views::drop(1)) {
                auto else_if_stmt = std::make_unique<ElseIfStatementAST>(
                        std::move(arm->cond), std::move(arm->body), arm->GetSourceLocation());
                else_if_stmts.push_back(std::move(else_if_stmt));
            }

            const auto if_stmt =
                    std::make_unique<IfStatementAST>(std::move(first_arm->cond), std::move(first_arm->body),
                                                     else_if_stmts, nullptr, first_arm->GetSourceLocation());

            for (const auto if_stmt_json = Generate(*if_stmt); const auto &if_json: if_stmt_json) {
                retInstruction.push_back(if_json);
            }
        }
    }

    return retInstruction;
}

json IRGenerator::Generate([[maybe_unused]] const RangeAST &ast) {
    json instruction;

    return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const WhileLoopAST &ast) {
    json instruction;

    return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const FunctionCallAST &ast) {
    json instruction;

    return instruction;
}

json IRGenerator::Generate(const FunctionCallExprAST &ast) {
    json retInstruction = json::array({});
    const std::string fn_name = ast.fn_name->name;

    auto& args = *ast.args;

    std::vector<std::string> arg_names;

    for (auto& elt: args.GetFunctionArguments()) {
        if (auto* numAST = dynamic_cast<NumberAST*>(elt.get())) {
            auto tempNumAST = std::make_unique<NumberAST>(numAST->num, numAST->has_decimal,
                                                      numAST->GetSourceLocation().front());
            std::string num = numAST->num;

            tempNumAST->value = numAST->value;
            tempNumAST->type = numAST->type;

            const std::string tempVarName = GetTemporaryVariableName();
            std::vector<SourceLocation> unknown{};

            VariableDeclareAndAssignAST tempAST(tempVarName, elt->type.value().to_string(), std::move(tempNumAST), unknown);
            tempAST.type_name = elt->type.value();

            auto new_gen_code = Generate(tempAST);
            if (new_gen_code.is_array()) {
                for (const auto& e: new_gen_code) {
                    retInstruction.push_back(e);

                }
            }
            else {
                retInstruction.push_back(new_gen_code);
            }
            arg_names.push_back(tempVarName);
        }
        else if (auto* idAST = dynamic_cast<IdentifierAST*>(elt.get())) {
            const std::string id_name = idAST->name;
            arg_names.push_back(id_name);
        }
        else {

            if (auto gen_code = Generate(*elt); !gen_code.is_array() && gen_code.contains("dest")) {
                arg_names.push_back(gen_code["dest"]);
            }
            else {
                arg_names.push_back(gen_code[gen_code.size() - 1]["dest"]);
                for (const auto& e : gen_code) {
                    retInstruction.push_back(e);
                }
            }
        }

    }

    json instruction = {
        {"op", "call"},
        {"funcs", {fn_name}},
        {"args", arg_names},
        {"type", ast.type.value().to_string()}
    };

    retInstruction.push_back(instruction);
    return retInstruction;
}


json IRGenerator::Generate([[maybe_unused]] const FunctionArgumentAST &ast) {
    json instruction;

    return instruction;
}

json IRGenerator::Generate(const ReturnStatementAST &ast) {

    const std::string type = ast.GetReturnExpression().type.value().to_string();

    if (const auto idAST = dynamic_cast<IdentifierAST *>(&ast.GetReturnExpression())) {
        json instruction;

        instruction["op"] = "ret";
        instruction["args"] = json::array({idAST->name});

        return instruction;
    }

    if (const auto numAST = dynamic_cast<NumberAST *>(&ast.GetReturnExpression())) {
        json instruction = json::array();
        std::vector<SourceLocation> unknown;
        // Create a temporary variable
        auto tempNumAST = std::make_unique<NumberAST>(numAST->num, numAST->has_decimal,
                                                      numAST->GetSourceLocation().front());
        std::string num = numAST->num;

        tempNumAST->value = numAST->value;
        tempNumAST->type  = numAST->type.value();

        VariableDeclareAndAssignAST tempAST(GetTemporaryVariableName(), type, std::move(tempNumAST), unknown);

        tempAST.type_name = ast.GetReturnExpression().type.value();

        const json declAssignJson = Generate(tempAST);
        if (declAssignJson.is_array()) {
            for (const auto& elt: declAssignJson) {
                instruction.push_back(elt);
            }
        }
        else {
            instruction.push_back(declAssignJson);
        }

        json returnJson;

        returnJson["op"] = "ret";
        returnJson["args"] = json::array({tempAST.variable_name});

        instruction.push_back(returnJson);
        return instruction;
    }

    if (auto binaryStatement = dynamic_cast<BinaryExpressionAST *>(&ast.GetReturnExpression())) {
        auto binary_statement_json = Generate(*binaryStatement);

        json retInstruction = json::array();

        for (const auto &elt: binary_statement_json) {
            retInstruction.push_back(elt);
        }

        const std::string return_value = binary_statement_json.back()["dest"];

        retInstruction.push_back({{"op", "ret"}, {"args", {return_value}}});

        return retInstruction;
    }

    if (auto callExpr = dynamic_cast<FunctionCallExprAST* >(&ast.GetReturnExpression())) {
        auto call_expr_json = Generate(*callExpr);

        json retInstruction = json::array();

        if (call_expr_json.is_array()) {
            for (const auto& elt : call_expr_json) {
                retInstruction.push_back(elt);
            }
        }
        else {
            retInstruction.push_back(call_expr_json);
        }

        // Create a temp var to store the call result
        const std::string temp_var_name = GetTemporaryVariableName();

        auto call_op = call_expr_json.back();

        call_op["dest"] = temp_var_name;

        retInstruction.push_back({{"op", "ret"} , {"args", {temp_var_name}}});

    }

    return {};
}

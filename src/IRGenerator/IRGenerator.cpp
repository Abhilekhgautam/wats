#include "IRGenerator.hpp"
#include "../AST/BinaryExpressionAST.hpp"
#include "../AST/ExpressionAST.hpp"
#include "../AST/IfStatementAST.hpp"
#include "../AST/LoopAST.hpp"
#include "../AST/NumberAST.hpp"
#include "../AST/ReturnStatementAST.h"
#include "../AST/VariableDeclarationAST.hpp"
#include "../AST/VariableDeclareAndAssignAST.hpp"

#include <format>

using nlohmann::json;

static std::string GetTemporaryVariableName() {
  static int count = 1;
  return std::format("var_{}", count++);
}

static std::string GetLabelName() {
  static int count = 0;
  return std::format(".bb_{}", count++);
}

// Helper for Arithmetic Operations
// ===================================================
//
json GenerateArithmeticOperations(IRGenerator &generator,
                                  const BinaryExpressionAST &ast
                                  ) {
  json retInstruction = json::array();

  auto& lhs_expr = ast.GetLeftOperand();
  const std::string lhs_type = lhs_expr.GetType();

  auto& rhs_expr = ast.GetRightOperand();
  const std::string rhs_type = rhs_expr.GetType();

  std::vector<SourceLocation> unknown;

  std::string lhs_name;
  std::string rhs_name;
  if (auto numAST = dynamic_cast<NumberAST *>(&lhs_expr)) {
    auto tempNumAST = std::make_unique<NumberAST>(numAST->GetNumber(), numAST->HasDecimal(), numAST->GetSourceLocation().front());
    std::string num = numAST->GetNumber();

    tempNumAST->SetValue(numAST->GetValue());
    const std::string tempVarName = GetTemporaryVariableName();
    VariableDeclareAndAssignAST tempAST(tempVarName, lhs_type, std::move(tempNumAST), unknown);
    tempAST.SetType(lhs_type);

    json tempASTJSON = generator.Generate(tempAST);
    retInstruction.push_back(tempASTJSON);
    lhs_name = tempVarName;
  } else {
    json lhs = generator.Generate(ast.GetLeftOperand());
    lhs_name = lhs["args"][0];
  }


  if (auto numAST = dynamic_cast<NumberAST *>(&rhs_expr)) {
    auto tempNumAST = std::make_unique<NumberAST>(numAST->GetNumber(), numAST->HasDecimal(), numAST->GetSourceLocation().front());
    std::string num = numAST->GetNumber();

    tempNumAST->SetValue(numAST->GetValue());
    const std::string tempVarName = GetTemporaryVariableName();
    VariableDeclareAndAssignAST tempAST(tempVarName, lhs_type, std::move(tempNumAST), unknown);
    tempAST.SetType(lhs_type);

    json tempASTJSON = generator.Generate(tempAST);
    retInstruction.push_back(tempASTJSON);
    rhs_name = tempVarName;
  } else {
    json rhs = generator.Generate(ast.GetRightOperand());
    rhs_name = rhs["args"][0];
  }


  json instruction;
  const std::string destination = GetTemporaryVariableName();
  if (ast.GetOperator() == "+") {
    instruction = {{"op", "add"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
  } else if (ast.GetOperator() == "-") {
    instruction = {{"op", "sub"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
  } else if (ast.GetOperator() == "*") {
    instruction = {{"op", "mul"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
  } else if (ast.GetOperator() == "/") {
    instruction = {{"op", "div"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
  } else if (ast.GetOperator() == ">") {
    instruction = {{"op", "gt"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
  } else if (ast.GetOperator() == "<") {
    instruction = {{"op", "lt"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
  } else if (ast.GetOperator() == "==") {
    instruction = {{"op", "eq"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
  } else if (ast.GetOperator() == ">=") {
    instruction = {{"op", "ge"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
  } else if (ast.GetOperator() == "<=") {
    instruction = {{"op", "le"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name , rhs_name})}};
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
  ExpressionAST &expr = ast.GetExpr();
  json gen_code = Generate(expr);
  json retInstruction = json::array();
  if (!gen_code.is_array() && gen_code["op"] == "const" && gen_code.contains("val"))
    retInstruction.push_back({{"op", "const"},
                   {"dest", ast.GetVarName()},
                   {"type", gen_code["type"]},
                   {"value", gen_code["val"]}});
  else {
    if (gen_code.is_array()) {
      for (const auto& elt : gen_code) {
        retInstruction.push_back(elt);
      }
      retInstruction .push_back({{"op", "id"},
                   {"dest", ast.GetVarName()},
                   {"type", expr.GetType()},
                   {"value", retInstruction.back()["dest"]}});
    }
    else {
      retInstruction.push_back({
        {"op", gen_code["op"]},
        {"dest", ast.GetVarName()},
        {"type", gen_code["type"]},
        {"args", gen_code["args"]},
    });
    }

  }
  return retInstruction;
}

json IRGenerator::Generate(const VariableDeclareAndAssignAST &ast) {
  ExpressionAST &expr = ast.GetExpr();
  std::string type = expr.GetType();
  json gen_code = Generate(expr);
  json retInstruction;
  if (!gen_code.is_array() && gen_code["op"] == "const" && gen_code.contains("val")) {
    retInstruction .push_back({{"op", "const"},
                   {"dest", ast.GetVarName()},
                   {"type", type},
                   {"value", gen_code["val"]}});
  } else if (gen_code.is_array()) {
    for (const auto& elt: gen_code) {
      retInstruction.push_back(elt);
    }
    retInstruction .push_back({{"op", "id"},
                   {"dest", ast.GetVarName()},
                   {"type", type},
                   {"value", retInstruction.back()["dest"]}});
  } else {
    retInstruction.push_back({
        {"op", gen_code["op"]},
        {"dest", ast.GetVarName()},
        {"type", type},
        {"args", gen_code["args"]},
    });
  }
  return retInstruction;
}

json IRGenerator::Generate(const BinaryExpressionAST &ast) {
  return GenerateArithmeticOperations(*this, ast);
  /*
  ExpressionAST &lhs = ast.GetLeftOperand();
  ExpressionAST &rhs = ast.GetRightOperand();

  json lhs_val = Generate(lhs);

  json rhs_val = Generate(rhs);

  json instrVec = json::array({});

  if (!rhs_val.is_array() && (rhs_val["op"] == "const" || rhs_val["op"] == "id")) {
    return GenerateArithmeticOperations(*this, ast, "abhilekh_todo");
  }

  else {
    auto &another_rhs = dynamic_cast<BinaryExpressionAST &>(rhs);
    // ExpressionAST &new_lhs = another_rhs.GetLeftOperand();
    ExpressionAST &new_rhs = another_rhs.GetRightOperand();

    return Generate(new_rhs);
    // Generate(new_lhs);
  }
  */
}

json IRGenerator::Generate(const NumberAST &ast) {
  json instruction;
  instruction["op"] = "const";
  instruction["type"] = ast.GetType();
  instruction["val"] = std::stol(ast.GetNumber());

  return instruction;
}

json IRGenerator::Generate(const IdentifierAST &ast) {
  json instruction;
  instruction["op"] = "id";
  instruction["type"] = ast.GetType();
  instruction["args"] = json::array({ast.GetName()});

  return instruction;
}

json IRGenerator::Generate(const VariableDeclarationAST &ast) {
  json instruction = {{"op", "const"},
                      {"dest", ast.GetVarName()},
                      {"type", ""},
                      {"value", "undef"}};

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const BreakStatementAST &ast) {
  json instruction = {
    {"op", "jmp"},
    {"labels", {}},
  };

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const ElseIfStatementAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const ElseStatementAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const ForLoopAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]]const FunctionDefinitionAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate(const IfStatementAST &ast) {
  json instruction = json::array({});

  ExpressionAST &if_condition = ast.GetIfCondition();

  json condition_val = Generate(if_condition);

  json cond;

  if (!condition_val["val"].is_null()) {
    cond = {{"op", "const"},
            {"dest", "cond"},
            {"type", "bool"},
            {"value", condition_val["val"]}};
  } else {
    cond = {{"op", condition_val["op"]},
            {"dest", "cond"},
            {"type", "bool"},
            {"args", condition_val["args"]}};
  }

  instruction.push_back(cond);

  std::string success_label = GetLabelName();
  std::string failure_label = GetLabelName();
  json break_instruction = {
      {"op", "br"},
      {"labels", {success_label, failure_label}},
      {"args", {"cond"}},
  };

  instruction.push_back(break_instruction);

  json label_instruction = {{"label", success_label}};
  instruction.push_back(label_instruction);

  for (const auto &elt : ast.GetIfBody()) {
    instruction.push_back(Generate(*elt));
  }

  if (!ast.hasElse() && !ast.hasElseIf()) {
    json label_instruction = {{"label", failure_label}};
    instruction.push_back(label_instruction);
  }

  return instruction;
}

json IRGenerator::Generate(const LoopAST &ast) {
  json retInstruction = json::array({});

  const std::string body_label = GetLabelName();
  const std::string outside_loop_body_label = GetLabelName();

  const json label_instruction = {{"label", body_label}};
  retInstruction.push_back(label_instruction);

  for (const auto &elt : ast.GetLoopBody()) {
    auto bodyElementJson = Generate(*elt);
    if (auto breakStatement = dynamic_cast<BreakStatementAST*>(elt.get())) {
        if (!bodyElementJson.is_array()) {
          if (bodyElementJson["op"] == "jmp" && bodyElementJson["labels"].is_null()) {
            bodyElementJson["labels"].push_back(outside_loop_body_label);
            retInstruction.push_back(bodyElementJson);
          }
        }
    } else {
      if (bodyElementJson.is_array()) {
        for (const auto& element: bodyElementJson) {
          retInstruction.push_back(element);
        }
      } else {
        retInstruction.push_back(bodyElementJson);
      }
    }
  }
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
  json instruction;

  return instruction;
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

json IRGenerator::Generate([[maybe_unused]] const FunctionCallExprAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] const FunctionArgumentAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate(const ReturnStatementAST& ast) {

  const std::string type = ast.GetReturnExpression().GetType();

  if (const auto idAST = dynamic_cast<IdentifierAST*>(&ast.GetReturnExpression())) {
    json instruction;

    instruction["op"] = "ret";
    instruction["args"] = json::array({idAST->GetName()});

    return instruction;
  }

  if (const auto numAST = dynamic_cast<NumberAST*>(&ast.GetReturnExpression())) {
    json instruction = json::array();
    std::vector<SourceLocation> unknown;
    // Create a temporary variable
    auto tempNumAST = std::make_unique<NumberAST>(numAST->GetNumber(), numAST->HasDecimal(), numAST->GetSourceLocation().front());
    std::string num = numAST->GetNumber();

    tempNumAST->SetValue(numAST->GetValue());

    VariableDeclareAndAssignAST tempAST(GetTemporaryVariableName(), type, std::move(tempNumAST), unknown);
    tempAST.SetType(type);

    const json declAssignJson = Generate(tempAST);
    instruction.push_back(declAssignJson);

    json returnJson;

    returnJson["op"] = "ret";
    returnJson["args"] = json::array({tempAST.GetVarName()});

    instruction.push_back(returnJson);
    return instruction;
  }

  return {};
}

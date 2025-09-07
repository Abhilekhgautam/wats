#include "IRGenerator.hpp"
#include "../AST/BinaryExpressionAST.hpp"
#include "../AST/ExpressionAST.hpp"
#include "../AST/IfStatementAST.hpp"
#include "../AST/LoopAST.hpp"
#include "../AST/NumberAST.hpp"
#include "../AST/VariableDeclarationAST.hpp"
#include "../AST/VariableDeclareAndAssignAST.hpp"

using nlohmann::json;

// Helper for Arithmetic Operations
// ===================================================
//
json GenerateArithmeticOperations(IRGenerator &generator,
                                  BinaryExpressionAST &ast,
                                  std::string destination) {
  json lhs = generator.Generate(ast.GetLeftOperand());
  json rhs = generator.Generate(ast.GetRightOperand());

  json instruction;
  if (ast.GetOperator() == "+") {
    instruction = {{"op", "add"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
  } else if (ast.GetOperator() == "-") {
    instruction = {{"op", "sub"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
    return instruction;
  } else if (ast.GetOperator() == "*") {
    instruction = {{"op", "mul"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
    return instruction;
  } else if (ast.GetOperator() == "/") {
    instruction = {{"op", "div"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
    return instruction;
  } else if (ast.GetOperator() == ">") {
    instruction = {{"op", "gt"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
  } else if (ast.GetOperator() == "<") {
    instruction = {{"op", "lt"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
  } else if (ast.GetOperator() == "==") {
    instruction = {{"op", "eq"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
  } else if (ast.GetOperator() == ">=") {
    instruction = {{"op", "ge"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
  } else if (ast.GetOperator() == "<=") {
    instruction = {{"op", "le"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs["val"], rhs["val"]})}};
  }
  return instruction;
}

// End Helper
// ====================================================
//

json IRGenerator::Generate(ExpressionAST &ast) { return ast.Accept(*this); }

json IRGenerator::Generate(StatementAST &ast) { return ast.Accept(*this); }

json IRGenerator::Generate(VariableAssignmentAST &ast) {
  ExpressionAST &expr = ast.GetExpr();
  json gen_code = Generate(expr);
  json instruction;
  if (gen_code["op"] == "const")
    instruction = {{"op", "const"},
                   {"dest", ast.GetVarName()},
                   {"type", gen_code["type"]},
                   {"value", gen_code["val"]}};
  else {
    instruction = {
        {"op", gen_code["op"]},
        {"dest", ast.GetVarName()},
        {"type", gen_code["type"]},
        {"args", gen_code["args"]},
    };
  }
  return instruction;
}

json IRGenerator::Generate(VariableDeclareAndAssignAST &ast) {
  ExpressionAST &expr = ast.GetExpr();
  json gen_code = Generate(expr);
  json instruction;
  if (!gen_code.is_array() && gen_code["op"] == "const") {
    instruction = {{"op", "const"},
                   {"dest", ast.GetVarName()},
                   {"type", gen_code["type"]},
                   {"value", gen_code["val"]}};
  } else if (gen_code.is_array()) {
    instruction = {{"op", "array"},
                   {"dest", ast.GetVarName()},
                   /*{"type", gen_code[0]["type"]},
                   {"value", gen_code[0]["val"]}*/};

  } else {
    instruction = {
        {"op", gen_code["op"]},
        {"dest", ast.GetVarName()},
        {"type", gen_code["type"]},
        {"args", gen_code["args"]},
    };
  }
  return instruction;
}

json IRGenerator::Generate(BinaryExpressionAST &ast) {
  ExpressionAST &lhs = ast.GetLeftOperand();
  ExpressionAST &rhs = ast.GetRightOperand();

  json lhs_val = Generate(lhs);

  json rhs_val = Generate(rhs);

  json instrVec = json::array({});

  if (!rhs_val.is_array() && rhs_val["op"] == "const") {
    return GenerateArithmeticOperations(*this, ast, "abhilekh_todo");
  }

  else {
    BinaryExpressionAST &another_rhs = dynamic_cast<BinaryExpressionAST &>(rhs);
    // ExpressionAST &new_lhs = another_rhs.GetLeftOperand();
    ExpressionAST &new_rhs = another_rhs.GetRightOperand();

    return Generate(new_rhs);
    // Generate(new_lhs);
  }
}

json IRGenerator::Generate(NumberAST &ast) {
  json instruction;
  instruction["op"] = "const";
  instruction["type"] = ast.GetType();
  instruction["val"] = ast.GetNumber();

  return instruction;
}

json IRGenerator::Generate(IdentifierAST &ast) {
  json instruction;
  instruction["op"] = "const";
  instruction["type"] = ast.GetType();
  instruction["val"] = ast.GetName();

  return instruction;
}

json IRGenerator::Generate(VariableDeclarationAST &ast) {
  json instruction = {{"op", "const"},
                      {"dest", ast.GetVarName()},
                      {"type", ""},
                      {"value", "undef"}};

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] BreakStatementAST &ast) {
  json instruction;

  instruction = {
      {"op", "jmp"},
      {"labels", {}},
  };

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] ElseIfStatementAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] ElseStatementAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] ForLoopAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] FunctionDefinitionAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate(IfStatementAST &ast) {
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

  json break_instruction = {
      {"op", "br"},
      {"labels", {"if_body", "after_if"}},
      {"args", {"cond"}},
  };

  instruction.push_back(break_instruction);

  json label_instruction = {{"label", "if_body"}};
  instruction.push_back(label_instruction);

  for (const auto &elt : ast.GetIfBody()) {
    instruction.push_back(Generate(*elt));
  }

  if (!ast.hasElse() && !ast.hasElseIf()) {
    json label_instruction = {{"label", "after_if"}};
    instruction.push_back(label_instruction);
  }

  return instruction;
}

json IRGenerator::Generate(LoopAST &ast) {
  json instruction = json::array({});
  json label_instruction = {{"label", "test"}};
  instruction.push_back(label_instruction);
  for (const auto &elt : ast.GetLoopBody()) {
    auto val = Generate(*elt);
    if (!val.is_array()) {
      // handle break: exactly within the loop body (not within any other
      // block)
      if (val["op"] == "jmp") {
        val["labels"].push_back("test_out");
      }
      instruction.push_back(val);
    } else {
      for (auto &v : val) {
        // handle break: not exactly within the loop body (within any other
        // block)
        if (v["op"] == "jmp" && v["labels"].is_null()) {
          v["labels"].push_back("test_out");
        }
        instruction.push_back(v);
      }
    }
  }
  json jump_instruction;
  jump_instruction = {{"op", "jmp"}, {"labels", {"test"}}};
  instruction.push_back(jump_instruction);

  json exit_label_instruction = {{"label", "test_out"}};
  instruction.push_back(exit_label_instruction);

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] MatchArmAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] MatchStatementAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] RangeAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] WhileLoopAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] FunctionCallAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] FunctionCallExprAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] FunctionArgumentAST &ast) {
  json instruction;

  return instruction;
}

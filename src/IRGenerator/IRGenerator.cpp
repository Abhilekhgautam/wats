#include "IRGenerator.hpp"
#include "../AST/BinaryExpressionAST.hpp"
#include "../AST/ExpressionAST.hpp"
#include "../AST/IfStatementAST.hpp"
#include "../AST/LoopAST.hpp"
#include "../AST/NumberAST.hpp"
#include "../AST/VariableDeclarationAST.hpp"
#include "../AST/VariableDeclareAndAssignAST.hpp"
#include "../AST/WhileLoopAST.hpp"

using nlohmann::json;

// Helper for Arithmetic Operations
// ===================================================
//
json GenerateArithmeticOperations(BinaryExpressionAST &ast,
                                  std::string destination, std::string lhs_name,
                                  std::string rhs_name) {
  json instruction;
  if (ast.GetOperator() == "+") {
    instruction = {{"op", "add"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
  } else if (ast.GetOperator() == "-") {
    instruction = {{"op", "sub"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
    return instruction;
  } else if (ast.GetOperator() == "*") {
    instruction = {{"op", "mul"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
    return instruction;
  } else if (ast.GetOperator() == "/") {
    instruction = {{"op", "div"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
    return instruction;
  } else if (ast.GetOperator() == ">") {
    instruction = {{"op", "gt"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
  } else if (ast.GetOperator() == "<") {
    instruction = {{"op", "lt"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
  } else if (ast.GetOperator() == "==") {
    instruction = {{"op", "eq"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
  } else if (ast.GetOperator() == ">=") {
    instruction = {{"op", "ge"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
  } else if (ast.GetOperator() == "<=") {
    instruction = {{"op", "le"},
                   {"dest", destination},
                   {"type", ast.GetType()},
                   {"args", json::array({lhs_name, rhs_name})}};
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
  json expression_json = Generate(expr);
  json instruction;
  if (expression_json.is_array()) {
    json last_expression = expression_json.back();
    for (size_t i = 0; i < expression_json.size() - 1; i++) {
      instruction.push_back(expression_json[i]);
    }
    last_expression["dest"] = ast.GetVarName();
    instruction.push_back(last_expression);

  } else if (expression_json["op"] == "const" &&
             expression_json.contains("val")) {
    instruction = {{"op", "const"},
                   {"dest", ast.GetVarName()},
                   {"type", expression_json["type"]},
                   {"value", expression_json["val"]}};
  } else {
    instruction = {
        {"op", expression_json["op"]},
        {"dest", ast.GetVarName()},
        {"type", expression_json["type"]},
        {"args", expression_json["args"]},
    };
  }

  return instruction;
}

json IRGenerator::Generate(VariableDeclareAndAssignAST &ast) {
  ExpressionAST &expr = ast.GetExpr();
  json expression_json = Generate(expr);
  json instruction;
  if (expression_json.is_array()) {
    json last_expression = expression_json.back();
    for (size_t i = 0; i < expression_json.size() - 1; i++) {
      instruction.push_back(expression_json[i]);
    }
    last_expression["dest"] = ast.GetVarName();
    instruction.push_back(last_expression);

  } else if (expression_json["op"] == "const" &&
             expression_json.contains("val")) {
    instruction = {{"op", "const"},
                   {"dest", ast.GetVarName()},
                   {"type", expression_json["type"]},
                   {"value", expression_json["val"]}};
  } else {
    instruction = {
        {"op", expression_json["op"]},
        {"dest", ast.GetVarName()},
        {"type", expression_json["type"]},
        {"args", expression_json["args"]},
    };
  }

  return instruction;
}

json IRGenerator::Generate(BinaryExpressionAST &ast) {
  json instructions = json::array({});

  json lhs_ir = Generate(ast.GetLeftOperand());
  std::string lhs_arg_name = extract_ir_result(lhs_ir, instructions);

  json rhs_ir = Generate(ast.GetRightOperand());
  std::string rhs_arg_name = extract_ir_result(rhs_ir, instructions);

  std::string final_dest = NewTempVar();

  json final_op =
      GenerateArithmeticOperations(ast, final_dest, lhs_arg_name, rhs_arg_name);
  instructions.push_back(final_op);
  return instructions;
}

json IRGenerator::Generate(NumberAST &ast) {
  json instruction;
  instruction["op"] = "const";
  instruction["type"] = ast.GetType();
  instruction["val"] = std::stol(ast.GetNumber());

  return instruction;
}

json IRGenerator::Generate(IdentifierAST &ast) {
  json instruction;
  instruction["op"] = "id";
  instruction["type"] = ast.GetType();
  instruction["args"] = json::array({ast.GetName()});

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
  json instruction = {
      {"op", "jmp"},
      {"labels", {}},
  };

  return instruction;
}

json IRGenerator::Generate(ElseIfStatementAST &ast) {
  json instructions = json::array({});

  // Generate a unique label for this block's body
  std::string elseif_body_label = NewTempVar() + "_elseif_body";

  // We use a placeholder for the exit jump, which the parent (IfStatementAST)
  // will fix to point to the next 'else if' or the final 'if_exit_label'.
  std::string temp_exit_label = NewTempVar() + "_temp_exit_jmp";

  ExpressionAST &cond = ast.GetCondition();

  // 1. Generate IR for the Condition Expression
  json condition_ir = Generate(cond);
  std::string cond_result_name = extract_ir_result(condition_ir, instructions);

  // 2. Conditional Branch: True -> elseif_body, False -> temp_exit_label
  //    (temp_exit_label will be retargeted by the parent to be the next block's
  //    check)
  json branch_instr = {
      {"op", "br"},
      {"labels", {elseif_body_label, temp_exit_label}},
      {"args", {cond_result_name}},
  };
  instructions.push_back(branch_instr);

  // 3. Else-If Body Label
  instructions.push_back({{"label", elseif_body_label}});

  // 4. Generate IR for the Body Statements
  for (auto &elt : ast.GetBody()) {
    auto val = Generate(*elt);
    if (val.is_array()) {
      for (auto &v : val)
        instructions.push_back(v);
    } else {
      instructions.push_back(val);
    }
  }

  // 5. Unconditional Jump: Must jump to the overall 'if_exit_label'
  //    (The parent will retarget this placeholder)
  instructions.push_back({{"op", "jmp"}, {"labels", {temp_exit_label}}});

  // The parent function (IfStatementAST) will collect this array and fix the
  // labels.
  return instructions;
}

json IRGenerator::Generate(ElseStatementAST &ast) {
  json instructions = json::array({});

  // 1. Generate IR for the Body Statements
  for (auto &body : ast.GetBody()) {
    auto val = Generate(*body);
    if (val.is_array()) {
      for (auto &v : val)
        instructions.push_back(v);
    } else {
      instructions.push_back(val);
    }
  }

  // NOTE: We do not add a jump instruction here. Execution simply proceeds
  // to the next label placed by the parent (IfStatementAST), which is the final
  // 'if_exit_label'.

  return instructions;
}

json IRGenerator::Generate(ForLoopAST &ast) {
  json instruction = json::array({});

  // 1. Setup unique labels and iteration variable
  std::string iter_var = ast.GetIterationVariableName();
  std::string loop_start_label = NewTempVar() + "_for_test";
  std::string loop_body_label = NewTempVar() + "_for_body";
  std::string loop_exit_label = NewTempVar() + "_for_exit";

  RangeAST &range = ast.GetRange();

  // --- 2. Initialization (i = start_value) ---
  // Generate IR for the start expression (e.g., 0)
  json start_ir = Generate(range.GetStart());
  std::string start_value_name = extract_ir_result(start_ir, instruction);

  // Assign the start value to the iteration variable 'i'
  json init_instr = {{"op", "id"}, // Use 'id' (identity copy) for assignment
                     {"dest", iter_var},
                     {"type", "i64"}, // Assuming standard integer type
                     {"args", json::array({start_value_name})}};
  instruction.push_back(init_instr);

  // --- 3. Condition Check Block (Loop Header) ---
  instruction.push_back({{"label", loop_start_label}});

  // a) Generate IR for the end expression (e.g., 10)
  json end_ir = Generate(range.GetEnd());
  std::string end_value_name = extract_ir_result(end_ir, instruction);

  // b) Generate comparison: temp_bool = (i <= end_value_name)
  std::string cond_result_name = NewTempVar();
  json comparison_instr = {{"op", "le"}, // <= operator for range
                           {"dest", cond_result_name},
                           {"type", "bool"},
                           {"args", json::array({iter_var, end_value_name})}};
  instruction.push_back(comparison_instr);

  // c) Conditional Branch: If true, go to body; if false, go to exit
  json branch_instr = {
      {"op", "br"},
      {"labels", {loop_body_label, loop_exit_label}},
      {"args", {cond_result_name}},
  };
  instruction.push_back(branch_instr);

  // --- 4. Loop Body ---
  instruction.push_back({{"label", loop_body_label}});

  for (auto &elt : ast.GetLoopBody()) {
    auto val = Generate(*elt);

    // Handle 'break' statements (jmp instruction without a fixed label)
    if (val.is_object() && val["op"] == "jmp" && val["labels"].empty()) {
      val["labels"].push_back(loop_exit_label); // Retarget break to loop exit
    } else if (val.is_array()) {
      // Handle breaks nested inside complex statements
      for (auto &v : val) {
        if (v.is_object() && v["op"] == "jmp" && v["labels"].empty()) {
          v["labels"].push_back(loop_exit_label);
        }
      }
    }

    // Push instructions
    if (val.is_array()) {
      for (auto &v : val)
        instruction.push_back(v);
    } else {
      instruction.push_back(val);
    }
  }

  // --- 5. Increment (i = i + 1) ---
  std::string one_const = NewTempVar();

  // a) Generate constant '1'
  instruction.push_back(
      {{"op", "const"}, {"dest", one_const}, {"type", "i64"}, {"value", 1}});

  // b) Generate add: temp_add = i + 1
  std::string temp_add = NewTempVar();
  instruction.push_back({{"op", "add"},
                         {"dest", temp_add},
                         {"type", "i64"},
                         {"args", json::array({iter_var, one_const})}});

  // c) Assign back: i = temp_add
  instruction.push_back({{"op", "id"},
                         {"dest", iter_var},
                         {"type", "i64"},
                         {"args", json::array({temp_add})}});

  // 6. Unconditional Jump back to the condition check
  instruction.push_back({{"op", "jmp"}, {"labels", {loop_start_label}}});

  // 7. Exit Label
  instruction.push_back({{"label", loop_exit_label}});

  return instruction;
}

json IRGenerator::Generate([[maybe_unused]] FunctionDefinitionAST &ast) {
  json instruction;

  return instruction;
}

json IRGenerator::Generate(IfStatementAST &ast) {
  json instruction = json::array({});

  // 1. Generate Unique Labels for the entire structure
  std::string if_body_label = NewTempVar() + "_if_body";
  std::string if_exit_label = NewTempVar() + "_if_exit"; // Overall exit point
  std::string next_test_label =
      NewTempVar() + "_next_check"; // Entry for the else if chain

  // --- A. IF Condition and Branch Setup ---
  ExpressionAST &if_condition = ast.GetIfCondition();

  json condition_val = Generate(if_condition);
  // Use extract_ir_result to handle array/complex conditions and get the result
  // name
  std::string cond_result_name = extract_ir_result(condition_val, instruction);

  // Note: The original condition standardization is now largely handled by
  // extract_ir_result and the BinaryExpressionAST visitor, but the resulting
  // boolean must be in 'cond_result_name'.

  // Conditional Branch: IF True -> if_body_label, IF False -> next_test_label
  json initial_branch_instr = {
      {"op", "br"},
      {"labels", {if_body_label, next_test_label}},
      {"args", {cond_result_name}},
  };
  instruction.push_back(initial_branch_instr);

  // --- B. IF Body Block ---
  instruction.push_back({{"label", if_body_label}});

  // Generate IR for the IF Body statements
  for (const auto &elt : ast.GetIfBody()) {
    auto val = Generate(*elt);
    if (val.is_array()) {
      for (auto &v : val)
        instruction.push_back(v);
    } else {
      instruction.push_back(val);
    }
  }

  // CRITICAL FIX: Jump to the overall exit after the IF body completes
  instruction.push_back({{"op", "jmp"}, {"labels", {if_exit_label}}});

  // --- C. ELSE IF Chain ---
  std::string current_test_label = next_test_label; // Start of the chain

  if (ast.hasElseIf()) {
    auto &elseifVec = ast.GetElseIfStatements();

    for (size_t i = 0; i < elseifVec.size(); ++i) {

      // 1. Place the label for the current check (jump target from the previous
      // block)
      instruction.push_back({{"label", current_test_label}});

      // 2. Generate IR for the current else-if block
      json elseif_ir = Generate(*elseifVec[i]);

      // 3. Define the jump target for the NEXT block (either the next else-if
      // or the else/exit)
      std::string next_chain_label =
          NewTempVar() + "_chain_" + std::to_string(i + 1);

      // 4. FIX LABELS within the generated ELSE IF block (Assuming structure:
      // [br, ..., jmp])

      // Fix the branch's 'false' label (which is index 1 of the 'labels' array)

      for (auto &instr : elseif_ir) {
        if (instr.is_object() && instr["op"] == "br") {
          instr["labels"][1] = next_chain_label;
          break;
        }
      }

      // Fix the exit jump (the last instruction in the array) to point to the
      // overall exit
      if (elseif_ir.is_array() && elseif_ir.back()["op"] == "jmp") {
        elseif_ir.back()["labels"][0] = if_exit_label;
      }

      // 5. Collect all instructions from this else-if block
      for (auto &instr : elseif_ir) {
        instruction.push_back(instr);
      }

      // 6. Update the test label for the next iteration
      current_test_label = next_chain_label;
    }
  }

  // --- D. ELSE Statement / Fallthrough ---
  // Place the label for the ELSE block (or the final fallthrough point)
  instruction.push_back({{"label", current_test_label}});

  if (ast.hasElse()) {
    // Generate IR for the else body
    json else_ir = Generate(ast.GetElseStatement());

    // Collect else body instructions
    if (else_ir.is_array()) {
      for (auto &instr : else_ir) {
        instruction.push_back(instr);
      }
    } else {
      instruction.push_back(else_ir);
    }
    // No JMP needed, execution naturally falls into the exit label.
  }

  // --- E. Overall Exit Label ---
  // All branches (if, else if bodies, and the else body) converge here.
  instruction.push_back({{"label", if_exit_label}});

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
  json jump_instruction = {{"op", "jmp"}, {"labels", {"test"}}};
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

json IRGenerator::Generate(WhileLoopAST &ast) {
  json instruction = json::array({});
  std::string test_label = NewTempVar() + "_while_test";
  std::string body_label = NewTempVar() + "_while_body";
  std::string exit_label = NewTempVar() + "_while_exit";
  // 1. Label for Condition Check (while_test)
  instruction.push_back({{"label", test_label}});

  // 2. Generate IR for the Condition Expression
  ExpressionAST &cond = ast.GetCondition();

  // This call must generate IR instructions for the condition
  json condition_ir = Generate(cond);

  // Assuming complex expressions return an array, collect them.
  // Assuming a helper function extracts result name and collects IR:
  // std::string cond_result_name = extract_ir_result(condition_ir,
  // instruction);

  // Since we don't have the helper, we'll assume Generate(cond) returns the
  // final instruction and we use its destination 'dest' field as the boolean
  // variable name.

  // --- TEMPORARY FIX: Assume 'Generate(cond)' returns the final instruction
  // (single JSON) ---
  if (condition_ir.is_array()) {
    // If it's an array, we need to extract the last instruction
    for (auto &instr : condition_ir) {
      instruction.push_back(instr);
    }
    condition_ir = condition_ir.back();
  }
  std::string cond_result_name = condition_ir["dest"];
  // -----------------------------------------------------------------------------------------

  // 3. Conditional Branch (br): If true, go to body; if false, go to exit
  json branch_instr = {
      {"op", "br"},
      {"labels", {body_label, exit_label}},
      {"args", {cond_result_name}},
  };
  instruction.push_back(branch_instr);

  // 4. Loop Body Label
  instruction.push_back({{"label", body_label}});

  // 5. Generate IR for the Loop Body
  for (auto &elt : ast.GetBody()) {
    auto val = Generate(*elt);
    // Break statement handling (similar to LoopAST)
    if (val.is_object() && val["op"] == "jmp") {
      val["labels"].push_back(exit_label);
    } else if (val.is_array()) {
      // If the body generates an array of instructions, update nested breaks
      for (auto &v : val) {
        if (v.is_object() && v["op"] == "jmp" && v["labels"].is_null()) {
          v["labels"].push_back(exit_label);
        }
      }
    }

    // Push single instruction or array elements
    if (val.is_array()) {
      for (auto &v : val)
        instruction.push_back(v);
    } else {
      instruction.push_back(val);
    }
  }

  // 6. Unconditional Jump back to the condition check
  json jump_instr = {{"op", "jmp"}, {"labels", {test_label}}};
  instruction.push_back(jump_instr);

  // 7. Exit Label
  instruction.push_back({{"label", exit_label}});

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

std::string IRGenerator::NewTempVar() {
  static int tempVarCounter = 0;
  return "tempVar_" + std::to_string(tempVarCounter++);
}

std::string IRGenerator::extract_ir_result(json &result_json,
                                           json &instruction_array) {
  if (result_json.is_array()) {
    for (auto &instr : result_json) {
      instruction_array.push_back(instr);
    }
    return result_json.back()["dest"];
  } else if (result_json["op"] == "id") {
    return result_json["args"][0];
  } else if (result_json["op"] == "const") {
    std::string temp_var = NewTempVar();
    result_json["dest"] = temp_var;
    instruction_array.push_back(result_json);
    return temp_var;
  }
}

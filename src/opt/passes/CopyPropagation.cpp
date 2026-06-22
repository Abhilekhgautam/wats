//
// Created by void on 6/21/26.
//

#include "CopyPropagation.h"

#include <algorithm>
#include <unordered_set>

#include "../CFGBuilder.h"

#include "../analysis/LVN.h"

#include <iostream>

void CopyPropagation::run(std::vector<nlohmann::json>& instrs) {
    auto CFG = CFGBuilder::build(instrs);

    for (auto& block : CFG) {
        const auto& [lvn_environment, lvn_table] = LVN::run(block.instrs);

        std::vector<nlohmann::json> optimized_instrs;
        for (auto& instr : block.instrs) {
            if (instr.contains("args")) {
                if (instr.contains("op")) {
                    const std::string op = instr["op"];
                    if (op == "id" || op == "ret" || op == "br") {
                        auto arg = instr["args"][0];
                        if (lvn_environment.contains(arg)) {
                            const int index = lvn_environment.at(arg);
                            LVNTableValue value = lvn_table.index2Value.at(index);

                            if (value.hasIndex()) {
                                auto index = value.getCorrespondingIndex();
                                const std::string ref_var_name = lvn_table.index2Name.at(index);

                                if (op == "id") {
                                    const nlohmann::json new_instr = {
                                        {"op", "id"},
                                        {"args", {ref_var_name}},
                                        {"dest", instr["dest"]}
                                    };
                                    optimized_instrs.push_back(new_instr);
                                }
                                else if (op == "br") {
                                    const nlohmann::json new_instr = {
                                        {"op", "br"},
                                        {"args", {ref_var_name}},
                                        {"labels", instr["labels"]}
                                    };
                                    optimized_instrs.push_back(new_instr);
                                }
                                else {
                                    const nlohmann::json new_instr = {
                                        {"op", "ret"},
                                        {"args", {ref_var_name}}
                                    };
                                    optimized_instrs.push_back(new_instr);
                                }

                            } else {
                                optimized_instrs.push_back(instr);
                            }
                        } else {
                            optimized_instrs.push_back(instr);
                        }
                    }
                    // Has args, has op but is not id must be some arithmetic operations
                    else {
                        const auto& args = instr["args"].get<std::vector<std::string>>();
                        const std::string dest = instr["dest"];

                        auto lhs = args[0];
                        auto rhs = args[1];

                        auto dest_index = lvn_environment.at(dest);

                        if (lvn_environment.contains(lhs)) {
                            auto lhs_index = lvn_environment.at(lhs);

                            if (lhs_index < dest_index) {
                                if (auto lhs_value = lvn_table.index2Value.at(lhs_index); lhs_value.hasIndex()) {
                                    auto new_lhs_index = lhs_value.getCorrespondingIndex();
                                    if (new_lhs_index < lhs_index) {
                                        lhs = lvn_table.index2Name.at(new_lhs_index);
                                    }
                                }
                                else if (lhs_value.op == "id") {
                                    auto possible_lhs = std::get<std::string>(lhs_value.lhs.value());
                                    if (lvn_environment.contains(possible_lhs)) {
                                        auto possible_rhs_index = lvn_environment.at(possible_lhs);
                                        if (possible_rhs_index < lhs_index) {
                                            lhs = possible_lhs;
                                        }
                                    }
                                }
                            }
                        }

                        if (lvn_environment.contains(rhs)) {
                            auto rhs_index = lvn_environment.at(rhs);

                            if (rhs_index < dest_index) {
                                auto rhs_value = lvn_table.index2Value.at(rhs_index);

                                if (rhs_value.hasIndex()) {
                                    auto new_rhs_index = rhs_value.getCorrespondingIndex();
                                    if (new_rhs_index < rhs_index) {
                                        rhs = lvn_table.index2Name.at(new_rhs_index);
                                    }
                                }
                                else if (rhs_value.op == "id") {
                                    auto possible_rhs = std::get<std::string>(rhs_value.lhs.value());
                                    if (lvn_environment.contains(possible_rhs)) {
                                        auto possible_rhs_index = lvn_environment.at(possible_rhs);
                                        if (possible_rhs_index < rhs_index) {
                                            rhs = possible_rhs;
                                        }
                                    }
                                }
                            }
                        }

                        instr["args"] = nlohmann::json::array({lhs, rhs});
                        optimized_instrs.push_back(instr);
                    }
                }
                else {
                    optimized_instrs.push_back(instr);
                }
            }
            else {
                optimized_instrs.push_back(instr);
            }
        }
        block.instrs = optimized_instrs;
    }
    std::vector<nlohmann::json> output;

    for (auto& block : CFG) {
        output.push_back({{
      "label", block.block_name}}
      );
        output.insert(output.end(), block.instrs.begin(), block.instrs.end());
    }

    instrs = output;
}

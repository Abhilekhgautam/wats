//
// Created by void on 6/18/26.
//

#include <ranges>
#include <nlohmann/json.hpp>

#include <set>

#include "../CFGBuilder.h"
#include "dce.h"

#include <algorithm>

std::vector<nlohmann::json> dce(std::vector<nlohmann::json>& instrs) {
    std::vector<nlohmann::json> optimized_instrs;
    std::set<std::string> usedVars;

    // Check for variables that are used in other ops.
    for (const auto& instr : instrs) {
        if (instr.contains("args")) {
            for (const auto& arg : instr["args"]) {
                usedVars.insert(arg);
            }
        }
    }

    // Remove instruction that assigns to unused variable.
    for (const auto& instr : instrs) {
        if (instr.contains("dest") && !usedVars.contains(instr["dest"])) {
            continue;
        }
        optimized_instrs.push_back(instr);
    }

    return optimized_instrs;
}

std::vector<nlohmann::json> dce_until_convergence(std::vector<nlohmann::json>& instrs) {
    auto output = dce(instrs);
    auto len = output.size();

    auto len_changed = true;
    while (len_changed) {
        output = dce(output);
        auto new_len = output.size();

        if (len == new_len) {
            len_changed = false;
        }
        len = new_len;
    }

    return output;
}

std::vector<nlohmann::json> trivial_dce(std::vector<nlohmann::json>& instrs) {
    return dce_until_convergence(instrs);
}

// Local Optimization. Handles scenario where a variable is redefined without previous use.
std::vector<nlohmann::json> dce_per_basic_block(std::vector<nlohmann::json>& instrs) {
    std::vector<nlohmann::json> optimized_instrs;
    std::unordered_map<std::string, nlohmann::json> last_defined;

    bool has_terminators {false};

    for (const auto& instr : instrs) {
        if (instr.contains("args") ) {
            for (const auto& arg : instr["args"]) {
                if (last_defined.contains(arg)) {
                    //std::erase(optimized_instrs, last_defined[arg]);
                    last_defined.erase(arg);
                }
            }

            // Handle br
            if (!instr.contains("dest")) {
                has_terminators = true;
            }
        }

        if (instr.contains("dest") && last_defined.contains(instr["dest"])) {
            std::erase(optimized_instrs, last_defined[instr["dest"]]);
            last_defined.erase(instr["dest"]);
        }

        if (instr.contains("dest")) {
            last_defined[instr["dest"]] = instr;
            optimized_instrs.push_back(instr);
        }

        if (instr.contains("op") && instr["op"] == "jmp") {
            has_terminators = true;
        }
    }

    if (has_terminators) {
        optimized_instrs.push_back(instrs.back());
    }

    return optimized_instrs;
}


std::vector<nlohmann::json> strong_dce(std::vector<nlohmann::json>& instrs) {
    auto dce_output = trivial_dce(instrs);

    auto CFG = CFGBuilder::build(dce_output);

    for (auto& block : CFG) {
        auto optimized_block_instr = dce_per_basic_block(block.instrs);
        auto len = optimized_block_instr.size();

        auto len_changed = true;

        while (len_changed) {
         optimized_block_instr = dce_per_basic_block(optimized_block_instr);
         const auto new_len = optimized_block_instr.size();

         if (new_len == len) {
             len_changed = false;
         }

         len = new_len;
        }
        block.instrs = optimized_block_instr;
    }

    std::vector<nlohmann::json> output;

    for (auto& block : CFG) {
        output.push_back({{
      "label", {block.block_name}}}
      );
        output.insert(output.end(), block.instrs.begin(), block.instrs.end());
    }

    return output;
}

void DCE::run(std::vector<nlohmann::json>& instrs) {
    const auto output = strong_dce(instrs);
    instrs = output;
}

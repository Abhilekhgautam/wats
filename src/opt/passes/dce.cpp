//
// Created by void on 6/18/26.
//

#include <ranges>
#include <nlohmann/json.hpp>

#include <set>

#include "../CFGBuilder.h"

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
        if (instr.contains("dest") && usedVars.contains(instr["dest"])) {
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

        if (len != new_len) {
            len_changed = false;
        }
    }

    return output;
}

std::vector<nlohmann::json> trivial_dce(std::vector<nlohmann::json>& instrs) {
    return dce_until_convergence(instrs);
}

// Local Optimization. Handles scenario where a variable is redefined without previous use.
std::vector<nlohmann::json> dce_per_basic_block(std::vector<nlohmann::json>& instrs) {
    std::vector<nlohmann::json> optimized_instrs;
    std::map<std::string, nlohmann::json> defined_and_unused;

    for (const auto& instr : instrs) {
        if (instr.contains("args") ) {
            if (defined_and_unused.contains(instr["args"])) {
                optimized_instrs.push_back(defined_and_unused[instr["args"]]);
                defined_and_unused.erase(instr["args"]);
            }

            if (!instr.contains("dest")) {
                optimized_instrs.push_back(instr);
            }
        }

        if (instr.contains("dest") && !defined_and_unused.contains(instr["dest"])) {
            defined_and_unused[instr["dest"]] =  instr;
        }
    }

    return optimized_instrs;
}


std::vector<nlohmann::json> strong_dce(std::vector<nlohmann::json>& instrs) {
    auto dce_output = trivial_dce(instrs);

    auto CFG = CFGBuilder::build(dce_output);

    for (auto& block : CFG) {
        const auto optimized_block_instr = dce_per_basic_block(block.instrs);
        block.instrs = optimized_block_instr;
    }

    std::vector<nlohmann::json> output;

    for (const auto& block : CFG) {
        output.insert(output.begin(), block.instrs.begin(), block.instrs.end());
    }

    return output;
}

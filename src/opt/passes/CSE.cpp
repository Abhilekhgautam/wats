//
// Created by void on 6/20/26.
//

#include "CSE.h"

#include "../CFGBuilder.h"
#include "../analysis/LVN.h"

void CSE::run(std::vector<nlohmann::json>& instrs) {
    auto CFG = CFGBuilder::build(instrs);
    for (auto& block: CFG) {
        const auto& [lvn_environment, lvn_table] = LVN::run(block.instrs);

        std::vector<nlohmann::json> optimized_instrs;
        for (auto& instr : block.instrs) {
            if (instr.contains("dest")) {
                auto dest = instr["dest"];
                if (lvn_environment.contains(dest) && !lvn_table.containsName(dest)) {
                    const int index = lvn_environment.at(dest);
                    const std::string canonical_var = lvn_table.index2Name.at(index);

                    const nlohmann::json new_instr = {
                        {"op", "id"},
                        {"args", {canonical_var}},
                            {"dest", dest}
                    };
                    optimized_instrs.push_back(new_instr);
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

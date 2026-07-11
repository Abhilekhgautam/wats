//
// Created by void on 6/23/26.
//

#include "ConstantFolding.h"

#include "../CFGBuilder.h"
#include "../analysis/LVN.h"

std::optional<int> getFoldedValue(const std::string &instr_op, int lhs_val, int rhs_val) {
    if (instr_op == "add") {
        return lhs_val + rhs_val;
    }
    if (instr_op == "sub") {
        return lhs_val - rhs_val;
    }
    if (instr_op == "mul") {
        return lhs_val * rhs_val;
    }
    if (instr_op == "div") {
        return lhs_val / rhs_val;
    }
    if (instr_op == "mod") {
        return lhs_val % rhs_val;
    }
    if (instr_op == "gt") {
        return lhs_val > rhs_val;
    }
    if (instr_op == "lt") {
        return lhs_val < rhs_val;
    }
    if (instr_op == "eq") {
        return (lhs_val == rhs_val);
    }
    if (instr_op == "le") {
        return (lhs_val <= rhs_val);
    }
    if (instr_op == "ge") {
        return (lhs_val >= rhs_val);
    }

    return {};
}

std::optional<int> getConstantFromId(auto &lvn_table, auto &val) {
    if (val.hasIndex()) {
        const auto index = val.getCorrespondingIndex();
        const auto index_value = lvn_table.index2Value.at(index);
        const std::string &op = index_value.op;

        if (op == "const") {
            return index_value.value;
        }
    }
    return {};
}

nlohmann::json createConstOp(const std::string &type, const std::string &dest, const int value) {
    nlohmann::json new_instr;

    if (type == "bool") {
        new_instr = {{"op", "const"},
                     {"dest", dest},
                     {"value", nlohmann::json::boolean_t{static_cast<bool>(value)}},
                     {"type", type}};
    } else {
        new_instr = {{"op", "const"}, {"dest", dest}, {"value", value}, {"type", type}};
    }

    return new_instr;
}

void ConstantFolding::run(std::vector<nlohmann::json> &instrs) {
    auto CFG = CFGBuilder::build(instrs);

    for (auto &block: CFG) {

        bool code_changed;
        do {
            code_changed = false;
            const auto &[lvn_environment, lvn_table] = LVN::run(block.instrs);
            std::vector<nlohmann::json> optimized_instrs;
            for (auto &instr: block.instrs) {
                if (instr.contains("op") && instr.contains("dest") && instr.contains("args") && instr["op"] != "call") {
                    const auto &args = instr["args"].get<std::vector<std::string>>();

                    if (args.size() == 2) {
                        const auto &lhs = args[0];
                        const auto &rhs = args[1];

                        const std::string instr_op = instr["op"];
                        const std::string type = instr["type"];
                        const auto dest_index = lvn_environment.at(instr["dest"]);

                        if (lvn_environment.contains(lhs) && lvn_environment.contains(rhs)) {
                            auto lhs_index = lvn_environment.at(lhs);
                            auto rhs_index = lvn_environment.at(rhs);

                            if (lhs_index < dest_index && rhs_index < dest_index) {
                                auto lhs_index_value = lvn_table.index2Value.at(lhs_index);
                                auto rhs_index_value = lvn_table.index2Value.at(rhs_index);

                                const std::string lhs_op = lhs_index_value.op;
                                const std::string rhs_op = rhs_index_value.op;

                                if (lhs_op == "const" && rhs_op == "const") {

                                    const auto lhs_val = lhs_index_value.value.value();
                                    const auto rhs_val = rhs_index_value.value.value();

                                    auto folded_value = getFoldedValue(instr_op, lhs_val, rhs_val).value();

                                    code_changed = true;
                                    optimized_instrs.push_back(createConstOp(type, instr["dest"], folded_value));
                                } else if (lhs_op == "id" && rhs_op == "id") {
                                    const auto lhs_val = getConstantFromId(lvn_table, lhs_index_value).value();
                                    const auto rhs_val = getConstantFromId(lvn_table, rhs_index_value).value();

                                    auto folded_value = getFoldedValue(instr_op, lhs_val, rhs_val).value();

                                    code_changed = true;
                                    optimized_instrs.push_back(createConstOp(type, instr["dest"], folded_value));
                                } else if (lhs_op == "id" && rhs_op == "const") {
                                    const auto lhs_val = getConstantFromId(lvn_table, lhs_index_value).value();
                                    const auto rhs_val = rhs_index_value.value.value();

                                    int folded_value = getFoldedValue(instr_op, lhs_val, rhs_val).value();

                                    code_changed = true;
                                    optimized_instrs.push_back(createConstOp(type, instr["dest"], folded_value));
                                } else if (lhs_op == "const" && rhs_op == "id") {
                                    const auto lhs_val = lhs_index_value.value.value();
                                    const auto rhs_val = getConstantFromId(lvn_table, rhs_index_value).value();

                                    auto folded_value = getFoldedValue(instr_op, lhs_val, rhs_val).value();

                                    code_changed = true;
                                    optimized_instrs.push_back(createConstOp(type, instr["dest"], folded_value));

                                } else {
                                    optimized_instrs.push_back(instr);
                                }
                            } else {
                                optimized_instrs.push_back(instr);
                            }
                        } else {
                            optimized_instrs.push_back(instr);
                        }
                    } else {
                        optimized_instrs.push_back(instr);
                    }
                } else {
                    optimized_instrs.push_back(instr);
                }
            }

            block.instrs = optimized_instrs;

        } while (code_changed);
    }
    std::vector<nlohmann::json> output;

    for (auto &block: CFG) {
        output.push_back({{"label", block.block_name}});
        output.insert(output.end(), block.instrs.begin(), block.instrs.end());
    }

    instrs = output;
}

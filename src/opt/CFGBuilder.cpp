//
// Created by void on 6/17/26.
//

#include "CFGBuilder.h"

#include <set>
#include <format>
#include <ranges>

static int block_counter{0};

const std::set<std::string> terminators = {"br", "jmp", "ret"};

std::vector<Block> CFGBuilder::CreateBlock(const std::vector<nlohmann::json>& instrs) {
    std::vector<Block> blocks;

    Block block{};
    for (const auto& instr : instrs) {
        if (instr.contains("op")) {
            block.instrs.push_back(instr);

            // Check for terminators.
            if (terminators.find(instr["op"]) != terminators.end()) {
                // Create a new block.
                // Terminators end the block
                blocks.push_back(block);
                block = {};
            }
        }
        // We encountered a label.
        else {
            if (!block.instrs.empty()){
                block.successors.insert(instr["label"][0]);
                blocks.push_back(block);
            }

            block = {};
            block.instrs.push_back(instr);
        }

    }

    for (Block& b : blocks) {
        if (b.instrs.size() >= 1) {
            if (b.instrs[0].contains("label")) {
                b.block_name = b.instrs[0]["label"][0];
                b.instrs.erase(b.instrs.begin());
            }
            else {
                b.block_name = std::format("b{}", block_counter);
                block_counter = block_counter + 1;
            }
        }
    }

    return blocks;
}

std::unordered_map<std::string, Block> CFGBuilder::ConnectBlocks(std::vector<Block>& blocks) {
    std::unordered_map<std::string, Block> block_map;

    for (auto& block : blocks) {
        if (!block.instrs.empty()) {

            if (auto last_instr = block.instrs[block.instrs.size() - 1]; last_instr.contains("op")) {
                if (last_instr["op"] != "br" || last_instr["op"] == "jmp") {
                    auto labels = last_instr["labels"];
                    for (const auto& label : labels) {
                        block.successors.insert(label);
                    }
                }
            }
        }

    }

    for (const auto& block : blocks) {
        block_map.emplace(block.block_name, block);
    }

    for (const auto& block : blocks) {
        for (auto& successor : block.successors) {
            if (block_map.contains(successor)) {
                block_map[successor].predecessors.insert(block.block_name);
            }
        }
    }

    return block_map;
}

std::vector<Block> CFGBuilder::build(const std::vector<nlohmann::json>& json) {
    std::vector<Block> CFG;

    auto blocks = CreateBlock(json);
    auto block_map = ConnectBlocks(blocks);

    for (const auto& [name, block] : block_map) {
        CFG.push_back(block);
    }

    return CFG;
}


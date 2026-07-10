//
// Created by void on 6/17/26.
//

#ifndef WATS_CFGBUILDER_H
#define WATS_CFGBUILDER_H
#include <nlohmann/json.hpp>
#include <set>

struct Block {
    std::string block_name;
    std::vector<nlohmann::json> instrs;
    std::set<std::string> predecessors;
    std::set<std::string> successors;
};

class CFGBuilder {
public:
    static std::vector<Block> build(const std::vector<nlohmann::json> &);

private:
    static std::vector<Block> CreateBlock(const std::vector<nlohmann::json> &);
    static std::map<std::string, Block> ConnectBlocks(std::vector<Block> &);
};


#endif // WATS_CFGBUILDER_H

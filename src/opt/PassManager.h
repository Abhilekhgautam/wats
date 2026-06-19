//
// Created by void on 6/19/26.
//

#ifndef WATS_PASSMANAGER_H
#define WATS_PASSMANAGER_H

#include "nlohmann/json.hpp"
#include "passes/Pass.h"

class PassManager {
public:
    void addPass(std::unique_ptr<Pass> p) {
        passes.push_back(std::move(p));
    }
    void run(std::vector<nlohmann::json>& instrs) const{
        for (const auto& pass : passes) {
            pass->run(instrs);
        }
    }
private:
    std::vector<std::unique_ptr<Pass>> passes;
};


#endif // WATS_PASSMANAGER_H

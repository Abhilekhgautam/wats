//
// Created by void on 6/23/26.
//

#ifndef WATS_CONSTANTFOLDING_H
#define WATS_CONSTANTFOLDING_H

#include "Pass.h"

class ConstantFolding : public Pass {
public:
    std::string name() override { return "const-folding"; }
    void run(std::vector<nlohmann::json> &) override;
};


#endif // WATS_CONSTANTFOLDING_H

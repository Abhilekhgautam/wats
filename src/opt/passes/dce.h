//
// Created by void on 6/19/26.
//

#ifndef WATS_DCE_H
#define WATS_DCE_H
#include "Pass.h"

class DCE : public Pass {
public:
    std::string name() override { return "dce"; }
    void run(std::vector<nlohmann::json> &) override;
};

#endif // WATS_DCE_H

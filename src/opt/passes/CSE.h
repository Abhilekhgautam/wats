//
// Created by void on 6/20/26.
//

#ifndef WATS_CSE_H
#define WATS_CSE_H

#include "Pass.h"

class CSE : public Pass {
public:
    std::string name() override { return "cse"; }
    void run(std::vector<nlohmann::json> &) override;
};


#endif // WATS_CSE_H

//
// Created by void on 6/21/26.
//

#ifndef WATS_COPYPROPAGATION_H
#define WATS_COPYPROPAGATION_H

#include "Pass.h"

class CopyPropagation : public Pass {
public:
    std::string name() override { return "copy-propagation"; }
    void run(std::vector<nlohmann::json> &) override;
};


#endif // WATS_COPYPROPAGATION_H

//
// Created by void on 6/19/26.
//

#ifndef WATS_PASS_H
#define WATS_PASS_H

#include <nlohmann/json.hpp>
#include <string>

class Pass {
public:
    virtual ~Pass() = default;
    virtual std::string name() = 0;
    virtual void run(std::vector<nlohmann::json> &) = 0;
};

#endif // WATS_PASS_H

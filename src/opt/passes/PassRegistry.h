//
// Created by void on 6/19/26.
//

#ifndef WATS_PASSREGISTRY_H
#define WATS_PASSREGISTRY_H

#include <string>

enum struct PassKind {
    DCE,
    CSE,
    CopyProp,
    ConstFolding
};

bool isValidPass(const std::string&);


#endif //WATS_PASSREGISTRY_H
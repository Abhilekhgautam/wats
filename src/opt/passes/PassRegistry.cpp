//
// Created by void on 6/19/26.
//

#include "PassRegistry.h"

#include <string>
#include <unordered_map>

inline std::unordered_map<std::string, PassKind> passTable = {{"dce", PassKind::DCE},
                                                              {"cse", PassKind::CSE},
                                                              {"copy-propagation", PassKind::CopyProp},
                                                              {"const-folding", PassKind::ConstFolding},
                                                              {"simplify-cfg", PassKind::SimplifyCFG}};

bool isValidPass(const std::string &name) { return passTable.contains(name); }

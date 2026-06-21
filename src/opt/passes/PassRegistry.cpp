//
// Created by void on 6/19/26.
//

#include "PassRegistry.h"

#include <unordered_map>
#include <string>

inline std::unordered_map<std::string, PassKind> passTable = {
 {"dce", PassKind::DCE},
    {"cse", PassKind::CSE},
    {"copy-propagation", PassKind::CopyProp}
};

bool isValidPass(const std::string& name) {
  return passTable.contains(name);
}

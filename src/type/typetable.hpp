#ifndef WATS_TYPE_TABLE
#define WATS_TYPE_TABLE 

#include <unordered_map>
#include <optional>

#include "type.hpp"

class TypeTable{
private:
    std::unordered_map<std::string, Type> names;
public:
    TypeTable();
    std::optional<Type> resolve(const std::string&);
};

#endif

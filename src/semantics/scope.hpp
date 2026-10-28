#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <optional>
#include <unordered_map>

#include "../type/type.hpp"

#include "scopeType.hpp"

class Scope {
public:
    Scope(Scope *parent = nullptr, ScopeType type = ScopeType::GLOBAL) : parent(parent), type(type) {}
    Scope(const Scope &) = default;
    Scope &operator=(const Scope &) = default;
    std::optional<Type> FindSymbolInCurrentScope(const std::string &name);
    std::optional<Type> FindSymbol(const std::string &name);

    void AddSymbol(const std::string &name, const Type &type_name);
    void UpdateSymbolTable(const std::string &var_name, const Type &var_type);

    ScopeType GetType() { return type; }
    Scope *GetParent() { return parent; }

private:
    std::unordered_map<std::string, Type> symbol_table;
    Scope *parent;
    ScopeType type;
};

#endif

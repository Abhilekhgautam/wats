#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <functional>
#include <optional>
#include <unordered_map>

#include "../AST/ASTType.hpp"
#include "scopeType.hpp"

class Scope {
public:
    Scope(Scope *parent = nullptr, ScopeType type = ScopeType::GLOBAL) : parent(parent), type(type) {}
    Scope(const Scope &) = default;
    Scope &operator=(const Scope &) = default;
    std::optional<std::reference_wrapper<Type>> FindSymbolInCurrentScope(const std::string &name);
    std::optional<std::reference_wrapper<Type>> FindSymbol(const std::string &name);

    void AddSymbol(const std::string &name, const std::string &type_name);
    void UpdateSymbolTable(const std::string &var_name, const std::string &var_type);

    ScopeType GetType() { return type; }
    Scope *GetParent() { return parent; }

private:
    std::unordered_map<std::string, std::unique_ptr<Type>> symbol_table;
    Scope *parent;
    ScopeType type;
};

#endif

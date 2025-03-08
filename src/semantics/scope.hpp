#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

#include "../AST/ASTType.hpp"
#include "scopeType.hpp"

class Scope{
    public:
    Scope(std::unique_ptr<Scope> parent = nullptr):parent(std::move(parent)), type(ScopeType::GLOBAL){}
    std::optional<std::reference_wrapper<Type>> FindSymbolInCurrentScope(std::string name);
    std::optional<std::reference_wrapper<Type>> FindSymbol(std::string name);

    void AddSymbol(std::string name, std::string type);
    void UpdateSymbolTable(std::string var_name, std::string var_type);
    private:
      std::unordered_map<std::string, std::unique_ptr<Type>> symbol_table;
      std::unique_ptr<Scope> parent;
      ScopeType type;
};

#endif

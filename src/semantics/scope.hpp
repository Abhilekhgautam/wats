#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <memory>
#include <optional>
#include <unordered_map>

#include "../AST/ASTType.hpp"
#include "scopeType.hpp"

class Scope{
    public:
    Scope(std::unique_ptr<Scope> parent = nullptr):parent(std::move(parent)), type(ScopeType::GLOBAL){}
    std::optional<std::unique_ptr<Type>> FindSymbolInCurrentScope(std::string name);
    std::optional<std::unique_ptr<Type>> FindSymbol(std::string name);

    void AddSymbol(std::string name, std::string type);
    private:
      std::unordered_map<std::string, std::unique_ptr<Type>> symbol_table;
      std::unique_ptr<Scope> parent;
      ScopeType type;
};

#endif

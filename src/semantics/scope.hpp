#ifndef SCOPE_HPP
#define SCOPE_HPP

#include <functional>
#include <optional>
#include <unordered_map>

#include "../AST/ASTType.hpp"
#include "scopeType.hpp"

class Scope {
public:
  Scope(Scope *parent = nullptr, ScopeType type = ScopeType::GLOBAL)
      : parent(parent), type(type) {}
  Scope(const Scope &) = default;
  Scope &operator=(const Scope &) = default;
  std::optional<std::reference_wrapper<Type>>
  FindSymbolInCurrentScope(std::string name);
  std::optional<std::reference_wrapper<Type>> FindSymbol(std::string name);

  void AddSymbol(std::string name, std::string type);
  void UpdateSymbolTable(std::string var_name, std::string var_type);

  ScopeType GetType() { return type; }
  Scope *GetParent() { return parent; }

private:
  std::unordered_map<std::string, std::unique_ptr<Type>> symbol_table;
  Scope *parent;
  ScopeType type;
};

#endif

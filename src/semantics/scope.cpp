#include "./scope.hpp"
#include <functional>
#include <memory>

std::optional<std::reference_wrapper<Type>>
Scope::FindSymbolInCurrentScope(std::string name) {
  auto val = symbol_table.find(name);
  if (val == symbol_table.end())
    return {};
  else
    return std::ref(*val->second);
}

std::optional<std::reference_wrapper<Type>>
Scope::FindSymbol(std::string name) {
  auto result = FindSymbolInCurrentScope(name);
  if (result.has_value())
    return FindSymbolInCurrentScope(name).value();

  else if (this->parent == nullptr) {
    return {};
  } else {
    auto temp = this->parent;
    while (temp) {
      auto result = temp->FindSymbolInCurrentScope(name);
      if (result.has_value()) {
        return temp->FindSymbolInCurrentScope(name).value();
      } else {
        temp = temp->parent;
      }
    }
    return {};
  }
}

void Scope::AddSymbol(std::string name, std::string type) {
  symbol_table[name] = TypeFactory::CreateType(type);
}

void Scope::UpdateSymbolTable(std::string var_name,
                              [[maybe_unused]] std::string var_type) {
  symbol_table[var_name] = TypeFactory::CreateType(var_name);
}

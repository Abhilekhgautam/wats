#include "./scope.hpp"
#include <functional>
#include <memory>

std::optional<std::reference_wrapper<Type>> Scope::FindSymbolInCurrentScope(const std::string &name) {
    auto val = symbol_table.find(name);
    if (val == symbol_table.end())
        return {};
    else
        return std::ref(*val->second);
}

std::optional<std::reference_wrapper<Type>> Scope::FindSymbol(const std::string &name) {
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

void Scope::AddSymbol(const std::string &name, const std::string &type_name) {
    symbol_table[name] = TypeFactory::CreateType(type_name);
}

void Scope::UpdateSymbolTable(const std::string &var_name, const std::string &var_type) {
    // Update Local Scope
    if (symbol_table.contains(var_name)) {
        symbol_table[var_name] = TypeFactory::CreateType(var_type);
    }
    // Lookup for the variable in the parent scope and update there.
    auto temp = this->parent;
    while (temp) {
        if (auto result = temp->FindSymbolInCurrentScope(var_name); result.has_value()) {
            temp->symbol_table[var_name] = TypeFactory::CreateType(var_type);
            break;
        } else {
            temp = temp->parent;
        }
    }
}

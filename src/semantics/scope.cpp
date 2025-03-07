#include "./scope.hpp"
#include <memory>

std::optional<std::unique_ptr<Type>> Scope::FindSymbolInCurrentScope(std::string name){
    auto val = symbol_table.find(name);
    if (val == symbol_table.end()) return {};
    else return std::move((val->second));
}

std::optional<std::unique_ptr<Type>> Scope::FindSymbol(std::string name){
    if (FindSymbolInCurrentScope(name).has_value()){
        return FindSymbolInCurrentScope(name).value();
    }
    else if (this->parent == nullptr){
        return {};
    }
    else {
        auto temp = std::move(this->parent);
        while(temp){
            if (temp->FindSymbolInCurrentScope(name).has_value()){
                return temp->FindSymbolInCurrentScope(name).value();
            } else {
                temp = std::move(temp->parent);
            }
        }
        return {};
    }
}

void Scope::AddSymbol(std::string name, std::string type){
    symbol_table.insert(name, type);
}

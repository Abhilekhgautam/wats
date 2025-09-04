#include "VariableDeclarationAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void VariableDeclarationAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json VariableDeclarationAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void VariableDeclarationAST::Debug(){
    std::cout << "Variable Declaration: " << variable_name << '\n';
    std::cout << "Type: " << GetType() << '\n';

}

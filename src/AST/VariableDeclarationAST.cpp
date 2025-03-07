#include "VariableDeclarationAST.hpp"
#include <iostream>

void VariableDeclarationAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void VariableDeclarationAST::Debug(){
    std::cout << "Variable Declaration: " << variable_name;

}

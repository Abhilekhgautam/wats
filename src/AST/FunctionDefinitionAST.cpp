#include "FunctionDefinitionAST.hpp"
#include <iostream>

void FunctionDefinitionAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void FunctionDefinitionAST::Debug(){
    std::cout << "Function: " << fn_name << '\n';
    std::cout << "Function Arguments:\n";
    if (arguments){
       for(const auto& elt: arguments->GetArgs()){
            std::cout << elt << " ";
        }
    }
    std::cout << '\n';
    std::cout << "Function Body:\n";
    for(const auto& elt: function_body){
        elt->Debug();
    }
}

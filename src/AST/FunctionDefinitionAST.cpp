#include "FunctionDefinitionAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void FunctionDefinitionAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json FunctionDefinitionAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void FunctionDefinitionAST::Debug(){
    std::cout << "Function: " << fn_name->GetName() << '\n';
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

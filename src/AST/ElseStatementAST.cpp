#include "ElseStatementAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void ElseStatementAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json ElseStatementAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void ElseStatementAST::Debug(){
     std::cout << "Else Statement: \n";
     std::cout << "Else Body:\n";
     for(const auto& elt: else_body){
         elt->Debug();
     }
 }

#include "IfStatementAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"

#include <iostream>

void IfStatementAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json IfStatementAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void IfStatementAST::Debug(){
     std::cout << "If Statement: \n";
     std::cout << "If Condition: \n";
     condition->Debug();
     std::cout << "\nIf Body:\n\t";
     for(const auto& elt: if_body){
         elt->Debug();
     }
 }

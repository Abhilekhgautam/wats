#include "MatchArmAST.hpp"

#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void MatchArmAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json MatchArmAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void MatchArmAST::Debug(){
     std::cout << "Match Arms: \n";
     std::cout << "Match Body:\n";
     for(const auto& elt: body){
         elt->Debug();
     }
 }

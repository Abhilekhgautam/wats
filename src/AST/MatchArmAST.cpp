#include "MatchArmAST.hpp"
#include <iostream>

void MatchArmAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void MatchArmAST::Debug(){
     std::cout << "Match Arms: \n";
     std::cout << "Match Body:\n";
     for(const auto& elt: body){
         elt->Debug();
     }
 }

#include "IfStatementAST.hpp"

#include <iostream>

void IfStatementAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void IfStatementAST::Debug(){
     std::cout << "If Statement: \n";
     std::cout << "If Body:\n";
     for(const auto& elt: if_body){
         elt->Debug();
     }
 }

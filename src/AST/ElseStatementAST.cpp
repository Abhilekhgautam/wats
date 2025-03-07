#include "ElseStatementAST.hpp"

#include <iostream>

void ElseStatementAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void ElseStatementAST::Debug(){
     std::cout << "Else Statement: \n";
     std::cout << "Else Body:\n";
     for(const auto& elt: else_body){
         elt->Debug();
     }
 }

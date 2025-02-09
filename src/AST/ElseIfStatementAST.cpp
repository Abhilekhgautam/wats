#include "ElseIfStatementAST.hpp"
#include <iostream>

void ElseIfStatementAST::Debug(){
     std::cout << "Else If Statement: \n";
     std::cout << "Else If Body:\n";
     for(const auto& elt: else_if_body){
         elt->Debug();
     }
 }

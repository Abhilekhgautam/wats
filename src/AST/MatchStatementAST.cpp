#include "MatchStatementAST.hpp"

#include <iostream>

void MatchStatementAST::Debug(){
     std::cout << "Match Statement: \n";
     for(const auto& elt: arms){
         elt->Debug();
     }
 }

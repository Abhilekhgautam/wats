#include "MatchArmAST.hpp"
#include <iostream>

void MatchArmAST::Debug(){
     std::cout << "Match Arms: \n";
     std::cout << "Match Body:\n";
     for(const auto& elt: body){
         elt->Debug();
     }
 }

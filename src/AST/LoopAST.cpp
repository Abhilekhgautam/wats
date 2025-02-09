#include "LoopAST.hpp"
#include <iostream>

void LoopAST::Debug(){
   std::cout << "Infinte Loop\n";
   std::cout << "Loop body\n";
   for(auto& elt: loop_body){
       elt->Debug();
   }
}

#include "LoopAST.hpp"
#include <iostream>

void LoopAST::Accept(SemanticAnalyzer& analyzer){
   analyzer.Visit(*this);
}

void LoopAST::Debug(){
   std::cout << "Infinte Loop\n";
   std::cout << "Loop body\n";
   for(auto& elt: loop_body){
       elt->Debug();
   }
}

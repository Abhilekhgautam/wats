#include "WhileLoopAST.hpp"
#include <iostream>

void WhileLoopAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void WhileLoopAST::Debug(){
    std::cout << "While loop\n";
    std::cout << "Loop body\n";
    for(auto& elt: loop_body){
        elt->Debug();
    }
}

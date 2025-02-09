#include "WhileLoopAST.hpp"
#include <iostream>

void WhileLoopAST::Debug(){
    std::cout << "While loop\n";
    std::cout << "Loop body\n";
    for(auto& elt: loop_body){
        elt->Debug();
    }
}

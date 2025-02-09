#include "ForLoopAST.hpp"
#include <iostream>

void ForLoopAST::Debug(){
    std::cout << "For loop\n";
    std::cout << "Iteration Variable: " << var_name << '\n';
    std::cout << "Loop body\n";
    for(const auto& elt: loop_body){
        elt->Debug();
    }
}

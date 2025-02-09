#include "FunctionArgumentAST.hpp"

#include <iostream>

void FunctionArgumentAST::Debug(){
    std::cout << "Function Argument:\n";
    for(auto elt: args){
        std::cout << elt << ' ';
    }
}

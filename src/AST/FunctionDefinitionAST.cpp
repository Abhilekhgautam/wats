#include "FunctionDefinitionAST.hpp"
#include <iostream>

void FunctionDefinitionAST::Debug(){
    std::cout << "Function: " << fn_name << '\n';
    std::cout << "Function Body:\n";
    for(const auto& elt: function_body){
        elt->Debug();
    }
}

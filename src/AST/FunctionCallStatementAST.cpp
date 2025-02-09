#include "FunctionCallStatementAST.hpp"
#include <iostream>

void FunctionCallAST::Debug(){
    std::cout << "Function Called: " << fn_name << '\n';
}

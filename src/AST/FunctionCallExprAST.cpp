#include "FunctionCallExprAST.hpp"
#include <iostream>

void FunctionCallExprAST::Debug(){
    std::cout << "Function Called: " << fn_name << '\n';
}

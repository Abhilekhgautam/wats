#include "FunctionCallExprAST.hpp"
#include <iostream>

void FunctionCallExprAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void FunctionCallExprAST::Debug(){
    std::cout << "Function Called: " << fn_name << '\n';
}

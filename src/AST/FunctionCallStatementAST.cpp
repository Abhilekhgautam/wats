#include "FunctionCallStatementAST.hpp"
#include <iostream>

void FunctionCallAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void FunctionCallAST::Debug(){
    std::cout << "Function Called: " << fn_name << '\n';
}

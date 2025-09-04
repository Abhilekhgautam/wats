#include "FunctionCallExprAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void FunctionCallExprAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json FunctionCallExprAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

int FunctionCallExprAST::GetLength(){
   // +2 for opening and closing paranthesis
   return fn_name.length() + args->GetLength() + 2;
}

void FunctionCallExprAST::Debug(){
    std::cout << "Function Called: " << fn_name << '\n';
}

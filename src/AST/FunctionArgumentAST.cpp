#include "FunctionArgumentAST.hpp"

#include <iostream>

void FunctionArgumentAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void FunctionArgumentAST::Debug(){
    std::cout << "Function Argument:\n";
    for(auto elt: args){
        std::cout << elt << ' ';
    }
}

std::string FunctionArgumentAST::GetArg(){
    if (args.size() >= 1){
        return args[0];
    }
    else return "";
}

std::vector<std::string> FunctionArgumentAST::GetArgs(){
    return args;
}

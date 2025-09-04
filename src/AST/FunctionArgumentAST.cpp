#include "FunctionArgumentAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void FunctionArgumentAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json FunctionArgumentAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void FunctionArgumentAST::Debug(){
    std::cout << "Function Argument:\n";
    for(const auto& elt: args){
        std::cout << elt->GetName() << ' ';
    }
}

std::string FunctionArgumentAST::GetArg(){
    if (args.size() >= 1){
        return args[0]->GetName();
    }
    else return "";
}

std::vector<std::string> FunctionArgumentAST::GetArgs(){
    std::vector<std::string> arg_str;
    for(const auto& elt: args){
       arg_str.push_back(elt->GetName());
    }
    return arg_str;
}

std::vector<std::unique_ptr<IdentifierAST>>& FunctionArgumentAST::GetIds(){return args;}

std::unique_ptr<IdentifierAST>& FunctionArgumentAST::GetId(){return args[0];}

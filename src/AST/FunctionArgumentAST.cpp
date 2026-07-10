#include "FunctionArgumentAST.hpp"
#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void FunctionArgumentAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json FunctionArgumentAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void FunctionArgumentAST::Debug() {
    std::cout << "Function Argument:\n";
    std::cout << GetIdName() << " : " << GetTypeName() << "\n";
}

std::string FunctionArgumentAST::GetIdName() const { return idName; };
std::string FunctionArgumentAST::GetTypeName() const { return typeName; };
std::pair<std::string, std::string> FunctionArgumentAST::GetArg() const {
    return std::make_pair(GetIdName(), GetTypeName());
}

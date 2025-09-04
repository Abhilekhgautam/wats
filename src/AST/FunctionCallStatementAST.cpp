#include "FunctionCallStatementAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>


void FunctionCallAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json FunctionCallAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void FunctionCallAST::Debug(){
    std::cout << "Function Called: " << fn_name->GetName() << '\n';
}

SourceLocation FunctionCallAST::GetSourceLocation(){
    return fn_name->GetSourceLocation()[0];
}

#include "FunctionCallExprAST.hpp"
#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void FunctionCallExprAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json FunctionCallExprAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

int FunctionCallExprAST::GetLength() {
    // +2 for opening and closing paranthesis
    return fn_name->GetName().length() + args->GetLength() + 2;
}

void FunctionCallExprAST::Debug() { std::cout << "Function Called: " << fn_name << '\n'; }

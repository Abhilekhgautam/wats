#include "BreakStatementAST.hpp"

#include <iostream>

void BreakStatementAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void BreakStatementAST::Debug(){
    std::cout << "Break Statement:\n break";
}

#include "RangeExpressionAST.hpp"
#include <iostream>

void RangeAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void RangeAST::Debug(){
    std::cout << "Range: \n";
    start->Debug();
    end->Debug();
}

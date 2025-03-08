#include "BinaryExpressionAST.hpp"
#include <iostream>

void BinaryExpressionAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void BinaryExpressionAST::Debug(){
    std::cout << "Binary Expression\n";
    std::cout << GetType() << '\n';
    std::cout << "LHS: ";
    expr_lhs->Debug();
    std::cout << "RHS: ";
    expr_rhs->Debug();
}

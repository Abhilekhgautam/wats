#include "BinaryExpressionAST.hpp"
#include <iostream>

#include "../IRGenerator/IRGenerator.hpp"

void BinaryExpressionAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json BinaryExpressionAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

int BinaryExpressionAST::GetLength(){
   return expr_lhs->GetLength() + expr_rhs->GetLength() + op.GetOperatorSymbol().length();
}

void BinaryExpressionAST::Debug(){
    std::cout << "Binary Expression\n";
    std::cout << GetType() << '\n';
    std::cout << "LHS: ";
    expr_lhs->Debug();
    std::cout << "RHS: ";
    expr_rhs->Debug();
}

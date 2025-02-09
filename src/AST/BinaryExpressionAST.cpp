#include "BinaryExpressionAST.hpp"
#include <iostream>

void BinaryExpressionAST::Debug(){
    std::cout << "Binary Expression\n";
    std::cout << "LHS: ";
    expr_lhs->Debug();
    expr_rhs->Debug();
}

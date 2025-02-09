#include "RangeExpressionAST.hpp"
#include <iostream>

void RangeAST::Debug(){
    std::cout << "Range: \n";
    start->Debug();
    end->Debug();
}

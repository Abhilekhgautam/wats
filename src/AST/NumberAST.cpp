#include "NumberAST.hpp"
#include <iostream>

void NumberAST::Accept(SemanticAnalyzer& analyzer) {
    analyzer.Visit(*this);
}

void NumberAST::Debug(){
    std::cout << "Number: " << num << '\n';

 }

#include "IdentifierAST.hpp"

#include <iostream>

void IdentifierAST::Accept(SemanticAnalyzer& analyzer) {
    analyzer.Visit(*this);
}

void IdentifierAST::Debug(){
    std::cout << "Identifier \n";
    std::cout << "Name " << name << '\n';
    std::cout << GetType() <<'\n';
}

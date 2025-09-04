#include "IdentifierAST.hpp"

#include <iostream>

#include "../IRGenerator/IRGenerator.hpp"

void IdentifierAST::Accept(SemanticAnalyzer& analyzer) {
    analyzer.Visit(*this);
}

nlohmann::json IdentifierAST::Accept(IRGenerator& generator) {
    return generator.Generate(*this);
}

void IdentifierAST::Debug(){
    std::cout << "Identifier \n";
    std::cout << "Name " << name << '\n';
    std::cout << GetType() <<'\n';
}

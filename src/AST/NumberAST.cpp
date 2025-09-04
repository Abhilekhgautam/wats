#include "NumberAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void NumberAST::Accept(SemanticAnalyzer& analyzer) {
    analyzer.Visit(*this);
}

nlohmann::json NumberAST::Accept(IRGenerator& generator) {
    return generator.Generate(*this);
}

void NumberAST::Debug(){
    std::cout << "Number: " << num << '\n';

 }

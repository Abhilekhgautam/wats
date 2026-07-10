#include "ElseStatementAST.hpp"
#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void ElseStatementAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json ElseStatementAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void ElseStatementAST::Debug() {
    std::cout << "Else Statement: \n";
    std::cout << "Else Body:\n";
    for (const auto &elt: else_body) {
        elt->Debug();
    }
}

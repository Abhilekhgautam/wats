#include "ElseIfStatementAST.hpp"
#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void ElseIfStatementAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json ElseIfStatementAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void ElseIfStatementAST::Debug() {
    std::cout << "Else If Statement: \n";
    std::cout << "Else If Body:\n";
    for (const auto &elt: else_if_body) {
        elt->Debug();
    }
}

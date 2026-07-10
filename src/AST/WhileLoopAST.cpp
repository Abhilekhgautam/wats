#include "WhileLoopAST.hpp"
#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void WhileLoopAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json WhileLoopAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void WhileLoopAST::Debug() {
    std::cout << "While loop\n";
    std::cout << "Loop body\n";
    for (auto &elt: loop_body) {
        elt->Debug();
    }
}

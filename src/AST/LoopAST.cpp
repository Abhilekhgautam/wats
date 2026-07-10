#include "LoopAST.hpp"
#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void LoopAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json LoopAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void LoopAST::Debug() {
    std::cout << "Infinte Loop\n";
    std::cout << "Loop body\n";
    for (auto &elt: loop_body) {
        elt->Debug();
    }
}

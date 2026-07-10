#include "MatchStatementAST.hpp"
#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void MatchStatementAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json MatchStatementAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void MatchStatementAST::Debug() {
    std::cout << "Match Statement: \n";
    for (const auto &elt: arms) {
        elt->Debug();
    }
}

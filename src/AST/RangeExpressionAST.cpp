#include "RangeExpressionAST.hpp"
#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void RangeAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json RangeAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void RangeAST::Debug() {
    std::cout << "Range: \n";
    start->Debug();
    end->Debug();
}

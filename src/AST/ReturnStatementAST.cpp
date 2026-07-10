//
// Created by void on 6/13/26.
//

#include "ReturnStatementAST.h"
#include "../IRGenerator/IRGenerator.hpp"

void ReturnStatementAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json ReturnStatementAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void ReturnStatementAST::Debug() {
    std::cout << "Return\n";
    expr->Debug();
}

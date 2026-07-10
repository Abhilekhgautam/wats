#include "BreakStatementAST.hpp"

#include <iostream>
#include "../IRGenerator/IRGenerator.hpp"

void BreakStatementAST::Accept(SemanticAnalyzer &analyzer) { analyzer.Visit(*this); }

nlohmann::json BreakStatementAST::Accept(IRGenerator &generator) { return generator.Generate(*this); }

void BreakStatementAST::Debug() { std::cout << "Break Statement:\n break"; }

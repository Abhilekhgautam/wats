// created by void at 2026-10-21 20:39:21
#include "StringExpressionAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"

StringExpressionAST::StringExpressionAST(std::string val, SourceLocation loc){
    this->value = val;
    this->loc = loc;
}

void StringExpressionAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json StringExpressionAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void StringExpressionAST::Debug(){

}

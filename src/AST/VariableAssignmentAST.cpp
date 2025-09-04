#include "VariableAssignmentAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void VariableAssignmentAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json VariableAssignmentAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void VariableAssignmentAST::Debug(){
    std::cout << "Variable Assignment: " << variable_name.GetName() << '\n';
    std::cout << "Assigned Value: \n";
    expr->Debug();
}

#include "VariableAssignmentAST.hpp"
#include <iostream>

void VariableAssignmentAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

void VariableAssignmentAST::Debug(){
    std::cout << "Variable Assignment: " << variable_name << '\n';
    std::cout << "Assigned Value: \n";
    expr->Debug();
}

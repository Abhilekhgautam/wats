#include "VariableAssignmentAST.hpp"
#include <iostream>

void VariableAssignmentAST::Debug(){
    std::cout << "Variable Assignment: " << variable_name << '\n';
    std::cout << "Assigned Value: \n";
    expr->Debug();
}

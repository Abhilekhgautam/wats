#include "VariableDeclareAndAssignAST.hpp"

#include <iostream>

void VariableDeclareAndAssignAST::Debug(){
    std::cout << "Variable Declare and Assignment : " << variable_name << '\n';
    std::cout << "Assigned Value: ";
    expr->Debug();
}

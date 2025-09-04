#include "VariableDeclareAndAssignAST.hpp"
#include "../IRGenerator/IRGenerator.hpp"
#include <iostream>

void VariableDeclareAndAssignAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json VariableDeclareAndAssignAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void VariableDeclareAndAssignAST::Debug(){
    std::cout << "Variable Declare and Assignment : " << variable_name << '\n';
    std::cout << "Type:" <<  GetType() << '\n';
    std::cout << "Assigned Value: " << '\n';
    expr->Debug();
}

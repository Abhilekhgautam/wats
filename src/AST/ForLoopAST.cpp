#include "ForLoopAST.hpp"
#include <iostream>

#include "../IRGenerator/IRGenerator.hpp"

void ForLoopAST::Accept(SemanticAnalyzer& analyzer){
    analyzer.Visit(*this);
}

nlohmann::json ForLoopAST::Accept(IRGenerator& generator){
    return generator.Generate(*this);
}

void ForLoopAST::Debug(){
    std::cout << "For loop\n";
    std::cout << "Iteration Variable: " << iter_var->GetName() << '\n';
    std::cout << "Loop body\n";
    for(const auto& elt: loop_body){
        elt->Debug();
    }
}

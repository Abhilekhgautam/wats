#include "IdentifierAST.hpp"

#include <iostream>

void IdentifierAST::Debug(){
    std::cout << "Identifier \n";
    std::cout << "Name " << name << '\n';
}

#ifndef ID_EXPR
#define ID_EXPR

#include "ExpressionAST.hpp"

class IdentifierAST : public ExpressionAST {
    public:
      IdentifierAST(std::string name):
      name(name){}

      virtual ~IdentifierAST() = default;
    private:
    std::string name;

    public:
       void Debug() override;
};

#endif

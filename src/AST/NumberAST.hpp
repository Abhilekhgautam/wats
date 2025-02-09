#ifndef NUM_AST
#define NUM_AST

#include "ExpressionAST.hpp"

class NumberAST : public ExpressionAST {
    public:
       NumberAST(std::string num): num(num){}

       virtual ~NumberAST() = default;
    private:
      std::string num;

      public:
         void Debug() override;
};

#endif

#ifndef EXPR_AST
#define EXPR_AST

#include <memory>

#include "ASTType.hpp"

class ExpressionAST {
  public:
   std::unique_ptr<Type> GetNodeType() { return std::move(type);}
   virtual ~ExpressionAST() = default;
   virtual void Debug() = 0;
  private:
    std::unique_ptr<Type> type;
};

#endif

#ifndef EXPR_AST
#define EXPR_AST

#include "../semantics/semanticAnalyzer.hpp"

class ExpressionAST {
  public:
   virtual std::string GetType() = 0;
   virtual ~ExpressionAST() = default;
   virtual void Accept(SemanticAnalyzer& analyzer) = 0;
   virtual void Debug() = 0;

};

#endif

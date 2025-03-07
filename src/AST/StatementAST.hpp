#ifndef STMT_AST
#define STMT_AST

#include "../semantics/semanticAnalyzer.hpp"

class StatementAST {
 public:
  virtual void Debug() = 0;
  virtual ~StatementAST() = default;

  virtual void Accept(SemanticAnalyzer& analyzer) = 0;

};

#endif

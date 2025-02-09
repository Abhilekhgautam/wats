#ifndef STMT_AST
#define STMT_AST

class StatementAST {
 public:
  virtual void Debug() = 0;
  virtual ~StatementAST() = default;
};

#endif

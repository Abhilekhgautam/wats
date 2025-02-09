#ifndef ELSE_AST
#define ELSE_AST

#include "StatementAST.hpp"

#include <memory>
#include <vector>

class ElseStatementAST : public StatementAST{
 public:
   ElseStatementAST(std::vector<std::unique_ptr<StatementAST>> else_body)
	: else_body(std::move(else_body)){}

   virtual ~ElseStatementAST() = default;
 private:
   std::vector<std::unique_ptr<StatementAST>> else_body;

   public:
      void Debug() override;
};

#endif

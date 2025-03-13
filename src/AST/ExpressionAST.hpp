#ifndef EXPR_AST
#define EXPR_AST

#include "../SourceLocation.hpp"
#include "../semantics/semanticAnalyzer.hpp"

#include <span>

class ExpressionAST {
public:
  virtual std::string GetType() = 0;
  virtual ~ExpressionAST() = default;
  virtual void Accept(SemanticAnalyzer &analyzer) = 0;
  virtual void Debug() = 0;

  virtual std::span<const SourceLocation> GetSourceLocation() = 0;
};

#endif

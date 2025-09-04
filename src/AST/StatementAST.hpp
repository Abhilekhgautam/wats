#ifndef STMT_AST
#define STMT_AST

#include "../semantics/semanticAnalyzer.hpp"
#include "../SourceLocation.hpp"

#include <nlohmann/json.hpp>

class IRGenerator;
class StatementAST {
 public:
  virtual void Debug() = 0;
  virtual ~StatementAST() = default;

  virtual void Accept(SemanticAnalyzer& analyzer) = 0;
  virtual nlohmann::json Accept(IRGenerator& generator) = 0;
  virtual SourceLocation GetSourceLocation() = 0;
};

#endif

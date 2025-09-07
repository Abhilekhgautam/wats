#ifndef ELSE_AST
#define ELSE_AST

#include "StatementAST.hpp"

#include <memory>
#include <vector>

class ElseStatementAST : public StatementAST {
public:
  ElseStatementAST(std::vector<std::unique_ptr<StatementAST>> else_body,
                   SourceLocation loc)
      : else_body(std::move(else_body)), loc(loc) {}

  virtual ~ElseStatementAST() = default;

  std::vector<std::unique_ptr<StatementAST>> &GetBody() { return else_body; }

  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator &generator) override;

private:
  std::vector<std::unique_ptr<StatementAST>> else_body;
  SourceLocation loc;

public:
  void Debug() override;
  SourceLocation GetSourceLocation() override { return loc; }
};

#endif

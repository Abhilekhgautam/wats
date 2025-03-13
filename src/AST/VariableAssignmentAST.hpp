#ifndef VAR_ASSIGN
#define VAR_ASSIGN

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"
#include <memory>
#include <vector>

/// x = 56
class VariableAssignmentAST : public StatementAST {
public:
  VariableAssignmentAST(const std::string variable_name,
                        std::unique_ptr<ExpressionAST> expr,
                        std::vector<SourceLocation> &loc)
      : variable_name(variable_name), expr(std::move(expr)), loc(loc) {}

  virtual ~VariableAssignmentAST() = default;
  void Accept(SemanticAnalyzer &analyzer) override;

private:
  std::string variable_name;
  std::unique_ptr<ExpressionAST> expr;
  std::vector<SourceLocation> loc;

public:
  void Debug() override;
  std::string GetVarName() const { return variable_name; }
  ExpressionAST &GetExpr() const { return *expr; }
  std::vector<SourceLocation> &GetSourceLocation() { return loc; }
};

#endif

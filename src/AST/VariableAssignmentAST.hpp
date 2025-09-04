#ifndef VAR_ASSIGN
#define VAR_ASSIGN

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"
#include <memory>
#include <vector>

#include "IdentifierAST.hpp"


/// x = 56
class VariableAssignmentAST : public StatementAST {
public:
  VariableAssignmentAST(IdentifierAST variable_name,
                        std::unique_ptr<ExpressionAST> expr,
                        std::vector<SourceLocation> &loc)
      : variable_name(variable_name), expr(std::move(expr)), loc(loc[0]) {}

  virtual ~VariableAssignmentAST() = default;
  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator& generator) override;

private:
  IdentifierAST variable_name;
  std::unique_ptr<ExpressionAST> expr;
  SourceLocation loc;

public:
  void Debug() override;
  std::string GetVarName() const { return variable_name.GetName(); }
  ExpressionAST& GetExpr() const { return *expr; }
  SourceLocation GetSourceLocation() override { return loc; }
};

#endif

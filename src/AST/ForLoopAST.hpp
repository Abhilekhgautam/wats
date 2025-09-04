#ifndef FOR_AST
#define FOR_AST

#include "RangeExpressionAST.hpp"
#include "StatementAST.hpp"
#include "IdentifierAST.hpp"
#include <memory>
#include <vector>

/// for i in 1 to 200 {
///     # loop body
/// }
class ForLoopAST : public StatementAST {
public:
  ForLoopAST(std::unique_ptr<IdentifierAST> iter_var, std::unique_ptr<RangeAST> range,
             std::vector<std::unique_ptr<StatementAST>> loop_body, [[maybe_unused]]SourceLocation loc)
      : iter_var(std::move(iter_var)), range(std::move(range)),
        loop_body(std::move(loop_body)) {}

  virtual ~ForLoopAST() = default;

  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator& generator) override;

private:
  std::unique_ptr<IdentifierAST> iter_var;
  std::unique_ptr<RangeAST> range;
  std::vector<std::unique_ptr<StatementAST>> loop_body;
  SourceLocation loc;

public:
  void Debug() override;
  std::string GetIterationVariableName() const { return iter_var->GetName(); }
  std::unique_ptr<IdentifierAST>& GetIterVar() {return iter_var;}
  RangeAST &GetRange() { return *range; }
  std::vector<std::unique_ptr<StatementAST>> &GetLoopBody() {
    return loop_body;
  }
  SourceLocation GetSourceLocation() override {return loc;}
};

#endif

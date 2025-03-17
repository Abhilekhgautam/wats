#ifndef FOR_AST
#define FOR_AST

#include "RangeExpressionAST.hpp"
#include "StatementAST.hpp"
#include <memory>
#include <vector>

/// for i in 1 to 200 {
///     # loop body
/// }
class ForLoopAST : public StatementAST {
public:
  ForLoopAST(std::string iter_var_name, std::unique_ptr<RangeAST> range,
             std::vector<std::unique_ptr<StatementAST>> loop_body)
      : var_name(iter_var_name), range(std::move(range)),
        loop_body(std::move(loop_body)) {}

  virtual ~ForLoopAST() = default;

  void Accept(SemanticAnalyzer &analyzer) override;

private:
  std::string var_name;
  std::unique_ptr<RangeAST> range;
  std::vector<std::unique_ptr<StatementAST>> loop_body;

public:
  void Debug() override;
  std::string GetIterationVariableName() const { return var_name; }
  RangeAST &GetRange() { return *range; }
  std::vector<std::unique_ptr<StatementAST>> &GetLoopBody() {
    return loop_body;
  }
};

#endif

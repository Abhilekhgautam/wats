#ifndef FN_AST
#define FN_AST

#include "FunctionArgumentAST.hpp"
#include "StatementAST.hpp"

#include <memory>
#include <vector>

/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST {
public:
  FunctionDefinitionAST(
      std::string fn_name, std::unique_ptr<FunctionArgumentAST> arguments,
      std::vector<std::unique_ptr<StatementAST>> function_body,
      std::string ret_type)
      : fn_name(fn_name), arguments(std::move(arguments)),
        function_body(std::move(function_body)), ret_type(ret_type) {}

  virtual ~FunctionDefinitionAST() = default;

  void Accept(SemanticAnalyzer &analyzer) override;

private:
  std::string fn_name;
  std::unique_ptr<FunctionArgumentAST> arguments;
  std::vector<std::unique_ptr<StatementAST>> function_body;
  std::string ret_type;

public:
  std::string GetFunctionName() const { return fn_name; }
  std::vector<std::unique_ptr<StatementAST>> &GetFunctionBody() {
    return function_body;
  }
  std::optional<FunctionArgumentAST> GetFunctionArguments() {
    if (!arguments)
      return {};
    else
      return *arguments;
  }
  std::string GetFunctionReturnType() const { return ret_type; }
  void Debug() override;
};
#endif

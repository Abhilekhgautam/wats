#ifndef FN_AST
#define FN_AST

#include "FunctionArgumentAST.hpp"
#include "IdentifierAST.hpp"
#include "StatementAST.hpp"
#include <memory>
#include <vector>

/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST {
public:
  FunctionDefinitionAST(
      std::unique_ptr<IdentifierAST> fn_name,
      std::unique_ptr<FunctionArgumentAST> arguments,
      std::vector<std::unique_ptr<StatementAST>> function_body,
      std::string ret_type, SourceLocation loc)
      : fn_name(std::move(fn_name)), arguments(std::move(arguments)),
        function_body(std::move(function_body)), ret_type(ret_type), loc(loc) {}

  virtual ~FunctionDefinitionAST() = default;

  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator &generator) override;

private:
  std::unique_ptr<IdentifierAST> fn_name;
  std::unique_ptr<FunctionArgumentAST> arguments;
  std::vector<std::unique_ptr<StatementAST>> function_body;
  std::string ret_type;
  SourceLocation loc;

public:
  std::string GetFunctionName() const { return fn_name->GetName(); }
  std::vector<std::unique_ptr<StatementAST>> &GetFunctionBody() {
    return function_body;
  }
  std::optional<std::reference_wrapper<FunctionArgumentAST>>
  GetFunctionArguments() {
    if (!arguments)
      return {};
    else
      return std::ref(*arguments);
  }
  std::string GetFunctionReturnType() const { return ret_type; }
  void Debug() override;
  SourceLocation GetSourceLocation() override { return loc; }
};
#endif

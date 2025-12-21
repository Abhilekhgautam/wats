#ifndef FN_CALL_EXPR
#define FN_CALL_EXPR

#include "ExpressionAST.hpp"

#include "../SourceLocation.hpp"
#include "FunctionParameterAST.hpp"

class FunctionCallExprAST : public ExpressionAST {
public:
  FunctionCallExprAST(std::string fn_name,
                      std::unique_ptr<FunctionParameterAST> args,
                      SourceLocation &loc)
      : fn_name(fn_name), args(std::move(args)), loc(loc) {}

  virtual ~FunctionCallExprAST() = default;
  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator &generator) override;

private:
  std::string fn_name;
  std::unique_ptr<FunctionParameterAST> args;
  SourceLocation loc;
  std::string expr_type;

public:
  std::string GetFunctionName() const { return fn_name; }
  void SetType([[maybe_unused]] std::string type) { this->expr_type = type; }
  void Debug() override;
  std::span<const SourceLocation> GetSourceLocation() override {
    return {&loc, 1};
  }
  std::string GetType() override {
    return expr_type.empty() ? "unknown" : expr_type;
  }

  int GetLength() override;
};

#endif

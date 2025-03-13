#ifndef FUNCTION_PARAM_HPP
#define FUNCTION_PARAM_HPP

#include "ExpressionAST.hpp"
#include <memory>
#include <vector>

class FunctionParameterAST {
public:
  FunctionParameterAST(std::vector<std::unique_ptr<ExpressionAST>> parameters)
      : parameters(std::move(parameters)) {}

  std::vector<std::unique_ptr<ExpressionAST>> &GetFunctionParameters() {
    return parameters;
  };

private:
  std::vector<std::unique_ptr<ExpressionAST>> parameters;
};

#endif

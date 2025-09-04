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

  int GetLength(){
      int length = 0;
      for(const auto& elt: parameters){
          length += elt->GetLength();
      }

      // Also include the commas
      length += parameters.size() - 2;
      return length;
  }
private:
  std::vector<std::unique_ptr<ExpressionAST>> parameters;
};

#endif

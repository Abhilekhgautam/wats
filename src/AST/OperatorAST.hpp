#ifndef OPERATOR_AST_HPP
#define OPERATOR_AST_HPP

#include "../SourceLocation.hpp"
#include <string>

class OperatorNode {
public:
  OperatorNode(std::string symbol, SourceLocation &loc)
      : symbol(symbol), loc(loc) {}

  SourceLocation &GetSourceLocation() { return loc; }
  std::string GetOperatorSymbol() const { return symbol; }

private:
  std::string symbol;
  SourceLocation loc;
};

#endif

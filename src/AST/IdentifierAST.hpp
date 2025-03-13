#ifndef ID_EXPR
#define ID_EXPR

#include "ExpressionAST.hpp"

class IdentifierAST : public ExpressionAST {
public:
  IdentifierAST(std::string name, SourceLocation loc) : name(name), loc(loc) {}

  virtual ~IdentifierAST() = default;

private:
  std::string name;
  std::string type;
  SourceLocation loc;

public:
  void Debug() override;
  void Accept(SemanticAnalyzer &analyzer) override;
  std::string GetName() const { return name; }
  std::string GetType() override { return type; }
  void SetType(std::string type) { this->type = type; }
  std::span<const SourceLocation> GetSourceLocation() override {
    return {&loc, 1};
  }
};

#endif

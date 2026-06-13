#ifndef NUM_AST
#define NUM_AST

#include "ExpressionAST.hpp"

#include <variant>

class NumberAST : public ExpressionAST {
public:
  NumberAST(const std::string num, bool has_decimal, const SourceLocation loc)
      : num(num), has_decimal(has_decimal), loc(loc) {}

  virtual ~NumberAST() = default;

private:
  std::string num;
  bool has_decimal;

  std::variant<long long, double> value;
  SourceLocation loc;

public:
  void Debug() override;
  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator& generator) override;

  std::string GetNumber() const { return num; }
  bool HasDecimal() const { return has_decimal; }
  void SetValue(long long val) { value = val; }
  void SetValue(double val) { value = val; }
  void SetValue(std::variant<long long, double> v) {value = v;}
  std::span<const SourceLocation> GetSourceLocation() override {
    return {&loc, 1};
  }
  int GetLength() override{return num.length();}
  std::variant<long long, double> GetValue() const {return value;}
};

#endif

#ifndef RANGE_EXPR
#define RANGE_EXPR

#include "ExpressionAST.hpp"
#include <vector>

/// 1 to 200
class RangeAST : public ExpressionAST {
public:
  RangeAST(std::unique_ptr<ExpressionAST> start,
           std::unique_ptr<ExpressionAST> end, std::vector<SourceLocation> &loc)
      : start(std::move(start)), end(std::move(end)), loc(loc) {}
  virtual ~RangeAST() = default;

private:
  std::unique_ptr<ExpressionAST> start;
  std::unique_ptr<ExpressionAST> end;
  std::vector<SourceLocation> loc;

public:
  void Debug() override;
  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator& generator) override;

  std::string GetType() override { return "range"; }
  ExpressionAST& GetStart() { return *start; }
  ExpressionAST& GetEnd() { return *end; }
  std::span<const SourceLocation> GetSourceLocation() override { return loc; }

  int GetLength() override{
      // +2 for the keyword to .
      return start->GetLength() + end->GetLength() + 2;
  }
};

#endif

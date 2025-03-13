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
  std::string GetType() override { return "range"; }
  std::span<const SourceLocation> GetSourceLocation() override { return loc; }
};

#endif

#ifndef RANGE_EXPR
#define RANGE_EXPR

#include "ExpressionAST.hpp"

/// 1 to 200
class RangeAST : public ExpressionAST {
    public:
       RangeAST(std::unique_ptr<ExpressionAST> start, std::unique_ptr<ExpressionAST> end)
           : start(std::move(start)), end(std::move(end)){}
       virtual ~RangeAST() = default;
    private:
       std::unique_ptr<ExpressionAST> start;
       std::unique_ptr<ExpressionAST> end;
    public:
      void Debug() override;
      void Accept(SemanticAnalyzer& analyzer) override;
      std::string GetType() override {return "range";}
};

#endif

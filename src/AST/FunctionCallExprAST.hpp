#ifndef FN_CALL_EXPR
#define FN_CALL_EXPR

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"

#include <vector>

class FunctionCallExprAST : public ExpressionAST {
    public:
       FunctionCallExprAST(std::string fn_name, std::vector<std::unique_ptr<StatementAST>> args)
       : fn_name(fn_name), args(std::move(args)){}

       virtual ~FunctionCallExprAST() = default;
    private:
      std::string fn_name;
      std::vector<std::unique_ptr<StatementAST>> args;

    public:
      void Debug() override;
};

#endif

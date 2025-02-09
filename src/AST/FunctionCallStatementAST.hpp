#ifndef FN_CALL
#define FN_CALL

#include "StatementAST.hpp"

#include <string>
#include <memory>
#include <vector>

class FunctionCallAST : public StatementAST {
    public:
       FunctionCallAST(std::string fn_name, std::vector<std::unique_ptr<StatementAST>> args)
       : fn_name(fn_name), args(std::move(args)){}

       virtual ~FunctionCallAST() = default;
    private:
      std::string fn_name;
      std::vector<std::unique_ptr<StatementAST>> args;

    public:
      void Debug() override;
};

#endif

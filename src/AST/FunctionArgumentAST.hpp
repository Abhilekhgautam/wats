#ifndef FN_ARGS
#define FN_ARGS

#include "StatementAST.hpp"
#include <string>
#include <vector>

class FunctionArgumentAST : public StatementAST{
    public:
     FunctionArgumentAST(std::string arg){
         args.push_back(arg);
     }
     FunctionArgumentAST(std::vector<std::string>& args)
     : args(args){}

     virtual ~FunctionArgumentAST() = default;
     void Debug() override;
     std::string GetArg();
     std::vector<std::string> GetArgs();

     void Accept(SemanticAnalyzer& analyzer) override;
    private:
     std::vector<std::string> args;
};

#endif

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
     std::string GetArg(){
        if (args.size() >= 1){
            return args[0];
        }
        else return "";
     }
     std::vector<std::string> GetArgs(){
         return args;
     }
    private:
     std::vector<std::string> args;
};

#endif

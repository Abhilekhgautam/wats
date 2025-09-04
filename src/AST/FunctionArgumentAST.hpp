#ifndef FN_ARGS
#define FN_ARGS

#include "StatementAST.hpp"
#include "IdentifierAST.hpp"
#include <string>
#include <memory>
#include <vector>
#include <cassert>

class FunctionArgumentAST : public StatementAST{
    public:
     FunctionArgumentAST(std::unique_ptr<IdentifierAST> arg){
         args.push_back(std::move(arg));
     }
     FunctionArgumentAST(std::vector<std::unique_ptr<IdentifierAST>>& args)
     : args(std::move(args)){}

     virtual ~FunctionArgumentAST() = default;
     void Debug() override;
     std::string GetArg();
     std::unique_ptr<IdentifierAST>& GetId();
     std::vector<std::string> GetArgs();
     std::vector<std::unique_ptr<IdentifierAST>>& GetIds();

     // Returns locaton to first arg
     SourceLocation GetSourceLocation() override {assert(args.size() > 0);return args[0]->GetSourceLocation().front();}

     void Accept(SemanticAnalyzer& analyzer) override;
     nlohmann::json Accept(IRGenerator& generator) override;

    private:
     std::vector<std::unique_ptr<IdentifierAST>> args;
     SourceLocation loc;
};
#endif

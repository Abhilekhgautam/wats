#ifndef FN_ARGS
#define FN_ARGS

#include "StatementAST.hpp"
#include "IdentifierAST.hpp"
#include <string>


class FunctionArgumentAST : public StatementAST{
    public:
     FunctionArgumentAST(const std::string& id, const std::string& type, const SourceLocation& loc) :
      idName(id), typeName(type), loc(loc){}


     virtual ~FunctionArgumentAST() = default;
     void Debug() override;
     [[nodiscard]]std::string GetIdName() const ;
     [[nodiscard]]std::string GetTypeName() const ;
     [[nodiscard]]std::pair<std::string, std::string> GetArg() const;

     // Returns locaton to first arg
     SourceLocation GetSourceLocation() override {
         return loc;
     }

     void Accept(SemanticAnalyzer& analyzer) override;
     nlohmann::json Accept(IRGenerator& generator) override;

    private:
     std::string idName;
     std::string typeName;
     SourceLocation loc;
};
#endif

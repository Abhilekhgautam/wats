#ifndef FN_ARGS
#define FN_ARGS

#include <string>
#include "StatementAST.hpp"


class FunctionArgumentAST : public StatementAST {
public:
    FunctionArgumentAST(const std::string &id, const std::string &type, const SourceLocation &loc) :
        idName(id), annotated_type(type), loc(loc) {}


    virtual ~FunctionArgumentAST() = default;
    void Debug() override;
    [[nodiscard]] std::pair<std::string, std::string> GetArg() const;

    // Returns locaton to first arg
    SourceLocation GetSourceLocation() override { return loc; }

    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::string idName;
    std::string annotated_type;
    Type typeName;
    SourceLocation loc;
};
#endif

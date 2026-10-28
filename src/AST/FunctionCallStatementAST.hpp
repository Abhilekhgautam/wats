#ifndef FN_CALL
#define FN_CALL

#include "StatementAST.hpp"

#include <memory>
#include <vector>

#include "IdentifierAST.hpp"

class FunctionCallAST : public StatementAST {
public:
    FunctionCallAST(std::unique_ptr<IdentifierAST> fn_name, std::vector<std::unique_ptr<StatementAST>> args) :
        fn_name(std::move(fn_name)), args(std::move(args)) {}

    virtual ~FunctionCallAST() override = default;

    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::unique_ptr<IdentifierAST> fn_name;
    std::vector<std::unique_ptr<StatementAST>> args;

    void Debug() override;
    SourceLocation GetSourceLocation() override;
};

#endif

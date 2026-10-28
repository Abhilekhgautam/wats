#ifndef FN_AST
#define FN_AST

#include <memory>
#include <vector>
#include "FunctionArgumentAST.hpp"
#include "IdentifierAST.hpp"
#include "StatementAST.hpp"

/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST {
public:
    FunctionDefinitionAST(std::unique_ptr<IdentifierAST> fn_name,
                          std::vector<std::unique_ptr<FunctionArgumentAST>> arguments,
                          std::vector<std::unique_ptr<StatementAST>> function_body, std::optional<std::string> ret_type,
                          SourceLocation loc) :
        fn_name(std::move(fn_name)), arguments(std::move(arguments)), function_body(std::move(function_body)),
        annotated_ret_type(ret_type), loc(loc) {}

    virtual ~FunctionDefinitionAST() = default;

    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::unique_ptr<IdentifierAST> fn_name;
    std::vector<std::unique_ptr<FunctionArgumentAST>> arguments;
    std::vector<std::unique_ptr<StatementAST>> function_body;
    std::optional<std::string> annotated_ret_type;
    Type ret_type;
    SourceLocation loc;

    [[nodiscard]] std::string GetFunctionName() const { return fn_name->name; }

    void Debug() override;
    SourceLocation GetSourceLocation() override { return loc; }
    IdentifierAST &GetFunctionNameAsIdentifier() { return *fn_name; }
};
#endif

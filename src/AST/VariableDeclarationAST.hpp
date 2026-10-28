#ifndef VAR_DECL
#define VAR_DECL

#include "../SourceLocation.hpp"
#include "../semantics/semanticAnalyzer.hpp"
#include "StatementAST.hpp"
#include "../type/type.hpp"

/// let x OR
/// let x: i32
class VariableDeclarationAST : public StatementAST {
public:
    VariableDeclarationAST(std::optional<std::string> type_name, const std::string variable_name, std::vector<SourceLocation> loc) :
        annotated_type(type_name), variable_name(variable_name), loc(loc) {}

    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    virtual ~VariableDeclarationAST() = default;

    std::optional<std::string> annotated_type;
    Type type_name;
    std::string variable_name;
    std::vector<SourceLocation> loc;

    SourceLocation &GetVarLocation() { return loc[0]; }
    SourceLocation &GetTypeLocation() { return loc[1]; }
    SourceLocation GetSourceLocation() override { return GetVarLocation(); }

    void Debug() override;
};

#endif

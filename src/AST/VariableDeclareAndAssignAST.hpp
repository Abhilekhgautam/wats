#ifndef VAR_DECL_ASSIGN
#define VAR_DECL_ASSIGN

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"

#include <vector>
#include "../SourceLocation.hpp"

/// let x = 45
class VariableDeclareAndAssignAST : public StatementAST {
public:
    VariableDeclareAndAssignAST(std::string variable_name, std::optional<std::string> type_name, std::unique_ptr<ExpressionAST> expr,
                                std::vector<SourceLocation> &loc) :
        variable_name(variable_name), annotated_type(type_name), expr(std::move(expr)), locations(loc) {}

    virtual ~VariableDeclareAndAssignAST() = default;

    void Accept(SemanticAnalyzer &analyzer) override;
    void Debug() override;
    SourceLocation GetSourceLocation() override { return locations[0]; }
    nlohmann::json Accept(IRGenerator &generator) override;

    std::string variable_name;
    std::optional<std::string> annotated_type;

    Type type_name;
    std::unique_ptr<ExpressionAST> expr;
    std::vector<SourceLocation> locations;

};

#endif

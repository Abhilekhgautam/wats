#ifndef VAR_DECL_ASSIGN
#define VAR_DECL_ASSIGN

#include "StatementAST.hpp"
#include "ExpressionAST.hpp"

/// let x = 45
class VariableDeclareAndAssignAST : public StatementAST{
  public:
    VariableDeclareAndAssignAST(std::string variable_name, std::unique_ptr<Type> type_name, std::unique_ptr<ExpressionAST> expr)
	    : variable_name(variable_name), type_name(std::move(type_name)), expr(std::move(expr)){}

	virtual ~VariableDeclareAndAssignAST() = default;

	void Accept(SemanticAnalyzer& analyzer) override;
  private:
    std::string variable_name;
    std::unique_ptr<Type> type_name;
    std::unique_ptr<ExpressionAST> expr;

    public:
    void Debug() override;
};

#endif

#ifndef VAR_ASSIGN
#define VAR_ASSIGN

#include "ExpressionAST.hpp"
#include "StatementAST.hpp"
#include <memory>

/// x = 56
class VariableAssignmentAST : public StatementAST{
  public:
    VariableAssignmentAST(const std::string variable_name, std::unique_ptr<ExpressionAST> expr):
	    variable_name(variable_name), expr(std::move(expr)){}

	virtual ~VariableAssignmentAST() = default;
	void Accept(SemanticAnalyzer& analyzer) override;

  private:
    std::string variable_name;
    std::unique_ptr<ExpressionAST> expr;

    public:
    void Debug() override;
    std::string GetVariableName() const {return variable_name;}

};

#endif

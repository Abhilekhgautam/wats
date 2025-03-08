#ifndef VAR_DECL_ASSIGN
#define VAR_DECL_ASSIGN

#include "StatementAST.hpp"
#include "ExpressionAST.hpp"

/// let x = 45
class VariableDeclareAndAssignAST : public StatementAST{
  public:
    VariableDeclareAndAssignAST(std::string variable_name, std::string type_name, std::unique_ptr<ExpressionAST> expr)
	    : variable_name(variable_name), type_name(type_name), expr(std::move(expr)){}

	virtual ~VariableDeclareAndAssignAST() = default;

	void Accept(SemanticAnalyzer& analyzer) override;
  private:
    std::string variable_name;
    std::string type_name;
    std::unique_ptr<ExpressionAST> expr;

    public:
    void Debug() override;
    std::string GetVarName() const {return variable_name;}
    std::string GetType() const {return type_name;}
    ExpressionAST& GetExpr() const {return *expr;}

    void SetType(std::string type){type_name = type;}
};

#endif

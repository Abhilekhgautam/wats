#ifndef FN_AST
#define FN_AST

#include "FunctionArgumentAST.hpp"
#include "StatementAST.hpp"
#include "ASTType.hpp"

#include <vector>
#include <memory>


/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST{
  public:
    FunctionDefinitionAST(std::string fn_name, std::unique_ptr<FunctionArgumentAST> arguments, std::vector<std::unique_ptr<StatementAST>> function_body, std::unique_ptr<Type> ret_type)
	    : fn_name(fn_name), arguments(std::move(arguments)), function_body(std::move(function_body)), ret_type(std::move(ret_type)){}

	virtual ~FunctionDefinitionAST() = default;

	void Accept(SemanticAnalyzer& analyzer) override;

  private:
    std::string fn_name;
    std::unique_ptr<FunctionArgumentAST> arguments;
    std::vector<std::unique_ptr<StatementAST>> function_body;
    std::unique_ptr<Type> ret_type;

  public:
    std::string GetFunctionName(){return fn_name;}
    void Debug() override;
};
#endif

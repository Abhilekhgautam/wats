#ifndef FN_AST
#define FN_AST

#include "StatementAST.hpp"
#include "ASTType.hpp"

#include <vector>
#include <memory>


/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST{
  public:
    FunctionDefinitionAST(std::string fn_name, std::vector<std::pair<std::unique_ptr<Type>, std::string>> parameters, std::vector<std::unique_ptr<StatementAST>> function_body, std::unique_ptr<Type> ret_type)
	    : fn_name(fn_name), parameters(std::move(parameters)), function_body(std::move(function_body)), ret_type(std::move(ret_type)){}

	virtual ~FunctionDefinitionAST() = default;
  private:
    std::string fn_name;
    std::vector<std::pair<std::unique_ptr<Type>, std::string>> parameters;
    std::vector<std::unique_ptr<StatementAST>> function_body;
    std::unique_ptr<Type> ret_type;

  public:
    std::string GetFunctionName(){return fn_name;}
    void Debug() override;
};
#endif

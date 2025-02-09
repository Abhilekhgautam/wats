#ifndef VAR_DECL
#define VAR_DECL

#include "StatementAST.hpp"
#include "ASTType.hpp"
#include <memory>
/// let x OR
/// let x: i32
class VariableDeclarationAST : public StatementAST{
   public:
     VariableDeclarationAST(std::unique_ptr<Type> type_name, const std::string variable_name)
	     : type_name(std::move(type_name)), variable_name(variable_name){}

	  virtual ~VariableDeclarationAST() = default;
   private:
     std::unique_ptr<Type> type_name;
     std::string variable_name;

    public:
     std::string GetVarName(){return variable_name;}
     public:
     void Debug() override;
};

#endif

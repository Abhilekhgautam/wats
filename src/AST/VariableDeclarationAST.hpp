#ifndef VAR_DECL
#define VAR_DECL

#include "../semantics/semanticAnalyzer.hpp"
#include "StatementAST.hpp"

/// let x OR
/// let x: i32
class VariableDeclarationAST : public StatementAST{
   public:
     VariableDeclarationAST(std::string type_name, const std::string variable_name)
	     : type_name(std::move(type_name)), variable_name(variable_name){}

	 void Accept(SemanticAnalyzer& analyzer) override;

	 virtual ~VariableDeclarationAST() = default;
   private:
     std::string type_name;
     std::string variable_name;

    public:
     std::string GetVarName() const{return variable_name;}
     std::string GetType() {return type_name;}
     public:
     void Debug() override;
};

#endif

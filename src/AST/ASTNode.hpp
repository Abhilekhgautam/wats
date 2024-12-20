#include <ASTType.hpp>
/*
class ASTNode{
  public:
   virtual void Visit() = 0;
};
*/

class StatementAST {
 // Todo: What can go in??

};

class ExpressionAST {
  public:
   Type GetNodeType() { return type;} 
  private:
    Type type;
};

/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST{
  public:
    FunctionDefinition(std::string_view fn_name, std::vector<std::pair<Type, std::string>> parameters, StatementAST* function_body, Type ret_type)
	    : fn_name(fn_name), parameters(parameters), function_body(function_body), ret_type(ret_type){}
  private:
    std::string fn_name;
    std::vector<std::pair<Type, std::string> parameter;
    StatementAST* function_body;
    Type ret_type;
};

/// 1 to 200
class RangeStatementAST : public StatementAST{
 public:
   RangeStatementAST(ExpressionAST* expr_first, ExpressionAST* expr_second):
	   expr_first(expr_first), expr_seconde(expr_second){}
 private:
   ExpressionAST* expr_first;
   ExpressionAST* expr_second;
};

/// for i in 1 to 200 {
///     # loop body
/// }
class ForLoopAST : public StatementAST{
  public:
   ForLoop(std::string iter_var_name, RangeStatementAST* range, StatementAST* loop_body) 
	   : var_name(iter_var_name), range(range), loop_body(loop_body){}
  private:
   std::string var_name;
   RangeStatementAST* range;
   StatementAST* loop_body;
};

/// while i < 5 {
///     # loop body
/// }
class WhileLoopAST : public StatementAST{
   public:
    WhileLoopAST(const ExpressionAST* condition, StatementAST* loop_body)
	    : condition(condition), loop_body(loop_body){}
   private:
    ExpressionAST* condition;
    StatementAST*  loop_body;
};

/// loop {
///   # loop body
/// }
class LoopAST : public StatementAST{
   public:
    LoopAST(StatementAST* loop_body) 
	    : loop_body(loop_body){}

};

/// let x OR
/// let x: i32
class VariableDeclarationAST : public StatementAST{
   public:
     VariableDeclarationAST(Type type_name, const std::string variable_name)
	     : type_name(type_name), variable_name(variable_name){}
   private:
     Type type_name;
     std::string variable_name;
};

/// x = 56
class VariableAssignmentAST : public StatementAST{
  public:
    VariableAssignmentAST(const std::string variable_name, ExpressionAST* expr):
	    variable_name(variable_name), expr(expr){}
  private:
    std::string variable_name;
    ExpressionAST* AST;
};

/// let x = 45
class VariableDeclareAndAssignAST : public Statement{
  public:
    VariableDeclareAndAssignAST(const std::string variable_name, Type type_name, ExpressionAST* expr)
	    : variable_name(variable_name), type_name(type_name), expr(expr){}
  private:
    std::string variable_name;
    Type type_name;
    ExpressionAST* expr;
};

class BinaryExpressionAST : public Expression {
  public:
    BinaryExpressionAST(ExpressionAST* expr_lhs, ExpressionAST* expr_rhs, std::string operator_symbol)
	    : expr_lhs(expr_lhs), expr_rhs(expr_rhs), operator_symbol(operator_symbol){}
  private:
    ExpressionAST* expr_lhs;
    ExpressionAST* expr_rhs;
    std::string operator_symbol;

};

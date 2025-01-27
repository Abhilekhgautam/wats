#include <vector>
#include "ASTType.hpp"

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
   Type* GetNodeType() { return type;}
  private:
    Type* type;
};

/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST{
  public:
    FunctionDefinitionAST(std::string_view fn_name, std::vector<std::pair<Type*, std::string>> parameters, StatementAST* function_body, Type* ret_type)
	    : fn_name(fn_name), parameters(parameters), function_body(function_body), ret_type(ret_type){}
  private:
    std::string fn_name;
    std::vector<std::pair<Type*, std::string>> parameters;
    StatementAST* function_body;
    Type* ret_type;

  public:
    std::string GetFunctionName(){return fn_name;}
};

/// 1 to 200
class RangeAST : public ExpressionAST {
    public:
       RangeAST(ExpressionAST* start, ExpressionAST* end)
           : start(start), end(end){}
    private:
       ExpressionAST* start;
       ExpressionAST* end;
};

/// for i in 1 to 200 {
///     # loop body
/// }
class ForLoopAST : public StatementAST{
  public:
   ForLoopAST(std::string iter_var_name, RangeAST* range, StatementAST* loop_body)
	   : var_name(iter_var_name), range(range), loop_body(loop_body){}
  private:
   std::string var_name;
   RangeAST* range;
   StatementAST* loop_body;
};

/// while i < 5 {
///     # loop body
/// }
class WhileLoopAST : public StatementAST{
   public:
    WhileLoopAST(ExpressionAST* condition, StatementAST* loop_body)
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
	private:
	 StatementAST* loop_body;

};

/// let x OR
/// let x: i32
class VariableDeclarationAST : public StatementAST{
   public:
     VariableDeclarationAST(Type* type_name, const std::string variable_name)
	     : type_name(type_name), variable_name(variable_name){}
   private:
     Type* type_name;
     std::string variable_name;

    public:
     std::string GetVarName(){return variable_name;}
};

/// x = 56
class VariableAssignmentAST : public StatementAST{
  public:
    VariableAssignmentAST(const std::string variable_name, ExpressionAST* expr):
	    variable_name(variable_name), expr(expr){}
  private:
    std::string variable_name;
    ExpressionAST* expr;
};

/// let x = 45
class VariableDeclareAndAssignAST : public StatementAST{
  public:
    VariableDeclareAndAssignAST(std::string variable_name, Type* type_name, ExpressionAST* expr)
	    : variable_name(variable_name), type_name(type_name), expr(expr){}
  private:
    std::string variable_name;
    Type* type_name;
    ExpressionAST* expr;
};

class IfStatementAST : public StatementAST{
 public:
    IfStatementAST(ExpressionAST* condition, StatementAST* if_body)
       : condition(condition), if_body(if_body){}
 private:
    ExpressionAST* condition;
    StatementAST* if_body;
};

class ElseStatementAST : public StatementAST{
 public:
   ElseStatementAST(StatementAST* else_body)
	: else_body(else_body){}
 private:
   StatementAST* else_body;
};

class ElseIfStatementAST : public StatementAST{
  public:
    ElseIfStatementAST(ExpressionAST* condition, StatementAST* else_if_body)
	: condition(condition), else_if_body(else_if_body){}
  private:
    ExpressionAST* condition;
    StatementAST* else_if_body;
};

class MatchArmAST : public StatementAST{
    public:
       MatchArmAST(ExpressionAST* cond, StatementAST* body)
       : cond(cond), body(body){}
    private:
      ExpressionAST* cond;
      StatementAST* body;
};

class MatchStatementAST : public StatementAST{
  public:
     MatchStatementAST(ExpressionAST* cond, std::vector<MatchArmAST*>& arms)
     : cond(cond), arms(arms){}
  private:
    ExpressionAST* cond;
    std::vector<MatchArmAST*> arms;
};


class BinaryExpressionAST : public ExpressionAST{
  public:
    BinaryExpressionAST(ExpressionAST* expr_lhs, ExpressionAST* expr_rhs, std::string operator_symbol)
	    : expr_lhs(expr_lhs), expr_rhs(expr_rhs), operator_symbol(operator_symbol){}
  private:
    ExpressionAST* expr_lhs;
    ExpressionAST* expr_rhs;
    std::string operator_symbol;

};

class IdentifierAST : public ExpressionAST {
    public:
      IdentifierAST(std::string name):
      name(name){}
    private:
    std::string name;
};

class NumberAST : public ExpressionAST {
    public:
       NumberAST(std::string num): num(num){}
    private:
      std::string num;
};

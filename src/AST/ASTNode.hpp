#include <iostream>
#include <vector>
#include "ASTType.hpp"

/*
class ASTNode{
  public:
   virtual void Visit() = 0;
};
*/

class StatementAST {
 public:
  virtual void Debug() = 0;

};

class ExpressionAST {
  public:
   Type* GetNodeType() { return type;}
   virtual void Debug() = 0;
  private:
    Type* type;
};

/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST{
  public:
    FunctionDefinitionAST(std::string_view fn_name, std::vector<std::pair<Type*, std::string>> parameters, std::vector<StatementAST*> function_body, Type* ret_type)
	    : fn_name(fn_name), parameters(parameters), function_body(function_body), ret_type(ret_type){}
  private:
    std::string fn_name;
    std::vector<std::pair<Type*, std::string>> parameters;
    std::vector<StatementAST*> function_body;
    Type* ret_type;

  public:
    std::string GetFunctionName(){return fn_name;}
    void Debug() override{
        std::cout << "Function: " << fn_name << '\n';
        std::cout << "Function Body:\n";
        for(auto elt: function_body){
            elt->Debug();
        }
    }
};

/// 1 to 200
class RangeAST : public ExpressionAST {
    public:
       RangeAST(ExpressionAST* start, ExpressionAST* end)
           : start(start), end(end){}
    private:
       ExpressionAST* start;
       ExpressionAST* end;
    public:
      void Debug() override {
          std::cout << "Range: \n";
          start->Debug();
          end->Debug();
      }
};

/// for i in 1 to 200 {
///     # loop body
/// }
class ForLoopAST : public StatementAST{
  public:
   ForLoopAST(std::string iter_var_name, RangeAST* range, std::vector<StatementAST*> loop_body)
	   : var_name(iter_var_name), range(range), loop_body(loop_body){}
  private:
   std::string var_name;
   RangeAST* range;
   std::vector<StatementAST*> loop_body;

  public:
   void Debug(){
       std::cout << "For loop\n";
       std::cout << "Iteration Variable: " << var_name << '\n';
       std::cout << "Loop body\n";
       for(auto elt: loop_body){
           elt->Debug();
       }
   }
};

/// while i < 5 {
///     # loop body
/// }
class WhileLoopAST : public StatementAST{
   public:
    WhileLoopAST(ExpressionAST* condition, std::vector<StatementAST*> loop_body)
	    : condition(condition), loop_body(loop_body){}
   private:
    ExpressionAST* condition;
    std::vector<StatementAST*>  loop_body;
   public:
   void Debug(){
       std::cout << "While loop\n";
       std::cout << "Loop body\n";
       for(auto elt: loop_body){
           elt->Debug();
       }
   }
};

/// loop {
///   # loop body
/// }
class LoopAST : public StatementAST{
   public:
    LoopAST(std::vector<StatementAST*> loop_body)
	    : loop_body(loop_body){}
	private:
	 std::vector<StatementAST*> loop_body;
	public:
    void Debug(){
       std::cout << "Infinte Loop\n";
       std::cout << "Loop body\n";
       for(auto elt: loop_body){
           elt->Debug();
       }
    }

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
     public:
     void Debug(){
         std::cout << "Variable Declaration: " << variable_name;

     }
};

/// x = 56
class VariableAssignmentAST : public StatementAST{
  public:
    VariableAssignmentAST(const std::string variable_name, ExpressionAST* expr):
	    variable_name(variable_name), expr(expr){}
  private:
    std::string variable_name;
    ExpressionAST* expr;

    public:
    void Debug(){
        std::cout << "Variable Assignment: " << variable_name << '\n';
        std::cout << "Assigned Value: \n";
        expr->Debug();
    }
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

    public:
    void Debug(){
        std::cout << "Variable Declare and Assignment : " << variable_name << '\n';
        std::cout << "Assigned Value: ";
        expr->Debug();
    }
};

class IfStatementAST : public StatementAST{
 public:
    IfStatementAST(ExpressionAST* condition, std::vector<StatementAST*> if_body)
       : condition(condition), if_body(if_body){}
 private:
    ExpressionAST* condition;
    std::vector<StatementAST*> if_body;

public:
   void Debug(){
        std::cout << "If Statement: \n";
        std::cout << "If Body:\n";
        for(auto elt: if_body){
            elt->Debug();
        }
    }
};

class ElseStatementAST : public StatementAST{
 public:
   ElseStatementAST(std::vector<StatementAST*> else_body)
	: else_body(else_body){}
 private:
   std::vector<StatementAST*> else_body;

   public:
      void Debug(){
           std::cout << "Else Statement: \n";
           std::cout << "Else Body:\n";
           for(auto elt: else_body){
               elt->Debug();
           }
       }
};

class ElseIfStatementAST : public StatementAST{
  public:
    ElseIfStatementAST(ExpressionAST* condition, std::vector<StatementAST*> else_if_body)
	: condition(condition), else_if_body(else_if_body){}
  private:
    ExpressionAST* condition;
    std::vector<StatementAST*> else_if_body;

    public:
       void Debug(){
            std::cout << "Else If Statement: \n";
            std::cout << "Else If Body:\n";
            for(auto elt: else_if_body){
                elt->Debug();
            }
        }
};

class MatchArmAST : public StatementAST{
    public:
       MatchArmAST(ExpressionAST* cond, std::vector<StatementAST*> body)
       : cond(cond), body(body){}
    private:
      ExpressionAST* cond;
      std::vector<StatementAST*> body;
      public:
         void Debug(){
              std::cout << "Match Arms: \n";
              std::cout << "Match Body:\n";
              for(auto elt: body){
                  elt->Debug();
              }
          }
};

class MatchStatementAST : public StatementAST{
  public:
     MatchStatementAST(ExpressionAST* cond, std::vector<MatchArmAST*>& arms)
     : cond(cond), arms(arms){}
  private:
    ExpressionAST* cond;
    std::vector<MatchArmAST*> arms;

    public:
       void Debug(){
            std::cout << "Match Statement: \n";
            for(auto elt: arms){
                elt->Debug();
            }
        }
};


class BinaryExpressionAST : public ExpressionAST{
  public:
    BinaryExpressionAST(ExpressionAST* expr_lhs, ExpressionAST* expr_rhs, std::string operator_symbol)
	    : expr_lhs(expr_lhs), expr_rhs(expr_rhs), operator_symbol(operator_symbol){}
  private:
    ExpressionAST* expr_lhs;
    ExpressionAST* expr_rhs;
    std::string operator_symbol;

    public:
       void Debug(){
            std::cout << "Binary Expression\n";
            std::cout << "LHS: ";
            expr_lhs->Debug();
            expr_rhs->Debug();
        }

};

class IdentifierAST : public ExpressionAST {
    public:
      IdentifierAST(std::string name):
      name(name){}
    private:
    std::string name;

    public:
       void Debug(){
        std::cout << "Identifier \n";
        std::cout << "Name " << name << '\n';
        }
};

class NumberAST : public ExpressionAST {
    public:
       NumberAST(std::string num): num(num){}
    private:
      std::string num;

      public:
         void Debug(){
             std::cout << "Number: " << num << '\n';

          }
};

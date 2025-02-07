#include <iostream>
#include <vector>
#include <memory>
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
  virtual ~StatementAST() = default;
};

class ExpressionAST {
  public:
   Type* GetNodeType() { return type;}
   virtual ~ExpressionAST() = default;
   virtual void Debug() = 0;
  private:
    Type* type;
};

/// function function_name ( para1, para2 ) {
///     # function body
/// }
class FunctionDefinitionAST : public StatementAST{
  public:
    FunctionDefinitionAST(std::string_view fn_name, std::vector<std::pair<std::unique_ptr<Type>, std::string>> parameters, std::vector<std::unique_ptr<StatementAST>> function_body, std::unique_ptr<Type> ret_type)
	    : fn_name(fn_name), parameters(std::move(parameters)), function_body(std::move(function_body)), ret_type(std::move(ret_type)){}

	virtual ~FunctionDefinitionAST() = default;
  private:
    std::string fn_name;
    std::vector<std::pair<std::unique_ptr<Type>, std::string>> parameters;
    std::vector<std::unique_ptr<StatementAST>> function_body;
    std::unique_ptr<Type> ret_type;

  public:
    std::string GetFunctionName(){return fn_name;}
    void Debug() override{
        std::cout << "Function: " << fn_name << '\n';
        std::cout << "Function Body:\n";
        for(const auto& elt: function_body){
            elt->Debug();
        }
    }
};

/// 1 to 200
class RangeAST : public ExpressionAST {
    public:
       RangeAST(std::unique_ptr<ExpressionAST> start, std::unique_ptr<ExpressionAST> end)
           : start(std::move(start)), end(std::move(end)){}
       virtual ~RangeAST() = default;
    private:
       std::unique_ptr<ExpressionAST> start;
       std::unique_ptr<ExpressionAST> end;
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
   ForLoopAST(std::string iter_var_name, std::unique_ptr<RangeAST> range, std::vector<std::unique_ptr<StatementAST>> loop_body)
	   : var_name(iter_var_name), range(std::move(range)), loop_body(std::move(loop_body)){}

	virtual ~ForLoopAST() = default;
  private:
   std::string var_name;
   std::unique_ptr<RangeAST> range;
   std::vector<std::unique_ptr<StatementAST>> loop_body;

  public:
   void Debug() override{
       std::cout << "For loop\n";
       std::cout << "Iteration Variable: " << var_name << '\n';
       std::cout << "Loop body\n";
       for(const auto& elt: loop_body){
           elt->Debug();
       }
   }
};

/// while i < 5 {
///     # loop body
/// }
class WhileLoopAST : public StatementAST{
   public:
    WhileLoopAST(std::unique_ptr<ExpressionAST> condition, std::vector<std::unique_ptr<StatementAST>> loop_body)
	    : condition(std::move(condition)), loop_body(std::move(loop_body)){}

	virtual ~WhileLoopAST() = default;
   private:
    std::unique_ptr<ExpressionAST> condition;
    std::vector<std::unique_ptr<StatementAST>>  loop_body;
   public:
   void Debug() override{
       std::cout << "While loop\n";
       std::cout << "Loop body\n";
       for(auto& elt: loop_body){
           elt->Debug();
       }
   }
};

/// loop {
///   # loop body
/// }
class LoopAST : public StatementAST{
   public:
    LoopAST(std::vector<std::unique_ptr<StatementAST>> loop_body)
	    : loop_body(std::move(loop_body)){}

	virtual ~LoopAST() = default;
	private:
	 std::vector<std::unique_ptr<StatementAST>> loop_body;
	public:
    void Debug() override{
       std::cout << "Infinte Loop\n";
       std::cout << "Loop body\n";
       for(auto& elt: loop_body){
           elt->Debug();
       }
    }

};

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
     void Debug() override{
         std::cout << "Variable Declaration: " << variable_name;

     }
};

/// x = 56
class VariableAssignmentAST : public StatementAST{
  public:
    VariableAssignmentAST(const std::string variable_name, std::unique_ptr<ExpressionAST> expr):
	    variable_name(variable_name), expr(std::move(expr)){}

	virtual ~VariableAssignmentAST() = default;
  private:
    std::string variable_name;
    std::unique_ptr<ExpressionAST> expr;

    public:
    void Debug() override{
        std::cout << "Variable Assignment: " << variable_name << '\n';
        std::cout << "Assigned Value: \n";
        expr->Debug();
    }
};

/// let x = 45
class VariableDeclareAndAssignAST : public StatementAST{
  public:
    VariableDeclareAndAssignAST(std::string variable_name, std::unique_ptr<Type> type_name, std::unique_ptr<ExpressionAST> expr)
	    : variable_name(variable_name), type_name(std::move(type_name)), expr(std::move(expr)){}

	virtual ~VariableDeclareAndAssignAST() = default;
  private:
    std::string variable_name;
    std::unique_ptr<Type> type_name;
    std::unique_ptr<ExpressionAST> expr;

    public:
    void Debug() override{
        std::cout << "Variable Declare and Assignment : " << variable_name << '\n';
        std::cout << "Assigned Value: ";
        expr->Debug();
    }
};

class IfStatementAST : public StatementAST{
 public:
    IfStatementAST(std::unique_ptr<ExpressionAST> condition, std::vector<std::unique_ptr<StatementAST>> if_body)
       : condition(std::move(condition)), if_body(std::move(if_body)){}

    virtual ~IfStatementAST() = default;
 private:
    std::unique_ptr<ExpressionAST> condition;
    std::vector<std::unique_ptr<StatementAST>> if_body;

public:
   void Debug() override{
        std::cout << "If Statement: \n";
        std::cout << "If Body:\n";
        for(const auto& elt: if_body){
            elt->Debug();
        }
    }
};

class ElseStatementAST : public StatementAST{
 public:
   ElseStatementAST(std::vector<std::unique_ptr<StatementAST>> else_body)
	: else_body(std::move(else_body)){}

   virtual ~ElseStatementAST() = default;
 private:
   std::vector<std::unique_ptr<StatementAST>> else_body;

   public:
      void Debug() override{
           std::cout << "Else Statement: \n";
           std::cout << "Else Body:\n";
           for(const auto& elt: else_body){
               elt->Debug();
           }
       }
};

class ElseIfStatementAST : public StatementAST{
  public:
    ElseIfStatementAST(std::unique_ptr<ExpressionAST> condition, std::vector<std::unique_ptr<StatementAST>> else_if_body)
	: condition(std::move(condition)), else_if_body(std::move(else_if_body)){}

	virtual ~ElseIfStatementAST() = default;
   private:
    std::unique_ptr<ExpressionAST> condition;
    std::vector<std::unique_ptr<StatementAST>> else_if_body;

    public:
       void Debug() override{
            std::cout << "Else If Statement: \n";
            std::cout << "Else If Body:\n";
            for(const auto& elt: else_if_body){
                elt->Debug();
            }
        }
};

class MatchArmAST : public StatementAST{
    public:
       MatchArmAST(std::unique_ptr<ExpressionAST> cond, std::vector<std::unique_ptr<StatementAST>> body)
       : cond(std::move(cond)), body(std::move(body)){}

       virtual ~MatchArmAST() = default;
    private:
      std::unique_ptr<ExpressionAST> cond;
      std::vector<std::unique_ptr<StatementAST>> body;
      public:
         void Debug() override{
              std::cout << "Match Arms: \n";
              std::cout << "Match Body:\n";
              for(const auto& elt: body){
                  elt->Debug();
              }
          }
};

class MatchStatementAST : public StatementAST{
  public:
     MatchStatementAST(std::unique_ptr<ExpressionAST> cond, std::vector<std::unique_ptr<MatchArmAST>> arms)
     : cond(std::move(cond)), arms(std::move(arms)){}

     virtual ~MatchStatementAST() = default;
  private:
    std::unique_ptr<ExpressionAST> cond;
    std::vector<std::unique_ptr<MatchArmAST>> arms;

    public:
       void Debug() override{
            std::cout << "Match Statement: \n";
            for(const auto& elt: arms){
                elt->Debug();
            }
        }
};

class BreakStatementAST : public StatementAST{
    public:
     BreakStatementAST() = default;
     virtual ~BreakStatementAST() = default;
    void Debug() override{
        std::cout << "Break Statement:\n break";
    }
};

class FunctionArgumentAST : public StatementAST{
    public:
     FunctionArgumentAST(std::vector<std::string>& args)
     : args(args){}

     virtual ~FunctionArgumentAST() = default;
     void Debug() override {
         std::cout << "Function Argument:\n";
         for(auto elt: args){
             std::cout << elt << ' ';
         }
     }
    private:
     std::vector<std::string> args;
};

class TypeAnnotatedFunctionArgumentAST : public StatementAST{
    public:
     TypeAnnotatedFunctionArgumentAST(std::vector<std::pair<Type*, std::string>> args)
     : args(args){}
    private:
     std::vector<std::pair<Type*, std::string>> args;
};

class BinaryExpressionAST : public ExpressionAST{
  public:
    BinaryExpressionAST(std::unique_ptr<ExpressionAST> expr_lhs, std::unique_ptr<ExpressionAST> expr_rhs, std::string operator_symbol)
	    : expr_lhs(std::move(expr_lhs)), expr_rhs(std::move(expr_rhs)), operator_symbol(operator_symbol){}

	virtual ~BinaryExpressionAST() = default;
  private:
    std::unique_ptr<ExpressionAST> expr_lhs;
    std::unique_ptr<ExpressionAST> expr_rhs;
    std::string operator_symbol;

    public:
       void Debug() override{
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

      virtual ~IdentifierAST() = default;
    private:
    std::string name;

    public:
       void Debug() override{
        std::cout << "Identifier \n";
        std::cout << "Name " << name << '\n';
        }
};

class NumberAST : public ExpressionAST {
    public:
       NumberAST(std::string num): num(num){}

       virtual ~NumberAST() = default;
    private:
      std::string num;

      public:
         void Debug() override{
             std::cout << "Number: " << num << '\n';

          }
};

class FunctionCallAST : public StatementAST {
    public:
       FunctionCallAST(std::string fn_name, std::vector<std::unique_ptr<StatementAST>> args)
       : fn_name(fn_name), args(std::move(args)){}

       virtual ~FunctionCallAST() = default;
    private:
      std::string fn_name;
      std::vector<std::unique_ptr<StatementAST>> args;

    public:
      void Debug() override{
          std::cout << "Function Called: " << fn_name << '\n';
      }
};

class FunctionCallExprAST : public ExpressionAST {
    public:
       FunctionCallExprAST(std::string fn_name, std::vector<std::unique_ptr<StatementAST>> args)
       : fn_name(fn_name), args(std::move(args)){}

       virtual ~FunctionCallExprAST() = default;
    private:
      std::string fn_name;
      std::vector<std::unique_ptr<StatementAST>> args;

    public:
      void Debug() override{
          std::cout << "Function Called: " << fn_name << '\n';
      }
};

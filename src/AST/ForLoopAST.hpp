#ifndef FOR_AST
#define FOR_AST

#include "StatementAST.hpp"
#include <memory>
#include "RangeExpressionAST.hpp"
#include <vector>

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
   void Debug() override;
};

#endif

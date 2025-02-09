#ifndef LOOP_AST
#define LOOP_AST

#include "StatementAST.hpp"
#include <vector>
#include <memory>
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
    void Debug() override;

};
#endif

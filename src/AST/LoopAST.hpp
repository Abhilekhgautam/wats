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
	void Accept(SemanticAnalyzer& analyzer) override;
    nlohmann::json Accept(IRGenerator& generator) override;

	private:
	 std::vector<std::unique_ptr<StatementAST>> loop_body;
	public:
    void Debug() override;
    const std::vector<std::unique_ptr<StatementAST>>& GetLoopBody() const {return loop_body;}
	std::vector<std::unique_ptr<StatementAST>>& GetLoopBody() {return loop_body;}

    SourceLocation GetSourceLocation() override {return {};}
};
#endif

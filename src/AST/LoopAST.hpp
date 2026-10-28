#ifndef LOOP_AST
#define LOOP_AST

#include <memory>
#include <vector>
#include "StatementAST.hpp"
/// loop {
///   # loop body
/// }
class LoopAST : public StatementAST {
public:
    LoopAST(std::vector<std::unique_ptr<StatementAST>> loop_body) : loop_body(std::move(loop_body)) {}

    virtual ~LoopAST() = default;
    void Accept(SemanticAnalyzer &analyzer) override;
    nlohmann::json Accept(IRGenerator &generator) override;

    std::vector<std::unique_ptr<StatementAST>> loop_body;

    void Debug() override;

    SourceLocation GetSourceLocation() override { return {}; }
};
#endif

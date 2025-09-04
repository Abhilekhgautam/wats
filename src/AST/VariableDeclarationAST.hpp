#ifndef VAR_DECL
#define VAR_DECL

#include "../SourceLocation.hpp"
#include "../semantics/semanticAnalyzer.hpp"
#include "StatementAST.hpp"

/// let x OR
/// let x: i32
class VariableDeclarationAST : public StatementAST {
public:
  VariableDeclarationAST(std::string type_name, const std::string variable_name,
                         std::vector<SourceLocation> loc)
      : type_name(std::move(type_name)), variable_name(variable_name),
        loc(loc) {}

  void Accept(SemanticAnalyzer &analyzer) override;
  nlohmann::json Accept(IRGenerator& generator) override;

  virtual ~VariableDeclarationAST() = default;

private:
  std::string type_name;
  std::string variable_name;
  std::vector<SourceLocation> loc;

public:
  std::string GetVarName() const { return variable_name; }
  std::string GetType() { return type_name; }
  std::vector<SourceLocation> &GetLocation() { return loc; }
  SourceLocation &GetVarLocation() { return loc[0]; }
  SourceLocation &GetTypeLocation() { return loc[1]; }
  SourceLocation GetSourceLocation() override {return GetVarLocation();}
public:
  void Debug() override;
};

#endif

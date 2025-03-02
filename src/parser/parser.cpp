#include "parser.hpp"
#include "../AST/FunctionArgumentAST.hpp"
#include "../lexer/tokens.hpp"
#include "../lexer/lexer.hpp"
#include "../utils.hpp"

#include "../AST/NumberAST.hpp"
#include "../AST/IdentifierAST.hpp"
#include "../AST/BinaryExpressionAST.hpp"
#include "../AST/FunctionCallExprAST.hpp"
#include "../AST/MatchStatementAST.hpp"

#include <cmath>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#define GENERATE_POSITION_PAST_ONE_COLUMN                                     \
  token_vec[current_parser_position].GetLine(),                                \
      token_vec[current_parser_position].GetColumn() + 1

#define GENERATE_CURRENT_POSITION \
token_vec[current_parser_position].GetLine(),                                \
    token_vec[current_parser_position].GetColumn()

inline bool Parser::CheckInsideFunction(){
    if (status_list.empty() || status_list.front() != ParserStatus::PARSING_FN_DEFINITION){
        return false;
    }
    return true;
}

void Parser::Parse() {
  auto stmts = ParseStatements();

  if (stmts.has_value()) {
    auto stmt_vec = std::move(stmts.value());
    for (const auto &elt : stmt_vec) {
      elt->Debug();
    }
  }
}

void Parser::DidYouMean(const std::string to_add, std::size_t line,
                        std::size_t column, bool space_before) {

  const std::string invalid_line = source_code_by_line[line - 1];
  const std::string contents_after_error = invalid_line.substr(column - 1);
  const std::string contents_before_error = invalid_line.substr(0, column - 1);

  std::string expected_correct_line;

  if (!space_before){
      expected_correct_line =
      contents_before_error + to_add + " " + contents_after_error;
  } else {
    expected_correct_line =
      contents_before_error + " " + to_add + contents_after_error;
  }
  Color("blue", "Did You Mean?", true);

  std::cout << expected_correct_line << '\n';
  if (!space_before)
    Color("green", SetPlus(column, to_add.length()), true);
  else
    Color("green", SetPlus(column + 1, to_add.length()), true);

}

void Parser::Expected(const std::string str, std::size_t line,
                      std::size_t column) {

  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << source_code_by_line[line - 1] << '\n';
  Color("green", SetArrow(column), true);
}

void Parser::Unexpected(const std::string str, std::size_t line,
                      std::size_t column, std::size_t times) {

  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << source_code_by_line[line - 1] << '\n';
  if (!times){
      Color("red", SetArrow(column - GetCurrentToken().GetValue().length() + 1, GetCurrentToken().GetValue().length()), true);
  } else {
    Color("red", SetArrow(column - times + 1, times), true);
  }
}

bool Parser::Peek(TokenName tok) {
  if (current_parser_position + 1 >= token_vec.size())
    return false;
  else if (token_vec.empty())
    return false;

  return token_vec.at(current_parser_position + 1).GetTokenName() == tok;
}

Token Parser::GetCurrentToken() { return token_vec[current_parser_position]; }

void Parser::StoreParserPosition() {
  temp_parser_position = current_parser_position;
}

void Parser::ConsumeNext() {
  current_parser_position = current_parser_position + 1;
}

void Parser::BackTrack() { current_parser_position = temp_parser_position; }

bool Parser::IsAtEnd() { return current_parser_position >= token_vec.size(); }

std::optional<std::unique_ptr<FunctionDefinitionAST>>
Parser::ParseFunctionWithRetType() {
  StoreParserPosition();
  // later when we add fn args..
  [[maybe_unused]] int parenthesis_position;

  if (Peek(TokenName::FUNCTION)){
    ConsumeNext();
    if (CheckInsideFunction()){
        Unexpected("You cannot define a function inside another function", GENERATE_CURRENT_POSITION);
        return {};
    }
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION);
  }
  else
    return {};

  std::string fn_name;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    fn_name = GetCurrentToken().GetValue();
  } else {
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) != Lexer::keywords.end()){
        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
        Unexpected("'" + GetCurrentToken().GetValue() + "'" + " is a keyword, it cannot be used as a function name.", GENERATE_CURRENT_POSITION);
        return {};
    }

    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
    Expected(
        "Expected an Identifier (Function Name) after the 'function' keyword",
        GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::OPEN_PARENTHESIS))
    ConsumeNext();
  else {
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
    Expected("Expected a '('", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("(", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  auto fn_args = ParseFunctionArguments();

  if (Peek(TokenName::CLOSE_PARENTHESIS))
    ConsumeNext();
  else {
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
    Expected("Expected a ')'", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(")", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  // fixme: add other types later.
  if (Peek(TokenName::I32)) {
    // todo: store the type
  }

  auto result = ParseCurlyBraceAndBody();
  if (result.has_value()) {
    auto val = fn_args.has_value() ? std::make_unique<FunctionDefinitionAST>(fn_name, std::move(fn_args.value()),std::move(result.value()), nullptr)
    :  std::make_unique<FunctionDefinitionAST>(fn_name, nullptr, std::move(result.value()), nullptr);

    status_list.clear();
    return val;
  } else {
    return {};
  }
}

std::optional<std::unique_ptr<FunctionDefinitionAST>>
Parser::ParseFunctionWithoutRetType() {
  StoreParserPosition();
  // later when we add fn args..
  [[maybe_unused]] int parenthesis_position;

  if (Peek(TokenName::FUNCTION)) {
    ConsumeNext();
    if (CheckInsideFunction()){
        Unexpected("You cannot define a function inside another function", GENERATE_CURRENT_POSITION);
        return {};
    }
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION);

  }
  else{
    return {};
  }
  std::string fn_name;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    fn_name = GetCurrentToken().GetValue();
  } else {
      ConsumeNext();
      if (Lexer::keywords.find(GetCurrentToken().GetValue()) != Lexer::keywords.end()){
          status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
          Unexpected("'" + GetCurrentToken().GetValue() + "'" + " is a keyword, it cannot be used as a function name.", GENERATE_CURRENT_POSITION);
          return {};
      }
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

    Expected(
        "Expected an Identifier (Function Name) after the 'function' keyword",
        GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::OPEN_PARENTHESIS))
    ConsumeNext();
  else {
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

    Expected("Expected a '('", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("(", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  auto fn_args = ParseFunctionArguments();

  if (Peek(TokenName::CLOSE_PARENTHESIS))
    ConsumeNext();
  else {
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

    Expected("Expected a ')'", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(")", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  auto result = ParseCurlyBraceAndBody();
  if (result.has_value()) {

    auto val = fn_args.has_value() ? std::make_unique<FunctionDefinitionAST>(fn_name, std::move(fn_args.value()),std::move(result.value()), nullptr)
                        : std::make_unique<FunctionDefinitionAST>(fn_name, nullptr,std::move(result.value()), nullptr);


    status_list.clear();

    return val;
  } else {

      return {};
  }
}

std::optional<std::unique_ptr<FunctionArgumentAST>> Parser::ParseFunctionArgument(){
    if (Peek(TokenName::ID)){
        ConsumeNext();
        return std::make_unique<FunctionArgumentAST>(GetCurrentToken().GetValue());
    } else return {};
}

std::optional<std::unique_ptr<FunctionArgumentAST>> Parser::ParseFunctionArguments(){
    auto argument = ParseFunctionArgument();

    if (!argument.has_value()){
        return {};
    }

    std::vector<std::string> str_args;
    str_args.push_back(argument.value()->GetArg());

    if(Peek(TokenName::COMMA)) ConsumeNext();
    else{
        return std::make_unique<FunctionArgumentAST>(str_args);
    }

    auto arguments = ParseFunctionArguments();

    if (!arguments.has_value()){
        Expected("Unexpected ',' found", GENERATE_POSITION_PAST_ONE_COLUMN);
        return {};
    }

    for(const auto& elt: arguments.value()->GetArgs()){
        str_args.push_back(elt);
    }

    return std::make_unique<FunctionArgumentAST>(str_args);
}

std::optional<std::unique_ptr<FunctionDefinitionAST>> Parser::ParseFunction() {
  StoreParserPosition();

  auto resultRetType = ParseFunctionWithRetType();
  if (resultRetType.has_value()) {
    return resultRetType;
  }

  BackTrack();

  auto result = ParseFunctionWithoutRetType();
  if ((result.has_value())) {
    return result;
  }

  return {};
}
// Function Parsing Ends here

// Parsing Variable Declaration Begins here

std::optional<std::unique_ptr<VariableDeclarationAST>>
Parser::ParseVariableDeclWithLet() {
  StoreParserPosition();

  if (Peek(TokenName::LET)) {
    ConsumeNext();
  } else
    return {};

  std::string var_name;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
  } else {
      ConsumeNext();
      if (Lexer::keywords.find(GetCurrentToken().GetValue()) != Lexer::keywords.end()){
          status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
          Unexpected("'" + GetCurrentToken().GetValue() + "'" + " is a keyword, it cannot be used as a variable name.", GENERATE_CURRENT_POSITION);
          return {};
      }
    Expected("A Variable name after a let expression is required",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::SEMI_COLON))
    ConsumeNext();
  else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
    return {};
  }

  return std::make_unique<VariableDeclarationAST>(nullptr, var_name);
}

std::optional<std::unique_ptr<VariableDeclarationAST>>
Parser::ParseVariableDeclWithType() {
  StoreParserPosition();

  if (Peek(TokenName::LET)) {
    ConsumeNext();
  } else
    return {};

  std::string var_name;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
  } else {
      ConsumeNext();
      if (Lexer::keywords.find(GetCurrentToken().GetValue()) != Lexer::keywords.end()){
          status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
          Unexpected("'" + GetCurrentToken().GetValue() + "'" + " is a keyword, it cannot be used as a variable name.", GENERATE_CURRENT_POSITION);
          return {};
      }
    Expected("A Variable name after a let expression is required",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::COLON))
    ConsumeNext();
  else
    return {};

  // FixME: add support for user defined type
  // later when support for classes are added.
  std::unique_ptr<Type> type;
  if (Peek(TokenName::I32) || Peek(TokenName::I64) || Peek(TokenName::F32) ||
      Peek(TokenName::F64)) {
    ConsumeNext();
    type = std::make_unique<PrimitiveType>(GetCurrentToken().GetValue());
  } else {
    Expected("Consider mentioning the type of the variable", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::SEMI_COLON))
    ConsumeNext();
  else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
    return {};
  }

  return std::make_unique<VariableDeclarationAST>(std::move(type), var_name);
}

std::optional<std::unique_ptr<VariableDeclarationAST>>
Parser::ParseVariableDecl() {
  auto result = ParseVariableDeclWithType();

  if (result.has_value()) {
    std::cout << result.value()->GetVarName() << '\n';
    return result;
  }

  BackTrack();
  result = ParseVariableDeclWithLet();

  if (result.has_value()) {
    std::cout << result.value()->GetVarName() << '\n';
    return result;
  }

  return {};
}

std::optional<std::unique_ptr<VariableAssignmentAST>>
Parser::ParseVariableAssignment() {
  StoreParserPosition();

  std::string var_name;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
  } else
    return {};

  if (Peek(TokenName::ASSIGN))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (!expr.has_value()) {
    Expected("Expected an value or expression for the assignment",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
    return std::make_unique<VariableAssignmentAST>(var_name,
                                                   std::move(expr.value()));
  } else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
    return {};
  }
}

std::optional<std::unique_ptr<VariableDeclareAndAssignAST>>
Parser::ParseVariableInitWithLet() {
  StoreParserPosition();

  if (Peek(TokenName::LET))
    ConsumeNext();
  else
    return {};

  std::string var_name;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
  } else {
    StoreParserPosition();
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) != Lexer::keywords.end()){
        Unexpected("'" + GetCurrentToken().GetValue() + "'" + " is a keyword, it cannot be used as a variable name.", GENERATE_CURRENT_POSITION);
        return {};
    }
  Expected("A Variable name after a let expression is required",
           GENERATE_POSITION_PAST_ONE_COLUMN);
  return {};
  }
  BackTrack();
  if (Peek(TokenName::ASSIGN))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (!expr.has_value()) {
    Expected("Expected an value or expression for the assignment",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
  } else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
    return {};
  }

  return std::make_unique<VariableDeclareAndAssignAST>(var_name, nullptr,
                                                       std::move(expr.value()));
}

std::optional<std::unique_ptr<VariableDeclareAndAssignAST>>
Parser::ParseVariableInitWithType() {
  StoreParserPosition();

  if (Peek(TokenName::LET))
    ConsumeNext();
  else
    return {};

  std::string var_name;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
  } else {
      StoreParserPosition();
      ConsumeNext();
      if (Lexer::keywords.find(GetCurrentToken().GetValue()) != Lexer::keywords.end()){
          Unexpected("'" + GetCurrentToken().GetValue() + "'" + " is a keyword, it cannot be used as a variable name.", GENERATE_CURRENT_POSITION);
          return {};
      }
    Expected("A Variable name after a let expression is required",
             GENERATE_POSITION_PAST_ONE_COLUMN);
  }

  if (Peek(TokenName::COLON))
    ConsumeNext();
  else
    return {};

  std::unique_ptr<Type> type;
  // FixME: Add support for user defined types
  // later when support for classes are added
  if (Peek(TokenName::I32) || Peek(TokenName::I64) || Peek(TokenName::F32) ||
      Peek(TokenName::F64)) {
    ConsumeNext();
    type = std::make_unique<PrimitiveType>(GetCurrentToken().GetValue());
  } else {
    Expected("Consider mentioning the type of the variable", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::ASSIGN))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (!expr.has_value()) {
    Expected("Expected an value or expression for the assignment",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
  } else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
    return {};
  }

  return std::make_unique<VariableDeclareAndAssignAST>(
      var_name, std::move(type), std::move(expr.value()));
}

// Parsing Variable Declaration Ends here

std::optional<std::unique_ptr<ExpressionAST>>
Parser::ParseExpressionBeginningWithID() {
  StoreParserPosition();

  std::string identifier_name;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    identifier_name = GetCurrentToken().GetValue();
  } else
    return {};

  std::unique_ptr<ExpressionAST> identifier_expr =
      std::make_unique<IdentifierAST>(identifier_name);
  // later if identifier is a function call.
  std::vector<std::unique_ptr<StatementAST>> args;

  if (Peek(TokenName::OPEN_PARENTHESIS)) {
    ConsumeNext();

    // Todo: look for parameter later

    if (Peek(TokenName::CLOSE_PARENTHESIS)) {
      ConsumeNext();
      identifier_expr =
          std::make_unique<FunctionCallExprAST>(identifier_name, std::move(args));
    } else {
      Expected("Expected a closing pair ')'", GENERATE_POSITION_PAST_ONE_COLUMN);
      return {};
    }
  } else {
    identifier_expr = std::make_unique<IdentifierAST>(identifier_name);
  }
  auto expr = ParseSubExpression();

  if (expr.has_value()) {
    auto value = std::move(expr.value());
    return std::make_unique<BinaryExpressionAST>(
        std::move(identifier_expr), std::move(value.first), std::move(value.second));
  } else
    return identifier_expr;
}

std::optional<std::unique_ptr<ExpressionAST>>
Parser::ParseExpressionBeginningWithNumber() {
  StoreParserPosition();

  std::string num;
  if (Peek(TokenName::NUMBER)) {
    ConsumeNext();
    num = GetCurrentToken().GetValue();
  } else
    return {};

  auto number_expr = std::make_unique<NumberAST>(num);

  auto expr = ParseSubExpression();

  if (expr.has_value()) {
    auto val = std::move(expr.value());
    return std::make_unique<BinaryExpressionAST>(
        std::move(number_expr), std::move(val).first, val.second);
  } else
    return std::make_unique<NumberAST>(num);
}

std::optional<std::unique_ptr<ExpressionAST>>
Parser::ParseExpressionBeginningWithBraces() {
  StoreParserPosition();
  [[maybe_unused]] auto parenthesis_position = current_parser_position;

  if (Peek(TokenName::OPEN_PARENTHESIS)) {
    ConsumeNext();
    parenthesis_position = current_parser_position;
  } else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    if (Peek(TokenName::CLOSE_PARENTHESIS)) {
      ConsumeNext();
      return expr;
    } else {
      Expected("Expected a closing pair for '('", GENERATE_POSITION_PAST_ONE_COLUMN);
      DidYouMean(")", GENERATE_POSITION_PAST_ONE_COLUMN);
      return {};
    }
  } else
    return {};
}

std::optional<std::pair<std::unique_ptr<ExpressionAST>, std::string>>
Parser::ParseSubExpression() {
  StoreParserPosition();

  auto expr = ParsePlusExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "+"};
  }

  BackTrack();

  expr = ParseMinusExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "-"};
  }

  BackTrack();

  expr = ParseMulExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "*"};
  }

  BackTrack();

  expr = ParseDivExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "/"};
  }

  BackTrack();

  expr = ParseModExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "%"};
  }

  BackTrack();

  expr = ParseGtExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), ">"};
  }

  BackTrack();

  expr = ParseLtExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "<"};
  }

  BackTrack();

  expr = ParseGteExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), ">="};
  }

  BackTrack();

  expr = ParseLteExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "<="};
  }

  BackTrack();

  expr = ParseEqualsExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "=="};
  }

  BackTrack();

  expr = ParseNotEqualsExpression();

  if (expr.has_value()) {
    return std::pair<std::unique_ptr<ExpressionAST>, std::string>{
        std::move(expr.value()), "!="};
  }

  return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParsePlusExpression() {
  StoreParserPosition();

  if (Peek(TokenName::PLUS))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseMinusExpression() {
  StoreParserPosition();

  if (Peek(TokenName::MINUS))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseMulExpression() {
  StoreParserPosition();

  if (Peek(TokenName::MUL))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseDivExpression() {
  StoreParserPosition();

  if (Peek(TokenName::DIV))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseModExpression() {
  StoreParserPosition();

  if (Peek(TokenName::MOD))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseGtExpression() {
  StoreParserPosition();

  if (Peek(TokenName::GT))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseLtExpression() {
  StoreParserPosition();

  if (Peek(TokenName::LT))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseGteExpression() {
  StoreParserPosition();

  if (Peek(TokenName::GTE))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseLteExpression() {
  StoreParserPosition();

  if (Peek(TokenName::LTE))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseEqualsExpression() {
  StoreParserPosition();

  if (Peek(TokenName::EQ))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>>
Parser::ParseNotEqualsExpression() {
  StoreParserPosition();

  if (Peek(TokenName::NEQ))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  if (expr.has_value()) {
    return expr;
  } else
    return {};
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseExpression() {
  StoreParserPosition();

  auto expr = ParseExpressionBeginningWithID();

  if (expr.has_value())
    return expr;

  BackTrack();

  expr = ParseExpressionBeginningWithNumber();

  if (expr.has_value())
    return expr;

  BackTrack();

  expr = ParseExpressionBeginningWithBraces();

  if (expr.has_value())
    return expr;

  return {};
}

std::optional<std::unique_ptr<RangeAST>> Parser::ParseRange() {
  StoreParserPosition();
  auto start = ParseExpression();

  if (!start.has_value()) {
    Expected("Expected an expression here", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::TO)) {
    ConsumeNext();
  } else {
    Expected("Expected the 'to' keyword.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("to", GENERATE_POSITION_PAST_ONE_COLUMN);
  }

  auto end = ParseExpression();

  if (!end.has_value()) {
    Expected("Expected an expression here", GENERATE_POSITION_PAST_ONE_COLUMN);
  }

  // todo: return range
  return std::make_unique<RangeAST>(std::move(start.value()),
                                    std::move(end.value()));
}

std::optional<std::vector<std::unique_ptr<StatementAST>>>
Parser::ParseCurlyBraceAndBody() {
  int curly_brace_position;

  if (Peek(TokenName::OPEN_CURLY)) {
    curly_brace_position = current_parser_position + 1;
    ConsumeNext();
  } else {
    Expected("Expected a '{' here", GENERATE_POSITION_PAST_ONE_COLUMN);
  }

  auto body = ParseStatements();

  if (!body.has_value())
    return {};

  if (Peek(TokenName::CLOSE_CURLY)) {
    ConsumeNext();
    return body;
  } else {
    Expected("Expected a closing pair for '{'",
             token_vec[curly_brace_position].GetLine(),
             token_vec[curly_brace_position].GetColumn());
    return {};
  }
}

std::optional<std::unique_ptr<LoopAST>> Parser::ParseLoop() {
  StoreParserPosition();

  if (Peek(TokenName::LOOP)) {
    ConsumeNext();
    if (!CheckInsideFunction()){
        Unexpected("A 'loop' must be inside a function definition", GENERATE_CURRENT_POSITION);
        return {};
    }
    status_list.push_back(ParserStatus::PARSING_LOOP);

    auto body = ParseCurlyBraceAndBody();

    if (body.has_value()) {
      status_list.pop_back();
      return std::make_unique<LoopAST>(std::move(body.value()));
    }
  }
  return {};
}

std::optional<std::unique_ptr<ForLoopAST>> Parser::ParseForLoop() {
  StoreParserPosition();

  if (Peek(TokenName::FOR)){
    ConsumeNext();
    if (!CheckInsideFunction()){
        Unexpected("A 'for loop' must be inside a function definition", GENERATE_CURRENT_POSITION);
        return {};
    }
    status_list.push_back(ParserStatus::PARSING_FOR_LOOP);

  }
  else
    return {};

  std::string iteration_variable;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    iteration_variable = GetCurrentToken().GetValue();
  } else {
    StoreParserPosition();
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) != Lexer::keywords.end()){
        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
        Unexpected("'" + GetCurrentToken().GetValue() + "'" + " is a keyword, it cannot be used as an iteration variable.", GENERATE_CURRENT_POSITION);
        return {};
    }
    Unexpected("Expected an iteration variable name after the 'for' keyword",
             GENERATE_CURRENT_POSITION);
    return {};
  }

  BackTrack();
  if (Peek(TokenName::IN))
    ConsumeNext();
  else {
    Expected("You missed the 'in' keyword in the for loop", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("in", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  auto range = ParseRange();

  if (!range.has_value())
    return {};

  auto body = ParseCurlyBraceAndBody();

  if (!body.has_value())
    return {};

  status_list.pop_back();

  return std::make_unique<ForLoopAST>(iteration_variable,
                                      std::move(range.value()), std::move(body.value()));
}

std::optional<std::unique_ptr<WhileLoopAST>> Parser::ParseWhileLoop() {
  StoreParserPosition();

  if (Peek(TokenName::WHILE)){
    ConsumeNext();
    if (!CheckInsideFunction()){
        Unexpected("A 'while loop' must be inside a function definition", GENERATE_CURRENT_POSITION);
        return {};
    }
    status_list.push_back(ParserStatus::PARSING_WHILE_LOOP);
  }
  else
    return {};

  auto loop_condition = ParseExpression();
  if (!loop_condition.has_value()) {
    Expected("Expected an expression after the 'while' keyword",
             GENERATE_POSITION_PAST_ONE_COLUMN);
  }

  auto body = ParseCurlyBraceAndBody();

  if (body.has_value()) {

    status_list.pop_back();

    return std::make_unique<WhileLoopAST>(std::move(loop_condition.value()),
                                          std::move(body.value()));

  } else
    return {};
}

std::optional<std::vector<std::unique_ptr<MatchArmAST>>>
Parser::ParseMatchArms() {
  StoreParserPosition();

  std::vector<std::unique_ptr<MatchArmAST>> arm_vec;

  auto arm = ParseMatchArm();

  if (!arm.has_value())
    return {};

  arm_vec.emplace_back(std::move(arm.value()));

  auto other_arms = ParseMatchArms();

  if (other_arms.has_value()) {
    auto other_arm = std::move(other_arms.value());
    for (auto &arm : other_arm) {
      arm_vec.push_back(std::move(arm));
    }
    return std::move(arm_vec);
  } else {
    return std::move(arm_vec);
  }
}

std::optional<std::unique_ptr<MatchArmAST>> Parser::ParseMatchArm() {
  StoreParserPosition();

  auto cond = ParseExpression();

  if (!cond.has_value())
    return {};

  if (Peek(TokenName::ARROW)) {
    ConsumeNext();
  } else {
    Expected("Expected an '=>' after the expression in 'match' body",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("=>", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  auto arm_body = ParseCurlyBraceAndBody();

  if (!arm_body.has_value())
    return {};
  else
    return std::make_unique<MatchArmAST>(std::move(cond.value()),
                                         std::move(arm_body.value()));
}

std::optional<std::unique_ptr<MatchStatementAST>> Parser::ParseMatchStatement() {
  StoreParserPosition();
  int curly_brace_position;

  if (Peek(TokenName::MATCH)){
    ConsumeNext();
    if (!CheckInsideFunction()){
        Unexpected("A 'match statement' must be inside a function definition", GENERATE_CURRENT_POSITION);
        return {};
    }
  }
  else
    return {};

  auto expr = ParseExpression();
  if (!expr.has_value()) {
    Expected("Expected an expression after the 'match' keyword",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::OPEN_CURLY)) {
    curly_brace_position = current_parser_position + 1;
    ConsumeNext();
  } else {
    Expected("Expected an '{' after the expression in match",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  auto arms = ParseMatchArms();

  if (!arms.has_value()) {
    return {};
  }

  if (Peek(TokenName::CLOSE_CURLY)) {
    ConsumeNext();
    return std::make_unique<MatchStatementAST>(std::move(expr.value()),
                                               std::move(arms.value()));
  } else {
    Expected("Expected a closing pair for '{'",
             token_vec[curly_brace_position].GetLine(),
             token_vec[curly_brace_position].GetColumn());
    return {};
  }
}

std::optional<std::unique_ptr<IfStatementAST>> Parser::ParseIfStatement() {
  StoreParserPosition();

  if (Peek(TokenName::IF)) {
    ConsumeNext();
    if (!CheckInsideFunction()){
        Unexpected("'if statement' must be inside a function definition", GENERATE_CURRENT_POSITION);
        return {};
    }
    status_list.push_back(ParserStatus::PARSING_IF_STATEMENT);
  }
  else
    return {};

  auto condition = ParseExpression();

  if (!condition.has_value()) {
    Expected("Expected an condition after the 'if' keyword", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  auto body = ParseCurlyBraceAndBody();

  if (body.has_value()) {
    status_list.push_back(ParserStatus::PARSED_IF_STATEMENT);
    return std::make_unique<IfStatementAST>(std::move(condition.value()),
                                            std::move(body.value()));
  } else {
      status_list.push_back(ParserStatus::PARSING_IF_STATEMENT_FAILED);
      return {};
  }
}

std::optional<std::unique_ptr<ElseIfStatementAST>> Parser::ParseElseIfStatement() {
  StoreParserPosition();

  if (Peek(TokenName::ELSE))
    ConsumeNext();
  else
    return {};

  if (Peek(TokenName::IF)){
    ConsumeNext();
    if (!CheckInsideFunction()){
        Unexpected("'else if' must be inside a function definition", GENERATE_CURRENT_POSITION, 7);
        return {};
    }
    // Check if we have an immediate else after either else-if or if statement
    if(status_list.back() != ParserStatus::PARSED_IF_STATEMENT && status_list.back() != ParserStatus::PARSED_ELSEIF_STATEMENT){
          Unexpected("'else if' without an previous 'if' found", GENERATE_CURRENT_POSITION, 7);
          return {};
    }

    status_list.push_back(ParserStatus::PARSING_ELSEIF_STATEMENT);
  }
  else
    return {};

  auto condition = ParseExpression();

  if (!condition.has_value()) {
    Expected("Expected an condition after 'else-if'", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  auto body = ParseCurlyBraceAndBody();

  if (body.has_value()) {
    // do sth

    status_list.push_back(ParserStatus::PARSED_ELSEIF_STATEMENT);
    return std::make_unique<ElseIfStatementAST>(std::move(condition.value()),
                                                std::move(body.value()));
  } else
    return {};
}

std::optional<std::unique_ptr<ElseStatementAST>> Parser::ParseElseStatement() {
  StoreParserPosition();

  if (Peek(TokenName::ELSE)) {
    ConsumeNext();
    if (!CheckInsideFunction()){
        Unexpected("else' must be inside a function definition", GENERATE_CURRENT_POSITION);
        return {};
    }
    // Check if we have an immediate else after either else-if or if statement
    if(status_list.back() != ParserStatus::PARSED_IF_STATEMENT && status_list.back() != ParserStatus::PARSED_ELSEIF_STATEMENT){
          Unexpected("else without an previous 'if' found", GENERATE_CURRENT_POSITION);
          return {};
    }

    auto body = ParseCurlyBraceAndBody();

    if (body.has_value()) {
      return std::make_unique<ElseStatementAST>(std::move(body.value()));
    } else
      return {};

  } else
    return {};
}

std::optional<std::unique_ptr<BreakStatementAST>>
Parser::ParseBreakStatement() {
  if (Peek(TokenName::BREAK)) {
    ConsumeNext();
    if (status_list.back() != ParserStatus::PARSING_FOR_LOOP &&
        status_list.back() != ParserStatus::PARSING_WHILE_LOOP &&
        status_list.back() != ParserStatus::PARSING_LOOP){
            Unexpected("'break' not within a loop body", GENERATE_CURRENT_POSITION, 5);
            return {};
        }
  } else
    return {};

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
    return std::make_unique<BreakStatementAST>();
  } else {
    Expected("Expected semicolon after 'break'", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }
}

std::optional<std::unique_ptr<FunctionCallAST>>
Parser::ParseFunctionCallStatement() {
  std::string fn_name;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    fn_name = GetCurrentToken().GetValue();
  } else
    return {};

  if (Peek(TokenName::OPEN_PARENTHESIS)) {
    ConsumeNext();
  } else
    return {};

  // TODO: check for fn arguments later.

  if (Peek(TokenName::CLOSE_PARENTHESIS)) {
    ConsumeNext();
  } else {
    Expected("Expected closing pair for '('", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
  } else {
    Expected("Expected a semi colon : ';'", GENERATE_POSITION_PAST_ONE_COLUMN);
    return {};
  }

  std::vector<std::unique_ptr<StatementAST>> args;
  return std::make_unique<FunctionCallAST>(fn_name, std::move(args));
}

std::optional<std::unique_ptr<StatementAST>> Parser::ParseStatement() {
  StoreParserPosition();

  std::optional<std::unique_ptr<StatementAST>> result = ParseForLoop();
  if (result.has_value()) {
    // can do sth here later
    return result;
  }

  BackTrack();

  result = ParseWhileLoop();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseLoop();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseVariableInitWithType();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseBreakStatement();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseVariableInitWithLet();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseVariableDecl();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseVariableAssignment();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseMatchStatement();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseIfStatement();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseElseIfStatement();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseElseStatement();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseFunction();
  if (result.has_value()) {
    return result;
  }

  BackTrack();

  result = ParseFunctionCallStatement();
  if (result.has_value()) {
    return result;
  }

  return {};
}

std::optional<std::vector<std::unique_ptr<StatementAST>>>
Parser::ParseStatements() {
  StoreParserPosition();

  std::vector<std::unique_ptr<StatementAST>> stmts;
  auto result = ParseStatement();

  if (result.has_value()) {
    stmts.push_back(std::move(result.value()));

    auto new_result = ParseStatements();

    if (new_result.has_value()) {
      auto stmt = std::move(new_result.value());
      for (auto &elt : stmt) {
        stmts.push_back(std::move(elt));
      }
      return stmts;
    } else
      return stmts;
  } else
    return stmts;
}

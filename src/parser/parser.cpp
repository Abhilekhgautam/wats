#include "parser.hpp"
#include "../AST/FunctionArgumentAST.hpp"
#include "../lexer/lexer.hpp"
#include "../lexer/tokens.hpp"
#include "../utils.hpp"

#include "../AST/BinaryExpressionAST.hpp"
#include "../AST/FunctionCallExprAST.hpp"
#include "../AST/FunctionParameterAST.hpp"
#include "../AST/IdentifierAST.hpp"
#include "../AST/MatchStatementAST.hpp"
#include "../AST/NumberAST.hpp"

#include <algorithm>
#include <iostream>
#include <memory>
#include <optional>
#include <vector>

#define GENERATE_POSITION_PAST_ONE_COLUMN                                      \
  token_vec[current_parser_position].GetLine(),                                \
      token_vec[current_parser_position].GetColumn() + 1

#define GENERATE_CURRENT_POSITION                                              \
  token_vec[current_parser_position].GetLine(),                                \
      token_vec[current_parser_position].GetColumn()

inline bool Parser::CheckInsideFunction() {
  if (status_list.empty() ||
      status_list.front() != ParserStatus::PARSING_FN_DEFINITION) {
    return false;
  }
  return true;
}

std::optional<std::vector<std::unique_ptr<StatementAST>>> Parser::Parse() {
  return ParseStatements();
  // auto stmts =ParseStatements();

  // if (stmts.has_value()) {
  //   auto stmt_vec = std::move(stmts.value());
  //   for (const auto &elt : stmt_vec) {
  //     elt->Debug();
  //   }
  // }
}

void Parser::DidYouMean(const std::string to_add, std::size_t line,
                        std::size_t column, bool space_before) {

  const std::string invalid_line = context.source_code_by_line[line - 1];
  const std::string contents_after_error = invalid_line.substr(column - 1);
  const std::string contents_before_error = invalid_line.substr(0, column - 1);

  std::string expected_correct_line;

  if (!space_before) {
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

  std::cout << context.source_code_by_line[line - 1] << '\n';
  Color("green", MultiPartArrow(column, 1), true);
}

void Parser::Unexpected(const std::string str, std::size_t line,
                        std::size_t column, std::size_t times) {

  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", str, true);

  std::cout << context.source_code_by_line[line - 1] << '\n';
  if (!times) {
    Color("red",
          SetArrow(column - GetCurrentToken().GetValue().length() + 1,
                   GetCurrentToken().GetValue().length()),
          true);
  } else {
    Color("red", SetArrow(column - times + 1, times), true);
  }
}

bool Parser::Peek(TokenName tok) const {
  if (current_parser_position + 1 >= token_vec.size())
    return false;
  else if (token_vec.empty())
    return false;

  return token_vec.at(current_parser_position + 1).GetTokenName() == tok;
}

Token Parser::GetCurrentToken() const {
  return token_vec[current_parser_position];
}

void Parser::StoreParserPosition() {
  temp_parser_position = current_parser_position;
}

void Parser::ConsumeNext() {
  current_parser_position = current_parser_position + 1;
}

void Parser::BackTrack() { current_parser_position = temp_parser_position; }

bool Parser::IsAtEnd() const {
  return current_parser_position >= token_vec.size();
}

std::optional<std::unique_ptr<FunctionDefinitionAST>>
Parser::ParseFunctionWithRetType() {
  StoreParserPosition();
  // later when we add fn args..
  [[maybe_unused]] int parenthesis_position;

  if (Peek(TokenName::FUNCTION)) {
    ConsumeNext();
    if (CheckInsideFunction()) {
      Unexpected("You cannot define a function inside another function",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();
      return {};
    }
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION);
  } else
    return {};

  SourceLocation loc = GetCurrentToken().GetSourceLocation();
  std::unique_ptr<IdentifierAST> fn_name;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    fn_name = std::make_unique<IdentifierAST>(
        GetCurrentToken().GetValue(), GetCurrentToken().GetSourceLocation());
  } else {
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) !=
        Lexer::keywords.end()) {
      status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
      Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                     " is a keyword, it cannot be used as a function name.",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }

    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
    Expected(
        "Expected an Identifier (Function Name) after the 'function' keyword",
        GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::OPEN_PARENTHESIS))
    ConsumeNext();
  else {
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
    Expected("Expected a '('", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("(", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();
    return {};
  }

  auto fn_args = ParseFunctionArguments();

  if (Peek(TokenName::CLOSE_PARENTHESIS))
    ConsumeNext();
  else {
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
    Expected("Expected a ')'", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(")", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();
    return {};
  }

  // fixme: add other types later.
  if (Peek(TokenName::I32)) {
    // todo: store the type
  }

  auto result = ParseCurlyBraceAndBody();
  if (result.has_value()) {
    auto val = fn_args.has_value()
                   ? std::make_unique<FunctionDefinitionAST>(
                         std::move(fn_name), std::move(fn_args.value()),
                         std::move(result.value()), "", loc)
                   : std::make_unique<FunctionDefinitionAST>(
                         std::move(fn_name), nullptr, std::move(result.value()),
                         "", loc);

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
    if (CheckInsideFunction()) {
      Unexpected("You cannot define a function inside another function",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION);

  } else {
    return {};
  }
  SourceLocation loc = GetCurrentToken().GetSourceLocation();
  std::unique_ptr<IdentifierAST> fn_name;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    fn_name = std::make_unique<IdentifierAST>(
        GetCurrentToken().GetValue(), GetCurrentToken().GetSourceLocation());
  } else {
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) !=
        Lexer::keywords.end()) {
      status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
      Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                     " is a keyword, it cannot be used as a function name.",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();
      return {};
    }
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

    Expected(
        "Expected an Identifier (Function Name) after the 'function' keyword",
        GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::OPEN_PARENTHESIS))
    ConsumeNext();
  else {
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

    Expected("Expected a '('", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("(", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }
  auto fn_args = ParseFunctionArguments();

  if (Peek(TokenName::CLOSE_PARENTHESIS))
    ConsumeNext();
  else {
    status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

    Expected("Expected a ')'", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(")", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  auto result = ParseCurlyBraceAndBody();
  if (result.has_value()) {
    auto val = fn_args.has_value()
                   ? std::make_unique<FunctionDefinitionAST>(
                         std::move(fn_name), std::move(fn_args.value()),
                         std::move(result.value()), "", loc)
                   : std::make_unique<FunctionDefinitionAST>(
                         std::move(fn_name), nullptr, std::move(result.value()),
                         "", loc);

    status_list.clear();

    return val;
  } else {

    return {};
  }
}

std::optional<std::unique_ptr<FunctionArgumentAST>>
Parser::ParseFunctionArgument() {
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    auto id = std::make_unique<IdentifierAST>(
        GetCurrentToken().GetValue(), GetCurrentToken().GetSourceLocation());
    return std::make_unique<FunctionArgumentAST>(std::move(id));
  } else
    return {};
}

std::optional<std::unique_ptr<FunctionArgumentAST>>
Parser::ParseFunctionArguments() {
  auto argument = ParseFunctionArgument();

  if (!argument.has_value()) {
    return {};
  }

  std::vector<std::unique_ptr<IdentifierAST>> id_args;
  id_args.push_back(std::move(argument.value()->GetId()));

  if (Peek(TokenName::COMMA))
    ConsumeNext();
  else {
    return std::make_unique<FunctionArgumentAST>(id_args);
  }

  auto arguments = ParseFunctionArguments();

  if (!arguments.has_value()) {
    Expected("Unexpected ',' found", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  for (auto &elt : arguments.value()->GetIds()) {
    id_args.push_back(std::move(elt));
  }

  return std::make_unique<FunctionArgumentAST>(id_args);
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

  std::vector<SourceLocation> locations;

  std::string var_name;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
    locations.push_back(GetCurrentToken().GetSourceLocation());
  } else {
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) !=
        Lexer::keywords.end()) {
      status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
      Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                     " is a keyword, it cannot be used as a variable name.",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    Expected("A Variable name after a let expression is required",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }
  // push (0, 0) for now to mean none
  locations.push_back({0, 0});

  if (Peek(TokenName::SEMI_COLON))
    ConsumeNext();
  else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);

    IncrementErrorCount();

    return {};
  }

  return std::make_unique<VariableDeclarationAST>("", var_name, locations);
}

std::optional<std::unique_ptr<VariableDeclarationAST>>
Parser::ParseVariableDeclWithType() {
  StoreParserPosition();

  if (Peek(TokenName::LET)) {
    ConsumeNext();
  } else
    return {};

  std::vector<SourceLocation> locations;
  std::string var_name;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
    locations.push_back(GetCurrentToken().GetSourceLocation());
  } else {
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) !=
        Lexer::keywords.end()) {
      status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
      Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                     " is a keyword, it cannot be used as a variable name.",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    Expected("A Variable name after a let expression is required",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::COLON))
    ConsumeNext();
  else
    return {};

  // FixME: add support for user defined type
  // later when support for classes are added.
  std::string type;
  if (Peek(TokenName::I32) || Peek(TokenName::I64) || Peek(TokenName::F32) ||
      Peek(TokenName::F64)) {
    ConsumeNext();
    type = GetCurrentToken().GetValue();
    locations.push_back(GetCurrentToken().GetSourceLocation());
  } else {
    Expected("Consider mentioning the type of the variable",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::SEMI_COLON))
    ConsumeNext();
  else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
    IncrementErrorCount();

    return {};
  }

  return std::make_unique<VariableDeclarationAST>(type, var_name, locations);
}

std::optional<std::unique_ptr<VariableDeclarationAST>>
Parser::ParseVariableDecl() {
  auto result = ParseVariableDeclWithType();

  if (result.has_value()) {
    return result;
  }

  BackTrack();
  result = ParseVariableDeclWithLet();

  if (result.has_value()) {
    return result;
  }

  return {};
}

std::optional<std::unique_ptr<VariableAssignmentAST>>
Parser::ParseVariableAssignment() {
  StoreParserPosition();

  std::vector<SourceLocation> locations;
  std::string var_name;
  IdentifierAST id = {"", {}};
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
    locations.push_back(GetCurrentToken().GetSourceLocation());
    id = {var_name, locations[0]};
  } else
    return {};

  if (Peek(TokenName::ASSIGN))
    ConsumeNext();
  else
    return {};

  auto expr = ParseExpression();

  auto &result = expr.value();

  auto loc = result->GetSourceLocation();

  if (!result) {
    Expected("Expected an value or expression for the assignment",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }
  for (const auto elt : loc) {
    locations.push_back(elt);
  }
  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
    return std::make_unique<VariableAssignmentAST>(id, std::move(expr.value()),
                                                   locations);
  } else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
    IncrementErrorCount();

    return {};
  }
}

std::optional<std::unique_ptr<VariableDeclareAndAssignAST>>
Parser::ParseVariableInitWithLet() {
  StoreParserPosition();

  std::vector<SourceLocation> locations;

  if (Peek(TokenName::LET))
    ConsumeNext();
  else
    return {};

  std::string var_name;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
    locations.push_back(GetCurrentToken().GetSourceLocation());
  } else {
    StoreParserPosition();
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) !=
        Lexer::keywords.end()) {
      Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                     " is a keyword, it cannot be used as a variable name.",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    BackTrack();

    Expected("A Variable name after a let expression is required",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }
  if (Peek(TokenName::ASSIGN))
    ConsumeNext();
  else
    return {};

  // No Type,so type has same location as the var_name
  locations.push_back(locations[0]);
  auto expr = ParseExpression();

  if (!expr.has_value()) {
    Expected("Expected an value or expression for the assignment",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  auto &result = expr.value();

  for (const auto &elt : result->GetSourceLocation()) {
    locations.push_back(elt);
  }

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
  } else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);

    IncrementErrorCount();

    return {};
  }

  return std::make_unique<VariableDeclareAndAssignAST>(
      var_name, "", std::move(expr.value()), locations);
}

std::optional<std::unique_ptr<VariableDeclareAndAssignAST>>
Parser::ParseVariableInitWithType() {
  StoreParserPosition();

  if (Peek(TokenName::LET))
    ConsumeNext();
  else
    return {};

  std::string var_name;
  std::vector<SourceLocation> locations;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    var_name = GetCurrentToken().GetValue();
    locations.push_back(GetCurrentToken().GetSourceLocation());
  } else {
    StoreParserPosition();
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) !=
        Lexer::keywords.end()) {
      Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                     " is a keyword, it cannot be used as a variable name.",
                 GENERATE_CURRENT_POSITION);

      IncrementErrorCount();

      return {};
    }
    Expected("A Variable name after a let expression is required",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::COLON))
    ConsumeNext();
  else
    return {};

  std::string type;
  // FixME: Add support for user defined types
  // later when support for classes are added
  if (Peek(TokenName::I32) || Peek(TokenName::I64) || Peek(TokenName::F32) ||
      Peek(TokenName::F64)) {
    ConsumeNext();
    type = GetCurrentToken().GetValue();
    locations.push_back(GetCurrentToken().GetSourceLocation());
  } else {
    Expected("Consider mentioning the type of the variable",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

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
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
  } else {
    Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);

    IncrementErrorCount();

    return {};
  }

  return std::make_unique<VariableDeclareAndAssignAST>(
      var_name, type, std::move(expr.value()), locations);
}

std::optional<std::unique_ptr<ExpressionAST>> Parser::ParseFunctionParameter() {
  StoreParserPosition();
  auto expr = ParseExpression();

  auto val = std::move(expr.value());

  if (val) {
    return std::move(val);
  } else
    return {};
}

std::optional<std::unique_ptr<FunctionParameterAST>>
Parser::ParseFunctionParameters() {
  auto para = ParseFunctionParameter();
  auto val = std::move(para.value());

  if (!val)
    return {};

  std::vector<std::unique_ptr<ExpressionAST>> params;
  params.push_back(std::move(val));

  if (Peek(TokenName::COMMA))
    ConsumeNext();
  else {
    return std::make_unique<FunctionParameterAST>(std::move(params));
  }

  auto parameters = ParseFunctionParameters();

  if (!parameters.has_value()) {
    Expected("Unexpected ',' found", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  for (auto &elt : parameters.value()->GetFunctionParameters()) {
    params.push_back(std::move(elt));
  }

  return std::make_unique<FunctionParameterAST>(std::move(params));
}
// Parsing Variable Declaration Ends here

std::optional<std::unique_ptr<ExpressionAST>>
Parser::ParseExpressionBeginningWithID() {
  StoreParserPosition();

  std::string identifier_name;
  if (!Peek(TokenName::ID)) {
    return {};
  }
  std::vector<SourceLocation> locations;
  ConsumeNext();
  identifier_name = GetCurrentToken().GetValue();
  auto loc = GetCurrentToken().GetSourceLocation();
  locations.push_back(loc);

  std::unique_ptr<ExpressionAST> identifier_expr =
      std::make_unique<IdentifierAST>(identifier_name, loc);
  // later if identifier is a function call.
  std::unique_ptr<FunctionParameterAST> params;

  if (Peek(TokenName::OPEN_PARENTHESIS)) {
    ConsumeNext();

    auto result = ParseFunctionParameters();

    auto &val = result.value();

    if (val) {
      params = std::move(val);
    }

    if (Peek(TokenName::CLOSE_PARENTHESIS)) {
      ConsumeNext();
      return std::make_unique<FunctionCallExprAST>(identifier_name,
                                                   std::move(params), loc);
    } else {
      Expected("Expected a closing pair ')'",
               GENERATE_POSITION_PAST_ONE_COLUMN);
      IncrementErrorCount();
      return {};
    }
  }
  auto expr = ParseSubExpression();

  if (expr.has_value()) {
    auto &value = expr.value();
    // Push the Operator Node location
    locations.push_back(value.first.GetSourceLocation());

    for (const auto &elt : value.second->GetSourceLocation()) {
      locations.push_back(elt);
    }
    return std::make_unique<BinaryExpressionAST>(std::move(identifier_expr),
                                                 std::move(value.second),
                                                 value.first, locations);
  } else
    return identifier_expr;
}

std::optional<std::unique_ptr<ExpressionAST>>
Parser::ParseExpressionBeginningWithNumber() {
  StoreParserPosition();

  std::string num;
  int decimal_count;
  SourceLocation loc;

  if (!Peek(TokenName::NUMBER))
    return {};

  ConsumeNext();
  num = GetCurrentToken().GetValue();
  loc = GetCurrentToken().GetSourceLocation();

  decimal_count = std::count(num.begin(), num.end(), '.');

  if (decimal_count > 1) {
    Unexpected("Too many decimal points in a numerical value",
               GENERATE_CURRENT_POSITION);
    IncrementErrorCount();

    return {};
  }

  auto number_expr = std::make_unique<NumberAST>(num, (bool)decimal_count, loc);
  std::vector<SourceLocation> locations;
  locations.push_back(loc);

  auto expr = ParseSubExpression();
  if (expr.has_value()) {
    auto &val = expr.value();

    locations.push_back(val.first.GetSourceLocation());

    for (const auto elt : val.second->GetSourceLocation()) {
      locations.push_back(elt);
    }

    return std::make_unique<BinaryExpressionAST>(
        std::move(number_expr), std::move(val.second), val.first, locations);
  } else
    return number_expr;
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
      Expected("Expected a closing pair for '('",
               GENERATE_POSITION_PAST_ONE_COLUMN);
      DidYouMean(")", GENERATE_POSITION_PAST_ONE_COLUMN);
      IncrementErrorCount();

      return {};
    }
  } else
    return {};
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseSubExpression() {
  StoreParserPosition();

  auto expr = ParsePlusExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseMinusExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseMulExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseDivExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseModExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseGtExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseLtExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseGteExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseLteExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseEqualsExpression();

  if (expr.has_value()) {
    return expr;
  }

  BackTrack();

  expr = ParseNotEqualsExpression();

  if (expr.has_value()) {
    return expr;
  }

  return {};
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParsePlusExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::PLUS)) {
    return {};
  }

  ConsumeNext();
  OperatorNode opNode(std::string("+"), GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the +
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseMinusExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::MINUS)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode("-", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the +
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseMulExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::MUL)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode("*", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the *
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseDivExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::DIV)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode("/", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the /
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseModExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::MOD))
    return {};

  ConsumeNext();
  OperatorNode opNode("%", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the %
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseGtExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::GT)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode(">", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the >
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseLtExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::LT)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode("<", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the <
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseGteExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::GTE)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode(">=", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the >=
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseLteExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::LTE)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode("<=", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the <=
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseEqualsExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::EQ)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode("==", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the ==
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

std::optional<std::pair<OperatorNode, std::unique_ptr<ExpressionAST>>>
Parser::ParseNotEqualsExpression() {
  StoreParserPosition();

  if (!Peek(TokenName::NEQ)) {
    return {};
  }
  ConsumeNext();
  OperatorNode opNode("!=", GetCurrentToken().GetSourceLocation());

  auto expr = ParseExpression();

  auto result = std::move(expr.value());
  if (!result) {
    // Todo: Error, expected an expression after the !=
    return {};
  } else {
    return std::make_pair(opNode, std::move(result));
  }
}

// expr -> expr + term | expr - term | term
// term -> term * factor | term / factor | factor
// factor -> Number | (expr)
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

  auto &result = start.value();

  if (!result) {
    Expected("Expected an expression here", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  std::vector<SourceLocation> locations(result->GetSourceLocation().begin(),
                                        result->GetSourceLocation().end());

  if (Peek(TokenName::TO)) {
    ConsumeNext();
  } else {
    Expected("Expected the 'to' keyword.", GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("to", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();
  }

  auto end = ParseExpression();

  auto &val = end.value();

  if (!val) {
    Expected("Expected an expression here", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  for (const auto &elt : val->GetSourceLocation()) {
    locations.push_back(elt);
  }

  // todo: return range
  return std::make_unique<RangeAST>(std::move(start.value()),
                                    std::move(end.value()), locations);
}

std::optional<std::vector<std::unique_ptr<StatementAST>>>
Parser::ParseCurlyBraceAndBody() {
  int curly_brace_position;

  if (Peek(TokenName::OPEN_CURLY)) {
    curly_brace_position = current_parser_position + 1;
    ConsumeNext();
  } else {
    ConsumeNext();
    Unexpected("Expected a '{' here", GENERATE_CURRENT_POSITION);
    IncrementErrorCount();

    return {};
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
    IncrementErrorCount();

    return {};
  }
}

std::optional<std::unique_ptr<LoopAST>> Parser::ParseLoop() {
  StoreParserPosition();

  if (Peek(TokenName::LOOP)) {
    ConsumeNext();
    if (!CheckInsideFunction()) {
      Unexpected("A 'loop' must be inside a function definition",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

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

  if (Peek(TokenName::FOR)) {
    ConsumeNext();
    if (!CheckInsideFunction()) {
      Unexpected("A 'for loop' must be inside a function definition",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    status_list.push_back(ParserStatus::PARSING_FOR_LOOP);

  } else
    return {};
  SourceLocation loc = GetCurrentToken().GetSourceLocation();
  std::unique_ptr<IdentifierAST> iteration_variable;

  if (Peek(TokenName::ID)) {
    ConsumeNext();
    iteration_variable = std::make_unique<IdentifierAST>(
        GetCurrentToken().GetValue(), GetCurrentToken().GetSourceLocation());
  } else {
    StoreParserPosition();
    ConsumeNext();
    if (Lexer::keywords.find(GetCurrentToken().GetValue()) !=
        Lexer::keywords.end()) {
      status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
      Unexpected(
          "'" + GetCurrentToken().GetValue() + "'" +
              " is a keyword, it cannot be used as an iteration variable.",
          GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    BackTrack();

    Unexpected("Expected an iteration variable name after the 'for' keyword",
               GENERATE_CURRENT_POSITION);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::IN))
    ConsumeNext();
  else {
    Expected("You missed the 'in' keyword in the for loop",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    DidYouMean("in", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  auto range = ParseRange();

  if (!range.has_value())
    return {};

  auto body = ParseCurlyBraceAndBody();

  if (!body.has_value())
    return {};

  status_list.pop_back();

  return std::make_unique<ForLoopAST>(std::move(iteration_variable),
                                      std::move(range.value()),
                                      std::move(body.value()), loc);
}

std::optional<std::unique_ptr<WhileLoopAST>> Parser::ParseWhileLoop() {
  StoreParserPosition();

  if (Peek(TokenName::WHILE)) {
    ConsumeNext();
    if (!CheckInsideFunction()) {
      Unexpected("A 'while loop' must be inside a function definition",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    status_list.push_back(ParserStatus::PARSING_WHILE_LOOP);
  } else
    return {};

  auto loop_condition = ParseExpression();
  if (!loop_condition.has_value()) {
    Expected("Expected an expression after the 'while' keyword",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
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
    IncrementErrorCount();

    return {};
  }

  auto arm_body = ParseCurlyBraceAndBody();

  if (!arm_body.has_value())
    return {};
  else
    return std::make_unique<MatchArmAST>(std::move(cond.value()),
                                         std::move(arm_body.value()));
}

std::optional<std::unique_ptr<MatchStatementAST>>
Parser::ParseMatchStatement() {
  StoreParserPosition();
  int curly_brace_position;

  if (Peek(TokenName::MATCH)) {
    ConsumeNext();
    if (!CheckInsideFunction()) {
      Unexpected("A 'match statement' must be inside a function definition",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
  } else
    return {};

  auto expr = ParseExpression();
  if (!expr.has_value()) {
    Expected("Expected an expression after the 'match' keyword",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::OPEN_CURLY)) {
    curly_brace_position = current_parser_position + 1;
    ConsumeNext();
  } else {
    Expected("Expected an '{' after the expression in match",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

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
    IncrementErrorCount();

    return {};
  }
}

std::optional<std::unique_ptr<IfStatementAST>> Parser::ParseIfStatement() {
  StoreParserPosition();
  SourceLocation loc;
  if (Peek(TokenName::IF)) {
    ConsumeNext();
    if (!CheckInsideFunction()) {
      Unexpected("'if statement' must be inside a function definition",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    loc = GetCurrentToken().GetSourceLocation();
    status_list.push_back(ParserStatus::PARSING_IF_STATEMENT);
  } else
    return {};

  auto condition = ParseExpression();

  if (!condition.has_value()) {
    Expected("Expected an condition after the 'if' keyword",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  auto body = ParseCurlyBraceAndBody();

  if (body.has_value()) {
    status_list.push_back(ParserStatus::PARSED_IF_STATEMENT);

    std::vector<std::unique_ptr<ElseIfStatementAST>> elseifVec;
    auto else_if_val = ParseElseIfStatement();
    while (else_if_val.has_value()) {
      elseifVec.push_back(std::move(else_if_val.value()));
      else_if_val = ParseElseIfStatement();
    }
    BackTrack();
    auto else_val = ParseElseStatement();
    if (else_val.has_value()) {
      return std::make_unique<IfStatementAST>(
          std::move(condition.value()), std::move(body.value()), elseifVec,
          std::move(else_val.value()), loc);
    } else {
      return std::make_unique<IfStatementAST>(std::move(condition.value()),
                                              std::move(body.value()),
                                              elseifVec, nullptr, loc);
    }

  } else {
    status_list.push_back(ParserStatus::PARSING_IF_STATEMENT_FAILED);
    return {};
  }
}

std::optional<std::unique_ptr<ElseIfStatementAST>>
Parser::ParseElseIfStatement() {
  StoreParserPosition();
  SourceLocation loc;
  if (Peek(TokenName::ELSE)) {
    ConsumeNext();
    loc = GetCurrentToken().GetSourceLocation();
  } else
    return {};

  if (Peek(TokenName::IF)) {
    ConsumeNext();
    if (!CheckInsideFunction()) {
      Unexpected("'else if' must be inside a function definition",
                 GENERATE_CURRENT_POSITION, 7);
      IncrementErrorCount();

      return {};
    }
    // Check if we have an immediate else after either else-if or if statement
    if (status_list.back() != ParserStatus::PARSED_IF_STATEMENT &&
        status_list.back() != ParserStatus::PARSED_ELSEIF_STATEMENT) {
      Unexpected("'else if' without an previous 'if' found",
                 GENERATE_CURRENT_POSITION, 7);
      IncrementErrorCount();

      return {};
    }

    status_list.push_back(ParserStatus::PARSING_ELSEIF_STATEMENT);
  } else
    return {};

  auto condition = ParseExpression();

  if (!condition.has_value()) {
    Expected("Expected an condition after 'else-if'",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  auto body = ParseCurlyBraceAndBody();

  if (body.has_value()) {
    // do sth

    status_list.push_back(ParserStatus::PARSED_ELSEIF_STATEMENT);
    return std::make_unique<ElseIfStatementAST>(std::move(condition.value()),
                                                std::move(body.value()), loc);
  } else
    return {};
}

std::optional<std::unique_ptr<ElseStatementAST>> Parser::ParseElseStatement() {
  StoreParserPosition();

  if (Peek(TokenName::ELSE)) {
    ConsumeNext();
    if (!CheckInsideFunction()) {
      Unexpected("else' must be inside a function definition",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    // Check if we have an immediate else after either else-if or if statement
    if (status_list.back() != ParserStatus::PARSED_IF_STATEMENT &&
        status_list.back() != ParserStatus::PARSED_ELSEIF_STATEMENT) {
      Unexpected("else without an previous 'if' found",
                 GENERATE_CURRENT_POSITION);
      IncrementErrorCount();

      return {};
    }
    SourceLocation loc = GetCurrentToken().GetSourceLocation();
    auto body = ParseCurlyBraceAndBody();

    if (body.has_value()) {
      return std::make_unique<ElseStatementAST>(std::move(body.value()), loc);
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
        status_list.back() != ParserStatus::PARSING_LOOP) {

      std::list<Parser::ParserStatus> backup_list;
      while (!status_list.empty()) {
        backup_list.push_back(status_list.back());
        status_list.pop_back();

        if (status_list.back() == ParserStatus::PARSING_FOR_LOOP ||
            status_list.back() == ParserStatus::PARSING_WHILE_LOOP ||
            status_list.back() == ParserStatus::PARSING_LOOP) {

          status_list.splice(status_list.end(), backup_list);
          break;
        }
      }

      if (status_list.empty()) {
        Unexpected("'break' not within a loop body", GENERATE_CURRENT_POSITION,
                   5);
        IncrementErrorCount();

        return {};
      }
    }
  } else
    return {};

  SourceLocation loc = GetCurrentToken().GetSourceLocation();
  if (status_list.empty()) {
    Unexpected("'break' not within a loop body", GENERATE_CURRENT_POSITION, 5);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
    return std::make_unique<BreakStatementAST>(loc);
  } else {
    Expected("Expected semicolon after 'break'",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }
}

std::optional<std::unique_ptr<FunctionCallAST>>
Parser::ParseFunctionCallStatement() {
  std::unique_ptr<IdentifierAST> fn_name;
  if (Peek(TokenName::ID)) {
    ConsumeNext();
    fn_name = std::make_unique<IdentifierAST>(
        GetCurrentToken().GetValue(), GetCurrentToken().GetSourceLocation());

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
    Expected("Expected closing pair for '('",
             GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  if (Peek(TokenName::SEMI_COLON)) {
    ConsumeNext();
  } else {
    Expected("Expected a semi colon : ';'", GENERATE_POSITION_PAST_ONE_COLUMN);
    IncrementErrorCount();

    return {};
  }

  std::vector<std::unique_ptr<StatementAST>> args;
  return std::make_unique<FunctionCallAST>(std::move(fn_name), std::move(args));
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

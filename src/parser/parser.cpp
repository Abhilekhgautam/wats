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
#include <format>
#include <iostream>
#include <memory>
#include <optional>
#include <string_view>
#include <vector>

#include "../AST/ReturnStatementAST.h"

#define GENERATE_POSITION_PAST_ONE_COLUMN                                                                              \
    token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn() + 1

#define GENERATE_CURRENT_POSITION                                                                                      \
    token_vec[current_parser_position].GetLine(), token_vec[current_parser_position].GetColumn()

inline bool Parser::CheckInsideFunction() const {
    if (status_list.empty() || status_list.front() != ParserStatus::PARSING_FN_DEFINITION) {
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

void Parser::DidYouMean(const std::string_view to_add, const std::size_t line, const std::size_t column,
                        bool space_before) const {

    const std::string_view invalid_line = context.source_code_by_line[line - 1];
    const std::string_view contents_after_error = invalid_line.substr(column - 1);
    const std::string_view contents_before_error = invalid_line.substr(0, column - 1);

    std::string expected_correct_line;

    if (!space_before) {
        expected_correct_line = std::format("{}{} {}", contents_before_error, to_add, contents_after_error);
    } else {
        expected_correct_line = std::format("{} {}{}", contents_before_error, to_add, contents_after_error);
    }
    Color("blue", "Did You Mean?", true);

    std::cout << expected_correct_line << '\n';
    if (!space_before)
        Color("green", SetPlus(column, to_add.length()), true);
    else
        Color("green", SetPlus(column + 1, to_add.length()), true);
}

void Parser::Expected(const std::string &str, const std::size_t line, const std::size_t column) {

    std::cout << "[ " << line << ":" << column << " ] ";
    Color("red", "Error: ");
    Color("blue", str, true);

    std::cout << context.source_code_by_line[line - 1] << '\n';
    Color("green", MultiPartArrow(static_cast<int>(column), 1), true);
}

void Parser::Unexpected(const std::string &str, std::size_t line, std::size_t column, std::size_t times) {

    std::cout << "[ " << line << ":" << column << " ] ";
    Color("red", "Error: ");
    Color("blue", str, true);

    std::cout << context.source_code_by_line[line - 1] << '\n';
    if (!times) {
        Color("red",
              SetArrow(column - GetCurrentToken().GetValue().length() + 1, GetCurrentToken().GetValue().length()),
              true);
    } else {
        Color("red", SetArrow(column - times + 1, times), true);
    }
}

int Parser::GetPrecedence(const std::string &op) {
    if (const auto res = operator2Precedence.find(op); res != operator2Precedence.end()) {
        return res->second;
    }
    return -1;
}

// Checks if the provided token is the current token in the sequence.
// For eg:
// let val = num * num * num;
//         ^
// Peek(tok) checks if tok is the current token as pointed by current parser position (^)
bool Parser::Peek(const TokenName tok) const {
    if (token_vec.empty() || current_parser_position >= token_vec.size())
        return false;

    return token_vec.at(current_parser_position).GetTokenName() == tok;
}

// Returns the token at the current parser position.
// For eg:
// let val = num * num * num;
//         ^
// GetCurrentToken() returns Token(=)
Token Parser::GetCurrentToken() const { return token_vec[current_parser_position]; }

void Parser::StoreParserPosition() { temp_parser_position = current_parser_position; }

// Moves the parser position to the next token.
// For eg:
// Before Consume():
// let val = num * num * num;
//         ^
// After Consume():
// let val = num * num * num;
//            ^
void Parser::Consume() { current_parser_position = current_parser_position + 1; }

void Parser::BackTrack() { current_parser_position = temp_parser_position; }

bool Parser::IsAtEnd() const { return current_parser_position >= token_vec.size(); }

std::unique_ptr<FunctionDefinitionAST> Parser::ParseFunctionWithRetType() {
    StoreParserPosition();
    // later when we add fn args..
    [[maybe_unused]] int parenthesis_position;

    if (Peek(TokenName::FUNCTION)) {
        Consume();
        if (CheckInsideFunction()) {
            Unexpected("You cannot define a function inside another function", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();
            return {};
        }
        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION);
    } else
        return {};

    SourceLocation loc = GetCurrentToken().GetSourceLocation();
    std::unique_ptr<IdentifierAST> fn_name;

    if (Peek(TokenName::ID)) {
        fn_name = std::make_unique<IdentifierAST>(GetCurrentToken().GetValue(), GetCurrentToken().GetSourceLocation());
        Consume();
    } else {
        if (Lexer::keywords.contains(GetCurrentToken().GetValue())) {
            status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
            const std::string err_msg = std::format("'{}' is a keyword, it cannot be used as a function name.",
                                                    GetCurrentToken().GetValue());
            Unexpected(err_msg, GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }

        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
        Expected("Expected an Identifier (Function Name) after the 'function' keyword",
                 GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::OPEN_PARENTHESIS))
        Consume();
    else {
        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
        Expected("Expected a '('", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean("(", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();
        return {};
    }

    auto fn_args = ParseFunctionArguments();

    if (Peek(TokenName::CLOSE_PARENTHESIS))
        Consume();
    else {
        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
        Expected("Expected a ')'", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean(")", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();
        return {};
    }

    std::string returnType{};
    if (Peek(TokenName::RET_ARROW)) {
        Consume();
        if (Peek(TokenName::I32) || Peek(TokenName::I64) || Peek(TokenName::F32) || Peek(TokenName::F64)) {
            returnType = GetCurrentToken().GetValue();
            Consume();
        } else {
            status_list.clear();
            IncrementErrorCount();
            Expected(std::format("Expected a type name after `->` found {}", GetCurrentToken().GetValue()),
                     GENERATE_CURRENT_POSITION);
            Consume();
            return {};
        }
    } else {
        status_list.clear();
        return {};
    }

    if (auto result = ParseCurlyBraceAndBody(); !result.empty()) {
        auto val = !fn_args.empty() ? std::make_unique<FunctionDefinitionAST>(std::move(fn_name), std::move(fn_args),
                                                                              std::move(result), returnType, loc)
                                    : std::make_unique<FunctionDefinitionAST>(
                                              std::move(fn_name), std::vector<std::unique_ptr<FunctionArgumentAST>>{},
                                              std::move(result), returnType, loc);

        status_list.clear();
        return val;
    } else {
        return {};
    }
}

std::unique_ptr<FunctionDefinitionAST> Parser::ParseFunctionWithoutRetType() {
    StoreParserPosition();
    // later when we add fn args..
    [[maybe_unused]] int parenthesis_position;

    if (Peek(TokenName::FUNCTION)) {
        Consume();
        if (CheckInsideFunction()) {
            Unexpected("You cannot define a function inside another function", GENERATE_CURRENT_POSITION);
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
        Token current_tok = GetCurrentToken();
        Consume();
        fn_name = std::make_unique<IdentifierAST>(current_tok.GetValue(), current_tok.GetSourceLocation());
    } else {
        if (Lexer::keywords.contains(GetCurrentToken().GetValue())) {
            status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
            Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                               " is a keyword, it cannot be used as a function name.",
                       GENERATE_CURRENT_POSITION);
            IncrementErrorCount();
            return {};
        }
        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

        Expected("Expected an Identifier (Function Name) after the 'function' keyword",
                 GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::OPEN_PARENTHESIS))
        Consume();
    else {
        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

        Expected("Expected a '('", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean("(", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }
    auto fn_args = ParseFunctionArguments();

    if (Peek(TokenName::CLOSE_PARENTHESIS))
        Consume();
    else {
        status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);

        Expected("Expected a ')'", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean(")", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    auto result = ParseCurlyBraceAndBody();
    if (!result.empty()) {
        auto val = !fn_args.empty() ? std::make_unique<FunctionDefinitionAST>(std::move(fn_name), std::move(fn_args),
                                                                              std::move(result), "", loc)
                                    : std::make_unique<FunctionDefinitionAST>(
                                              std::move(fn_name), std::vector<std::unique_ptr<FunctionArgumentAST>>{},
                                              std::move(result), "", loc);

        status_list.clear();

        return val;
    } else {

        return {};
    }
}

std::unique_ptr<FunctionArgumentAST> Parser::ParseFunctionArgument() {
    if (Peek(TokenName::ID)) {
        const auto id =
                std::make_unique<IdentifierAST>(GetCurrentToken().GetValue(), GetCurrentToken().GetSourceLocation());
        Consume();

        if (Peek(TokenName::COLON)) {
            Consume();
            // type is a id
            if (Peek(TokenName::I32) || Peek(TokenName::I64) || Peek(TokenName::F32) || Peek(TokenName::F64)) {
                const auto type = std::make_unique<IdentifierAST>(GetCurrentToken().GetValue(),
                                                                  GetCurrentToken().GetSourceLocation());
                Consume();
                return std::make_unique<FunctionArgumentAST>(id->GetName(), type->GetName(),
                                                             type->GetSourceLocation().front());
            } else {
                // Expected a `type-name` after the colon.
                Expected("Expected a type name after the ':'.", GENERATE_CURRENT_POSITION);
            }
        } else {
            // Error expected colon.
            Expected("Expected a ':' here", GENERATE_CURRENT_POSITION);
        }
        return {};
    } else
        return {};
}

std::vector<std::unique_ptr<FunctionArgumentAST>> Parser::ParseFunctionArguments() {
    auto argument = ParseFunctionArgument();

    if (!argument) {
        return {};
    }

    std::vector<std::unique_ptr<FunctionArgumentAST>> args;
    args.push_back(std::move(argument));

    if (Peek(TokenName::COMMA))
        Consume();
    else {
        return args;
    }

    auto arguments = ParseFunctionArguments();

    if (arguments.empty()) {
        Expected("Unexpected ',' found", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    for (auto &elt: arguments) {
        args.push_back(std::move(elt));
    }

    return args;
}

std::unique_ptr<FunctionDefinitionAST> Parser::ParseFunction() {
    StoreParserPosition();

    if (auto resultRetType = ParseFunctionWithRetType()) {
        return resultRetType;
    }

    BackTrack();

    if (auto result = ParseFunctionWithoutRetType()) {
        return result;
    }

    return nullptr;
}
// Function Parsing Ends here

// Parsing Variable Declaration Begins here

std::unique_ptr<VariableDeclarationAST> Parser::ParseVariableDeclWithLet() {
    StoreParserPosition();

    if (Peek(TokenName::LET)) {
        Consume();
    } else
        return {};

    std::vector<SourceLocation> locations;

    std::string var_name;
    if (Peek(TokenName::ID)) {
        Token current_tok = GetCurrentToken();
        Consume();
        var_name = current_tok.GetValue();
        locations.push_back(current_tok.GetSourceLocation());
    } else {
        if (Lexer::keywords.contains(GetCurrentToken().GetValue())) {
            status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
            Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                               " is a keyword, it cannot be used as a variable name.",
                       GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        Expected("A Variable name after a let expression is required", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return nullptr;
    }
    // push (0, 0) for now to mean none
    locations.emplace_back(0, 0);

    if (Peek(TokenName::SEMI_COLON))
        Consume();
    else {
        Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);

        IncrementErrorCount();

        return nullptr;
    }

    return std::make_unique<VariableDeclarationAST>("", var_name, locations);
}

std::unique_ptr<VariableDeclarationAST> Parser::ParseVariableDeclWithType() {
    StoreParserPosition();

    if (Peek(TokenName::LET)) {
        Consume();
    } else
        return {};

    std::vector<SourceLocation> locations;
    std::string var_name;
    if (Peek(TokenName::ID)) {
        Token current_tok = GetCurrentToken();
        Consume();
        var_name = current_tok.GetValue();
        locations.push_back(current_tok.GetSourceLocation());
    } else {
        if (Lexer::keywords.contains(GetCurrentToken().GetValue())) {
            status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
            Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                               " is a keyword, it cannot be used as a variable name.",
                       GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        Expected("A Variable name after a let expression is required", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::COLON))
        Consume();
    else
        return {};

    // FixME: add support for user defined type
    // later when support for classes are added.
    std::string type;
    if (Peek(TokenName::I32) || Peek(TokenName::I64) || Peek(TokenName::F32) || Peek(TokenName::F64)) {
        Token current_tok = GetCurrentToken();
        Consume();
        type = current_tok.GetValue();
        locations.push_back(current_tok.GetSourceLocation());
    } else {
        type = GetCurrentToken().GetValue();
        Expected("Expected a type name, found " + type, GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::SEMI_COLON))
        Consume();
    else {
        Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
        IncrementErrorCount();

        return {};
    }

    return std::make_unique<VariableDeclarationAST>(type, var_name, locations);
}

std::unique_ptr<VariableDeclarationAST> Parser::ParseVariableDecl() {
    auto result = ParseVariableDeclWithType();

    if (result) {
        return result;
    }

    BackTrack();
    result = ParseVariableDeclWithLet();

    if (result) {
        return result;
    }

    return {};
}

std::unique_ptr<VariableAssignmentAST> Parser::ParseVariableAssignment() {
    StoreParserPosition();

    std::vector<SourceLocation> locations;
    std::string var_name{};
    IdentifierAST id = {"", {}};
    if (Peek(TokenName::ID)) {
        Token current_tok = GetCurrentToken();
        Consume();
        var_name = current_tok.GetValue();
        locations.push_back(current_tok.GetSourceLocation());
        id = {var_name, locations[0]};
    } else
        return {};

    if (Peek(TokenName::ASSIGN))
        Consume();
    else
        return {};

    auto expr = ParseExpression();

    const auto &result = expr;

    const auto loc = result->GetSourceLocation();

    if (!result) {
        Expected("Expected an value or expression for the assignment", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }
    for (const auto elt: loc) {
        locations.push_back(elt);
    }
    if (Peek(TokenName::SEMI_COLON)) {
        Consume();
        return std::make_unique<VariableAssignmentAST>(id, std::move(expr), locations);
    } else {
        Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);
        IncrementErrorCount();

        return {};
    }
}

std::unique_ptr<VariableDeclareAndAssignAST> Parser::ParseVariableInitWithLet() {
    StoreParserPosition();

    std::vector<SourceLocation> locations;

    if (Peek(TokenName::LET))
        Consume();
    else
        return {};

    std::string var_name;

    if (Peek(TokenName::ID)) {
        var_name = GetCurrentToken().GetValue();
        locations.push_back(GetCurrentToken().GetSourceLocation());
        Consume();
    } else {
        StoreParserPosition();
        if (Lexer::keywords.contains(GetCurrentToken().GetValue())) {
            Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                               " is a keyword, it cannot be used as a variable name.",
                       GENERATE_CURRENT_POSITION);
            IncrementErrorCount();
            Consume();

            return {};
        }
        BackTrack();

        Expected("A Variable name after a let expression is required", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }
    if (Peek(TokenName::ASSIGN))
        Consume();
    else
        return {};

    // No Type,so type has same location as the var_name
    locations.push_back(locations[0]);
    auto expr = ParseExpression();

    if (!expr) {
        Expected("Expected an value or expression for the assignment", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    const auto &result = expr;

    for (const auto &elt: result->GetSourceLocation()) {
        locations.push_back(elt);
    }

    if (Peek(TokenName::SEMI_COLON)) {
        Consume();
    } else {
        Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);

        IncrementErrorCount();

        return {};
    }

    return std::make_unique<VariableDeclareAndAssignAST>(var_name, "", std::move(expr), locations);
}

std::unique_ptr<VariableDeclareAndAssignAST> Parser::ParseVariableInitWithType() {
    StoreParserPosition();

    if (Peek(TokenName::LET))
        Consume();
    else
        return {};

    std::string var_name;
    std::vector<SourceLocation> locations;
    if (Peek(TokenName::ID)) {
        Token current_tok = GetCurrentToken();
        Consume();
        var_name = current_tok.GetValue();
        locations.push_back(current_tok.GetSourceLocation());
    } else {
        StoreParserPosition();
        Consume();
        if (Lexer::keywords.contains(GetCurrentToken().GetValue())) {
            Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                               " is a keyword, it cannot be used as a variable name.",
                       GENERATE_CURRENT_POSITION);

            IncrementErrorCount();

            return {};
        }
        Expected("A Variable name after a let expression is required", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::COLON))
        Consume();
    else
        return {};

    std::string type;
    // FixME: Add support for user defined types
    // later when support for classes are added
    if (Peek(TokenName::I32) || Peek(TokenName::I64) || Peek(TokenName::F32) || Peek(TokenName::F64)) {
        Token current_tok = GetCurrentToken();
        Consume();
        type = current_tok.GetValue();
        locations.push_back(current_tok.GetSourceLocation());
    } else {
        type = GetCurrentToken().GetValue();
        Expected("Expected type name, found " + type, GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::ASSIGN))
        Consume();
    else
        return {};

    auto expr = ParseExpression();

    if (!expr) {
        Expected("Expected an value or expression for the assignment", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::SEMI_COLON)) {
        Consume();
    } else {
        Expected("Expected a ';' here.", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean(";", GENERATE_POSITION_PAST_ONE_COLUMN, false);

        IncrementErrorCount();

        return {};
    }

    return std::make_unique<VariableDeclareAndAssignAST>(var_name, type, std::move(expr), locations);
}

std::unique_ptr<ExpressionAST> Parser::ParseFunctionParameter() {
    StoreParserPosition();
    auto expr = ParseExpression();


    auto val = std::move(expr);

    return val;
}

std::unique_ptr<FunctionParameterAST> Parser::ParseFunctionParameters() {
    auto para = ParseFunctionParameter();
    auto val = std::move(para);

    if (!val)
        return {};

    std::vector<std::unique_ptr<ExpressionAST>> params;
    params.push_back(std::move(val));

    if (Peek(TokenName::COMMA))
        Consume();
    else {
        return std::make_unique<FunctionParameterAST>(std::move(params));
    }

    const auto parameters = ParseFunctionParameters();

    if (!parameters) {
        Expected("Unexpected ',' found", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    for (auto &elt: parameters->GetFunctionArguments()) {
        params.push_back(std::move(elt));
    }

    return std::make_unique<FunctionParameterAST>(std::move(params));
}
// Parsing Variable Declaration Ends here

std::unique_ptr<ExpressionAST> Parser::ParseIdentifier() {
    if (Peek(TokenName::ID)) {
        auto current_token = GetCurrentToken();
        Consume();
        return std::make_unique<IdentifierAST>(current_token.GetValue(), current_token.GetSourceLocation());
    }
    return {};
}

std::unique_ptr<ExpressionAST> Parser::ParseNumber() {
    bool isNegative{false};
    if (Peek(TokenName::MINUS)) {
        isNegative = true;
        Consume();
    }
    if (Peek(TokenName::NUMBER)) {
        Token current_tok = GetCurrentToken();
        Consume();
        const std::string num = current_tok.GetValue();
        const auto decimal_count = std::ranges::count(num.begin(), num.end(), '.');

        if (decimal_count > 1) {
            Unexpected("Too many decimal points in a numerical value", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }

        return std::make_unique<NumberAST>(isNegative ? "-" + num : num, static_cast<bool>(decimal_count),
                                           current_tok.GetSourceLocation());
    }
    return {};
}

std::unique_ptr<ExpressionAST> Parser::ParseParenExpr() {
    if (Peek(TokenName::OPEN_PARENTHESIS)) {
        Consume();
        auto expr = ParseExpression();
        if (!expr) {
            return nullptr;
        }
        if (!Peek(TokenName::CLOSE_PARENTHESIS)) {
            Expected("Expected a `)` here", GENERATE_POSITION_PAST_ONE_COLUMN);
            return {};
        }
        Consume();
        return expr;
    }

    return {};
}

std::unique_ptr<ExpressionAST> Parser::ParsePrimary() {
    if (Peek(TokenName::ID)) {
        StoreParserPosition();
        Consume();
        if (Peek(TokenName::OPEN_PARENTHESIS)) {
            BackTrack();
            return ParseFunctionCall();
        }
        BackTrack();
        return ParseIdentifier();
    }

    if (Peek(TokenName::NUMBER) || Peek(TokenName::MINUS)) {
        return ParseNumber();
    }

    if (Peek(TokenName::OPEN_PARENTHESIS)) {
        return ParseParenExpr();
    }


    return {};
}

std::unique_ptr<ExpressionAST> Parser::ParseBinaryOpRHS(int exprPrec, std::unique_ptr<ExpressionAST> LHS) {
    for (;;) {
        const std::string current_tok = GetCurrentToken().GetValue();
        SourceLocation op_loc = GetCurrentToken().GetSourceLocation();
        int current_tok_precedence = GetPrecedence(current_tok);

        if (current_tok_precedence < exprPrec) {
            return LHS;
        }

        const std::string &op = current_tok;

        Consume();
        auto RHS = ParsePrimary();

        if (!RHS) {
            // TODO: Report error
            return {};
        }

        const std::string next_tok = GetCurrentToken().GetValue();
        int next_tok_precedence = GetPrecedence(next_tok);

        if (current_tok_precedence < next_tok_precedence) {
            RHS = ParseBinaryOpRHS(current_tok_precedence + 1, std::move(RHS));
            if (!RHS) {
                return nullptr;
            }
        }
        auto loc = LHS->GetSourceLocation();
        std::vector<SourceLocation> loc_vec = {loc.front()};
        LHS = std::make_unique<BinaryExpressionAST>(std::move(LHS), std::move(RHS), OperatorNode(op, op_loc), loc_vec);
    }
}

std::unique_ptr<ExpressionAST> Parser::ParseExpression() {
    auto LHS = ParsePrimary();
    if (!LHS) {
        return {};
    }
    return ParseBinaryOpRHS(0, std::move(LHS));
}

std::unique_ptr<RangeAST> Parser::ParseRange() {
    StoreParserPosition();
    auto start = ParseExpression();

    if (!start) {
        Expected("Expected an expression here", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    std::vector<SourceLocation> locations(start->GetSourceLocation().begin(), start->GetSourceLocation().end());

    if (Peek(TokenName::TO)) {
        Consume();
    } else {
        Expected("Expected the 'to' keyword.", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean("to", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();
    }

    auto end = ParseExpression();

    if (!end) {
        Expected("Expected an expression here", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    for (const auto &elt: end->GetSourceLocation()) {
        locations.push_back(elt);
    }

    // todo: return range
    return std::make_unique<RangeAST>(std::move(start), std::move(end), locations);
}

std::vector<std::unique_ptr<StatementAST>> Parser::ParseCurlyBraceAndBody() {
    int curly_brace_position;

    if (Peek(TokenName::OPEN_CURLY)) {
        curly_brace_position = static_cast<int>(current_parser_position) + 1;
        Consume();
    } else {
        Consume();
        Unexpected("Expected a '{' here", GENERATE_CURRENT_POSITION);
        IncrementErrorCount();

        return {};
    }

    auto body = ParseStatements();

    if (body.empty())
        return {};

    if (Peek(TokenName::CLOSE_CURLY)) {
        Consume();
        return body;
    } else {
        Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(),
                 token_vec[curly_brace_position].GetColumn());
        IncrementErrorCount();

        return {};
    }
}

std::unique_ptr<LoopAST> Parser::ParseLoop() {
    StoreParserPosition();

    if (Peek(TokenName::LOOP)) {
        Consume();
        if (!CheckInsideFunction()) {
            Unexpected("A 'loop' must be inside a function definition", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        status_list.push_back(ParserStatus::PARSING_LOOP);

        auto body = ParseCurlyBraceAndBody();

        if (!body.empty()) {
            status_list.pop_back();
            return std::make_unique<LoopAST>(std::move(body));
        }
    }
    return {};
}

std::unique_ptr<ForLoopAST> Parser::ParseForLoop() {
    StoreParserPosition();

    if (Peek(TokenName::FOR)) {
        Consume();
        if (!CheckInsideFunction()) {
            Unexpected("A 'for loop' must be inside a function definition", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        status_list.push_back(ParserStatus::PARSING_FOR_LOOP);

    } else
        return {};
    SourceLocation loc = GetCurrentToken().GetSourceLocation();
    std::unique_ptr<IdentifierAST> iteration_variable;

    if (Peek(TokenName::ID)) {
        Token current_tok = GetCurrentToken();
        Consume();
        iteration_variable = std::make_unique<IdentifierAST>(current_tok.GetValue(), current_tok.GetSourceLocation());
    } else {
        StoreParserPosition();
        if (Lexer::keywords.contains(GetCurrentToken().GetValue())) {
            status_list.push_back(ParserStatus::PARSING_FN_DEFINITION_FAILED);
            Unexpected("'" + GetCurrentToken().GetValue() + "'" +
                               " is a keyword, it cannot be used as an iteration variable.",
                       GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        BackTrack();

        Unexpected("Expected an iteration variable name after the 'for' keyword", GENERATE_CURRENT_POSITION);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::IN))
        Consume();
    else {
        Expected("You missed the 'in' keyword in the for loop", GENERATE_POSITION_PAST_ONE_COLUMN);
        DidYouMean("in", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    auto range = ParseRange();

    if (!range)
        return {};

    auto body = ParseCurlyBraceAndBody();

    if (body.empty())
        return {};

    status_list.pop_back();

    return std::make_unique<ForLoopAST>(std::move(iteration_variable), std::move(range), std::move(body), loc);
}

std::unique_ptr<WhileLoopAST> Parser::ParseWhileLoop() {
    StoreParserPosition();

    if (Peek(TokenName::WHILE)) {
        Consume();
        if (!CheckInsideFunction()) {
            Unexpected("A 'while loop' must be inside a function definition", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        status_list.push_back(ParserStatus::PARSING_WHILE_LOOP);
    } else
        return {};

    auto loop_condition = ParseExpression();
    if (!loop_condition) {
        Expected("Expected an expression after the 'while' keyword", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    auto body = ParseCurlyBraceAndBody();

    if (!body.empty()) {

        status_list.pop_back();

        return std::make_unique<WhileLoopAST>(std::move(loop_condition), std::move(body));

    } else
        return {};
}

std::vector<std::unique_ptr<MatchArmAST>> Parser::ParseMatchArms() {
    StoreParserPosition();

    std::vector<std::unique_ptr<MatchArmAST>> arm_vec;

    auto arm = ParseMatchArm();

    if (!arm)
        return {};

    arm_vec.emplace_back(std::move(arm));

    if (auto other_arms = ParseMatchArms(); !other_arms.empty()) {
        for (auto other_arm = std::move(other_arms); auto &a: other_arm) {
            arm_vec.push_back(std::move(a));
        }
        return arm_vec;
    } else {
        return arm_vec;
    }
}

std::unique_ptr<MatchArmAST> Parser::ParseMatchArm() {
    StoreParserPosition();

    auto cond = ParseExpression();

    if (!cond)
        return {};

    if (Peek(TokenName::ARROW)) {
        Consume();
    } else {
        Expected("Expected an '=>' after the expression in 'match' body", GENERATE_CURRENT_POSITION);
        DidYouMean("=>", GENERATE_CURRENT_POSITION);
        IncrementErrorCount();

        return {};
    }

    auto arm_body = ParseCurlyBraceAndBody();

    if (arm_body.empty())
        return {};
    else
        return std::make_unique<MatchArmAST>(std::move(cond), std::move(arm_body));
}

std::unique_ptr<MatchStatementAST> Parser::ParseMatchStatement() {
    StoreParserPosition();
    int curly_brace_position;

    if (Peek(TokenName::MATCH)) {
        Consume();
        if (!CheckInsideFunction()) {
            Unexpected("A 'match statement' must be inside a function definition", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
    } else
        return {};

    if (!Peek(TokenName::ALL) && !Peek(TokenName::ONCE)) {
        Expected("Expected one of 'all' or 'once' after the 'match' keyword", GENERATE_CURRENT_POSITION);
        IncrementErrorCount();

        return {};
    }

    const std::string match_type = GetCurrentToken().GetValue();
    Consume();

    if (Peek(TokenName::OPEN_CURLY)) {
        curly_brace_position = static_cast<int>(current_parser_position) + 1;
        Consume();
    } else {
        Expected("Expected an '{' after the expression in match", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    auto arms = ParseMatchArms();

    if (arms.empty()) {
        Expected("Expected match arms for the match statement", GENERATE_POSITION_PAST_ONE_COLUMN);
        return {};
    }

    if (Peek(TokenName::CLOSE_CURLY)) {
        Consume();
        MatchType type = match_type == "all" ? MatchType::ALL : MatchType::ONCE;
        return std::make_unique<MatchStatementAST>(type, std::move(arms));
    }

    Expected("Expected a closing pair for '{'", token_vec[curly_brace_position].GetLine(),
             token_vec[curly_brace_position].GetColumn());
    IncrementErrorCount();

    return {};
}

std::unique_ptr<IfStatementAST> Parser::ParseIfStatement() {
    StoreParserPosition();
    SourceLocation loc;
    if (Peek(TokenName::IF)) {
        Consume();
        if (!CheckInsideFunction()) {
            Unexpected("'if statement' must be inside a function definition", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        loc = GetCurrentToken().GetSourceLocation();
        status_list.push_back(ParserStatus::PARSING_IF_STATEMENT);
    } else
        return {};

    auto condition = ParseExpression();

    if (!condition) {
        Expected("Expected an condition after the 'if' keyword", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    auto body = ParseCurlyBraceAndBody();

    if (!body.empty()) {
        status_list.push_back(ParserStatus::PARSED_IF_STATEMENT);

        std::vector<std::unique_ptr<ElseIfStatementAST>> elseifVec;
        auto else_if_val = ParseElseIfStatement();
        while (else_if_val) {
            elseifVec.push_back(std::move(else_if_val));
            else_if_val = ParseElseIfStatement();
        }
        BackTrack();
        if (auto else_val = ParseElseStatement(); else_val) {
            return std::make_unique<IfStatementAST>(std::move(condition), std::move(body), elseifVec,
                                                    std::move(else_val), loc);
        } else {
            return std::make_unique<IfStatementAST>(std::move(condition), std::move(body), elseifVec, nullptr, loc);
        }

    } else {
        status_list.push_back(ParserStatus::PARSING_IF_STATEMENT_FAILED);
        return {};
    }
}

std::unique_ptr<ElseIfStatementAST> Parser::ParseElseIfStatement() {
    StoreParserPosition();
    SourceLocation loc;
    if (Peek(TokenName::ELSE)) {
        Consume();
        loc = GetCurrentToken().GetSourceLocation();
    } else
        return {};

    if (Peek(TokenName::IF)) {
        Consume();
        if (!CheckInsideFunction()) {
            Unexpected("'else if' must be inside a function definition", GENERATE_CURRENT_POSITION, 7);
            IncrementErrorCount();

            return {};
        }
        // Check if we have an immediate else after either else-if or if statement
        if (status_list.back() != ParserStatus::PARSED_IF_STATEMENT &&
            status_list.back() != ParserStatus::PARSED_ELSEIF_STATEMENT) {
            Unexpected("'else if' without an previous 'if' found", GENERATE_CURRENT_POSITION, 7);
            IncrementErrorCount();

            return {};
        }

        status_list.push_back(ParserStatus::PARSING_ELSEIF_STATEMENT);
    } else
        return {};

    auto condition = ParseExpression();

    if (!condition) {
        Expected("Expected an condition after 'else-if'", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    auto body = ParseCurlyBraceAndBody();

    if (!body.empty()) {
        // do sth

        status_list.push_back(ParserStatus::PARSED_ELSEIF_STATEMENT);
        return std::make_unique<ElseIfStatementAST>(std::move(condition), std::move(body), loc);
    } else
        return {};
}

std::unique_ptr<ElseStatementAST> Parser::ParseElseStatement() {
    StoreParserPosition();

    if (Peek(TokenName::ELSE)) {
        Consume();
        if (!CheckInsideFunction()) {
            Unexpected("else' must be inside a function definition", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        // Check if we have an immediate else after either else-if or if statement
        if (status_list.back() != ParserStatus::PARSED_IF_STATEMENT &&
            status_list.back() != ParserStatus::PARSED_ELSEIF_STATEMENT) {
            Unexpected("else without an previous 'if' found", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();

            return {};
        }
        SourceLocation loc = GetCurrentToken().GetSourceLocation();
        auto body = ParseCurlyBraceAndBody();

        if (!body.empty()) {
            return std::make_unique<ElseStatementAST>(std::move(body), loc);
        } else
            return {};

    } else
        return {};
}

std::unique_ptr<BreakStatementAST> Parser::ParseBreakStatement() {
    StoreParserPosition();
    if (Peek(TokenName::BREAK)) {
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
                Unexpected("'break' not within a loop body", GENERATE_CURRENT_POSITION, 5);
                IncrementErrorCount();

                return {};
            }
        }
    } else
        return {};

    SourceLocation loc = GetCurrentToken().GetSourceLocation();
    Consume();

    if (status_list.empty()) {
        Unexpected("'break' not within a loop body", GENERATE_CURRENT_POSITION, 5);
        IncrementErrorCount();

        return {};
    }

    if (Peek(TokenName::SEMI_COLON)) {
        Consume();
        return std::make_unique<BreakStatementAST>(loc);
    } else {
        Expected("Expected semicolon after 'break'", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }
}

std::unique_ptr<FunctionCallExprAST> Parser::ParseFunctionCall() {
    StoreParserPosition();
    std::unique_ptr<IdentifierAST> fn_name;
    SourceLocation loc;
    if (Peek(TokenName::ID)) {
        Token current_tok = GetCurrentToken();
        Consume();
        fn_name = std::make_unique<IdentifierAST>(current_tok.GetValue(), current_tok.GetSourceLocation());
        loc = current_tok.GetSourceLocation();
    } else
        return {};

    if (Peek(TokenName::OPEN_PARENTHESIS)) {
        Consume();
    } else
        return {};

    auto params = ParseFunctionParameters();

    if (Peek(TokenName::CLOSE_PARENTHESIS)) {
        Consume();
    } else {
        Expected("Expected closing pair for '('", GENERATE_POSITION_PAST_ONE_COLUMN);
        IncrementErrorCount();

        return {};
    }

    // if (Peek(TokenName::SEMI_COLON)) {
    //     Consume();
    // } else {
    //     Expected("Expected a semi colon : ';'", GENERATE_POSITION_PAST_ONE_COLUMN);
    //     IncrementErrorCount();
    //
    //     return {};
    // }

    return std::make_unique<FunctionCallExprAST>(std::move(fn_name), std::move(params), loc);
}

std::unique_ptr<ReturnStatementAST> Parser::ParseReturnStatement() {
    StoreParserPosition();
    if (Peek(TokenName::RETURN)) {
        Consume();
        if (!CheckInsideFunction()) {
            Unexpected("A `return` statement cannot be outside a function body.", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();
            return {};
        }

        auto expr = ParseExpression();

        if (!expr) {
            Expected("Expected an expression after the `return` keyword", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();
            return {};
        }

        if (!Peek(TokenName::SEMI_COLON)) {
            Expected("Expected a `;` here", GENERATE_CURRENT_POSITION);
            IncrementErrorCount();
            return {};
        }
        Consume();
        return std::make_unique<ReturnStatementAST>(std::move(expr), expr->GetSourceLocation().front());
    }

    return {};
}

std::unique_ptr<StatementAST> Parser::ParseStatement() {
    StoreParserPosition();

    std::unique_ptr<StatementAST> result = ParseForLoop();
    if (result) {
        // can do sth here later
        return result;
    }

    BackTrack();

    result = ParseWhileLoop();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseLoop();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseVariableInitWithType();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseBreakStatement();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseReturnStatement();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseVariableInitWithLet();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseVariableDecl();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseVariableAssignment();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseMatchStatement();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseIfStatement();
    if (result) {
        return result;
    }

    BackTrack();

    result = ParseFunction();
    if (result) {
        return result;
    }

    BackTrack();

    // result = ParseFunctionCallStatement();
    // if (result) {
    //     return result;
    // }

    return {};
}

std::vector<std::unique_ptr<StatementAST>> Parser::ParseStatements() {
    StoreParserPosition();

    std::vector<std::unique_ptr<StatementAST>> stmts;


    if (auto result = ParseStatement(); result) {
        stmts.push_back(std::move(result));

        auto new_result = ParseStatements();

        if (!new_result.empty()) {
            auto stmt = std::move(new_result);
            for (auto &elt: stmt) {
                stmts.push_back(std::move(elt));
            }
            return stmts;
        } else
            return stmts;
    } else
        return stmts;
}

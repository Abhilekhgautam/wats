#include "tokens.hpp"

std::ostream& operator<<(std::ostream& os, TokenType type) {
  switch (type) {
    case TokenType::PLUS: os << "+"; break;
    case TokenType::MINUS: os << "-"; break;
    case TokenType::MUL: os << "*"; break;
    case TokenType::DIV: os << "/"; break;
    case TokenType::MOD: os << "%"; break;
    case TokenType::EQ: os << "EQ"; break;
    case TokenType::NOT: os << "NOT"; break;
    case TokenType::NEQ: os << "NEQ"; break;
    case TokenType::LT: os << "LT"; break;
    case TokenType::GT: os << "GT"; break;
    case TokenType::LTE: os << "LTE"; break;
    case TokenType::GTE: os << "GTE"; break;
    case TokenType::ID: os << "ID"; break;
    case TokenType::NUM: os << "NUM"; break;
    case TokenType::OPEN_PARENTHESIS: os << "OPEN_PARENTHESIS"; break;
    case TokenType::CLOSE_PARENTHESIS: os << "CLOSE_PARENTHESIS"; break;
    case TokenType::OPEN_SQUARE: os << "OPEN_SQUARE"; break;
    case TokenType::CLOSE_SQUARE: os << "CLOSE_SQUARE"; break;
    case TokenType::OPEN_CURLY: os << "OPEN_CURLY"; break;
    case TokenType::CLOSE_CURLY: os << "CLOSE_CURLY"; break;
    case TokenType::COMMA: os << "COMMA"; break;
    case TokenType::ASSIGN: os << "ASSIGN"; break;
    case TokenType::NUMBER: os << "NUMBER"; break;
    case TokenType::EXPORT: os << "EXPORT"; break;
    case TokenType::IMPORT: os << "IMPORT"; break;
    case TokenType::I32: os << "I32"; break;
    case TokenType::I64: os << "I64"; break;
    case TokenType::F32: os << "F32"; break;
    case TokenType::F64: os << "F64"; break;
    case TokenType::LOOP: os << "LOOP"; break;
    case TokenType::FUNCTION: os << "FUNCTION"; break;
    case TokenType::CONST: os << "CONST"; break;
    case TokenType::WHERE: os << "WHERE"; break;
    case TokenType::FROM: os << "FROM"; break;
    case TokenType::TO: os << "TO"; break;
    case TokenType::FOR: os << "FOR"; break;
    case TokenType::IN: os << "IN"; break;
    case TokenType::LET: os << "LET"; break;
    case TokenType::WHILE: os << "WHILE"; break;
    default: os << "Unknown"; break;
  }
  return os;
}

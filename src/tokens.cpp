#include "tokens.hpp"

std::ostream& operator<<(std::ostream& os, TokenName type) {
  switch (type) {
    case TokenName::PLUS: os << "+"; break;
    case TokenName::MINUS: os << "-"; break;
    case TokenName::MUL: os << "*"; break;
    case TokenName::DIV: os << "/"; break;
    case TokenName::MOD: os << "%"; break;
    case TokenName::EQ: os << "EQ"; break;
    case TokenName::NOT: os << "NOT"; break;
    case TokenName::NEQ: os << "NEQ"; break;
    case TokenName::LT: os << "LT"; break;
    case TokenName::GT: os << "GT"; break;
    case TokenName::LTE: os << "LTE"; break;
    case TokenName::GTE: os << "GTE"; break;
    case TokenName::ID: os << "ID"; break;
    case TokenName::NUM: os << "NUM"; break;
    case TokenName::OPEN_PARENTHESIS: os << "OPEN_PARENTHESIS"; break;
    case TokenName::CLOSE_PARENTHESIS: os << "CLOSE_PARENTHESIS"; break;
    case TokenName::OPEN_SQUARE: os << "OPEN_SQUARE"; break;
    case TokenName::CLOSE_SQUARE: os << "CLOSE_SQUARE"; break;
    case TokenName::OPEN_CURLY: os << "OPEN_CURLY"; break;
    case TokenName::CLOSE_CURLY: os << "CLOSE_CURLY"; break;
    case TokenName::COMMA: os << "COMMA"; break;
    case TokenName::ASSIGN: os << "ASSIGN"; break;
    case TokenName::NUMBER: os << "NUMBER"; break;
    case TokenName::EXPORT: os << "EXPORT"; break;
    case TokenName::IMPORT: os << "IMPORT"; break;
    case TokenName::I32: os << "I32"; break;
    case TokenName::I64: os << "I64"; break;
    case TokenName::F32: os << "F32"; break;
    case TokenName::F64: os << "F64"; break;
    case TokenName::LOOP: os << "LOOP"; break;
    case TokenName::FUNCTION: os << "FUNCTION"; break;
    case TokenName::CONST: os << "CONST"; break;
    case TokenName::WHERE: os << "WHERE"; break;
    case TokenName::FROM: os << "FROM"; break;
    case TokenName::TO: os << "TO"; break;
    case TokenName::FOR: os << "FOR"; break;
    case TokenName::IN: os << "IN"; break;
    case TokenName::LET: os << "LET"; break;
    case TokenName::WHILE: os << "WHILE"; break;
    default: os << "Unknown"; break;
  }
  return os;
}

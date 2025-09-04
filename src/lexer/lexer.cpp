#include "lexer.hpp"
#include "../utils.hpp"
#include "tokens.hpp"

#include <format>
#include <iostream>
#include <stdexcept>

std::map<std::string, TokenName> Lexer::keywords = {
    {"export", TokenName::EXPORT}, {"function", TokenName::FUNCTION},
    {"where", TokenName::WHERE},   {"from", TokenName::FROM},
    {"import", TokenName::IMPORT}, {"i32", TokenName::I32},
    {"i64", TokenName::I64},       {"f32", TokenName::F32},
    {"f64", TokenName::F64},       {"let", TokenName::LET},
    {"const", TokenName::CONST},   {"to", TokenName::TO},
    {"in", TokenName::IN},         {"for", TokenName::FOR},
    {"while", TokenName::WHILE},   {"loop", TokenName::LOOP},
    {"match", TokenName::MATCH},   {"if", TokenName::IF},
    {"else", TokenName::ELSE},     {"break", TokenName::BREAK}};

Lexer::Lexer(CompilerContext &context) : context(context) {
  column = 1;
  line = 1;
  current_scan_position = 0;
}

bool Lexer::IsAtEnd() {
  return current_scan_position >= context.source_code.length();
}

void Lexer::Tokenize() {
  while (!IsAtEnd()) {
    char current_char;
    try {
      current_char = context.source_code.at(current_scan_position);
    } catch (std::out_of_range &e) {
      return;
    }
    ScanToken(current_char);
    current_scan_position = current_scan_position + 1;
  }
}

char Lexer::Peek() {
  if ((current_scan_position + 1) > context.source_code.size())
    return '\0';

  else
    return context.source_code[current_scan_position + 1];
}

char Lexer::PeekNext() {
  if ((current_scan_position + 2) > context.source_code.size())
    return '\0';
  else
    return context.source_code.at(current_scan_position + 2);
}

void Lexer::ConsumeNext() { current_scan_position = current_scan_position + 1; }

void Lexer::AddToken(Token tok) { token_vec.push_back(tok); }

void Lexer::ScanToken(const char c) {
  switch (c) {
  case '\n': {
    line = line + 1;
    column = 1;
    break;
  }
  case ' ': {
    column = column + 1;
    break;
  }
  case ':': {
    AddToken(Token{TokenName::COLON, {line, column}, c});
    column = column + 1;
    break;
  }

  case ';': {
    AddToken(Token{TokenName::SEMI_COLON, {line, column}, c});
    column = column + 1;
    break;
  }

  case '+': {
    AddToken(Token{TokenName::PLUS, {line, column}, c});
    column = column + 1;
    break;
  }
  case '-': {
    AddToken(Token{TokenName::MINUS, {line, column}, c});
    column = column + 1;
    break;
  }
  case '*': {
    AddToken(Token{TokenName::MUL, {line, column}, c});
    column = column + 1;
    break;
  }
  case '/': {
    AddToken(Token{TokenName::DIV, {line, column}, c});
    column = column + 1;
    break;
  }
  case '%': {
    AddToken({TokenName::MOD, {line, column}, c});
    column = column + 1;
    break;
  }
  case '(': {
    AddToken(Token{TokenName::OPEN_PARENTHESIS, {line, column}, c});
    column = column + 1;
    break;
  }
  case ')': {
    AddToken(Token{TokenName::CLOSE_PARENTHESIS, {line, column}, c});
    column = column + 1;
    break;
  }
  case '{': {
    AddToken(Token{TokenName::OPEN_CURLY, {line, column}, c});
    column = column + 1;
    break;
  }
  case '}': {
    AddToken(Token{TokenName::CLOSE_CURLY, {line, column}, c});
    column = column + 1;
    break;
  }
  case '[': {
    AddToken(Token{TokenName::OPEN_SQUARE, {line, column}, c});
    column = column + 1;
    break;
  }
  case ']': {
    AddToken(Token{TokenName::CLOSE_SQUARE, {line, column}, c});
    column = column + 1;
    break;
  }
  case '#': {
    while (!(Peek() == '\n')) {
      ConsumeNext(); // skip comment;
    }
    column = column + 1;
    break;
  }
  case ',': {
    AddToken(Token{TokenName::COMMA, {line, column}, c});
    column = column + 1;
    break;
  }
  case '=': {
    if (Peek() == '=') {
      AddToken(Token{TokenName::EQ, {line, column}, "=="});
      ConsumeNext();
      column = column + 2;
    } else if (Peek() == '>') {
      AddToken(Token{TokenName::ARROW, {line, column + 1}, "=>"});
      ConsumeNext();
      column = column + 2;
    } else {
      AddToken(Token{TokenName::ASSIGN, {line, column}, c});
      column = column + 1;
    }

    break;
  }
  case '<': {
    if (Peek() == '=') {
      AddToken(Token{TokenName::LTE, {line, column}, "<="});
      ConsumeNext();
      column = column + 2;
    } else {
      AddToken(Token{TokenName::LT, {line, column}, c});
      column = column + 1;
    }
    break;
  }
  case '>': {
    if (Peek() == '=') {
      AddToken(Token{TokenName::GTE, {line, column}, ">="});
      ConsumeNext();
      column = column + 2;
    } else {
      AddToken(Token{TokenName::GT, {line, column}, c});
      ConsumeNext();
      column = column + 1;
    }

    break;
  }
  case '!': {
    if (Peek() == '=') {
      AddToken(Token{TokenName::NEQ, {line, column}, "!="});
      ConsumeNext();
      column = column + 2;
    } else {
      AddToken(Token{TokenName::NOT, {line, column}, c});
      column = column + 1;
    }
    break;
  }
  /*
  case '"': {
      std::string str = String();
      // TODO: Add string token
      std::cout << str << '\n';

      break;
  }
  */
  default:
    // handle numbers
    if (isdigit(c)) {
      std::string number = std::format("{}{}", c, Number());
      AddToken(Token{TokenName::NUMBER, {line, column}, number});
      column = column + 1;
    } else if (isalnum(c)) { // handle id and keywords
      std::string id = std::format("{}{}", c, Identifier());

      auto found = keywords.find(id);

      if (found != keywords.end()) {
        AddToken(Token{found->second, {line, column}, found->first});
      } else
        AddToken(Token{TokenName::ID, {line, column}, id});

      column = column + 1;

    } else {
      std::string err_msg = std::format("Unkown Token: '{}'", c);
      Error(err_msg);
    }
  }
}

std::string Lexer::Number() {
  std::string num_str = "";
  while (isdigit(Peek()) || Peek() == '.') {
    num_str.push_back(Peek());
    ConsumeNext();
    column = column + 1;
  }
  return num_str;
}

std::string Lexer::Identifier() {
  std::string id_str = "";
  while (isalnum(Peek()) || Peek() == '_') {
    id_str.push_back(Peek());
    ConsumeNext();
    column = column + 1;
  }
  return id_str;
}

// Fixme: Doesn't work
std::string Lexer::String() {
  std::string str;
  while (!IsAtEnd() && Peek() != '"') {
    if (Peek() == '\\') {
      ConsumeNext();
      column = column + 1;
      if (!IsAtEnd()) {
        char escapedChar [[maybe_unused]] = Peek();
        str.push_back(Peek());
        ConsumeNext();
        column = column + 1;
      } else {
        Error("Unterminated \"");
      }
      continue;
    }
    str.push_back(Peek());
    ConsumeNext();
    column = column + 1;

    if (IsAtEnd()) {
      Error("Unterminated \".");
    }
  }
  return str;
}

void Lexer::Error(const std::string &err_msg) {
  std::cout << context.source_code_by_line[line - 1] << '\n';
  Color("green", SetArrow(column), true);
  std::cout << "[ " << line << ":" << column << " ] ";
  Color("red", "Error: ");
  Color("blue", err_msg);

  IncrementErrorCount();
  std::exit(0);
}

const std::vector<Token> &Lexer::GetTokens() { return token_vec; }

void Lexer::Debug() {
  for (const auto &elt : token_vec) {
    std::cout << elt << '\n';
  }
}

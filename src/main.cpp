#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "cmd-parser.hpp"
#include "./lexer/lexer.hpp"
#include "./parser/parser.hpp"
#include "./semantics/semanticAnalyzer.hpp"

#include "utils.hpp"

#ifdef __EMSCRIPTEN__
extern "C" {
EMSCRIPTEN_KEEPALIVE
void compile_program(const char *source_code) {
  std::string test(source_code);

  Lexer L(test);
  L.Tokenize();

  Parser P(L.GetTokens(), L.GetSourceCode());
  auto ast_vec = P.Parse();

  SemanticAnalyzer s(ast_vec.value());
  s.analyze();

  // Only for debug
  if (ast_vec.has_value()) {
    auto stmt_vec = std::move(ast_vec.value());
    for (const auto &elt : stmt_vec) {
      elt->Debug();
    }
  }
}
}
#endif

int main(int argc, char **argv) {

  if (!verify_args(argc, argv)) {
    return 0;
  }

  std::string test = read_file(std::filesystem::path{argv[1]});

  Lexer L(test);
  L.Tokenize();
  L.Debug();

  Parser P(L.GetTokens(), L.GetSourceCode());
  auto ast_vec = P.Parse();

  SemanticAnalyzer s(ast_vec.value());
  s.analyze();

  // Only for debug
  if (ast_vec.has_value()) {
    auto stmt_vec = std::move(ast_vec.value());
    for (const auto &elt : stmt_vec) {
      elt->Debug();
    }
  }
}

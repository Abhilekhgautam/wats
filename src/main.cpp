#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "./lexer/lexer.hpp"
#include "./parser/parser.hpp"
#include "./semantics/semanticAnalyzer.hpp"
#include "CompilerContext.hpp"
#include "cmd-parser.hpp"

#include "utils.hpp"

#ifdef __EMSCRIPTEN__
extern "C" {
EMSCRIPTEN_KEEPALIVE
void compile_program(const char *source_code) {
  std::string source(source_code);

  CompilerContext context(source);

  Lexer L(context);
  L.Tokenize();

  Parser P(context, L.GetTokens());
  auto ast_vec = P.Parse();

  SemanticAnalyzer s(context, ast_vec.value());
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

  std::string source_code = read_file(std::filesystem::path{argv[1]});

  CompilerContext context(source_code);

  Lexer L(context);
  L.Tokenize();
  L.Debug();

  Parser P(context, L.GetTokens());
  auto ast_vec = P.Parse();

  SemanticAnalyzer s(context, ast_vec.value());
  s.analyze();

  // Only for debug
  if (ast_vec.has_value()) {
    auto stmt_vec = std::move(ast_vec.value());
    for (const auto &elt : stmt_vec) {
      elt->Debug();
    }
  }
}

#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "cmd-parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

#ifdef __EMSCRIPTEN__
extern "C" {
EMSCRIPTEN_KEEPALIVE
void compile_program(const char *source_code) {
  std::string test(source_code);

  Lexer L(test);
  L.Tokenize();

  Parser P(L.GetTokens(), L.GetSourceCode());
  P.Parse();
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
  P.Parse();
}

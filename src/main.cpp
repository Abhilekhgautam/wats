#include "cmd-parser.hpp"
#include "lexer.hpp"
#include "parser.hpp"
#include "utils.hpp"

int main(int argc, char **argv) {

  if (!verify_args(argc, argv)){
     return 0;	  
  }

  std::string test = read_file(std::filesystem::path{argv[1]});

  Lexer L(test);
  L.Tokenize();
  L.Debug();

  Parser P(L.GetTokens(), L.GetSourceCode());
  P.Parse();
}

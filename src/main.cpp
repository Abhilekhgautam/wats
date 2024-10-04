#include "cmd-parser.hpp"
#include "semantics.hpp"
#include "lexer.hpp"

int main(int argc, char **argv) {

  if (!verify_args(argc, argv)){
     return 0;	  
  }

  std::string test = "# this is a comment.\n"
	             "+ * < ( ) int x = 5;";

  Lexer L(test);
  L.Tokenize();
 // Func f(false, "add", ObjType::FUNC, true, {"i32"}, "i32");

  //f.write();

}

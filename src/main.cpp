#include "utils.hpp"
#include "cmd-parser.hpp"
#include "semantics.hpp"

int main(int argc, char **argv) {

  if (!verify_args(argc, argv)){
     return 0;	  
  }

  Func f(true, "add", ObjType::FUNC, true, {"i32", "i32"}, "i32");

  write(&f);

}

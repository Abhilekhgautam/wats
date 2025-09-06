# Wats

Wats is an experimental language designed to provide excellent error reporting along with modern programming language features.

# Try Online
Wats can be built to Web Assembly that allows running wats compiler directly on the web.
Run your code online on [Wats Playground](https://abhilekhgautam.github.io/wats-playground/src/playground.html)

# Current Implementation Status
- Lexer and Parser working neat.
- Recursive Decent Parser.
- Work in progress for AST (Parser and AST)
- Type System based on HM algorithm
- Generates Bril as an IR.

# Better Error Reporting, How?
- I have a assumption and trying to prove it. Work under progress.
- Whenever possible, the compiler provides hint to solve syntax error.

# Build Instruction [Native]
 - Ensure you have make and a C++ compiler that supports at least C++20
 - Clone the repo.
 - From root directory run:
   ```make```
# Build Instruction [WASM]
  - Ensure you have make and the latest emsdk (for emscripten) installed.
  - clone the repo
  - From root directory run:
     ```make wasm ```

# Demo Program
```
function test (){
  # automatic type deduction
  let x = 45;

  # can also explicitly mention type
  let x: i32 = 45;

  if x < 45 {}
  else if x < 30 {}
  else {}

  # pattern matching
  match x {
   x < 50 => {}
   x > 50 => {}
   x == 50 => {}
  }

  # infinite loop
  loop {
   if x > 1000{
     break;
   }
   x = x + 1;
  }

  # for loop
  for i in 1 to 100{
     # do whatever
  }

  # while loop
  while x < 30000 {
   # do whatever
  }

}
```


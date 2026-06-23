# Wats

Wats is a compiler front-end and optimizer targeting Bril IR. It has nothing to do with [Wat](https://www.destroyallsoftware.com/talks/wat)
# Try Online
Wats can be built to Web Assembly that allows running wats compiler directly on the web.
Run your code online on [Wats Playground](https://abhilekhgautam.github.io/wats-playground/src/playground.html)

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
function factorial (num: i32) -> i32{
  let result = 1;
  
  for i in 2 to num {
      result = result * i;
  }
  
  return result;
}
```
# Compiling a Wats Program

```bash
wats -p <pass-name> <file-path>
```



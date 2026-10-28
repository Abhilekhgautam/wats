// created by void at 2026-10-17 12:06:17
#include "bytewriter.hpp"
#include "modules.hpp"
#include "types.hpp"

#include <cassert>
#include <initializer_list>
#include <string>
#include <fstream>
#include <iostream>

inline bool write_file(const std::string& path, const std::vector<std::uint8_t>& w) {
    std::ofstream out(path, std::ios::binary);
    if (!out) return false;
    out.write(reinterpret_cast<const char*>(w.data()),
              static_cast<std::streamsize>(w.size()));
    return out.good();
}

// (module
//   (func (export "main") (result i32)
//     i32.const 42
//   )
// )
int test_module(){
  
   Module m;

   FuncType ft = {{}, {wasm::ValType::I32}};

   FunctionBody fb = {{}, {0x41, 0, 0x0B}};

   auto idx = m.add_function(ft, fb);

   m.add_export("main", idx);

   bool success = write_file("test.wasm", m.Serialize());

   if (success){
       std::cout << "File Saved to test.wasm.\n";
   }
   else {
       std::cout << "Failed writing to a file.\n";
   }
}

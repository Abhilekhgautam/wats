// created by void at 2026-10-18 11:47:30
#include "modules.hpp"
#include "bytewriter.hpp"
#include "types.hpp"
#include "section.hpp"

#include <cstdint>
#include <initializer_list>
#include <vector>

std::size_t TypeSection::size(){return func_type.size();}

std::string TypeSection::encode_key(const FuncType &t){
    std::string key;
    key.push_back(static_cast<char>(t.params.size()));
    for(const auto elt: t.params){
        key.push_back(static_cast<char>(elt));
    }
    key.push_back(static_cast<char>(t.results.size()));
    for(const auto elt: t.results){
        key.push_back(static_cast<char>(elt));
    }
    return key;
}

std::uint32_t TypeSection::intern(FuncType &t){
    const std::string key = encode_key(t);

    if(auto it = type_indices.find(key); it  == type_indices.end()){
        auto idx = static_cast<std::uint32_t>(func_type.size());
        type_indices[key] = idx; 
        auto retval = func_type.size();
        func_type.push_back(t);
        return retval;
    }
    else {
        return it->second;
    }
}

/*
  TypeSection  = sectionId <payload size> list(FunctionType)
  FunctionType = 0x60 <parameter length> list(ValType) <result legth> list(ValType)
  ValType = I32 | I64 | F32 | F64
*/
void TypeSection::write(ByteWriter& bytewriter){
    ByteWriter body;
    body.u32_leb(static_cast<std::uint32_t>(func_type.size()));
    for(const auto& t: func_type){
      // payload type marker - function
      body.u8(0x60); 
      // function parameter length
      body.u32_leb(t.params.size());
      for (auto param : t.params){
          body.u8(static_cast<std::uint8_t>(param));
      }
      body.u32_leb(t.results.size());
      for (auto r: t.results){
          body.u8(static_cast<std::uint8_t>(r));
      }
    }

    wasm::finish_section(bytewriter, wasm::SectionId::Type, body.getBytes());
}

std::uint32_t FunctionSection::add(std::uint32_t type_idx){
    auto idx = static_cast<std::uint32_t>(type_indices.size());
    type_indices.push_back(type_idx);
    return idx;
}

/*
  
   FunctionSection = functionId <payload size> list(Index)
   Index = 0 | 1 | 2 | .....

*/
void FunctionSection::write(ByteWriter& bytewriter) const{

    ByteWriter body;

    body.u32_leb(static_cast<std::uint32_t>(type_indices.size()));
    for(const auto& index: type_indices){
        body.u32_leb(index);
    }

    wasm::finish_section(bytewriter, wasm::SectionId::Function, body.getBytes());
}

void CodeSection::add(FunctionBody &fb){
    code.push_back(fb);
}

void CodeSection::write(ByteWriter& bytewriter){

    ByteWriter body;
    body.u32_leb(static_cast<std::uint32_t>(code.size()));
    for(const auto& ins: code){
      ByteWriter fnbody;
      fnbody.u32_leb(ins.locals.size());
      for (const auto& local : ins.locals){
          fnbody.u32_leb(local.count);
          fnbody.u8(static_cast<std::uint8_t>(local.type));
      }
      
      fnbody.raw(ins.instruction_bytes);
      body.u32_leb(fnbody.getBytes().size());
      body.raw(fnbody.getBytes());
     }

    wasm::finish_section(bytewriter, wasm::SectionId::Code, body.getBytes());
}

void ExportSection::add(const std::string& name, std::uint32_t idx){
    exports.push_back({name, idx});
}

void ExportSection::write(ByteWriter& bytewriter){
    ByteWriter body;
    body.u32_leb(static_cast<std::uint32_t>(exports.size()));
    for(const auto& e : exports){
        body.str(e.name);
        // function marker
        body.u8(0x00);
        // function index
        body.u32_leb(e.idx);
    }

    wasm::finish_section(bytewriter, wasm::SectionId::Export, body.getBytes());
}

std::uint32_t Module::add_function(FuncType& type, FunctionBody& body){
   auto idx = typeSection.intern(type); 
   idx = fnSection.add(idx);
   codeSection.add(body);
   return idx;
}

void Module::add_export(std::string name, std::uint32_t idx){
    exportSection.add(name, idx);
}

std::vector<std::uint8_t> Module::Serialize(){
    ByteWriter b;

    // WASM header + version

    b.raw(std::initializer_list<std::uint8_t>{0x00, 0x61, 0x73, 0x6D, 0x01, 0x00, 0x00, 0x00});     

    typeSection.write(b);
    fnSection.write(b);
    exportSection.write(b);
    codeSection.write(b);

    return b.getBytes();
}


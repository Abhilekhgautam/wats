#ifndef WATS_MODULE
#define WATS_MODULE

#include <unordered_map>
#include <vector>
#include <cstdint>
#include <string>

#include "bytewriter.hpp"
#include "types.hpp"

class CustomSection{};

struct FuncType{
    std::vector<wasm::ValType> params;
    std::vector<wasm::ValType> results;
};

class TypeSection{
private:
    std::string encode_key(const FuncType&);
    // TODO: impl types other than func as well.
    std::vector<FuncType> func_type;
    std::unordered_map<std::string, std::uint32_t> type_indices;
public:
    std::size_t size();
    void write(ByteWriter& bytewriter);
    std::uint32_t intern(FuncType&);
};

class ImportSection{};
class FunctionSection{
private:
    std::vector<std::uint32_t> type_indices;
public:
    std::uint32_t add(std::uint32_t type_idx);
    std::size_t size() const;
    void write(ByteWriter&) const;
};
class TableSection{};
class MemorySection{};
class GlobalSection{};

struct Export {
    std::string name;
    std::uint32_t idx;
};

class ExportSection{
private:
    std::vector<Export> exports;
public:
    void write(ByteWriter&);
    void add(const std::string&, std::uint32_t);
};
class StartSection{};
class ElementSection{};

struct LocalDeclaration {
    std::uint32_t count;
    wasm::ValType type;
};

struct FunctionBody {
   std::vector<LocalDeclaration> locals;
   std::vector<std::uint8_t> instruction_bytes;
};

// Holds bytes(encoded) of instruction
class CodeSection{
private:
    std::vector<FunctionBody> code;
public:
    void add(FunctionBody&);
    void write(ByteWriter&);
    std::size_t size();
};

class DataSection{};
class DataCountSection{};
class TagSection{};

class Module{
private:
    TypeSection     typeSection;
    FunctionSection fnSection;
    CodeSection     codeSection;
    ExportSection   exportSection;
public:
    std::uint32_t add_function(FuncType&, FunctionBody&);
    void add_export(std::string name, std::uint32_t idx);

    std::vector<std::uint8_t> Serialize();
};

#endif

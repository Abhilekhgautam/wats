#ifndef WATS_TYPE
#define WATS_TYPE

#include <cstdint>
namespace wasm {

enum struct ValType : std::uint8_t{
    I32 = 0x7F,
    I64 = 0X7E,
    F32 = 0X7D,
    F64 = 0X7C, 
};

enum class SectionId : std::uint8_t {
    Custom   = 0,
    Type     = 1,
    Import   = 2,
    Function = 3,
    Table    = 4,
    Memory   = 5,
    Global   = 6,
    Export   = 7,
    Start    = 8,
    Element  = 9,
    Code     = 10,
    Data     = 11,
};

}

#endif

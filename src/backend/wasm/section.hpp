#ifndef WATS_SECTION
#define WATS_SECTION

#include <cstdint>
#include "bytewriter.hpp"
#include "types.hpp"

namespace wasm {
inline void finish_section(ByteWriter& out, SectionId section_id, const std::vector<std::uint8_t>& body){
   out.u8(static_cast<std::uint8_t>(section_id)); 
   out.u8(body.size());

   out.raw(body);
}
}

#endif

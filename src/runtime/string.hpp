#ifndef WATS_STRING
#define WATS_STRING

#include <cstdint>

namespace runtime{
   void* alloc_string(const std::size_t len);
   std::int64_t string_len(void* str);

   const std::uint8_t* string_bytes(void* str);
   std::uint8_t* string_byte_mutable(void* str);

   void* string_cat(void* str_one, void* str_two);
   bool string_eq(void* str_one, void* str_two);
}

#endif

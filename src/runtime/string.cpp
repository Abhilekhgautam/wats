#include "allocator.hpp"
#include "string.hpp"
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace runtime {

/*    
-----------------------------
Object Header|length| Payload
-----------------------------    
*/
void* alloc_string(const std::size_t len){
    std::size_t actual_len = sizeof(int64_t) + static_cast<size_t>(len);
    auto* str_ptr = static_cast<std::byte*>(heap().alloc(1, actual_len));


    auto* len_ptr = reinterpret_cast<std::int64_t*>(str_ptr + sizeof(ObjectHeader));

    // Update the len in header.
    *len_ptr = len;
    return str_ptr;
}

std::int64_t string_len(void* str){
   auto* str_ptr = static_cast<std::byte*>(str);
   auto* len_ptr = reinterpret_cast<std::int64_t*>(str_ptr + sizeof(ObjectHeader));

   return *len_ptr;
}

const std::uint8_t* string_bytes(void* str){
   auto* str_ptr = static_cast<std::byte*>(str);

   auto* str_byte = reinterpret_cast<uint8_t*>(str_ptr + sizeof(ObjectHeader) + sizeof(std::int64_t));

   return str_byte;
}

std::uint8_t* string_byte_mutable(void* str){
   return const_cast<std::uint8_t*>(string_bytes(str));
}

void* string_cat(void* str_one, void* str_two){
    std::int64_t str_one_len = string_len(str_one);
    std::int64_t str_two_len = string_len(str_two);

    // TODO: Handle Overflow.
    void* concat_string = alloc_string(str_one_len + str_two_len);

    std::uint8_t* mut_concat_byte = string_byte_mutable(concat_string);

    std::memcpy(mut_concat_byte, string_bytes(str_one), str_one_len);
    std::memcpy(mut_concat_byte + str_one_len, string_bytes(str_two), str_two_len);

    return concat_string;
}

bool string_eq(void* str_one, void* str_two){
    std::int64_t str_one_len = string_len(str_one);
    std::int64_t str_two_len = string_len(str_two);

    if (str_one_len != str_two_len) return false;

    return std::memcmp(string_bytes(str_one), string_bytes(str_two), static_cast<size_t>(str_one_len)) == 0;
}

}


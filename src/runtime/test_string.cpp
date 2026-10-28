// created by void at 2026-10-15 23:23:23

#include <cstring>
#include "string.hpp"
#include "io.h"
int test(){
    auto* str = runtime::alloc_string(7);
    auto* another_str = runtime::alloc_string(15);

    auto* mut_str = runtime::string_byte_mutable(str);
    auto* mut_another_str = runtime::string_byte_mutable(another_str);

    std::memcpy(mut_another_str, "Abhilekh Likes ", 15);
    std::memcpy(mut_str, "Compiler\n", 9);

    auto* final_str =  runtime::string_cat(another_str, str);

    auto mut_final_str = runtime::string_byte_mutable(final_str);

    runtime::fd_write(1, mut_final_str, runtime::string_len(final_str));

    return 0;    
}

// created by void at 2026-10-15 23:19:52
#include "io.h"
#include <cstdint>
#include <unistd.h>

namespace runtime{
    std::int64_t fd_write(std::int32_t fd, const std::uint8_t* buf, std::int64_t len){
       return ::write(fd, buf, static_cast<std::size_t>(len));    
    }
}

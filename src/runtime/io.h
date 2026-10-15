// created by void at 2026-10-15 23:13:03
#ifndef WATS_IO
#define WATS_IO 

#include <cstdint>
namespace runtime {
    std::int64_t fd_write(std::int32_t fd, const std::uint8_t* buf, std::int64_t len);

    std::int64_t fd_read(std::int32_t fd, std::uint8_t* buf, std::int64_t len);
}

#endif

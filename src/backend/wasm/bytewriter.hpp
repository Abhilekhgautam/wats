#ifndef WATS_BYTEWRITER
#define WATS_BYTEWRITER

#include <span>
#include <vector>
#include <cstdint>
#include <string_view>

class ByteWriter{
private:
    std::vector<uint8_t> bytes;
public:
    void u8(std::uint8_t);

    void u32_leb(std::uint32_t);
    void u64_leb(std::uint64_t);

    void i32_sleb(std::int32_t);
    void i64_sleb(std::int64_t);

    void raw(std::span<const uint8_t> list);

    void raw(const std::uint8_t*, std::size_t);

    void str(std::string_view);

    std::size_t size();

    uint8_t* data();

    std::vector<std::uint8_t> getBytes() const;
};

#endif

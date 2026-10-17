#ifndef WAT_LEB128
#define WAT_LEB128

#include <cstdint>
#include <vector>

namespace leb{

void uleb128(std::vector<std::uint8_t>& out, std::uint32_t);
void uleb128_64(std::vector<std::uint8_t>& out, std::uint64_t);

void sleb128(std::vector<std::uint8_t>& out, std::int32_t);
void sleb128_64(std::vector<std::uint8_t>& out, std::int64_t);

}
#endif

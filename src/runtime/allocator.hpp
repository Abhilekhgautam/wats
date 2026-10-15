#ifndef WATS_ALLOCATOR
#define WATS_ALLOCATOR

#include <cstdint>
#include <stdint.h>
#include <cstdlib>
#include <vector>

namespace runtime {
struct ObjectHeader {
        std::uint32_t type_tag;
        std::uint32_t mark;
        // header + payload
        std::uint32_t size;
};

class Heap {
private:
        struct Chunk{
        std::byte* base;
        std::size_t used;
        std::size_t capacity;
    };

    std::vector<Chunk> chunks;
    void new_chunk(std::size_t);
public:    
    void init();
    void* alloc(const std::uint32_t tag, const std::size_t bytes, const std::size_t align=8);
};

Heap& heap();
}

#endif

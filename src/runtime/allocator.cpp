// created by void at 2026-10-15 15:57:42

#include "allocator.hpp"
#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace runtime {

static constexpr std::size_t chunkSize = 1024 * 1024;    

// Add a new chuk to the heap.
void Heap::new_chunk(const std::size_t min_bytes){
    const std::size_t cap = std::max(chunkSize, min_bytes);

    auto* base = static_cast<std::byte*>(std::malloc(cap));

    if(!base){
        // TODO: Better message passing from the rt maybe
        std::abort();
    }

    chunks.push_back({base, 0, cap});
}

void* Heap::alloc(const std::uint32_t tag,  std::size_t bytes, const std::size_t align){
    if (chunks.empty()) new_chunk(bytes);

    Chunk& last_chunk = chunks.back();

    const std::size_t misalign = last_chunk.used & (align - 1);
    const std::size_t padding  = misalign ? (align - misalign) : 0;

    if (last_chunk.used + padding + sizeof(ObjectHeader) +  bytes > last_chunk.capacity){
        // we need a new chunk if the required bytes
        // cannot fit into the last chunk

        new_chunk( bytes);

        return alloc(tag, bytes, align);
    }

    last_chunk.used += padding;

    // fill in the header,
    // later will be used by GC

    auto* header = reinterpret_cast<ObjectHeader*>(last_chunk.base + last_chunk.used);
    header->type_tag = tag;
    header->mark = 0;
    header->size = sizeof(ObjectHeader) + bytes;
    
    void* p = header;

    last_chunk.used += sizeof(ObjectHeader) + bytes;

    return p;
}

Heap& heap(){
    static Heap heap;
    return heap;
}

}

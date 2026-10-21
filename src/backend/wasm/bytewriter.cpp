// created by void at 2026-10-17 11:56:22
#include "bytewriter.hpp"
#include <cstddef>
#include "leb128.hpp"

void ByteWriter::u8(std::uint8_t byte){
    bytes.push_back(byte);    
}

void ByteWriter::u32_leb(std::uint32_t byte){
    leb::uleb128(bytes, byte);
}

void ByteWriter::u64_leb(std::uint64_t byte){
    leb::uleb128_64(bytes, byte);
}

void ByteWriter::i32_sleb(std::int32_t byte){
    leb::sleb128(bytes, byte);
}

void ByteWriter::i64_sleb(std::int64_t byte){
    leb::sleb128_64(bytes, byte);
}

void ByteWriter::raw(std::span<const uint8_t> list){
    bytes.insert(bytes.end(), list.begin(), list.end());
}

void ByteWriter::raw(const std::uint8_t* ptr, std::size_t size){
    bytes.insert(bytes.end(), ptr, ptr + size);
}

void ByteWriter::str(std::string_view str){
   u32_leb(static_cast<std::uint32_t>(str.size()));
   raw(reinterpret_cast<const std::uint8_t*>(str.data()), str.size());
}

std::size_t ByteWriter::size() {return bytes.size();}

std::uint8_t* ByteWriter::data() {return bytes.data();}

std::vector<std::uint8_t> ByteWriter::getBytes() const {return bytes;}

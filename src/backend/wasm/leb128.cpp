// created by void at 2026-10-17 11:04:10
#include "leb128.hpp"

namespace leb{

void uleb128(std::vector<std::uint8_t>& out, std::uint32_t x) {

    std::uint8_t byte;

    do{
        // get the lower 7 bits.
        byte = x & 127;

        // shift by 7.
        x = x >> 7;

        if (x != 0){
            // set the continuation bit. 
            byte = byte | 128;
        }

        out.push_back(byte);
        
    } while(x != 0);
}

void uleb128_64(std::vector<std::uint8_t>& out, std::uint64_t x) {

    std::uint8_t byte;

    do{
        // get the lower 7 bits.
        byte = x & 127;

        // shift by 7.
        x = x >> 7;

        if (x != 0){
            // set the continuation bit. 
            byte = byte | 128;
        }

        out.push_back(byte);
        
    } while(x != 0);
}

void sleb128(std::vector<std::uint8_t>& out, std::int32_t x) {
    std::uint8_t byte;

    bool more = true;

    while (more) {
        // get the lower 7 bits.
        byte = x & 127;

        // shift by 7.
        x = x >> 7;

        // get the sign bit
        bool sign = byte & 64;

        if ((x == 0 && !sign) || (x == -1 && sign)){
            more = false;
        }
        else {
            more = true;
            // set continuation bit
            byte = byte | 128;
        }

        out.push_back(byte);        
    }
}

void sleb128_64(std::vector<std::uint8_t>& out, std::int64_t x) {
    std::uint8_t byte;

    bool more = true;

    while (more) {
        // get the lower 7 bits.
        byte = x & 127;

        // shift by 7.
        x = x >> 7;

        // get the sign bit
        bool sign = byte & 64;

        if ((x == 0 && !sign) || (x == -1 && sign)){
            more = false;
        }
        else {
            more = true;
            // set continuation bit
            byte = byte | 128;
        }

        out.push_back(byte);        
    }
}

}

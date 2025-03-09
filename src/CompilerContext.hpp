#ifndef COMPILER_CONTEXT
#define COMPILER_CONTEXT

#include "utils.hpp"

#include <vector>
#include <string>

class CompilerContext {
    public:
        CompilerContext(std::string& source_code){
            this->source_code = source_code;
            source_code_by_line = split_str(source_code);
        }
        std::string source_code;
        std::vector<std::string> source_code_by_line;
};
#endif

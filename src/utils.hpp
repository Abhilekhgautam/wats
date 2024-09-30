#include <filesystem>
#include <string>
#include <vector>

std::string read_file(std::filesystem::path path);
std::vector<std::string> split_str(const std::string &text,
                                   const std::string delim = "\n");

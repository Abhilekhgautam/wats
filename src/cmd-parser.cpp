#include "cmd-parser.hpp"
#include <filesystem>
#include <iostream>

bool verify_args(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Error: Invalid Command Usage\n";
    std::cerr << "Usage: ./app <file_name>";

    return false;
  }

  std::filesystem::path file_path(argv[1]);

  if (!std::filesystem::exists(file_path)) {
    std::cerr << "Error: " << file_path << " Not Found\n";
    return false;
    if (!std::filesystem::is_regular_file(file_path)) {
      std::cerr << "Error: " << file_path << " is not a file\n";
      return false;
    }
  }
  return true;
}

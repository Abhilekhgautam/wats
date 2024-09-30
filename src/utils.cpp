#include "utils.hpp"
#include <fstream>

// Thank You StackOverflow:
// https://stackoverflow.com/questions/116038/how-do-i-read-an-entire-file-into-a-stdstring-in-c/40903508#40903508
std::string read_file(std::filesystem::path path) {
  // Open the stream to 'lock' the file.
  std::ifstream file(path, std::ios::in | std::ios::binary);

  // Obtain the size of the file.
  const auto file_size = std::filesystem::file_size(path);

  // Create a buffer.
  std::string result(file_size, '\0');

  // Read the whole file into the buffer.
  file.read(result.data(), file_size);

  return result;
}

std::vector<std::string> split_str(const std::string &text,
                                   const std::string delim) {
  std::vector<std::string> return_vec;

  if (text.empty()) {
    return return_vec;
  }

  size_t pos = 0;
  size_t new_pos = text.find(delim);

  while (new_pos != std::string::npos) {
    return_vec.push_back(text.substr(pos, new_pos - pos));
    pos = new_pos + 1;
    new_pos = text.find(delim, pos);
  }

  // Add the remaining part of the string after the last newline, if any
  if (pos < text.size()) {
    return_vec.push_back(text.substr(pos));
  }

  return return_vec;
}

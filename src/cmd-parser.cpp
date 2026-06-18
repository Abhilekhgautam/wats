#include "cmd-parser.hpp"
#include <filesystem>
#include <iostream>
#include <vector>

std::filesystem::path CommandParser::input_file_path;
std::vector<std::string> CommandParser::requested_pass;

void CommandParser::displayHelp() {
  std::cerr << "Error: Invalid Command Usage:\n";
  std::cerr << "wats -p <pass name> <file name>\n";
}

bool CommandParser::verify_args(const int argc, char** argv) {
  if (argc < 2) {
    displayHelp();
    return false;
  }

  int arg_count = 1;
  while (arg_count < argc) {
      if (std::string(argv[arg_count]) == "-p") {
        if (arg_count + 1 < argc) {
          requested_pass.emplace_back(argv[arg_count + 1]);
          arg_count = arg_count + 2; // skip -p and pass name
        }
        else {
          displayHelp();
          return false;
        }
      }
      else {
        input_file_path = argv[arg_count];
        arg_count = arg_count + 1;
      }
  }

  if (input_file_path.empty()) {
    displayHelp();
    return false;
  }

  if (!std::filesystem::exists(input_file_path)) {
    std::cerr << "Error: " << input_file_path << " Not Found\n";
    return false;
  }
  return true;
}

#ifndef CMD_PARSER_HPP
#define CMD_PARSER_HPP

#include <filesystem>
#include <vector>

class CommandParser {
public:
    static bool verify_args(int argc, char** argv);
    static void displayHelp();

    static std::filesystem::path GetInputFilePath() {return input_file_path;}
    static std::vector<std::string> GetPassVec() {return std::move(requested_pass);}
private:

    static std::filesystem::path input_file_path;
    static std::vector<std::string> requested_pass;
};

#endif


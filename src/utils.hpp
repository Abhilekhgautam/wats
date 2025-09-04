
#ifndef UTILS_HPP
#define UTILS_HPP

#include <filesystem>
#include <iostream>
#include <string>
#include <vector>
#include <cstdarg>

std::string read_file(std::filesystem::path path);
std::vector<std::string> split_str(const std::string &text,
                                   const std::string delim = "\n");
#ifndef HIGHLIGHT_TERM_
#define HIGHLIGHT_TERM_
#ifdef _WIN32

#include <windows.h>
inline void Color(std::string color, std::string line, bool newLine = false) {
  HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
  int col = 7;

  if (color == "blue")
    col = 1;
  else if (color == "green")
    col = 2;
  else if (color == "red")
    col = 4;

  SetConsoleTextAttribute(hConsole, col);
  std::cout << line;
  if (newLine) {
    std::cout << '\n';
  }

  SetConsoleTextAttribute(hConsole, 7);
}

#else

inline void Color(std::string color, std::string line, bool newLine = false) {
  std::string col = "\033[0m";

  if (color == "blue")
    col = "\033[0;34m";
  else if (color == "green")
    col = "\033[0;32m";
  else if (color == "red")
    col = "\033[0;31m";

  std::cout << col << line << "\033[0m";
  if (newLine) {
    std::cout << '\n';
  }
}


#endif
inline std::string SetArrowLeft(int pos, int times = 1) {
  std::string arrows;
  for (int i = pos - times; i > 0; --i)
    arrows += ' ';
  for (int i = 0; i < times; ++i)
    arrows += '^';
  return arrows;
}

inline std::string SetArrowLeft(){
    return "";
}

template <typename... TArgs>
inline std::string MultiPartArrow(int start, int len, TArgs... rest) {
    std::string current = SetArrowLeft(start, len);
    std::string tail = SetArrowLeft(rest...).erase(start, len);
    return current + tail;
}

inline std::string SetArrow(std::size_t pos, std::size_t times = 1) {
  std::string arrows;
  for (size_t i = 0; i < pos - 1; ++i)
    arrows += ' ';
  for (size_t i = 0; i < times; ++i)
    arrows += '^';
  return arrows;
}

inline std::string SetPlus(std::size_t pos, std::size_t times = 1) {
  std::string plus;
  for (size_t i = 0; i < pos - 1; ++i)
    plus += ' ';
  for (size_t i = 0; i < times; ++i)
    plus += '+';

  return plus;
}

#endif

#endif

#ifndef FN_SYM_TABLE
#define FN_SYM_TABLE

#include <optional>
#include <string>
#include <vector>

#include <unordered_map>

struct FunctionInfo {
  std::vector<std::string> fn_args_type;
  std::string fn_ret_type;

  bool operator==(FunctionInfo fn_info) {
    if (fn_args_type == fn_info.fn_args_type &&
        fn_ret_type == fn_info.fn_ret_type) {
      return true;
    }
    return false;
  }

  // Inside struct FunctionInfo { ...
public:
  FunctionInfo() : fn_args_type({}), fn_ret_type("") {}
  // Missing getters:
  const std::vector<std::string> &GetArgs() const {
    return fn_args_type;
  } // For error 520
  const std::string &GetRetType() const { return fn_ret_type; } // For error 559

  // Constructor fix (required based on your semantic analyzer logic):
  FunctionInfo(std::vector<std::string> args)
      : fn_args_type(std::move(args)), fn_ret_type("") {}
  FunctionInfo(std::vector<std::string> args, std::string ret_type)
      : fn_args_type(std::move(args)), fn_ret_type(std::move(ret_type)) {}
};

class FunctionSymbolTable {

  std::unordered_map<std::string, FunctionInfo> table;

public:
  void AddToSymbolTable(std::string fn_name, FunctionInfo fn_info) {
    table.insert({fn_name, fn_info});
  }
  std::optional<FunctionInfo> FindInSymbolTable(std::string fn_name) {
    if (table.empty())
      return {};
    auto val = table.find(fn_name);
    if (val == table.end())
      return {};
    else
      return val->second;
  }
};

#endif

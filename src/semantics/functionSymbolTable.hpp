#ifndef FN_SYM_TABLE
#define FN_SYM_TABLE

#include <optional>
#include <string>
#include <vector>

#include <unordered_map>

struct FunctionInfo {
    std::vector<std::string> fn_params_type;
    std::string fn_ret_type;

    bool operator==(const FunctionInfo &fn_info) const {
        if (fn_params_type == fn_info.fn_params_type && fn_ret_type == fn_info.fn_ret_type) {
            return true;
        }
        return false;
    }
};

class FunctionSymbolTable {

    std::unordered_map<std::string, FunctionInfo> table;

public:
    void AddToSymbolTable(const std::string &fn_name, const FunctionInfo &fn_info) { table.insert({fn_name, fn_info}); }
    std::optional<FunctionInfo> FindInSymbolTable(const std::string &fn_name) {
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

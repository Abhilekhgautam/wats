//
// Created by void on 6/20/26.
//

#ifndef WATS_LVN_H
#define WATS_LVN_H

#include <string>
#include <ostream>
#include <variant>
#include <nlohmann/json.hpp>


struct LVNTableValue {
    // Stores value for const and index for id.
    std::optional<int> value;
    // lhs operand name or index
    std::optional<std::variant<int, std::string>> lhs;
    // rhs operand name or index
    std::optional<std::variant<int, std::string>> rhs;
    // Stores the operations - add , id , const
    std::string op;

    bool operator == (const LVNTableValue& other) const{
        return (value == other.value && op == other.op && lhs == other.lhs && rhs == other.rhs);
    }

    // friend std::ostream& operator << (std::ostream& os, LVNTableValue other) {
    //     if (other.value.has_value()) {
    //         os << other.op << " " << other.value.value();
    //     }
    //     else if (other.lhs.has_value() && other.rhs.has_value()) {
    //         os << other.op << " " << other.lhs.value() << " " << other.rhs.value();
    //     }
    //
    //     else if (other.lhs.has_value()) {
    //         os << other.op << " " << other.lhs.value();
    //
    //     }
    //
    //     return os;
    // }

    // Does the value field contains an index
    [[nodiscard]]bool hasIndex() const {
        return (op == "id" && value.has_value());
    }

    // Get the index.
    // Only call if hasIndex() returns true.
    [[nodiscard]]int getCorrespondingIndex() const{
        return value.value();
    }
};

struct LVNTable {
    std::unordered_map<int, LVNTableValue> index2Value;
    std::unordered_map<int, std::string> index2Name;

    void addValue(const int index, const std::string& name, const LVNTableValue& value) {
        index2Name[index] = name;
        index2Value[index] = value;
    }
    bool containsValue(const LVNTableValue& value) {
        return std::ranges::any_of(index2Value | std::views::values, [value](const auto& v) {
            return v == value;
        });

    }

    bool containsName(const std::string& name) {
        return std::ranges::any_of(index2Name | std::views::values, [name](const auto& n) {
            return n == name;
        });

    }

    bool containsValue(const LVNTableValue& value) const {
        return std::ranges::any_of(index2Value | std::views::values, [value](const auto& v) {
            return v == value;
        });
    }

    bool containsName(const std::string& name) const {
        return std::ranges::any_of(index2Name | std::views::values, [name](const auto& n) {
            return n == name;
        });

    }

    std::optional<int> getIndex(const LVNTableValue& value) {
        for (const auto& [id, v] : index2Value) {
            if (v == value) return id;
        }
        // Caller must ensure the value was previously added
        // Unreachable, just to supress warning
        return {};
    }
};

struct LVNResult {
    std::unordered_map<std::string, int> lvn_environment;
    LVNTable table;
};

class LVN {
public:
    static LVNResult run(const std::vector<nlohmann::json>&);
};


#endif //WATS_LVN_H
//
// Created by void on 6/20/26.
//

#ifndef WATS_LVN_H
#define WATS_LVN_H

#include <string>
#include <ostream>
#include <nlohmann/json.hpp>


struct LVNTableValue {
    std::optional<int> value;
    std::optional<int> lhs;
    std::optional<int> rhs;
    std::string op;

    bool operator == (const LVNTableValue& other) const{
        return (value == other.value && op == other.op && lhs == other.lhs && rhs == other.rhs);
    }

    friend std::ostream& operator << (std::ostream& os, LVNTableValue other) {
        if (other.value.has_value()) {
            os << other.op << " " << other.value.value();
        }
        else if (other.lhs.has_value() && other.rhs.has_value()) {
            os << other.op << " " << other.lhs.value() << " " << other.rhs.value();
        }

        else if (other.lhs.has_value()) {
            os << other.op << " " << other.lhs.value();

        }

        return os;
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
        for (const auto& [id, v] : index2Value) {
            if (v == value) return true;
        }
        return false;
    }

    bool containsName(const std::string& name) {
        for (const auto& [id, v] : index2Name) {
            if (v == name) return true;
        }
        return false;
    }

    bool containsValue(const LVNTableValue& value) const {
        for (const auto& [id, v] : index2Value) {
            if (v == value) return true;
        }
        return false;
    }

    bool containsName(const std::string& name) const {
        for (const auto& [id, v] : index2Name) {
            if (v == name) return true;
        }
        return false;
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
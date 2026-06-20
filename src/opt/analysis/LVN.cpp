//
// Created by void on 6/20/26.
//

#include "LVN.h"
#include <iostream>

LVNResult LVN::run(const std::vector<nlohmann::json>& instrs) {
    std::unordered_map<std::string, int> lvn_environment;
    LVNTable table;

    int index = 1;
    for (auto& instr : instrs) {
       // Handle constant assignments.
       if (instr.contains("value")) {
          LVNTableValue value{instr["value"].get<int>(), {}, {}, "const"};

           if (instr.contains("dest")) {
               if (!table.containsValue(value)) {
                   table.addValue(index, instr["dest"], value);
                   lvn_environment[instr["dest"]] = index;
                   index = index + 1;
               }
               else {
                   // Safety: Guaranteed to be there, confirmed by the above if.
                   lvn_environment[instr["dest"]] = table.getIndex(value).value();
               }
           }
       }
       else if (instr.contains("args")) {
           auto args = instr["args"].get<std::vector<std::string>>();
           // Check if it is an arithmetic op
           if (args.size() == 2) {
               int lhs = lvn_environment[args[0]];
               int rhs = lvn_environment[args[1]];
               if (instr["op"] == "mul" || instr["op"] == "add") {
                  if (lhs > rhs) {
                      std::swap(lhs, rhs);
                  }
               }
               LVNTableValue value{{}, lhs, rhs, instr["op"]};

               if (instr.contains("dest")) {
                   if (!table.containsValue(value)) {
                       table.addValue(index, instr["dest"], value);
                       lvn_environment[instr["dest"]] = index;
                       index = index + 1;
                   }
                   else {
                       // Safety: Guaranteed to be there, confirmed by the above if.
                       lvn_environment[instr["dest"]] = table.getIndex(value).value();
                   }
               }
           }
           else if (args.size() == 1 && instr["op"] == "id") {
               LVNTableValue value{{}, lvn_environment[instr["args"][0]], {}, "id"};

               if (instr.contains("dest")) {
                   if (!table.containsValue(value)) {
                       table.addValue(index, instr["dest"], value);
                       lvn_environment[instr["dest"]] = index;
                       index = index + 1;
                   } else {
                       // Safety: Guaranteed to be there, confirmed by the above if.
                       lvn_environment[instr["dest"]] = table.getIndex(value).value();
                   }

               }
           }
       }

    }

    return {lvn_environment, table};
}

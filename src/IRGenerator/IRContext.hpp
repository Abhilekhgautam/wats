#ifndef IR_CONTEXT
#define IR_CONTEXT

#include <string>

// This class holds the necessary information required to generate the IR
// For eg: where to break to , if a break statement pops inside an if.
class IRContext {
public:
  IRContext() : tempVarCount(0) {}

private:
  std::string break_point_name {}; // this is where the break statement jump to.
  std::string skip_point_name {}; // this is where the skip statement jump to.
  std::string if_exit_label_name {}; // this is where the if, else if body jump to at the last.
  int tempVarCount;

public:
  void IncrementTempCount() { tempVarCount = tempVarCount + 1;}

  [[nodiscard]] int GetCurrentCount() { return tempVarCount; }
  void SetBreakPointName(const std::string& bp ) {break_point_name = bp;}
  void SetSkipPointName(const std::string& sp) {skip_point_name = sp;}
  void SetIfExitLabel(const std::string& el) {if_exit_label_name = el;}

  std::string GetBreakPointName() {return break_point_name;}
  std::string GetSkipPointName() {return skip_point_name;}
  std::string GetIfExitLabelName() {return if_exit_label_name;}
};

#endif

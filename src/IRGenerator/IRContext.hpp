#ifndef IR_CONTEXT
#define IR_CONTEXT

#include <string>

template <typename T> class IRContext {
public:
  IRContext(std::string name) : fn_name(name), tempVarCount(0) {}

private:
  std::string fn_name;
  int tempVarCount;

public:
  std::string GetFnName() { return fn_name; }
  void IncrementTempCount() { tempVarCount = tempVarCount + 1; }
  int GetCurrentCount() { return tempVarCount; }
};

#endif

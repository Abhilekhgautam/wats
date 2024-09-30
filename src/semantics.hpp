#include <string>
#include <vector>

enum class ObjType {
  FUNC, GLOBAL, MEM	
};

struct LangObj{
 LangObj(bool is_exported, std::string name, ObjType type): is_exported(is_exported), name(name), type(type){}
  bool is_exported;
  std::string name;
  ObjType type;
};

struct Func : public LangObj {
 Func(bool exports, std::string name, ObjType type, bool has_return, std::vector<std::string> args, std::string func_return_type): LangObj(exports, name, type), has_return(has_return), args(args), func_return_type(func_return_type){}
 bool has_return;
 std::vector<std::string> args;
 std::string func_return_type;
};

void write(LangObj* obj);

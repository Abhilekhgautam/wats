#ifndef AST_TYPE
#define AST_TYPE

#include <string>

class Type{
  public:
    virtual std::string GetType() = 0;
    virtual ~Type() = default;
};

class PrimitiveType : public Type{
  public:
    PrimitiveType(const std::string type_name): type_name(type_name){}
    std::string GetType() const { return type_name; }

    virtual ~PrimitiveType() = default;

  private:
    std::string type_name;
};

class UserDefinedType : public Type{
  public:
    UserDefinedType(const std::string type_name) : type_name(type_name){}
    std::string GetType() const{ return type_name;}

    virtual ~UserDefinedType() = default;

  private:
    std::string type_name;
};

#endif

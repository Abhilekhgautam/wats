#ifndef AST_TYPE
#define AST_TYPE

#include <memory>
#include <string>

class Type {
public:
  virtual std::string GetType() = 0;
  virtual ~Type() = default;
};

class PrimitiveType : public Type {
public:
  PrimitiveType(const std::string type_name) : type_name(type_name) {}
  std::string GetType() override { return type_name; }

  virtual ~PrimitiveType() = default;

private:
  std::string type_name;
};

class UserDefinedType : public Type {
public:
  UserDefinedType(const std::string type_name) : type_name(type_name) {}
  std::string GetType() override { return type_name; }

  virtual ~UserDefinedType() = default;

private:
  std::string type_name;
};

class TypeFactory {
public:
  static std::unique_ptr<Type> CreateType(std::string name) {
    if (name == "i32") {
      return std::make_unique<PrimitiveType>(name);
    } else if (name == "i64") {
      return std::make_unique<PrimitiveType>(name);
    } else if (name == "f32") {
      return std::make_unique<PrimitiveType>(name);
    } else if (name == "f64") {
      return std::make_unique<PrimitiveType>(name);
    } else {
      return std::make_unique<UserDefinedType>(name);
    }
  }
};

#endif

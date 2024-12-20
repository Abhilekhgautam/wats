class Type{
  public:
    virtual std::string GetType() = 0;
};

class PrimitiveType : public Type{
  public:
    PrimitiveType(const std::string type_name): type_name(type_name){}
    std::string GetType() { return type_name; }

  private:
    std::string type_name;
};

class UserDefinedType : public Type{
  public:
    UserDefinedType(const std::string type_name) : type_name(type_name){}
    std::string GetType() { return type_name;}
  
  private:
    std::string type_name;
};


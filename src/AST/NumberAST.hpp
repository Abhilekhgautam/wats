#ifndef NUM_AST
#define NUM_AST

#include "ExpressionAST.hpp"

#include <variant>

class NumberAST : public ExpressionAST {
    public:
       NumberAST(std::string num, bool has_decimal): num(num), has_decimal(has_decimal){}

       virtual ~NumberAST() = default;
    private:
      std::string num;
      bool has_decimal;

      std::variant<long long, double> value;
      std::string type;
      public:
         void Debug() override;
         void Accept(SemanticAnalyzer& analyzer) override;
         std::string GetNumber() const {return num;}
         bool HasDecimal() const {return has_decimal;}
         void SetValue(long long val) {value = val;}
         void SetValue(double val) {value = val;}
         std::string GetType() const {return type;}
         void SetType(std::string type) {this->type = type;}
         std::string GetType() override {
            return type;
         }
};

#endif






// class FunctionArgumentAST : public StatementAST{
//     public:
//      FunctionArgumentAST(std::vector<std::string>& args)
//      : args(args){}

//      virtual ~FunctionArgumentAST() = default;
//      void Debug() override {
//          std::cout << "Function Argument:\n";
//          for(auto elt: args){
//              std::cout << elt << ' ';
//          }
//      }
//     private:
//      std::vector<std::string> args;
// };

// class TypeAnnotatedFunctionArgumentAST : public StatementAST{
//     public:
//      TypeAnnotatedFunctionArgumentAST(std::vector<std::pair<Type*, std::string>> args)
//      : args(args){}
//     private:
//      std::vector<std::pair<Type*, std::string>> args;
// };

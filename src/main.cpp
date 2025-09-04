#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "./lexer/lexer.hpp"
#include "./parser/parser.hpp"
#include "./semantics/semanticAnalyzer.hpp"
#include "CompilerContext.hpp"
#include "cmd-parser.hpp"

#include "IRGenerator/IRGenerator.hpp"
#include "utils.hpp"
#include <nlohmann/json.hpp>

using nlohmann::json;
#ifdef __EMSCRIPTEN__
extern "C" {
EMSCRIPTEN_KEEPALIVE
void compile_program(const char *source_code) {
  std::string source(source_code);

  CompilerContext context(source);

  Lexer L(context);
  L.Tokenize();

  Parser P(context, L.GetTokens());
  auto ast_vec = P.Parse();

  SemanticAnalyzer s(context, ast_vec.value());
  s.analyze();

  nlohmann::json program = {{"functions", nlohmann::json::array({})}};

  // Only for debug
  if (ast_vec.has_value()) {
    auto stmt_vec = std::move(ast_vec.value());
    for (const auto &elt : stmt_vec) {
      if (auto val = dynamic_cast<FunctionDefinitionAST *>(elt.get())) {
        std::vector<nlohmann::json> instrVec;
        std::string fnName;

        fnName = val->GetFunctionName();
        // IRContext ctx(fnName);
        IRGenerator brilGenerator;
        for (const auto &elt : val->GetFunctionBody()) {
          instrVec.push_back(brilGenerator.Generate(*elt));
        }

        nlohmann::json fn_obj = {{"name", fnName}, {"instrs", instrVec}};

        program["functions"].push_back(fn_obj);
      }
    }
  }
  std::cout << program.dump(4) << '\n';
}
}
#endif

int main(int argc, char **argv) {

  if (!verify_args(argc, argv)) {
    return 0;
  }

  std::string source_code = read_file(std::filesystem::path{argv[1]});

  CompilerContext context(source_code);

  Lexer L(context);
  L.Tokenize();

  Parser P(context, L.GetTokens());
  // function name, var name ....
  auto ast_vec = P.Parse();

  if (P.HasErrors()) {
    return 1;
  }

  SemanticAnalyzer s(context, ast_vec.value());
  s.analyze();

  if (s.HasErrors()) {
    return 0;
  }

  nlohmann::json program = {{"functions", nlohmann::json::array({})}};

  // Only for debug
  if (ast_vec.has_value()) {
    auto stmt_vec = std::move(ast_vec.value());
    for (const auto &elt : stmt_vec) {
      if (auto val = dynamic_cast<FunctionDefinitionAST *>(elt.get())) {
        std::vector<nlohmann::json> instrVec;
        std::string fnName;

        fnName = val->GetFunctionName();

        std::map<std::string, std::string> name2type;
        auto args = val->GetFunctionArguments();

        std::vector<std::string> argVec;
        if (args.has_value()) {
          for (const auto &elt : args->get().GetArgs()) {
            name2type[elt] = "i64";
          }
          argVec = args->get().GetArgs();
        }

        // IRContext ctx(fnName);
        IRGenerator brilGenerator;
        for (const auto &elt : val->GetFunctionBody()) {
          json generated_json = brilGenerator.Generate(*elt);
          if (!generated_json.is_array())
            instrVec.push_back(brilGenerator.Generate(*elt));
          else {
            for (const auto &elt : generated_json) {
              instrVec.push_back(elt);
            }
          }
        }

        json fnArgs = json::array({});
        for (const auto &[name, type] : name2type) {
          fnArgs.push_back({{"name", name}, {"type", type}});
        }

        nlohmann::json fn_obj = {
            {"name", fnName}, {"instrs", instrVec}, {"args", fnArgs}};

        program["functions"].push_back(fn_obj);
      }
    }
  }
  std::cout << program.dump(4);
}

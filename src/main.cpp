#include "opt/analysis/LVN.h"
#include "opt/passes/CSE.h"
#include "opt/passes/ConstantFolding.h"
#include "opt/passes/CopyPropagation.h"
#include "opt/passes/SimplifyCFG.h"
#include "opt/passes/dce.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include "./lexer/lexer.hpp"
#include "./parser/parser.hpp"
#include "./semantics/semanticAnalyzer.hpp"
#include "CompilerContext.hpp"
#include "cmd-parser.hpp"

#include <nlohmann/json.hpp>
#include "IRGenerator/IRGenerator.hpp"
#include "utils.hpp"

#include "opt/PassManager.h"

using nlohmann::json;

using PassFactory = std::function<std::unique_ptr<Pass>()>;

#ifdef __EMSCRIPTEN__
std::optional<std::vector<std::string>> parse_passes(std::string_view args) {
    auto tokens = args | std::views::split(' ') | std::views::transform([](auto &&r) {
                      return std::string_view(&*r.begin(), std::ranges::distance(r));
                  });

    std::vector<std::string> passes;

    auto it = tokens.begin();
    while (it != tokens.end()) {
        if (*it != "-p") {
            std::cerr << "Unknown argument " << *it << '\n';
            return {};
        }

        ++it;
        if (it == tokens.end()) {
            std::cerr << "-p requires a pass name " << '\n';
            return {};
        }

        passes.emplace_back(*it);
        ++it;
    }

    return passes;
}
extern "C" {
EMSCRIPTEN_KEEPALIVE
void compile_program(const char *source_code, const char *flags) {
    std::string source(source_code);
    std::string options(flags);

    auto opt_req_passes = parse_passes(options);

    if (!opt_req_passes.has_value()) {
        return;
    }
    CompilerContext context(source);

    Lexer L(context);
    L.Tokenize();

    Parser P(context, L.GetTokens());
    // function name, var name ....
    auto ast_vec = P.Parse();

    if (P.HasErrors()) {
        return;
    }

    SemanticAnalyzer s(context, ast_vec.value());
    s.analyze();

    if (s.HasErrors()) {
        return;
    }

    nlohmann::json program = {{"functions", nlohmann::json::array({})}};

    // Only for debug
    if (ast_vec.has_value()) {
        auto stmt_vec = std::move(ast_vec.value());
        for (const auto &elt: stmt_vec) {
            if (auto val = dynamic_cast<FunctionDefinitionAST *>(elt.get())) {
                std::vector<nlohmann::json> instrVec;

                const std::string fnName = val->GetFunctionName();
                const std::string fnRetType = val->GetFunctionReturnType();

                std::map<std::string, std::string> name2type;
                auto &args = val->GetFunctionArguments();

                for (const auto &arg: args) {
                    name2type[arg->GetIdName()] = arg->GetTypeName();
                }

                // IRContext ctx(fnName);
                IRGenerator brilGenerator;
                for (const auto &elt: val->GetFunctionBody()) {
                    json generated_json = brilGenerator.Generate(*elt);
                    if (!generated_json.is_array())
                        instrVec.push_back(brilGenerator.Generate(*elt));
                    else {
                        for (const auto &elt: generated_json) {
                            instrVec.push_back(elt);
                        }
                    }
                }

                json fnArgs = json::array({});
                for (const auto &[name, type]: name2type) {
                    fnArgs.push_back({{"name", name}, {"type", type}});
                }

                nlohmann::json fn_obj = {{"name", fnName}, {"instrs", instrVec}, {"args", fnArgs}};

                if (!fnRetType.empty()) {
                    fn_obj["type"] = fnRetType;
                }

                program["functions"].push_back(fn_obj);
            }
        }
    }

    PassManager pm;

    std::unordered_map<std::string, PassFactory> registry;

    registry["dce"] = [] { return std::make_unique<DCE>(); };

    registry["cse"] = [] { return std::make_unique<CSE>(); };

    registry["copy-propagation"] = [] { return std::make_unique<CopyPropagation>(); };

    registry["const-folding"] = [] { return std::make_unique<ConstantFolding>(); };

    for (const auto &pass_name: opt_req_passes.value()) {
        if (!registry.contains(pass_name)) {
            std::cerr << "Unknown Pass option : " << pass_name << '\n';
            return;
        }
        pm.addPass(registry.at(pass_name)());
    }

    for (auto &fn: program["functions"]) {
        auto instrs = fn["instrs"].get<std::vector<nlohmann::json>>();
        pm.run(instrs);
        fn["instrs"] = std::move(instrs);
    }

    std::cout << program.dump(4) << std::endl;
}
}
#endif

int main(int argc, char **argv) {

    if (!CommandParser::verify_args(argc, argv)) {
        return 0;
    }

    const std::string source_code = read_file(CommandParser::GetInputFilePath());

    const std::vector<std::string> requested_pass = CommandParser::GetPassVec();

    CompilerContext context(source_code);

    Lexer L(context);
    L.Tokenize();

    Parser P(context, L.GetTokens());
    // function name, var name ....
    auto ast_vec = P.Parse();

    if (P.HasErrors()) {
        return 0;
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
        for (const auto &elt: stmt_vec) {
            if (auto val = dynamic_cast<FunctionDefinitionAST *>(elt.get())) {
                std::vector<nlohmann::json> instrVec;

                const std::string fnName = val->GetFunctionName();
                const std::string fnRetType = val->GetFunctionReturnType();

                std::map<std::string, std::string> name2type;
                auto &args = val->GetFunctionArguments();

                for (const auto &arg: args) {
                    name2type[arg->GetIdName()] = arg->GetTypeName();
                }

                // IRContext ctx(fnName);
                IRGenerator brilGenerator;
                for (const auto &elt: val->GetFunctionBody()) {
                    json generated_json = brilGenerator.Generate(*elt);
                    if (!generated_json.is_array())
                        instrVec.push_back(brilGenerator.Generate(*elt));
                    else {
                        for (const auto &elt: generated_json) {
                            instrVec.push_back(elt);
                        }
                    }
                }

                json fnArgs = json::array({});
                for (const auto &[name, type]: name2type) {
                    fnArgs.push_back({{"name", name}, {"type", type}});
                }

                json fn_obj = {{"name", fnName}, {"instrs", instrVec}, {"args", fnArgs}};

                if (!fnRetType.empty()) {
                    fn_obj["type"] = fnRetType;
                }

                program["functions"].push_back(fn_obj);
            }
        }
    }

    PassManager pm;

    std::unordered_map<std::string, PassFactory> registry;

    registry["dce"] = [] { return std::make_unique<DCE>(); };

    registry["cse"] = [] { return std::make_unique<CSE>(); };

    registry["copy-propagation"] = [] { return std::make_unique<CopyPropagation>(); };

    registry["const-folding"] = [] { return std::make_unique<ConstantFolding>(); };

    registry["simplify-cfg"] = [] { return std::make_unique<SimplifyCFG>(); };

    for (const auto &pass_name: requested_pass) {
        pm.addPass(registry.at(pass_name)());
    }

    for (auto &fn: program["functions"]) {
        auto instrs = fn["instrs"].get<std::vector<nlohmann::json>>();
        pm.run(instrs);
        fn["instrs"] = std::move(instrs);
    }

    std::cout << program.dump(4);
}

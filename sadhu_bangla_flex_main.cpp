#include <bits/stdc++.h>
using namespace std;

#include "part01_token.hpp"
#include "part03_ast_types.cpp"
#include "part04_parser.cpp"
#include "part05_type_checker.cpp"
#include "part06_python_code_generator.cpp"
#include "part07_utils.cpp"
#include "part09_debug_printer.cpp"
#include "part10_ir.cpp"

vector<Token> lexWithFlex(const string &source, vector<string> &errors);

// ------------------------------ Main -----------------------------
int main(int argc, char **argv) {
    ios::sync_with_stdio(false);
    cin.tie(nullptr);

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <source.sbb> [output.py] [--no-run] [--tokens] [--ast] [--ir] [--all]\n";
        cerr << "Example: " << argv[0] << " examples/demo.sbb generated.py\n";
        cerr << "  --tokens   print every token (line & column), then stop\n";
        cerr << "  --ast      print the parsed AST, then stop\n";
        cerr << "  --ir       print the generated three-address code (IR), then stop\n";
        cerr << "  --all      print tokens + AST + type-check status + IR, then generate & run\n";
        return 1;
    }

    const string sourcePath = argv[1];
    string outputPath = "generated.py";
    bool outputPathSet = false;
    bool runProgram = true;
    bool showTokens = false;
    bool showAst = false;
    bool showIr = false;
    bool showAll = false;

    for (int i = 2; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--no-run") {
            runProgram = false;
        } else if (arg == "--tokens") {
            showTokens = true;
        } else if (arg == "--ast") {
            showAst = true;
        } else if (arg == "--ir") {
            showIr = true;
        } else if (arg == "--all") {
            showAll = true;
        } else if (!arg.empty() && arg[0] == '-') {
            cerr << "Unknown flag: " << arg << '\n';
            return 1;
        } else if (!outputPathSet) {
            outputPath = arg;
            outputPathSet = true;
        } else {
            cerr << "Unknown or duplicate argument: " << arg << '\n';
            return 1;
        }
    }

    if (showAll) { showTokens = true; showAst = true; showIr = true; }
    const bool stopAfterTokens = showTokens && !showAst && !showIr && !showAll;
    const bool stopAfterAst = showAst && !showIr && !showAll;
    const bool stopAfterIr = showIr && !showAll;

    string source;
    try {
        source = readFile(sourcePath);
    } catch (const exception &e) {
        cerr << e.what() << '\n';
        return 1;
    }

    vector<string> errors;

    // 1. Lexical Analysis (Flex)
    vector<Token> tokens = lexWithFlex(source, errors);
    if (!errors.empty()) {
        cerr << "\n=== ERRORS ===\n";
        for (const auto &e : errors) cerr << e << '\n';
        return 1;
    }

    if (showTokens) printTokens(tokens);
    if (stopAfterTokens) return 0;

    // 2. Syntax Analysis (Recursive Descent)
    Parser parser(tokens, errors);
    Program program = parser.parse();
    if (!errors.empty()) {
        cerr << "\n=== ERRORS ===\n";
        for (const auto &e : errors) cerr << e << '\n';
        return 1;
    }

    if (showAst) printAST(program);
    if (stopAfterAst) return 0;

    // 3. Semantic Analysis / Type Checking
    TypeChecker checker(errors);
    checker.check(program);
    if (!errors.empty()) {
        cerr << "\n=== ERRORS ===\n";
        for (const auto &e : errors) cerr << e << '\n';
        return 1;
    }
    if (showAll) cout << "=== TYPE CHECK ===\nOK — no type errors.\n";

    // 4. Intermediate Representation / Three-Address Code
    IRGenerator irGen;
    IRModule ir = irGen.generate(program);
    if (showIr) printIR(ir);
    if (stopAfterIr) return 0;

    // 5. Target Code Generation
    PythonGenerator generator;
    const string pythonCode = generator.generate(program);

    if (!writeFile(outputPath, pythonCode)) {
        cerr << "Cannot write generated target file: " << outputPath << '\n';
        return 1;
    }

    if (showAll) cout << "=== GENERATED PYTHON (" << outputPath << ") ===\n" << pythonCode;

    cout << "Compilation successful.\n";
    cout << "Generated Python file: " << outputPath << '\n';

    if (!runProgram) return 0;

    cout << "\n=== PROGRAM OUTPUT ===\n";
    cout.flush();
    const int status = runPythonProgram(outputPath);
    if (status != 0) {
        cerr << "\nProgram execution failed (python3 exited with a non-zero status).\n";
        return 1;
    }

    return 0;
}

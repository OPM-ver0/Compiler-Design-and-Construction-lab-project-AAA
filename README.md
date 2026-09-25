# Sadhu Bangla Compiler

Sadhu Bangla is a small educational compiler written in C++17. It translates Sadhu Bangla source files (`.sbb`) into Python 3 source code.

## Compiler architecture

```text
Sadhu Bangla source
        |
        v
   Lex / Flex Lexer
        |
        v
      Tokens
        |
        v
Recursive Descent Parser
        |
        v
       AST
        |
        v
   Type Checker
        |
        v
 Python Code Generator
        |
        v
   generated .py
```

The rubric-facing implementation is:

- **Lexer:** Lex/Flex — `sadhu_bangla_lexer.l`
- **Parser:** Recursive Descent — `part04_parser.cpp`
- **Implementation:** C++17
- **Target:** Python 3

## Language features

| Feature | Sadhu Bangla syntax |
|---|---|
| Integer | `purno` |
| Floating point | `doshomik` |
| String | `okkhormala` |
| If | `jodi` |
| Else / Else-if | `onnothay`, `onnothay jodi` |
| While | `jotokkhon` |
| Print | `mudron(...)` |
| Input | `grohon()` |
| Continue | `ogrosor` |
| Break | `sthogito` |
| Assignment | `=` |
| Arithmetic | `+ - * / %` |
| Comparison | `< <= > >= == !=` |
| Logical | `&& ||` |
| Comments | `// ...` and `# ...` |

## Example program

```text
purno marks = 78;
doshomik half = marks / 2;
okkhormala msg = "Marks: ";

jodi (marks >= 80) {
    mudron("Ati Uttom");
} onnothay jodi (marks >= 70 && marks < 80) {
    mudron(msg + "Bhalo");
} onnothay {
    mudron("Aro Parishrom Proyojon");
}

jotokkhon (marks < 80) {
    marks = marks + 1;
}
```

## Build with Flex — required submission path

### Windows / PowerShell

From the project directory, use a clean rebuild so an older generated lexer or executable cannot be reused:

```powershell
make clean
make
.\sadhu_bangla_compiler.exe .\onothha_jodi_demo.sbb
```

You can also force a rebuild with:

```powershell
make rebuild
```

Expected output for `onothha_jodi_demo.sbb`:

```text
Compilation successful.
Generated Python file: generated.py

=== PROGRAM OUTPUT ===
Shadharoner Tulonay Bhalo
1
3
4
6
```


Install:

- Flex
- `g++` or another C++17 compiler
- Python 3

Then run:

```bash
make
```

The Makefile performs:

```bash
flex -o lex.yy.cpp sadhu_bangla_lexer.l
g++ -std=c++17 -O2 -Wall -Wextra -pedantic lex.yy.cpp sadhu_bangla_flex_main.cpp -o sadhu_bangla_compiler
```

Compile and run:

```bash
./sadhu_bangla_compiler examples/demo.sbb generated.py
```

Compile without running the generated program:

```bash
./sadhu_bangla_compiler examples/demo.sbb generated.py --no-run
python3 generated.py
```

The executable accepts the same command-line style as the BanglishPP reference project: source file, optional output file, and optional `--no-run`.

## Testing

With Flex installed:

```bash
make test
```

This exercises the **actual Flex lexer path**, then validates the generated Python with `python3 -m py_compile` and checks the runtime output of `examples/demo.sbb`.

When Flex is unavailable, the standalone fallback can still be tested:

```bash
make test-standalone
```

For a course submission where the rubric explicitly requires Lex/Flex, submit and demonstrate the `make` / `make test` path and include `sadhu_bangla_lexer.l`.

## Recursive-descent grammar sketch

```text
program        -> statement* EOF ;
statement      -> declaration
               | assignment
               | print
               | ifStatement
               | whileStatement
               | continue
               | break
               | expression ";" ;

declaration    -> type IDENTIFIER ("=" expression)? ";" ;
assignment     -> IDENTIFIER "=" expression ";" ;
print          -> "mudron" "(" expression ")" ";" ;
ifStatement    -> "jodi" "(" expression ")" block
                  ("onnothay" "jodi" "(" expression ")" block)*
                  ("onnothay" block)? ;
whileStatement -> "jotokkhon" "(" expression ")" block ;
continue       -> "ogrosor" ";" ;
break          -> "sthogito" ";" ;
block          -> "{" statement* "}" ;

expression     -> logicalOr ;
logicalOr      -> logicalAnd ("||" logicalAnd)* ;
logicalAnd     -> equality ("&&" equality)* ;
equality       -> comparison (("==" | "!=") comparison)* ;
comparison     -> term (("<" | "<=" | ">" | ">=") term)* ;
term           -> factor (("+" | "-") factor)* ;
factor         -> unary (("*" | "/" | "%") unary)* ;
unary          -> "-" unary | primary ;
primary        -> INTEGER | FLOAT | STRING | IDENTIFIER
               | "grohon" "(" ")"
               | "(" expression ")" ;
```

## Project structure

```text
part01_token.cpp                  Original token definitions / reference lexer data
part01_token.hpp                  Shared token header for Flex build
part02_lexer.cpp                  Handwritten lexer for standalone fallback
part03_ast_types.cpp              AST and type definitions
part04_parser.cpp                 Recursive Descent parser
part05_type_checker.cpp           Semantic/type checking
part06_python_code_generator.cpp  Python target generator
part07_utils.cpp                  File and Python execution utilities
part08_main.cpp                   Standalone compiler main
sadhu_bangla_lexer.l              Flex/Lex lexer specification
sadhu_bangla_flex_main.cpp        Flex-based compiler main
sadhu_bangla_all_parts.cpp        Standalone all-in-one compiler
Makefile                          Reproducible Flex/fallback build and tests
PROJECT_REPORT.md                 Project report
README_PARTS.md                   Part-by-part implementation notes
examples/                         Example Sadhu Bangla programs
tests/                            Regression tests
```

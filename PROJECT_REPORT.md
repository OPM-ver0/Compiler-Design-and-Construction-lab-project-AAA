# Sadhu Bangla Compiler — Project Report

## 1. Project Overview

Sadhu Bangla is a small educational compiler implemented in C++17. It translates programs written in a Romanized Sadhu Bangla syntax into Python 3 source code.

The compiler pipeline is:

```text
Sadhu Bangla source
        |
        v
   Flex/Lex Lexer
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
   generated.py
```

## 2. Required Tools and Implementation

| Component | Requirement | Implementation |
|---|---|---|
| Lexer | Lex/Flex | `sadhu_bangla_lexer.l` |
| Parser | Recursive Descent / parser generator | Recursive Descent (`part04_parser.cpp`) |
| Implementation language | C++ / Java | C++17 |
| Target language | Java / Python | Python 3 |

The project also retains `sadhu_bangla_all_parts.cpp` and `part02_lexer.cpp` as a standalone fallback. The rubric-facing build is the **Flex build** from the Makefile.

## 3. Language Features

### 3.1 Data types

Three user-visible types are supported:

- `purno` — integer
- `doshomik` — floating point
- `okkhormala` — string

The type checker detects undeclared variables, duplicate declarations, incompatible assignments, invalid arithmetic operands, invalid comparisons, and invalid logical operands.

### 3.2 Arithmetic and precedence

Arithmetic operators:

```text
+  -  *  /  %
```

The recursive-descent parser uses these precedence levels from lowest to highest:

```text
||
&&
== !=
< <= > >=
+ -
* / %
unary -
primary
```

For example:

```text
2 + 3 * 4
```

is parsed as:

```text
2 + (3 * 4)
```

Division produces `doshomik`, matching Python `/` semantics.

### 3.3 Assignment

Supported forms:

```text
purno x = 10;
x = x + 5;
```

Assignments are checked against the declared type.

### 3.4 Conditional control flow

The following forms are supported:

```text
jodi (...) { ... }
onnothay jodi (...) { ... }
onnothay { ... }
```

They are emitted as Python `if`, `elif`, and `else` blocks.

### 3.5 While loops and loop control

The loop construct is:

```text
jotokkhon (...) {
    ...
}
```

Loop control is:

```text
ogrosor;
sthogito;
```

The type checker rejects these statements outside a `jotokkhon` loop.

### 3.6 Input and output

```text
mudron("Hello");
okkhormala naam = grohon();
purno age = grohon();
```

Typed assignments generate `input()`, `int(input())`, or `float(input())` as appropriate.

### 3.7 Comments

Both comment styles are accepted:

```text
// comment
# comment
```

## 4. Flex/Lex Lexer

The file `sadhu_bangla_lexer.l` is the required Lex/Flex specification. It recognizes:

- Sadhu Bangla keywords
- identifiers
- integer and floating-point literals
- strings with common escapes
- arithmetic, comparison, and logical operators
- assignment and punctuation
- `//` and `#` comments
- malformed `!`, `&`, and `|`
- unterminated string literals

The lexer records line and column information and exposes tokens through `lexWithFlex()`.

## 5. Recursive Descent Parser

The parser in `part04_parser.cpp` does not use Bison/Yacc. It directly implements the grammar with functions for declarations, statements, blocks, conditions, loops, and expression precedence.

A basic recovery mechanism catches parse failures and synchronizes at a semicolon, the next source line, or a block boundary. This lets malformed source produce compiler diagnostics instead of an uncontrolled crash.

## 6. Semantic / Type Checking

The type checker validates:

- variable declarations and redeclarations
- undeclared-variable use
- assignment compatibility
- numeric arithmetic
- string concatenation with `+`
- `%` operands
- comparison compatibility
- boolean `&&` and `||`
- boolean `jodi` and `jotokkhon` conditions
- `ogrosor` only inside `jotokkhon`
- `sthogito` only inside `jotokkhon`

Integer-to-floating-point widening is allowed.

## 7. Python Target Hardening

The code generator applies two safeguards:

### Python-safe identifiers

Every Sadhu Bangla source variable is emitted as `sbb_<name>`.

Example:

```text
purno class = 7;
```

becomes:

```python
sbb_class = 7
```

### Python-safe numeric literals

Leading zeros are removed before generation. For example, `00042` becomes `42`, and `0001.50` becomes `1.50`.

## 8. Build

### Required Flex build

```bash
make
```

Equivalent commands:

```bash
flex -o lex.yy.cpp sadhu_bangla_lexer.l
g++ -std=c++17 -O2 -Wall -Wextra -pedantic lex.yy.cpp sadhu_bangla_flex_main.cpp -o sadhu_bangla_compiler
```

Run:

```bash
./sadhu_bangla_compiler examples/demo.sbb generated.py
```

Compile-only mode:

```bash
./sadhu_bangla_compiler examples/demo.sbb generated.py --no-run
```

### Standalone fallback

```bash
make standalone
./sadhu_bangla_standalone examples/demo.sbb generated.py --no-run
python3 generated.py
```

For a course submission whose rubric requires Flex, use the **Flex build** and include `sadhu_bangla_lexer.l`.

## 9. Testing

Run the main regression suite with:

```bash
make test
```

The suite uses the Flex compiler and checks:

- arithmetic precedence
- division typing
- leading-zero literals
- Python-keyword identifiers
- empty input
- assignment/type errors
- missing semicolons
- missing braces
- malformed expressions
- multiple syntax errors
- invalid loop control
- runtime output of `examples/demo.sbb`

Successful Python output is checked using:

```bash
python3 -m py_compile
```

A fallback-only suite is available with:

```bash
make test-standalone
```

## 10. Conclusion

The final Sadhu Bangla implementation follows the same overall structure as the BanglishPP reference: a Flex lexer feeds a shared token stream into a recursive-descent parser, which builds an AST used by semantic checking and Python code generation. The project has a reproducible Makefile, regression tests, basic parser error recovery, and a standalone fallback for environments where Flex is not installed.

# Sadhu Bangla Compiler — Part-by-Part Guide

## Part 01 — Tokens

`part01_token.hpp` defines the token enum, token structure, and readable token names. The Flex lexer and parser share this header.

## Part 02 — Lexer

The rubric-facing lexer is `sadhu_bangla_lexer.l`. It recognizes keywords, identifiers, integers, floating-point numbers, strings, operators, punctuation, and comments while recording line and column positions.

`part02_lexer.cpp` is a handwritten equivalent used only by the standalone fallback.

## Part 03 — AST

`part03_ast_types.cpp` defines the expression and statement nodes used by the parser and later compiler stages.

## Part 04 — Recursive Descent Parser

`part04_parser.cpp` implements recursive descent directly. Expression precedence is encoded as separate functions:

```text
logicalOr
  -> logicalAnd
      -> equality
          -> comparison
              -> term
                  -> factor
                      -> unary
                          -> primary
```

The parser also has basic error recovery by synchronizing at semicolons, source-line boundaries, and block boundaries.

## Part 05 — Type Checker

`part05_type_checker.cpp` checks declarations, assignments, arithmetic, comparisons, logical expressions, conditions, and loop-control placement. Integer-to-floating-point widening is allowed. Division is typed as `doshomik`.

## Part 06 — Python Code Generator

`part06_python_code_generator.cpp` emits Python 3. It prefixes source variables with `sbb_` so a source identifier cannot become a Python keyword collision. Numeric literals are normalized so leading-zero literals remain valid Python.

## Part 07 — Utilities

`part07_utils.cpp` provides file reading/writing, safe shell quoting, and optional execution of generated Python.

## Part 08 — Driver

`part08_main.cpp` is the standalone driver. `sadhu_bangla_flex_main.cpp` is the required Flex-based driver. Both support:

```text
compiler source.sbb [output.py] [--no-run]
```

The Flex driver uses the same parser, type checker, and code generator as the standalone compiler.

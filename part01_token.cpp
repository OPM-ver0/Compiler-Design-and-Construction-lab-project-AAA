#include <bits/stdc++.h>
using namespace std;

#define ll long long

// ================================================================
// Sadhu Bangla Compiler
// Pipeline: Source -> Lexer -> Tokens -> Parser/AST -> Type Checker
//          -> Python Code Generator
//
// Supported features:
//   purno  : integer
//   doshomik : double / floating-point
//   okkhormala   : string
//   jodi / onnothay / onnothay jodi : if / else-if / else
//   jotokkhon   : while
//   mudron(expr)  : print(expr)
//   grohon()     : input()
//   + - * / % with precedence
//   < > <= >= == != comparisons
//   && || logical operators
//   ogrosor / sthogito : continue / break inside jotokkhon loops
//   && / || logical operators
//   && logical AND, || logical OR
//   ogrosor : continue (only inside jotokkhon)
//   sthogito : break (only inside jotokkhon)
//   = assignment
//   // and # comments
// ================================================================

// ------------------------------- Token ---------------------------
enum class TokenType {
    END_OF_FILE,
    IDENTIFIER,
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,

    KW_PURNO,
    KW_DOSHOMIK,
    KW_OKKHORMALA,
    KW_JODI,
    KW_ONNOTHAY,
    KW_JOTOKKHON,
    KW_MUDRON,
    KW_GROHON,
    KW_OGROSOR,
    KW_STHOGITO,

    PLUS,
    MINUS,
    STAR,
    SLASH,
    PERCENT,

    ASSIGN,
    EQUAL_EQUAL,
    NOT_EQUAL,
    LESS,
    LESS_EQUAL,
    GREATER,
    GREATER_EQUAL,
    AND_AND,
    OR_OR,

    LPAREN,
    RPAREN,
    LBRACE,
    RBRACE,
    SEMICOLON,
};

struct Token {
    TokenType type;
    string lexeme;
    int line;
    int column;

    Token(TokenType type, string lexeme, int line, int column)
        : type(type), lexeme(move(lexeme)), line(line), column(column) {}
};

static string tokenTypeName(TokenType t) {
    switch (t) {
        case TokenType::END_OF_FILE: return "end of file";
        case TokenType::IDENTIFIER: return "identifier";
        case TokenType::INTEGER_LITERAL: return "integer literal";
        case TokenType::FLOAT_LITERAL: return "float literal";
        case TokenType::STRING_LITERAL: return "string literal";
        case TokenType::KW_PURNO: return "purno";
        case TokenType::KW_DOSHOMIK: return "doshomik";
        case TokenType::KW_OKKHORMALA: return "okkhormala";
        case TokenType::KW_JODI: return "jodi";
        case TokenType::KW_ONNOTHAY: return "onnothay";
        case TokenType::KW_JOTOKKHON: return "jotokkhon";
        case TokenType::KW_MUDRON: return "mudron";
        case TokenType::KW_GROHON: return "grohon";
        case TokenType::KW_OGROSOR: return "ogrosor";
        case TokenType::KW_STHOGITO: return "sthogito";
        case TokenType::PLUS: return "+";
        case TokenType::MINUS: return "-";
        case TokenType::STAR: return "*";
        case TokenType::SLASH: return "/";
        case TokenType::PERCENT: return "%";
        case TokenType::ASSIGN: return "=";
        case TokenType::EQUAL_EQUAL: return "==";
        case TokenType::NOT_EQUAL: return "!=";
        case TokenType::LESS: return "<";
        case TokenType::LESS_EQUAL: return "<=";
        case TokenType::GREATER: return ">";
        case TokenType::GREATER_EQUAL: return ">=";
        case TokenType::AND_AND: return "&&";
        case TokenType::OR_OR: return "||";
        case TokenType::LPAREN: return "(";
        case TokenType::RPAREN: return ")";
        case TokenType::LBRACE: return "{";
        case TokenType::RBRACE: return "}";
        case TokenType::SEMICOLON: return ";";
    }
    return "unknown token";
}

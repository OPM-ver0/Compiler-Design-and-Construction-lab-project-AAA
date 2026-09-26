// ------------------------- Debug / Trace Printers -------------------------
// Pretty-printers for --tokens / --ast / --all. Pure read-only helpers over
// the Token stream (part01) and the AST (part03) — safe to include in any
// driver (part08_main.cpp, sadhu_bangla_flex_main.cpp, ...).

static string sbbIndent(int depth) { return string(static_cast<size_t>(depth) * 2, ' '); }

static void printTokens(const vector<Token> &tokens) {
    cout << "=== TOKENS ===\n";
    for (const auto &t : tokens) {
        cout << "L" << t.line << ":C" << t.column << "  "
             << left << setw(18) << tokenTypeName(t.type) << right;
        if (!t.lexeme.empty()) cout << " '" << t.lexeme << "'";
        cout << '\n';
    }
    cout << "(" << tokens.size() << " tokens)\n";
}

static void printExprTree(const Expr *e, int depth);
static void printStmtTree(const Stmt *s, int depth);

static void printBlockTree(const BlockStmt *block, int depth) {
    if (!block) { cout << sbbIndent(depth) << "<empty>\n"; return; }
    if (block->statements.empty()) { cout << sbbIndent(depth) << "<empty>\n"; return; }
    for (const auto &s : block->statements) printStmtTree(s.get(), depth);
}

static void printExprTree(const Expr *e, int depth) {
    const string pad = sbbIndent(depth);
    if (!e) { cout << pad << "<none>\n"; return; }

    if (const auto *lit = dynamic_cast<const LiteralExpr *>(e)) {
        string kind = lit->kind == LiteralExpr::Kind::INT ? "INT"
                    : lit->kind == LiteralExpr::Kind::DOUBLE ? "DOUBLE" : "STRING";
        cout << pad << "Literal<" << kind << ">(" << lit->value << ")\n";
    } else if (const auto *v = dynamic_cast<const VariableExpr *>(e)) {
        cout << pad << "Variable(" << v->name << ")\n";
    } else if (dynamic_cast<const InputExpr *>(e)) {
        cout << pad << "Input(grohon)\n";
    } else if (const auto *u = dynamic_cast<const UnaryExpr *>(e)) {
        cout << pad << "Unary(" << tokenTypeName(u->op) << ")\n";
        printExprTree(u->right.get(), depth + 1);
    } else if (const auto *b = dynamic_cast<const BinaryExpr *>(e)) {
        cout << pad << "Binary(" << tokenTypeName(b->op) << ")\n";
        printExprTree(b->left.get(), depth + 1);
        printExprTree(b->right.get(), depth + 1);
    } else {
        cout << pad << "<unknown-expr>\n";
    }
}

static void printStmtTree(const Stmt *s, int depth) {
    const string pad = sbbIndent(depth);
    if (!s) { cout << pad << "<none>\n"; return; }

    if (const auto *v = dynamic_cast<const VarDeclStmt *>(s)) {
        cout << pad << "VarDecl(" << dataTypeName(v->type) << " " << v->name
             << ") [line " << v->line << "]\n";
        if (v->initializer) printExprTree(v->initializer.get(), depth + 1);
    } else if (const auto *a = dynamic_cast<const AssignmentStmt *>(s)) {
        cout << pad << "Assignment(" << a->name << ") [line " << a->line << "]\n";
        printExprTree(a->value.get(), depth + 1);
    } else if (const auto *p = dynamic_cast<const PrintStmt *>(s)) {
        cout << pad << "Print(mudron) [line " << p->line << "]\n";
        printExprTree(p->value.get(), depth + 1);
    } else if (const auto *ex = dynamic_cast<const ExprStmt *>(s)) {
        cout << pad << "ExprStmt [line " << ex->line << "]\n";
        printExprTree(ex->value.get(), depth + 1);
    } else if (const auto *blk = dynamic_cast<const BlockStmt *>(s)) {
        cout << pad << "Block\n";
        printBlockTree(blk, depth + 1);
    } else if (const auto *i = dynamic_cast<const IfStmt *>(s)) {
        cout << pad << "If [line " << i->line << "]\n";
        cout << pad << "  Condition:\n";
        printExprTree(i->condition.get(), depth + 2);
        cout << pad << "  Then:\n";
        printBlockTree(i->thenBranch.get(), depth + 2);
        if (i->elseIf) {
            cout << pad << "  ElseIf:\n";
            printStmtTree(i->elseIf.get(), depth + 2);
        }
        if (i->elseBranch) {
            cout << pad << "  Else:\n";
            printBlockTree(i->elseBranch.get(), depth + 2);
        }
    } else if (const auto *w = dynamic_cast<const WhileStmt *>(s)) {
        cout << pad << "While [line " << w->line << "]\n";
        cout << pad << "  Condition:\n";
        printExprTree(w->condition.get(), depth + 2);
        cout << pad << "  Body:\n";
        printBlockTree(w->body.get(), depth + 2);
    } else if (dynamic_cast<const ContinueStmt *>(s)) {
        cout << pad << "Continue(ogrosor) [line " << s->line << "]\n";
    } else if (dynamic_cast<const BreakStmt *>(s)) {
        cout << pad << "Break(sthogito) [line " << s->line << "]\n";
    } else {
        cout << pad << "<unknown-stmt>\n";
    }
}

static void printAST(const Program &program) {
    cout << "=== AST ===\n";
    cout << "Program\n";
    if (program.statements.empty()) {
        cout << "  <empty>\n";
        return;
    }
    for (const auto &s : program.statements) printStmtTree(s.get(), 1);
}

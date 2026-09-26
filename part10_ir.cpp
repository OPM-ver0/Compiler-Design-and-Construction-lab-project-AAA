// ------------------------- Intermediate Representation -------------------------
// Three-Address Code (3AC) IR.
//
// Every instruction has at most one operator and writes at most one result,
// e.g. `t0 = a + b`. Control flow (jodi / onnothay / jotokkhon / sthogito /
// ogrosor) is lowered to labels and (conditional) jumps, so this stage sits
// between the type checker (part05) and any target code generator (part06
// generates Python directly from the AST; this IR is an additional,
// target-independent view of the same program, useful for optimization
// passes or for driving a different backend later).

enum class IROp {
    // dst = arg1 <binop> arg2
    ADD, SUB, MUL, DIV, MOD,
    EQ, NEQ, LT, LTE, GT, GTE,
    AND, OR,
    // dst = <unop> arg1
    NEG,
    // dst = arg1   (plain copy / materialization of a literal or variable)
    COPY,
    // dst = grohon() (read from stdin); arg1 holds the source DataType name
    INPUT,
    // mudron arg1
    PRINT,
    // control flow
    LABEL,          // dst    = label name being defined
    GOTO,           // dst    = target label
    IF_FALSE_GOTO,  // arg1   = condition operand, dst = target label
    NOP,
};

static string irOpName(IROp op) {
    switch (op) {
        case IROp::ADD: return "ADD";
        case IROp::SUB: return "SUB";
        case IROp::MUL: return "MUL";
        case IROp::DIV: return "DIV";
        case IROp::MOD: return "MOD";
        case IROp::EQ: return "EQ";
        case IROp::NEQ: return "NEQ";
        case IROp::LT: return "LT";
        case IROp::LTE: return "LTE";
        case IROp::GT: return "GT";
        case IROp::GTE: return "GTE";
        case IROp::AND: return "AND";
        case IROp::OR: return "OR";
        case IROp::NEG: return "NEG";
        case IROp::COPY: return "COPY";
        case IROp::INPUT: return "INPUT";
        case IROp::PRINT: return "PRINT";
        case IROp::LABEL: return "LABEL";
        case IROp::GOTO: return "GOTO";
        case IROp::IF_FALSE_GOTO: return "IF_FALSE_GOTO";
        case IROp::NOP: return "NOP";
    }
    return "?";
}

struct IRInstr {
    IROp op;
    string dst;              // result temp/variable, or label name for LABEL/GOTO/IF_FALSE_GOTO
    string arg1;
    string arg2;
    DataType type = DataType::ERROR; // result type, when the instruction produces a value
    int line = 0;

    IRInstr(IROp op, string dst, string arg1, string arg2, DataType type, int line)
        : op(op), dst(move(dst)), arg1(move(arg1)), arg2(move(arg2)), type(type), line(line) {}
};

struct IRModule {
    vector<IRInstr> instructions;
};

// Renders one 3AC instruction as `dst = arg1 op arg2`-style text, matching
// how the operation reads on paper.
static string irInstrToString(const IRInstr &ins) {
    ostringstream o;
    switch (ins.op) {
        case IROp::LABEL:
            o << ins.dst << ":";
            break;
        case IROp::GOTO:
            o << "goto " << ins.dst;
            break;
        case IROp::IF_FALSE_GOTO:
            o << "ifFalse " << ins.arg1 << " goto " << ins.dst;
            break;
        case IROp::PRINT:
            o << "print " << ins.arg1;
            break;
        case IROp::INPUT:
            o << ins.dst << " = input(" << ins.arg1 << ")";
            break;
        case IROp::COPY:
            o << ins.dst << " = " << ins.arg1;
            break;
        case IROp::NEG:
            o << ins.dst << " = -" << ins.arg1;
            break;
        case IROp::NOP:
            o << "nop";
            break;
        default: {
            static const unordered_map<IROp, string> symbol = {
                {IROp::ADD, "+"}, {IROp::SUB, "-"}, {IROp::MUL, "*"},
                {IROp::DIV, "/"}, {IROp::MOD, "%"},
                {IROp::EQ, "=="}, {IROp::NEQ, "!="},
                {IROp::LT, "<"}, {IROp::LTE, "<="},
                {IROp::GT, ">"}, {IROp::GTE, ">="},
                {IROp::AND, "&&"}, {IROp::OR, "||"},
            };
            auto it = symbol.find(ins.op);
            o << ins.dst << " = " << ins.arg1 << " "
              << (it != symbol.end() ? it->second : irOpName(ins.op)) << " " << ins.arg2;
            break;
        }
    }
    return o.str();
}

static void printIR(const IRModule &module) {
    cout << "=== 3AC / IR ===\n";
    for (size_t i = 0; i < module.instructions.size(); ++i) {
        const IRInstr &ins = module.instructions[i];
        if (ins.op == IROp::LABEL) {
            cout << irInstrToString(ins) << "\n";
        } else {
            cout << "  " << setw(4) << left << i << right << irInstrToString(ins) << "\n";
        }
    }
    cout << "(" << module.instructions.size() << " instructions)\n";
}

// Walks the (type-checked) AST and lowers it into linear three-address code.
class IRGenerator {
private:
    IRModule module;
    int tempCounter = 0;
    int labelCounter = 0;

    struct LoopLabels {
        string continueLabel;
        string breakLabel;
    };
    vector<LoopLabels> loopStack;

    string newTemp() { return "t" + to_string(tempCounter++); }
    string newLabel() { return "L" + to_string(labelCounter++); }

    void emit(IROp op, const string &dst, const string &arg1, const string &arg2,
              DataType type, int line) {
        module.instructions.emplace_back(op, dst, arg1, arg2, type, line);
    }

    static IROp binOpToIR(TokenType op) {
        switch (op) {
            case TokenType::PLUS: return IROp::ADD;
            case TokenType::MINUS: return IROp::SUB;
            case TokenType::STAR: return IROp::MUL;
            case TokenType::SLASH: return IROp::DIV;
            case TokenType::PERCENT: return IROp::MOD;
            case TokenType::EQUAL_EQUAL: return IROp::EQ;
            case TokenType::NOT_EQUAL: return IROp::NEQ;
            case TokenType::LESS: return IROp::LT;
            case TokenType::LESS_EQUAL: return IROp::LTE;
            case TokenType::GREATER: return IROp::GT;
            case TokenType::GREATER_EQUAL: return IROp::GTE;
            case TokenType::AND_AND: return IROp::AND;
            case TokenType::OR_OR: return IROp::OR;
            default: return IROp::ADD;
        }
    }

    // Lowers an expression, returning the operand (a temp, a variable name,
    // or a literal) that other instructions can reference.
    string genExpr(const Expr *e, optional<DataType> expected = nullopt) {
        if (!e) return "0";

        if (const auto *x = dynamic_cast<const LiteralExpr *>(e)) {
            if (x->kind == LiteralExpr::Kind::STRING) return "\"" + x->value + "\"";
            return x->value;
        }
        if (const auto *x = dynamic_cast<const VariableExpr *>(e)) {
            return x->name;
        }
        if (dynamic_cast<const InputExpr *>(e)) {
            string t = newTemp();
            DataType dt = expected.value_or(DataType::ERROR);
            emit(IROp::INPUT, t, dataTypeName(dt), "", dt, 0);
            return t;
        }
        if (const auto *x = dynamic_cast<const UnaryExpr *>(e)) {
            string operand = genExpr(x->right.get());
            string t = newTemp();
            emit(IROp::NEG, t, operand, "", e->inferredType, 0);
            return t;
        }
        if (const auto *x = dynamic_cast<const BinaryExpr *>(e)) {
            string left = genExpr(x->left.get());
            string right = genExpr(x->right.get());
            string t = newTemp();
            emit(binOpToIR(x->op), t, left, right, e->inferredType, 0);
            return t;
        }
        return "0";
    }

    void genBlock(const BlockStmt *block) {
        if (!block) return;
        for (const auto &stmt : block->statements) genStmt(stmt.get());
    }

    // `onnothay jodi` chains are represented as nested IfStmt (elseIf); this
    // lowers one link of that chain once its enclosing `ifFalse` has already
    // jumped here.
    void genIfChain(const IfStmt *s, const string &endLabel) {
        string cond = genExpr(s->condition.get());
        string elseLabel = newLabel();
        emit(IROp::IF_FALSE_GOTO, elseLabel, cond, "", DataType::BOOL, s->line);
        genBlock(s->thenBranch.get());
        emit(IROp::GOTO, endLabel, "", "", DataType::ERROR, s->line);
        emit(IROp::LABEL, elseLabel, "", "", DataType::ERROR, s->line);

        if (s->elseIf) {
            genIfChain(s->elseIf.get(), endLabel);
        } else if (s->elseBranch) {
            genBlock(s->elseBranch.get());
        }
    }

    void genStmt(const Stmt *stmt) {
        if (!stmt) return;

        if (const auto *s = dynamic_cast<const VarDeclStmt *>(stmt)) {
            if (dynamic_cast<const InputExpr *>(s->initializer.get())) {
                emit(IROp::INPUT, s->name, dataTypeName(s->type), "", s->type, s->line);
            } else {
                string value = genExpr(s->initializer.get(), s->type);
                emit(IROp::COPY, s->name, value, "", s->type, s->line);
            }
            return;
        }

        if (const auto *s = dynamic_cast<const AssignmentStmt *>(stmt)) {
            if (dynamic_cast<const InputExpr *>(s->value.get())) {
                emit(IROp::INPUT, s->name, dataTypeName(s->value->inferredType), "",
                     s->value->inferredType, s->line);
            } else {
                string value = genExpr(s->value.get());
                emit(IROp::COPY, s->name, value, "", s->value->inferredType, s->line);
            }
            return;
        }

        if (const auto *s = dynamic_cast<const PrintStmt *>(stmt)) {
            string value = genExpr(s->value.get());
            emit(IROp::PRINT, "", value, "", DataType::ERROR, s->line);
            return;
        }

        if (const auto *s = dynamic_cast<const ExprStmt *>(stmt)) {
            genExpr(s->value.get());
            return;
        }

        if (const auto *s = dynamic_cast<const IfStmt *>(stmt)) {
            string endLabel = newLabel();
            genIfChain(s, endLabel);
            emit(IROp::LABEL, endLabel, "", "", DataType::ERROR, s->line);
            return;
        }

        if (const auto *s = dynamic_cast<const WhileStmt *>(stmt)) {
            string startLabel = newLabel();
            string endLabel = newLabel();
            emit(IROp::LABEL, startLabel, "", "", DataType::ERROR, s->line);
            string cond = genExpr(s->condition.get());
            emit(IROp::IF_FALSE_GOTO, endLabel, cond, "", DataType::BOOL, s->line);

            loopStack.push_back({startLabel, endLabel});
            genBlock(s->body.get());
            loopStack.pop_back();

            emit(IROp::GOTO, startLabel, "", "", DataType::ERROR, s->line);
            emit(IROp::LABEL, endLabel, "", "", DataType::ERROR, s->line);
            return;
        }

        if (const auto *s = dynamic_cast<const ContinueStmt *>(stmt)) {
            if (!loopStack.empty()) {
                emit(IROp::GOTO, loopStack.back().continueLabel, "", "", DataType::ERROR, s->line);
            }
            return;
        }

        if (const auto *s = dynamic_cast<const BreakStmt *>(stmt)) {
            if (!loopStack.empty()) {
                emit(IROp::GOTO, loopStack.back().breakLabel, "", "", DataType::ERROR, s->line);
            }
            return;
        }
    }

public:
    IRModule generate(const Program &program) {
        module = IRModule();
        tempCounter = 0;
        labelCounter = 0;
        loopStack.clear();
        for (const auto &stmt : program.statements) genStmt(stmt.get());
        return module;
    }
};

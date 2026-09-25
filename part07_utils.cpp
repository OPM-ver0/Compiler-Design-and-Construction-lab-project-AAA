// ------------------------------ Utils ----------------------------
static string readFile(const string &path) {
    ifstream file(path);
    if (!file) throw runtime_error("Cannot open source file: " + path);
    return string((istreambuf_iterator<char>(file)), istreambuf_iterator<char>());
}

static bool writeFile(const string &path, const string &content) {
    ofstream file(path);
    if (!file) return false;
    file << content;
    return true;
}

static string shellQuote(const string &value) {
    string result = "'";
    for (char c : value) {
        if (c == '\'') result += "'\\''";
        else result += c;
    }
    result += "'";
    return result;
}

static int runPythonProgram(const string &path) {
#ifdef _WIN32
    // PowerShell/Windows cmd does not use POSIX single-quote escaping.
    // Quote the generated Python path with double quotes and escape any
    // embedded double quotes/backslashes for the Windows command shell.
    string quoted = "\"";
    for (char c : path) {
        if (c == '\\' || c == '\"') quoted += '\\';
        quoted += c;
    }
    quoted += "\"";
    const string command = "python " + quoted;
#else
    const string command = "python3 " + shellQuote(path);
#endif
    return system(command.c_str());
}

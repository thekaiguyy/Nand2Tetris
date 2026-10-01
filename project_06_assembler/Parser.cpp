// Parser.cpp
#include "Parser.h"

#include <cctype>
#include <fstream>

Parser::Parser(const std::string& path) {
    std::ifstream in(path);
    if (!in) {
        throw AsmError(0, "cannot open input file '" + path + "'");
    }

    std::string raw;
    int lineNo = 0;
    while (std::getline(in, raw)) {
        ++lineNo;

        // Step 1: strip a "//" comment (everything from "//" to end of line).
        std::size_t c = raw.find("//");
        if (c != std::string::npos) raw.erase(c);

        // Step 2: remove ALL whitespace. The Hack spec says whitespace is
        // meaningless, so "D = M + 1" is the same as "D=M+1". Removing it
        // now means the rest of the parser never has to think about spaces.
        // (This also deals with '\r' from Windows line endings.)
        std::string cleaned;
        for (char ch : raw) {
            if (!std::isspace(static_cast<unsigned char>(ch))) cleaned += ch;
        }

        // Step 3: skip lines that were blank or comment-only.
        if (cleaned.empty()) continue;

        commands_.push_back(cleaned);
        lineNums_.push_back(lineNo);
    }
}

bool Parser::hasMoreCommands() const {
    return pos_ + 1 < static_cast<int>(commands_.size());
}

void Parser::reset() {
    pos_ = -1;
}

void Parser::advance() {
    ++pos_;
    parseCurrent();
}

void Parser::parseCurrent() {
    const std::string& cmd = commands_[pos_];
    const int line = lineNums_[pos_];

    // Clear fields from the previous command so stale data can't leak through.
    symbol_.clear();
    dest_.clear();
    comp_.clear();
    jump_.clear();

    // ---- A-command: "@xxx" ------------------------------------------------
    if (cmd[0] == '@') {
        type_ = CommandType::A_COMMAND;
        symbol_ = cmd.substr(1);
        if (symbol_.empty()) {
            throw AsmError(line, "'@' must be followed by a number or symbol");
        }
        return;
    }

    // ---- L-command: "(LABEL)" --------------------------------------------
    if (cmd[0] == '(') {
        type_ = CommandType::L_COMMAND;
        if (cmd.back() != ')') {
            throw AsmError(line, "label is missing its closing ')': " + cmd);
        }
        symbol_ = cmd.substr(1, cmd.size() - 2);
        if (symbol_.empty()) {
            throw AsmError(line, "empty label '()'");
        }
        return;
    }

    // ---- C-command: "dest=comp;jump" -------------------------------------
    // Both "dest=" and ";jump" are optional, but comp is always required.
    //   D=M        -> dest + comp
    //   D;JGT      -> comp + jump
    //   0;JMP      -> comp + jump
    //   M=D+1;JEQ  -> all three
    type_ = CommandType::C_COMMAND;
    std::string rest = cmd;

    std::size_t eq = rest.find('=');
    if (eq != std::string::npos) {
        if (rest.find('=', eq + 1) != std::string::npos) {
            throw AsmError(line, "more than one '=' in instruction: " + cmd);
        }
        dest_ = rest.substr(0, eq);
        if (dest_.empty()) {
            throw AsmError(line, "'=' with nothing before it: " + cmd);
        }
        rest = rest.substr(eq + 1);  // what's left is "comp" or "comp;jump"
    }

    std::size_t semi = rest.find(';');
    if (semi != std::string::npos) {
        if (rest.find(';', semi + 1) != std::string::npos) {
            throw AsmError(line, "more than one ';' in instruction: " + cmd);
        }
        jump_ = rest.substr(semi + 1);
        if (jump_.empty()) {
            throw AsmError(line, "';' with no jump mnemonic after it: " + cmd);
        }
        comp_ = rest.substr(0, semi);
    } else {
        comp_ = rest;
    }

    if (comp_.empty()) {
        throw AsmError(line, "instruction has no computation part: " + cmd);
    }
}

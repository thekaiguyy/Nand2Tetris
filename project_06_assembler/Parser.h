// Parser.h
// ---------------------------------------------------------------------------
// The Parser's job: read the .asm file and break each instruction into its
// parts. It knows NOTHING about binary codes or symbol addresses - it only
// understands the *syntax* of Hack assembly.
//
// This follows the API from the nand2tetris book (Chapter 6, Figure 6.4).
// ---------------------------------------------------------------------------
#ifndef PARSER_H
#define PARSER_H

#include <stdexcept>
#include <string>
#include <vector>

// The three kinds of Hack assembly commands.
enum class CommandType {
    A_COMMAND,  // @xxx         (xxx is a number or a symbol)
    C_COMMAND,  // dest=comp;jump
    L_COMMAND   // (LABEL)      a "pseudo-command": produces no machine code
};

// One error type for the whole assembler. Carries the source line number so
// the user can jump straight to the problem.
class AsmError : public std::runtime_error {
public:
    AsmError(int line, const std::string& msg)
        : std::runtime_error(msg), line_(line) {}
    int line() const { return line_; }

private:
    int line_;
};

class Parser {
public:
    // Opens the file and loads every real command (comments/blank lines are
    // thrown away here). Throws AsmError(0, ...) if the file can't be opened.
    explicit Parser(const std::string& path);

    // Are there more commands to read?
    bool hasMoreCommands() const;

    // Moves to the next command. Call this BEFORE reading the first command.
    // Throws AsmError if the command is syntactically malformed.
    void advance();

    // Go back to the start of the file. The assembler makes two passes over
    // the same program, so we need this.
    void reset();

    CommandType commandType() const { return type_; }

    // For A_COMMAND "@xxx" -> "xxx".  For L_COMMAND "(xxx)" -> "xxx".
    std::string symbol() const { return symbol_; }

    // For C_COMMAND only. Each returns the *text* mnemonic ("" if absent).
    // Example: "MD=D+1;JGT" -> dest "MD", comp "D+1", jump "JGT".
    std::string dest() const { return dest_; }
    std::string comp() const { return comp_; }
    std::string jump() const { return jump_; }

    // Line number in the ORIGINAL file (1-based) of the current command.
    // Used for error messages.
    int lineNumber() const { return pos_ >= 0 ? lineNums_[pos_] : 0; }

private:
    // One cleaned-up command per entry (no comments, no whitespace).
    std::vector<std::string> commands_;
    // lineNums_[i] = original file line where commands_[i] came from.
    std::vector<int> lineNums_;

    // Index of the current command. Starts at -1 ("before the first one").
    int pos_ = -1;

    // Fields filled in by advance() for the current command.
    CommandType type_ = CommandType::A_COMMAND;
    std::string symbol_, dest_, comp_, jump_;

    void parseCurrent();
};

#endif

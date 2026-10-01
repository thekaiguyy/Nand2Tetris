// main.cpp - the Hack assembler driver
// ---------------------------------------------------------------------------
// Usage:  ./assembler Prog.asm [Prog.hack]
//
// Translates Hack assembly into Hack machine code (one 16-bit binary string
// per line). It makes TWO passes over the source:
//
//   Pass 1: find every (LABEL) and record the ROM address it points to.
//           This is needed because code can jump FORWARD to a label that
//           hasn't been seen yet, e.g.  @END ... (END)
//
//   Pass 2: translate each instruction. Now every label is known, so any
//           symbol that is still unknown must be a variable: give it the
//           next free RAM address, starting at 16.
// ---------------------------------------------------------------------------
#include <bitset>
#include <cctype>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "Code.h"
#include "Parser.h"
#include "SymbolTable.h"

namespace {

const int kFirstVariableAddress = 16;
const int kMaxVariableAddress   = 16383;  // 16384 is SCREEN, so variables must stop before it
const int kMaxAConstant         = 32767;  // A-instructions only have 15 bits for the value

// "123" -> true. Used to decide: is "@xxx" a constant or a symbol?
bool isAllDigits(const std::string& s) {
    if (s.empty()) return false;
    for (char c : s) {
        if (!std::isdigit(static_cast<unsigned char>(c))) return false;
    }
    return true;
}

// Hack symbols: letters, digits, '_', '.', '$', ':' and they can't start with a digit.
bool isValidSymbol(const std::string& s) {
    if (s.empty() || std::isdigit(static_cast<unsigned char>(s[0]))) return false;
    for (char c : s) {
        if (!(std::isalnum(static_cast<unsigned char>(c)) ||
              c == '_' || c == '.' || c == '$' || c == ':')) {
            return false;
        }
    }
    return true;
}

// "foo.asm" -> "foo.hack";  "foo" -> "foo.hack"
std::string defaultOutputName(const std::string& in) {
    const std::string ext = ".asm";
    if (in.size() > ext.size() && in.compare(in.size() - ext.size(), ext.size(), ext) == 0) {
        return in.substr(0, in.size() - ext.size()) + ".hack";
    }
    return in + ".hack";
}

// Pass 1: record every label's ROM address.
void firstPass(Parser& parser, SymbolTable& symbols) {
    int romAddress = 0;  // address of the next instruction that will be emitted

    while (parser.hasMoreCommands()) {
        parser.advance();

        if (parser.commandType() == CommandType::L_COMMAND) {
            // A label emits NO code. It just names the address of whatever
            // instruction comes next - which is exactly romAddress right now.
            const std::string label = parser.symbol();
            if (!isValidSymbol(label)) {
                throw AsmError(parser.lineNumber(), "invalid label name '" + label + "'");
            }
            if (symbols.contains(label)) {
                throw AsmError(parser.lineNumber(), "symbol '" + label + "' is already defined");
            }
            symbols.addEntry(label, romAddress);
        } else {
            // A- and C-commands each become exactly one line of machine code.
            ++romAddress;
        }
    }
}

// Translate "@xxx" into 16 bits: a leading 0, then the 15-bit value.
std::string translateA(Parser& parser, SymbolTable& symbols, int& nextVariable) {
    const std::string sym = parser.symbol();
    const int line = parser.lineNumber();
    int value;

    if (std::isdigit(static_cast<unsigned char>(sym[0]))) {
        // Constant, e.g. @21
        if (!isAllDigits(sym)) {
            throw AsmError(line, "invalid number or symbol '@" + sym + "'");
        }
        // Check length first so stoi can't overflow on something like @99999999999.
        if (sym.size() > 5 || std::stoi(sym) > kMaxAConstant) {
            throw AsmError(line, "constant '@" + sym + "' is too large (max " +
                                     std::to_string(kMaxAConstant) + ")");
        }
        value = std::stoi(sym);
    } else {
        // Symbol, e.g. @LOOP or @i
        if (!isValidSymbol(sym)) {
            throw AsmError(line, "invalid symbol name '" + sym + "'");
        }
        if (!symbols.contains(sym)) {
            // Not predefined, not a label -> it's a variable. Give it the next free RAM slot.
            if (nextVariable > kMaxVariableAddress) {
                throw AsmError(line, "out of RAM for variables (too many distinct symbols)");
            }
            symbols.addEntry(sym, nextVariable++);
        }
        value = symbols.getAddress(sym);
    }

    // bitset<15> keeps exactly 15 bits; prepend '0' to mark an A-instruction.
    return "0" + std::bitset<15>(value).to_string();
}

// Translate "dest=comp;jump" into "111" + comp(7) + dest(3) + jump(3).
std::string translateC(Parser& parser) {
    try {
        return "111" + Code::comp(parser.comp()) + Code::dest(parser.dest()) +
               Code::jump(parser.jump());
    } catch (const std::invalid_argument& e) {
        // Code doesn't know about line numbers, so we attach one here.
        throw AsmError(parser.lineNumber(), e.what());
    }
}

// Pass 2: emit machine code.
std::vector<std::string> secondPass(Parser& parser, SymbolTable& symbols) {
    std::vector<std::string> output;
    int nextVariable = kFirstVariableAddress;

    parser.reset();  // rewind to the top of the file
    while (parser.hasMoreCommands()) {
        parser.advance();

        switch (parser.commandType()) {
            case CommandType::A_COMMAND:
                output.push_back(translateA(parser, symbols, nextVariable));
                break;
            case CommandType::C_COMMAND:
                output.push_back(translateC(parser));
                break;
            case CommandType::L_COMMAND:
                break;  // labels were fully handled in pass 1; nothing to emit
        }
    }
    return output;
}

}  // namespace

int main(int argc, char* argv[]) {
    if (argc < 2 || argc > 3) {
        std::cerr << "Usage: " << argv[0] << " Prog.asm [Prog.hack]\n";
        return 2;
    }

    const std::string inPath  = argv[1];
    const std::string outPath = (argc == 3) ? argv[2] : defaultOutputName(inPath);

    try {
        Parser parser(inPath);
        SymbolTable symbols;

        firstPass(parser, symbols);
        std::vector<std::string> machineCode = secondPass(parser, symbols);

        // Only write the file once everything succeeded, so a failed run
        // never leaves a half-written .hack file behind.
        std::ofstream out(outPath);
        if (!out) {
            throw AsmError(0, "cannot open output file '" + outPath + "'");
        }
        for (const std::string& word : machineCode) {
            out << word << '\n';
        }
        std::cout << "Assembled " << machineCode.size() << " instructions -> " << outPath << "\n";
    } catch (const AsmError& e) {
        // Format like a compiler error: file:line: message
        std::cerr << inPath;
        if (e.line() > 0) std::cerr << ":" << e.line();
        std::cerr << ": error: " << e.what() << "\n";
        return 1;
    }
    return 0;
}

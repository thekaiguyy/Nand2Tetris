// SymbolTable.h
// ---------------------------------------------------------------------------
// Maps symbol names to addresses. Three kinds of symbols end up in here:
//
//   1. Predefined:  R0..R15, SP, LCL, ARG, THIS, THAT, SCREEN, KBD
//   2. Labels:      (LOOP)  -> address of the NEXT instruction in ROM
//   3. Variables:   @i      -> next free RAM slot, starting at 16
//
// (Header-only: it's small, and keeping it inline avoids an extra .cpp file.)
// ---------------------------------------------------------------------------
#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>

class SymbolTable {
public:
    // Starts out containing the predefined symbols.
    SymbolTable() {
        for (int i = 0; i <= 15; ++i) {
            table_["R" + std::to_string(i)] = i;  // R0..R15 -> 0..15
        }
        table_["SP"]     = 0;
        table_["LCL"]    = 1;
        table_["ARG"]    = 2;
        table_["THIS"]   = 3;
        table_["THAT"]   = 4;
        table_["SCREEN"] = 16384;  // start of the screen memory map
        table_["KBD"]    = 24576;  // keyboard memory map (1 word)
    }

    void addEntry(const std::string& symbol, int address) {
        table_[symbol] = address;
    }

    bool contains(const std::string& symbol) const {
        return table_.count(symbol) != 0;
    }

    int getAddress(const std::string& symbol) const {
        return table_.at(symbol);
    }

private:
    std::unordered_map<std::string, int> table_;
};

#endif

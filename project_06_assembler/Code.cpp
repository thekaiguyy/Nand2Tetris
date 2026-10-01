// Code.cpp
#include "Code.h"

#include <bitset>
#include <stdexcept>
#include <unordered_map>

// ---------------------------------------------------------------------------
// dest: which register(s) receive the result. d1=A, d2=D, d3=M.
//
//   null=000  M=001  D=010  MD=011  A=100  AM=101  AD=110  AMD=111
//
// Rather than a table, we build the 3 bits from the letters. That way "MD" and
// "DM" both work (they mean the same thing) and we don't have to list all 8.
// ---------------------------------------------------------------------------
std::string Code::dest(const std::string& m) {
    int bits = 0;
    for (char c : m) {
        int bit;
        switch (c) {
            case 'A': bit = 4; break;  // 100 -> d1
            case 'D': bit = 2; break;  // 010 -> d2
            case 'M': bit = 1; break;  // 001 -> d3
            default:
                throw std::invalid_argument("invalid dest '" + m + "' (only A, D, M allowed)");
        }
        if (bits & bit) {
            throw std::invalid_argument("invalid dest '" + m + "' (repeated register)");
        }
        bits |= bit;
    }
    return std::bitset<3>(bits).to_string();  // e.g. 5 -> "101"
}

// ---------------------------------------------------------------------------
// comp: what the ALU computes. The first bit is 'a'.
// Left column of the table uses A, right column uses M (a=1).
// ---------------------------------------------------------------------------
std::string Code::comp(const std::string& m) {
    static const std::unordered_map<std::string, std::string> table = {
        // a = 0  (operates on A)
        {"0",   "0101010"}, {"1",   "0111111"}, {"-1",  "0111010"},
        {"D",   "0001100"}, {"A",   "0110000"}, {"!D",  "0001101"},
        {"!A",  "0110001"}, {"-D",  "0001111"}, {"-A",  "0110011"},
        {"D+1", "0011111"}, {"A+1", "0110111"}, {"D-1", "0001110"},
        {"A-1", "0110010"}, {"D+A", "0000010"}, {"D-A", "0010011"},
        {"A-D", "0000111"}, {"D&A", "0000000"}, {"D|A", "0010101"},
        // a = 1  (same ALU codes, but operates on M instead of A)
        {"M",   "1110000"}, {"!M",  "1110001"}, {"-M",  "1110011"},
        {"M+1", "1110111"}, {"M-1", "1110010"}, {"D+M", "1000010"},
        {"D-M", "1010011"}, {"M-D", "1000111"}, {"D&M", "1000000"},
        {"D|M", "1010101"},
    };

    auto it = table.find(m);
    if (it == table.end()) {
        throw std::invalid_argument("invalid comp '" + m + "'");
    }
    return it->second;
}

// ---------------------------------------------------------------------------
// jump: j1=(out<0), j2=(out=0), j3=(out>0). A jump happens if ANY selected
// condition is true, so JGE = "greater OR equal" = 011.
// ---------------------------------------------------------------------------
std::string Code::jump(const std::string& m) {
    static const std::unordered_map<std::string, std::string> table = {
        {"",    "000"},  // no jump
        {"JGT", "001"}, {"JEQ", "010"}, {"JGE", "011"},
        {"JLT", "100"}, {"JNE", "101"}, {"JLE", "110"},
        {"JMP", "111"},
    };

    auto it = table.find(m);
    if (it == table.end()) {
        throw std::invalid_argument("invalid jump '" + m + "'");
    }
    return it->second;
}

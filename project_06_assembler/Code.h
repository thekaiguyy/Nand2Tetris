// Code.h
// ---------------------------------------------------------------------------
// The Code module translates Hack mnemonics into binary *strings* of 0s and 1s.
// It is a pure lookup/translation layer: no file I/O, no symbols.
//
// A C-instruction is 16 bits:
//
//     bit:   15 14 13 | 12 | 11 10 09 08 07 06 | 05 04 03 | 02 01 00
//             1  1  1 |  a |  c1 c2 c3 c4 c5 c6 | d1 d2 d3 | j1 j2 j3
//            (fixed)   (comp, 7 bits incl. 'a')  (dest)      (jump)
//
// 'a' says whether the ALU's second input is A (a=0) or M = RAM[A] (a=1).
// ---------------------------------------------------------------------------
#ifndef CODE_H
#define CODE_H

#include <string>

class Code {
public:
    // Each function returns a binary string and throws std::invalid_argument
    // if the mnemonic isn't valid. main.cpp catches that and adds the line number.

    static std::string dest(const std::string& mnemonic);  // 3 bits: d1 d2 d3
    static std::string comp(const std::string& mnemonic);  // 7 bits: a c1..c6
    static std::string jump(const std::string& mnemonic);  // 3 bits: j1 j2 j3
};

#endif

/**
* literals.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef LITERALS_H
#define LITERALS_H

#include <cstdint>
#include <string>
#include <string.h>

// unsigned int types
constexpr std::size_t operator "" _z(unsigned long long v) { return static_cast<std::size_t>(v); }

constexpr std::uint8_t operator"" _u8(unsigned long long v) { return static_cast<std::uint8_t>(v); }
constexpr std::uint16_t operator"" _u16(unsigned long long v) { return static_cast<std::uint16_t>(v); }
constexpr std::uint32_t operator"" _u32(unsigned long long v) { return static_cast<std::uint32_t>(v); }
constexpr std::uint64_t operator"" _u64(unsigned long long v) { return static_cast<std::uint64_t>(v); }

constexpr unsigned char operator"" _uc(unsigned long long v) { return static_cast<unsigned char>(v); }
constexpr unsigned short operator"" _us(unsigned long long v) { return static_cast<unsigned short>(v); }

// C-string literal
constexpr const char* operator "" _cs(const char* v) { return v; }

#endif //LITERALS_H

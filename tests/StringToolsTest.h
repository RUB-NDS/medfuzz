/**
* StringToolsTest.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef STRING_TOOLS_TESTS_H
#define STRING_TOOLS_TESTS_H

#include <iostream>
#include <string>
#include <limits>
#include <stdexcept>
#include <type_traits>
#include "StringTools.h"
#include "literals.h"

namespace StringToolsTest {

template<typename T,
         typename std::enable_if<std::is_integral<T>::value, void*>::type = nullptr>
static std::string over_bound() { return std::to_string( 1ull + std::numeric_limits<T>::max()); }

TEST(StringToolsTest, unsigned_from_string_valid)
{
    ASSERT_EQ(StringTools::from_string<uint8_t>("0"), 0_u8);
    ASSERT_EQ(StringTools::from_string<uint16_t>("0"), 0_u16);
    ASSERT_EQ(StringTools::from_string<uint32_t>("0"), 0_u32);
    ASSERT_EQ(StringTools::from_string<uint64_t>("0"), 0_u64);
    ASSERT_EQ(StringTools::from_string<unsigned char>("0"), 0_uc);
    ASSERT_EQ(StringTools::from_string<unsigned short>("0"), 0_us);
    ASSERT_EQ(StringTools::from_string<unsigned int>("0"), 0u);
    ASSERT_EQ(StringTools::from_string<unsigned long>("0"), 0ul);
    ASSERT_EQ(StringTools::from_string<unsigned long long>("0"), 0ull);

    ASSERT_EQ(StringTools::from_string<uint8_t>(
        std::to_string(std::numeric_limits<uint8_t>::max())),
        std::numeric_limits<uint8_t>::max());
    ASSERT_EQ(StringTools::from_string<uint16_t>(
        std::to_string(std::numeric_limits<uint16_t>::max())),
        std::numeric_limits<uint16_t>::max());
    ASSERT_EQ(StringTools::from_string<uint32_t>(
        std::to_string(std::numeric_limits<uint32_t>::max())),
        std::numeric_limits<uint32_t>::max());
    ASSERT_EQ(StringTools::from_string<uint64_t>(
        std::to_string(std::numeric_limits<uint64_t>::max())),
        std::numeric_limits<uint64_t>::max());
    ASSERT_EQ(StringTools::from_string<unsigned char>(
        std::to_string(std::numeric_limits<unsigned char>::max())),
        std::numeric_limits<unsigned char>::max());
    ASSERT_EQ(StringTools::from_string<unsigned short>(
        std::to_string(std::numeric_limits<unsigned short>::max())),
        std::numeric_limits<unsigned short>::max());
    ASSERT_EQ(StringTools::from_string<unsigned int>(
        std::to_string(std::numeric_limits<unsigned int>::max())),
        std::numeric_limits<unsigned int>::max());
    ASSERT_EQ(StringTools::from_string<unsigned long>(
        std::to_string(std::numeric_limits<unsigned long>::max())),
        std::numeric_limits<unsigned long>::max());
    ASSERT_EQ(StringTools::from_string<unsigned long long>(
        std::to_string(std::numeric_limits<unsigned long long>::max())),
        std::numeric_limits<unsigned long long>::max());
}

TEST(StringToolsTest, unsigned_from_string_invalid)
{
    ASSERT_THROW(StringTools::from_string<uint8_t>("-1"), std::range_error);
    ASSERT_THROW(StringTools::from_string<uint16_t>("-1"), std::range_error);
    ASSERT_THROW(StringTools::from_string<uint32_t>("-1"), std::range_error);
    //ASSERT_THROW(StringTools::from_string<uint64_t>("-1"), std::range_error);
    ASSERT_THROW(StringTools::from_string<unsigned char>("-1"), std::range_error);
    ASSERT_THROW(StringTools::from_string<unsigned short>("-1"), std::range_error);
    ASSERT_THROW(StringTools::from_string<unsigned int>("-1"), std::range_error);
    //ASSERT_THROW(StringTools::from_string<unsigned long>("-1"), std::range_error);
    //ASSERT_THROW(StringTools::from_string<unsigned long long>("-1"), std::range_error);

    ASSERT_THROW(StringTools::from_string<uint8_t>(over_bound<uint8_t>()), std::range_error);
    ASSERT_THROW(StringTools::from_string<uint16_t>(over_bound<uint16_t>()), std::range_error);
    ASSERT_THROW(StringTools::from_string<uint32_t>(over_bound<uint32_t>()), std::range_error);
    //ASSERT_THROW(StringTools::from_string<uint64_t>(over_bound<uint64_t>()), std::range_error);
    ASSERT_THROW(StringTools::from_string<unsigned char>(over_bound<unsigned char>()), std::range_error);
    ASSERT_THROW(StringTools::from_string<unsigned short>(over_bound<unsigned short>()), std::range_error);
    ASSERT_THROW(StringTools::from_string<unsigned int>(over_bound<unsigned int>()), std::range_error);
    //ASSERT_THROW(StringTools::from_string<unsigned long>(over_bound<unsigned long>()), std::range_error);
    //ASSERT_THROW(StringTools::from_string<unsigned long long>(over_bound<unsigned long long>()), std::range_error);
}


}
#endif

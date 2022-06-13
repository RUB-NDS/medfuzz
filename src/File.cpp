/**
* File.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <iterator>
#include "File.h"
#include "Log.h"

std::vector<uint8_t> File::to_bytes()
{
    auto ifs = std::ifstream(m_path, std::ios::binary);
    if(ifs.fail())
    {
        Log::error("Error reading from file.", Log::Verbosity::NORMAL);
    }

    std::vector<uint8_t> v { std::istreambuf_iterator<char>(ifs), {} };
    ifs.close();
    return v;
}

std::string File::to_string()
{
    auto content_bytes = to_bytes();
    return std::string { content_bytes.begin(), content_bytes.end() };
}

/**
* DelimitedRawStreamWriter.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <ostream>
#include "DelimitedRawStreamWriter.h"

void DelimitedRawStreamWriter::write(const std::vector<uint8_t>& data)
{
    stream() << std::endl << m_header << std::endl;
    RawStreamWriter::write(data);
    stream() << std::endl << m_footer << std::endl;
}

std::string DelimitedRawStreamWriter::get_delimiter(
    const std::string& label,
    const std::string& prefix)
{
    return label == "" ?
        "--- " + prefix + " ---" :
        "--- " + prefix + "[" + label + "] ---";
}


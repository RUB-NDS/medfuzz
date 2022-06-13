/**
* RawStreamWriter.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include <algorithm>
#include <iostream>
#include "RawStreamWriter.h"

void RawStreamWriter::write(const std::vector<uint8_t>& data)
{
    std::for_each(data.begin(), data.end(), [&](uint8_t d) { stream() << d; });
}

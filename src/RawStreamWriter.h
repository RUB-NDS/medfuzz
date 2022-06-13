/**
* RawStreamWriter.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef RAW_STREAM_WRITER_H
#define RAW_STREAM_WRITER_H

#include <cstdint>
#include <vector>
#include "StreamWriter.h"

/**
 * Writes raw bytes to an output stream.
 */
class RawStreamWriter
    : public StreamWriter<std::vector<uint8_t>>
{
    public:
        using StreamWriter::StreamWriter;
        virtual void write(const std::vector<uint8_t>& data) override;
};

#endif // RAW_STREAM_WRITER_H

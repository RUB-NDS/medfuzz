/**
* DelimitedRawStreamWriter.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef DELIMITED_RAW_STREAM_WRITER_H
#define DELIMITED_RAW_STREAM_WRITER_H

#include <cstdint>
#include <string>
#include <vector>
#include "RawStreamWriter.h"

class DelimitedRawStreamWriter
    : public RawStreamWriter
{
    public:
        /**
         * Writes network messages to an output stream. A begin
         * header and an end footer delimits distinct messages.
         **/
        explicit DelimitedRawStreamWriter(const std::string& label)
            : RawStreamWriter(),
              m_header(get_delimiter(label, "BEGIN")),
              m_footer(get_delimiter(label, "END")) {}
            virtual void write (
                const std::vector<uint8_t> & data) override;

    private:
        static std::string get_delimiter(const std::string& label,
            const std::string& prefix);
        const std::string m_header;
        const std::string m_footer;
};

#endif //DELIMITED_RAW_STREAM_WRITER_H

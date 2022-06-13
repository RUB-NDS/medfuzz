/**
* StringWriter.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef STRING_WRITER_H
#define STRING_WRITER_H

#include <sstream>
#include <ostream>
#include "RawStreamWriter.h"

class StringWriter : public RawStreamWriter
{
    public:
        StringWriter()
            : RawStreamWriter(), m_stream() {}
        virtual ~StringWriter() {}
        std::string to_string() { return m_stream.str(); }

    protected:
        virtual std::ostream& stream() override { return m_stream; }
        
    private:
        std::stringstream m_stream;
};

#endif //STRING_WRITER_H

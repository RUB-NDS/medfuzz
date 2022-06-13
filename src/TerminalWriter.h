/**
* TerminalWriter.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef TERMINAL_WRITER_H
#define TERMINAL_WRITER_H

#include <iostream>
#include "StringWriter.h"
#include "RawStreamWriter.h"

class TerminalWriter : public RawStreamWriter
{
    public:
        using RawStreamWriter::RawStreamWriter;
    
    protected:
        virtual std::ostream& stream() override { return std::cout; }
    
};

#endif //TERMINAL_WRITER_H

/**
* StreamWriter.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef STREAM_WRITER_H
#define STREAM_WRITER_H

#include<ostream>

/**
 * Interface for output stream writing capabilities.
 */
template<typename T>
class StreamWriter
{
    public:
        StreamWriter<T>() {}
        virtual ~StreamWriter<T>() {}
        virtual void write (const T& data) = 0;        
    
    protected:
        virtual std::ostream& stream() = 0;
};

#endif // STREAM_WRITER_H

/**
* Network/IO.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef NETWORK_IO_H
#define NETWORK_IO_H

#include <cstdint>
#include <string>
#include <vector>
#include "Closable.h"
#include "Network/IOEventProvider.h"

namespace Network {

class IO
    : public virtual IOEventProvider,
      public virtual Closable
{
    public:
        virtual void write(std::vector<uint8_t> data) = 0;
        virtual void write(std::string data) = 0;

    protected:
        IO();
        virtual ~IO();
};

}

#endif //NETWORK_IO_H

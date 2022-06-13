/**
* Closable.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef CLOSABLE_H
#define CLOSABLE_H

class Closable
{
    public:
        virtual void close() = 0;
};

#endif //CLOSABLE_H

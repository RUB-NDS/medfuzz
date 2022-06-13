/**
* EventListener.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef EVENT_LISTENER_H
#define EVENT_LISTENER_H

template<class E>
class EventListener
{
    public:
        virtual void handle_event(E ev) = 0;
};

#endif //EVENT_LISTENER_H

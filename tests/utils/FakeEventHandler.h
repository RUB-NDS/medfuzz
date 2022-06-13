/**
* utils/FakeEventHandler.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef TESTS_UTILS_FAKE_EVENT_HANDLER_H
#define TESTS_UTILS_FAKE_EVENT_HANDLER_H

#include <vector>
#include "EventListener.h"

namespace Tests { namespace Utils { 

template<typename E, typename T>
class FakeEventHandler
    : public EventListener<E>
{
    public:
        FakeEventHandler() : m_data() {}
        FakeEventHandler(T& data) : m_data(data) {}
        virtual ~FakeEventHandler() {}

        void handle_event(E ev) { check(ev); m_event.emplace_back(ev); }
        std::vector<E>& events() { return m_event; }
        T& data() { return m_data.value(); }
        virtual void check(E ev) { return; }
    
    private:
        std::vector<E> m_event;
        std::optional<T> m_data;
};
}}

#endif //TESTS_UTILS_FAKE_EVENT_HANDLER_H

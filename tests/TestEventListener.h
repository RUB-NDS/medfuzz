/**
* TestEventListener.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef TEST_EVENT_LISTENER_H
#define TEST_EVENT_LISTENER_H

#include <string>
#include "EventListener.h"
#include "TestEvent.h"

class TestEventListener : public EventListener<TestEvent>
{
    public:
        TestEventListener() {}
        virtual ~TestEventListener() {}
        void handle_event(TestEvent e) { m_events.emplace_back(e); }
        std::vector<TestEvent>& events() { return m_events; }

     private:
        std::vector<TestEvent> m_events;
};

#endif //TEST_EVENT_LISTENER_H


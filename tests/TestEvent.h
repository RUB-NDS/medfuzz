/**
* TestEvent.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef TEST_EVENT_H
#define TEST_EVENT_H

class TestEvent
{
    public:
        TestEvent() : m_data() {}
        TestEvent(const std::string& data) : m_data(data) {}
        std::string& data() { return m_data; }

    private:
        std::string m_data;
};

#endif //TEST_EVENT_H


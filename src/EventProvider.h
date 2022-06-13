/**
* EventProvider.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef EVENT_PROVIDER_H
#define EVENT_PROVIDER_H

#include <vector>
#include <memory>
#include <type_traits>
#include <algorithm>
#include <mutex>
#include "EventListener.h"

template<class E>
class IEventProvider
{
    public:
        virtual void subscribe(std::shared_ptr<EventListener<E>> listener) = 0;
        virtual void unsubscribe(std::shared_ptr<EventListener<E>> listener) = 0;
};

template<class E>
class EventProvider : public IEventProvider<E>
{
    public:
        EventProvider() : m_listeners() {};
        virtual ~EventProvider () {};
        virtual void subscribe(std::shared_ptr<EventListener<E>> listener) final;
        virtual void unsubscribe(std::shared_ptr<EventListener<E>> listener) final;
        void notify(E ev);

    private:
        std::vector<std::weak_ptr<EventListener<E>>> m_listeners;
        std::mutex m_mtx;
};

template<class E>
void EventProvider<E>::subscribe(std::shared_ptr<EventListener<E>> listener)
{
    std::lock_guard<std::mutex> lock { m_mtx };
    auto l_it = m_listeners.begin();
    while(l_it != m_listeners.end())
    {
        auto l = (*l_it).lock();
        if(l == listener)
        {
            return;
        }
        ++l_it;
    }
    m_listeners.emplace_back(listener);
}

template<class E>
void EventProvider<E>::unsubscribe(std::shared_ptr<EventListener<E>> listener)
{
    std::lock_guard<std::mutex> lock { m_mtx };
    auto l_it = m_listeners.begin();
    while(l_it != m_listeners.end())
    {
        auto l = (*l_it).lock();
        if(l == listener)
        {
            m_listeners.erase(l_it);
            return;
        }
        ++l_it;
    }
}

template<class E>
void EventProvider<E>::notify(E ev)
{
    std::lock_guard<std::mutex> lock { m_mtx };
    auto l_it = m_listeners.begin();
    while(l_it != m_listeners.end())
    {
        if(auto l = (*l_it).lock())
        {
            l->handle_event(ev);
            ++l_it;
        }
        else
        {
            m_listeners.erase(l_it);
        }
    }
}

#endif //EVENT_PROVIDER_H

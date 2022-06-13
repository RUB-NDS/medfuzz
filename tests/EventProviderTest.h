/**
* EventProviderTest.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef EVENT_PROVIDER_TEST_H
#define EVENT_PROVIDER_TEST_H

#include <string>
#include <vector>
#include <algorithm>
#include <memory>
#include "EventProvider.h"
#include "TestEvent.h"
#include "TestEventListener.h"
#include "literals.h"

namespace EventProviderTests {

TEST(EventProviderTest, SubscribeListener)
{
    auto l = std::make_shared<TestEventListener>();
    EventProvider<TestEvent> ep;
    ep.subscribe(l);
    const std::string EVENT_DATA { "Hallo" };
    ep.notify(TestEvent(EVENT_DATA));
    ASSERT_EQ(l->events().size(), static_cast<size_t>(1));
    ASSERT_EQ(l->events().at(0).data(), EVENT_DATA);
}


TEST(EventProviderTest, UnsubscribeListener)
{
    auto l = std::make_shared<TestEventListener>();
    EventProvider<TestEvent> ep;

    // Before we can test unsubscribe we need to subscribe
    // to an event and make sure the subscription worked.
    ep.subscribe(l);
    const std::string EVENT_DATA { "42" };
    ep.notify(TestEvent(EVENT_DATA));
    ASSERT_EQ(l->events().size(), static_cast<size_t>(1));
    ASSERT_EQ(l->events().at(0).data(), EVENT_DATA);

    l->events().clear();
    ep.unsubscribe(l);
    ep.notify(TestEvent(EVENT_DATA));
    ASSERT_EQ(l->events().size(), static_cast<size_t>(0));
}

}
#endif //EVENT_PROVIDER_TEST_H

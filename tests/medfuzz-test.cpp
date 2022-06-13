/**
* medfuzz-test.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include "EventProviderTest.h"
#include "StringToolsTest.h"
#include "NetworkConnectionTest.h"
#include "ServerConnectionTests.h"
#include "literals.h"
#include "Log.h"

int main(int argc, char**argv)
{
    Log::addTarget(Log::ConsoleOut(Log::Verbosity::NORMAL));
    ::testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}

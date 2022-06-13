/**
* Log.cpp
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#include "Log.h"

Log Log::_self = Log();

void Log::info(const std::string& msg, Verbosity v)
{
    writeln({Log::Entry::Type::INFO, msg}, v);
}

void Log::warning(const std::string& msg, Verbosity v)
{
    writeln({Log::Entry::Type::WARNING, msg}, v);
}

void Log::error(const std::string& msg, Verbosity v)
{
    writeln({Log::Entry::Type::ERROR, msg}, v);
}

void Log::write(const Entry& e, Verbosity v)
{
    for(auto target : _self.m_targets)
    {
        if(target->verbosity() >= v)
        {
            target->write(e);
        }
    }
}

void Log::writeln(const Entry& e, Verbosity v)
{
    for(auto target : _self.m_targets)
    {
        if(target->verbosity() >= v)
        {
            target->writeln(e);
        }
    }
}


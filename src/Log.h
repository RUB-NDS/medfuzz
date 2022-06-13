/**
* Log.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef LOG_H
#define LOG_H

#include <cstdint>
#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <type_traits>
#include <unordered_set>

class Log
{
    public:
        enum Verbosity
        {
            NORMAL,
            VERBOSE,
            DEBUG,
            TRACE
        };

        class Entry
        {
            public:
                enum class Type : uint8_t
                {
                    INFO,
                    WARNING,
                    ERROR
                };

                Entry(Type e, const std::string& msg) :
                      m_type(e), m_msg(msg) {}

                const Type& type() const { return m_type; }
                const std::string& message() const { return m_msg; }

            private:
                const Type m_type;
                const std::string m_msg;
        };

        class Target
        {
            public:
                Target(Log::Verbosity verb) : m_verb(verb) {}
                virtual ~Target() {}
                const Verbosity& verbosity() const { return m_verb; }
                Verbosity& verbosity() { return m_verb; }
                virtual void write(const Entry& e) = 0;
                virtual void writeln(const Entry& r) = 0;

            private:
                Log::Verbosity m_verb;
        };

        template <std::ostream& os>
        class StreamTarget : public Log::Target
        {
            public:
                StreamTarget(Log::Verbosity verb = Verbosity::NORMAL)
                    : Log::Target(verb) {}

                StreamTarget(StreamTarget&& s) : StreamTarget(s.verbosity()) {}
                StreamTarget& operator=(const StreamTarget&) = delete;
                virtual ~StreamTarget() { os.flush(); }
                virtual void write(const Entry& e) override;
                virtual void writeln(const Entry& e) override;

            private:
                static std::mutex mtx;
        };

        typedef StreamTarget<std::cout> ConsoleOut;
        typedef StreamTarget<std::cerr> ConsoleError;
        template<typename T>
        using IsLogTarget = typename std::enable_if<std::is_base_of<Log::Target, T>::value, bool>::type;


        template <typename T, IsLogTarget<T> = true>
        static void addTarget(T&& t);
        template <typename T, IsLogTarget<T> = true>
        static void addTarget(std::shared_ptr<T> t);
        template <typename T, IsLogTarget<T> = true>
        static void removeTarget(std::shared_ptr<T> t);
        static void info(const std::string& msg, Verbosity v);
        static void warning(const std::string& msg, Verbosity v);
        static void error(const std::string& msg, Verbosity v);

    private:
        static void write(const Entry& e, Verbosity v);
        static void writeln(const Entry& e, Verbosity v);

        Log() : m_targets() {}
        std::unordered_set<std::shared_ptr<Target>> m_targets;
        static Log _self;
};

inline Log::Verbosity& operator++(Log::Verbosity& v)
{
    uint8_t val = { static_cast<uint8_t>((static_cast<uint8_t>(v) + 1) % 4) };
    v = static_cast<Log::Verbosity>(val);
    return v;
}

inline Log::Entry::Type& operator++(Log::Entry::Type& t)
{
    uint8_t val { static_cast<uint8_t>((static_cast<uint8_t>(t) + 1) % 3) };
    t = static_cast<Log::Entry::Type>(val);
    return t;
}

inline std::ostream& operator<<(std::ostream& os, const Log::Entry::Type& t)
{
    switch(t)
    {
        case Log::Entry::Type::INFO:
            os << "INFO";
            break;
        case Log::Entry::Type::WARNING:
            os << "WARNING";
            break;
        case Log::Entry::Type::ERROR:
            os << "ERROR";
    }
    return os;
}

template <typename T, Log::IsLogTarget<T>>
void Log::addTarget(T&& target)
{
    _self.addTarget(std::make_shared<T>(std::move(target)));
}

template <typename T, Log::IsLogTarget<T>>
void Log::addTarget(std::shared_ptr<T> target)
{
    _self.m_targets.emplace(target);
}

template <typename T, Log::IsLogTarget<T>>
void Log::removeTarget(std::shared_ptr<T> target)
{
    for(auto t = _self.m_targets.cbegin(); t != _self.m_targets.cend();)
    {
        if(target == *t)
        {
            t = _self.m_targets.erase(t);
        }
        else
        {
            ++t;
        }
    }
}


template<std::ostream& os>
std::mutex Log::StreamTarget<os>::mtx{};

template<std::ostream& os>
void Log::StreamTarget<os>::write(const Log::Entry& e)
{
    std::lock_guard<std::mutex> { Log::StreamTarget<os>::mtx };
    os << "[" << e.type() << "] " << e.message();
}

template<std::ostream& os>
void Log::StreamTarget<os>::writeln(const Log::Entry& e)
{
    std::lock_guard<std::mutex> { Log::StreamTarget<os>::mtx };
    write(e);
    os << std::endl;
}

#endif //LOG_H

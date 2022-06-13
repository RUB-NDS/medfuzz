/**
* StringTools.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef STRING_TOOLS_H
#define STRING_TOOLS_H

#include <limits>
#include <algorithm>
#include <sstream>
#include <string>
#include <stdexcept>
#include <type_traits>
/**
 * Various utilities to deal with strings more conveniently.
 **/
class StringTools
{
    public:
        StringTools() = delete;
        StringTools(const StringTools&) = delete;
        StringTools(StringTools&&) = delete;
        StringTools& operator=(const StringTools&) = delete;
        StringTools& operator=(StringTools&&) = delete;

        /**
         * \brief Converts a string representing an unsigned integer to an
         * unsigned integral type, with bounds check.
         *
         * \tparam T an unsigned integral type.
         * \param str A string representing an unsigned integer.
         **/
        template<typename T,
                 typename std::enable_if<std::is_unsigned<T>::value, void*>::type = nullptr>
        static T from_string(const std::string& str);

        /**
         * \brief Converts a string representing a signed integer to an
         * integral type, with bounds check.
         *
         * \tparam T an unsigned integral type.
         * \param str A string representing an unsigned integer.
         **/
        template<typename T,
                 typename std::enable_if<std::is_signed<T>::value, void*>::type = nullptr>
        static T from_string(const std::string& str);

    private:
        template<typename U,
                 typename T,
                 typename std::enable_if<std::is_integral<T>::value, void*>::type = nullptr>
        static T _from_string(U val);

        template<typename T,
                 typename std::enable_if<std::is_integral<T>::value, void*>::type = nullptr>
        static std::string bounds_error_message(T val);

        template<typename T>
        static std::string bounds_error_message(std::string str);

};

template<typename T,
         typename std::enable_if<std::is_signed<T>::value, void*>::type>
T StringTools::from_string(const std::string& str)
{
    try
    {
        return _from_string<long long, T>(std::stoll(str));
    }
    catch (const std::exception& ex)
    {
        throw std::range_error(bounds_error_message<T>(str));
    }
}

template<typename T,
         typename std::enable_if<std::is_unsigned<T>::value, void*>::type>
T StringTools::from_string(const std::string& str)
{
    try
    {
        return _from_string<unsigned long long, T>(std::stoull(str));
    }
    catch (const std::exception& ex)
    {
        throw std::range_error(bounds_error_message<T>(str));
    }
}

template<typename U,
         typename T,
         typename std::enable_if<std::is_integral<T>::value, void*>::type>
T StringTools::_from_string(U val)
{
    try
    {
        if(val < std::numeric_limits<T>::min() ||
           val > std::numeric_limits<T>::max())
        {
            throw std::range_error("");
        }
    }
    catch (const std::exception& ex)
    {
        throw std::range_error("");
    }
    return static_cast<T>(val);
}


template<typename T,
         typename std::enable_if<std::is_integral<T>::value, void*>::type>
std::string StringTools::bounds_error_message(T value)
{
    return bounds_error_message<T>(std::to_string(value));
}

template<typename T>
std::string StringTools::bounds_error_message(std::string value)
{
    std::stringstream ss;
    std::replace(value.begin(), value.end(), '\n', ' ');
    ss << "\"" << value << "\" is not in range of type \"T\"";
    return ss.str();
}

#endif //STRING_TOOLS_H

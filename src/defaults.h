/**
* defaults.h
*
* Copyright:
*   2020, 2021   Matthias Gierlings (matthias.gierlings@rub.de)
*
* This software is released under the Simplified BSD License
* (see license.txt)
**/

#ifndef DEFAULTS_H
#define DEFAULTS_H

#include <cassert>

template<class T>
struct same { typedef T type; };

template<class T*>
static T* get_or_default(T* def, typename same<T*>::type val)
{
    assert(def != nullptr);
    return val == nullptr ? def : val;
}

template<class T>
static T get_or_default(T def, typename same<T>::type val)
{
    T t{};
    return val == t ? def : val;
}

#endif

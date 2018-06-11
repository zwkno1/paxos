#pragma once

#include <system_error>
#include <stdexcept>

class Exception : public std::exception
{
public:
    virtual const char * where() const noexcept = 0;
};

void throwException(const char * where, const char * what);

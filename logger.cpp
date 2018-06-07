#include "logger.h"

#include <iostream>

Logger::Logger(const std::string &prefix)
    : prefix_(prefix)
{
}

Logger::~Logger()
{
    std::cout << prefix_ << stream_.str() << std::endl;
}

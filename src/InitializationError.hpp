#ifndef __INITIALIZATIONERROR_HPP___
#define __INITIALIZATIONERROR_HPP___

#include <stdexcept>
class InitializationError : public std::runtime_error{
    public:
        InitializationError(const std::string& cause) noexcept;
};

#endif
#ifndef __INPUTREADINGERROR_HPP___
#define __INPUTREADINGERROR_HPP___

#include <stdexcept>

class InputReadingError : public std::runtime_error{
    public:
        InputReadingError(const std::string& cause) noexcept;
};

#endif
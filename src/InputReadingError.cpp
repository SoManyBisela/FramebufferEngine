#include "InputReadingError.hpp"

InputReadingError::InputReadingError(const std::string& cause) noexcept
    :runtime_error("Error while reading input: " + cause){}
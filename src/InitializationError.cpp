#include "InitializationError.hpp"

InitializationError::InitializationError(const std::string& cause) noexcept
    :runtime_error("Error during initialization: " + cause){}
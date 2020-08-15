#pragma once

#include <iostream>
#include <stdexcept>
#include <string>

//=====================================================================================================================

namespace wm
{
    template<class ErrorType>
    void throwError(const std::string& msg)
    {
        std::cerr << msg << '\n';
        throw ErrorType(msg);
    }
}

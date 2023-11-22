#include "dida/scalar.hpp"

#include <iostream>

namespace dida
{

std::ostream& operator << (std::ostream& s, ScalarDeg1 scalar)
{
    return s << static_cast<double>(scalar);
}

std::ostream& operator << (std::ostream& s, ScalarDeg2 scalar)
{
    return s << static_cast<double>(scalar);
}

}
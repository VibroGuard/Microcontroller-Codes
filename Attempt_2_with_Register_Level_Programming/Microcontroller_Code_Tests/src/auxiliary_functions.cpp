#include "auxiliary_functions.h"

float map_range(float value, float prevLimitLower, float prevLimitUpper, float nextLimitLower, float nextLimitUpper)
{
    return nextLimitLower + ((value - prevLimitLower) / (prevLimitUpper - prevLimitLower) * (nextLimitUpper - nextLimitLower));
}

const char *to_string(float value)
{
    std::stringstream ss;
    ss << value;
    return ss.str().c_str();
}
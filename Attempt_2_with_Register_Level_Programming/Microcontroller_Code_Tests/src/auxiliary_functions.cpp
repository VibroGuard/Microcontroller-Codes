#include "auxiliary_functions.h"

float map_range(float value, float prevLimitLower, float prevLimitUpper, float nextLimitLower, float nextLimitUpper) {
    return nextLimitLower + ((value - prevLimitLower) / (prevLimitUpper - prevLimitLower) * (nextLimitUpper - nextLimitLower));
}
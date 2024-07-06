#ifndef AUXILIARY_FUNCTIONS_H
#define AUXILIARY_FUNCTIONS_H

#include <avr/interrupt.h>
#include <string>
#include <sstream>
#include <iomanip>

float map_range(float value, float prevLimitLower, float prevLimitUpper, float nextLimitLower, float nextLimitUpper);
const char *to_string(float value);
void setup_millis_counter();
unsigned long millis_elapsed();

#endif
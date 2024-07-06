/*
 * MIT License
 * 
 * Copyright (c) 2024 Linuka Ratnayake
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include "auxiliary_functions.h"

float map_range(float value, float prevLimitLower, float prevLimitUpper, float nextLimitLower, float nextLimitUpper)
{
    return nextLimitLower + ((value - prevLimitLower) / (prevLimitUpper - prevLimitLower) * (nextLimitUpper - nextLimitLower));
}

const char *to_string(float value)
{
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << value; // Formatting to two decimal places
    std::string str = ss.str();                        // Store the result in a std::string
    char *cstr = new char[str.length() + 1];           // +1 for null terminator
    std::copy(str.begin(), str.end(), cstr);           // Copy characters
    cstr[str.length()] = '\0';                         // Null terminator
    return cstr;                                       // Return the C-string
}

volatile unsigned long timer0_millis_ = 0;

void setup_millis_counter()
{
    // Configure Timer0
    // Set Timer0 to CTC (Clear Timer on Compare Match) mode
    TCCR0A = 0;
    TCCR0B = 0;

    // Set compare match register to desired timer count
    OCR0A = 249; // Timer0 counts 250 ticks (prescaler is 64)

    // Enable Timer0 compare match A interrupt
    TIMSK0 |= (1 << OCIE0A);

    // Set prescaler to 64 and start Timer0
    TCCR0B |= (1 << CS01) | (1 << CS00); // prescaler = 64

    // Enable global interrupts
    sei();
}

// Timer0 compare match A interrupt service routine
ISR(TIMER0_COMPA_vect)
{
    timer0_millis_++;
}

unsigned long millis_elapsed()
{
    unsigned long millis;
    // Ensure consistent reading (interrupts should be disabled when reading)
    cli();
    millis = timer0_millis_;
    sei();
    return millis;
}
#ifndef VALIDATION_LIB_H
#define VALIDATION_LIB_H

#include <stdint.h>
#include <stdbool.h>

// Validation function prototypes
bool validate_voltage(float measured, float expected, float tolerance);
bool validate_frequency(uint32_t measured_hz, uint32_t expected_hz, uint32_t tolerance_hz);
bool validate_power(float voltage, float current, float max_power);

// Test logging functions
void log_test_result(const char* test_name, bool passed, float measured, float expected);
void print_test_summary(void);
void reset_test_counters(void);

// Test suite runner
int run_validation_suite(void);

#endif // VALIDATION_LIB_H
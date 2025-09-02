#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "validation_lib.h"

// Test result structure
typedef struct {
    char test_name[64];
    bool passed;
    float measured_value;
    float expected_value;
    float tolerance;
} test_result_t;

// Global test statistics
static int total_tests = 0;
static int passed_tests = 0;

bool validate_voltage(float measured, float expected, float tolerance) {
    float diff = (measured > expected) ? (measured - expected) : (expected - measured);
    return (diff <= tolerance);
}

bool validate_frequency(uint32_t measured_hz, uint32_t expected_hz, uint32_t tolerance_hz) {
    uint32_t diff = (measured_hz > expected_hz) ? 
                    (measured_hz - expected_hz) : (expected_hz - measured_hz);
    return (diff <= tolerance_hz);
}

bool validate_power(float voltage, float current, float max_power) {
    float calculated_power = voltage * current;
    return (calculated_power <= max_power);
}

void log_test_result(const char* test_name, bool passed, float measured, float expected) {
    total_tests++;
    if (passed) {
        passed_tests++;
        printf("[PASS] %s: %.3f (expected: %.3f)\n", test_name, measured, expected);
    } else {
        printf("[FAIL] %s: %.3f (expected: %.3f)\n", test_name, measured, expected);
    }
}

void print_test_summary(void) {
    printf("\n=== Test Summary ===\n");
    printf("Total Tests: %d\n", total_tests);
    printf("Passed: %d\n", passed_tests);
    printf("Failed: %d\n", total_tests - passed_tests);
    printf("Success Rate: %.1f%%\n", 
           total_tests > 0 ? (float)passed_tests / total_tests * 100.0f : 0.0f);
}

void reset_test_counters(void) {
    total_tests = 0;
    passed_tests = 0;
}

// Validation test suite runner
int run_validation_suite(void) {
    printf("FPGA Validation Test Suite\n");
    printf("==========================\n");
    
    reset_test_counters();
    
    // Voltage validation tests
    bool result1 = validate_voltage(3.25f, 3.30f, 0.10f);
    log_test_result("Core Voltage", result1, 3.25f, 3.30f);
    
    bool result2 = validate_voltage(1.85f, 1.80f, 0.05f);
    log_test_result("IO Voltage", result2, 1.85f, 1.80f);
    
    // Frequency validation tests
    bool result3 = validate_frequency(99800000, 100000000, 500000);
    log_test_result("Clock Frequency", result3, 99.8f, 100.0f);
    
    bool result4 = validate_frequency(50200000, 50000000, 100000);
    log_test_result("Bus Frequency", result4, 50.2f, 50.0f);
    
    // Power validation tests
    bool result5 = validate_power(3.3f, 1.2f, 4.0f);
    log_test_result("Power Consumption", result5, 3.96f, 4.0f);
    
    print_test_summary();
    
    return (passed_tests == total_tests) ? 0 : 1;
}

int main() {
    return run_validation_suite();
}
/*
 * Day 1 Exercise 2: Voltage Validator
 *
 * This program validates voltage measurements against specification limits.
 * It demonstrates:
 * - Variable declarations and initialization
 * - User input with scanf
 * - Conditional statements (if/else)
 * - Floating-point arithmetic
 * - Formatted output with printf
 *
 * Compile with: gcc -Wall -Wextra -std=c99 -o voltage_validator voltage_validator.c
 * Run with: ./voltage_validator
 */

#include <stdio.h>

int main(void) {
    // Chip specification limits
    const float MIN_VOLTAGE = 1.8f;    // Minimum operating voltage
    const float NOMINAL_VOLTAGE = 3.3f; // Nominal operating voltage
    const float MAX_VOLTAGE = 3.6f;    // Maximum safe voltage
    const float TOLERANCE = 0.05f;     // 5% tolerance for nominal

    float measured_voltage;
    char test_result = 'F'; // Default to Fail

    // Get user input
    printf("=== Chip Voltage Validation System ===\n");
    printf("Chip Specifications:\n");
    printf("- Operating Range: %.1fV - %.1fV\n", MIN_VOLTAGE, MAX_VOLTAGE);
    printf("- Nominal Voltage: %.1fV ±%.0f%%\n", NOMINAL_VOLTAGE, TOLERANCE * 100);
    printf("\n");

    printf("Enter measured voltage (V): ");
    scanf("%f", &measured_voltage);

    // Display input
    printf("\nMeasured Voltage: %.3fV\n", measured_voltage);

    // Validation logic
    if (measured_voltage < MIN_VOLTAGE) {
        printf("❌ FAIL: Voltage too low! (Minimum: %.1fV)\n", MIN_VOLTAGE);
        printf("   This could cause chip malfunction or instability.\n");
    } else if (measured_voltage > MAX_VOLTAGE) {
        printf("❌ FAIL: Voltage too high! (Maximum: %.1fV)\n", MAX_VOLTAGE);
        printf("   This could damage the chip or reduce lifespan.\n");
    } else {
        // Within range, check if close to nominal
        float deviation = (measured_voltage - NOMINAL_VOLTAGE) / NOMINAL_VOLTAGE;
        if (deviation < 0) {
            deviation = -deviation; // Make positive for comparison
        }

        if (deviation <= TOLERANCE) {
            printf("✅ PASS: Voltage within specification\n");
            printf("   Deviation from nominal: %.1f%%\n", deviation * 100);
            test_result = 'P';
        } else {
            printf("⚠️  MARGINAL: Voltage acceptable but outside tolerance\n");
            printf("   Deviation from nominal: %.1f%% (Tolerance: %.0f%%)\n",
                   deviation * 100, TOLERANCE * 100);
            test_result = 'M'; // Marginal
        }
    }

    // Summary
    printf("\n=== Test Summary ===\n");
    printf("Result: %c\n", test_result);
    printf("Voltage: %.3fV\n", measured_voltage);
    printf("Range: %.1fV - %.1fV\n", MIN_VOLTAGE, MAX_VOLTAGE);

    return 0;
}
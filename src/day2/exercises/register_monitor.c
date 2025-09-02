/*
 * Day 2 Exercise 1: Register Monitor
 *
 * This program demonstrates conditional logic for monitoring hardware register states.
 * It simulates reading MicroBlaze GPIO registers and checking their status.
 *
 * Learning objectives:
 * - if/else conditional statements
 * - Logical operators (&&, ||, !)
 * - Hexadecimal number handling
 * - Register bit manipulation basics
 *
 * Compile with: gcc -Wall -Wextra -std=c99 -g -o register_monitor register_monitor.c
 * Cross-compile: microblaze-elf-gcc -mcpu=v11.0 -mxl-soft-mul -Wall -g -o register_monitor_mb register_monitor.c
 */

#include <stdio.h>
#include <stdint.h>

// MicroBlaze GPIO register addresses (simulated)
#define GPIO_BASE_ADDR    0x40000000
#define GPIO_DATA_OFFSET  0x00
#define GPIO_TRI_OFFSET   0x04
#define GPIO_DATA2_OFFSET 0x08
#define GPIO_TRI2_OFFSET  0x0C

// GPIO register addresses
#define GPIO_DATA  (GPIO_BASE_ADDR + GPIO_DATA_OFFSET)
#define GPIO_TRI   (GPIO_BASE_ADDR + GPIO_TRI_OFFSET)
#define GPIO_DATA2 (GPIO_BASE_ADDR + GPIO_DATA2_OFFSET)
#define GPIO_TRI2  (GPIO_BASE_ADDR + GPIO_TRI2_OFFSET)

// Status flags
#define STATUS_OK       0
#define STATUS_WARNING  1
#define STATUS_ERROR    2

// Function to simulate reading a register (in real hardware, this would be memory-mapped I/O)
uint32_t read_register(uint32_t address) {
    // Simulate different register values based on address
    switch (address) {
        case GPIO_DATA:  return 0x12345678;
        case GPIO_TRI:   return 0x0000FFFF;
        case GPIO_DATA2: return 0xABCDEF00;
        case GPIO_TRI2:  return 0xFFFF0000;
        default:         return 0x00000000;
    }
}

int main(void) {
    printf("=== MicroBlaze GPIO Register Monitor ===\n");
    printf("Simulating hardware register monitoring for validation\n\n");

    // Read all GPIO registers
    uint32_t gpio_data = read_register(GPIO_DATA);
    uint32_t gpio_tri = read_register(GPIO_TRI);
    uint32_t gpio_data2 = read_register(GPIO_DATA2);
    uint32_t gpio_tri2 = read_register(GPIO_TRI2);

    // Display register values
    printf("Register Values:\n");
    printf("GPIO_DATA  (0x%08X): 0x%08X\n", GPIO_DATA, gpio_data);
    printf("GPIO_TRI   (0x%08X): 0x%08X\n", GPIO_TRI, gpio_tri);
    printf("GPIO_DATA2 (0x%08X): 0x%08X\n", GPIO_DATA2, gpio_data2);
    printf("GPIO_TRI2  (0x%08X): 0x%08X\n", GPIO_TRI2, gpio_tri2);
    printf("\n");

    // Monitor individual bits and conditions
    printf("=== Register Analysis ===\n");

    // Check if specific bits are set
    if (gpio_data & 0x00000001) {
        printf("✅ GPIO_DATA bit 0 is HIGH\n");
    } else {
        printf("❌ GPIO_DATA bit 0 is LOW\n");
    }

    // Check multiple conditions
    if ((gpio_data & 0x0000000F) == 0x00000008) {
        printf("✅ GPIO_DATA lower nibble matches expected pattern (0x8)\n");
    } else {
        printf("⚠️  GPIO_DATA lower nibble: 0x%X (expected: 0x8)\n", gpio_data & 0x0000000F);
    }

    // Tristate register analysis
    if (gpio_tri == 0x0000FFFF) {
        printf("✅ GPIO_TRI: Lower 16 bits configured as inputs\n");
    } else if (gpio_tri == 0xFFFF0000) {
        printf("✅ GPIO_TRI: Upper 16 bits configured as inputs\n");
    } else {
        printf("⚠️  GPIO_TRI: Mixed I/O configuration (0x%08X)\n", gpio_tri);
    }

    // Complex validation logic
    int overall_status = STATUS_OK;
    int error_count = 0;
    int warning_count = 0;

    // Validation Rule 1: Data registers should not be all zeros
    if (gpio_data == 0x00000000) {
        printf("❌ ERROR: GPIO_DATA is all zeros - possible hardware failure\n");
        overall_status = STATUS_ERROR;
        error_count++;
    }

    if (gpio_data2 == 0x00000000) {
        printf("❌ ERROR: GPIO_DATA2 is all zeros - possible hardware failure\n");
        overall_status = STATUS_ERROR;
        error_count++;
    }

    // Validation Rule 2: Check for suspicious patterns
    if (gpio_data == 0xFFFFFFFF) {
        printf("⚠️  WARNING: GPIO_DATA is all ones - check connections\n");
        if (overall_status != STATUS_ERROR) {
            overall_status = STATUS_WARNING;
        }
        warning_count++;
    }

    // Validation Rule 3: Tristate consistency check
    if ((gpio_tri & gpio_tri2) != 0) {
        printf("⚠️  WARNING: Overlapping tristate configurations detected\n");
        if (overall_status != STATUS_ERROR) {
            overall_status = STATUS_WARNING;
        }
        warning_count++;
    }

    // Validation Rule 4: Data/Tristate correlation
    uint32_t output_pins = ~gpio_tri;  // Inverted tristate = output pins
    uint32_t driven_high = gpio_data & output_pins;
    uint32_t driven_low = (~gpio_data) & output_pins;

    if (driven_high != 0) {
        printf("ℹ️  INFO: Pins driving HIGH: 0x%08X\n", driven_high);
    }
    if (driven_low != 0) {
        printf("ℹ️  INFO: Pins driving LOW: 0x%08X\n", driven_low);
    }

    // Final status report
    printf("\n=== Validation Summary ===\n");
    printf("Errors: %d\n", error_count);
    printf("Warnings: %d\n", warning_count);

    if (overall_status == STATUS_OK) {
        printf("✅ OVERALL STATUS: PASS - All registers within specification\n");
    } else if (overall_status == STATUS_WARNING) {
        printf("⚠️  OVERALL STATUS: MARGINAL - Warnings detected, review recommended\n");
    } else {
        printf("❌ OVERALL STATUS: FAIL - Critical errors detected\n");
    }

    // Return appropriate exit code
    return overall_status;
}
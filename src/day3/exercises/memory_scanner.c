/*
 * Day 3 Exercise 1: Memory Scanner
 *
 * This program demonstrates pointer arithmetic and memory inspection
 * for hardware validation. It simulates scanning memory regions for
 * hardware registers and data patterns.
 *
 * Learning objectives:
 * - Pointer declaration and initialization
 * - Pointer arithmetic
 * - Memory access patterns
 * - Hexadecimal output formatting
 * - Memory-mapped I/O concepts
 *
 * Compile with: gcc -Wall -Wextra -std=c99 -g -o memory_scanner memory_scanner.c
 * Cross-compile: riscv32-unknown-elf-gcc -march=rv32imafczicsr_zifencei -mabi=ilp32f -Wall -g -o memory_scanner_mbv memory_scanner.c
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>

// Simulated hardware memory regions
#define MEMORY_BASE     0x40000000
#define MEMORY_SIZE     0x1000      // 4KB memory region
#define REGISTER_COUNT  16

// Function to simulate reading from memory (would be hardware access in real system)
uint32_t read_memory(uint32_t address) {
    // Simulate different patterns based on address
    uint32_t offset = address - MEMORY_BASE;

    if (offset < REGISTER_COUNT * 4) {
        // Register area - return register-like values
        return 0x10000000 | (offset / 4);
    } else if (offset < 0x400) {
        // Data area - return data patterns
        return 0xDEADBEEF - offset;
    } else {
        // Configuration area - return config values
        return 0xC0FFEE00 + offset;
    }
}

void print_memory_region(uint32_t start_addr, uint32_t end_addr, const char* region_name) {
    printf("\n=== %s (0x%08X - 0x%08X) ===\n", region_name, start_addr, end_addr);

    for (uint32_t addr = start_addr; addr < end_addr; addr += 16) {
        printf("0x%08X: ", addr);

        // Print 4 words per line (16 bytes)
        for (int i = 0; i < 4 && (addr + i*4) < end_addr; i++) {
            uint32_t value = read_memory(addr + i*4);
            printf("%08X ", value);
        }
        printf("\n");
    }
}

void scan_for_patterns(uint32_t start_addr, uint32_t end_addr, uint32_t pattern) {
    printf("\n=== Scanning for Pattern 0x%08X ===\n", pattern);

    int matches = 0;
    for (uint32_t addr = start_addr; addr < end_addr; addr += 4) {
        uint32_t value = read_memory(addr);
        if (value == pattern) {
            printf("✅ Pattern found at 0x%08X\n", addr);
            matches++;
        }
    }

    if (matches == 0) {
        printf("❌ Pattern not found in scanned region\n");
    } else {
        printf("📊 Total matches: %d\n", matches);
    }
}

void analyze_register_block(uint32_t base_addr, int register_count) {
    printf("\n=== Register Block Analysis (0x%08X) ===\n", base_addr);

    uint32_t* reg_ptr = (uint32_t*)base_addr;  // Simulate pointer to register block

    for (int i = 0; i < register_count; i++) {
        uint32_t reg_value = read_memory(base_addr + i * 4);
        uint32_t reg_addr = base_addr + i * 4;

        printf("Register %2d (0x%08X): 0x%08X", i, reg_addr, reg_value);

        // Analyze register value
        if (reg_value & 0x80000000) {
            printf(" [BIT31=1]");
        }
        if ((reg_value & 0x0000FFFF) == 0x0000FFFF) {
            printf(" [LOWER=ALL]");
        }
        if (reg_value == 0x00000000) {
            printf(" [ZERO]");
        }
        printf("\n");
    }
}

int main(void) {
    printf("=== Hardware Memory Scanner ===\n");
    printf("Simulating memory inspection for post-silicon validation\n");

    // Define memory regions
    uint32_t reg_start = MEMORY_BASE;
    uint32_t reg_end = MEMORY_BASE + REGISTER_COUNT * 4;
    uint32_t data_start = reg_end;
    uint32_t data_end = MEMORY_BASE + 0x400;
    uint32_t config_start = data_end;
    uint32_t config_end = MEMORY_BASE + MEMORY_SIZE;

    // Scan different memory regions
    print_memory_region(reg_start, reg_end, "Register Region");
    print_memory_region(data_start, data_end, "Data Region");
    print_memory_region(config_start, config_end, "Configuration Region");

    // Pattern scanning
    scan_for_patterns(MEMORY_BASE, MEMORY_BASE + MEMORY_SIZE, 0xDEADBEEF);
    scan_for_patterns(MEMORY_BASE, MEMORY_BASE + MEMORY_SIZE, 0x12345678);

    // Register block analysis
    analyze_register_block(MEMORY_BASE, REGISTER_COUNT);

    // Pointer arithmetic demonstration
    printf("\n=== Pointer Arithmetic Demo ===\n");
    uint32_t* memory_ptr = (uint32_t*)MEMORY_BASE;

    printf("Base pointer: %p\n", (void*)memory_ptr);
    printf("First register: 0x%08X\n", read_memory((uint32_t)memory_ptr));
    printf("Second register: 0x%08X\n", read_memory((uint32_t)(memory_ptr + 1)));
    printf("Tenth register: 0x%08X\n", read_memory((uint32_t)(memory_ptr + 9)));

    // Memory range validation
    printf("\n=== Memory Range Validation ===\n");
    uint32_t test_addr = MEMORY_BASE + 0x500;
    if (test_addr >= MEMORY_BASE && test_addr < MEMORY_BASE + MEMORY_SIZE) {
        printf("✅ Address 0x%08X is within valid range\n", test_addr);
        printf("   Value at address: 0x%08X\n", read_memory(test_addr));
    } else {
        printf("❌ Address 0x%08X is outside valid range\n", test_addr);
    }

    printf("\n=== Scan Complete ===\n");
    return 0;
}
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Basic bit manipulation macros
#define SET_BIT(reg, bit)       ((reg) |= (1U << (bit)))
#define CLEAR_BIT(reg, bit)     ((reg) &= ~(1U << (bit)))
#define TOGGLE_BIT(reg, bit)    ((reg) ^= (1U << (bit)))
#define CHECK_BIT(reg, bit)     (((reg) >> (bit)) & 1U)

// Advanced bit manipulation macros
#define SET_BITS(reg, mask)     ((reg) |= (mask))
#define CLEAR_BITS(reg, mask)   ((reg) &= ~(mask))
#define GET_FIELD(reg, mask, shift) (((reg) & (mask)) >> (shift))
#define SET_FIELD(reg, mask, shift, value) \
    ((reg) = ((reg) & ~(mask)) | (((value) << (shift)) & (mask)))

// Include chip structures from previous exercise
typedef struct {
    uint32_t control_register;
    uint32_t status_register;
    uint32_t error_register;
    uint32_t config_register;
} register_set_t;

typedef struct {
    char chip_id[16];
    char part_number[32];
    uint32_t serial_number;
    float temperature;
    float voltage;
    register_set_t registers;
    bool is_initialized;
    bool has_errors;
    uint32_t error_count;
    uint64_t uptime_seconds;
} chip_state_t;

// Bit manipulation demonstration
void demonstrate_bit_operations(void) {
    printf("=== Bit Operations Demonstration ===\n");
    
    uint32_t test_register = 0x00000000;
    printf("Initial register value: 0x%08X\n", test_register);
    
    // Set individual bits
    SET_BIT(test_register, 0);
    SET_BIT(test_register, 4);
    SET_BIT(test_register, 8);
    printf("After setting bits 0,4,8: 0x%08X\n", test_register);
    
    // Clear a bit
    CLEAR_BIT(test_register, 4);
    printf("After clearing bit 4: 0x%08X\n", test_register);
    
    // Toggle bits
    TOGGLE_BIT(test_register, 1);
    TOGGLE_BIT(test_register, 8);
    printf("After toggling bits 1,8: 0x%08X\n", test_register);
    
    // Check bits
    for (int i = 0; i < 16; i++) {
        if (CHECK_BIT(test_register, i)) {
            printf("Bit %d is SET\n", i);
        }
    }
    
    // Field operations
    uint32_t field_reg = 0x12345678;
    printf("\nField operations on 0x%08X:\n", field_reg);
    
    // Extract 4-bit field at position 8
    uint32_t field_value = GET_FIELD(field_reg, 0x00000F00, 8);
    printf("4-bit field at position 8: 0x%X\n", field_value);
    
    // Set 4-bit field at position 12
    SET_FIELD(field_reg, 0x0000F000, 12, 0xA);
    printf("After setting field at position 12 to 0xA: 0x%08X\n", field_reg);
}

uint32_t create_test_pattern(int pattern_type) {
    switch (pattern_type) {
        case 0: // All zeros
            return 0x00000000;
        case 1: // All ones
            return 0xFFFFFFFF;
        case 2: // Alternating bits
            return 0x55555555;
        case 3: // Inverse alternating
            return 0xAAAAAAAA;
        case 4: // Walking ones
            return 0x00000001;
        case 5: // Walking zeros
            return 0xFFFFFFFE;
        case 6: // Checkerboard
            return 0x33333333;
        case 7: // Inverse checkerboard
            return 0xCCCCCCCC;
        default:
            return 0x12345678; // Default pattern
    }
}

int validate_bit_pattern(uint32_t value, uint32_t expected_pattern, uint32_t mask) {
    uint32_t masked_value = value & mask;
    uint32_t masked_expected = expected_pattern & mask;
    
    if (masked_value == masked_expected) {
        printf("Bit pattern validation PASS: 0x%08X matches expected 0x%08X (mask: 0x%08X)\n",
               masked_value, masked_expected, mask);
        return 1;
    } else {
        printf("Bit pattern validation FAIL: 0x%08X != expected 0x%08X (mask: 0x%08X)\n",
               masked_value, masked_expected, mask);
        return 0;
    }
}

void analyze_register_bits(uint32_t register_value, const char* register_name) {
    printf("\n=== Bit Analysis for %s (0x%08X) ===\n", register_name, register_value);
    
    // Count set bits
    int set_bits = 0;
    for (int i = 0; i < 32; i++) {
        if (CHECK_BIT(register_value, i)) {
            set_bits++;
        }
    }
    
    printf("Set bits: %d/32 (%.1f%%)\n", set_bits, (float)set_bits / 32.0f * 100.0f);
    
    // Analyze bit patterns
    printf("Bit pattern analysis:\n");
    
    // Check for all zeros/ones
    if (register_value == 0x00000000) {
        printf("  Pattern: ALL ZEROS\n");
    } else if (register_value == 0xFFFFFFFF) {
        printf("  Pattern: ALL ONES\n");
    } else if (register_value == 0x55555555) {
        printf("  Pattern: ALTERNATING (0101...)\n");
    } else if (register_value == 0xAAAAAAAA) {
        printf("  Pattern: ALTERNATING (1010...)\n");
    } else {
        printf("  Pattern: MIXED\n");
    }
    
    // Check specific bit fields (example: status bits)
    printf("Status bit analysis:\n");
    printf("  Enable bit (0): %s\n", CHECK_BIT(register_value, 0) ? "ENABLED" : "DISABLED");
    printf("  Ready bit (1): %s\n", CHECK_BIT(register_value, 1) ? "READY" : "NOT READY");
    printf("  Error bit (2): %s\n", CHECK_BIT(register_value, 2) ? "ERROR" : "OK");
    printf("  Interrupt bit (3): %s\n", CHECK_BIT(register_value, 3) ? "PENDING" : "CLEAR");
    
    // Extract multi-bit fields
    uint32_t mode_field = GET_FIELD(register_value, 0x000000F0, 4); // Bits 4-7
    uint32_t priority_field = GET_FIELD(register_value, 0x00000700, 8); // Bits 8-10
    
    printf("  Mode field (bits 4-7): %d\n", mode_field);
    printf("  Priority field (bits 8-10): %d\n", priority_field);
}

// Register control functions using bit operations
void enable_chip_power(chip_state_t* chip) {
    if (chip == NULL) return;
    
    printf("Enabling power for chip %s\n", chip->chip_id);
    SET_BIT(chip->registers.control_register, 0); // Power enable bit
    SET_BIT(chip->registers.status_register, 1);  // Ready bit
    
    chip->voltage = 3.3f; // Simulate power on
}

void disable_chip_power(chip_state_t* chip) {
    if (chip == NULL) return;
    
    printf("Disabling power for chip %s\n", chip->chip_id);
    CLEAR_BIT(chip->registers.control_register, 0); // Power enable bit
    CLEAR_BIT(chip->registers.status_register, 1);  // Ready bit
    
    chip->voltage = 0.0f; // Simulate power off
}

bool is_chip_ready(const chip_state_t* chip) {
    if (chip == NULL) return false;
    
    bool power_enabled = CHECK_BIT(chip->registers.control_register, 0);
    bool ready_status = CHECK_BIT(chip->registers.status_register, 1);
    bool no_errors = (chip->registers.error_register == 0);
    
    return power_enabled && ready_status && no_errors;
}

uint32_t get_error_flags(const chip_state_t* chip) {
    if (chip == NULL) return 0xFFFFFFFF;
    
    return chip->registers.error_register;
}

void clear_error_flags(chip_state_t* chip, uint32_t flags_to_clear) {
    if (chip == NULL) return;
    
    printf("Clearing error flags 0x%08X from chip %s\n", flags_to_clear, chip->chip_id);
    
    uint32_t old_errors = chip->registers.error_register;
    CLEAR_BITS(chip->registers.error_register, flags_to_clear);
    
    printf("Error register: 0x%08X -> 0x%08X\n", old_errors, chip->registers.error_register);
    
    // Update chip error status
    if (chip->registers.error_register == 0) {
        chip->has_errors = false;
        printf("All errors cleared for chip %s\n", chip->chip_id);
    }
}

// Comprehensive test
int main() {
    printf("=== Bit Operations and Chip Control Test ===\n");
    
    // Demonstrate basic bit operations
    demonstrate_bit_operations();
    
    // Test pattern generation and validation
    printf("\n=== Pattern Testing ===\n");
    for (int i = 0; i < 8; i++) {
        uint32_t pattern = create_test_pattern(i);
        printf("Pattern %d: 0x%08X\n", i, pattern);
        
        // Validate pattern with different masks
        validate_bit_pattern(pattern, pattern, 0xFFFFFFFF); // Full match
        validate_bit_pattern(pattern, pattern, 0x0000FFFF); // Lower 16 bits
    }
    
    // Test chip control with bit operations
    printf("\n=== Chip Control Test ===\n");
    chip_state_t test_chip;
    memset(&test_chip, 0, sizeof(chip_state_t));
    strcpy(test_chip.chip_id, "TEST_FPGA");
    strcpy(test_chip.part_number, "XC7A35T");
    test_chip.is_initialized = true;
    
    // Test power control
    enable_chip_power(&test_chip);
    printf("Chip ready status: %s\n", is_chip_ready(&test_chip) ? "READY" : "NOT READY");
    
    // Inject some errors
    SET_BIT(test_chip.registers.error_register, 2); // Temperature error
    SET_BIT(test_chip.registers.error_register, 5); // Communication error
    test_chip.has_errors = true;
    
    printf("Error flags: 0x%08X\n", get_error_flags(&test_chip));
    printf("Chip ready after errors: %s\n", is_chip_ready(&test_chip) ? "READY" : "NOT READY");
    
    // Clear errors
    clear_error_flags(&test_chip, 0x00000024); // Clear bits 2 and 5
    printf("Chip ready after clearing errors: %s\n", is_chip_ready(&test_chip) ? "READY" : "NOT READY");
    
    // Analyze register states
    analyze_register_bits(test_chip.registers.control_register, "CONTROL");
    analyze_register_bits(test_chip.registers.status_register, "STATUS");
    analyze_register_bits(test_chip.registers.error_register, "ERROR");
    
    return 0;
}
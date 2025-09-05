#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#define MAX_CHIPS 16

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

typedef struct {
    chip_state_t chips[MAX_CHIPS];
    int active_chip_count;
    int total_error_count;
    float average_temperature;
    char system_status[64];
} system_state_t;

// Structure manipulation functions
void init_chip_state(chip_state_t* chip, const char* id, const char* part_num) {
    if (chip == NULL || id == NULL || part_num == NULL) {
        printf("ERROR: NULL pointer in init_chip_state\n");
        return;
    }
    
    // Initialize all fields
    strncpy(chip->chip_id, id, 15);
    chip->chip_id[15] = '\0';
    strncpy(chip->part_number, part_num, 31);
    chip->part_number[31] = '\0';
    
    chip->serial_number = rand() % 1000000; // Simulate serial number
    chip->temperature = 25.0f; // Room temperature default
    chip->voltage = 3.3f; // Default voltage
    
    // Initialize registers
    chip->registers.control_register = 0x00000001; // Default enabled
    chip->registers.status_register = 0x00000000;  // Clear status
    chip->registers.error_register = 0x00000000;   // No errors
    chip->registers.config_register = 0x12345678;  // Default config
    
    chip->is_initialized = true;
    chip->has_errors = false;
    chip->error_count = 0;
    chip->uptime_seconds = 0;
    
    printf("Initialized chip: %s (%s) SN:%d\n", chip->chip_id, chip->part_number, chip->serial_number);
}

void update_chip_temperature(chip_state_t* chip, float new_temp) {
    if (chip == NULL) {
        printf("ERROR: NULL chip pointer\n");
        return;
    }
    
    if (!chip->is_initialized) {
        printf("ERROR: Chip not initialized\n");
        return;
    }
    
    float old_temp = chip->temperature;
    chip->temperature = new_temp;
    
    printf("Chip %s temperature: %.1fC -> %.1fC\n", chip->chip_id, old_temp, new_temp);
    
    // Check for temperature errors
    if (new_temp > 85.0f || new_temp < -40.0f) {
        chip->has_errors = true;
        chip->error_count++;
        chip->registers.error_register |= 0x00000001; // Temperature error bit
        printf("WARNING: Temperature out of range for chip %s\n", chip->chip_id);
    }
}

void update_chip_registers(chip_state_t* chip, register_set_t* new_regs) {
    if (chip == NULL || new_regs == NULL) {
        printf("ERROR: NULL pointer in update_chip_registers\n");
        return;
    }
    
    if (!chip->is_initialized) {
        printf("ERROR: Chip not initialized\n");
        return;
    }
    
    printf("Updating registers for chip %s:\n", chip->chip_id);
    printf("  CONTROL: 0x%08X -> 0x%08X\n", chip->registers.control_register, new_regs->control_register);
    printf("  STATUS:  0x%08X -> 0x%08X\n", chip->registers.status_register, new_regs->status_register);
    printf("  ERROR:   0x%08X -> 0x%08X\n", chip->registers.error_register, new_regs->error_register);
    printf("  CONFIG:  0x%08X -> 0x%08X\n", chip->registers.config_register, new_regs->config_register);
    
    chip->registers = *new_regs;
    
    // Check for new errors
    if (new_regs->error_register != 0) {
        chip->has_errors = true;
        chip->error_count++;
    }
}

int validate_chip_state(const chip_state_t* chip) {
    if (chip == NULL) {
        printf("ERROR: NULL chip pointer\n");
        return 0;
    }
    
    int validation_score = 0;
    int total_checks = 0;
    
    printf("Validating chip %s:\n", chip->chip_id);
    
    // Check initialization
    total_checks++;
    if (chip->is_initialized) {
        printf("  ✓ Initialization: PASS\n");
        validation_score++;
    } else {
        printf("  ✗ Initialization: FAIL\n");
    }
    
    // Check temperature range
    total_checks++;
    if (chip->temperature >= -40.0f && chip->temperature <= 85.0f) {
        printf("  ✓ Temperature (%.1fC): PASS\n", chip->temperature);
        validation_score++;
    } else {
        printf("  ✗ Temperature (%.1fC): FAIL - Out of range\n", chip->temperature);
    }
    
    // Check voltage range
    total_checks++;
    if (chip->voltage >= 1.8f && chip->voltage <= 3.6f) {
        printf("  ✓ Voltage (%.1fV): PASS\n", chip->voltage);
        validation_score++;
    } else {
        printf("  ✗ Voltage (%.1fV): FAIL - Out of range\n", chip->voltage);
    }
    
    // Check error status
    total_checks++;
    if (!chip->has_errors && chip->registers.error_register == 0) {
        printf("  ✓ Error Status: PASS - No errors\n");
        validation_score++;
    } else {
        printf("  ✗ Error Status: FAIL - %d errors, register: 0x%08X\n", 
               chip->error_count, chip->registers.error_register);
    }
    
    // Check control register
    total_checks++;
    if (chip->registers.control_register & 0x00000001) {
        printf("  ✓ Control Register: PASS - Enabled\n");
        validation_score++;
    } else {
        printf("  ✗ Control Register: FAIL - Disabled\n");
    }
    
    printf("Validation Score: %d/%d (%.1f%%)\n", 
           validation_score, total_checks, 
           (float)validation_score / total_checks * 100.0f);
    
    return validation_score == total_checks;
}

void print_chip_summary(const chip_state_t* chip) {
    if (chip == NULL) {
        printf("ERROR: NULL chip pointer\n");
        return;
    }
    
    printf("\n=== Chip Summary: %s ===\n", chip->chip_id);
    printf("Part Number: %s\n", chip->part_number);
    printf("Serial Number: %d\n", chip->serial_number);
    printf("Temperature: %.1fC\n", chip->temperature);
    printf("Voltage: %.1fV\n", chip->voltage);
    printf("Initialized: %s\n", chip->is_initialized ? "Yes" : "No");
    printf("Has Errors: %s\n", chip->has_errors ? "Yes" : "No");
    printf("Error Count: %d\n", chip->error_count);
    printf("Uptime: %llu seconds\n", chip->uptime_seconds);
    
    printf("Registers:\n");
    printf("  CONTROL: 0x%08X\n", chip->registers.control_register);
    printf("  STATUS:  0x%08X\n", chip->registers.status_register);
    printf("  ERROR:   0x%08X\n", chip->registers.error_register);
    printf("  CONFIG:  0x%08X\n", chip->registers.config_register);
    printf("========================\n");
}

// System state management
void init_system_state(system_state_t* system) {
    if (system == NULL) {
        printf("ERROR: NULL system pointer\n");
        return;
    }
    
    memset(system, 0, sizeof(system_state_t));
    strcpy(system->system_status, "INITIALIZING");
    
    printf("System state initialized\n");
}

void add_chip_to_system(system_state_t* system, const char* id, const char* part_num) {
    if (system == NULL || system->active_chip_count >= MAX_CHIPS) {
        printf("ERROR: Cannot add chip - system full or NULL\n");
        return;
    }
    
    chip_state_t* new_chip = &system->chips[system->active_chip_count];
    init_chip_state(new_chip, id, part_num);
    system->active_chip_count++;
    
    printf("Added chip to system. Total chips: %d\n", system->active_chip_count);
}

void update_system_statistics(system_state_t* system) {
    if (system == NULL) {
        printf("ERROR: NULL system pointer\n");
        return;
    }
    
    float total_temp = 0.0f;
    int total_errors = 0;
    int healthy_chips = 0;
    
    for (int i = 0; i < system->active_chip_count; i++) {
        chip_state_t* chip = &system->chips[i];
        total_temp += chip->temperature;
        total_errors += chip->error_count;
        
        if (!chip->has_errors) {
            healthy_chips++;
        }
    }
    
    if (system->active_chip_count > 0) {
        system->average_temperature = total_temp / system->active_chip_count;
    }
    
    system->total_error_count = total_errors;
    
    // Update system status
    if (total_errors == 0) {
        strcpy(system->system_status, "HEALTHY");
    } else if (healthy_chips > system->active_chip_count / 2) {
        strcpy(system->system_status, "DEGRADED");
    } else {
        strcpy(system->system_status, "CRITICAL");
    }
    
    printf("System statistics updated: %s (%.1fC avg, %d errors)\n",
           system->system_status, system->average_temperature, total_errors);
}

// Test the chip structures
int main() {
    printf("=== Chip State Structures Test ===\n");
    
    // Initialize system
    system_state_t validation_system;
    init_system_state(&validation_system);
    
    // Add test chips
    add_chip_to_system(&validation_system, "FPGA_01", "XC7A35T");
    add_chip_to_system(&validation_system, "FPGA_02", "XC7A50T");
    add_chip_to_system(&validation_system, "FPGA_03", "XC7A100T");
    
    // Update chip states
    update_chip_temperature(&validation_system.chips[0], 45.5f);
    update_chip_temperature(&validation_system.chips[1], 52.3f);
    update_chip_temperature(&validation_system.chips[2], 38.7f);
    
    // Update some registers
    register_set_t new_regs = {
        .control_register = 0x00000003,
        .status_register = 0x00000001,
        .error_register = 0x00000000,
        .config_register = 0x87654321
    };
    
    update_chip_registers(&validation_system.chips[1], &new_regs);
    
    // Inject an error for testing
    update_chip_temperature(&validation_system.chips[2], 95.0f); // Over temperature
    
    // Update system statistics
    update_system_statistics(&validation_system);
    
    // Print summaries
    for (int i = 0; i < validation_system.active_chip_count; i++) {
        print_chip_summary(&validation_system.chips[i]);
        
        printf("Validation result for %s: %s\n", 
               validation_system.chips[i].chip_id,
               validate_chip_state(&validation_system.chips[i]) ? "PASS" : "FAIL");
    }
    
    printf("\n=== System Summary ===\n");
    printf("Active Chips: %d\n", validation_system.active_chip_count);
    printf("System Status: %s\n", validation_system.system_status);
    printf("Average Temperature: %.1fC\n", validation_system.average_temperature);
    printf("Total Errors: %d\n", validation_system.total_error_count);
    
    return 0;
}
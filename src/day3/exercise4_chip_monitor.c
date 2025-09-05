#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define MAX_CHIPS 16

// Include structures from previous exercises
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

// System state management functions
void init_system_state(system_state_t* system) {
    if (system == NULL) return;
    
    memset(system, 0, sizeof(system_state_t));
    system->active_chip_count = 0;
    system->total_error_count = 0;
    system->average_temperature = 0.0f;
    strcpy(system->system_status, "INITIALIZING");
    
    printf("System state initialized\n");
}

int add_chip_to_system(system_state_t* system, const char* chip_id, const char* part_number) {
    if (system == NULL || chip_id == NULL || part_number == NULL) {
        printf("ERROR: NULL pointer in add_chip_to_system\n");
        return -1;
    }
    
    if (system->active_chip_count >= MAX_CHIPS) {
        printf("ERROR: Maximum chip count reached\n");
        return -1;
    }
    
    int chip_index = system->active_chip_count;
    chip_state_t* new_chip = &system->chips[chip_index];
    
    // Initialize the new chip
    strncpy(new_chip->chip_id, chip_id, 15);
    new_chip->chip_id[15] = '\0';
    strncpy(new_chip->part_number, part_number, 31);
    new_chip->part_number[31] = '\0';
    
    new_chip->serial_number = 0x10000000 + chip_index; // Auto-generate serial
    new_chip->temperature = 25.0f;
    new_chip->voltage = 3.3f;
    new_chip->is_initialized = true;
    new_chip->has_errors = false;
    new_chip->error_count = 0;
    new_chip->uptime_seconds = 0;
    
    // Initialize registers
    new_chip->registers.control_register = 0x00000001; // Power on
    new_chip->registers.status_register = 0x00000001;  // Ready
    new_chip->registers.error_register = 0x00000000;   // No errors
    new_chip->registers.config_register = 0x00000000;  // Default config
    
    system->active_chip_count++;
    
    printf("Added chip %s (%s) to system at index %d\n", chip_id, part_number, chip_index);
    return chip_index;
}

void update_system_statistics(system_state_t* system) {
    if (system == NULL) return;
    
    if (system->active_chip_count == 0) {
        system->average_temperature = 0.0f;
        system->total_error_count = 0;
        strcpy(system->system_status, "NO_CHIPS");
        return;
    }
    
    // Calculate average temperature
    float temp_sum = 0.0f;
    int error_sum = 0;
    int ready_chips = 0;
    int error_chips = 0;
    
    for (int i = 0; i < system->active_chip_count; i++) {
        chip_state_t* chip = &system->chips[i];
        temp_sum += chip->temperature;
        error_sum += chip->error_count;
        
        if (chip->registers.status_register & 0x00000001) {
            ready_chips++;
        }
        
        if (chip->has_errors) {
            error_chips++;
        }
    }
    
    system->average_temperature = temp_sum / system->active_chip_count;
    system->total_error_count = error_sum;
    
    // Determine system status
    if (error_chips == 0) {
        strcpy(system->system_status, "ALL_GOOD");
    } else if (error_chips < system->active_chip_count) {
        strcpy(system->system_status, "PARTIAL_ERRORS");
    } else {
        strcpy(system->system_status, "SYSTEM_FAILURE");
    }
    
    printf("System statistics updated:\n");
    printf("  Average temperature: %.1fC\n", system->average_temperature);
    printf("  Total errors: %d\n", system->total_error_count);
    printf("  Ready chips: %d/%d\n", ready_chips, system->active_chip_count);
    printf("  System status: %s\n", system->system_status);
}

void print_system_summary(const system_state_t* system) {
    if (system == NULL) return;
    
    printf("\n=== System Summary ===\n");
    printf("Active chips: %d/%d\n", system->active_chip_count, MAX_CHIPS);
    printf("Average temperature: %.1fC\n", system->average_temperature);
    printf("Total error count: %d\n", system->total_error_count);
    printf("System status: %s\n", system->system_status);
    
    printf("\nChip Details:\n");
    for (int i = 0; i < system->active_chip_count; i++) {
        const chip_state_t* chip = &system->chips[i];
        printf("  [%d] %s (%s) - %.1fC, %s\n", 
               i, chip->chip_id, chip->part_number, chip->temperature,
               chip->has_errors ? "ERRORS" : "OK");
    }
    printf("=====================\n");
}

// Comprehensive chip monitoring system
void run_chip_monitor(system_state_t* system) {
    printf("\n=== Comprehensive Chip Monitor ===\n");
    
    // Monitor each chip
    for (int i = 0; i < system->active_chip_count; i++) {
        chip_state_t* chip = &system->chips[i];
        
        printf("\nMonitoring chip %s:\n", chip->chip_id);
        
        // Check status register
        uint32_t status = chip->registers.status_register;
        analyze_register_bits(status, "STATUS_REGISTER");
        
        // Check for specific status conditions
        if (CHECK_BIT(status, 0)) printf("  ✓ Chip is ready\n");
        if (CHECK_BIT(status, 1)) printf("  ⚠ Chip is busy\n");
        if (CHECK_BIT(status, 2)) printf("  ⚠ Chip in test mode\n");
        if (CHECK_BIT(status, 3)) printf("  ⚠ Chip in debug mode\n");
        
        // Check error register
        uint32_t errors = chip->registers.error_register;
        if (errors != 0) {
            analyze_register_bits(errors, "ERROR_REGISTER");
            
            if (CHECK_BIT(errors, 0)) printf("  ✗ Temperature error\n");
            if (CHECK_BIT(errors, 1)) printf("  ✗ Voltage error\n");
            if (CHECK_BIT(errors, 2)) printf("  ✗ Clock error\n");
            if (CHECK_BIT(errors, 3)) printf("  ✗ Communication error\n");
        }
        
        // Update uptime
        chip->uptime_seconds++;
    }
    
    // Update system-wide statistics
    update_system_statistics(system);
}

// Test the complete monitoring system
int main() {
    printf("Comprehensive Chip Monitor Test\n");
    printf("===============================\n");
    
    // Initialize system
    system_state_t validation_system;
    init_system_state(&validation_system);
    
    // Add test chips
    add_chip_to_system(&validation_system, "FPGA_001", "XC7A35T-2CPG236C");
    add_chip_to_system(&validation_system, "FPGA_002", "5CGXFC7C7F23C8");
    add_chip_to_system(&validation_system, "FPGA_003", "LFE5U-85F-6BG381C");
    
    // Simulate some chip states
    validation_system.chips[0].temperature = 45.5f;
    validation_system.chips[1].temperature = 92.0f; // Over temperature
    validation_system.chips[2].temperature = 38.2f;
    
    // Inject some errors
    SET_BIT(validation_system.chips[1].registers.error_register, 0); // Temperature error
    SET_BIT(validation_system.chips[1].registers.error_register, 1); // Voltage error
    validation_system.chips[1].has_errors = true;
    validation_system.chips[1].error_count = 2;
    
    // Run monitoring cycles
    for (int cycle = 0; cycle < 3; cycle++) {
        printf("\n--- Monitoring Cycle %d ---\n", cycle + 1);
        run_chip_monitor(&validation_system);
        
        // Simulate some changes
        if (cycle == 1) {
            // Clear some errors
            clear_error_flags(&validation_system.chips[1], 0x00000001);
        }
    }
    
    // Final system summary
    print_system_summary(&validation_system);
    
    // Test bit pattern validation
    printf("\n--- Bit Pattern Validation Test ---\n");
    uint32_t test_patterns[] = {0x12345678, 0xAAAA5555, 0xFFFF0000, 0x00000001};
    
    for (int i = 0; i < 4; i++) {
        uint32_t pattern = test_patterns[i];
        analyze_register_bits(pattern, "TEST_PATTERN");
        
        // Validate against expected patterns
        validate_bit_pattern(pattern, pattern, 0xFFFFFFFF);
        validate_bit_pattern(pattern, 0x12345678, 0xFF000000); // Check upper byte
    }
    
    printf("\nComprehensive chip monitor test complete!\n");
    return 0;
}
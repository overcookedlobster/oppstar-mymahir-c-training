#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define REGISTER_BANK_SIZE 256
#define BASE_REGISTER_ADDR 0x40000000

// Simulated register bank (in real hardware, this would be memory-mapped)
static uint32_t register_bank[REGISTER_BANK_SIZE];

// Pointer-based register access functions
uint32_t* get_register_pointer(uint32_t address) {
    if (address < BASE_REGISTER_ADDR || 
        address >= BASE_REGISTER_ADDR + (REGISTER_BANK_SIZE * sizeof(uint32_t))) {
        printf("ERROR: Invalid register address 0x%08X\n", address);
        return NULL;
    }
    
    uint32_t index = (address - BASE_REGISTER_ADDR) / sizeof(uint32_t);
    return &register_bank[index];
}

uint32_t read_register_via_pointer(uint32_t address) {
    uint32_t* reg_ptr = get_register_pointer(address);
    if (reg_ptr == NULL) {
        return 0xDEADBEEF; // Error value
    }
    
    printf("Reading register 0x%08X: 0x%08X\n", address, *reg_ptr);
    return *reg_ptr;
}

void write_register_via_pointer(uint32_t address, uint32_t value) {
    uint32_t* reg_ptr = get_register_pointer(address);
    if (reg_ptr == NULL) {
        return;
    }
    
    printf("Writing register 0x%08X: 0x%08X -> 0x%08X\n", address, *reg_ptr, value);
    *reg_ptr = value;
}

int validate_register_pointer(uint32_t* reg_ptr) {
    if (reg_ptr == NULL) {
        printf("ERROR: NULL pointer\n");
        return 0;
    }
    
    // Check if pointer is within valid register bank range
    if (reg_ptr < register_bank || reg_ptr >= register_bank + REGISTER_BANK_SIZE) {
        printf("ERROR: Pointer out of bounds\n");
        return 0;
    }
    
    return 1; // Valid pointer
}

// Pointer arithmetic functions
void bulk_register_write(uint32_t start_addr, uint32_t* values, int count) {
    printf("Bulk writing %d registers starting at 0x%08X\n", count, start_addr);
    
    for (int i = 0; i < count; i++) {
        uint32_t addr = start_addr + (i * sizeof(uint32_t));
        uint32_t* reg_ptr = get_register_pointer(addr);
        
        if (validate_register_pointer(reg_ptr)) {
            *reg_ptr = values[i];
            printf("  [%d] 0x%08X = 0x%08X\n", i, addr, values[i]);
        }
    }
}

void bulk_register_read(uint32_t start_addr, uint32_t* buffer, int count) {
    printf("Bulk reading %d registers starting at 0x%08X\n", count, start_addr);
    
    for (int i = 0; i < count; i++) {
        uint32_t addr = start_addr + (i * sizeof(uint32_t));
        uint32_t* reg_ptr = get_register_pointer(addr);
        
        if (validate_register_pointer(reg_ptr)) {
            buffer[i] = *reg_ptr;
            printf("  [%d] 0x%08X = 0x%08X\n", i, addr, buffer[i]);
        } else {
            buffer[i] = 0xDEADBEEF;
        }
    }
}

uint32_t* find_register_by_value(uint32_t value, uint32_t start_addr, int range) {
    printf("Searching for value 0x%08X in range 0x%08X to 0x%08X\n", 
           value, start_addr, start_addr + (range * sizeof(uint32_t)));
    
    for (int i = 0; i < range; i++) {
        uint32_t addr = start_addr + (i * sizeof(uint32_t));
        uint32_t* reg_ptr = get_register_pointer(addr);
        
        if (validate_register_pointer(reg_ptr) && *reg_ptr == value) {
            printf("Found value 0x%08X at address 0x%08X\n", value, addr);
            return reg_ptr;
        }
    }
    
    printf("Value 0x%08X not found in specified range\n", value);
    return NULL;
}

// Test the pointer-based register access
int main() {
    printf("=== Pointer-Based Register Access Test ===\n");
    
    // Initialize some test data
    uint32_t test_values[] = {0x12345678, 0xABCDEF00, 0x55AA55AA, 0xDEADBEEF};
    int test_count = sizeof(test_values) / sizeof(test_values[0]);
    
    // Test bulk write
    bulk_register_write(BASE_REGISTER_ADDR, test_values, test_count);
    
    // Test bulk read
    uint32_t read_buffer[test_count];
    bulk_register_read(BASE_REGISTER_ADDR, read_buffer, test_count);
    
    // Verify data integrity
    printf("\nData integrity check:\n");
    for (int i = 0; i < test_count; i++) {
        if (read_buffer[i] == test_values[i]) {
            printf("  [%d] PASS: 0x%08X\n", i, read_buffer[i]);
        } else {
            printf("  [%d] FAIL: Expected 0x%08X, Got 0x%08X\n", 
                   i, test_values[i], read_buffer[i]);
        }
    }
    
    // Test search functionality
    printf("\nSearch functionality test:\n");
    uint32_t* found_ptr = find_register_by_value(0xABCDEF00, BASE_REGISTER_ADDR, 10);
    if (found_ptr != NULL) {
        printf("Search successful: Found at offset %ld\n", 
               found_ptr - register_bank);
    }
    
    // Test error conditions
    printf("\nError condition tests:\n");
    uint32_t invalid_value = read_register_via_pointer(0x50000000); // Invalid address
    printf("Invalid read returned: 0x%08X\n", invalid_value);
    
    write_register_via_pointer(0x50000000, 0x12345678); // Invalid address
    
    return 0;
}
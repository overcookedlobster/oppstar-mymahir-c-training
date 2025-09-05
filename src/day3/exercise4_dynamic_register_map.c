#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

// Dynamic register mapping structures
typedef struct register_entry {
    uint32_t address;
    char name[32];
    char description[64];
    uint32_t default_value;
    uint32_t current_value;
    uint32_t access_mask;  // Which bits can be written
    struct register_entry* next;
} register_entry_t;

typedef struct {
    register_entry_t* head;
    int register_count;
    uint32_t base_address;
    uint32_t address_range;
    char map_name[64];
} register_map_t;

// Dynamic register map management
register_map_t* create_register_map(const char* name, uint32_t base_addr, uint32_t range) {
    register_map_t* map = malloc(sizeof(register_map_t));
    if (map == NULL) {
        printf("ERROR: Failed to allocate register map\n");
        return NULL;
    }
    
    map->head = NULL;
    map->register_count = 0;
    map->base_address = base_addr;
    map->address_range = range;
    strncpy(map->map_name, name, 63);
    map->map_name[63] = '\0';
    
    printf("Created register map: %s (Base: 0x%08X, Range: 0x%08X)\n", 
           name, base_addr, range);
    
    return map;
}

int add_register_to_map(register_map_t* map, uint32_t address, const char* name, 
                       const char* description, uint32_t default_val, uint32_t access_mask) {
    if (map == NULL || name == NULL) {
        printf("ERROR: NULL pointer in add_register_to_map\n");
        return 0;
    }
    
    // Check if address is within range
    if (address < map->base_address || 
        address >= map->base_address + map->address_range) {
        printf("ERROR: Address 0x%08X out of range for map %s\n", address, map->map_name);
        return 0;
    }
    
    // Allocate new register entry
    register_entry_t* new_reg = malloc(sizeof(register_entry_t));
    if (new_reg == NULL) {
        printf("ERROR: Failed to allocate register entry\n");
        return 0;
    }
    
    // Initialize register entry
    new_reg->address = address;
    strncpy(new_reg->name, name, 31);
    new_reg->name[31] = '\0';
    strncpy(new_reg->description, description, 63);
    new_reg->description[63] = '\0';
    new_reg->default_value = default_val;
    new_reg->current_value = default_val;
    new_reg->access_mask = access_mask;
    new_reg->next = NULL;
    
    // Add to linked list (insert at head for simplicity)
    new_reg->next = map->head;
    map->head = new_reg;
    map->register_count++;
    
    printf("Added register: %s @ 0x%08X to map %s\n", name, address, map->map_name);
    return 1;
}

register_entry_t* find_register_by_address(register_map_t* map, uint32_t address) {
    if (map == NULL) return NULL;
    
    register_entry_t* current = map->head;
    while (current != NULL) {
        if (current->address == address) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

register_entry_t* find_register_by_name(register_map_t* map, const char* name) {
    if (map == NULL || name == NULL) return NULL;
    
    register_entry_t* current = map->head;
    while (current != NULL) {
        if (strcmp(current->name, name) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

int write_register(register_map_t* map, uint32_t address, uint32_t value) {
    register_entry_t* reg = find_register_by_address(map, address);
    if (reg == NULL) {
        printf("ERROR: Register at address 0x%08X not found\n", address);
        return 0;
    }
    
    // Apply access mask
    uint32_t masked_value = value & reg->access_mask;
    uint32_t protected_bits = reg->current_value & ~reg->access_mask;
    uint32_t final_value = masked_value | protected_bits;
    
    printf("Writing register %s @ 0x%08X: 0x%08X -> 0x%08X (mask: 0x%08X)\n",
           reg->name, address, reg->current_value, final_value, reg->access_mask);
    
    reg->current_value = final_value;
    return 1;
}

uint32_t read_register(register_map_t* map, uint32_t address) {
    register_entry_t* reg = find_register_by_address(map, address);
    if (reg == NULL) {
        printf("ERROR: Register at address 0x%08X not found\n", address);
        return 0xDEADBEEF;
    }
    
    printf("Reading register %s @ 0x%08X: 0x%08X\n", 
           reg->name, address, reg->current_value);
    
    return reg->current_value;
}

void print_register_map(register_map_t* map) {
    if (map == NULL) {
        printf("ERROR: NULL register map\n");
        return;
    }
    
    printf("\n=== Register Map: %s ===\n", map->map_name);
    printf("Base Address: 0x%08X, Range: 0x%08X, Count: %d\n",
           map->base_address, map->address_range, map->register_count);
    printf("%-12s %-10s %-10s %-10s %s\n", 
           "Name", "Address", "Current", "Default", "Description");
    printf("------------------------------------------------------------------------\n");
    
    register_entry_t* current = map->head;
    while (current != NULL) {
        printf("%-12s 0x%08X 0x%08X 0x%08X %s\n",
               current->name, current->address, current->current_value,
               current->default_value, current->description);
        current = current->next;
    }
    printf("========================================================================\n");
}

void destroy_register_map(register_map_t* map) {
    if (map == NULL) return;
    
    printf("Destroying register map: %s\n", map->map_name);
    
    register_entry_t* current = map->head;
    while (current != NULL) {
        register_entry_t* next = current->next;
        free(current);
        current = next;
    }
    
    free(map);
    printf("Register map destroyed\n");
}

// Test the dynamic register mapping
int main() {
    printf("=== Dynamic Register Map Test ===\n");
    
    // Create register map for FPGA GPIO controller
    register_map_t* gpio_map = create_register_map("FPGA_GPIO", 0x40000000, 0x1000);
    if (gpio_map == NULL) {
        return 1;
    }
    
    // Add GPIO registers
    add_register_to_map(gpio_map, 0x40000000, "GPIO_DATA", 
                       "GPIO data register", 0x00000000, 0xFFFFFFFF);
    add_register_to_map(gpio_map, 0x40000004, "GPIO_DIR", 
                       "GPIO direction register", 0x00000000, 0xFFFFFFFF);
    add_register_to_map(gpio_map, 0x40000008, "GPIO_INT_EN", 
                       "GPIO interrupt enable", 0x00000000, 0xFFFFFFFF);
    add_register_to_map(gpio_map, 0x4000000C, "GPIO_INT_ST", 
                       "GPIO interrupt status", 0x00000000, 0x00000000); // Read-only
    add_register_to_map(gpio_map, 0x40000010, "GPIO_PULL", 
                       "GPIO pull-up/down", 0x00000000, 0xFFFFFFFF);
    
    // Create register map for FPGA Timer
    register_map_t* timer_map = create_register_map("FPGA_TIMER", 0x40001000, 0x1000);
    if (timer_map == NULL) {
        destroy_register_map(gpio_map);
        return 1;
    }
    
    // Add Timer registers
    add_register_to_map(timer_map, 0x40001000, "TIMER_CTRL", 
                       "Timer control register", 0x00000000, 0x000000FF);
    add_register_to_map(timer_map, 0x40001004, "TIMER_COUNT", 
                       "Timer count register", 0x00000000, 0x00000000); // Read-only
    add_register_to_map(timer_map, 0x40001008, "TIMER_COMPARE", 
                       "Timer compare register", 0xFFFFFFFF, 0xFFFFFFFF);
    add_register_to_map(timer_map, 0x4000100C, "TIMER_STATUS", 
                       "Timer status register", 0x00000000, 0x00000001); // Only bit 0 writable
    
    // Print initial register maps
    print_register_map(gpio_map);
    print_register_map(timer_map);
    
    // Test register operations
    printf("\n=== Register Operation Tests ===\n");
    
    // GPIO operations
    write_register(gpio_map, 0x40000004, 0x0000FFFF); // Set GPIO direction
    write_register(gpio_map, 0x40000000, 0x5555AAAA); // Set GPIO data
    write_register(gpio_map, 0x40000008, 0x000000FF); // Enable interrupts
    
    uint32_t gpio_data = read_register(gpio_map, 0x40000000);
    uint32_t gpio_dir = read_register(gpio_map, 0x40000004);
    
    // Timer operations
    write_register(timer_map, 0x40001000, 0x00000001); // Enable timer
    write_register(timer_map, 0x40001008, 0x00100000); // Set compare value
    
    // Try to write to read-only register (should be masked)
    write_register(timer_map, 0x40001004, 0x12345678); // Timer count (read-only)
    
    // Test register search
    printf("\n=== Register Search Tests ===\n");
    register_entry_t* found_reg = find_register_by_name(gpio_map, "GPIO_DATA");
    if (found_reg != NULL) {
        printf("Found register by name: %s @ 0x%08X = 0x%08X\n",
               found_reg->name, found_reg->address, found_reg->current_value);
        
        analyze_register_bits(found_reg->current_value, found_reg->name);
    }
    
    // Print final register states
    printf("\n=== Final Register States ===\n");
    print_register_map(gpio_map);
    print_register_map(timer_map);
    
    // Memory usage report
    printf("\n=== Memory Usage ===\n");
    printf("GPIO map: %d registers, ~%zu bytes\n", 
           gpio_map->register_count, 
           sizeof(register_map_t) + gpio_map->register_count * sizeof(register_entry_t));
    printf("Timer map: %d registers, ~%zu bytes\n", 
           timer_map->register_count,
           sizeof(register_map_t) + timer_map->register_count * sizeof(register_entry_t));
    
    // Cleanup
    destroy_register_map(gpio_map);
    destroy_register_map(timer_map);
    
    printf("\nDynamic register mapping test completed successfully!\n");
    return 0;
}
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

// Hardware register definitions
#define FPGA_BASE           0x40000000
#define GPIO_BASE           (FPGA_BASE + 0x0000)
#define TIMER_BASE          (FPGA_BASE + 0x1000)
#define INTERRUPT_BASE      (FPGA_BASE + 0x2000)
#define DMA_BASE            (FPGA_BASE + 0x3000)

// GPIO registers
#define GPIO_DATA           (GPIO_BASE + 0x00)
#define GPIO_DIRECTION      (GPIO_BASE + 0x04)
#define GPIO_INTERRUPT_EN   (GPIO_BASE + 0x08)
#define GPIO_INTERRUPT_ST   (GPIO_BASE + 0x0C)

// Timer registers
#define TIMER_CONTROL       (TIMER_BASE + 0x00)
#define TIMER_COUNT         (TIMER_BASE + 0x04)
#define TIMER_COMPARE       (TIMER_BASE + 0x08)
#define TIMER_STATUS        (TIMER_BASE + 0x0C)

// Debug state structure
typedef struct {
    uint32_t breakpoint_count;
    uint32_t watchpoint_count;
    uint32_t step_count;
    bool debug_enabled;
    uint32_t last_pc;
    uint32_t last_sp;
} debug_state_t;

static debug_state_t debug_state = {0};

// Platform-specific register access
#ifdef __riscv
    #define REG_READ(addr)      (*(volatile uint32_t*)(addr))
    #define REG_WRITE(addr, val) (*(volatile uint32_t*)(addr) = (val))
#else
    // Simulation for native builds
    static uint32_t sim_memory[0x10000] = {0};
    #define REG_READ(addr)      (sim_memory[((addr) - FPGA_BASE) >> 2])
    #define REG_WRITE(addr, val) (sim_memory[((addr) - FPGA_BASE) >> 2] = (val))
#endif

// Debug helper functions
void debug_print_registers(void) {
    printf("\n=== Hardware Register Dump ===\n");
    
    printf("GPIO Registers:\n");
    printf("  DATA:      0x%08X\n", REG_READ(GPIO_DATA));
    printf("  DIRECTION: 0x%08X\n", REG_READ(GPIO_DIRECTION));
    printf("  INT_EN:    0x%08X\n", REG_READ(GPIO_INTERRUPT_EN));
    printf("  INT_ST:    0x%08X\n", REG_READ(GPIO_INTERRUPT_ST));
    
    printf("Timer Registers:\n");
    printf("  CONTROL:   0x%08X\n", REG_READ(TIMER_CONTROL));
    printf("  COUNT:     0x%08X\n", REG_READ(TIMER_COUNT));
    printf("  COMPARE:   0x%08X\n", REG_READ(TIMER_COMPARE));
    printf("  STATUS:    0x%08X\n", REG_READ(TIMER_STATUS));
}

void debug_memory_dump(uint32_t start_addr, uint32_t word_count) {
    printf("\n=== Memory Dump: 0x%08X ===\n", start_addr);
    
    for (uint32_t i = 0; i < word_count; i++) {
        if (i % 4 == 0) {
            printf("0x%08X: ", start_addr + (i * 4));
        }
        
        uint32_t value = REG_READ(start_addr + (i * 4));
        printf("0x%08X ", value);
        
        if ((i + 1) % 4 == 0) {
            printf("\n");
        }
    }
    
    if (word_count % 4 != 0) {
        printf("\n");
    }
}

// Simulated hardware operations with debug hooks
void gpio_set_pin_debug(uint32_t pin, uint32_t value) {
    printf("DEBUG: Setting GPIO pin %d to %d\n", pin, value);
    
    uint32_t current_data = REG_READ(GPIO_DATA);
    uint32_t new_data;
    
    if (value) {
        new_data = current_data | (1 << pin);
    } else {
        new_data = current_data & ~(1 << pin);
    }
    
    printf("DEBUG: GPIO_DATA: 0x%08X -> 0x%08X\n", current_data, new_data);
    REG_WRITE(GPIO_DATA, new_data);
    
    // Verify the write
    uint32_t readback = REG_READ(GPIO_DATA);
    if (readback != new_data) {
        printf("ERROR: GPIO write verification failed! Expected: 0x%08X, Got: 0x%08X\n", 
               new_data, readback);
    }
}

void timer_start_debug(uint32_t compare_value) {
    printf("DEBUG: Starting timer with compare value %d\n", compare_value);
    
    // Reset timer count
    REG_WRITE(TIMER_COUNT, 0);
    printf("DEBUG: Timer count reset to 0\n");
    
    // Set compare value
    REG_WRITE(TIMER_COMPARE, compare_value);
    printf("DEBUG: Timer compare set to %d\n", compare_value);
    
    // Enable timer
    uint32_t control = 0x00000001; // Enable bit
    REG_WRITE(TIMER_CONTROL, control);
    printf("DEBUG: Timer enabled (CONTROL = 0x%08X)\n", control);
    
    debug_print_registers();
}

uint32_t timer_get_count_debug(void) {
    uint32_t count = REG_READ(TIMER_COUNT);
    printf("DEBUG: Timer count read: %d\n", count);
    return count;
}

// Complex debugging scenario
void complex_hardware_sequence(void) {
    printf("\n=== Complex Hardware Debugging Scenario ===\n");
    
    // Step 1: Initialize GPIO
    printf("\nStep 1: GPIO Initialization\n");
    REG_WRITE(GPIO_DIRECTION, 0x0000000F); // Pins 0-3 as outputs
    debug_print_registers();
    
    // Step 2: Set up timer
    printf("\nStep 2: Timer Setup\n");
    timer_start_debug(1000);
    
    // Step 3: GPIO pattern with timing
    printf("\nStep 3: GPIO Pattern Generation\n");
    for (int i = 0; i < 8; i++) {
        uint32_t pattern = i & 0x0F;
        printf("Setting GPIO pattern: 0x%X\n", pattern);
        
        REG_WRITE(GPIO_DATA, pattern);
        
        // Wait for timer
        uint32_t start_count = timer_get_count_debug();
        uint32_t target_count = start_count + 100;
        
        while (timer_get_count_debug() < target_count) {
            // Busy wait - good place for GDB breakpoint
            // GDB: break here and examine registers
        }
        
        // Verify GPIO state
        uint32_t gpio_readback = REG_READ(GPIO_DATA);
        if (gpio_readback != pattern) {
            printf("ERROR: GPIO pattern mismatch! Expected: 0x%X, Got: 0x%X\n", 
                   pattern, gpio_readback);
        }
    }
    
    // Step 4: Interrupt simulation
    printf("\nStep 4: Interrupt Simulation\n");
    REG_WRITE(GPIO_INTERRUPT_EN, 0x00000001); // Enable interrupt on pin 0
    
    // Simulate interrupt condition
    gpio_set_pin_debug(0, 1);
    
    // Check interrupt status
    uint32_t int_status = REG_READ(GPIO_INTERRUPT_ST);
    printf("Interrupt status: 0x%08X\n", int_status);
    
    if (int_status & 0x00000001) {
        printf("Interrupt detected on GPIO pin 0\n");
        // Clear interrupt
        REG_WRITE(GPIO_INTERRUPT_ST, 0x00000001);
        printf("Interrupt cleared\n");
    }
    
    debug_print_registers();
}

// Memory access patterns for debugging
void memory_access_patterns(void) {
    printf("\n=== Memory Access Pattern Testing ===\n");
    
    // Test different access sizes
    uint32_t test_addr = FPGA_BASE + 0x4000; // Test area
    
    printf("Testing 32-bit access:\n");
    REG_WRITE(test_addr, 0x12345678);
    uint32_t val32 = REG_READ(test_addr);
    printf("  Written: 0x12345678, Read: 0x%08X\n", val32);
    
    printf("Testing sequential access:\n");
    for (int i = 0; i < 4; i++) {
        uint32_t addr = test_addr + (i * 4);
        uint32_t value = 0xDEADBEE0 + i;
        REG_WRITE(addr, value);
        printf("  [%d] Addr: 0x%08X, Value: 0x%08X\n", i, addr, value);
    }
    
    printf("Readback verification:\n");
    for (int i = 0; i < 4; i++) {
        uint32_t addr = test_addr + (i * 4);
        uint32_t expected = 0xDEADBEE0 + i;
        uint32_t actual = REG_READ(addr);
        printf("  [%d] Expected: 0x%08X, Actual: 0x%08X %s\n", 
               i, expected, actual, (expected == actual) ? "PASS" : "FAIL");
    }
    
    debug_memory_dump(test_addr, 8);
}

// Main debugging exercise
int main() {
    printf("Advanced GDB Debugging Exercise\n");
    printf("===============================\n");
    
    debug_state.debug_enabled = true;
    
    // Initialize debug state
    debug_state.breakpoint_count = 0;
    debug_state.watchpoint_count = 0;
    debug_state.step_count = 0;
    
    printf("Debug state initialized\n");
    
    // Run debugging scenarios
    complex_hardware_sequence();
    memory_access_patterns();
    
    printf("\n=== Final Register State ===\n");
    debug_print_registers();
    
    printf("\nDebugging exercise complete!\n");
    printf("Suggested GDB commands to try:\n");
    printf("  (gdb) break complex_hardware_sequence\n");
    printf("  (gdb) watch *(uint32_t*)0x40000000\n");
    printf("  (gdb) x/16wx 0x40000000\n");
    printf("  (gdb) info registers\n");
    printf("  (gdb) step\n");
    printf("  (gdb) continue\n");
    
    return 0;
}

/*
GDB Debugging Session Guide:

1. Compile with debug symbols:
   riscv32-unknown-elf-gcc -g -O0 -o debug_exercise exercise1_advanced_gdb.c

2. Start GDB:
   riscv32-unknown-elf-gdb debug_exercise

3. Set breakpoints:
   (gdb) break main
   (gdb) break complex_hardware_sequence
   (gdb) break gpio_set_pin_debug

4. Set watchpoints:
   (gdb) watch *(uint32_t*)0x40000000  # Watch GPIO_DATA
   (gdb) watch *(uint32_t*)0x40001004  # Watch TIMER_COUNT

5. Run and debug:
   (gdb) run
   (gdb) step
   (gdb) next
   (gdb) continue

6. Examine state:
   (gdb) info registers
   (gdb) x/16wx 0x40000000
   (gdb) print debug_state
   (gdb) backtrace

7. Advanced commands:
   (gdb) display *(uint32_t*)0x40000000
   (gdb) commands 1
   > print "GPIO changed"
   > continue
   > end
*/
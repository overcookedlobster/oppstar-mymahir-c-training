#include <stdio.h>
#include <stdint.h>
#include "fpga_hal.h"
#include "validation_lib.h"

// Platform detection
#ifdef __riscv
    #define PLATFORM_NAME "RISC-V FPGA"
    #define PLATFORM_RISCV 1
#else
    #define PLATFORM_NAME "Native Simulation"
    #define PLATFORM_RISCV 0
#endif

// Cross-platform timing functions
static uint32_t get_system_tick(void) {
#ifdef __riscv
    return hal_timer_get_count();
#else
    // Simulate timer for native builds
    static uint32_t sim_tick = 0;
    return ++sim_tick;
#endif
}

// Performance measurement
typedef struct {
    uint32_t start_tick;
    uint32_t end_tick;
    uint32_t duration;
} perf_counter_t;

void perf_start(perf_counter_t* counter) {
    counter->start_tick = get_system_tick();
}

void perf_end(perf_counter_t* counter) {
    counter->end_tick = get_system_tick();
    counter->duration = counter->end_tick - counter->start_tick;
}

// Cross-platform validation test
int run_cross_platform_test(void) {
    perf_counter_t perf;
    
    printf("=== Cross-Platform Validation Test ===\n");
    printf("Platform: %s\n", PLATFORM_NAME);
    printf("RISC-V Target: %s\n", PLATFORM_RISCV ? "Yes" : "No");
    
    // Initialize hardware abstraction layer
    perf_start(&perf);
    hal_system_init();
    perf_end(&perf);
    printf("HAL Init Time: %d ticks\n", perf.duration);
    
    // Run validation tests
    printf("\n--- Running Validation Suite ---\n");
    perf_start(&perf);
    int validation_result = run_validation_suite();
    perf_end(&perf);
    printf("Validation Time: %d ticks\n", perf.duration);
    
    // Test GPIO functionality
    printf("\n--- Testing GPIO Operations ---\n");
    perf_start(&perf);
    
    for (int i = 0; i < 4; i++) {
        hal_gpio_set_direction(i, GPIO_OUTPUT);
        hal_gpio_write(i, i % 2);
        
        // Small delay between operations
        hal_delay_ms(10);
    }
    
    perf_end(&perf);
    printf("GPIO Test Time: %d ticks\n", perf.duration);
    
    // Test ADC readings (simulated on native)
    printf("\n--- Testing ADC Readings ---\n");
    perf_start(&perf);
    
    for (int channel = 0; channel < 4; channel++) {
        uint16_t adc_value = hal_adc_read_channel(channel);
        
        // Convert ADC reading to voltage (assuming 3.3V reference, 12-bit ADC)
        float voltage = (adc_value * 3.3f) / 4095.0f;
        
        // Validate voltage is within reasonable range
        bool voltage_valid = validate_voltage(voltage, 1.65f, 1.65f); // 0-3.3V range
        log_test_result("ADC Channel", voltage_valid, voltage, 1.65f);
    }
    
    perf_end(&perf);
    printf("ADC Test Time: %d ticks\n", perf.duration);
    
    // Platform-specific tests
    if (PLATFORM_RISCV) {
        printf("\n--- RISC-V Specific Tests ---\n");
        
        // Test UART communication
        hal_uart_send_string("RISC-V FPGA validation complete\n");
        
        // Test timer precision
        uint32_t timer_start = hal_timer_get_count();
        hal_delay_ms(100);
        uint32_t timer_end = hal_timer_get_count();
        uint32_t measured_delay = timer_end - timer_start;
        
        printf("Timer precision test: %d ticks for 100ms delay\n", measured_delay);
        
    } else {
        printf("\n--- Native Simulation Tests ---\n");
        printf("Running in simulation mode - hardware tests skipped\n");
    }
    
    print_test_summary();
    
    return validation_result;
}

// Compiler and build information
void print_build_info(void) {
    printf("\n=== Build Information ===\n");
    
#ifdef __GNUC__
    printf("Compiler: GCC %d.%d.%d\n", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
#endif

#ifdef __riscv
    printf("Target Architecture: RISC-V\n");
    #ifdef __riscv_xlen
    printf("XLEN: %d\n", __riscv_xlen);
    #endif
    #ifdef __riscv_flen
    printf("FLEN: %d\n", __riscv_flen);
    #endif
#else
    printf("Target Architecture: Native\n");
#endif

#ifdef CMAKE_BUILD_TYPE
    printf("Build Type: %s\n", CMAKE_BUILD_TYPE);
#endif

#ifdef CROSS_COMPILING
    printf("Cross Compilation: Enabled\n");
#endif

#ifdef NATIVE_BUILD
    printf("Native Build: Enabled\n");
#endif

    printf("Compilation Date: %s %s\n", __DATE__, __TIME__);
}

int main() {
    print_build_info();
    
    int result = run_cross_platform_test();
    
    printf("\n=== Test Complete ===\n");
    printf("Overall Result: %s\n", (result == 0) ? "PASS" : "FAIL");
    
    return result;
}
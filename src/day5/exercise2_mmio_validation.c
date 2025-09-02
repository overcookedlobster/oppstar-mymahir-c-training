#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// Memory-mapped I/O base addresses
#define MMIO_BASE           0x40000000
#define GPIO_MMIO_BASE      (MMIO_BASE + 0x0000)
#define UART_MMIO_BASE      (MMIO_BASE + 0x1000)
#define SPI_MMIO_BASE       (MMIO_BASE + 0x2000)
#define I2C_MMIO_BASE       (MMIO_BASE + 0x3000)
#define ADC_MMIO_BASE       (MMIO_BASE + 0x4000)
#define PWM_MMIO_BASE       (MMIO_BASE + 0x5000)

// GPIO MMIO registers
typedef struct {
    volatile uint32_t data;         // 0x00
    volatile uint32_t direction;    // 0x04
    volatile uint32_t interrupt_en; // 0x08
    volatile uint32_t interrupt_st; // 0x0C
    volatile uint32_t pull_up;      // 0x10
    volatile uint32_t pull_down;    // 0x14
    volatile uint32_t drive_str;    // 0x18
    volatile uint32_t reserved;     // 0x1C
} gpio_mmio_t;

// UART MMIO registers
typedef struct {
    volatile uint32_t data;         // 0x00
    volatile uint32_t status;       // 0x04
    volatile uint32_t control;      // 0x08
    volatile uint32_t baud_div;     // 0x0C
    volatile uint32_t fifo_ctrl;    // 0x10
    volatile uint32_t interrupt_en; // 0x14
    volatile uint32_t interrupt_st; // 0x18
    volatile uint32_t reserved;     // 0x1C
} uart_mmio_t;

// SPI MMIO registers
typedef struct {
    volatile uint32_t data;         // 0x00
    volatile uint32_t control;      // 0x04
    volatile uint32_t status;       // 0x08
    volatile uint32_t clock_div;    // 0x0C
    volatile uint32_t cs_control;   // 0x10
    volatile uint32_t fifo_level;   // 0x14
    volatile uint32_t reserved[2];  // 0x18-0x1C
} spi_mmio_t;

// ADC MMIO registers
typedef struct {
    volatile uint32_t data[8];      // 0x00-0x1C (8 channels)
    volatile uint32_t control;      // 0x20
    volatile uint32_t status;       // 0x24
    volatile uint32_t sample_rate;  // 0x28
    volatile uint32_t threshold;    // 0x2C
} adc_mmio_t;

// Platform-specific MMIO access
#ifdef __riscv
    // Direct hardware access on RISC-V
    #define GPIO_MMIO   ((gpio_mmio_t*)GPIO_MMIO_BASE)
    #define UART_MMIO   ((uart_mmio_t*)UART_MMIO_BASE)
    #define SPI_MMIO    ((spi_mmio_t*)SPI_MMIO_BASE)
    #define ADC_MMIO    ((adc_mmio_t*)ADC_MMIO_BASE)
#else
    // Simulation structures for native builds
    static gpio_mmio_t sim_gpio = {0};
    static uart_mmio_t sim_uart = {0};
    static spi_mmio_t sim_spi = {0};
    static adc_mmio_t sim_adc = {0};
    
    #define GPIO_MMIO   (&sim_gpio)
    #define UART_MMIO   (&sim_uart)
    #define SPI_MMIO    (&sim_spi)
    #define ADC_MMIO    (&sim_adc)
#endif

// MMIO validation results
typedef struct {
    char peripheral[16];
    uint32_t address;
    uint32_t expected;
    uint32_t actual;
    bool passed;
    char description[64];
} mmio_test_result_t;

static mmio_test_result_t test_results[32];
static int test_count = 0;

// MMIO test helper functions
void mmio_log_test(const char* peripheral, uint32_t addr, uint32_t expected, 
                   uint32_t actual, const char* description) {
    if (test_count >= 32) return;
    
    mmio_test_result_t* result = &test_results[test_count++];
    strncpy(result->peripheral, peripheral, 15);
    result->peripheral[15] = '\0';
    result->address = addr;
    result->expected = expected;
    result->actual = actual;
    result->passed = (expected == actual);
    strncpy(result->description, description, 63);
    result->description[63] = '\0';
    
    printf("[%s] %s @ 0x%08X: Expected 0x%08X, Got 0x%08X - %s\n",
           result->passed ? "PASS" : "FAIL",
           peripheral, addr, expected, actual, description);
}

void mmio_print_summary(void) {
    int passed = 0;
    printf("\n=== MMIO Validation Summary ===\n");
    
    for (int i = 0; i < test_count; i++) {
        if (test_results[i].passed) passed++;
    }
    
    printf("Total Tests: %d\n", test_count);
    printf("Passed: %d\n", passed);
    printf("Failed: %d\n", test_count - passed);
    printf("Success Rate: %.1f%%\n", 
           test_count > 0 ? (float)passed / test_count * 100.0f : 0.0f);
}

// GPIO MMIO validation
void validate_gpio_mmio(void) {
    printf("\n=== GPIO MMIO Validation ===\n");
    
    // Test 1: Data register read/write
    GPIO_MMIO->data = 0x12345678;
    uint32_t gpio_data = GPIO_MMIO->data;
    mmio_log_test("GPIO", (uint32_t)&GPIO_MMIO->data, 0x12345678, gpio_data, "Data register R/W");
    
    // Test 2: Direction register
    GPIO_MMIO->direction = 0x0000FFFF;
    uint32_t gpio_dir = GPIO_MMIO->direction;
    mmio_log_test("GPIO", (uint32_t)&GPIO_MMIO->direction, 0x0000FFFF, gpio_dir, "Direction register");
    
    // Test 3: Interrupt enable
    GPIO_MMIO->interrupt_en = 0x000000FF;
    uint32_t gpio_int_en = GPIO_MMIO->interrupt_en;
    mmio_log_test("GPIO", (uint32_t)&GPIO_MMIO->interrupt_en, 0x000000FF, gpio_int_en, "Interrupt enable");
    
    // Test 4: Pull-up configuration
    GPIO_MMIO->pull_up = 0x55555555;
    uint32_t gpio_pullup = GPIO_MMIO->pull_up;
    mmio_log_test("GPIO", (uint32_t)&GPIO_MMIO->pull_up, 0x55555555, gpio_pullup, "Pull-up config");
    
    // Test 5: Bit manipulation
    GPIO_MMIO->data = 0x00000000;
    GPIO_MMIO->data |= (1 << 5);  // Set bit 5
    uint32_t bit_set = GPIO_MMIO->data;
    mmio_log_test("GPIO", (uint32_t)&GPIO_MMIO->data, 0x00000020, bit_set, "Bit set operation");
    
    GPIO_MMIO->data &= ~(1 << 5); // Clear bit 5
    uint32_t bit_clear = GPIO_MMIO->data;
    mmio_log_test("GPIO", (uint32_t)&GPIO_MMIO->data, 0x00000000, bit_clear, "Bit clear operation");
}

// UART MMIO validation
void validate_uart_mmio(void) {
    printf("\n=== UART MMIO Validation ===\n");
    
    // Test 1: Control register
    UART_MMIO->control = 0x00000007; // Enable TX, RX, and interrupts
    uint32_t uart_ctrl = UART_MMIO->control;
    mmio_log_test("UART", (uint32_t)&UART_MMIO->control, 0x00000007, uart_ctrl, "Control register");
    
    // Test 2: Baud rate divisor
    UART_MMIO->baud_div = 868; // 115200 baud at 100MHz clock
    uint32_t uart_baud = UART_MMIO->baud_div;
    mmio_log_test("UART", (uint32_t)&UART_MMIO->baud_div, 868, uart_baud, "Baud divisor");
    
    // Test 3: FIFO control
    UART_MMIO->fifo_ctrl = 0x000000C0; // Enable FIFOs, clear both
    uint32_t uart_fifo = UART_MMIO->fifo_ctrl;
    mmio_log_test("UART", (uint32_t)&UART_MMIO->fifo_ctrl, 0x000000C0, uart_fifo, "FIFO control");
    
    // Test 4: Data transmission (simulated)
    UART_MMIO->data = 0x41; // ASCII 'A'
    uint32_t uart_data = UART_MMIO->data;
    mmio_log_test("UART", (uint32_t)&UART_MMIO->data, 0x41, uart_data, "Data transmission");
}

// SPI MMIO validation
void validate_spi_mmio(void) {
    printf("\n=== SPI MMIO Validation ===\n");
    
    // Test 1: Control register
    SPI_MMIO->control = 0x00000081; // Enable SPI, Master mode
    uint32_t spi_ctrl = SPI_MMIO->control;
    mmio_log_test("SPI", (uint32_t)&SPI_MMIO->control, 0x00000081, spi_ctrl, "Control register");
    
    // Test 2: Clock divisor
    SPI_MMIO->clock_div = 100; // 1MHz SPI clock from 100MHz system clock
    uint32_t spi_clk = SPI_MMIO->clock_div;
    mmio_log_test("SPI", (uint32_t)&SPI_MMIO->clock_div, 100, spi_clk, "Clock divisor");
    
    // Test 3: Chip select control
    SPI_MMIO->cs_control = 0x00000001; // Select CS0
    uint32_t spi_cs = SPI_MMIO->cs_control;
    mmio_log_test("SPI", (uint32_t)&SPI_MMIO->cs_control, 0x00000001, spi_cs, "Chip select");
    
    // Test 4: Data transfer
    SPI_MMIO->data = 0x5A; // Test pattern
    uint32_t spi_data = SPI_MMIO->data;
    mmio_log_test("SPI", (uint32_t)&SPI_MMIO->data, 0x5A, spi_data, "Data transfer");
}

// ADC MMIO validation
void validate_adc_mmio(void) {
    printf("\n=== ADC MMIO Validation ===\n");
    
    // Test 1: Control register
    ADC_MMIO->control = 0x00000001; // Enable ADC
    uint32_t adc_ctrl = ADC_MMIO->control;
    mmio_log_test("ADC", (uint32_t)&ADC_MMIO->control, 0x00000001, adc_ctrl, "Control register");
    
    // Test 2: Sample rate
    ADC_MMIO->sample_rate = 1000; // 1kHz sampling
    uint32_t adc_rate = ADC_MMIO->sample_rate;
    mmio_log_test("ADC", (uint32_t)&ADC_MMIO->sample_rate, 1000, adc_rate, "Sample rate");
    
    // Test 3: Threshold setting
    ADC_MMIO->threshold = 2048; // Mid-scale threshold
    uint32_t adc_thresh = ADC_MMIO->threshold;
    mmio_log_test("ADC", (uint32_t)&ADC_MMIO->threshold, 2048, adc_thresh, "Threshold setting");
    
    // Test 4: Channel data (simulated readings)
    for (int ch = 0; ch < 4; ch++) {
        uint32_t test_value = 1000 + (ch * 100);
        ADC_MMIO->data[ch] = test_value;
        uint32_t adc_data = ADC_MMIO->data[ch];
        
        char desc[32];
        snprintf(desc, sizeof(desc), "Channel %d data", ch);
        mmio_log_test("ADC", (uint32_t)&ADC_MMIO->data[ch], test_value, adc_data, desc);
    }
}

// Memory barrier and synchronization tests
void validate_memory_barriers(void) {
    printf("\n=== Memory Barrier Validation ===\n");
    
    // Test memory ordering with volatile accesses
    volatile uint32_t *test_addr1 = (volatile uint32_t*)(MMIO_BASE + 0x6000);
    volatile uint32_t *test_addr2 = (volatile uint32_t*)(MMIO_BASE + 0x6004);
    
    // Write sequence that must be ordered
    *test_addr1 = 0xDEADBEEF;
    
#ifdef __riscv
    // RISC-V memory fence
    __asm__ volatile ("fence" ::: "memory");
#else
    // Compiler barrier for simulation
    __asm__ volatile ("" ::: "memory");
#endif
    
    *test_addr2 = 0xCAFEBABE;
    
    // Verify ordering
    uint32_t val1 = *test_addr1;
    uint32_t val2 = *test_addr2;
    
    mmio_log_test("BARRIER", (uint32_t)test_addr1, 0xDEADBEEF, val1, "Ordered write 1");
    mmio_log_test("BARRIER", (uint32_t)test_addr2, 0xCAFEBABE, val2, "Ordered write 2");
}

// Comprehensive MMIO stress test
void mmio_stress_test(void) {
    printf("\n=== MMIO Stress Test ===\n");
    
    const int iterations = 100;
    int errors = 0;
    
    for (int i = 0; i < iterations; i++) {
        // Rapid GPIO toggling
        GPIO_MMIO->data = (i & 1) ? 0xFFFFFFFF : 0x00000000;
        uint32_t gpio_readback = GPIO_MMIO->data;
        uint32_t expected = (i & 1) ? 0xFFFFFFFF : 0x00000000;
        
        if (gpio_readback != expected) {
            errors++;
            if (errors < 5) { // Limit error reporting
                printf("Stress test error at iteration %d: Expected 0x%08X, Got 0x%08X\n",
                       i, expected, gpio_readback);
            }
        }
        
        // Brief delay
        for (volatile int j = 0; j < 100; j++);
    }
    
    printf("Stress test completed: %d iterations, %d errors\n", iterations, errors);
    mmio_log_test("STRESS", 0, 0, errors, "Error count");
}

int main() {
    printf("Memory-Mapped I/O Validation Suite\n");
    printf("==================================\n");
    
    // Initialize test counter
    test_count = 0;
    
    // Run MMIO validation tests
    validate_gpio_mmio();
    validate_uart_mmio();
    validate_spi_mmio();
    validate_adc_mmio();
    validate_memory_barriers();
    mmio_stress_test();
    
    // Print final summary
    mmio_print_summary();
    
    printf("\n=== MMIO Address Map ===\n");
    printf("GPIO:    0x%08X - 0x%08X\n", GPIO_MMIO_BASE, GPIO_MMIO_BASE + 0xFFF);
    printf("UART:    0x%08X - 0x%08X\n", UART_MMIO_BASE, UART_MMIO_BASE + 0xFFF);
    printf("SPI:     0x%08X - 0x%08X\n", SPI_MMIO_BASE, SPI_MMIO_BASE + 0xFFF);
    printf("I2C:     0x%08X - 0x%08X\n", I2C_MMIO_BASE, I2C_MMIO_BASE + 0xFFF);
    printf("ADC:     0x%08X - 0x%08X\n", ADC_MMIO_BASE, ADC_MMIO_BASE + 0xFFF);
    printf("PWM:     0x%08X - 0x%08X\n", PWM_MMIO_BASE, PWM_MMIO_BASE + 0xFFF);
    
    // Calculate pass rate
    int passed = 0;
    for (int i = 0; i < test_count; i++) {
        if (test_results[i].passed) passed++;
    }
    
    return (passed == test_count) ? 0 : 1;
}
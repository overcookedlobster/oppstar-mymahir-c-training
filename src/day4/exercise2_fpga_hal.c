#include <stdio.h>
#include <stdint.h>
#include "fpga_hal.h"

// Hardware register base addresses
#define FPGA_BASE_ADDR    0x40000000
#define GPIO_BASE_OFFSET  0x00000000
#define UART_BASE_OFFSET  0x00001000
#define TIMER_BASE_OFFSET 0x00002000
#define ADC_BASE_OFFSET   0x00003000

// GPIO register offsets
#define GPIO_DATA_REG     0x00
#define GPIO_DIR_REG      0x04
#define GPIO_INT_REG      0x08

// UART register offsets
#define UART_DATA_REG     0x00
#define UART_STATUS_REG   0x04
#define UART_CONTROL_REG  0x08

// Timer register offsets
#define TIMER_COUNT_REG   0x00
#define TIMER_COMPARE_REG 0x04
#define TIMER_CONTROL_REG 0x08

// ADC register offsets
#define ADC_DATA_REG      0x00
#define ADC_CONTROL_REG   0x04
#define ADC_STATUS_REG    0x08

// Platform-specific implementations
#ifdef __riscv
    // RISC-V FPGA implementation
    #define REG_WRITE(addr, val) (*(volatile uint32_t*)(addr) = (val))
    #define REG_READ(addr) (*(volatile uint32_t*)(addr))
#else
    // Native simulation implementation
    static uint32_t sim_registers[1024] = {0};
    #define REG_WRITE(addr, val) (sim_registers[((addr) - FPGA_BASE_ADDR) >> 2] = (val))
    #define REG_READ(addr) (sim_registers[((addr) - FPGA_BASE_ADDR) >> 2])
#endif

// GPIO HAL functions
void hal_gpio_init(void) {
    uint32_t gpio_base = FPGA_BASE_ADDR + GPIO_BASE_OFFSET;
    REG_WRITE(gpio_base + GPIO_DIR_REG, 0x00000000);  // All inputs initially
    REG_WRITE(gpio_base + GPIO_DATA_REG, 0x00000000); // All low initially
    printf("GPIO HAL initialized\n");
}

void hal_gpio_set_direction(uint32_t pin, gpio_direction_t direction) {
    if (pin >= 32) return;
    
    uint32_t gpio_base = FPGA_BASE_ADDR + GPIO_BASE_OFFSET;
    uint32_t dir_reg = REG_READ(gpio_base + GPIO_DIR_REG);
    
    if (direction == GPIO_OUTPUT) {
        dir_reg |= (1 << pin);
    } else {
        dir_reg &= ~(1 << pin);
    }
    
    REG_WRITE(gpio_base + GPIO_DIR_REG, dir_reg);
    printf("GPIO pin %d set as %s\n", pin, (direction == GPIO_OUTPUT) ? "OUTPUT" : "INPUT");
}

void hal_gpio_write(uint32_t pin, uint32_t value) {
    if (pin >= 32) return;
    
    uint32_t gpio_base = FPGA_BASE_ADDR + GPIO_BASE_OFFSET;
    uint32_t data_reg = REG_READ(gpio_base + GPIO_DATA_REG);
    
    if (value) {
        data_reg |= (1 << pin);
    } else {
        data_reg &= ~(1 << pin);
    }
    
    REG_WRITE(gpio_base + GPIO_DATA_REG, data_reg);
    printf("GPIO pin %d set to %s\n", pin, value ? "HIGH" : "LOW");
}

uint32_t hal_gpio_read(uint32_t pin) {
    if (pin >= 32) return 0;
    
    uint32_t gpio_base = FPGA_BASE_ADDR + GPIO_BASE_OFFSET;
    uint32_t data_reg = REG_READ(gpio_base + GPIO_DATA_REG);
    
    return (data_reg >> pin) & 1;
}

// UART HAL functions
void hal_uart_init(uint32_t baudrate) {
    uint32_t uart_base = FPGA_BASE_ADDR + UART_BASE_OFFSET;
    
    // Configure UART (simplified)
    REG_WRITE(uart_base + UART_CONTROL_REG, 0x00000001); // Enable UART
    printf("UART HAL initialized at %d baud\n", baudrate);
}

void hal_uart_send_char(char c) {
    uint32_t uart_base = FPGA_BASE_ADDR + UART_BASE_OFFSET;
    
    // Wait for transmit ready (simplified)
    while (!(REG_READ(uart_base + UART_STATUS_REG) & 0x01));
    
    REG_WRITE(uart_base + UART_DATA_REG, c);
}

void hal_uart_send_string(const char* str) {
    while (*str) {
        hal_uart_send_char(*str++);
    }
}

// Timer HAL functions
void hal_timer_init(void) {
    uint32_t timer_base = FPGA_BASE_ADDR + TIMER_BASE_OFFSET;
    
    REG_WRITE(timer_base + TIMER_COUNT_REG, 0);
    REG_WRITE(timer_base + TIMER_CONTROL_REG, 0x00000001); // Enable timer
    printf("Timer HAL initialized\n");
}

uint32_t hal_timer_get_count(void) {
    uint32_t timer_base = FPGA_BASE_ADDR + TIMER_BASE_OFFSET;
    return REG_READ(timer_base + TIMER_COUNT_REG);
}

void hal_timer_set_compare(uint32_t value) {
    uint32_t timer_base = FPGA_BASE_ADDR + TIMER_BASE_OFFSET;
    REG_WRITE(timer_base + TIMER_COMPARE_REG, value);
}

// ADC HAL functions
void hal_adc_init(void) {
    uint32_t adc_base = FPGA_BASE_ADDR + ADC_BASE_OFFSET;
    
    REG_WRITE(adc_base + ADC_CONTROL_REG, 0x00000001); // Enable ADC
    printf("ADC HAL initialized\n");
}

uint16_t hal_adc_read_channel(uint32_t channel) {
    if (channel >= 8) return 0;
    
    uint32_t adc_base = FPGA_BASE_ADDR + ADC_BASE_OFFSET;
    
    // Start conversion
    REG_WRITE(adc_base + ADC_CONTROL_REG, 0x00000001 | (channel << 4));
    
    // Wait for conversion complete
    while (!(REG_READ(adc_base + ADC_STATUS_REG) & 0x01));
    
    return (uint16_t)REG_READ(adc_base + ADC_DATA_REG);
}

// System HAL functions
void hal_system_init(void) {
    printf("Initializing FPGA HAL...\n");
    hal_gpio_init();
    hal_uart_init(115200);
    hal_timer_init();
    hal_adc_init();
    printf("FPGA HAL initialization complete\n");
}

void hal_delay_ms(uint32_t ms) {
#ifdef __riscv
    // RISC-V implementation using timer
    uint32_t start_count = hal_timer_get_count();
    uint32_t target_count = start_count + (ms * 1000); // Assuming 1MHz timer
    while (hal_timer_get_count() < target_count);
#else
    // Native simulation - simple loop
    volatile uint32_t i;
    for (uint32_t j = 0; j < ms; j++) {
        for (i = 0; i < 1000; i++);
    }
#endif
}

// Test the HAL
int main() {
    hal_system_init();
    
    printf("\nTesting GPIO HAL:\n");
    hal_gpio_set_direction(0, GPIO_OUTPUT);
    hal_gpio_set_direction(1, GPIO_INPUT);
    hal_gpio_write(0, 1);
    uint32_t pin1_state = hal_gpio_read(1);
    printf("Pin 1 state: %d\n", pin1_state);
    
    printf("\nTesting UART HAL:\n");
    hal_uart_send_string("Hello FPGA World!\n");
    
    printf("\nTesting Timer HAL:\n");
    uint32_t timer_val = hal_timer_get_count();
    printf("Timer count: %d\n", timer_val);
    hal_delay_ms(100);
    timer_val = hal_timer_get_count();
    printf("Timer count after delay: %d\n", timer_val);
    
    printf("\nTesting ADC HAL:\n");
    uint16_t adc_val = hal_adc_read_channel(0);
    printf("ADC Channel 0: %d\n", adc_val);
    
    printf("\nHAL test complete!\n");
    return 0;
}
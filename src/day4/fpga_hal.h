#ifndef FPGA_HAL_H
#define FPGA_HAL_H

#include <stdint.h>

// GPIO direction enumeration
typedef enum {
    GPIO_INPUT = 0,
    GPIO_OUTPUT = 1
} gpio_direction_t;

// GPIO HAL functions
void hal_gpio_init(void);
void hal_gpio_set_direction(uint32_t pin, gpio_direction_t direction);
void hal_gpio_write(uint32_t pin, uint32_t value);
uint32_t hal_gpio_read(uint32_t pin);

// UART HAL functions
void hal_uart_init(uint32_t baudrate);
void hal_uart_send_char(char c);
void hal_uart_send_string(const char* str);

// Timer HAL functions
void hal_timer_init(void);
uint32_t hal_timer_get_count(void);
void hal_timer_set_compare(uint32_t value);

// ADC HAL functions
void hal_adc_init(void);
uint16_t hal_adc_read_channel(uint32_t channel);

// System HAL functions
void hal_system_init(void);
void hal_delay_ms(uint32_t ms);

#endif // FPGA_HAL_H
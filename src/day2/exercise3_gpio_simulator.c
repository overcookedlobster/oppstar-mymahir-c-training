#include <stdio.h>
#include <stdint.h>

#define GPIO_BASE 0x40000000
#define GPIO_DATA (GPIO_BASE + 0x00)
#define GPIO_TRI  (GPIO_BASE + 0x04)

// Simulated GPIO registers
static uint32_t gpio_data_reg = 0x00000000;
static uint32_t gpio_tri_reg = 0xFFFFFFFF;

uint32_t gpio_read(uint32_t address) {
    switch (address) {
        case GPIO_DATA:
            return gpio_data_reg;
        case GPIO_TRI:
            return gpio_tri_reg;
        default:
            printf("ERROR: Invalid GPIO address 0x%08X\n", address);
            return 0xDEADBEEF;
    }
}

void gpio_write(uint32_t address, uint32_t value) {
    switch (address) {
        case GPIO_DATA:
            gpio_data_reg = value;
            printf("GPIO_DATA written: 0x%08X\n", value);
            break;
        case GPIO_TRI:
            gpio_tri_reg = value;
            printf("GPIO_TRI written: 0x%08X\n", value);
            break;
        default:
            printf("ERROR: Invalid GPIO address 0x%08X\n", address);
    }
}

void gpio_set_pin(int pin) {
    if (pin >= 0 && pin < 32) {
        gpio_data_reg |= (1 << pin);
        printf("Pin %d set HIGH\n", pin);
    }
}

void gpio_clear_pin(int pin) {
    if (pin >= 0 && pin < 32) {
        gpio_data_reg &= ~(1 << pin);
        printf("Pin %d set LOW\n", pin);
    }
}

void gpio_configure_output(int pin) {
    if (pin >= 0 && pin < 32) {
        gpio_tri_reg &= ~(1 << pin);
        printf("Pin %d configured as OUTPUT\n", pin);
    }
}

void gpio_configure_input(int pin) {
    if (pin >= 0 && pin < 32) {
        gpio_tri_reg |= (1 << pin);
        printf("Pin %d configured as INPUT\n", pin);
    }
}

void display_gpio_status() {
    printf("\nGPIO Status:\n");
    printf("DATA: 0x%08X\n", gpio_data_reg);
    printf("TRI:  0x%08X\n", gpio_tri_reg);
    printf("Active pins: ");
    for (int i = 0; i < 32; i++) {
        if (gpio_data_reg & (1 << i)) {
            printf("%d ", i);
        }
    }
    printf("\n");
}

int main() {
    int choice, pin;
    
    printf("MicroBlaze GPIO Controller Simulator\n");
    printf("====================================\n");
    
    while (1) {
        printf("\nMenu:\n");
        printf("1. Set pin HIGH\n");
        printf("2. Set pin LOW\n");
        printf("3. Configure pin as OUTPUT\n");
        printf("4. Configure pin as INPUT\n");
        printf("5. Display GPIO status\n");
        printf("6. Exit\n");
        printf("Choice: ");
        
        scanf("%d", &choice);
        
        switch (choice) {
            case 1:
                printf("Enter pin number (0-31): ");
                scanf("%d", &pin);
                gpio_set_pin(pin);
                break;
            case 2:
                printf("Enter pin number (0-31): ");
                scanf("%d", &pin);
                gpio_clear_pin(pin);
                break;
            case 3:
                printf("Enter pin number (0-31): ");
                scanf("%d", &pin);
                gpio_configure_output(pin);
                break;
            case 4:
                printf("Enter pin number (0-31): ");
                scanf("%d", &pin);
                gpio_configure_input(pin);
                break;
            case 5:
                display_gpio_status();
                break;
            case 6:
                printf("Exiting GPIO simulator\n");
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }
}
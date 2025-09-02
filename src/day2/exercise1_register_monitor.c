#include <stdio.h>
#include <stdint.h>

#define GPIO_BASE 0x40000000
#define GPIO_DATA (GPIO_BASE + 0x00)
#define GPIO_TRI  (GPIO_BASE + 0x04)

typedef struct {
    uint32_t address;
    uint32_t value;
    char name[32];
} register_t;

int main() {
    register_t registers[] = {
        {GPIO_DATA, 0x00000000, "GPIO_DATA"},
        {GPIO_TRI,  0xFFFFFFFF, "GPIO_TRI"},
        {0x40000008, 0x12345678, "GPIO_DATA2"},
        {0x4000000C, 0x87654321, "GPIO_TRI2"}
    };
    
    int num_registers = sizeof(registers) / sizeof(register_t);
    
    printf("Hardware Register Monitor\n");
    printf("=========================\n");
    
    for (int i = 0; i < num_registers; i++) {
        printf("Checking %s at 0x%08X: ", registers[i].name, registers[i].address);
        
        if (registers[i].value == 0x00000000) {
            printf("CLEAR - All bits low\n");
        } else if (registers[i].value == 0xFFFFFFFF) {
            printf("SET - All bits high\n");
        } else {
            printf("MIXED - Value: 0x%08X\n", registers[i].value);
        }
        
        if (registers[i].value & 0x80000000) {
            printf("  WARNING: MSB is set\n");
        }
    }
    
    return 0;
}
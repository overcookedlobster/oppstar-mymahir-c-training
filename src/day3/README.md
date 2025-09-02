# Day 3: Memory & Data Structures

## Learning Objectives
- Understand memory management and pointers
- Master arrays and strings in C
- Learn struct definitions and usage
- Implement dynamic memory allocation
- Work with memory-mapped I/O for hardware registers

## Topics Covered
- Pointers and memory addresses
- Arrays and array operations
- String handling and manipulation
- Struct definitions and typedef
- Dynamic memory allocation (malloc/free)
- Memory-mapped I/O concepts
- Hardware register simulation

## Exercises

### Exercise 1: Memory Scanner
Pointer arithmetic and memory inspection for hardware validation

### Exercise 2: Register Array Simulator
Arrays for managing multiple hardware registers

### Exercise 3: Configuration Struct
Structs for organizing hardware configuration data

### Exercise 4: Dynamic Register Map
Dynamic memory allocation for flexible register mapping

### Exercise 5: Memory Test Pattern
Advanced memory operations for validation testing

## Build Instructions

```bash
# Native compilation
gcc -Wall -Wextra -std=c99 -g -o program program.c

# MicroBlaze cross-compilation
riscv32-unknown-elf-gcc -march=rv32imafczicsr_zifencei -mabi=ilp32f -Wall -g -o program_mbv program.c

# Test in QEMU
qemu-system-riscv32 -M amd-microblaze-v-generic -kernel program_mbv -nographic -serial mon:stdio
```

## Memory Concepts for Hardware Engineers

### Memory-Mapped I/O
Hardware registers appear as memory locations that can be read/written:
```c
#define GPIO_BASE 0x40000000
volatile uint32_t* gpio_data = (uint32_t*)(GPIO_BASE + 0x00);
*gpio_data = 0xFF;  // Write to GPIO register
uint32_t value = *gpio_data;  // Read from GPIO register
```

### Pointer Arithmetic
Navigate through register arrays and memory blocks:
```c
uint32_t* reg_array = (uint32_t*)0x40000000;
for (int i = 0; i < 16; i++) {
    printf("Register %d: 0x%08X\n", i, *(reg_array + i));
}
```

### Struct Alignment
Hardware registers often require specific alignment:
```c
typedef struct __attribute__((packed)) {
    uint32_t control;
    uint32_t status;
    uint32_t data[8];
} HardwareBlock;
```
# Day 4: Advanced Functions & Cross-Compilation

## Learning Objectives
- Master modular programming with functions and header files
- Understand cross-compilation for RISC-V targets
- Learn advanced CMake configuration for FPGA projects
- Implement hardware abstraction layers
- Work with RISC-V toolchain and debugging

## Topics Covered
- Function design and modular programming
- Header files and library creation
- RISC-V cross-compilation setup
- CMake advanced features for embedded targets
- Hardware abstraction layer patterns
- Cross-platform debugging techniques

## Exercises

### Exercise 1: Modular Validation Library
Create reusable validation functions with proper header organization

### Exercise 2: Hardware Abstraction Layer
Implement HAL for FPGA register access

### Exercise 3: Cross-Compilation Setup
Configure RISC-V toolchain and build system

### Exercise 4: Advanced CMake Configuration
Multi-target builds for native and FPGA platforms

### Exercise 5: Cross-Platform Testing
Test validation code on both native and RISC-V targets

## Build Instructions

```bash
# Native compilation
mkdir build-native && cd build-native
cmake .. -DCMAKE_BUILD_TYPE=Debug
make

# RISC-V cross-compilation
mkdir build-riscv && cd build-riscv
cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/riscv-toolchain.cmake
make

# QEMU testing
qemu-system-riscv32 -M virt -kernel program_riscv -nographic -serial mon:stdio
```

## Hardware Abstraction Concepts

### Register Access Patterns
```c
// Direct register access
#define REG_WRITE(addr, val) (*(volatile uint32_t*)(addr) = (val))
#define REG_READ(addr) (*(volatile uint32_t*)(addr))

// HAL wrapper functions
void hal_gpio_write(uint32_t pin, uint32_t value);
uint32_t hal_gpio_read(uint32_t pin);
```

### Cross-Platform Compatibility
```c
#ifdef __riscv
    // RISC-V specific implementations
#else
    // Native/simulation implementations
#endif
```
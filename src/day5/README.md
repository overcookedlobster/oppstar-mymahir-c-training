# Day 5: Hardware Debugging & FPGA Integration

## Learning Objectives
- Master advanced GDB debugging techniques for embedded systems
- Understand memory-mapped I/O programming patterns
- Implement hardware-in-the-loop testing methodologies
- Learn fault injection and error handling techniques
- Work with JTAG debugging and hardware breakpoints

## Topics Covered
- Advanced GDB commands and scripting
- Memory-mapped I/O debugging techniques
- Hardware breakpoints and watchpoints
- Fault injection and error simulation
- Performance profiling on FPGA targets
- Hardware-in-the-loop testing patterns

## Exercises

### Exercise 1: Advanced GDB Debugging
Complex debugging scenarios with hardware registers

### Exercise 2: Memory-Mapped I/O Validation
Direct hardware register manipulation and validation

### Exercise 3: Fault Injection Framework
Systematic error injection for robustness testing

### Exercise 4: Hardware-in-the-Loop Testing
Automated testing with real hardware feedback

### Exercise 5: Performance Profiling
Timing analysis and optimization for FPGA targets

## Build Instructions

```bash
# Native compilation with debug symbols
gcc -Wall -Wextra -std=c99 -g -O0 -o program program.c

# RISC-V cross-compilation with debug info
riscv32-unknown-elf-gcc -march=rv32imac -mabi=ilp32 -g -O0 -o program_riscv program.c

# Advanced GDB debugging
riscv32-unknown-elf-gdb program_riscv
(gdb) target remote localhost:3333  # Connect to OpenOCD
(gdb) load                          # Load program to target
(gdb) break main
(gdb) continue

# QEMU debugging
qemu-system-riscv32 -M amd-microblaze-v-generic -kernel program_riscv -s -S -nographic
# In another terminal:
riscv32-unknown-elf-gdb program_riscv
(gdb) target remote localhost:1234
```

## Hardware Debugging Concepts

### Memory-Mapped I/O Debugging
```c
// Watch for register changes
(gdb) watch *(uint32_t*)0x40000000
(gdb) info watchpoints

// Examine memory regions
(gdb) x/16wx 0x40000000
(gdb) monitor mdw 0x40000000 16
```

### Hardware Breakpoints
```c
// Set hardware breakpoint
(gdb) hbreak function_name
(gdb) info breakpoints

// Memory access breakpoints
(gdb) awatch variable_name
(gdb) rwatch variable_name
```

### Register Analysis
```c
// RISC-V register examination
(gdb) info registers
(gdb) print $pc
(gdb) print $sp
(gdb) set $t0 = 0x12345678
```
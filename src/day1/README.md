# Day 1: C Fundamentals & Compilation

## Learning Objectives
- Understand C syntax and basic data types
- Learn compilation process with GCC
- Write simple validation calculations
- Set up development environment

## Topics Covered
- Variables and data types (int, float, char)
- Operators and expressions
- Input/output with printf/scanf
- GCC compilation flags and error handling

## Exercises

### Exercise 1: Hello Validation World
Basic program structure and compilation

### Exercise 2: Voltage Validator
Simple validation calculations using variables and operators

### Exercise 3: Power Calculator
More complex calculations with user input

### Exercise 4: Compilation Flags
Understanding GCC compilation options

## Build Instructions

```bash
# Native compilation
gcc -Wall -Wextra -std=c99 -o program program.c

# With debugging symbols
gcc -Wall -Wextra -std=c99 -g -o program program.c

# MicroBlaze cross-compilation (for later use)
microblaze-elf-gcc -mcpu=v11.0 -mxl-soft-mul -o program program.c
```
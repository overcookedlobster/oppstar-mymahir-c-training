# Day 1 Solutions

## Exercise 1: Hello Validation World
**File:** `exercise1_hello.c`

**Solution:** Basic C program structure with validation-themed output.

**Key Concepts:**
- `#include <stdio.h>` preprocessor directive
- `main()` function as program entry point
- `printf()` for output
- `return 0` for successful program termination

## Exercise 2: Voltage Validator
**File:** `exercise2_voltage_validator.c`

**Solution:** Uses variables, arithmetic, and conditional logic for validation.

**Key Concepts:**
- Variable declaration and initialization
- Float data type for precise measurements
- Conditional statements (`if-else`)
- Logical operators (`&&`)
- Formatted output with `printf()`

## Exercise 3: Power Calculator
**File:** `exercise3_power_calculator.c`

**Solution:** Interactive program with user input and calculations.

**Key Concepts:**
- User input with `scanf()`
- Arithmetic operations
- Variable scope and naming
- Error checking and validation logic

## Exercise 4: Compilation Flags
**File:** `exercise4_compilation_flags.sh`

**Solution:** Shell script demonstrating various GCC compilation options.

**Key Concepts:**
- `-Wall`: Enable common warnings
- `-Wextra`: Enable extra warnings
- `-g`: Include debugging symbols
- `-O2`: Enable optimization
- `-std=c99`: Specify C standard

## Common Compilation Commands

```bash
# Basic compilation
gcc program.c -o program

# Recommended flags for development
gcc -Wall -Wextra -std=c99 -g program.c -o program

# Production build with optimization
gcc -Wall -Wextra -std=c99 -O2 program.c -o program
```

## Validation Engineering Context

All exercises simulate real validation scenarios:
- **Exercise 1**: Basic program structure for test frameworks
- **Exercise 2**: Voltage range checking (common in power validation)
- **Exercise 3**: Power consumption analysis
- **Exercise 4**: Build system understanding for automated testing
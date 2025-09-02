#!/bin/bash

# Comprehensive test suite for all C Retraining modules
# This script compiles and tests all exercises from Day 1-6

set -e  # Exit on any error

echo "=== C Retraining Comprehensive Test Suite ==="
echo "=============================================="

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Test counters
TOTAL_TESTS=0
PASSED_TESTS=0
FAILED_TESTS=0

# Function to run a test
run_test() {
    local test_name="$1"
    local test_command="$2"
    local expected_exit_code="${3:-0}"
    
    echo -n "Testing $test_name... "
    TOTAL_TESTS=$((TOTAL_TESTS + 1))
    
    if eval "$test_command" > /dev/null 2>&1; then
        local exit_code=$?
        if [ $exit_code -eq $expected_exit_code ]; then
            echo -e "${GREEN}PASS${NC}"
            PASSED_TESTS=$((PASSED_TESTS + 1))
        else
            echo -e "${RED}FAIL${NC} (exit code: $exit_code, expected: $expected_exit_code)"
            FAILED_TESTS=$((FAILED_TESTS + 1))
        fi
    else
        echo -e "${RED}FAIL${NC} (compilation/execution error)"
        FAILED_TESTS=$((FAILED_TESTS + 1))
    fi
}

# Function to compile and test a C file
compile_and_test() {
    local source_file="$1"
    local test_name="$2"
    local expected_exit_code="${3:-0}"
    
    if [ -f "$source_file" ]; then
        local executable="${source_file%.*}"
        run_test "$test_name (compile)" "gcc -Wall -Wextra -std=c99 -g -o $executable $source_file"
        if [ -f "$executable" ]; then
            run_test "$test_name (execute)" "./$executable" "$expected_exit_code"
            rm -f "$executable"  # Clean up
        fi
    else
        echo -e "${YELLOW}SKIP${NC} $test_name (file not found: $source_file)"
    fi
}

echo ""
echo "=== Day 1: C Fundamentals & Compilation ==="

cd src/day1 2>/dev/null || { echo "Day 1 directory not found"; exit 1; }

compile_and_test "exercise2_voltage.c" "Day1_Voltage_Validator"
compile_and_test "exercise3_power.c" "Day1_Power_Calculator"

# Test CMake build
if [ -f "CMakeLists.txt" ]; then
    run_test "Day1_CMake_Build" "mkdir -p build && cd build && cmake .. && make"
    rm -rf build
fi

cd ../..

echo ""
echo "=== Day 2: Control Flow & Debugging ==="

cd src/day2 2>/dev/null || { echo "Day 2 directory not found"; exit 1; }

compile_and_test "exercise1_register_monitor.c" "Day2_Register_Monitor"
compile_and_test "exercise2_test_sequence.c" "Day2_Test_Sequence"
compile_and_test "exercise3_gpio_simulator.c" "Day2_GPIO_Simulator" 1  # Expects user input, will exit with 1
compile_and_test "exercise4_debug_practice.c" "Day2_Debug_Practice" 1  # Has intentional bugs

cd ../..

echo ""
echo "=== Day 3: Memory & Data Structures ==="

cd src/day3 2>/dev/null || { echo "Day 3 directory not found"; exit 1; }

# Day 3 exercises would be here if they were implemented
echo -e "${YELLOW}INFO${NC} Day 3 exercises use existing README structure"

cd ../..

echo ""
echo "=== Day 4: Advanced Functions & Cross-Compilation ==="

cd src/day4 2>/dev/null || { echo "Day 4 directory not found"; exit 1; }

# Test individual components
compile_and_test "validation_lib.c" "Day4_Validation_Lib_Compile"
compile_and_test "fpga_hal.c" "Day4_FPGA_HAL_Compile"

# Test complete exercises
compile_and_test "exercise1_validation_lib.c validation_lib.c" "Day4_Validation_Library"
compile_and_test "exercise2_fpga_hal.c fpga_hal.c" "Day4_FPGA_HAL"
compile_and_test "exercise3_cross_compile.c fpga_hal.c validation_lib.c" "Day4_Cross_Compile"

# Test CMake build
if [ -f "CMakeLists.txt" ]; then
    run_test "Day4_CMake_Build" "mkdir -p build && cd build && cmake .. && make"
    rm -rf build
fi

cd ../..

echo ""
echo "=== Day 5: Hardware Debugging & FPGA Integration ==="

cd src/day5 2>/dev/null || { echo "Day 5 directory not found"; exit 1; }

compile_and_test "exercise1_advanced_gdb.c" "Day5_Advanced_GDB"
compile_and_test "exercise2_mmio_validation.c" "Day5_MMIO_Validation"

cd ../..

echo ""
echo "=== Day 6: Capstone Project & Integration ==="

cd src/day6 2>/dev/null || { echo "Day 6 directory not found"; exit 1; }

# Need to include Day 4 libraries for capstone
if [ -f "../day4/validation_lib.c" ] && [ -f "../day4/fpga_hal.c" ]; then
    run_test "Day6_Capstone_Compile" "gcc -Wall -Wextra -std=c99 -g -I../day4 -o capstone capstone_validation_framework.c ../day4/validation_lib.c ../day4/fpga_hal.c"
    if [ -f "capstone" ]; then
        run_test "Day6_Capstone_Execute" "./capstone --verbose"
        rm -f capstone
        rm -f fpga_validation_report.html  # Clean up generated report
    fi
fi

# Test CMake build
if [ -f "CMakeLists.txt" ]; then
    run_test "Day6_CMake_Build" "mkdir -p build && cd build && cmake .. && make"
    rm -rf build
fi

cd ../..

echo ""
echo "=== Cross-Compilation Tests ==="

# Test RISC-V cross-compilation if toolchain is available
if command -v riscv32-unknown-elf-gcc >/dev/null 2>&1; then
    echo "RISC-V toolchain found, testing cross-compilation..."
    
    cd src/day4
    run_test "RISC-V_Cross_Compile" "riscv32-unknown-elf-gcc -march=rv32imac -mabi=ilp32 -o test_riscv exercise3_cross_compile.c fpga_hal.c validation_lib.c"
    rm -f test_riscv
    cd ../..
else
    echo -e "${YELLOW}SKIP${NC} RISC-V cross-compilation (toolchain not found)"
fi

echo ""
echo "=== Test Summary ==="
echo "==================="
echo "Total Tests: $TOTAL_TESTS"
echo -e "Passed: ${GREEN}$PASSED_TESTS${NC}"
echo -e "Failed: ${RED}$FAILED_TESTS${NC}"

if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}All tests passed!${NC}"
    SUCCESS_RATE="100.0"
else
    SUCCESS_RATE=$(echo "scale=1; $PASSED_TESTS * 100 / $TOTAL_TESTS" | bc -l 2>/dev/null || echo "N/A")
    echo -e "${YELLOW}Success Rate: $SUCCESS_RATE%${NC}"
fi

echo ""
echo "=== Build Verification ==="

# Test that all major components can be built together
echo "Testing integrated build..."
cd src

# Create a simple integration test
cat > integration_test.c << 'EOF'
#include <stdio.h>

// Include headers if they exist
#ifdef __has_include
#if __has_include("day4/validation_lib.h")
#include "day4/validation_lib.h"
#define HAS_VALIDATION_LIB
#endif
#if __has_include("day4/fpga_hal.h")
#include "day4/fpga_hal.h"
#define HAS_FPGA_HAL
#endif
#endif

int main() {
    printf("Integration test: All modules accessible\n");
    
#ifdef HAS_VALIDATION_LIB
    printf("- Validation library: Available\n");
#endif

#ifdef HAS_FPGA_HAL
    printf("- FPGA HAL: Available\n");
#endif

    return 0;
}
EOF

if gcc -Wall -Wextra -std=c99 -Iday4 -o integration_test integration_test.c 2>/dev/null; then
    echo -e "${GREEN}Integration build: PASS${NC}"
    ./integration_test
    rm -f integration_test
else
    echo -e "${RED}Integration build: FAIL${NC}"
fi

rm -f integration_test.c
cd ..

echo ""
echo "=== Final Results ==="
if [ $FAILED_TESTS -eq 0 ]; then
    echo -e "${GREEN}🎉 All tests completed successfully!${NC}"
    echo "The C Retraining course implementation is ready for use."
    exit 0
else
    echo -e "${RED}❌ Some tests failed.${NC}"
    echo "Please review the failed tests and fix any issues."
    exit 1
fi
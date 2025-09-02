cmake_minimum_required(VERSION 3.10)
project(Day1_Fundamentals)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags for strict checking
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")

# Release flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

# Executables
add_executable(hello exercise1_hello.c)
add_executable(voltage exercise2_voltage.c)
add_executable(power exercise3_power.c)

# Optional: RISC-V cross-compilation support
if(RISCV_TOOLCHAIN)
    set(CMAKE_C_COMPILER riscv64-unknown-elf-gcc)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=rv32i -mabi=ilp32")
endif()
cmake_minimum_required(VERSION 3.10)
project(Day4_AdvancedFunctions)

set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)

# Compiler flags for strict checking
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wpedantic")

# Debug flags
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -g -O0")

# Release flags
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -O2")

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

# Create validation library
add_library(validation_lib STATIC
    validation_lib.c
)

# Create FPGA HAL library
add_library(fpga_hal STATIC
    fpga_hal.c
)

# Exercise 1: Modular Validation Library
add_executable(validation_test exercise1_validation_lib.c)
target_link_libraries(validation_test validation_lib)

# Exercise 2: Hardware Abstraction Layer
add_executable(hal_test exercise2_fpga_hal.c)
target_link_libraries(hal_test fpga_hal)

# Exercise 3: Cross-compilation demo
add_executable(cross_compile_demo exercise3_cross_compile.c)
target_link_libraries(cross_compile_demo fpga_hal validation_lib)

# Exercise 4: Advanced CMake features
if(CMAKE_CROSSCOMPILING)
    message(STATUS "Cross-compiling for RISC-V")
    add_definitions(-DCROSS_COMPILING=1)
    
    # RISC-V specific optimizations
    set_target_properties(validation_test PROPERTIES
        COMPILE_FLAGS "-march=rv32imac -mabi=ilp32"
    )
    set_target_properties(hal_test PROPERTIES
        COMPILE_FLAGS "-march=rv32imac -mabi=ilp32"
    )
    set_target_properties(cross_compile_demo PROPERTIES
        COMPILE_FLAGS "-march=rv32imac -mabi=ilp32"
    )
else()
    message(STATUS "Native compilation")
    add_definitions(-DNATIVE_BUILD=1)
endif()

# Testing support
enable_testing()

add_test(NAME validation_lib_test COMMAND validation_test)
add_test(NAME hal_test COMMAND hal_test)
add_test(NAME cross_compile_test COMMAND cross_compile_demo)

# Custom targets for different build configurations
add_custom_target(build_native
    COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/native
    COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/native
    COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/native
    COMMENT "Building for native platform"
)

if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/cmake/riscv-toolchain.cmake)
    add_custom_target(build_riscv
        COMMAND ${CMAKE_COMMAND} -E make_directory ${CMAKE_BINARY_DIR}/riscv
        COMMAND ${CMAKE_COMMAND} -S ${CMAKE_SOURCE_DIR} -B ${CMAKE_BINARY_DIR}/riscv 
                -DCMAKE_TOOLCHAIN_FILE=${CMAKE_CURRENT_SOURCE_DIR}/cmake/riscv-toolchain.cmake
        COMMAND ${CMAKE_COMMAND} --build ${CMAKE_BINARY_DIR}/riscv
        COMMENT "Cross-compiling for RISC-V"
    )
endif()

# Install targets
install(TARGETS validation_test hal_test cross_compile_demo
    RUNTIME DESTINATION bin
)

install(FILES validation_lib.h fpga_hal.h
    DESTINATION include
)
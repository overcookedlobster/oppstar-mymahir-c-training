cmake_minimum_required(VERSION 3.10)
project(Day6_CapstoneProject)

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
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../day4)

# Link Day 4 libraries
link_directories(${CMAKE_CURRENT_SOURCE_DIR}/../day4)

# Capstone validation framework
add_executable(validation_framework capstone_validation_framework.c)

# Link with Day 4 libraries if they exist
if(EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/../day4/libvalidation_lib.a)
    target_link_libraries(validation_framework validation_lib fpga_hal)
endif()

# Testing support
enable_testing()

add_test(NAME capstone_framework_test COMMAND validation_framework --verbose)

# Custom targets for different execution modes
add_custom_target(run_validation
    COMMAND ./validation_framework --verbose --report=detailed_report.html
    DEPENDS validation_framework
    COMMENT "Running comprehensive validation framework"
)

add_custom_target(run_quick_test
    COMMAND ./validation_framework
    DEPENDS validation_framework
    COMMENT "Running quick validation test"
)

# Documentation generation (if Doxygen is available)
find_package(Doxygen)
if(DOXYGEN_FOUND)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in 
                   ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
    add_custom_target(docs
        ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
        COMMENT "Generating API documentation with Doxygen"
        VERBATIM
    )
endif()

# Install targets
install(TARGETS validation_framework
    RUNTIME DESTINATION bin
)

# Package configuration
set(CPACK_PACKAGE_NAME "FPGA_Validation_Framework")
set(CPACK_PACKAGE_VERSION "1.0.0")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Comprehensive FPGA validation framework")
set(CPACK_PACKAGE_VENDOR "C Retraining Course")

include(CPack)
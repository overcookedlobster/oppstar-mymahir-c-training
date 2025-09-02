# RISC-V Toolchain Configuration for CMake
set(CMAKE_SYSTEM_NAME Generic)
set(CMAKE_SYSTEM_PROCESSOR riscv32)

# Specify the cross compiler
set(CMAKE_C_COMPILER riscv32-unknown-elf-gcc)
set(CMAKE_CXX_COMPILER riscv32-unknown-elf-g++)
set(CMAKE_ASM_COMPILER riscv32-unknown-elf-gcc)

# Specify the target environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)

# RISC-V specific compiler flags
set(CMAKE_C_FLAGS_INIT "-march=rv32imac -mabi=ilp32 -mcmodel=medlow")
set(CMAKE_CXX_FLAGS_INIT "-march=rv32imac -mabi=ilp32 -mcmodel=medlow")

# Linker flags for RISC-V
set(CMAKE_EXE_LINKER_FLAGS_INIT "-nostartfiles -T${CMAKE_CURRENT_LIST_DIR}/riscv.ld")

# Debug and optimization flags
set(CMAKE_C_FLAGS_DEBUG "-g -O0")
set(CMAKE_C_FLAGS_RELEASE "-O2 -DNDEBUG")

# Additional RISC-V specific definitions
add_definitions(-D__riscv)
add_definitions(-DRISCV_TARGET)
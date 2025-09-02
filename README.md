# C Retraining - MicroBlaze-V Platform Migration

This repository contains a comprehensive 6-day C programming course designed for post-silicon validation engineers. The course targets AMD MicroBlaze™ V processors (RISC-V based) running on FPGA platforms with QEMU emulation support. and it should teach people c from scratch. can read the syllabus

## Platform Overview
- **Target Platform**: AMD MicroBlaze™ V (RISC-V ISA) on FPGA
- **QEMU Target**: `qemu-system-riscv32 -M virt`
- **Toolchain**: `riscv32-unknown-elf-gcc` (RISC-V RV32I + extensions m,a,f,c)
- **Architecture**: RV32I base with Zicsr, Zifencei, and extensions m,a,f,c
- **Documentation**: LaTeX-based slides and manuals

## Migration Status: RP2040 → MicroBlaze-V

### Phase 1: Documentation Updates
- [x] Update main README.md QEMU target to MicroBlaze-V
- [ ] Replace 100+ RP2040 references in slides with MicroBlaze-V
- [ ] Update course manual for MicroBlaze-V SDK setup
- [ ] Update troubleshooting guide for MicroBlaze-V debugging
- [ ] Update all slides with MicroBlaze-V toolchain examples

### Phase 2: Source Code Implementation
- [x] Day 1: Basic C exercises (Complete - platform agnostic)
- [ ] Day 2: Control flow with MicroBlaze-V GPIO examples
- [ ] Day 3: Memory operations with MicroBlaze-V memory mapping
- [ ] Day 4: Cross-compilation for MicroBlaze-V platform
- [ ] Day 5: Peripheral validation on MicroBlaze-V
- [ ] Day 6: Complete MicroBlaze-V system integration

### Phase 3: Tooling & Testing
- [ ] MicroBlaze-V toolchain setup scripts
- [ ] QEMU MicroBlaze-V configuration files
- [ ] Automated testing framework for exercises
- [ ] CI/CD pipeline for validation
- [ ] Documentation compilation testing

### Phase 4: Validation & Deployment
- [ ] All exercises tested on QEMU MicroBlaze-V
- [ ] Hardware validation on FPGA boards
- [ ] Performance benchmarking
- [ ] Final documentation review

## Quick Start

### Prerequisites
```bash
# Install RISC-V toolchain for MicroBlaze-V (Ubuntu)
sudo apt update
sudo apt install -y gcc-riscv64-unknown-elf qemu-system-misc

# Verify installation
riscv32-unknown-elf-gcc --version
qemu-system-riscv32 --version
```

### Build and Test
```bash
# Compile for native testing
gcc -Wall -Wextra -std=c99 -o program program.c

# Cross-compile for MicroBlaze-V (RISC-V)
riscv32-unknown-elf-gcc -march=rv32imafczicsr_zifencei -mabi=ilp32f -Wall -g program.c -o program_mbv

# Test in QEMU
qemu-system-riscv32 -M virt -kernel program_mbv -nographic -serial mon:stdio
```

### Documentation
```bash
# Compile LaTeX documentation
cd docs/
pdflatex syllabus.tex
pdflatex slides/day1_slides.tex
```

## Course Structure
```
c/
├── docs/           # LaTeX documentation
│   ├── syllabus.tex
│   ├── slides/     # Daily presentation slides
│   ├── manual/     # Course manual
│   └── guides/     # Setup and troubleshooting
├── src/            # Source code exercises
│   ├── day1/       # C fundamentals
│   ├── day2/       # Control flow & debugging
│   ├── day3/       # Memory & data structures
│   ├── day4/       # Cross-compilation
│   ├── day5/       # Hardware debugging
│   └── day6/       # Capstone project
├── solutions/      # Exercise solutions
└── tools/          # Setup and testing scripts
```

## Contributing
1. Update relevant checklist items in this README
2. Test all changes with both native and MicroBlaze-V compilation
3. Verify LaTeX documentation compiles without errors
4. Make descriptive git commits for tracking progress

## References
- [QEMU RISC-V Documentation](https://www.qemu.org/docs/master/system/riscv.html)
- [MicroBlaze-V User Guide](https://docs.amd.com/r/en-US/ug1629-microblaze-v-user-guide/MicroBlaze-V-Architecture)
- [RISC-V ISA Specification](https://riscv.org/technical/specifications/)
- [Course Syllabus](docs/syllabus.pdf)


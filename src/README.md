# C Retraining Source Code

This directory contains the source code for the 6-day C programming course for post-silicon validation engineers.

## Directory Structure

```
src/
├── day1/           # C Fundamentals & Compilation
├── day2/           # Control Flow & Debugging  
├── day3/           # Memory & Data Structures
├── day4/           # Advanced Functions & Cross-Compilation
├── day5/           # Hardware Debugging & FPGA Integration
├── day6/           # Capstone Project & Integration
├── common/         # Shared libraries and utilities
├── tests/          # Test suites for all modules
└── tools/          # Build scripts and utilities
```

## Build System

Each day's exercises use CMake for cross-platform builds:
- Native compilation for desktop testing
- RISC-V cross-compilation for FPGA targets
- QEMU emulation support for development

## Hardware Platform

The course targets RISC-V MicroBlaze-V processors running on FPGA development boards. Code is designed to be portable across different FPGA vendors (Xilinx, Intel, etc.).

## Getting Started

1. Install the RISC-V toolchain
2. Set up your FPGA development environment
3. Clone this repository
4. Follow the day-by-day progression

Each day builds upon the previous day's concepts and code.
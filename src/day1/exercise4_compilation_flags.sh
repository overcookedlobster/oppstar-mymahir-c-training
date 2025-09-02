#!/bin/bash

echo "=== Day 1 Exercise 4: Compilation Flags Demo ==="

# Create a simple C program with potential issues
cat > demo_program.c << 'EOF'
#include <stdio.h>

int main() {
    int unused_var = 42;
    int x = 10;
    int y = 0;
    
    printf("Result: %d\n", x/y);  // Division by zero
    return 0;
}
EOF

echo "1. Basic compilation (no warnings):"
gcc demo_program.c -o demo_basic
echo "   gcc demo_program.c -o demo_basic"

echo -e "\n2. With warning flags:"
gcc -Wall -Wextra demo_program.c -o demo_warnings 2>&1 || true
echo "   gcc -Wall -Wextra demo_program.c -o demo_warnings"

echo -e "\n3. With debugging symbols:"
gcc -Wall -Wextra -g demo_program.c -o demo_debug
echo "   gcc -Wall -Wextra -g demo_program.c -o demo_debug"

echo -e "\n4. With optimization:"
gcc -Wall -Wextra -O2 demo_program.c -o demo_optimized
echo "   gcc -Wall -Wextra -O2 demo_program.c -o demo_optimized"

echo -e "\n5. Strict C99 standard:"
gcc -Wall -Wextra -std=c99 -pedantic demo_program.c -o demo_strict
echo "   gcc -Wall -Wextra -std=c99 -pedantic demo_program.c -o demo_strict"

echo -e "\nGenerated executables:"
ls -la demo_*

# Clean up
rm -f demo_program.c demo_basic demo_warnings demo_debug demo_optimized demo_strict

echo -e "\nRecommended compilation flags for this course:"
echo "gcc -Wall -Wextra -std=c99 -g -o program program.c"
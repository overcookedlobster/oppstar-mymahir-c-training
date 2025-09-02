#include <stdio.h>

int buggy_function(int a, int b) {
    int result = a + b;
    // Bug 1: Uninitialized variable
    int multiplier;
    result = result * multiplier;  // Should be: int multiplier = 2;
    
    // Bug 2: Off-by-one error in loop
    for (int i = 0; i <= 10; i++) {  // Should be: i < 10
        result += i;
    }
    
    return result;
}

int calculate_average(int *array, int size) {
    int sum = 0;
    
    // Bug 3: Potential division by zero
    for (int i = 0; i < size; i++) {
        sum += array[i];
    }
    
    return sum / size;  // Should check if size != 0
}

int main() {
    printf("Debug Practice Session\n");
    printf("======================\n");
    
    // Test buggy_function
    printf("Testing buggy_function(5, 3):\n");
    int result1 = buggy_function(5, 3);
    printf("Result: %d\n", result1);
    
    // Test calculate_average with normal array
    int test_array[] = {10, 20, 30, 40, 50};
    int size = sizeof(test_array) / sizeof(test_array[0]);
    
    printf("\nTesting calculate_average with array [10,20,30,40,50]:\n");
    int avg = calculate_average(test_array, size);
    printf("Average: %d\n", avg);
    
    // Test calculate_average with empty array (will cause division by zero)
    printf("\nTesting calculate_average with empty array:\n");
    int empty_avg = calculate_average(test_array, 0);
    printf("Average: %d\n", empty_avg);
    
    return 0;
}

/*
GDB Debugging Exercise:

1. Compile with debug symbols:
   gcc -Wall -Wextra -std=c99 -g -o debug_practice exercise4_debug_practice.c

2. Run with GDB:
   gdb ./debug_practice

3. Set breakpoints and investigate:
   (gdb) break buggy_function
   (gdb) break calculate_average
   (gdb) run
   
4. When stopped at breakpoint:
   (gdb) print a
   (gdb) print b
   (gdb) print multiplier
   (gdb) step
   (gdb) continue

5. Find and fix the bugs:
   - Uninitialized variable 'multiplier'
   - Off-by-one error in loop condition
   - Division by zero in calculate_average

Expected fixes:
- Initialize multiplier = 2
- Change loop condition to i < 10
- Add check: if (size == 0) return 0;
*/
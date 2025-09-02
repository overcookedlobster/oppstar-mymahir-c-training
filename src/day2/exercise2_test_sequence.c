#include <stdio.h>
#include <stdint.h>

void generate_test_pattern(uint32_t *pattern, int size, int pattern_type) {
    switch (pattern_type) {
        case 0: // Walking ones
            for (int i = 0; i < size; i++) {
                pattern[i] = 1 << (i % 32);
            }
            break;
        case 1: // Walking zeros
            for (int i = 0; i < size; i++) {
                pattern[i] = ~(1 << (i % 32));
            }
            break;
        case 2: // Alternating pattern
            for (int i = 0; i < size; i++) {
                pattern[i] = (i % 2) ? 0xAAAAAAAA : 0x55555555;
            }
            break;
        default:
            for (int i = 0; i < size; i++) {
                pattern[i] = 0x00000000;
            }
    }
}

int validate_pattern(uint32_t expected, uint32_t actual) {
    return (expected == actual) ? 1 : 0;
}

int main() {
    const int PATTERN_SIZE = 8;
    uint32_t test_patterns[PATTERN_SIZE];
    uint32_t simulated_readback[PATTERN_SIZE];
    
    printf("Test Sequence Generator for FPGA Validation\n");
    printf("==========================================\n");
    
    for (int pattern_type = 0; pattern_type < 3; pattern_type++) {
        printf("\nPattern Type %d: ", pattern_type);
        switch (pattern_type) {
            case 0: printf("Walking Ones\n"); break;
            case 1: printf("Walking Zeros\n"); break;
            case 2: printf("Alternating Pattern\n"); break;
        }
        
        generate_test_pattern(test_patterns, PATTERN_SIZE, pattern_type);
        
        // Simulate readback (with occasional errors)
        for (int i = 0; i < PATTERN_SIZE; i++) {
            simulated_readback[i] = test_patterns[i];
            // Inject error in pattern 1, position 3
            if (pattern_type == 1 && i == 3) {
                simulated_readback[i] ^= 0x00000001; // Flip LSB
            }
        }
        
        int pass_count = 0;
        for (int i = 0; i < PATTERN_SIZE; i++) {
            int result = validate_pattern(test_patterns[i], simulated_readback[i]);
            printf("  Test %d: Expected 0x%08X, Got 0x%08X - %s\n", 
                   i, test_patterns[i], simulated_readback[i], 
                   result ? "PASS" : "FAIL");
            pass_count += result;
        }
        
        printf("  Summary: %d/%d tests passed (%.1f%%)\n", 
               pass_count, PATTERN_SIZE, (float)pass_count/PATTERN_SIZE * 100);
    }
    
    return 0;
}
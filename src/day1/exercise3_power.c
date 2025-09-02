#include <stdio.h>

int main() {
    float voltage, current, power;
    float max_power = 5.0;
    
    printf("Power Calculator for Chip Validation\n");
    printf("Maximum allowed power: %.1fW\n", max_power);
    
    printf("Enter voltage (V): ");
    scanf("%f", &voltage);
    
    printf("Enter current (A): ");
    scanf("%f", &current);
    
    power = voltage * current;
    
    printf("Calculated power: %.3fW\n", power);
    
    if (power <= max_power) {
        printf("PASS: Power consumption within limits\n");
    } else {
        printf("FAIL: Power consumption exceeds %.1fW limit\n", max_power);
        printf("Excess power: %.3fW\n", power - max_power);
    }
    
    return 0;
}
#include <stdio.h>

int main() {
    float voltage, current, power;
    float max_power = 5.0; // Maximum allowed power in watts
    
    printf("Power Consumption Calculator\n");
    printf("Enter supply voltage (V): ");
    scanf("%f", &voltage);
    
    printf("Enter current consumption (A): ");
    scanf("%f", &current);
    
    power = voltage * current;
    
    printf("\nCalculated power: %.3f W\n", power);
    printf("Maximum allowed: %.1f W\n", max_power);
    
    if (power <= max_power) {
        printf("PASS: Power consumption within limits\n");
    } else {
        printf("FAIL: Power consumption exceeds maximum\n");
        printf("Excess power: %.3f W\n", power - max_power);
    }
    
    return 0;
}
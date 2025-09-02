#include <stdio.h>

int main() {
    float voltage_min = 1.8;
    float voltage_max = 3.3;
    float measured_voltage;
    
    printf("Voltage Validation System\n");
    printf("Valid range: %.1fV to %.1fV\n", voltage_min, voltage_max);
    printf("Enter measured voltage: ");
    
    scanf("%f", &measured_voltage);
    
    if (measured_voltage >= voltage_min && measured_voltage <= voltage_max) {
        printf("PASS: Voltage %.2fV is within specification\n", measured_voltage);
    } else {
        printf("FAIL: Voltage %.2fV is out of specification\n", measured_voltage);
    }
    
    return 0;
}
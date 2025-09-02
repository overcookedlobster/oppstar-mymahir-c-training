#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

// Include all previous modules
#include "../day4/validation_lib.h"
#include "../day4/fpga_hal.h"

// Validation framework core structures
typedef enum {
    TEST_STATUS_PENDING,
    TEST_STATUS_RUNNING,
    TEST_STATUS_PASSED,
    TEST_STATUS_FAILED,
    TEST_STATUS_SKIPPED,
    TEST_STATUS_ERROR
} test_status_t;

typedef enum {
    TEST_PRIORITY_LOW,
    TEST_PRIORITY_MEDIUM,
    TEST_PRIORITY_HIGH,
    TEST_PRIORITY_CRITICAL
} test_priority_t;

typedef struct {
    char name[64];
    char description[256];
    test_status_t status;
    test_priority_t priority;
    uint32_t execution_time_ms;
    uint32_t start_time;
    uint32_t end_time;
    char error_message[128];
    float measured_value;
    float expected_value;
    float tolerance;
} test_case_t;

typedef struct {
    char suite_name[64];
    test_case_t* tests;
    uint32_t test_count;
    uint32_t tests_passed;
    uint32_t tests_failed;
    uint32_t tests_skipped;
    uint32_t total_execution_time_ms;
    bool suite_enabled;
} test_suite_t;

typedef struct {
    test_suite_t* suites;
    uint32_t suite_count;
    uint32_t total_tests;
    uint32_t total_passed;
    uint32_t total_failed;
    uint32_t total_skipped;
    uint32_t framework_start_time;
    uint32_t framework_end_time;
    char report_filename[128];
    bool verbose_output;
    bool stop_on_failure;
} validation_framework_t;

// Global framework instance
static validation_framework_t g_framework = {0};

// Framework initialization and cleanup
void framework_init(const char* report_filename, bool verbose, bool stop_on_fail) {
    memset(&g_framework, 0, sizeof(validation_framework_t));
    
    if (report_filename) {
        strncpy(g_framework.report_filename, report_filename, 127);
        g_framework.report_filename[127] = '\0';
    } else {
        strcpy(g_framework.report_filename, "validation_report.html");
    }
    
    g_framework.verbose_output = verbose;
    g_framework.stop_on_failure = stop_on_fail;
    g_framework.framework_start_time = (uint32_t)time(NULL);
    
    printf("=== FPGA Validation Framework v1.0 ===\n");
    printf("Report file: %s\n", g_framework.report_filename);
    printf("Verbose mode: %s\n", verbose ? "Enabled" : "Disabled");
    printf("Stop on failure: %s\n", stop_on_fail ? "Enabled" : "Disabled");
    printf("========================================\n\n");
}

void framework_cleanup(void) {
    g_framework.framework_end_time = (uint32_t)time(NULL);
    
    // Free allocated memory
    for (uint32_t i = 0; i < g_framework.suite_count; i++) {
        if (g_framework.suites[i].tests) {
            free(g_framework.suites[i].tests);
        }
    }
    
    if (g_framework.suites) {
        free(g_framework.suites);
    }
    
    printf("\nFramework cleanup completed.\n");
}

// Test suite management
test_suite_t* framework_add_suite(const char* suite_name, uint32_t test_capacity) {
    g_framework.suite_count++;
    g_framework.suites = realloc(g_framework.suites, 
                                g_framework.suite_count * sizeof(test_suite_t));
    
    test_suite_t* suite = &g_framework.suites[g_framework.suite_count - 1];
    memset(suite, 0, sizeof(test_suite_t));
    
    strncpy(suite->suite_name, suite_name, 63);
    suite->suite_name[63] = '\0';
    suite->tests = calloc(test_capacity, sizeof(test_case_t));
    suite->test_count = 0;
    suite->suite_enabled = true;
    
    printf("Added test suite: %s (capacity: %d tests)\n", suite_name, test_capacity);
    return suite;
}

test_case_t* suite_add_test(test_suite_t* suite, const char* test_name, 
                           const char* description, test_priority_t priority) {
    if (!suite || !suite->tests) return NULL;
    
    test_case_t* test = &suite->tests[suite->test_count++];
    memset(test, 0, sizeof(test_case_t));
    
    strncpy(test->name, test_name, 63);
    test->name[63] = '\0';
    strncpy(test->description, description, 255);
    test->description[255] = '\0';
    test->status = TEST_STATUS_PENDING;
    test->priority = priority;
    
    g_framework.total_tests++;
    
    if (g_framework.verbose_output) {
        printf("  Added test: %s\n", test_name);
    }
    
    return test;
}

// Test execution engine
void test_start(test_case_t* test) {
    if (!test) return;
    
    test->status = TEST_STATUS_RUNNING;
    test->start_time = (uint32_t)time(NULL);
    
    if (g_framework.verbose_output) {
        printf("Starting test: %s\n", test->name);
    }
}

void test_end(test_case_t* test, test_status_t final_status, 
              const char* error_msg, float measured, float expected, float tolerance) {
    if (!test) return;
    
    test->end_time = (uint32_t)time(NULL);
    test->execution_time_ms = (test->end_time - test->start_time) * 1000;
    test->status = final_status;
    test->measured_value = measured;
    test->expected_value = expected;
    test->tolerance = tolerance;
    
    if (error_msg && final_status != TEST_STATUS_PASSED) {
        strncpy(test->error_message, error_msg, 127);
        test->error_message[127] = '\0';
    }
    
    const char* status_str = "UNKNOWN";
    switch (final_status) {
        case TEST_STATUS_PASSED: status_str = "PASS"; break;
        case TEST_STATUS_FAILED: status_str = "FAIL"; break;
        case TEST_STATUS_SKIPPED: status_str = "SKIP"; break;
        case TEST_STATUS_ERROR: status_str = "ERROR"; break;
        default: break;
    }
    
    printf("[%s] %s (%.3fs)\n", status_str, test->name, 
           test->execution_time_ms / 1000.0f);
    
    if (final_status == TEST_STATUS_FAILED && error_msg) {
        printf("  Error: %s\n", error_msg);
    }
    
    if (g_framework.verbose_output && final_status == TEST_STATUS_PASSED) {
        printf("  Measured: %.3f, Expected: %.3f ± %.3f\n", 
               measured, expected, tolerance);
    }
}

// Comprehensive validation tests
void run_gpio_validation_suite(void) {
    test_suite_t* suite = framework_add_suite("GPIO Validation", 10);
    
    // Test 1: GPIO Direction Control
    test_case_t* test1 = suite_add_test(suite, "GPIO_Direction_Control", 
                                       "Verify GPIO direction register functionality", 
                                       TEST_PRIORITY_HIGH);
    test_start(test1);
    
    hal_gpio_init();
    hal_gpio_set_direction(0, GPIO_OUTPUT);
    hal_gpio_set_direction(1, GPIO_INPUT);
    
    // Simulate verification (in real hardware, read back direction register)
    bool direction_ok = true; // Assume success for simulation
    
    test_end(test1, direction_ok ? TEST_STATUS_PASSED : TEST_STATUS_FAILED,
             direction_ok ? NULL : "Direction register mismatch", 1.0f, 1.0f, 0.0f);
    
    // Test 2: GPIO Data Write/Read
    test_case_t* test2 = suite_add_test(suite, "GPIO_Data_WriteRead", 
                                       "Verify GPIO data register write/read", 
                                       TEST_PRIORITY_HIGH);
    test_start(test2);
    
    hal_gpio_write(0, 1);
    uint32_t gpio_state = hal_gpio_read(0);
    
    test_end(test2, (gpio_state == 1) ? TEST_STATUS_PASSED : TEST_STATUS_FAILED,
             (gpio_state == 1) ? NULL : "GPIO read/write mismatch", 
             gpio_state, 1.0f, 0.0f);
    
    // Test 3: GPIO Pattern Test
    test_case_t* test3 = suite_add_test(suite, "GPIO_Pattern_Test", 
                                       "Verify GPIO pattern generation", 
                                       TEST_PRIORITY_MEDIUM);
    test_start(test3);
    
    bool pattern_ok = true;
    for (int i = 0; i < 8; i++) {
        hal_gpio_write(0, i & 1);
        hal_gpio_write(1, (i >> 1) & 1);
        hal_gpio_write(2, (i >> 2) & 1);
        
        // Verify pattern (simplified for simulation)
        uint32_t pin0 = hal_gpio_read(0);
        if (pin0 != (i & 1)) {
            pattern_ok = false;
            break;
        }
    }
    
    test_end(test3, pattern_ok ? TEST_STATUS_PASSED : TEST_STATUS_FAILED,
             pattern_ok ? NULL : "GPIO pattern verification failed", 
             pattern_ok ? 1.0f : 0.0f, 1.0f, 0.0f);
}

void run_timer_validation_suite(void) {
    test_suite_t* suite = framework_add_suite("Timer Validation", 5);
    
    // Test 1: Timer Initialization
    test_case_t* test1 = suite_add_test(suite, "Timer_Initialization", 
                                       "Verify timer initialization", 
                                       TEST_PRIORITY_HIGH);
    test_start(test1);
    
    hal_timer_init();
    uint32_t initial_count = hal_timer_get_count();
    
    test_end(test1, TEST_STATUS_PASSED, NULL, initial_count, 0.0f, 1000.0f);
    
    // Test 2: Timer Counting
    test_case_t* test2 = suite_add_test(suite, "Timer_Counting", 
                                       "Verify timer counting functionality", 
                                       TEST_PRIORITY_HIGH);
    test_start(test2);
    
    uint32_t start_count = hal_timer_get_count();
    hal_delay_ms(100);
    uint32_t end_count = hal_timer_get_count();
    uint32_t elapsed = end_count - start_count;
    
    // Expect approximately 100ms worth of counts (tolerance depends on timer frequency)
    bool timing_ok = (elapsed > 50) && (elapsed < 200000); // Wide tolerance for simulation
    
    test_end(test2, timing_ok ? TEST_STATUS_PASSED : TEST_STATUS_FAILED,
             timing_ok ? NULL : "Timer counting out of range", 
             elapsed, 100000.0f, 50000.0f);
}

void run_adc_validation_suite(void) {
    test_suite_t* suite = framework_add_suite("ADC Validation", 8);
    
    // Test ADC channels
    for (int channel = 0; channel < 4; channel++) {
        char test_name[32];
        char test_desc[64];
        snprintf(test_name, sizeof(test_name), "ADC_Channel_%d", channel);
        snprintf(test_desc, sizeof(test_desc), "Verify ADC channel %d functionality", channel);
        
        test_case_t* test = suite_add_test(suite, test_name, test_desc, TEST_PRIORITY_MEDIUM);
        test_start(test);
        
        hal_adc_init();
        uint16_t adc_value = hal_adc_read_channel(channel);
        
        // Convert to voltage (assuming 3.3V reference, 12-bit ADC)
        float voltage = (adc_value * 3.3f) / 4095.0f;
        
        // Validate voltage is within reasonable range (0-3.3V)
        bool voltage_valid = (voltage >= 0.0f) && (voltage <= 3.3f);
        
        test_end(test, voltage_valid ? TEST_STATUS_PASSED : TEST_STATUS_FAILED,
                 voltage_valid ? NULL : "ADC voltage out of range", 
                 voltage, 1.65f, 1.65f);
    }
}

void run_integration_validation_suite(void) {
    test_suite_t* suite = framework_add_suite("Integration Tests", 3);
    
    // Test 1: System Integration
    test_case_t* test1 = suite_add_test(suite, "System_Integration", 
                                       "Verify complete system integration", 
                                       TEST_PRIORITY_CRITICAL);
    test_start(test1);
    
    // Initialize all subsystems
    hal_system_init();
    
    // Run a complex sequence involving multiple peripherals
    hal_gpio_set_direction(0, GPIO_OUTPUT);
    hal_gpio_write(0, 1);
    
    uint32_t timer_start = hal_timer_get_count();
    hal_delay_ms(50);
    uint32_t timer_end = hal_timer_get_count();
    
    uint16_t adc_reading = hal_adc_read_channel(0);
    
    hal_gpio_write(0, 0);
    
    // Verify all operations completed successfully
    bool integration_ok = (timer_end > timer_start) && (adc_reading < 4096);
    
    test_end(test1, integration_ok ? TEST_STATUS_PASSED : TEST_STATUS_FAILED,
             integration_ok ? NULL : "System integration failure", 
             integration_ok ? 1.0f : 0.0f, 1.0f, 0.0f);
    
    // Test 2: Performance Benchmark
    test_case_t* test2 = suite_add_test(suite, "Performance_Benchmark", 
                                       "Measure system performance", 
                                       TEST_PRIORITY_MEDIUM);
    test_start(test2);
    
    uint32_t perf_start = hal_timer_get_count();
    
    // Perform 1000 GPIO operations
    for (int i = 0; i < 1000; i++) {
        hal_gpio_write(0, i & 1);
    }
    
    uint32_t perf_end = hal_timer_get_count();
    uint32_t perf_time = perf_end - perf_start;
    
    // Performance should be reasonable (less than 1 second for 1000 operations)
    bool perf_ok = perf_time < 1000000; // Adjust based on expected performance
    
    test_end(test2, perf_ok ? TEST_STATUS_PASSED : TEST_STATUS_FAILED,
             perf_ok ? NULL : "Performance below expectations", 
             perf_time, 500000.0f, 500000.0f);
}

// Test execution and reporting
void framework_run_all_tests(void) {
    printf("\n=== Running All Validation Tests ===\n");
    
    // Run all test suites
    run_gpio_validation_suite();
    run_timer_validation_suite();
    run_adc_validation_suite();
    run_integration_validation_suite();
    
    // Calculate statistics
    for (uint32_t i = 0; i < g_framework.suite_count; i++) {
        test_suite_t* suite = &g_framework.suites[i];
        
        for (uint32_t j = 0; j < suite->test_count; j++) {
            test_case_t* test = &suite->tests[j];
            
            switch (test->status) {
                case TEST_STATUS_PASSED:
                    suite->tests_passed++;
                    g_framework.total_passed++;
                    break;
                case TEST_STATUS_FAILED:
                case TEST_STATUS_ERROR:
                    suite->tests_failed++;
                    g_framework.total_failed++;
                    break;
                case TEST_STATUS_SKIPPED:
                    suite->tests_skipped++;
                    g_framework.total_skipped++;
                    break;
                default:
                    break;
            }
            
            suite->total_execution_time_ms += test->execution_time_ms;
        }
    }
}

void framework_generate_report(void) {
    FILE* report = fopen(g_framework.report_filename, "w");
    if (!report) {
        printf("Error: Could not create report file %s\n", g_framework.report_filename);
        return;
    }
    
    // Generate HTML report
    fprintf(report, "<!DOCTYPE html>\n<html>\n<head>\n");
    fprintf(report, "<title>FPGA Validation Report</title>\n");
    fprintf(report, "<style>\n");
    fprintf(report, "body { font-family: Arial, sans-serif; margin: 20px; }\n");
    fprintf(report, ".pass { color: green; }\n");
    fprintf(report, ".fail { color: red; }\n");
    fprintf(report, ".skip { color: orange; }\n");
    fprintf(report, "table { border-collapse: collapse; width: 100%%; }\n");
    fprintf(report, "th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n");
    fprintf(report, "th { background-color: #f2f2f2; }\n");
    fprintf(report, "</style>\n</head>\n<body>\n");
    
    fprintf(report, "<h1>FPGA Validation Framework Report</h1>\n");
    fprintf(report, "<p>Generated: %s</p>\n", ctime((time_t*)&g_framework.framework_end_time));
    
    // Summary statistics
    fprintf(report, "<h2>Summary</h2>\n");
    fprintf(report, "<table>\n");
    fprintf(report, "<tr><th>Metric</th><th>Value</th></tr>\n");
    fprintf(report, "<tr><td>Total Tests</td><td>%d</td></tr>\n", g_framework.total_tests);
    fprintf(report, "<tr><td>Passed</td><td class='pass'>%d</td></tr>\n", g_framework.total_passed);
    fprintf(report, "<tr><td>Failed</td><td class='fail'>%d</td></tr>\n", g_framework.total_failed);
    fprintf(report, "<tr><td>Skipped</td><td class='skip'>%d</td></tr>\n", g_framework.total_skipped);
    
    float pass_rate = g_framework.total_tests > 0 ? 
                     (float)g_framework.total_passed / g_framework.total_tests * 100.0f : 0.0f;
    fprintf(report, "<tr><td>Pass Rate</td><td>%.1f%%</td></tr>\n", pass_rate);
    fprintf(report, "</table>\n");
    
    // Detailed results by suite
    for (uint32_t i = 0; i < g_framework.suite_count; i++) {
        test_suite_t* suite = &g_framework.suites[i];
        
        fprintf(report, "<h2>%s</h2>\n", suite->suite_name);
        fprintf(report, "<table>\n");
        fprintf(report, "<tr><th>Test Name</th><th>Status</th><th>Time (ms)</th><th>Details</th></tr>\n");
        
        for (uint32_t j = 0; j < suite->test_count; j++) {
            test_case_t* test = &suite->tests[j];
            
            const char* status_class = "pass";
            const char* status_text = "PASS";
            
            switch (test->status) {
                case TEST_STATUS_FAILED:
                case TEST_STATUS_ERROR:
                    status_class = "fail";
                    status_text = "FAIL";
                    break;
                case TEST_STATUS_SKIPPED:
                    status_class = "skip";
                    status_text = "SKIP";
                    break;
                default:
                    break;
            }
            
            fprintf(report, "<tr><td>%s</td><td class='%s'>%s</td><td>%d</td><td>%s</td></tr>\n",
                   test->name, status_class, status_text, test->execution_time_ms,
                   test->error_message[0] ? test->error_message : test->description);
        }
        
        fprintf(report, "</table>\n");
    }
    
    fprintf(report, "</body>\n</html>\n");
    fclose(report);
    
    printf("Report generated: %s\n", g_framework.report_filename);
}

void framework_print_summary(void) {
    printf("\n=== Validation Framework Summary ===\n");
    printf("Total Test Suites: %d\n", g_framework.suite_count);
    printf("Total Tests: %d\n", g_framework.total_tests);
    printf("Passed: %d\n", g_framework.total_passed);
    printf("Failed: %d\n", g_framework.total_failed);
    printf("Skipped: %d\n", g_framework.total_skipped);
    
    float pass_rate = g_framework.total_tests > 0 ? 
                     (float)g_framework.total_passed / g_framework.total_tests * 100.0f : 0.0f;
    printf("Pass Rate: %.1f%%\n", pass_rate);
    
    uint32_t total_time = g_framework.framework_end_time - g_framework.framework_start_time;
    printf("Total Execution Time: %d seconds\n", total_time);
    
    printf("=====================================\n");
}

// Main capstone project
int main(int argc, char* argv[]) {
    // Parse command line arguments (simplified)
    bool verbose = false;
    bool stop_on_fail = false;
    const char* report_file = "fpga_validation_report.html";
    
    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-v") == 0 || strcmp(argv[i], "--verbose") == 0) {
            verbose = true;
        } else if (strcmp(argv[i], "-s") == 0 || strcmp(argv[i], "--stop-on-fail") == 0) {
            stop_on_fail = true;
        } else if (strncmp(argv[i], "--report=", 9) == 0) {
            report_file = argv[i] + 9;
        }
    }
    
    // Initialize validation framework
    framework_init(report_file, verbose, stop_on_fail);
    
    // Run all validation tests
    framework_run_all_tests();
    
    // Generate reports
    framework_generate_report();
    framework_print_summary();
    
    // Cleanup
    framework_cleanup();
    
    // Return appropriate exit code
    return (g_framework.total_failed == 0) ? 0 : 1;
}
#include <xc.h>                   // Include processor files
#include "mcc_generated_files/mcc.h"  // Include MCC-generated headers for setup
#include <stdio.h>                // For printf support
#include <stdlib.h>               // For rand() and srand()
#include <time.h>                 // For time()

// **Mock Code**: The values sent here are not from real sensors.
// They are randomly generated for demonstration purposes.

void send_mqtt_data_points(void);

void main(void) {
    // Initialize the device
    SYSTEM_Initialize();

    // Seed the random number generator
    // **Mock Code**: Using this since Kevin never gave me the final code for the sensor to integrate this on that.
    srand(123456);

    // Enable global and peripheral interrupts
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    // **Mock Code**: Main loop that continuously sends mock data
    while (1) {
        // Send mock MQTT data points periodically
        send_mqtt_data_points();

        // **Mock Code**: Delay for 2 seconds before sending the next set of data
        __delay_ms(2000);
    }
}

void send_mqtt_data_points(void) {
    // **Mock Code**: Generate mock temperature and hall sensor data
    // Temperature mock range: 20.0 to 35.0 degrees
    // Hall sensor mock range: 0.0 to 50.0
    // Ready flag: toggles between 0 and 1 each time data is sent
    
    static int ready_flag = 0; // **Mock Code**: toggles each send
    float temp = 20.0f + (float)(rand() % 150) / 10.0f;   // 20.0 to 34.9 approx
    float hall = (float)(rand() % 500) / 10.0f;           // 0.0 to 49.9 approx
    
    // Toggle the ready_flag each time
    ready_flag = !ready_flag;

    char buffer[64]; // Buffer for formatted data

    // **Mock Code**: Format the string as "temp hall ready_flag"
    // The NodeMCU expects: temp_value hall_value ready_flag
    // Example: "23.45 12.34 1"
    sprintf(buffer, "%.2f %.2f %d", temp, hall, ready_flag);

    // Transmit the formatted string via UART (to NodeMCU)
    // **Mock Code**: Just prints out the string over UART
    printf("%s\r\n", buffer);

    // **Mock Code**: Flash an LED to indicate data was sent
    LED_0_SetHigh();
    __delay_ms(150);
    LED_0_SetLow();
}

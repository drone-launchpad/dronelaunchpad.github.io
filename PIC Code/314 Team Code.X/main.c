
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"
#include "mcc_generated_files/i2c2_master.h"

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

// ============================
// I2C Sensor Addresses (from provided code)
// ============================
#define TEMP_SENSOR_ADDRESS  0b1001100   // Temp sensor address
#define HALL_SLAVE_ADDRESS   0x36        // Hall sensor address
#define HALL_RAW_ANGLE       0x0E
#define HALL_RAW_ANGLE_1     0x0F

// ============================
// Motor Control Commands (SPI)
// ============================
uint8_t fwd  = 0b11101111;  // Roll platform out
uint8_t bkwd = 0b11101101;  // Roll platform in

// ============================
// Global Variables
// ============================
volatile uint32_t time_ms = 0;
int temp_value = 0;               // Raw temperature reading (int)
uint8_t hall_angle = 0;           
uint8_t hall_angle_1 = 0;         
uint16_t hall_angle_combined = 0; 

bool taken_off = false; // Tracks if platform is rolled out

// For demonstration, we are mocking wind speed.
// In a real scenario, integrate a wind sensor reading or use actual data.
float mock_wind_speed = 5.0f; // <10 mph considered safe

// ============================
// Function Prototypes
// ============================
uint8_t ReadTemperature(void);
void ReadHallSensor(void);
void timer_callback(void);
void RollPlatformOut(void);
void RollPlatformIn(void);
void send_data_to_NodeMCU(float temp, float hall, int ready_flag);

// ============================
// Function Implementations
// ============================

// Read Temperature Sensor Data
uint8_t ReadTemperature(void) {
    return I2C2_Read1ByteRegister(TEMP_SENSOR_ADDRESS, 0x00);
}

// Read Hall Sensor Data
void ReadHallSensor(void) {
    hall_angle = I2C2_Read1ByteRegister(HALL_SLAVE_ADDRESS, HALL_RAW_ANGLE);
    hall_angle_1 = I2C2_Read1ByteRegister(HALL_SLAVE_ADDRESS, HALL_RAW_ANGLE_1);
    hall_angle_combined = ((uint16_t)(hall_angle << 8) | hall_angle_1);
}

// Timer Callback: Called every 1ms, update sensors once per second
void timer_callback(void) {
    time_ms++;
    if (time_ms >= 1000) {
        // Read sensors once per second
        temp_value = (int)ReadTemperature();
        ReadHallSensor();
        time_ms -= 1000;
    }
}

// Motor control to roll platform out (forward)
void RollPlatformOut(void) {
    CSN_SetLow();
    SPI1_ExchangeByte(fwd);
    CSN_SetHigh();
}

// Motor control to roll platform in (backward)
void RollPlatformIn(void) {
    CSN_SetLow();
    SPI1_ExchangeByte(bkwd);
    CSN_SetHigh();
}

// Send data to NodeMCU in the format: "temp hall ready_flag"
// NodeMCU code expects these three values separated by spaces, 
// with temp and hall as floats, and ready_flag as int (0 or 1).
void send_data_to_NodeMCU(float temp, float hall, int ready_flag) {
    char buffer[64];
    sprintf(buffer, "%.2f %.2f %d", temp, hall, ready_flag);
    printf("%s\r\n", buffer);
}

// ============================
// Main Application with Diagram Logic
// ============================
void main(void) {
    // Initialize system
    SYSTEM_Initialize();
    I2C2_Initialize();
    EUSART2_Initialize();
    SPI1_Initialize();
    SPI1_Open(SPI1_DEFAULT);

    // Enable interrupts
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();

    // Set timer callback
    TMR2_SetInterruptHandler(timer_callback);
    TMR2_StartTimer();

    // Initial LED blink test
    LED_Toggle();
    __delay_ms(1000);
    LED_Toggle();
    __delay_ms(1000);

    while (1) {
        // Determine safety conditions
        // Temp_Safe if 32 < Temp < 104
        // Wind_Safe if Wind Speed < 10 mph
        bool Temp_Safe = (temp_value > 32 && temp_value < 104);
        bool Wind_Safe = (mock_wind_speed < 10.0f);

        // If both safe:
        // If platform is not out, roll out and set taken_off = true
        // Else if unsafe, roll in and set taken_off = false
        if (Temp_Safe && Wind_Safe) {
            if (!taken_off) {
                RollPlatformOut();
                taken_off = true;
            }
        } else {
            if (taken_off) {
                RollPlatformIn();
                taken_off = false;
            }
        }

        // Convert raw temp to float
        float temperature_float = (float)temp_value;
        // Convert hall angle to a meaningful float value
        // Example: If 1024 counts = 90 degrees, angle in degrees:
        float hall_angle_degrees = (hall_angle_combined * 90.0f / 1024.0f);

        // ready_flag will reflect 'taken_off' state
        int ready_flag = taken_off ? 1 : 0;

        // Send data to NodeMCU
        send_data_to_NodeMCU(temperature_float, hall_angle_degrees, ready_flag);

        // Blink LED to show loop running
        LED_Toggle();
        __delay_ms(2000); // Adjust timing as needed
        LED_Toggle();
        __delay_ms(200);
    }
}
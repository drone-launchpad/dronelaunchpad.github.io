
//file inclusions
#include "mcc_generated_files/mcc.h"
#include "mcc_generated_files/examples/i2c2_master_example.h"
#include "mcc_generated_files/i2c2_master.h" 

//library inclusions
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

//hall effect info
/*
                         typedef uint8_t i2c1_address_t;
 * 
 * slave address 0x36 (0110110)
 * angle add 1 0x0E
 * angle add 2 0x0F
 * 1024 is a 90 degree turn
*/

//define hall effect addresses
#define slave_address 0x36 // slave address
#define raw 0x0E // raw angle
#define raw_1 0x0F //1111  angle
int temp;

int data = 0x0;
//define temp sensor address
#define address 0b1001100

//variables
uint32_t time_ms = 0; 
uint32_t time_s = 0;
uint8_t angle, angle_1 = 0; //this is the data variable

//#define MCP9600_ADDR 0x4C
//#define MCP9600_Reg 0x0

//temperature sensor function to call in code
uint8_t ReadTemperature (void)
{
    uint8_t curr_temp = 0;
    curr_temp = I2C2_Read1ByteRegister( address, 0x00);
    return curr_temp;
}

//timer callback function for reasonable numbers
void timer_callback(void)
{
    time_ms++; 
    if (time_ms >= 1000) 
    { 
        
        temp = I2C2_Read1ByteRegister( address, 0x00);
        
        //hall effect code
        angle = I2C2_Read1ByteRegister(slave_address, raw);
        angle_1 = I2C2_Read1ByteRegister(slave_address, raw_1);
        
        //data = angle<<8|angle_1;
        
        if(EUSART2_is_tx_ready())
            {
            
                //printf("Hall \n\r");
                printf("sensor reading: %u \n\r", (angle) );
                printf("hall sensor reading 1: %u \n\r", (angle_1) ); 
                //printf("Hall sensor reading %i \n\r", (data) );
                printf("Temp reading: %i \n\r", (temp) );
                
            } 
        time_ms -= 1000;
        //time_s++; 
    } 
}

//main code start
void main(void)
{   
//    //initialize systems
    SYSTEM_Initialize();
    
    TMR2_StartTimer();
    
    I2C2_Initialize();
    
    EUSART2_Initialize();
    
    
    //interrupts
    INTERRUPT_GlobalInterruptEnable();
    INTERRUPT_PeripheralInterruptEnable();
        LED_Toggle();
        __delay_ms(2000);
        LED_Toggle();
        __delay_ms(2000);
    TMR2_SetInterruptHandler(timer_callback);
    
    int Temp;
    uint8_t angle, angle_1 = 0; //this is the data variable for hall effect
    
    while (1)
    {
        //testing LED blink
        LED_Toggle();
        __delay_ms(200);
        LED_Toggle();
        __delay_ms(200);
        
        //timer_callback();
        /*
        Temp = ReadTemperature();
        
        //hall effect code
        angle = I2C2_Read1ByteRegister(slave_address, raw);
        angle_1 = I2C2_Read1ByteRegister(slave_address, raw_1);
        
        
        data = angle<<8|angle_1;
        
        if(EUSART2_is_tx_ready())
            {

                //printf("Hall \n\r");
                //printf("sensor reading %u \n\r", (angle) );
                //printf("sensor reading %u \n\r", (angle_1) ); 
                printf("Hall sensor reading %i \n\r", (data) );
                printf("Temp reading %i \n\r", (Temp) );
            } 
         */
    }
}

/**
 End of File 0x36 slave address
*/
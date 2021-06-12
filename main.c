#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>



#include "UART.h"
#include "stdint.h"


#define CPAC (*((volatile uint32_t *)0xE000ED88))
void SystemInit() {
    CPAC |= 0X00F00000;
}

char s_lat[50];
char s_long[50];
double lat = 0, lon = 0;
double totalDistance = 0.0;
double lat1, lat2, long1, long2;





//#########################################
//############### LED STUFF ###############
//#########################################
void PortF_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x00000020; // activate Port F
    while ((SYSCTL_PRGPIO_R & 0x00000020) == 0)
    {
    };
    GPIO_PORTF_PCTL_R = 0x1F; // allow changes to PF4-0
    GPIO_PORTF_DIR_R = 0x0E;  // PF4,PF0 in, PF3-1 out
    GPIO_PORTF_DEN_R = 0x1F;  // digital I/O on PF4-0
}

void LED_Init()
{
    PortF_Init();
}

void PortF_Output(uint32_t data)
{ // write PF3-PF1 outputs
    GPIO_PORTF_DATA_R = data;
}

void LED_Out(uint8_t x)
{
    if (x == 1)
        PortF_Output(0x02);
    else
        PortF_Output(0x00);
}

//#########################################
//############### PORT B ###############
//#########################################

//port B will be used for displaying the units and tens digits
//of distance on the 7-segment display
void PortB_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x00000002; // activate Port B
    while ((SYSCTL_PRGPIO_R & 0x00000002) == 0)
    {
    };                           //wait for Port B to be ready
    GPIO_PORTB_PCTL_R = 0xFF;    // allow changes
    GPIO_PORTB_DIR_R = 0xFF;     // all ports are output ports
    GPIO_PORTB_PUR_R = 0x00;     // no pull-up resistor
    GPIO_PORTB_DEN_R = 0xFF;     // all ports are digital
    GPIO_PORTB_AMSEL_R &= ~0xFF; // disable analog
    GPIO_PORTB_AFSEL_R &= ~0xFF;
}

//#########################################
//############### PORT A ##################
//#########################################

//last 4 bits of Port A will be used to display
//the hundreds digit of distance on 7 segment display
void PortA_Init(void)
{
    SYSCTL_RCGCGPIO_R |= 0x00000001; // activate Port A
    while ((SYSCTL_PRGPIO_R & 0x00000001) == 0)
    {
    };                           //wait for Port A to be ready
    GPIO_PORTA_PCTL_R |= 0xF0;   // allow changes
    GPIO_PORTA_DIR_R |= 0xF0;    // PA4,PA5,PA6,PA7 output
    GPIO_PORTA_DEN_R |= 0xF0;    // digital I/O on PA4,PA5,PA6,PA7
    GPIO_PORTA_AMSEL_R &= ~0xF0; // disable analog
    GPIO_PORTA_AFSEL_R &= ~0xF0;
}

//#########################################
//############### SYSTICK #################
//#########################################
void SysTick_Init(void)
{
    NVIC_ST_CTRL_R = 0;            // 1) disable SysTick during setup
    NVIC_ST_RELOAD_R = 0x00FFFFFF; // 2) maximum reload value
    NVIC_ST_CURRENT_R = 0;         // 3) any write to current clears it
    NVIC_ST_CTRL_R = 0x00000005;   // 4) enable SysTick with core clock
}

//The delay parameter is in units of the
void SysTick_Wait(uint32_t delay)
{
    NVIC_ST_RELOAD_R = delay - 1; // number of counts to wait
    NVIC_ST_CURRENT_R = 0;        // any value written to CURRENT clears
    while ((NVIC_ST_CTRL_R & 0x00010000) == 0)
    {
    } // wait for count flag
}

// 10000us equals 1ms
void wait1ms(uint32_t delay)
{
    uint32_t i;
    for (i = 0; i < delay; i++)
    {
        SysTick_Wait(16000); // wait 1ms
    }
}

//#########################################
//############### UART0 ###################
//#########################################
void UART0_init(void)
{
    SYSCTL_RCGCUART_R |= 1;          // ACTIVATE UART0 CLOCK
    SYSCTL_RCGCGPIO_R |= 0x00000001; // ACTIVATE CLOCK FOR PORT A
    while ((SYSCTL_PRGPIO_R & 0x00000001) == 0)
    {
    };

    UART0_CTL_R = 0; // DISBALE UART
    /* DECLARING THE BAUD RATE  */
    UART0_IBRD_R = 104;
    UART0_FBRD_R = 11;

    UART0_LCRH_R = 0x0070;
    UART0_CTL_R = 0x0301;

    GPIO_PORTA_AMSEL_R &= ~0X03;
    GPIO_PORTA_AFSEL_R |= 0X03;                                        // TO ACTIVATE ALTERNATIVE FUNCTION SELECT FOR PIN0 AND PIN1
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) | 0X00000011; // CHOOSE PIN0 AND PIN1 TO BE UART0 RECEIVE AND TRANSMIT RESPECTIVELY
    GPIO_PORTA_DEN_R |= 0X03;                                          // TO ENABLE PIN TO BE DIGITAL
}

uint8_t UART0_recieveByte(void)
{
    while (((UART0_FR_R & 0x10) != 0))
        ;
    return (uint8_t)UART0_DR_R; // RETURN FIRST 8 BITS IN UART0 DATA REGISTER
}

//send data to coputer
void UART0_sendByte(char data)
{
    while (((UART0_FR_R & 0x20) != 0))
        ;
    UART0_DR_R = data;
}

void UART0_sendString(char *Str)
{
    uint8_t i = 0;
    while (Str[i] != '\0')
    {
        UART0_sendByte(Str[i]);
        i++;
    }
}

//#########################################
//############### 7 SEGMENT ###############
//#########################################

void displayDigit1andDigit2(uint8_t dig1, uint8_t dig2)
{
    GPIO_PORTB_DATA_R = dig1 + (dig2 << 4);
}
void displayDigit3(uint8_t dig)
{
    //pa 4 5 6 7 ditit 3     ### hundreds ###
    GPIO_PORTA_DATA_R &= 0x0F;
    GPIO_PORTA_DATA_R |= (dig << 4);
}

void displayTotalDigit(uint32_t digit)
{
    uint8_t digit1;
    uint8_t digit2;
    uint8_t digit3;
    if (digit < 1000)
    {
        // unit digit
        digit1 = digit % 10;
        digit /= 10;
        //displayDigit1(digit1);

        // tens digit
        digit2 = digit % 10;
        digit /= 10;
        //displayDigit2(digit2);

        displayDigit1andDigit2(digit1, digit2);

        // hunds digit
        digit3 = digit;
        displayDigit3(digit3);
    }
}

//#########################################
//############### GPS #####################
//#########################################

//$GPRMC,092751.000,A,5321.6802,N,00630.3371,W,0.06,31.66,280511,,,A*45
void readGPSModule()
{
    //	uint8_t in;
    //	*lat = 123.4;
    //	*lon = 34.56;
    //in = UART7_Read();

    char c0, GPSValues[100], parseValue[12][20], *token;
    double latitude = 0.0, longitude = 0.0, seconds = 0.0, minutes = 0.0;
    const char comma[2] = ",";
    int index = 0, degrees;

    c0 = UART7_Read();
    //UART0_sendByte(c0);
    if (c0 == '$')
    {
        char c1 = (char)UART7_Read();
        if (c1 == 'G')
        {
            char c2 = (char)UART7_Read();
            if (c2 == 'P')
            {
                char c3 = (char)UART7_Read();
                if (c3 == 'R')
                {
                    char c4 = (char)UART7_Read();
                    if (c4 == 'M')
                    {
                        char c5 = (char)UART7_Read();
                        if (c5 == 'C')
                        {
                            char c6 = (char)UART7_Read();
                            if (c6 == ',')
                            {
                                char c7 = (char)UART7_Read();

                                //reading
                                while (c7 != '*')
                                {
                                    GPSValues[index] = c7;
                                    c7 = (char)UART7_Read();
                                    // UART0_sendByte(c7);
                                    index++;
                                }

                                index = 0;
                                token = strtok(GPSValues, comma);
                                while (token != NULL)
                                {
                                    strcpy(parseValue[index], token);
                                    token = strtok(NULL, comma);
                                    index++;
                                }

                                // get latitude and longitude
                                if (strcmp(parseValue[1], "A") == 0)
                                {
                                    latitude = atof(parseValue[2]);
                                    longitude = atof(parseValue[4]);

                                    //latitude calculations
                                    degrees = latitude / 100;
                                    minutes = latitude - (double)(degrees * 100);
                                    seconds = minutes / 60.00;
                                    lat = degrees + seconds;

                                    sprintf(s_lat, "%f", lat);
                                    UART0_Write('L');
                                    UART0_sendString(s_lat);

                                    //longitude calculations
                                    degrees = longitude / 100;
                                    minutes = longitude - (double)(degrees * 100);
                                    seconds = minutes / 60.00;
                                    lon = degrees + seconds;
                                    sprintf(s_long, "%f", lon);
                                    UART0_Write('G');
                                    UART0_sendString(s_long);
                                }
                            }
                        }
                    }
                }
            }
        }
    }
}

//#########################################
//############### DISTANCE ################
//#########################################
#define PI 3.14159265358979323846

double deg2rad(double deg)
{
    return deg * (PI / 180);
}

double getDistanceInM(double lat1, double lon1, double lat2, double lon2)
{
    int R = 6371;                       // Radius of the earth in m
    double dLat = deg2rad(lat2 - lat1); // deg2rad below
    double dLon = deg2rad(lon2 - lon1);
    double a =
        sin(dLat / 2) * sin(dLat / 2) +
        cos(deg2rad(lat1)) * cos(deg2rad(lat2)) *
            sin(dLon / 2) * sin(dLon / 2);

    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double d = R * c; // Distance in m
    return d * 1000;
}

int main(void)
{

    // init
    LED_Init();
    SysTick_Init();
    PortA_Init();
    PortB_Init();

    UART0_Init();
    UART7_Init();
    LED_Out(0);

    /*
    	//start of the code
    totalDistance =13;

        while (1) {
    displayTotalDigit(totalDistance)
    if(totalDistance>=10){
    LED_Out(1);
    }
    */


    while (1)
    {

        readGPSModule();
        lat1 = lat;
        long1 = lon;

        readGPSModule();
        lat2 = lat;
        long2 = lon;

        totalDistance += getDistanceInM(lat1, long1, lat2, long2); //return distance btw 2 points

        displayTotalDigit((uint32_t)totalDistance); //display current distance on 7 segment

        //uint8_t in;
        //in = UART7_Read();
        //UART0_Write(in);

        // At distance 100 m
        if (totalDistance >= 100) {
            LED_Out(1); //turn on led
        }

    }
}

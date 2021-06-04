#include "tm4c123gh6pm.h"
#include <stdint.h>
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>

//#########################################
//############### LED STUFF ###############
//#########################################
void PortF_Init(void){
SYSCTL_RCGCGPIO_R |= 0x00000020; // activate Port F
while((SYSCTL_PRGPIO_R&0x00000020) == 0){};
GPIO_PORTF_PCTL_R = 0x1F; // allow changes to PF4-0
GPIO_PORTF_DIR_R = 0x0E; // PF4,PF0 in, PF3-1 out
GPIO_PORTF_DEN_R = 0x1F; // digital I/O on PF4-0
}

void LED_Init(){
	PortF_Init();
}

void PortF_Output(uint32_t data){ // write PF3-PF1 outputs
GPIO_PORTF_DATA_R = data;
}

void LED_Out(uint8_t x){
	if(x==1)
	PortF_Output(0x02);
	else
		PortF_Output(0x00);
}

//#########################################
//############### PORT B ###############
//#########################################


//port B will be used for displaying the units and tens digits
//of distance on the 7-segment display
void PortB_Init(void){
SYSCTL_RCGCGPIO_R |= 0x00000002; // activate Port B
while((SYSCTL_PRGPIO_R&0x00000002) == 0){}; //wait for Port B to be ready
GPIO_PORTB_PCTL_R = 0xFF; // allow changes
GPIO_PORTB_DIR_R = 0xFF; // all ports are output ports
GPIO_PORTB_PUR_R = 0x00; // no pull-up resistor
GPIO_PORTB_DEN_R = 0xFF; // all ports are digital
GPIO_PORTB_AMSEL_R &= ~0xFF; // disable analog
GPIO_PORTB_AFSEL_R &= ~0xFF;
}


//#########################################
//############### PORT A ##################
//#########################################

//last 4 bits of Port A will be used to display
//the hundreds digit of distance on 7 segment display
void PortA_Init(void){
SYSCTL_RCGCGPIO_R |= 0x00000001; // activate Port A
while((SYSCTL_PRGPIO_R&0x00000001) == 0){}; //wait for Port A to be ready
GPIO_PORTA_PCTL_R |= 0xF0; // allow changes
GPIO_PORTA_DIR_R |= 0xF0; // PA4,PA5,PA6,PA7 output
GPIO_PORTA_DEN_R |= 0xF0; // digital I/O on PA4,PA5,PA6,PA7
GPIO_PORTA_AMSEL_R &= ~0xF0; // disable analog
GPIO_PORTA_AFSEL_R &= ~0xF0;
}



//#########################################
//############### UART0 ###################
//#########################################
void UART0_init(void)
{
SYSCTL_RCGCUART_R |= 1;                                   // ACTIVATE UART0 CLOCK
SYSCTL_RCGCGPIO_R |= 0x00000001;                           // ACTIVATE CLOCK FOR PORT A
while((SYSCTL_PRGPIO_R&0x00000001) == 0){};

UART0_CTL_R  = 0;                                           // DISBALE UART
/* DECLARING THE BAUD RATE  */
UART0_IBRD_R = 104;
UART0_FBRD_R = 11;

UART0_LCRH_R = 0x0070;
UART0_CTL_R  = 0x0301;

GPIO_PORTA_AMSEL_R &= ~0X03;
GPIO_PORTA_AFSEL_R |= 0X03;                      // TO ACTIVATE ALTERNATIVE FUNCTION SELECT FOR PIN0 AND PIN1
GPIO_PORTA_PCTL_R  = (GPIO_PORTA_PCTL_R & 0xFFFFFF00)| 0X00000011;                             // CHOOSE PIN0 AND PIN1 TO BE UART0 RECEIVE AND TRANSMIT RESPECTIVELY
GPIO_PORTA_DEN_R |= 0X03;                     // TO ENABLE PIN TO BE DIGITAL
}

uint8_t UART0_recieveByte(void)
{
  while( ((UART0_FR_R & 0x10) != 0));
  return (uint8_t)UART0_DR_R;     // RETURN FIRST 8 BITS IN UART0 DATA REGISTER
}

//send data to coputer
void UART0_sendByte(char data)
{
    while( ((UART0_FR_R & 0x20) != 0));
    UART0_DR_R = data;
}

void UART0_sendString(char *Str)
{
    uint8_t i = 0;
    while(Str[i] != '\0')
    {
        UART0_sendByte(Str[i]);
        i++;
    }
}


//#########################################
//############### UART7 ###################
//#########################################
void UART7_init(void)
{
SYSCTL_RCGCUART_R |= 0x80;                                   // ACTIVATE UART0 CLOCK
SYSCTL_RCGCGPIO_R |= 0x00000010;                           // ACTIVATE CLOCK FOR PORT E
while((SYSCTL_PRGPIO_R&0x00000010) == 0){};

UART7_CTL_R  = 0;                                           // DISBALE UART
/* DECLARING THE BAUD RATE  */
UART7_IBRD_R = 104;
UART7_FBRD_R = 11;


UART7_LCRH_R = 0x0070;
UART7_CTL_R  = 0x0301;

GPIO_PORTE_AMSEL_R &= ~0X03;
GPIO_PORTE_AFSEL_R |= 0X03;                      // TO ACTIVATE ALTERNATIVE FUNCTION SELECT FOR PIN0 AND PIN1
GPIO_PORTE_PCTL_R  = (GPIO_PORTE_PCTL_R & 0xFFFFFF00)| 0X00000011;                             // CHOOSE PIN0 AND PIN1 TO BE UART0 RECEIVE AND TRANSMIT RESPECTIVELY
GPIO_PORTE_DEN_R |= 0X03;                     // TO ENABLE PIN TO BE DIGITAL

}

char UART7_recieveByte(void)
{
  while( ((UART7_FR_R & 0x10) != 0));
	UART0_sendByte((char)UART7_DR_R&0xFF);
	//return 0;
  return (char)(UART7_DR_R&0xFF);     // RETURN FIRST 8 BITS IN UART0 DATA REGISTER
}


//#########################################
//############### 7 SEGMENT ###############
//#########################################

void displayDigit1andDigit2(uint8_t dig1,uint8_t dig2)
{
	GPIO_PORTB_DATA_R = dig1 + (dig2<<4);
}
void displayDigit3(uint8_t dig){
//pa 4 5 6 7 ditit 3     ### hundreds ###
	GPIO_PORTA_DATA_R &= 0x0F;
	GPIO_PORTA_DATA_R |= (dig<<4);
}

void displayTotalDigit(uint32_t digit){
	uint8_t digit1;
	uint8_t digit2;
	uint8_t digit3;
if (digit < 1000){
// unit digit
digit1 = digit % 10;
digit /= 10;
//displayDigit1(digit1);

 // tens digit
digit2 = digit % 10;
digit /= 10;
//displayDigit2(digit2);

displayDigit1andDigit2(digit1,digit2);

 // hunds digit
digit3 = digit;
displayDigit3(digit3);
}
}


//#########################################
//############### DISTANCE ################
//#########################################
#define PI 3.14159265358979323846

double deg2rad(double deg) {
	return deg * (PI / 180);
}

double getDistanceInM(double lat1, double lon1, double lat2, double lon2) {
	int R = 6371; // Radius of the earth in m
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




int main(){




    return 0;
}
#include "tm4c123gh6pm.h"
#include "stdint.h"

#define GPIO_PA10_M 0x03

//#########################################
//############### UART7 ###################
//#########################################

void UART7_Init(void){
	SYSCTL_RCGCUART_R |= SYSCTL_RCGCUART_R7;
	SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4;
	
	UART7_CTL_R &= ~UART_CTL_UARTEN;
	UART7_IBRD_R = 104;    
    UART7_FBRD_R = 11;
	UART7_LCRH_R = (UART_LCRH_WLEN_8 | UART_LCRH_FEN);
	UART7_CTL_R |= (UART_CTL_RXE | UART_CTL_TXE | UART_CTL_UARTEN);
	
	GPIO_PORTE_AFSEL_R |= GPIO_PA10_M;
	GPIO_PORTE_PCTL_R = (GPIO_PORTE_PCTL_R&0xFFFFFF00) | (GPIO_PCTL_PE1_U7TX | GPIO_PCTL_PE0_U7RX);
	GPIO_PORTE_DEN_R |= GPIO_PA10_M;
}

uint8_t UART7_Available(void){
	return ((UART7_FR_R&UART_FR_RXFE) == UART_FR_RXFE) ? 0 : 1;
}

uint8_t UART7_Read(void){
	while(UART7_Available() != 1);
	return (uint8_t)(UART7_DR_R&0xFF);
}
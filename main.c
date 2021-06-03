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









int main(){




    return 0;
}

#include "ADC.h"
#include "..//tm4c123gh6pm.h"

// This initialization function sets up the ADC 
// Max sample rate: <=125,000 samples/second
// SS3 triggering event: software trigger
// SS3 1st sample source:  channel 1
// SS3 interrupts: enabled but not promoted to controller
void ADC0_Init(void){ 
  volatile unsigned long delay;
	SYSCTL_RCGC2_R |=0x00000010;
	delay=SYSCTL_RCGC2_R;
	GPIO_PORTE_DIR_R &= ~0x04;
	GPIO_PORTE_AFSEL_R |=0x04;
	GPIO_PORTE_DEN_R &= ~0x04;
	GPIO_PORTE_AMSEL_R |=0x04;
	SYSCTL_RCGC0_R |=0x00000300;
	SYSCTL_RCGCADC_R |=0x00001;
	ADC0_SSPRI_R = 0x0123;
	ADC0_ACTSS_R &= ~0x0008;
	ADC0_EMUX_R &= ~0xF000;
	ADC0_SSMUX3_R &=0x000F;
	ADC0_SSMUX3_R +=1;
	ADC0_SSCTL3_R = 0x0006;
	ADC0_ACTSS_R |=0x0008;
}


//------------ADC0_In------------
// Busy-wait Analog to digital conversion
// Input: none
// Output: 12-bit result of ADC conversion
unsigned long ADC0_In(void){  
  unsigned long result;
  ADC0_PSSI_R = 0x0008;            // 1) initiate SS3
  while((ADC0_RIS_R&0x08)==0){};   // 2) wait for conversion done
  result = ADC0_SSFIFO3_R&0xFFF;   // 3) read result
  ADC0_ISC_R = 0x0008;             // 4) acknowledge completion
	//measurment= 
  return result; 
}

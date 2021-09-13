
// Slide pot pin 3 connected to +3.3V
// Slide pot pin 2 connected to PE2(Ain1) and PD3
// Slide pot pin 1 connected to ground


#include "ADC.h"
#include "..//tm4c123gh6pm.h"
#include "Nokia5110.h"
#include "TExaS.h"

void EnableInterrupts(void);  // Enable interrupts

unsigned char String[10]; // null-terminated ASCII string
unsigned long Distance;   // units 0.001 cm
unsigned long ADCdata;    // 12-bit 0 to 4095 sample
unsigned long Flag =0;       // 1 means valid Distance, 0 means Distance is empty

//********Convert****************
// Convert a 12-bit binary ADC sample into a 32-bit unsigned
// fixed-point distance (resolution 0.001 cm).  Calibration
// data is gathered using known distances and reading the
// ADC value measured on PE1.  
// Overflow and dropout should be considered 
// Input: sample  12-bit ADC sample
// Output: 32-bit distance (resolution 0.001cm)
unsigned long Convert(unsigned long sample){
	Distance = (500*sample)>>10;
  return Distance;  // replace this line with real code
}

// Initialize SysTick interrupts to trigger at 40 Hz, 25 ms
void SysTick_Init(unsigned long period){
	NVIC_ST_CTRL_R = 0;
	NVIC_ST_RELOAD_R = period-1;
	NVIC_ST_CURRENT_R = 0;
	NVIC_SYS_PRI3_R = 0;
	NVIC_ST_CTRL_R = 0x07;
}
// executes every 25 ms, collects a sample, converts and stores in mailbox
void SysTick_Handler(void){ 
	Distance = Convert(ADC0_In());
	Flag = 1;
}

//-----------------------UART_ConvertDistance-----------------------
// Converts a 32-bit distance into an ASCII string
// Input: 32-bit number to be converted (resolution 0.001cm)
// Output: store the conversion in global variable String[10]
// Fixed format 1 digit, point, 3 digits, space, units, null termination
// Examples
//    4 to "0.004 cm"  
//   31 to "0.031 cm" 
//  102 to "0.102 cm" 
// 2210 to "2.210 cm"
//10000 to "*.*** cm"  any value larger than 9999 converted to "*.*** cm"
void UART_ConvertDistance(unsigned long n){
// as part of Lab 11 you implemented this function
if (n>9999){
		unsigned long i=0;
		for(;i<4;i++)
		String[i]='*';
	}
	else {
		unsigned long mod=10000;
		unsigned long k=0;
		//unsigned long once=1; //dummy variable to write the '.' only one time
		while(mod!=0){
			String[k]=n/(mod/10)+'0';
			if(mod==10000)// && (once==1))
				{ k++;
					String[k]='.';
					//once--;
					n=n%(mod/10);
					mod=mod/10;
					k++;
				}
			else {n=n%(mod/10);
			mod=mod/10;
			k++;}
		}
		String [k]=' ';
		String [k+1]='c';
		String [k+2]='m';
	}
}

// main1 is a simple main program allowing you to debug the ADC interface
int main1(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
  }
}
// once the ADC is operational, you can use main2 to debug the convert to distance
int main2(void){ 
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_NoScope);
  ADC0_Init();    // initialize ADC0, channel 1, sequencer 3
  Nokia5110_Init();             // initialize Nokia5110 LCD
  EnableInterrupts();
  while(1){ 
    ADCdata = ADC0_In();
    Nokia5110_SetCursor(0, 0);
    Distance = Convert(ADCdata);
    UART_ConvertDistance(Distance); // from Lab 11
    Nokia5110_OutString(String);    // output to Nokia5110 LCD (optional)
  }
}
// once the ADC and convert to distance functions are operational,
// you should use this main to build the final solution with interrupts and mailbox
int main(void){ 
  volatile unsigned long delay;
  TExaS_Init(ADC0_AIN1_PIN_PE2, SSI0_Real_Nokia5110_Scope);
	ADC0_Init();		// initialize ADC0, channel 1, sequencer 3
	Nokia5110_Init();		// initialize Nokia5110 LCD (optional)
	SysTick_Init(2000000);		// initialize SysTick for 40 Hz interrupts
// initialize profiling on PF1 (optional)
                                    //    wait for clock to stabilize

  EnableInterrupts();
// print a welcome message  (optional)
  while(1){ 
	if(Flag==1){
		Nokia5110_SetCursor(0, 0);
		UART_ConvertDistance(Distance); 
    Nokia5110_OutString(String);
		Flag = 0;
	}
// output to Nokia5110 LCD (optional)
  }
}

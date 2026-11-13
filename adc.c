#include<LPC214x.h>         // Include LPC214x microcontroller register definitions
//#include<LPC21xx.h>       // Commented-out alternative header for LPC21xx series
#include"lcd.h"             // Include LCD display function prototypes
#include"lcd_definesh.h"    // Include LCD command and hardware pin definitions
#include"kpm.h"             // Include keypad function prototypes
#include"delay.h"           // Include delay function prototypes
#include "adc.h"            // Include ADC function prototypes

// Function to initialize ADC0 and configure pin P0.27 as analog input AD0.0
void init_adc(unsigned int ch){
	PINSEL1&=~(3<<((27-16)*2));   // Clear bits 23:22 in PINSEL1 to reset P0.27 pin function
	PINSEL1|=(1<<((27-16)*2));    // Set bit 22 in PINSEL1 (01) to configure P0.27 as AD0.0
	AD0CR=0x00000000;             // Reset all bits in ADC0 Control Register to 0
	AD0CR|=1<<ch;                 // Select ADC channel 'ch' (sets bit 0 for AD0.0)
	AD0CR|=13<<8;                 // Set CLKDIV = 13 (bits 15:8) so ADC clock = PCLK/(13+1) <= 4.5MHz
	//AD0CR|=1<<16;               // Commented-out BURST mode enable (software-controlled mode used instead)
	AD0CR|=1<<21;                 // Set PDN bit (bit 21 = 1) to bring ADC out of power-down mode
}

// Function to start an ADC conversion, calculate voltage, display it on LCD, and return it
float adc(void){
	unsigned int res;             // Variable to store the 10-bit digital ADC output (0 to 1023)
	float v;                      // Variable to store the converted analog voltage (0.0V to 3.3V)
	AD0CR|=1<<24;                 // Set START bits to 001 (bit 24 = 1) to trigger conversion immediately
	while(((AD0GDR>>31)&1)==0);   // Wait in loop until DONE bit (bit 31 of AD0GDR) becomes 1
	res=((AD0GDR>>6)&0x3FF);      // Shift right by 6 and mask with 0x3FF to extract 10-bit result (bits 15:6)
	v=res*(3.3/1023);             // Convert digital count (0-1023) to analog voltage using 3.3V reference
	CmdLCD(GOTO_LINE1_POS0+9);    // Move LCD cursor to Line 1, Column 9
	StrLCD("LCD:");               // Print the label "LCD:" at Line 1, Column 9
	CmdLCD(GOTO_LINE1_POS0+13);   // Move LCD cursor to Line 1, Column 13
	F32LCD(v,2);                  // Display the floating-point voltage 'v' rounded to 2 decimal places
	return v;                     // Return the calculated voltage value to the caller
}

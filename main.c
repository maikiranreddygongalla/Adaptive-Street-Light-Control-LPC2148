#include<LPC214x.h>         // Include LPC214x register definitions
#include"lcd.h"             // Include LCD function prototypes
#include"lcd_definesh.h"    // Include LCD command macros
#include"kpm.h"             // Include keypad function prototypes
#include"delay.h"           // Include delay function prototypes
#include "adc.h"            // Include ADC function prototypes
#include "rtc.h"            // Include RTC function prototypes
#include "interrupt.h"      // Include interrupt and edit function prototypes

void eint0_fiq_isr(void) __irq; // ISR prototype for External Interrupt 0
void led_status(void);          // Prototype for automated LED/street-light control logic
void rtc_lcd(void);             // Prototype for displaying RTC time on LCD
unsigned int flag=0,ec;         // Global flag toggled by EINT0 ISR to trigger time editing

int main(){
	
	unsigned char last;                      // Stores last key pressed during keypad entry
	unsigned int num=0;                      // Accumulates multi-digit number entered on keypad
	
	InitLCD();                               // Initialize 16x2 LCD in 8-bit mode
	init_eint();                             // Configure External Interrupt 0 on P0.1
	VICVectAddr0=(unsigned)eint0_fiq_isr;    // Register ISR address in VIC Slot 0
	init_rtc();                              // Initialize and start the internal RTC
	InitKPM();                               // Initialize 4x4 matrix keypad on Port 1
	init_adc(0);                             // Initialize ADC0 on Channel 0 (P0.27)

	HOUR=16;                                 // Set initial RTC hour to 16 (4:00 PM)
	MIN=15;                                  // Set initial RTC minute to 15
	SEC=1;                                   // Set initial RTC second to 01
	IODIR0|=(1<<7)|(1<<5)|(1<<0);            // Configure P0.0, P0.5, and P0.7 as GPIO outputs
	
	while(1)                                 // Infinite super-loop
		{
			rtc_lcd();                       // Display live HH:MM:SS time on LCD Line 1
			IOPIN0^=(1<<5);                  // Toggle P0.5 on every loop iteration (heartbeat/debug pin)
			flag=edits(flag,&num,&last);     // Check if interrupt occurred and handle time editing
			led_status();                    // Read ADC sensor and update LED state based on time/light
		}	
}

// Controls an active-low LED on P0.0 based on time of day (night) or ambient light voltage
void led_status(void)
	{	
		float adc_val=adc();                 // Read sensor voltage from ADC0 and display on LCD
		// Turn LED ON if time is after 5 PM (>17), before 7 AM (<7), or sensor voltage < 1.5V (dark)
		if((HOUR>17) || (HOUR<7) || (adc_val<1.5)){
				IOCLR0=(1<<0);               // Clear P0.0 (LOW) to turn ON active-low LED
				CmdLCD(GOTO_LINE2_POS0);     // Move LCD cursor to Line 2, Column 0
				StrLCD("LED:ON ");           // Display "LED:ON " status on LCD
			}
			else{
				IOSET0=(1<<0);               // Set P0.0 (HIGH) to turn OFF active-low LED
				CmdLCD(GOTO_LINE2_POS0);     // Move LCD cursor to Line 2, Column 0
				StrLCD("LED:OFF");          // Display "LED:OFF" status on LCD
			}
	}

// Interrupt Service Routine executed when a falling edge occurs on EINT0 (P0.1)
void eint0_fiq_isr(void) __irq
	{
	CmdLCD(CLEAR_LCD);                       // Clear the LCD screen
	StrLCD("Interupt Occured");              // Display interrupt notification message
	delay_ms(500);                           // Hold message on screen for 500ms
	IOPIN0^=1<<7;                            // Toggle indicator LED on P0.7
	CmdLCD(CLEAR_LCD);                       // Clear the LCD screen
	flag=(flag)?0:1;                         // Toggle global 'flag' between 0 and 1
	EXTINT=1<<0;                             // Clear the EINT0 peripheral interrupt flag
	VICVectAddr=0;                           // Acknowledge interrupt completion to the VIC
}

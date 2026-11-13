#include <LPC214x.h>        // Include LPC214x register definitions
#include "lcd.h"            // Include LCD function prototypes
#include "lcd_definesh.h"   // Include LCD command macros
#include "kpm.h"            // Include keypad function prototypes
#include "delay.h"          // Include delay function prototypes
#include "adc.h"            // Include ADC function prototypes
#include "rtc.h"            // Include RTC function prototypes

// Initializes the LPC214x internal Real-Time Clock (RTC) using PCLK
void init_rtc(void){
	
	CCR=1<<1;               // Set CTCRST (bit 1) to reset the RTC clock tick counter
	CCR=1<<0;               // Set CLKEN (bit 0) to enable the RTC time counters (clears reset)
	PREINT=456;             // Set integer prescaler for 15MHz PCLK: int(15,000,000 / 32768) - 1 = 456
	PREFRAC=25024;          // Set fractional prescaler: 15,000,000 - ((456 + 1) * 32768) = 25024
}

// Formats and displays the current RTC time (HH:MM:SS) on LCD Line 1
void rtc_lcd(){
	CmdLCD(GOTO_LINE1_POS0);      // Move LCD cursor to Line 1, Column 0
	CharLCD((HOUR/10)+48);        // Extract tens digit of HOUR, convert to ASCII, and display
	CharLCD((HOUR%10)+48);        // Extract units digit of HOUR, convert to ASCII, and display
	CharLCD(':');                 // Display colon separator
	CharLCD((MIN/10)+48);         // Extract tens digit of MIN, convert to ASCII, and display
	CharLCD((MIN%10)+48);         // Extract units digit of MIN, convert to ASCII, and display
	CharLCD(':');                 // Display colon separator
	CharLCD((SEC/10)+48);         // Extract tens digit of SEC, convert to ASCII, and display
	CharLCD((SEC%10)+48);         // Extract units digit of SEC, convert to ASCII, and display
}

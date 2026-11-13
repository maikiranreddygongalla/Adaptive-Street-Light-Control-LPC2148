#include <LPC21xx.h>         // Include LPC21xx register definitions
#include "lcd_definesh.h"    // Include LCD command and pin macros
#include "defines.h"         // Include bit manipulation macros
#include "types.h"           // Include custom data types
#include "delay.h"           // Include delay function prototypes

// Sends a single byte (either command or data) to the LCD data bus
void WriteLCD(u8 byte)
{
	IOCLR0=1<<LCD_RW;                 // Clear RW pin (P0.17 = 0) to select LCD write mode
	WRITEBYTE(IOPIN0,LCD_DATA,byte);  // Place the 8-bit byte onto pins P0.8 through P0.15
	IOSET0=1<<LCD_EN;                 // Set EN pin HIGH (P0.18 = 1) to start latch pulse
	delay_us(1);                      // Wait 1 microsecond for enable pulse width
	IOCLR0=1<<LCD_EN;                 // Clear EN pin LOW (P0.18 = 0) to latch data on falling edge
	delay_ms(2);                      // Wait 2 milliseconds for LCD internal execution
}

// Sends a control command to the LCD
void CmdLCD(u8 cmd)
{
	IOCLR0=1<<LCD_RS;                 // Clear RS pin (P0.16 = 0) to select Command Register
	WriteLCD(cmd);                    // Write the command byte to the LCD
}

// Initializes the HD44780 LCD module in 8-bit, 2-line mode
void InitLCD(void)
{
	// Configure P0.8-P0.15 (Data) and P0.16-P0.18 (RS, RW, EN) as GPIO outputs
	IODIR0|=(0xFF<<LCD_DATA)|(1<<LCD_RS)|
	        (1<<LCD_RW)|(1<<LCD_EN);
	delay_ms(15);                     // Wait 15ms after power-on for LCD voltage stabilization
	CmdLCD(0x30);                     // Send first 8-bit initialization wake-up command
	delay_ms(4);                      // Wait 4ms per HD44780 datasheet
	delay_us(100);                    // Wait an additional 100us (total > 4.1ms)
	CmdLCD(0x30);                     // Send second 8-bit initialization command
	delay_us(100);                    // Wait 100us per HD44780 datasheet
	CmdLCD(0x30);                     // Send third 8-bit initialization command
	CmdLCD(MODE_8BIT_2LINE);          // Configure LCD for 8-bit interface and 2 display lines
	CmdLCD(DSP_ON_CUR_OFF);           // Turn display ON and hide the cursor
	CmdLCD(CLEAR_LCD);                // Clear any garbage characters from the display
	CmdLCD(SHIFT_CUR_RIGHT);          // Set cursor to automatically move right after each character
}

// Displays a single ASCII character on the LCD
void CharLCD(u8 asciiVal)
{
	IOSET0=1<<LCD_RS;                 // Set RS pin (P0.16 = 1) to select Data Register
	WriteLCD(asciiVal);               // Write ASCII value to DDRAM
}

// Displays a null-terminated string on the LCD
void StrLCD(s8 *s)
{
	while(*s)                         // Loop until the null terminator ('\0') is reached
		CharLCD(*s++);                // Display current character and increment string pointer
}

// Converts and displays an unsigned 32-bit integer on the LCD
void U32LCD(u32 n)
{
	s32 i=0;	u8 a[10];             // Declare loop index and array to hold up to 10 ASCII digits
	if(n==0)                          // Check if the number is zero
	{
		CharLCD('0');                 // Directly print '0'
	}
  else
	{
		while(n>0)                    // Extract digits from least significant to most significant
		{
			a[i++]=(n%10)+48;         // Convert remainder digit to ASCII (+48) and store in array
			n/=10;                    // Remove the last digit by integer division
		}
		for(--i;i>=0;i--)             // Print digits in reverse order (most significant first)
		{
			CharLCD(a[i]);            // Display each ASCII digit on the LCD
		}
	}		
}

// Converts and displays a signed 32-bit integer on the LCD
void S32LCD(s32 n)
{
	if(n<0)                           // Check if the integer is negative
	{
		CharLCD('-');                 // Display a minus sign
		n=-n;                         // Convert number to positive magnitude
	}
	U32LCD(n);                        // Print the integer magnitude using U32LCD
}

// Displays a floating-point number with 'nDP' decimal places on the LCD
void F32LCD(f32 fn,u8 nDP)
{
   u32 n,i;                           // Declare integer part holder 'n' and loop counter 'i'
	 if(fn<0.0)                       // Check if the float value is negative
	 {	 
		 CharLCD('-');                // Display a minus sign
		 fn=-fn;                      // Make the float value positive
	 }	 
   n=fn;                              // Extract the integer part by truncating decimals
   U32LCD(n);                         // Display the integer part on the LCD
   CharLCD('.');                      // Display the decimal point
	 for(i=0;i<nDP;i++)               // Loop for the specified number of decimal places
	 {
		fn=(fn-n) * 10;               // Subtract integer part and shift next decimal digit left
		n=fn;                         // Extract the shifted digit as an integer
    CharLCD(n+48);		              // Convert digit to ASCII and display it
	 }		 
}

// Loads custom character bitmaps into LCD CGRAM
void BuildCGRAM(u8 *p,u8 nBytes)
{
	u32 i;                            // Declare loop index
	CmdLCD(GOTO_CGRAM_START);         // Point LCD address counter to start of CGRAM (0x40)
	IOCLR0=1<<LCD_RW;                 // Select write mode (RW = 0)
	IOSET0=1<<LCD_RS;                 // Select data register (RS = 1)
	for(i=0;i<nBytes;i++)             // Loop through all pattern bytes
	{
		WriteLCD(p[i]);               // Write each row byte of the custom character to CGRAM
	}
	CmdLCD(GOTO_LINE1_POS0);          // Switch address counter back to DDRAM (Line 1, Pos 0)
}

#include <LPC21xx.h>         // Include LPC21xx register definitions
#include "types.h"           // Include custom data types
#include "defines.h"         // Include bit manipulation macros
#include "kpm_defines.h"     // Include keypad row/column pin definitions
#include "lcd.h"             // Include LCD function prototypes
#include "lcd_definesh.h"    // Include LCD command macros

// 4x4 Keypad Look-Up Table mapping row/column indices to ASCII characters
u8 kpmLUT[4][4]=
{
	{'7','8','9','a'},       // Row 0 keys
	{'4','5','6','b'},       // Row 1 keys
	{'1','2','3','c'},       // Row 2 keys
	{'*','0','#','d'}        // Row 3 keys
};

// Initializes the 4x4 keypad GPIO pins on Port 1
void InitKPM(void)
{
  // Configure Row pins (P1.16 to P1.19) as outputs (default output state is LOW/0)
  SETBIT(IODIR1,ROW0);
	SETBIT(IODIR1,ROW1);
	SETBIT(IODIR1,ROW2);
	SETBIT(IODIR1,ROW3);
	// Column pins (P1.20 to P1.23) remain inputs by default (pulled HIGH externally/internally)
}	

// Scans all 4 column pins; returns 1 if no key is pressed, 0 if any key is pressed
u8 ColScan(void)
{
	u8 t;
	// Bitwise AND all 4 column states; if any column is 0 (pressed), result is 0
	t=(READBIT(IOPIN1,COL0)&READBIT(IOPIN1,COL1)&READBIT(IOPIN1,COL2)&READBIT(IOPIN1,COL3));
	return t; 
}

// Identifies which row (0-3) contains the pressed key by grounding one row at a time
u8 RowCheck(void)
{
	u8 r;
	
	// Ground Row 0 (LOW) and set Rows 1, 2, 3 HIGH
	IOCLR1 = (1 << ROW0);
	IOSET1 = (1 << ROW1) | (1 << ROW2) | (1 << ROW3);
	if(ColScan() == 0) { r = 0; goto last; } // If a column reads LOW, key is in Row 0
	
	// Ground Row 1 (LOW) and set Rows 0, 2, 3 HIGH
	IOSET1 = (1 << ROW0);
	IOCLR1 = (1 << ROW1);
	IOSET1 = (1 << ROW2) | (1 << ROW3);
	if(ColScan() == 0) { r = 1; goto last; } // If a column reads LOW, key is in Row 1
	
	// Ground Row 2 (LOW) and set Rows 0, 1, 3 HIGH
	IOSET1 = (1 << ROW0) | (1 << ROW1);
	IOCLR1 = (1 << ROW2);
	IOSET1 = (1 << ROW3);
	if(ColScan() == 0) { r = 2; goto last; } // If a column reads LOW, key is in Row 2
	
	// Ground Row 3 (LOW) and set Rows 0, 1, 2 HIGH
	IOSET1 = (1 << ROW0) | (1 << ROW1) | (1 << ROW2);
	IOCLR1 = (1 << ROW3);
	if(ColScan() == 0) { r = 3; }            // If a column reads LOW, key is in Row 3

last:
	// Reset all rows back to LOW (grounded) for normal idle scanning
	IOCLR1 = (1 << ROW0) | (1 << ROW1) | (1 << ROW2) | (1 << ROW3);
	return r;                                // Return detected row index (0-3)
}

// Identifies which column (0-3) is pulled LOW by the pressed key
u8 ColCheck(void)
{
	u8 c;
	
	if(READBIT(IOPIN1,COL0)==0)              // Check if Column 0 is LOW
		c=0;
	else if(READBIT(IOPIN1,COL1)==0)         // Check if Column 1 is LOW
		c=1;
	else if(READBIT(IOPIN1,COL2)==0)         // Check if Column 2 is LOW
		c=2;
	else if(READBIT(IOPIN1,COL3)==0)         // Check if Column 3 is LOW
		c=3;
	
	return c;                                // Return detected column index (0-3)
}

// Blocking function that waits for a key press and returns its corresponding ASCII character
u8 KeyScan(void)
{
	 u8 r,c,keyV;	 
   while(ColScan());         // Wait in loop while all columns are HIGH (no key pressed)
   r=RowCheck();             // Identify the active row index
   c=ColCheck();             // Identify the active column index
   keyV=kpmLUT[r][c];        // Fetch the corresponding character from the look-up table
	
   return keyV;	             // Return the pressed key character
}

// Reads a single numeric key press from the keypad, displays it, and returns its integer value
u32 ReadNum(void)
{
	u32 keyV,sum=0;	
	while(1)
	{
		keyV=KeyScan();                      // Wait for and capture a key press
		if((keyV>='0') && (keyV<='9'))       // Check if the pressed key is a digit ('0'-'9')
		{
			sum=(sum*10)+(keyV-'0');         // Convert ASCII digit to integer value
			CmdLCD(GOTO_LINE2_POS0);         // Move LCD cursor to Line 2, Position 0
			U32LCD(sum);                     // Display the entered number on the LCD
			while(ColScan()==0);             // Wait until the user releases the key
			if (sum == 'd')                  // Check if sum equals ASCII 'd' (100)
				return 0;
			return sum;                      // Return the single digit immediately
		}
		else                                 // If a non-numeric key is pressed
		{
			if(keyV=='C')                    // If key is 'C', clear the LCD
			{
				CmdLCD(CLEAR_LCD);
			}
			while(ColScan()==0);             // Wait until the user releases the key
			break;                           // Exit the loop
		}
	}
	return sum;                              // Return 0 if non-numeric key was pressed
}

// Reads up to a 2-digit number from the keypad and stores it via pointer *num
void ReadNum2(u32 *num,u8 *lastKey)
{
	u32 count=0;                             // Counter to track how many digits have been entered
	while(1)
	{
		if(count>1)                          // Stop reading once 2 digits (count 0 and 1) are entered
			break;
		*lastKey=KeyScan();                  // Wait for and capture the next key press
		if((*lastKey>='0')&&(*lastKey<='9')) // Check if the key is a numeric digit ('0'-'9')
		{
			count++;                         // Increment digit counter
			*num=(*num * 10)+(*lastKey-'0'); // Shift existing number left and add new digit
			CmdLCD(GOTO_LINE2_POS0);         // Move LCD cursor to Line 2, Position 0
			U32LCD(*num);                    // Display updated number on the LCD
			while(ColScan()==0);             // Wait until the key is released
		}
		else                                 // If a non-numeric key is pressed (e.g., Enter/Cancel)
		{
			if(*lastKey=='C')                // If key is 'C', clear the LCD
			{
				CmdLCD(CLEAR_LCD);
			}
			while(ColScan()==0);             // Wait until the key is released
			break;                           // Exit loop early (allows single-digit entry)
		}
	}
}

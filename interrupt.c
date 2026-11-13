#include <LPC214x.h>        // Include LPC214x register definitions
#include "lcd.h"            // Include LCD function prototypes
#include "lcd_definesh.h"   // Include LCD command macros
#include "kpm.h"            // Include keypad function prototypes
#include "delay.h"          // Include delay function prototypes
#include "adc.h"            // Include ADC function prototypes
#include "rtc.h"            // Include RTC function prototypes
#include "interrupt.h"      // Include interrupt and edit function prototypes

// Configures Pin P0.1 as EINT0 and enables Vectored IRQ Slot 0
void init_eint(void)
{
    PINSEL0 &= ~(3 << 2);          // Clear bits 3:2 in PINSEL0 for pin P0.1
    PINSEL0 |= (3 << 2);           // Set bits 3:2 to 11 to configure P0.1 as EINT0

    VICIntSelect &= ~(1 << 14);    // Configure interrupt channel 14 (EINT0) as IRQ (not FIQ)

    VICVectCntl0 = (1 << 5) | 14;  // Enable Vectored IRQ slot 0 (bit 5) and assign EINT0 (14)

    VICIntEnable = (1 << 14);      // Enable EINT0 interrupt in the Vectored Interrupt Controller

    EXTINT = (1 << 0);             // Clear any pending EINT0 interrupt flag
    EXTMODE = (1 << 0);            // Configure EINT0 as edge-sensitive (bit 0 = 1)
    EXTPOLAR = 0;                  // Configure EINT0 to trigger on falling edge (bit 0 = 0)
}

// Displays a menu to edit RTC Hours, Minutes, or Seconds when triggered by interrupt flag
unsigned int edits(unsigned int flag, unsigned int *num,
                   unsigned char *last)
{
    unsigned int temp, op;         // 'temp' backs up current RTC value; 'op' holds menu choice

    if(flag == 1)                  // Execute edit menu only if EINT0 ISR set flag to 1
    {
        CmdLCD(CLEAR_LCD);         // Clear the LCD screen
        StrLCD("1.HH 2.MM 3.SS");  // Prompt user to choose Hours (1), Minutes (2), or Seconds (3)

        op = ReadNum();            // Read menu selection digit from keypad
        delay_ms(300);             // Short debounce/visual delay

        if(op == 0)                // If 0 or invalid key was pressed, exit edit mode
            flag = 0;

        CmdLCD(CLEAR_LCD);         // Clear LCD before prompting for new value

        if(op == 1)                // Option 1: Edit RTC Hours
        {
            temp = HOUR;           // Save current HOUR register value in case input is invalid
            StrLCD("Enter hours"); // Prompt user to enter new hours

            ReadNum2(num, last);   // Read up to 2 digits from keypad into *num

            if(*num <= 23)         // Validate that entered hours are within 0 to 23
                HOUR = *num;       // Update RTC HOUR register
            else
                HOUR = temp;       // Restore previous HOUR value if input > 23

            *num = 0;              // Reset input accumulator variable to 0
            flag = 0;              // Clear edit flag to return to normal display mode
            CmdLCD(CLEAR_LCD);     // Clear the LCD
        }

        else if(op == 2)           // Option 2: Edit RTC Minutes
        {
            temp = MIN;            // Save current MIN register value
            StrLCD("Enter Minutes");// Prompt user to enter new minutes

            ReadNum2(num, last);   // Read up to 2 digits from keypad into *num

            if(*num <= 59)         // Validate that entered minutes are within 0 to 59
                MIN = *num;        // Update RTC MIN register
            else
                MIN = temp;        // Restore previous MIN value if input > 59

            *num = 0;              // Reset input accumulator variable to 0
            flag = 0;              // Clear edit flag to return to normal display mode
            CmdLCD(CLEAR_LCD);     // Clear the LCD
        }

        else if(op == 3)           // Option 3: Edit RTC Seconds
        {
            temp = SEC;            // Save current SEC register value
            StrLCD("Enter Seconds");// Prompt user to enter new seconds

            ReadNum2(num, last);   // Read up to 2 digits from keypad into *num

            if(*num <= 59)         // Validate that entered seconds are within 0 to 59
                SEC = *num;        // Update RTC SEC register
            else
                SEC = temp;        // Restore previous SEC value if input > 59

            *num = 0;              // Reset input accumulator variable to 0
            flag = 0;              // Clear edit flag to return to normal display mode
            CmdLCD(CLEAR_LCD);     // Clear the LCD
        }
    }

    return flag;                   // Return updated flag state to main loop
}

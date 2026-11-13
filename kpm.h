#include "types.h"                // Include custom data types

void InitKPM(void);               // Prototype to initialize keypad GPIO pins
u8   ColScan(void);               // Prototype to check if any column line is pulled LOW
u8   RowCheck(void);              // Prototype to identify which row has a pressed key
u8   ColCheck(void);              // Prototype to identify which column has a pressed key
u8   KeyScan(void);               // Prototype to wait for a keypress and return its mapped character
u32  ReadNum(void);               // Prototype to read a single-digit numeric selection
void ReadNum2(u32 *,u8 *);        // Prototype to read up to a 2-digit number from the keypad

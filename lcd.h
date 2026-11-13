#include "types.h"                          // Include custom data type definitions
void WriteLCD(u8 byte);                     // Prototype to send raw byte (command/data) to LCD
void CmdLCD(u8 cmd);                        // Prototype to send command byte to LCD
void InitLCD(void);                         // Prototype to initialize LCD in 8-bit mode
void CharLCD(u8 asciiVal);                  // Prototype to display a single ASCII character
void StrLCD(s8 *);                          // Prototype to display a null-terminated string
void U32LCD(u32);                           // Prototype to display an unsigned 32-bit integer
void S32LCD(s32);                           // Prototype to display a signed 32-bit integer
void F32LCD(f32,u8 nDP);                    // Prototype to display a float up to nDP decimal places
void BuildCGRAM(const u8 *p,u8 nBytes);     // Prototype to store custom character patterns in CGRAM

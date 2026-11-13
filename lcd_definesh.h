// HD44780 LCD Controller Commands
#define CLEAR_LCD            0x01  // Clear entire display and reset cursor to home
#define RET_CUR_HOME         0x02  // Return cursor to home position (0,0)
#define SHIFT_CUR_RIGHT      0x06  // Auto-increment cursor position to the right after write
#define SHIFT_CUR_LEFT       0x07  // Auto-decrement cursor position to the left after write
#define DSP_OFF              0x08  // Turn display off, cursor off
#define DSP_ON_CUR_OFF       0x0c  // Turn display on, cursor off
#define DSP_ON_CUR_ON        0x0E  // Turn display on, cursor on (underline)
#define DSP_ON_CUR_BLINK     0x0F  // Turn display on, cursor blinking
#define SHIFT_DISP_LEFT      0x10  // Shift entire display to the left
#define SHIFT_DISP_RIGHT     0x14  // Shift entire display to the right
#define MODE_8BIT_1LINE      0x30  // Configure 8-bit data interface, 1 display line
#define MODE_4BIT_1LINE      0x20  // Configure 4-bit data interface, 1 display line
#define MODE_8BIT_2LINE      0x38  // Configure 8-bit data interface, 2 display lines (5x7 dots)
#define MODE_4BIT_2LINE      0x28  // Configure 4-bit data interface, 2 display lines (5x7 dots)
#define GOTO_LINE1_POS0      0x80  // Set DDRAM address to Line 1, Column 0
#define GOTO_LINE2_POS0      0xC0  // Set DDRAM address to Line 2, Column 0
#define GOTO_LINE3_POS0      0x94  // Set DDRAM address to Line 3, Column 0 (for 20x4 LCD)
#define GOTO_LINE4_POS0      0xD4  // Set DDRAM address to Line 4, Column 0 (for 20x4 LCD)
#define GOTO_CGRAM_START     0x40  // Set CGRAM address to 0x00 for custom characters

// LCD Hardware Pin Mapping on Port 0
#define LCD_DATA 8                 // Starting bit for 8-bit data bus (P0.8 to P0.15)
#define LCD_RS   16                // Register Select pin connected to P0.16 (0=Cmd, 1=Data)
#define LCD_EN   18                // Enable pin connected to P0.18 (High-to-Low latch)
#define LCD_RW   17                // Read/Write pin connected to P0.17 (0=Write, 1=Read)

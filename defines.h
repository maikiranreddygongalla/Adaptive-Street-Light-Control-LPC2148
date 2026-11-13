// Macro to write an 8-bit BYTE into a 32-bit WORD starting at bit position STARTBIT
#define WRITEBYTE(WORD,STARTBIT,BYTE) WORD=((WORD&~(0xFF<<STARTBIT))| (BYTE<<STARTBIT))

// Macro to read a single bit (0 or 1) at position BIT from WORD
#define READBIT(WORD,BIT) ((WORD>>BIT)&1)

// Macro to set (write 1 to) a specific BIT in WORD
#define SETBIT(WORD,BIT) (WORD|=(1<<BIT))

// Macro to clear (write 0 to) a specific BIT in WORD
#define CLRBIT(WORD,BIT) (WORD&=~(1<<BIT))



#ifndef LCDLIB_H
#define LCDLIB_H

#include <pic16f877a.h>

// Pin assignment Macros

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7
// Prototypes

void Lcd_SetBit(char data_bit);
void Lcd_Cmd(char a);
void Lcd_Clear(void);
void Lcd_Set_Cursor(char a, char b);
void Lcd_Start();
void Lcd_Print_Char(char data);
void Lcd_Print_String(char *a);


#endif
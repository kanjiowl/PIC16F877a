#define _XTAL_FREQ 20000000

#define RS RD2
#define EN RD3
#define D4 RD4
#define D5 RD5
#define D6 RD6
#define D7 RD7

#include <xc.h>

#pragma config FOSC = HS        // Oscillator Selection bits (HS oscillator)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable bit (PWRT enabled)
#pragma config BOREN = ON       // Brown-out Reset Enable bit (BOR enabled)
#pragma config LVP = OFF        // Low-Voltage (Single-Supply) In-Circuit Serial Programming Enable bit (RB3 is digital I/O, HV on MCLR must be used for programming)
#pragma config CPD = OFF        // Data EEPROM Memory Code Protection bit (Data EEPROM code protection off)
#pragma config WRT = OFF        // Flash Program Memory Write Enable bits (Write protection off; all program memory may be written to by EECON control)
#pragma config CP = OFF         // Flash Program Memory Code Protection bit (Code protection off)



#define THRES 400  // Sensor trigger threshold

// ADC Macros 
#define SBIT_ADON     0
#define SBIT_CHS0     3
#define SBIT_ADFM     7


//LCD Functions Developed by Circuit Digest.
void Lcd_SetBit(char data_bit) //Based on the Hex value Set the Bits of the Data Lines
{
    if(data_bit& 1) 
        D4 = 1;
    else
        D4 = 0;

    if(data_bit& 2)
        D5 = 1;
    else
        D5 = 0;

    if(data_bit& 4)
        D6 = 1;
    else
        D6 = 0;

    if(data_bit& 8) 
        D7 = 1;
    else
        D7 = 0;
}

void Lcd_Cmd(char a)
{
    RS = 0;           
    Lcd_SetBit(a); //Incoming Hex value
    EN  = 1;         
        __delay_ms(4);
        EN  = 0;         
}

Lcd_Clear()
{
    Lcd_Cmd(0); //Clear the LCD
    Lcd_Cmd(1); //Move the curser to first position
}

void Lcd_Set_Cursor(char a, char b)
{
    char temp,z,y;
    if(a== 1)
    {
      temp = 0x80 + b - 1; //80H is used to move the curser
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
    else if(a== 2)
    {
        temp = 0xC0 + b - 1;
        z = temp>>4; //Lower 8-bits
        y = temp & 0x0F; //Upper 8-bits
        Lcd_Cmd(z); //Set Row
        Lcd_Cmd(y); //Set Column
    }
}

void Lcd_Start()
{
  Lcd_SetBit(0x00);
  for(int i=1065244; i<=0; i--)  NOP();  
  Lcd_Cmd(0x03);
    __delay_ms(5);
  Lcd_Cmd(0x03);
    __delay_ms(11);
  Lcd_Cmd(0x03); 
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x02); //02H is used for Return home -> Clears the RAM and initializes the LCD
  Lcd_Cmd(0x08); //Select Row 1
  Lcd_Cmd(0x00); //Clear Row 1 Display
  Lcd_Cmd(0x0C); //Select Row 2
  Lcd_Cmd(0x00); //Clear Row 2 Display
  Lcd_Cmd(0x06);
}

void Lcd_Print_Char(char data)  //Send 8-bits through 4-bit mode
{
   char Lower_Nibble,Upper_Nibble;
   Lower_Nibble = data&0x0F;
   Upper_Nibble = data&0xF0;
   RS = 1;             // => RS = 1
   Lcd_SetBit(Upper_Nibble>>4);             //Send upper half by shifting by 4
   EN = 1;
   for(int i=2130483; i<=0; i--)  NOP(); 
   EN = 0;
   Lcd_SetBit(Lower_Nibble); //Send Lower half
   EN = 1;
   for(int i=2130483; i<=0; i--)  NOP();
   EN = 0;
}

void Lcd_Print_String(char *a)
{
    int i;
    for(i=0;a[i]!='\0';i++)
       Lcd_Print_Char(a[i]);  //Split the string using pointers and call the Char function 
}
///////////////////////////////////////////////////////////////////////////////////////////////////
unsigned int parked  = 0;
int adc_value = 0;

// Convert int to str for LCD view
char* intToStr(int adcValue)
{
    char str[4] = {'0','0' ,'0' , '0'}; 
    int i = 3;
    
    for(int i = 3 ; adcValue != 0; --i)
    {
        int val  = adcValue % 10;
        str[i] = val+ 48;
        adcValue /= 10;
    }
    return &(str[0]);
}

// Init ADC 
void ADC_Init()
{
  ADCON0 = 0b01000001; //ADC ON and Fosc/16 is selected
  ADCON1 = 0b11000000; // Internal reference voltage is selected
}


// Read ADC

int ADC_Read(unsigned char channel)
 {  
   if(channel > 7) //If Invalid channel selected 
    return 0;     //Return 0

  ADCON0 &= 0xC5; //Clearing the Channel Selection Bits
  ADCON0 |= channel<<3; //Setting the required Bits
  __delay_ms(2); //Acquisition time to charge hold capacitor
  GO_nDONE = 1; //Initializes A/D Conversion
  while(GO_nDONE); //Wait for A/D Conversion to complete
  return ((ADRESH<<8)+ADRESL); //Returns Result
}


// Check if  a spot is filled
int spotFilled(int spot )
{
    adc_value = ADC_Read(spot);
    if (adc_value > THRES) {
        return 1;
    }
    return 0;
}

// Count parked cars
void countParked(void)
{
    parked  = 0;
    if (spotFilled(0)) ++parked;
//    __delay_ms(100);
    if (spotFilled(1)) ++parked;
//    __delay_ms(100);
    if (spotFilled(2)) ++parked;
//    __delay_ms(100);
    if (spotFilled(3)) ++parked;
//    __delay_ms(100);
}

 
///////////////////////////////////////////////////////////////////////////////
int main()
{
    unsigned int a;
    TRISD = 0x00;
    TRISB = 0x00;
    Lcd_Start();
    ADC_Init(); 

    while(1)
    {
        PORTB = 0xff;
        Lcd_Clear();
        Lcd_Set_Cursor(1,1);
        Lcd_Print_String("Parked");
        Lcd_Set_Cursor(1, 8);
        countParked(); 
        Lcd_Print_Char(48+parked);
        //
//         Lcd_Set_Cursor(2,1);
//        Lcd_Print_String("Vacant");
//        Lcd_Set_Cursor(2, 8); 
//        Lcd_Print_Char(48+(4-parked));
        __delay_ms(1000);
    }
    return 0;
}

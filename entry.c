#include <xc.h>
#include "config.h"
#include "lcdlib.h"
#include "ADC.h"

#define THRES 400  // Sensor trigger threshold

unsigned int parked = 0;
int adc_value = 0;
int p1 = 0, p2 = 0, p3 = 0, p4 = 0;

// Convert int to str for LCD view
// Don't need it anymore
/*
char* intToStr(int adcValue) {
    char str[4] = {'0', '0', '0', '0'};

    for (int i = 3; adcValue != 0; --i) {
        int val = adcValue % 10;
        str[i] = val + 48;
        adcValue /= 10;
    }
    return &(str[0]);
}
*/

// Check if  a spot is filled
int spotFilled(int spot) {
    adc_value = ADC_Read(spot);
    if (adc_value > THRES) {
        if (spot == 0) p1 = 1;
        if (spot == 1) p2 = 1;
        if (spot == 2) p3 = 1;
        if (spot == 3) p4 = 1;
        return 1;
    }
    return 0;
}

// Count parked cars

void countParked(void) {
    p1 = 0;
    p2 = 0;
    p3 = 0;
    p4 = 0;
    parked = 0;
    if (spotFilled(0)) ++parked;
    if (spotFilled(1)) ++parked;
    if (spotFilled(2)) ++parked;
    if (spotFilled(3)) ++parked;
}


int main() {
    unsigned int a;
    TRISD = 0x00;
    TRISB = 0x00;
    Lcd_Start();
    ADC_Init();

    while (1) {

        PORTB = 0xff;

        Lcd_Clear();
        countParked();
        if (parked == 4) {
            Lcd_Set_Cursor(1, 1);
            Lcd_Print_String("No vacant space!");

            Lcd_Set_Cursor(2, 1);
            Lcd_Print_String("Come Back later!");
        }
        else if (parked == 0) {
            Lcd_Set_Cursor(1, 1);
            Lcd_Print_String("Welcome to ");
            Lcd_Set_Cursor(1, 1);
            Lcd_Print_String("401 parking lot!");
        } else {
            Lcd_Set_Cursor(1, 1);
            Lcd_Print_String("1");
            Lcd_Set_Cursor(1, 3);
            if (p1 == 1) Lcd_Print_String("FULL");
            else Lcd_Print_String("OPEN");

            Lcd_Set_Cursor(1, 10);
            Lcd_Print_String("2");
            Lcd_Set_Cursor(1, 12);
            if (p2 == 1) Lcd_Print_String("FULL");
            else Lcd_Print_String("OPEN");

            Lcd_Set_Cursor(2, 1);
            Lcd_Print_String("3");
            Lcd_Set_Cursor(2, 3);
            if (p3 == 1) Lcd_Print_String("FULL");
            else Lcd_Print_String("OPEN");

            Lcd_Set_Cursor(2, 10);
            Lcd_Print_String("4");
            Lcd_Set_Cursor(2, 12);
            if (p4 == 1) Lcd_Print_String("FULL");
            else Lcd_Print_String("OPEN");

        }
        __delay_ms(1000);
    }
    return 0;
}

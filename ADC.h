

#ifndef ADC_H 
#define ADC_H 
// ADC Macros 
#define SBIT_ADON     0
#define SBIT_CHS0     3
#define SBIT_ADFM     7

// Prototypes
void ADC_Init();
int ADC_Read(unsigned char channel);


#endif
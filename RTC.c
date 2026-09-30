#include "I2C_H.h"
#include "RTC_H.h"
#define RTC 0x68
static unsigned char b2d(unsigned char x){return (x>>4)*10+(x&15);}static unsigned char d2b(unsigned char x){return ((x/10)<<4)|(x%10);}
unsigned char RTC_Init(void){return 1;}
unsigned char RTC_SetTime(unsigned char h,unsigned char m,unsigned char s){if(!I2C_Start())return 0;if(!I2C_Write(RTC<<1)){I2C_Stop();return 0;}I2C_Write(0);I2C_Write(d2b(s));I2C_Write(d2b(m));I2C_Write(d2b(h));I2C_Stop();return 1;}
unsigned char RTC_GetTime(RTC_Time*t){if(!I2C_Start())return 0;if(!I2C_Write(RTC<<1)){I2C_Stop();return 0;}I2C_Write(0);if(!I2C_Start())return 0;if(!I2C_Write((RTC<<1)|1)){I2C_Stop();return 0;}t->second=b2d(I2C_Read_ACK());t->minute=b2d(I2C_Read_ACK());t->hour=b2d(I2C_Read_NACK());I2C_Stop();return 1;}

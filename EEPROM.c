#define F_CPU 16000000UL
#include <util/delay.h>
#include "I2C_H.h"
#include "EEPROM_H.h"
#include "UART_H.h"
#define EE 0x50
#define SLOT 20
#define LOG 0x0100
#define LOGSIZE 8
void EEPROM_Init(void){}
unsigned char EEPROM_WriteByte(unsigned int a,unsigned char d){if(!I2C_Start())return 0;if(!I2C_Write(EE<<1)){I2C_Stop();return 0;}I2C_Write(a>>8);I2C_Write(a);I2C_Write(d);I2C_Stop();_delay_ms(5);return 1;}
unsigned char EEPROM_ReadByte(unsigned int a,unsigned char*d){if(!I2C_Start())return 0;if(!I2C_Write(EE<<1)){I2C_Stop();return 0;}I2C_Write(a>>8);I2C_Write(a);if(!I2C_Start())return 0;if(!I2C_Write((EE<<1)|1)){I2C_Stop();return 0;}*d=I2C_Read_NACK();I2C_Stop();return 1;}
unsigned char EEPROM_SaveCandidate(unsigned char slot,unsigned char*uid,const char*name){unsigned char i;unsigned int a=slot*SLOT;for(i=0;i<4;i++)if(!EEPROM_WriteByte(a+i,uid[i]))return 0;EEPROM_WriteByte(a+4,1);for(i=0;i<15;i++)EEPROM_WriteByte(a+5+i,name[i]);return 1;}
unsigned char EEPROM_FindCandidate(unsigned char*uid){unsigned char s,j,x,same;for(s=0;s<20;s++){unsigned int a=s*SLOT;if(!EEPROM_ReadByte(a+4,&x))return 255;if(x==1){same=1;for(j=0;j<4;j++){EEPROM_ReadByte(a+j,&x);if(x!=uid[j])same=0;}if(same)return s;}}return 255;}
unsigned char EEPROM_IsDuplicate(unsigned char*uid){unsigned char n,i,j,x;if(!EEPROM_ReadByte(LOG,&n))return 0;if(n==255)n=0;for(i=0;i<n;i++){unsigned int a=LOG+1+i*LOGSIZE;for(j=0;j<4;j++){EEPROM_ReadByte(a+j,&x);if(x!=uid[j])break;}if(j==4)return 1;}return 0;}
unsigned char EEPROM_SaveLog(unsigned char*uid,unsigned char h,unsigned char m,unsigned char s,unsigned char late){unsigned char n,i;if(!EEPROM_ReadByte(LOG,&n)||n==255)n=0;if(n>=50)return 0;{unsigned int a=LOG+1+n*LOGSIZE;for(i=0;i<4;i++)EEPROM_WriteByte(a+i,uid[i]);EEPROM_WriteByte(a+4,h);EEPROM_WriteByte(a+5,m);EEPROM_WriteByte(a+6,s);EEPROM_WriteByte(a+7,late);}EEPROM_WriteByte(LOG,n+1);return 1;}
void EEPROM_ListLogs(void){unsigned char n,i,j,x;char hex[]="0123456789ABCDEF";if(!EEPROM_ReadByte(LOG,&n))return;if(n==255)n=0;UART_SendString("LIST\r\n");for(i=0;i<n;i++){unsigned int a=LOG+1+i*LOGSIZE;UART_SendString("UID=");for(j=0;j<4;j++){EEPROM_ReadByte(a+j,&x);UART_SendChar(hex[x>>4]);UART_SendChar(hex[x&15]);}UART_SendString(" TIME=");for(j=4;j<=6;j++){EEPROM_ReadByte(a+j,&x);UART_SendChar(x/10+'0');UART_SendChar(x%10+'0');if(j<6)UART_SendChar(':');}EEPROM_ReadByte(a+7,&x);UART_SendString(x?" LATE\r\n":" OK\r\n");}}

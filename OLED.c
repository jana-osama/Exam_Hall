#include "I2C_H.h"
#include "OLED_H.h"
#define OLED 0x3C
static void cmd(unsigned char c){I2C_Start();I2C_Write(OLED<<1);I2C_Write(0);I2C_Write(c);I2C_Stop();}static void dat(unsigned char d){I2C_Start();I2C_Write(OLED<<1);I2C_Write(0x40);I2C_Write(d);I2C_Stop();}
void OLED_Init(void){cmd(0xAE);cmd(0xD5);cmd(0x80);cmd(0xA8);cmd(0x3F);cmd(0xD3);cmd(0);cmd(0x40);cmd(0x8D);cmd(0x14);cmd(0x20);cmd(0);cmd(0xA1);cmd(0xC8);cmd(0xDA);cmd(0x12);cmd(0x81);cmd(0x7F);cmd(0xD9);cmd(0xF1);cmd(0xDB);cmd(0x40);cmd(0xA4);cmd(0xA6);cmd(0xAF);OLED_Clear();}
void OLED_Clear(void){unsigned char p,c;for(p=0;p<8;p++){cmd(0xB0+p);cmd(0);cmd(0x10);for(c=0;c<128;c++)dat(0);}}
void OLED_SetCursor(unsigned char x,unsigned char p){cmd(0xB0+p);cmd(x&15);cmd(0x10|(x>>4));}
static const unsigned char f[10][5]={{62,81,73,69,62},{0,66,127,64,0},{66,97,81,73,70},{33,65,69,75,49},{24,20,18,127,16},{39,69,69,69,57},{60,74,73,73,48},{1,113,9,5,3},{54,73,73,73,54},{6,73,73,41,30}};
void OLED_WriteChar(char c){unsigned char i;if(c>='0'&&c<='9'){for(i=0;i<5;i++)dat(f[c-'0'][i]);dat(0);return;}if(c==':'){dat(0);dat(54);dat(54);dat(0);dat(0);dat(0);return;}for(i=0;i<6;i++)dat(0);}
void OLED_WriteString(const char*s){while(*s)OLED_WriteChar(*s++);}void OLED_WriteNumber(unsigned int n){char b[6];unsigned char i=0;if(n==0){OLED_WriteChar('0');return;}while(n){b[i++]=n%10+'0';n/=10;}while(i)OLED_WriteChar(b[--i]);}

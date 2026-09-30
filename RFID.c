#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "SPI_H.h"
#include "RFID_H.h"
#define SS PB2
#define RST PC0
#define CommandReg 0x01
#define ComIrqReg 0x04
#define ErrorReg 0x06
#define FIFODataReg 0x09
#define FIFOLevelReg 0x0A
#define BitFramingReg 0x0D
#define TxControlReg 0x14
#define ModeReg 0x11
#define TxASKReg 0x15
#define TModeReg 0x2A
#define TPrescalerReg 0x2B
#define TReloadH 0x2C
#define TReloadL 0x2D
static void wr(unsigned char r,unsigned char d){PORTB&=~(1<<SS);SPI_Transfer((r<<1)&0x7E);SPI_Transfer(d);PORTB|=1<<SS;}static unsigned char rd(unsigned char r){unsigned char d;PORTB&=~(1<<SS);SPI_Transfer(((r<<1)&0x7E)|0x80);d=SPI_Transfer(0);PORTB|=1<<SS;return d;}static void setb(unsigned char r,unsigned char m){wr(r,rd(r)|m);}
void RFID_Init(void){DDRC|=1<<RST;PORTC|=1<<RST;SPI_Init();PORTB|=1<<SS;_delay_ms(50);wr(TModeReg,0x8D);wr(TPrescalerReg,0x3E);wr(TReloadL,30);wr(TReloadH,0);wr(TxASKReg,0x40);wr(ModeReg,0x3D);setb(TxControlReg,3);}
static unsigned char trans(unsigned char*d,unsigned char n,unsigned char*b,unsigned char*bn){unsigned char i,q;wr(CommandReg,0);wr(ComIrqReg,0x7F);wr(FIFOLevelReg,0x80);for(i=0;i<n;i++)wr(FIFODataReg,d[i]);wr(CommandReg,0x0C);setb(BitFramingReg,0x80);for(i=0;i<200;i++){q=rd(ComIrqReg);if(q&0x30)break;}wr(BitFramingReg,rd(BitFramingReg)&0x7F);if(i==200||(rd(ErrorReg)&0x1B))return 0;q=rd(FIFOLevelReg);if(!q)return 0;if(q>18)q=18;*bn=q;for(i=0;i<q;i++)b[i]=rd(FIFODataReg);return 1;}
unsigned char RFID_CheckCard(unsigned char*uid){unsigned char r[10],n,d[2]={0x26};wr(BitFramingReg,7);if(!trans(d,1,r,&n))return 0;d[0]=0x93;d[1]=0x20;wr(BitFramingReg,0);if(!trans(d,2,r,&n)||n<5)return 0;uid[0]=r[0];uid[1]=r[1];uid[2]=r[2];uid[3]=r[3];return 1;}

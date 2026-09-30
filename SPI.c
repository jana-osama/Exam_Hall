#include <avr/io.h>
#include "SPI_H.h"
void SPI_Init(void){DDRB|=(1<<PB2)|(1<<PB3)|(1<<PB5);DDRB&=~(1<<PB4);PORTB|=(1<<PB2);SPCR=(1<<SPE)|(1<<MSTR)|(1<<SPR0);}
unsigned char SPI_Transfer(unsigned char d){SPDR=d;while(!(SPSR&(1<<SPIF)));return SPDR;}

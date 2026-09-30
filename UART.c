#include <avr/io.h>
#include "UART_H.h"
void UART_Init(void){UBRR0H=0;UBRR0L=103;UCSR0B=(1<<RXEN0)|(1<<TXEN0);UCSR0C=(1<<UCSZ01)|(1<<UCSZ00);}
void UART_SendChar(char c){while(!(UCSR0A&(1<<UDRE0)));UDR0=c;}
void UART_SendString(const char*s){while(*s)UART_SendChar(*s++);}
char UART_ReceiveChar(void){while(!(UCSR0A&(1<<RXC0)));return UDR0;}
unsigned char UART_Available(void){return (UCSR0A&(1<<RXC0))!=0;}

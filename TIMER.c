#include <avr/io.h>
#include <avr/interrupt.h>
#include "TIMER_H.h"
volatile unsigned char one_second=0;
void TIMER_Init(void){TCCR1A=0;TCCR1B=(1<<WGM12)|(1<<CS12);OCR1A=62499;TIMSK1=(1<<OCIE1A);sei();}
ISR(TIMER1_COMPA_vect){one_second=1;}
unsigned char TIMER_GetSecond(void){if(one_second){one_second=0;return 1;}return 0;}

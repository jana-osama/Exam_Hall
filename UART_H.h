#ifndef UART_H_H_
#define UART_H_H_
void UART_Init(void);void UART_SendChar(char c);void UART_SendString(const char*s);char UART_ReceiveChar(void);unsigned char UART_Available(void);
#endif

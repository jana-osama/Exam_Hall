#ifndef OLED_H_H_
#define OLED_H_H_
void OLED_Init(void);void OLED_Clear(void);void OLED_SetCursor(unsigned char x,unsigned char p);void OLED_WriteChar(char c);void OLED_WriteString(const char*s);void OLED_WriteNumber(unsigned int n);
#endif

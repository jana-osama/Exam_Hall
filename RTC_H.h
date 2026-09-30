#ifndef RTC_H_H_
#define RTC_H_H_
typedef struct{unsigned char hour,minute,second;}RTC_Time;
unsigned char RTC_Init(void);unsigned char RTC_SetTime(unsigned char h,unsigned char m,unsigned char s);unsigned char RTC_GetTime(RTC_Time*t);
#endif

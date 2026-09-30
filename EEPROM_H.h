#ifndef EEPROM_H_H_
#define EEPROM_H_H_
void EEPROM_Init(void);unsigned char EEPROM_ReadByte(unsigned int a,unsigned char*d);unsigned char EEPROM_WriteByte(unsigned int a,unsigned char d);unsigned char EEPROM_SaveCandidate(unsigned char slot,unsigned char*uid,const char*name);unsigned char EEPROM_FindCandidate(unsigned char*uid);unsigned char EEPROM_IsDuplicate(unsigned char*uid);unsigned char EEPROM_SaveLog(unsigned char*uid,unsigned char h,unsigned char m,unsigned char s,unsigned char late);void EEPROM_ListLogs(void);
#endif

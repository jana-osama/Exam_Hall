#ifndef I2C_H_H_
#define I2C_H_H_
unsigned char I2C_Init(void);
unsigned char I2C_Start(void);
void I2C_Stop(void);
unsigned char I2C_Write(unsigned char data);
unsigned char I2C_Read_ACK(void);
unsigned char I2C_Read_NACK(void);
#endif

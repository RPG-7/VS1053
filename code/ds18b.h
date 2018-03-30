#ifndef __DS18B_H_
#define __DS18B_H_

#define  dq1 GPIO_SetBits(GPIOA,GPIO_Pin_2)
#define  dq0 GPIO_ResetBits(GPIOA,GPIO_Pin_2)

void             delay_us(int time);
unsigned char    Ds18b20Init(void);
void             Ds18b20WriteByte(unsigned char com);
unsigned char    Ds18b20ReadByte(void);
void  		  Ds18b20ChangTemp(void);
void  		  Ds18b20ReadTempCom(void);
float      	  Ds18b20ReadTemp(void);
int 			  Temp(void);
#endif

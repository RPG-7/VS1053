#include "ds18b.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x.h"
void delay_us(int time)
{    
   int i=0;  
   while(time--)
   {
      i=10;  
      while(i--);    
   }
}

unsigned char Ds18b20Init(void)
{
	char i;
	
	dq1;		
	delay_us(50);
	
	dq0;	
	delay_us(500);	
	dq1;
	delay_us(40);
	
	i = 0;
	while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2))	
	{
		delay_us(1);
		i++;
		if(i>50)
		{
			return 0;
		}
	
	}
	
	delay_us(500);
	dq1;
	
	return 1;
}

void Ds18b20WriteByte(unsigned char dat)
{
	char j;

	for(j=0; j<8; j++)
	{
		
		dq0;	     	  
		delay_us(6);
		
		if(dat & 0x01==1)
		{
			dq1;	
		}
		else
		{
			dq0;	
		}
		
		delay_us(65); 
		dq1;
		delay_us(3);		
		dat >>= 1;
	}
}

unsigned char Ds18b20ReadByte(void)
{
	unsigned char byte, bi;
	char j;	
	
	for(j=8; j>0; j--)
	{
		dq1;
		delay_us(5);
		
		dq0;
		delay_us(6);
		
		dq1;
		delay_us(6);
		
		bi = GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2);	 
		byte = (byte >> 1) | (bi << 7);						  
		delay_us(65);
	}				
	return byte;
}

void  Ds18b20ChangTemp(void)
{
	Ds18b20Init();
	delay_us(100);
	Ds18b20WriteByte(0xcc);	
	Ds18b20WriteByte(0x44);	    
}

void  Ds18b20ReadTempCom(void)
{	

	Ds18b20Init();
	delay_us(100);
	Ds18b20WriteByte(0xcc);	 
	Ds18b20WriteByte(0xbe);	 
}

float Ds18b20ReadTemp(void)
{
	int dat = 0;
	unsigned char tmh, tml;
	float temp;
	Ds18b20ChangTemp();			 	
	Ds18b20ReadTempCom();			
	tml = Ds18b20ReadByte();		
	tmh = Ds18b20ReadByte();		
	dat = tmh;
	dat <<= 8;
	dat |= tml;
	temp=dat*(0.0625);
	return temp;
}

int Temp(void)
{
	int dat = 0;
	unsigned char tmh, tml;
	Ds18b20ChangTemp();			 	
	Ds18b20ReadTempCom();			
	tml = Ds18b20ReadByte();		
	tmh = Ds18b20ReadByte();		
	dat = tmh;
	dat <<= 8;
	dat |= tml;
	return dat;
}

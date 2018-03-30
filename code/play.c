#include "stm32f10x_gpio.h"
#include "stm32f10x_exti.h"
#include "stm32f10x_tim.h"
#include "LCD.h"
#include "mmc_sd.h"
#include "vs.h"
#include "play.h"
#include "touch.h"
int  flag_wav=0,num_wav=0;
u8   wav1,wav2,last_wav1=0,last_wav2=0;
u8   SD_DATA1[512],SD_DATA2[512];
u8   song_DATA[512];
int  sendcount; //循环计数
int  midiflag=0;
void send_dat(char dat)
{
	char i;
	//GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	//GPIO_ResetBits(GPIOA,GPIO_Pin_3);
	for(i=0;i<8;i++)
	{
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
		if((dat&0x80)==0x80)
		{
			GPIO_SetBits(GPIOA,GPIO_Pin_3);
		}
		else
		{
			GPIO_ResetBits(GPIOA,GPIO_Pin_3);
		}
		GPIO_SetBits(GPIOA,GPIO_Pin_2);
		dat=dat<<1;
		GPIO_ResetBits(GPIOA,GPIO_Pin_2);
	}
}
void play_delay()
{
	int a=60000;
	while(a--);
}
void SysTick_Handler(void)
{
	if(flag_wav==0)
	{
 		wav1=((SD_DATA1[num_wav+1]<<8)+SD_DATA1[num_wav]  +32768)>>8;
		TIM_SetCompare3(TIM2,wav1);
 		wav2=((SD_DATA1[num_wav+3]<<8)+SD_DATA1[num_wav+2]+32768)>>8;
		TIM_SetCompare4(TIM5,wav2);
		//send_dat(wav1);
// 		GPIO_Write(GPIOC,(wav1));
// 		if(last_wav1<wav1)
// 		{
// 			GPIO_SetBits(GPIOA,GPIO_Pin_2);
// 		}
// 		else
// 		{
// 			GPIO_ResetBits(GPIOA,GPIO_Pin_2);
// 		} 
// 		if(last_wav2<wav2)
// 		{
// 			GPIO_SetBits(GPIOA,GPIO_Pin_3);
// 		}
// 		else
// 		{
// 			GPIO_ResetBits(GPIOA,GPIO_Pin_3);
// 		} 
// 		last_wav1=wav1;
// 		last_wav2=wav2;
	}
     if(flag_wav==1)
	{
 		wav1=((SD_DATA2[num_wav+1]<<8)+SD_DATA2[num_wav]  +32768)>>8;
		TIM_SetCompare3(TIM2,wav1);
 		wav2=((SD_DATA2[num_wav+3]<<8)+SD_DATA2[num_wav+2]+32768)>>8;
		TIM_SetCompare4(TIM5,wav2);
		//send_dat(wav1);
// 		GPIO_Write(GPIOC,(wav1));
// 		if(last_wav1<wav1)
// 		{
// 			GPIO_SetBits(GPIOA,GPIO_Pin_2);
// 		}
// 		else
// 		{
// 			GPIO_ResetBits(GPIOA,GPIO_Pin_2);
// 		} 
// 		if(last_wav2<wav2)
// 		{
// 			GPIO_SetBits(GPIOA,GPIO_Pin_3);
// 		}
// 		else
// 		{
// 			GPIO_ResetBits(GPIOA,GPIO_Pin_3);
// 		} 
// 		last_wav1=wav1;
// 		last_wav2=wav2;
	}
	num_wav=num_wav+4;
}
void ExtINTConfig(char enable)
{
		EXTI_InitTypeDef EXTI_InitStructure;
		NVIC_InitTypeDef NVIC_InitStructure;
		if(enable==1)
		{
			//外中断初始化
			EXTI_DeInit();
			//PA15中断模式 设置
			GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);
			EXTI_InitStructure.EXTI_Line = EXTI_Line15;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;     //下降沿触发
			EXTI_InitStructure.EXTI_LineCmd = ENABLE;
			EXTI_Init(&EXTI_InitStructure);
			//外中断配置
			NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
			NVIC_Init(&NVIC_InitStructure);
		}
		if(enable==0)
		{
			//外中断初始化
			EXTI_DeInit();
			//PA15中断模式 设置
			GPIO_EXTILineConfig(GPIO_PortSourceGPIOA, GPIO_PinSource15);
			EXTI_InitStructure.EXTI_Line = EXTI_Line15;
			EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
			EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;     //下降沿触发
			EXTI_InitStructure.EXTI_LineCmd = DISABLE;
			EXTI_Init(&EXTI_InitStructure);
			//外中断配置
			NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
			NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
			NVIC_InitStructure.NVIC_IRQChannelCmd = DISABLE;
			NVIC_Init(&NVIC_InitStructure);
		}
		
}

void EXTI15_10_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line15)) 
	{
		Read_ADS(&touchx,&touchy);
		if((touchx<1000)&&(touchy<1000))
		{
				
			touchx=3400;
			touchy=3400;
			POINT_COLOR=0xF800;
			LCD_ShowString(180,312,"NEXT MUSIC");
			POINT_COLOR=0x07E0;
				
			xdcs1;
			xcs0;
			command_write(0x0b,0x80,0x80);
			xcs1;
			xdcs0;
			delay_vs(30);			
			xdcs1;
			xcs0;
			command_write(0x00,0x08,0x97);
			delay_vs(30);
			command_write(0x0b,vol,vol);
			xcs1;
			xdcs0;
			midiflag=1;
		}
		
		if((touchx<1000)&&((touchy>2000)&&(touchy<2500)))
		{
			touchx=3400;
			touchy=3400;
			POINT_COLOR=0xF800;
			LCD_ShowString(180,190,"OUT       ");
			POINT_COLOR=0x07E0;
			xdcs1;
			xcs0;
			command_write(0x0b,0x80,0x80);
			xcs1;
			xdcs0;
			delay_vs(30);			
			xdcs1;
			xcs0;
			command_write(0x00,0x08,0x97);
			delay_vs(30);
			command_write(0x0b,vol,vol);
			xcs1;
			xdcs0;
			LCD_ShowString(180,190,"OUT     ");
			touchx=3400;
			touchy=3400;
			midiflag=2;
		}
		
		if((touchx<1000)&&(touchy>3000))
		{
			touchx=3400;
			touchy=3400;
			POINT_COLOR=0xF800;
			LCD_ShowString(180,60,"VOL+");
			POINT_COLOR=0x07E0;
			increase_volume();
			LCD_ShowString(180,60,"VOL+");
		}
		
		if(((touchx>2000)&&(touchx<2500))&&(touchy>3000))
		{
			touchx=3400;
			touchy=3400;
			POINT_COLOR=0xF800;
			LCD_ShowString(90,60,"VOL-");
			POINT_COLOR=0x07E0;
			reduce_volume();
			LCD_ShowString(90,60,"VOL-");				
		}
		EXTI_ClearFlag(EXTI_Line15);  
		EXTI_ClearITPendingBit(EXTI_Line15);
	}  
}
/*mp3 wma ogg play*/
char play_mp3(long int firstSector,long int filelong)
{
	     long int Sector;
		long int count;
		int i;
		Sector =firstSector;
		count  =(filelong/512)+1;
		putnums(32,80,count);
		show_line2(200,320,160);
		show_line1(160,240,200); 
		LCD_ShowString(180,312,"NEXT MUSIC");
		  
		show_line2(80,200,160);
		show_line1(160,240,80); 
		LCD_ShowString(180,190,"OUT       ");
		  
		show_line2(0,80,160);
		show_line1(160,240,0); 
		LCD_ShowString(180,60,"VOL+");
		  
		show_line2(0,80,80);
		show_line1(80,160,80); 
		show_line1(80,160,0); 
		LCD_ShowString(90,60,"VOL-");
		SD_ReadSingleBlock(Sector,song_DATA);
		for(i=0;i<512;i++)//待优化
		{
			if(song_DATA[i]=='i')
			{
				if(song_DATA[i+1]=='m')
				{
					if(song_DATA[i+2]=='a')
					{
						Sector=Sector+700;
						count =count-700;
						putnums(32,80,count);
						goto loop1;
					}
				}
			}
		}
loop1:	for(i=0;i<8;i++)
		{
			for(sendcount=0;sendcount<12;sendcount++)
			{
				LCD_ShowChar(0+16*i,176-sendcount*8,song_DATA[(12*i)+sendcount]);
			}
		}
		
		GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	     while(1)
		{
		     if(count==(filelong/512)+1)
			{
				touchx=3400;
				touchy=3400;
			}
		     SD_ReadSingleBlock(Sector,song_DATA);
			GPIO_SetBits(GPIOB,GPIO_Pin_1);
		     xdcs0;
	          xcs1;
			for(sendcount=0;sendcount<512;sendcount++)
			{
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) 
				{
					putnums(16,80,count);
				}
				spi_write(song_DATA[sendcount]);
			}	   
			xdcs1;
			xcs0;
			Sector++;
			count--;
			
			if(count==-1)
			{
				xdcs1;
				xcs0;
				command_write(0x0b,0x80,0x80);
				xcs1;
				xdcs0;
				for(sendcount=0;sendcount<2048;sendcount++)
				{
					while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
					spi_write(0xff);
				}	   
				xdcs1;
				xcs0;
				command_write(0x00,0x08,0x97);
				delay_vs(30);
				command_write(0x0b,vol,vol);
				xcs1;
				xdcs0;
				return 4;
			}
			
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0)
			{
				Read_ADS(&touchx,&touchy);
				if((touchx<1000)&&(touchy<1000))
				{
					
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,312,"NEXT MUSIC");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					
					xdcs1;
					xcs0;
					command_write(0x0b,0x80,0x80);
					xcs1;
					xdcs0;
					for(sendcount=0;sendcount<2048;sendcount++)
					{
						while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
						spi_write(0xff);
					}	   
					xdcs1;
					xcs0;
					command_write(0x00,0x08,0x97);
					delay_vs(30);
					command_write(0x0b,vol,vol);
					xcs1;
					xdcs0;
					return 4;
				}
				if((touchx<1000)&&((touchy>2000)&&(touchy<2500)))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,190,"OUT       ");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					xdcs1;
					xcs0;
					command_write(0x0b,0x80,0x80);
					xcs1;
					xdcs0;
					for(sendcount=0;sendcount<2048;sendcount++)
					{
						while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
						spi_write(0xff);
					}	   
					xdcs1;
					xcs0;
					command_write(0x00,0x08,0x97);
					delay_vs(30);
					command_write(0x0b,vol,vol);
					xcs1;
					xdcs0;
					LCD_ShowString(180,190,"OUT     ");
					return 3;
				}
				if((touchx<1000)&&(touchy>3000))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,60,"VOL+");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					increase_volume();
					LCD_ShowString(180,60,"VOL+");
				}
				if(((touchx>2000)&&(touchx<2500))&&(touchy>3000))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(90,60,"VOL-");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					reduce_volume();
					LCD_ShowString(90,60,"VOL-");				
				}
			}
			SD_ReadSingleBlock(Sector,song_DATA);
			GPIO_ResetBits(GPIOB,GPIO_Pin_1);
		     xdcs0;
	          xcs1;
			for(sendcount=0;sendcount<512;sendcount++)
			{
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11)) 
				{
					putnums(16,80,count);
				}
				spi_write(song_DATA[sendcount]);
			}	   
			xdcs1;
			xcs0;
			Sector++;
			count--;
			
			if(count==-1)
			{
				xdcs1;
				xcs0;
				command_write(0x0b,0x80,0x80);
				xcs1;
				xdcs0;
				for(sendcount=0;sendcount<2048;sendcount++)
				{
					while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
					spi_write(0xff);
				}	   
				xdcs1;
				xcs0;
				command_write(0x00,0x08,0x97);
				delay_vs(30);
				command_write(0x0b,vol,vol);
				xcs1;
				xdcs0;
				return 4;
			}
	     }
}	

/*MIDI play*/
char play_midi(long int firstSector,long int filelong)
{
	     long int Sector;
		long int count;
		ExtINTConfig(1);
		Sector =firstSector;
		count  =(filelong/512)+4;
		putnums(32,80,count);
		show_line2(200,320,160);
		show_line1(160,240,200); 
		LCD_ShowString(180,312,"NEXT MUSIC");
		  
		show_line2(80,200,160);
		show_line1(160,240,80); 
		LCD_ShowString(180,190,"OUT       ");
		  
		show_line2(0,80,160);
		show_line1(160,240,0); 
		LCD_ShowString(180,60,"VOL+");
		  
		show_line2(0,80,80);
		show_line1(80,160,80); 
		show_line1(80,160,0); 
		LCD_ShowString(90,60,"VOL-");
	     while(1)
		{
		     SD_ReadSingleBlock(Sector,song_DATA);
			GPIO_SetBits(GPIOB,GPIO_Pin_1);
		     xdcs0;
	          xcs1;
			for(sendcount=0;sendcount<512;sendcount++)
			{
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
				spi_write(song_DATA[sendcount]);
			}	   
			xdcs1;
			xcs0;
			Sector++;
			count--;
			putnums(16,80,count);
			if(count==-1)
			{
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
 				xdcs1;
 				xcs0;
 				command_write(0x0b,0x80,0x80);
                    delay_vs(30);
 				command_write(0x00,0x08,0x97);
 				delay_vs(30);
 				command_write(0x0b,vol,vol);
				xcs1;
 				xdcs0;
				ExtINTConfig(0);
				return 4;
			}
			if(midiflag==1)
			{
				midiflag=0;
				ExtINTConfig(0);
				touchx=3400;
				touchy=3400;
				return 4;
			}
			if(midiflag==2)
			{
				midiflag=0;
				ExtINTConfig(0);
				touchx=3400;
				touchy=3400;
				return 3;
			}
			
			SD_ReadSingleBlock(Sector,song_DATA);
			GPIO_ResetBits(GPIOB,GPIO_Pin_1);
		     xdcs0;
	          xcs1;
			for(sendcount=0;sendcount<512;sendcount++)
			{
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
				spi_write(song_DATA[sendcount]);
			}	   
			xdcs1;
			xcs0;
			Sector++;
			count--;
			putnums(16,80,count);
			if(count==-1)
			{
				while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
 				xdcs1;
 				xcs0;
 				command_write(0x0b,0x80,0x80);
				delay_vs(30);
 				command_write(0x00,0x08,0x97);
 				delay_vs(30);
 				command_write(0x0b,vol,vol);
				xcs1;
 				xdcs0;
				ExtINTConfig(0);
				return 4;
			}
			if(midiflag==1)
			{
				midiflag=0;
				ExtINTConfig(0);
				return 4;
			}
			if(midiflag==2)
			{
				midiflag=0;
				ExtINTConfig(0);
				return 3;
			}
	     }
}

/*txt play*/
char play_txt(long int firstSector,long int filelong)
{
	     long int Sector=0;
		long int count=0;
		char     txtflag=1,i=0;
		Sector =firstSector;
		count  =(filelong/512)+1;
		putnums(32,80,count);
		show_line2(200,320,160);
		show_line1(160,240,200); 
		LCD_ShowString(180,312,"NEXT file ");
		  
		show_line2(80,200,160);
		show_line1(160,240,80); 
		LCD_ShowString(180,190,"OUT       ");
		  
		show_line2(0,80,160);
		show_line1(160,240,0); 
		LCD_ShowString(180,60,"page+");
		  
		show_line2(0,80,80);
		show_line1(80,160,80); 
		show_line1(80,160,0); 
		LCD_ShowString(90,60,"page-");
		
		touchx=3400;
		touchy=3400;
		SD_ReadSingleBlock(Sector,song_DATA);
	     while(1)
		{
			if(count<0)
			{
loop1:			count=5;
				Sector--;
				SD_ReadSingleBlock(Sector,song_DATA);
			}
		     if(txtflag==1)
			{
				txtflag=0;
				for(i=0;i<8;i++)
				{
					for(sendcount=0;sendcount<12;sendcount++)
					{
						LCD_ShowChar(0+16*i,176-sendcount*8,song_DATA[count*96+(12*i)+sendcount]);
						
					}
				}
				count++;
			}
			
			if(txtflag==2)
			{
				txtflag=0;
				count=count-2;
				if(count==-1)
				{
					goto loop1;
				}
				for(i=0;i<8;i++)
				{
					for(sendcount=0;sendcount<12;sendcount++)
					{
						LCD_ShowChar(0+16*i,176-sendcount*8,song_DATA[count*96+(12*i)+sendcount]);
						
					}
				}
				count++;
			}
			
			if(count>5)
			{
				count=0;
				Sector++;
				SD_ReadSingleBlock(Sector,song_DATA);
			}
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0)
			{
				Read_ADS(&touchx,&touchy);
				if((touchx<1000)&&(touchy<1000))
				{
					
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,312,"NEXT file ");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					
					xdcs1;
					xcs0;
					command_write(0x0b,0x80,0x80);
					xcs1;
					xdcs0;
					for(sendcount=0;sendcount<2048;sendcount++)
					{
						while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
						spi_write(0xff);
					}	   
					xdcs1;
					xcs0;
					command_write(0x00,0x08,0x97);
					delay_vs(30);
					command_write(0x0b,vol,vol);
					xcs1;
					xdcs0;
					return 4;
				}
				if((touchx<1000)&&((touchy>2000)&&(touchy<2500)))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,190,"OUT       ");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					xdcs1;
					xcs0;
					command_write(0x0b,0x80,0x80);
					xcs1;
					xdcs0;
					for(sendcount=0;sendcount<2048;sendcount++)
					{
						while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_11));
						spi_write(0xff);
					}	   
					xdcs1;
					xcs0;
					command_write(0x00,0x08,0x97);
					delay_vs(30);
					command_write(0x0b,vol,vol);
					xcs1;
					xdcs0;
					LCD_ShowString(180,190,"OUT     ");
					return 3;
				}
				if((touchx<1000)&&(touchy>3000))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,60,"page+");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					txtflag=1;
					LCD_ShowString(180,60,"page+");
				}
				if(((touchx>2000)&&(touchx<2500))&&(touchy>3000))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(90,60,"page-");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					txtflag=2;
					LCD_ShowString(90,60,"page-");				
				}
			}		
	     }
}	

char play_bin(long int firstSector,long int filelong)
{
	     long int Sector=0;
		long int count=0,countfile=0;
		char     txtflag=1;
		int      x=0,y=16,t=0,pos=0,num=0,temp=0,i;
		Sector   =firstSector;
		binadd   =firstSector;
		
		LCD_Clear(0x0000);
		ShowString(100,220,"字库载入成功！");
		for(i=0;i<100;i++)
		{
			play_delay();
		}
		LCD_Clear(0x0000);
		return 3;
		
	     countfile=(filelong/512)+1;
		show_line2(200,320,160);
		show_line1(160,240,200); 
		LCD_ShowString(180,312,"NEXT file ");
		  
		show_line2(80,200,160);
		show_line1(160,240,80); 
		LCD_ShowString(180,190,"OUT       ");
		  
		show_line2(0,80,160);
		show_line1(160,240,0); 
		LCD_ShowString(180,60,"page+");
		  
		show_line2(0,80,80);
		show_line1(80,160,80); 
		show_line1(80,160,0); 
		LCD_ShowString(90,60,"page-");
		LCD_Clear(0x0000);
		touchx=3400;
		touchy=3400;
		SD_ReadSingleBlock(Sector,song_DATA);
		countfile--;
	     while(1)
		{
		     if(txtflag==1)
			{
				txtflag=0;
				for(pos=0;pos<16;pos++)
				{
					temp=song_DATA[pos+num+count*32];		 
					for(t=7;t>=0;t--)
					{                 
						if(temp&0x01)LCD_DrawPoint(x+pos,y-t);     
						temp>>=1; 
					}
					num++;
					temp=song_DATA[pos+num+count*32];
					for(t=7;t>=0;t--)
					{                 
						if(temp&0x01)LCD_DrawPoint(x+pos,y-t-8);     
						temp>>=1; 
					}
				}    	
				count++;
				num=0;
				y=y+16;
			}
			if(count>16)
			{
				touchx=3400;
				touchy=3400;
				count=0;
				Sector++;
				SD_ReadSingleBlock(Sector,song_DATA);
				countfile--;
				if(countfile==0)
				{
					LCD_Clear(0x0000);
					ShowString(100,150,"字库载入成功！");
					for(i=0;i<200;i++)
					{
						play_delay();
					}
					LCD_Clear(0x0000);
					return 3;
				}
			}
			if(y>320)
			{
				y=16;
				x=x+16;
				if(x==240)
				{
					y=16;
					x=0;
					for(i=0;i<50;i++)
					{
						play_delay();
					}
					LCD_Clear(0x0000);
				}
			}
			play_delay();
			txtflag=1;
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0)
			{
				Read_ADS(&touchx,&touchy);
				if((touchx<1000)&&(touchy<1000))
				{
					
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,312,"NEXT file ");
					POINT_COLOR=0x07E0;
					LCD_ShowString(180,312,"NEXT file ");
					LCD_Clear(0x0000);
					return 4;
				}
				if((touchx<1000)&&((touchy>2000)&&(touchy<2500)))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,190,"OUT       ");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					LCD_Clear(0x0000);
					ShowString(100,150,"字库载入成功！");
					for(i=0;i<100;i++)
					{
						play_delay();
					}
					LCD_Clear(0x0000);
					LCD_ShowString(180,190,"OUT     ");
					touchx=3400;
					touchy=3400;
					return 3;
				}
				if((touchx<1000)&&(touchy>3000))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,60,"page+");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					txtflag=1;
					LCD_ShowString(180,60,"page+");
				}
				if(((touchx>2000)&&(touchx<2500))&&(touchy>3000))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(90,60,"page-");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					txtflag=2;
					LCD_ShowString(90,60,"page-");				
				}
			}		
	     }
}	

char play_wav(long int firstSector,long int filelong)
{
	     long int Sector;
		long int count;
		int      speed;
		double   dataf;
		Sector =firstSector;
		count  =(filelong/512)+1;
		putnums(32,80,count);
		show_line2(200,320,160);
		show_line1(160,240,200); 
		LCD_ShowString(180,312,"NEXT MUSIC");
		  
		show_line2(80,200,160);
		show_line1(160,240,80); 
		LCD_ShowString(180,190,"OUT       ");
		  
		show_line2(0,80,160);
		show_line1(160,240,0); 
		LCD_ShowString(180,60,"VOL+");
		  
		show_line2(0,80,80);
		show_line1(80,160,80); 
		show_line1(80,160,0); 
		LCD_ShowString(90,60,"VOL-");
		
		
		SD_ReadSingleBlock(firstSector,SD_DATA1);
		speed=(SD_DATA1[61]<<8)+SD_DATA1[60];
		dataf=1.0/speed;
		Sector++;
		SD_ReadSingleBlock(Sector,SD_DATA2);
		Sector++;
		//__enable_irq( ); 
		SysTick_Config(72*(dataf*1000000+0.5));
	     while(1)
		{
			
			if(num_wav>=512)
			{
				num_wav=0;
				
				if(flag_wav==0)
				{
					flag_wav=1;
					SD_ReadSingleBlock(Sector,SD_DATA1);
					goto loopx;
				}
				if(flag_wav==1)
				{
					flag_wav=0;
					SD_ReadSingleBlock(Sector,SD_DATA2);
					goto loopx;
				}
				loopx: Sector++;
				count--;
			}
			
			if(count==-1)
			{
				SysTick_Config(0);
				//__disable_irq( );
				return 4;
			}
			
			if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0)
			{
				Read_ADS(&touchx,&touchy);
				if((touchx<1000)&&(touchy<1000))
				{
					
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,312,"NEXT MUSIC");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					SysTick_Config(0);
					//__disable_irq( );
					return 4;
				}
				if((touchx<1000)&&((touchy>2000)&&(touchy<2500)))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,190,"OUT       ");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					LCD_ShowString(180,190,"OUT     ");
					SysTick_Config(0);
					//__disable_irq( );
					return 3;
				}
				if((touchx<1000)&&(touchy>3000))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(180,60,"VOL+");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					//increase_volume();
					LCD_ShowString(180,60,"VOL+");
				}
				if(((touchx>2000)&&(touchx<2500))&&(touchy>3000))
				{
					touchx=3400;
					touchy=3400;
					POINT_COLOR=0xF800;
					LCD_ShowString(90,60,"VOL-");
					POINT_COLOR=0x07E0;
					while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
					//reduce_volume();
					LCD_ShowString(90,60,"VOL-");				
				}
			}
			
	     }
}

#include "stm32f10x_gpio.h"
#include "stm32f10x_adc.h"
#include "stm32f10x_tim.h"
#include "mmc_sd.h"
#include "vs.h"
#include "pwm.h"
#include "touch.h"
#include "ini.h"
#include "LCD.h"
#include "play.h"
#include "file.h"
#include "ds18b.h"
//flag== 1 IN 0 OUT 2 null 3 outfile 4 nextfile 5 nextscan 6lastscan
struct folder folderdata[256];
struct file   filedata[256];
short  int    point=0,pcount=0,count=0,folderstack=0;
short pcm[48]=
{
	0x52,0x49,0x46,0x46,0xff,0xff,0xff,0xff,
	0x57,0x41,0x56,0x45,0x66,0x6d,0x74,0x20,
	0x10,0x00,0x00,0x00,0x01,0x00,0x02,0x00,
	0x44,0xac,0x00,0x00,0x10,0xb1,0x02,0x00,
	0x04,0x00,0x10,0x00,0x64,0x61,0x74,0x61,
	0xff,0xff,0xff,0xff,0x00,0x00,0x00,0x00,
};
//delay
void delay(u16 time)
{    
   u16 i=0;  
   while(time--)
   {
      i=10;  
      while(i--) ;    
   }
}
//touch scan
char touch_point(void)     //文件选择指针移动
{
	if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0)
	{
		Read_ADS(&touchx,&touchy);
	}
	if((touchx<1000)&&(touchy<1000))
	{
		touchx=3400;
		touchy=3400;
		POINT_COLOR=0xF800;
		LCD_ShowString(180,312,"NEXT ");
		POINT_COLOR=0x07E0;
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
		point++;
		pcount++;
		if((pcount>=8)&&(point!=100))
		{
			pcount=0;
			count++;
			LCD_ShowString(0+pcount*16,200,"<-");
			LCD_ShowString(7*16,200,"  ");
			LCD_ShowString(180,312,"NEXT ");
			return 5;
		}
		if(point==100)
		{
			point=100;
		}		
		LCD_ShowString(0+pcount*16,200,"<-");
		LCD_ShowString(0+(pcount-1)*16,200,"  ");
		LCD_ShowString(180,312,"NEXT ");
	}
	if((touchx<1000)&&((touchy>2000)&&(touchy<2500)))
	{
		touchx=3400;
		touchy=3400;
		POINT_COLOR=0xF800;
		LCD_ShowString(180,190,"LAST ");
		POINT_COLOR=0x07E0;
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
		point--;
		pcount--;
		if(point<=0)
		{
			point=0;
			pcount=0;
		}
		if((pcount<0)&&(point!=0)&&(count!=0))
		{
			pcount=7;
			count--;
			LCD_ShowString(0+pcount*16,200,"<-");
			LCD_ShowString(0,200,"  ");
			LCD_ShowString(180,190,"LAST ");
			return 6;
		}
		LCD_ShowString(0+pcount*16,200,"<-");
		LCD_ShowString(0+(pcount+1)*16,200,"  ");
		LCD_ShowString(180,190,"LAST ");
	}
	if((touchx<1000)&&(touchy>3000))
	{
		touchx=3400;
		touchy=3400;
		POINT_COLOR=0xF800;
		LCD_ShowString(180,60,"IN");
		POINT_COLOR=0x07E0;
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
		LCD_ShowString(180,60,"IN");
		return 1;	
	}
	if(((touchx>2000)&&(touchx<2500))&&(touchy>3000))
	{
		touchx=3400;
		touchy=3400;
		POINT_COLOR=0xF800;
		LCD_ShowString(90,60,"OUT");
		POINT_COLOR=0x07E0;
		while(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_15)==0);
		LCD_ShowString(90,60,"OUT");	
		return 0;		
	}
	return 2;
}
void print_scan(void)      //显示文件选择
{
	//point=0;
	LCD_ShowString(0+pcount*16,200,"<-");
	show_line2(200,320,160);
	show_line1(160,240,200); 
	LCD_ShowString(180,312,"NEXT      ");
	  
	show_line2(80,200,160);
	show_line1(160,240,80); 
	LCD_ShowString(180,190,"LAST      ");
	  
	show_line2(0,80,160);
	show_line1(160,240,0); 
	LCD_ShowString(180,60,"IN   ");
	  
	show_line2(0,80,80);
	show_line1(80,160,80); 
	show_line1(80,160,0); 
	LCD_ShowString(90,60,"OUT   ");
}
//clear data
void cleardata(void)
{
	int i=0,j=0;
	for(j=0;j<256;j++)
	{
		for(i=0;i<11;i++)
		{
			filedata[j].filename[i]=0x00;
		}
		filedata[j].fileSector=0x00;
		filedata[j].filecut   =0x00;
		filedata[j].filelong  =0x00;
		filedata[j].filenums  =0x00;
	}
}
//readfile
void thefile(void)
{
	short int i=0,j=0,flag=-1,dir=0;
	//ini_FAT//
	dir=ini_FAT();
	read_file(filedata,dir,2);
	folderdata[folderstack].folderSector=dir;
	folderdata[folderstack].foldercut   =2;
	folderdata[folderstack].folderlong  =0x00;
	folderdata[folderstack].foldernums  =0x00;
	folderstack++;
	for(point=0;point<100;point++)
	{
		if(((filedata[point].filename[8]=='B')&&(filedata[point].filename[9]=='I')&&(filedata[point].filename[10]=='N')))
		{
			binadd=filedata[point].fileSector;
		}
	}
	point=0;
	ShowString(84 ,250,"....................");
	ShowString(100,250,"欢迎使用播放器ver1.0");
	ShowString(116,250,"....................");
	ShowString(132,250,".......by yhzj......");
	ShowString(148,250,"....................");
	for(i=0;i<700;i++)
	{
		delay(3000);	
	}
	LCD_Clear(0x0000);
	//ShowString(50,80,"");
	//the FAT//
	while(1)
	{
loop1:	for(j=0;j<8;j++)
 		{
 			for(i=0;i<11;)
			{	
				if((filedata[j+count*8].filename[i]&0x80)==0x80)
				{
					show_char(0+j*16,304-i*8,filedata[j+count*8].filename[i],filedata[j+count*8].filename[i+1]);
					i=i+2;
				}
				else
				{	
					i=i+1;
					LCD_ShowChar(0+j*16,304-i*8,filedata[j+count*8].filename[i-1]);	
				}  
				//LCD_ShowChar(0+j*16,312-i*8,filedata[j+count*8].filename[i]);
			}
		}
		print_scan(); 
		while(1)
		{
			flag=touch_point();
			if(flag==1)
			{
				flag=-1;
				if(filedata[point].filelong==0)
				{
					for(i=0;i<11;i++)
					{
						folderdata[folderstack].foldername[i]=filedata[point].filename[i];
					}
					folderdata[folderstack].folderSector=filedata[point].fileSector;
					folderdata[folderstack].foldercut   =filedata[point].filecut;
					folderdata[folderstack].folderlong  =filedata[point].filelong;
					folderdata[folderstack].foldernums  =filedata[point].filenums;
					LCD_ShowString((pcount)*16,200,"  ");
					point=0,pcount=0,count=0;
					cleardata();
					read_file(filedata,folderdata[folderstack].folderSector,folderdata[folderstack].foldercut);
					folderstack++;
					//putnums(16,80,folderstack);
					goto loop1;//进入下一级文件夹
				}
				while(1)
				{
					if(((filedata[point].filename[8]==0x00)&&(filedata[point].filename[9]==0x00)&&(filedata[point].filename[10]==0x00)))
					{
						POINT_COLOR=0xF800;
						LCD_ShowString(0,80,"no files");
						POINT_COLOR=0x07E0;
						LCD_ShowString((pcount)*16,200,"  ");
						delay(65532);
						delay(65532);
						delay(65532);
						LCD_ShowString(0,80,"        ");
						point=0,pcount=0,count=0;
						goto loop1;
					}
					if(filedata[point].filelong==0)
					{
						flag=4;
					}
					if(((filedata[point].filename[8]=='M')&&(filedata[point].filename[9]=='I')&&(filedata[point].filename[10]=='D')))
					{
						flag=play_midi(filedata[point].fileSector,filedata[point].filelong);
						goto jum;
					}
					if(((filedata[point].filename[8]=='T')&&(filedata[point].filename[9]=='X')&&(filedata[point].filename[10]=='T')))
					{
						flag=play_txt(filedata[point].fileSector,filedata[point].filelong);
						goto jum;
					}
					if(((filedata[point].filename[8]=='B')&&(filedata[point].filename[9]=='I')&&(filedata[point].filename[10]=='N'))||
		                  ((filedata[point].filename[8]==' ')&&(filedata[point].filename[9]==' ')&&(filedata[point].filename[10]==' ')))
					{
						flag=play_bin(filedata[point].fileSector,filedata[point].filelong);
						goto jum;
					}
					if(((filedata[point].filename[8]=='W')&&(filedata[point].filename[9]=='A')&&(filedata[point].filename[10]=='V')))
					{
						flag=play_wav(filedata[point].fileSector,filedata[point].filelong);
						goto jum;
					}
					if(((filedata[point].filename[8]=='F')&&(filedata[point].filename[9]=='L')&&(filedata[point].filename[10]=='A')))
					{
						Load_VS_Load_FlacPatch();
						flag=play_mp3(filedata[point].fileSector,filedata[point].filelong);
						goto jum;
					}
					if(((filedata[point].filename[8]=='M')&&(filedata[point].filename[9]=='P')&&(filedata[point].filename[10]=='3'))||
					   ((filedata[point].filename[8]=='W')&&(filedata[point].filename[9]=='M')&&(filedata[point].filename[10]=='A'))||
		                  ((filedata[point].filename[8]=='O')&&(filedata[point].filename[9]=='G')&&(filedata[point].filename[10]=='G')))
					{
						flag=play_mp3(filedata[point].fileSector,filedata[point].filelong);
					}
					else
					{
						flag=4;
					}						
jum:					if(flag==3)
					{
						flag=-1;
						goto loop1;
					}
					if(flag==4)
					{
						flag=-1;
						point++;
						pcount++;
						if((pcount>=8)&&(point!=100))
						{
							pcount=0;
							count++;
							for(j=0;j<8;j++)
							{
								for(i=0;i<11;)
								{	
									if((filedata[j+count*8].filename[i]&0x80)==0x80)
									{
										show_char(0+j*16,304-i*8,filedata[j+count*8].filename[i],filedata[j+count*8].filename[i+1]);
										i=i+2;
									}
									else
									{	
										i=i+1;
										LCD_ShowChar(0+j*16,304-i*8,filedata[j+count*8].filename[i-1]);	
									}  
									//LCD_ShowChar(0+j*16,312-i*8,filedata[j+count*8].filename[i]);
								}
							}
						}
						if(point==100)
						{
							point=100;
						}	
						LCD_ShowString(0+pcount*16,200,"<-");
						if(pcount!=0)
						{
							LCD_ShowString(0+(pcount-1)*16,200,"  ");
						}
						else
						{
							LCD_ShowString(7*16,200,"  ");
						}
					}
				}
			}
			if(flag==5)
			{
				flag=-1;
				goto loop1;//打印下8个文件
			}
			if(flag==6)
			{
				flag=-1;
				goto loop1;//打印上8个文件
			}
			if(flag==0)     //退出这一级文件夹
			{
				flag=-1;
				if(folderstack>1)
				{
					folderstack--;
					LCD_ShowString((pcount)*16,200,"  ");
					point=0,pcount=0,count=0;
					cleardata();
					//putnums(16,80,folderstack);
					read_file(filedata,folderdata[folderstack-1].folderSector,folderdata[folderstack-1].foldercut);
					goto loop1;
				}
			}		
		}
	}	
}
//MAIN
int main(void)
{
	//int x=0,y=320,i=0,j=0;
	//char led_dt=0,led_fx=0;
	//drive//
	sys_ini();
	GPIO_SetBits(GPIOB,GPIO_Pin_1);
	delay(30000);
	
	Lcd_Init();
	LCD_Clear(0x0000);
	delay(3000);
	LCD_Clear(0x0000);
	delay(3000);
	LCD_ShowString(0,312,"LCD ...............OK!");
	delay(3000);
	
	Touch_ini();
	LCD_ShowString(16,312,"touch ............OK!");
	delay(1000);
	
	mp3_Init();
	LCD_ShowString(32,312,"VS1053 ...........OK!");
	delay(1000);
	
// 	adc_ini();
// 	LCD_ShowString(32,312,"adc ..............OK!");
// 	delay(1000);
	
	SD_Init();
	LCD_ShowString(48,312,"SDCARD ...........OK!");
	delay(60000);
	GPIO_ResetBits(GPIOB,GPIO_Pin_1);
	delay(60000);
	LCD_Clear(0x0000);
	delay(3000);
	
	pwm1_ini();
	pwm2_ini();
	//fat//
	thefile();
}

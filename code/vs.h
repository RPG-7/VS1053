#ifndef __VS_H__
#define __VS_H__

#define  dreq1 GPIO_SetBits(GPIOB,GPIO_Pin_11)
#define  dreq0 GPIO_ResetBits(GPIOB,GPIO_Pin_11)
#define  rst1  GPIO_SetBits(GPIOB,GPIO_Pin_10)
#define  rst0  GPIO_ResetBits(GPIOB,GPIO_Pin_10)
#define  xdcs1 GPIO_SetBits(GPIOA,GPIO_Pin_8)
#define  xdcs0 GPIO_ResetBits(GPIOA,GPIO_Pin_8)
#define  xcs1  GPIO_SetBits(GPIOB,GPIO_Pin_12)
#define  xcs0  GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define  clk1  GPIO_SetBits(GPIOB,GPIO_Pin_13)
#define  clk0  GPIO_ResetBits(GPIOB,GPIO_Pin_13)
#define  miso1  GPIO_SetBits(GPIOB,GPIO_Pin_14)
#define  miso0  GPIO_ResetBits(GPIOB,GPIO_Pin_14)
#define  mosi1  GPIO_SetBits(GPIOB,GPIO_Pin_15)
#define  mosi0  GPIO_ResetBits(GPIOB,GPIO_Pin_15)
extern int vol;
void mp3_Init(void);
void command_write(unsigned char addr,unsigned char hadr,unsigned char ladr);	                                                 
void reduce_volume(void);						                           
void increase_volume(void );					                                                                                          //Çå»º´æ 
void delay_vs(int z);
char spi_write(u8 TxData);
u16  command_read(unsigned char addr);
void Load_VS_Load_FlacPatch(void);
#endif

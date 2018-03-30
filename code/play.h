#ifndef __PLAY_H__
#define __PLAY_H__
void SysTick_Handler(void);
void ExtINTConfig(char enable);
void EXTI15_10_IRQHandler(void);
char play_mp3(long int firstSector,long int filelong);
char play_midi(long int firstSector,long int filelong);
char play_txt(long int firstSector,long int filelong);
char play_bin(long int firstSector,long int filelong);
char play_wav(long int firstSector,long int filelong);
#endif

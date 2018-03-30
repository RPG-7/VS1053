#include "mmc_sd.h"
#include "file.h"
#include "LCD.h"
//SD_DATA
u8 SD_DATA[512];
u8 FAT_DATA[512];
//FAT数据
short int BytesPerSector;         
short int SectorsPerCluster;      
short int ReserveSectors;      
short int FatTableNums;              
short int SectorsPerFat;                  
short int RootDir;
long  int ReadDir;                 //读根目录
long  int	firstcut;	
long  int	nextcut;  	
long  int	firstSector;
long  int Sector;
long  int filelong; 
long  int fileSector;             //文件物理地址
long  int Readfile;               //读文件目录
long  int pianyi=0;                 //物理偏移
//DBRFAT
void read_DBR(void)
{
	BytesPerSector=(SD_DATA[12]<<8)+SD_DATA[11];
	SectorsPerCluster=SD_DATA[13];
	ReserveSectors=(SD_DATA[15]<<8)+SD_DATA[14];
	FatTableNums=(SD_DATA[17]<<8)+SD_DATA[16];
	SectorsPerFat=(SD_DATA[37]<<8)+SD_DATA[36];
	RootDir=pianyi+ReserveSectors+FatTableNums*SectorsPerFat;	
}
//DIR
void read_Dir(char filecount)
{
	firstcut=(SD_DATA[21+filecount*32]<<24)+(SD_DATA[20+filecount*32]<<16)+(SD_DATA[27+filecount*32]<<8)+SD_DATA[26+filecount*32];
	firstSector=RootDir+(firstcut-2)*SectorsPerCluster;
	filelong=(SD_DATA[31+filecount*32]<<24)+(SD_DATA[30+filecount*32]<<16)+(SD_DATA[29+filecount*32]<<8)+SD_DATA[28+filecount*32];
}
//readfile
char read_file(struct file *file_data,long int folderSector,long int foldercut)
{
	char countfile=2;
	int  i=0;//循环计数
	int  filenum=0;
	if(foldercut==2)//跳过 . 和..
	{
		countfile=0;
	}
	SD_ReadSingleBlock(folderSector,SD_DATA);
	Readfile=folderSector;
	nextcut=foldercut;
	//putnums(0,80,pianyi);
	while(1)
	{			
		if(countfile>15)
		{
			countfile=0;
			Readfile++;
			if((Readfile-folderSector)==SectorsPerCluster)
			{
				
				SD_ReadSingleBlock((ReserveSectors+pianyi+nextcut/128),FAT_DATA);//putnums(16,80,(ReserveSectors+pianyi+nextcut/128));
				if((FAT_DATA[4*(nextcut%128)+3]==0x0f)&&(FAT_DATA[4*(nextcut%128)+2]==0xff)&&(FAT_DATA[4*(nextcut%128)+1]==0xff)&&(FAT_DATA[4*(nextcut%128)]==0xff))
				{
					return 1;
				}	
				nextcut=(FAT_DATA[4*(nextcut%128)+3]<<24)+(FAT_DATA[4*(nextcut%128)+2]<<16)+(FAT_DATA[4*(nextcut%128)+1]<<8)+(FAT_DATA[4*(nextcut%128)]);
				folderSector=RootDir+(nextcut-2)*SectorsPerCluster;
				Readfile=folderSector;
			}
			SD_ReadSingleBlock(Readfile,SD_DATA);
		}
		if((SD_DATA[0+countfile*32]==0)&&(SD_DATA[1+countfile*32]==0)&&(SD_DATA[2+countfile*32]==0)&&(SD_DATA[3+countfile*32]==0))
		{
			return 1;
		}
		read_Dir(countfile);		
		if((filelong==0)||
		   ((SD_DATA[8+countfile*32]=='M')&&(SD_DATA[9+countfile*32]=='P')&&(SD_DATA[10+countfile*32]=='3'))||
		   ((SD_DATA[8+countfile*32]=='W')&&(SD_DATA[9+countfile*32]=='M')&&(SD_DATA[10+countfile*32]=='A'))||
		   ((SD_DATA[8+countfile*32]=='M')&&(SD_DATA[9+countfile*32]=='I')&&(SD_DATA[10+countfile*32]=='D'))||
		   ((SD_DATA[8+countfile*32]=='W')&&(SD_DATA[9+countfile*32]=='A')&&(SD_DATA[10+countfile*32]=='V'))||
		   ((SD_DATA[8+countfile*32]=='F')&&(SD_DATA[9+countfile*32]=='L')&&(SD_DATA[10+countfile*32]=='A'))||
		   ((SD_DATA[8+countfile*32]=='T')&&(SD_DATA[9+countfile*32]=='X')&&(SD_DATA[10+countfile*32]=='T'))||
		   ((SD_DATA[8+countfile*32]=='B')&&(SD_DATA[9+countfile*32]=='I')&&(SD_DATA[10+countfile*32]=='N'))||
		   ((SD_DATA[8+countfile*32]==' ')&&(SD_DATA[9+countfile*32]==' ')&&(SD_DATA[10+countfile*32]==' '))||
		   ((SD_DATA[8+countfile*32]=='O')&&(SD_DATA[9+countfile*32]=='G')&&(SD_DATA[10+countfile*32]=='G')))
		{
			if(SD_DATA[0+countfile*32]!=0xE5)
			{
				for(i=0;i<11;i++)
				{
					file_data->filename[i]=SD_DATA[i+countfile*32];
				}
				file_data->fileSector=firstSector;
				file_data->filecut   =firstcut;
				file_data->filelong  =filelong;
				file_data->filenums  =filenum;
				filenum++;
				file_data++;
				
			}
			
		}
		countfile++;
	}		
}
//ini_FAT
long int ini_FAT(void)
{
	SD_ReadSingleBlock(0,SD_DATA);
	if(((SD_DATA[457]<<24)+(SD_DATA[456]<<16)+(SD_DATA[455]<<8)+(SD_DATA[454]))<10000)//有待修改
	{
		pianyi=(SD_DATA[457]<<24)+(SD_DATA[456]<<16)+(SD_DATA[455]<<8)+(SD_DATA[454]);
	     SD_ReadSingleBlock(0+pianyi,SD_DATA);
	}
	read_DBR();
	return RootDir;
}

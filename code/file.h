#ifndef __FILE_H__
#define __FILE_H__
struct folder//文件夹
{
	char     foldername[12];
	long int foldercut;
	long int folderSector;
	long int folderlong;
	int      foldernums;
};
struct file //文件
{
	char     filename[12];
	long int filecut;
	long int fileSector;
	long int filelong;
	int      filenums;
};
void read_DBR(void);
void read_Dir(char filecount);
char read_file(struct file *file_data,long int folderSector,long int foldercut);
long int ini_FAT(void);
#endif

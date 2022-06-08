#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
 
void find_file(char *path,char *target)
{
	//printf("enter find file fun\n");
	FILE *file=fopen(path,"r");
	char line[256];
	while(fgets(line,sizeof(line),file)){
		if(strstr(line,target))
			printf("%s:%s",path,line);
	}
	fclose(file);
}
//search the dir and find the file
void find_dir(char *path,char *target)
{
	//printf("enter find dir fun\n");
	DIR *dir=opendir(path);//open the dir
	struct dirent *entry;
	while(entry=readdir(dir))//read the dir & point to next obj
	{
		if(strcmp(entry->d_name,".")==0)
			continue;
		if(strcmp(entry->d_name,"..")==0)
			continue;
		char base[80]={0};
		strcpy(base,path);
		strcat(base,"/");
		strcat(base,entry->d_name);//fill the path
		//printf("base %s\n",base);
		if(entry->d_type==DT_DIR)//file type is dir
		{
			//printf("dir %s\n",entry->d_name);
			find_dir(base,target);
		}
		else if(entry->d_type==DT_REG)
		{
			//printf("file %s\n",entry->d_name);
			find_file(base,target);
		}
	}
	closedir(dir);
}
int main(int argc,char *argv[])
{
	if(argc!=3)
	{
		puts("usage: sfind file string");
		return 0;
	}
	char *path = argv[1];
	char *string = argv[2];
 
	struct stat info;
	stat(path,&info);
	if(S_ISDIR(info.st_mode))
		find_dir(path,string);
	else
		find_file(path,string);
	return 0;
}

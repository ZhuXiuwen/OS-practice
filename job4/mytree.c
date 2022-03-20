#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include<errno.h>
#include<string.h>
#include<dirent.h>
void myerror(const char *strerr){
	perror(strerr);
	exit(1);
}
void mytree(char direntName[], int level){
	DIR *p_dir = NULL;
	struct dirent *p_dirent = NULL;
	p_dir = opendir(direntName);
	if(p_dir == NULL)
	{
		myerror("opendir error");
	}
	while((p_dirent = readdir(p_dir)) != NULL){
		char *backupDirName = NULL;
		if(p_dirent->d_name[0] == '.')
			continue;
		int i;
		for(i = 0; i < level; i++){
			printf(" ");
		}
		printf(" ");
		printf("%s\n", p_dirent->d_name);
		if(p_dirent->d_type == DT_DIR){
			int curDirentNameLen = strlen(direntName) + 1;
			backupDirName = (char *)malloc(curDirentNameLen);
			memset(backupDirName, 0, curDirentNameLen);
			memcpy(backupDirName, direntName, curDirentNameLen);
			strcat(direntName, "/");
			strcat(direntName, p_dirent->d_name);
			mytree(direntName, level + 1);
			memcpy(direntName, backupDirName, curDirentNameLen);
			free(backupDirName);
			backupDirName = NULL;
		}
	}
	closedir(p_dir);
}
int main(int argc, char *argv[]){
	char direntName[256];
	memset(direntName, 0, sizeof(direntName));
	if(argc == 1)
		direntName[0] = '.';
	else if(argc == 2)
		strcat(direntName, argv[1]);
	else
		myerror("argument error");
	printf("%s\n", direntName);
	mytree(direntName, 0);
	return 0;
}


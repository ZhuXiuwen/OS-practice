#include<stdio.h>
#include<sys/types.h>
#include<dirent.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>
void myls(char dirname[]){
	DIR *dir_ptr;
	struct dirent *direntp;
	if((dir_ptr = opendir(dirname)) == NULL)
		fprintf(stderr, "ls: cannot open %s\n", dirname);
	else{
		while((direntp = readdir(dir_ptr)) != NULL){
			if(strcmp(direntp->d_name, ".") == 0 || strcmp(direntp->d_name, "..") == 0)
				continue;
			printf("%s\n", direntp->d_name);
		}
		closedir(dir_ptr);
	}
}
int main(int argc, char *argv[]){
	if(argc == 1) 
		myls(".");
	else
		myls(argv[1]);

}



#include<stdio.h>
#include<sys/types.h>
#include<unistd.h>
#include<stdlib.h>
#include<sys/wait.h>
void mysys(char *command)
{
	pid_t pid;
	if(command == NULL){
		printf("Error: wrong shell command!\n");
		exit(0);
	}
	pid = fork();
	if(pid == 0)
		execl("/bin/sh", "sh", "-c", command, NULL);
	wait(NULL);	

}
int main()
{
	printf("----------------------------------------------------------------\n");
	mysys("echo HELLO WORLD");
	printf("----------------------------------------------------------------\n");
	mysys("ls /");
	printf("----------------------------------------------------------------\n");
	return 0;
}
